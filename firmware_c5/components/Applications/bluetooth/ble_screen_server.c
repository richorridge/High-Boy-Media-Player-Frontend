// Copyright (c) 2025 HIGH CODE LLC
//
// TentacleOS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TentacleOS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TentacleOS. If not, see <https://www.gnu.org/licenses/>.

#include "ble_screen_server.h"

#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "BLE_SCREEN_SERVER";

#define MAX_CHUNK_SIZE             480
#define COMPRESSION_BUFFER_SIZE    (32 * 1024)
#define COMPRESSION_OVERFLOW_LIMIT 32000
#define COMPRESSION_TAIL_LIMIT     32765
#define RLE_HEADER_MAGIC           0xFE

static const ble_uuid128_t s_gatt_svc_screen_uuid = BLE_UUID128_INIT(
    0x00, 0x00, 0x64, 0x6c, 0x6f, 0x00, 0x41, 0x89, 0xc0, 0x46, 0x72, 0x69, 0xb6, 0x56, 0x5a, 0x59);

static const ble_uuid128_t s_gatt_chr_stream_uuid = BLE_UUID128_INIT(
    0x01, 0x00, 0x64, 0x6c, 0x6f, 0x00, 0x41, 0x89, 0xc0, 0x46, 0x72, 0x69, 0xb6, 0x56, 0x5a, 0x59);

static uint16_t s_screen_chr_val_handle;
static uint8_t *s_compression_buffer = NULL;
static bool s_is_initialized = false;

static int screen_access_cb(uint16_t conn_handle,
                            uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt,
                            void *arg) {
  return 0;
}

static const struct ble_gatt_svc_def s_screen_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &s_gatt_svc_screen_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
                                                           .uuid = &s_gatt_chr_stream_uuid.u,
                                                           .access_cb = screen_access_cb,
                                                           .flags = BLE_GATT_CHR_F_NOTIFY,
                                                           .val_handle = &s_screen_chr_val_handle,
                                                       },
                                                       {0}},
    },
    {0}};

esp_err_t ble_screen_server_init(void) {
  if (s_is_initialized)
    return ESP_OK;

  int rc = ble_gatts_count_cfg(s_screen_svcs);
  if (rc != 0)
    return rc;

  rc = ble_gatts_add_svcs(s_screen_svcs);
  if (rc != 0)
    return rc;

  s_compression_buffer = heap_caps_malloc(COMPRESSION_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
  if (s_compression_buffer == NULL)
    return ESP_ERR_NO_MEM;

  s_is_initialized = true;
  ESP_LOGI(TAG, "Screen Stream Service Initialized");
  return ESP_OK;
}

void ble_screen_server_deinit(void) {
  if (s_compression_buffer != NULL) {
    free(s_compression_buffer);
    s_compression_buffer = NULL;
  }
  s_is_initialized = false;
}

void ble_screen_server_send_partial(const uint16_t *px_map, int x, int y, int w, int h) {
  if (!s_is_initialized || s_compression_buffer == NULL || px_map == NULL)
    return;

  size_t comp_idx = 0;

  // Header: [Magic: 0xFE] [X:2] [Y:2] [W:2] [H:2]
  s_compression_buffer[comp_idx++] = RLE_HEADER_MAGIC;
  s_compression_buffer[comp_idx++] = x & 0xFF;
  s_compression_buffer[comp_idx++] = (x >> 8) & 0xFF;
  s_compression_buffer[comp_idx++] = y & 0xFF;
  s_compression_buffer[comp_idx++] = (y >> 8) & 0xFF;
  s_compression_buffer[comp_idx++] = w & 0xFF;
  s_compression_buffer[comp_idx++] = (w >> 8) & 0xFF;
  s_compression_buffer[comp_idx++] = h & 0xFF;
  s_compression_buffer[comp_idx++] = (h >> 8) & 0xFF;

  // RLE Compression of the partial map
  uint16_t current_pixel = px_map[0];
  uint8_t run_count = 0;
  size_t total_pixels = w * h;

  for (size_t i = 0; i < total_pixels; i++) {
    uint16_t px = px_map[i];

    if (px == current_pixel && run_count < 255) {
      run_count++;
    } else {
      s_compression_buffer[comp_idx++] = run_count;
      s_compression_buffer[comp_idx++] = current_pixel & 0xFF;
      s_compression_buffer[comp_idx++] = (current_pixel >> 8) & 0xFF;

      current_pixel = px;
      run_count = 1;
    }

    if (comp_idx > COMPRESSION_OVERFLOW_LIMIT)
      break;
  }

  if (run_count > 0 && comp_idx < COMPRESSION_TAIL_LIMIT) {
    s_compression_buffer[comp_idx++] = run_count;
    s_compression_buffer[comp_idx++] = current_pixel & 0xFF;
    s_compression_buffer[comp_idx++] = (current_pixel >> 8) & 0xFF;
  }

  size_t total_len = comp_idx;
  size_t sent = 0;

  while (sent < total_len) {
    size_t chunk_len = total_len - sent;
    if (chunk_len > MAX_CHUNK_SIZE)
      chunk_len = MAX_CHUNK_SIZE;

    struct os_mbuf *om = ble_hs_mbuf_from_flat(s_compression_buffer + sent, chunk_len);
    if (om != NULL) {
      ble_gattc_notify_custom(0, s_screen_chr_val_handle, om);
    }
    sent += chunk_len;
  }
}

bool ble_screen_server_is_active(void) {
  return s_is_initialized;
}
