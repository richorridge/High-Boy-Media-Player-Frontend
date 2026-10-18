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

#include "ble_hid_keyboard.h"

#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/ans/ble_svc_ans.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "BLE_HID_KEYBOARD";

#define HID_REPORT_SIZE    8
#define KEY_DEBOUNCE_MS    15
#define HID_APPEARANCE_KBD 0x03C1
#define HID_ADV_NAME       "BadKB-BLE"
#define HID_ADV_NAME_LEN   9

// BLE GATT UUID constants
#define UUID_DEVICE_INFO_SVC   0x180A
#define UUID_HID_SVC           0x1812
#define UUID_MFG_NAME_CHR      0x2A29
#define UUID_REPORT_MAP_CHR    0x2A4B
#define UUID_REPORT_CHR        0x2A4D
#define UUID_HID_INFO_CHR      0x2A4A
#define UUID_PROTOCOL_MODE_CHR 0x2A4E

static uint16_t s_hid_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static bool s_is_connected = false;
static uint16_t s_hid_report_handle;

// HID Report Map for a standard keyboard
static const uint8_t HID_REPORT_MAP[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)
    0x05, 0x07, //   Usage Page (Key Codes)
    0x19, 0xE0, //   Usage Minimum (224)
    0x29, 0xE7, //   Usage Maximum (231)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x01, //   Logical Maximum (1)
    0x75, 0x01, //   Report Size (1)
    0x95, 0x08, //   Report Count (8)
    0x81, 0x02, //   Input (Data, Variable, Absolute) ; Modifier byte
    0x95, 0x01, //   Report Count (1)
    0x75, 0x08, //   Report Size (8)
    0x81, 0x01, //   Input (Constant) ; Reserved byte
    0x95, 0x05, //   Report Count (5)
    0x75, 0x01, //   Report Size (1)
    0x05, 0x08, //   Usage Page (LEDs)
    0x19, 0x01, //   Usage Minimum (1)
    0x29, 0x05, //   Usage Maximum (5)
    0x91, 0x02, //   Output (Data, Variable, Absolute) ; LED report
    0x95, 0x01, //   Report Count (1)
    0x75, 0x03, //   Report Size (3)
    0x91, 0x01, //   Output (Constant) ; LED report padding
    0x95, 0x06, //   Report Count (6)
    0x75, 0x08, //   Report Size (8)
    0x15, 0x00, //   Logical Minimum (0)
    0x25, 0x65, //   Logical Maximum (101)
    0x05, 0x07, //   Usage Page (Key Codes)
    0x19, 0x00, //   Usage Minimum (0)
    0x29, 0x65, //   Usage Maximum (101)
    0x81, 0x00, //   Input (Data, Array) ; Key arrays (6 bytes)
    0xC0        // End Collection
};

static int hid_gap_event(struct ble_gap_event *event, void *arg);

static int hid_access_cb(uint16_t conn_handle,
                         uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt,
                         void *arg) {
  uint16_t uuid16 = ble_uuid_u16(ctxt->chr->uuid);

  // Report Map
  if (uuid16 == UUID_REPORT_MAP_CHR) {
    int rc = os_mbuf_append(ctxt->om, HID_REPORT_MAP, sizeof(HID_REPORT_MAP));
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // HID Information: bcdHID(2), bCountryCode(1), Flags(1)
  if (uuid16 == UUID_HID_INFO_CHR) {
    // HID v1.11, Country 0, RemoteWake+NormallyConnectable
    uint8_t info[] = {0x11, 0x01, 0x00, 0x02};
    int rc = os_mbuf_append(ctxt->om, info, sizeof(info));
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  // Protocol Mode
  if (uuid16 == UUID_PROTOCOL_MODE_CHR) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
      uint8_t mode = 1; // Report Protocol
      os_mbuf_append(ctxt->om, &mode, 1);
      return 0;
    }
  }

  return 0;
}

static const struct ble_gatt_svc_def s_hid_svcs[] = {
    {
        // Service: Device Information
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(UUID_DEVICE_INFO_SVC),
        .characteristics =
            (struct ble_gatt_chr_def[]){{
                                            .uuid = BLE_UUID16_DECLARE(UUID_MFG_NAME_CHR),
                                            .access_cb = hid_access_cb,
                                            .flags = BLE_GATT_CHR_F_READ,
                                        },
                                        {
                                            0,
                                        }},
    },
    {
        // Service: Human Interface Device
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(UUID_HID_SVC),
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    // Report Map
                    .uuid = BLE_UUID16_DECLARE(UUID_REPORT_MAP_CHR),
                    .access_cb = hid_access_cb,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    // Report (Input) - Handle saved for notifications
                    .uuid = BLE_UUID16_DECLARE(UUID_REPORT_CHR),
                    .access_cb = hid_access_cb,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                    .val_handle = &s_hid_report_handle,
                },
                {
                    // HID Information
                    .uuid = BLE_UUID16_DECLARE(UUID_HID_INFO_CHR),
                    .access_cb = hid_access_cb,
                    .flags = BLE_GATT_CHR_F_READ,
                },
                {
                    // Protocol Mode
                    .uuid = BLE_UUID16_DECLARE(UUID_PROTOCOL_MODE_CHR),
                    .access_cb = hid_access_cb,
                    .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE_NO_RSP,
                },
                {
                    0,
                }},
    },
    {
        0,
    }};

esp_err_t ble_hid_init(void) {
  int rc = ble_gatts_count_cfg(s_hid_svcs);
  if (rc != 0) {
    ESP_LOGE(TAG, "GATTS count cfg failed: %d", rc);
    return rc;
  }

  rc = ble_gatts_add_svcs(s_hid_svcs);
  if (rc != 0) {
    ESP_LOGE(TAG, "GATTS add svcs failed: %d", rc);
    return rc;
  }

  struct ble_gap_adv_params adv_params = {0};
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

  struct ble_hs_adv_fields fields = {0};
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
  fields.appearance = HID_APPEARANCE_KBD;
  fields.name = (uint8_t *)HID_ADV_NAME;
  fields.name_len = HID_ADV_NAME_LEN;
  fields.name_is_complete = 1;
  fields.uuids16 = (ble_uuid16_t[]){BLE_UUID16_INIT(UUID_HID_SVC)};
  fields.num_uuids16 = 1;
  fields.uuids16_is_complete = 1;

  ble_gap_adv_set_fields(&fields);
  ble_gap_adv_start(BLE_OWN_ADDR_RANDOM, NULL, BLE_HS_FOREVER, &adv_params, hid_gap_event, NULL);

  ESP_LOGI(TAG, "HID Services Added & Advertising started");
  return ESP_OK;
}

esp_err_t ble_hid_deinit(void) {
  if (s_is_connected) {
    ble_gap_terminate(s_hid_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
  }
  ble_gap_adv_stop();
  // NimBLE does not easily support removing services at runtime without reset.
  // We just stop advertising.
  return ESP_OK;
}

bool ble_hid_is_connected(void) {
  return s_is_connected;
}

void ble_hid_send_key(uint8_t keycode, uint8_t modifier) {
  if (!s_is_connected)
    return;

  // Report: Modifier, Reserved, Key1, Key2, Key3, Key4, Key5, Key6
  uint8_t report[HID_REPORT_SIZE] = {0};
  report[0] = modifier;
  report[2] = keycode;

  struct os_mbuf *om = ble_hs_mbuf_from_flat(report, sizeof(report));
  ble_gattc_notify_custom(s_hid_conn_handle, s_hid_report_handle, om);

  vTaskDelay(pdMS_TO_TICKS(KEY_DEBOUNCE_MS));

  // Release
  memset(report, 0, sizeof(report));
  om = ble_hs_mbuf_from_flat(report, sizeof(report));
  ble_gattc_notify_custom(s_hid_conn_handle, s_hid_report_handle, om);

  vTaskDelay(pdMS_TO_TICKS(KEY_DEBOUNCE_MS));
}

static int hid_gap_event(struct ble_gap_event *event, void *arg) {
  switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
      if (event->connect.status == 0) {
        s_hid_conn_handle = event->connect.conn_handle;
        s_is_connected = true;
        ESP_LOGI(TAG, "HID Connected");
      }
      break;
    case BLE_GAP_EVENT_DISCONNECT:
      s_hid_conn_handle = BLE_HS_CONN_HANDLE_NONE;
      s_is_connected = false;
      ESP_LOGI(TAG, "HID Disconnected");
      // Auto re-advertise
      ble_hid_init();
      break;
  }
  return 0;
}
