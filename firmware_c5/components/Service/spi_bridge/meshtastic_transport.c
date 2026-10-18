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

#include "meshtastic_transport.h"

#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "meshtastic_gatt.h"
#include "meshtastic_tcp.h"
#include "spi_bridge.h"

static const char *TAG = "MESH_TRANSPORT";

#define TRANSPORT_FRAME_BUF_SIZE   512
#define TRANSPORT_LOG_BUF_SIZE     512
#define TRANSPORT_CHUNK_HDR_SIZE   sizeof(spi_mesh_chunk_hdr_t)
#define TRANSPORT_MUTEX_TIMEOUT_MS 50

typedef struct {
  bool is_active;
  uint8_t seq;
  uint8_t total_chunks;
  uint8_t next_chunk_idx;
  uint16_t accumulated_len;
  uint8_t buf[TRANSPORT_FRAME_BUF_SIZE];
} fromradio_reassembly_t;

typedef struct {
  bool is_active;
  uint8_t seq;
  uint8_t total_chunks;
  uint8_t next_chunk_idx;
  uint16_t accumulated_len;
  uint8_t buf[TRANSPORT_LOG_BUF_SIZE];
} log_reassembly_t;

static bool s_is_initialized = false;
static SemaphoreHandle_t s_mutex = NULL;
static fromradio_reassembly_t s_fromradio = {0};
static log_reassembly_t s_log = {0};
static uint32_t s_fromnum_counter = 0;
static uint8_t s_toradio_seq = 0;

static bool take_mutex(void);
static void give_mutex(void);

esp_err_t meshtastic_transport_init(void) {
  if (s_is_initialized) {
    return ESP_OK;
  }
  s_mutex = xSemaphoreCreateMutex();
  if (s_mutex == NULL) {
    ESP_LOGE(TAG, "Failed to create transport mutex");
    return ESP_ERR_NO_MEM;
  }
  memset(&s_fromradio, 0, sizeof(s_fromradio));
  memset(&s_log, 0, sizeof(s_log));
  s_fromnum_counter = 0;
  s_toradio_seq = 0;
  s_is_initialized = true;
  spi_bridge_stream_enable(SPI_ID_MESH_TORADIO_STREAM, true);
  ESP_LOGI(TAG, "Transport initialized");
  return ESP_OK;
}

void meshtastic_transport_inject_fromradio_chunk(const uint8_t *payload, uint8_t len) {
  if (payload == NULL || len < (uint8_t)TRANSPORT_CHUNK_HDR_SIZE) {
    return;
  }
  if (!take_mutex()) {
    return;
  }

  spi_mesh_chunk_hdr_t hdr;
  memcpy(&hdr, payload, sizeof(hdr));
  const uint8_t *data = payload + sizeof(hdr);
  uint8_t data_len = (uint8_t)(len - sizeof(hdr));

  if (hdr.total_chunks == 0) {
    give_mutex();
    return;
  }

  if (hdr.chunk_idx == 0) {
    s_fromradio.is_active = true;
    s_fromradio.seq = hdr.seq;
    s_fromradio.total_chunks = hdr.total_chunks;
    s_fromradio.next_chunk_idx = 0;
    s_fromradio.accumulated_len = 0;
  } else if (!s_fromradio.is_active) {
    give_mutex();
    return;
  }

  if (hdr.seq != s_fromradio.seq || hdr.chunk_idx != s_fromradio.next_chunk_idx ||
      hdr.total_chunks != s_fromradio.total_chunks) {
    s_fromradio.is_active = false;
    give_mutex();
    return;
  }

  if ((uint16_t)(s_fromradio.accumulated_len + data_len) > sizeof(s_fromradio.buf)) {
    s_fromradio.is_active = false;
    give_mutex();
    return;
  }

  memcpy(s_fromradio.buf + s_fromradio.accumulated_len, data, data_len);
  s_fromradio.accumulated_len = (uint16_t)(s_fromradio.accumulated_len + data_len);
  s_fromradio.next_chunk_idx++;

  if (s_fromradio.next_chunk_idx >= s_fromradio.total_chunks) {
    uint8_t snapshot[TRANSPORT_FRAME_BUF_SIZE];
    uint16_t snapshot_len = s_fromradio.accumulated_len;
    memcpy(snapshot, s_fromradio.buf, snapshot_len);
    s_fromradio.is_active = false;
    s_fromnum_counter++;
    give_mutex();

    meshtastic_gatt_enqueue_fromradio(snapshot, snapshot_len);
    meshtastic_tcp_send_fromradio(snapshot, snapshot_len);
    return;
  }

  give_mutex();
}

void meshtastic_transport_inject_log_chunk(const uint8_t *payload, uint8_t len) {
  if (payload == NULL || len < (uint8_t)TRANSPORT_CHUNK_HDR_SIZE) {
    return;
  }
  if (!meshtastic_gatt_is_logradio_subscribed()) {
    return;
  }
  if (!take_mutex()) {
    return;
  }

  spi_mesh_chunk_hdr_t hdr;
  memcpy(&hdr, payload, sizeof(hdr));
  const uint8_t *data = payload + sizeof(hdr);
  uint8_t data_len = (uint8_t)(len - sizeof(hdr));

  if (hdr.total_chunks == 0) {
    give_mutex();
    return;
  }

  if (hdr.chunk_idx == 0) {
    s_log.is_active = true;
    s_log.seq = hdr.seq;
    s_log.total_chunks = hdr.total_chunks;
    s_log.next_chunk_idx = 0;
    s_log.accumulated_len = 0;
  } else if (!s_log.is_active) {
    give_mutex();
    return;
  }

  if (hdr.seq != s_log.seq || hdr.chunk_idx != s_log.next_chunk_idx ||
      hdr.total_chunks != s_log.total_chunks) {
    s_log.is_active = false;
    give_mutex();
    return;
  }

  if ((uint16_t)(s_log.accumulated_len + data_len) > sizeof(s_log.buf)) {
    s_log.is_active = false;
    give_mutex();
    return;
  }

  memcpy(s_log.buf + s_log.accumulated_len, data, data_len);
  s_log.accumulated_len = (uint16_t)(s_log.accumulated_len + data_len);
  s_log.next_chunk_idx++;

  if (s_log.next_chunk_idx >= s_log.total_chunks) {
    uint8_t snapshot[TRANSPORT_LOG_BUF_SIZE];
    uint16_t snapshot_len = s_log.accumulated_len;
    memcpy(snapshot, s_log.buf, snapshot_len);
    s_log.is_active = false;
    give_mutex();

    meshtastic_gatt_notify_log(snapshot, snapshot_len);
    return;
  }

  give_mutex();
}

bool meshtastic_transport_send_toradio(const uint8_t *frame, uint16_t len) {
  if (frame == NULL || len == 0) {
    return false;
  }
  if (!spi_bridge_stream_is_enabled(SPI_ID_MESH_TORADIO_STREAM)) {
    return false;
  }

  uint16_t total_u16 =
      (uint16_t)((len + SPI_MESH_CHUNK_PAYLOAD_MAX - 1) / SPI_MESH_CHUNK_PAYLOAD_MAX);
  if (total_u16 == 0 || total_u16 > 255) {
    return false;
  }
  uint8_t total_chunks = (uint8_t)total_u16;
  uint8_t seq;
  if (take_mutex()) {
    seq = s_toradio_seq++;
    give_mutex();
  } else {
    return false;
  }

  uint8_t buf[SPI_MESH_PAYLOAD_LIMIT];
  spi_mesh_chunk_hdr_t hdr;
  uint16_t offset = 0;

  for (uint8_t idx = 0; idx < total_chunks; idx++) {
    uint16_t remaining = (uint16_t)(len - offset);
    uint16_t this_chunk =
        remaining > SPI_MESH_CHUNK_PAYLOAD_MAX ? SPI_MESH_CHUNK_PAYLOAD_MAX : remaining;

    hdr.seq = seq;
    hdr.chunk_idx = idx;
    hdr.total_chunks = total_chunks;
    hdr.flags = (idx == (uint8_t)(total_chunks - 1)) ? SPI_MESH_CHUNK_FLAG_LAST : 0;

    memcpy(buf, &hdr, sizeof(hdr));
    memcpy(buf + sizeof(hdr), frame + offset, this_chunk);

    uint8_t push_len = (uint8_t)(sizeof(hdr) + this_chunk);
    if (!spi_bridge_stream_push(SPI_ID_MESH_TORADIO_STREAM, buf, push_len)) {
      return false;
    }
    offset += this_chunk;
  }
  return true;
}

void meshtastic_transport_get_status(spi_mesh_status_t *out_status) {
  if (out_status == NULL) {
    return;
  }
  out_status->ble_connected = meshtastic_gatt_is_connected() ? 1 : 0;
  out_status->ble_subscribed = meshtastic_gatt_is_fromnum_subscribed() ? 1 : 0;
  out_status->tcp_clients = meshtastic_tcp_get_client_count();
  out_status->logradio_subscribed = meshtastic_gatt_is_logradio_subscribed() ? 1 : 0;
  if (take_mutex()) {
    out_status->fromnum_counter = s_fromnum_counter;
    give_mutex();
  } else {
    out_status->fromnum_counter = 0;
  }
}

uint32_t meshtastic_transport_bump_fromnum(void) {
  uint32_t value = 0;
  if (take_mutex()) {
    s_fromnum_counter++;
    value = s_fromnum_counter;
    give_mutex();
  }
  return value;
}

void meshtastic_transport_reset(void) {
  if (!take_mutex()) {
    return;
  }
  memset(&s_fromradio, 0, sizeof(s_fromradio));
  memset(&s_log, 0, sizeof(s_log));
  s_fromnum_counter = 0;
  s_toradio_seq = 0;
  give_mutex();
}

static bool take_mutex(void) {
  if (s_mutex == NULL) {
    return false;
  }
  return xSemaphoreTake(s_mutex, pdMS_TO_TICKS(TRANSPORT_MUTEX_TIMEOUT_MS)) == pdTRUE;
}

static void give_mutex(void) {
  if (s_mutex != NULL) {
    xSemaphoreGive(s_mutex);
  }
}
