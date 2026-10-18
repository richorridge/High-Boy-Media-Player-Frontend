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

#include "probe_monitor.h"

#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "cJSON.h"
#include "mac_vendor.h"
#include "sd_card_init.h"
#include "sd_card_write.h"
#include "storage_write.h"
#include "tos_flash_paths.h"
#include "wifi_80211.h"
#include "wifi_service.h"

static const char *TAG = "PROBE_MONITOR";

#define MONITOR_STACK_SIZE  4096
#define MAX_SCAN_RESULTS    300
#define SSID_MAX_LEN        32
#define BSSID_LEN           6
#define US_PER_S            1000000
#define SSID_TAG_ID         0
#define PROBE_FRAME_TYPE    0
#define PROBE_FRAME_SUBTYPE 4
#define MAC_HEADER_LEN      24

static TaskHandle_t s_monitor_task_handle = NULL;
static StackType_t *s_monitor_task_stack = NULL;
static StaticTask_t *s_monitor_task_tcb = NULL;

static probe_monitor_record_t *s_scan_results = NULL;
static uint16_t s_scan_count = 0;
static bool s_is_running = false;

static void add_or_update_probe(const uint8_t *mac, const char *ssid, int8_t rssi);
static void sniffer_callback(void *buf, wifi_promiscuous_pkt_type_t type);
static bool save_results_to_path(const char *path, bool use_sd_driver);

bool probe_monitor_start(void) {
  if (s_is_running)
    return false;

  if (s_scan_results != NULL)
    free(s_scan_results);
  s_scan_results = (probe_monitor_record_t *)heap_caps_malloc(
      MAX_SCAN_RESULTS * sizeof(probe_monitor_record_t), MALLOC_CAP_SPIRAM);
  if (s_scan_results == NULL)
    return false;

  s_scan_count = 0;
  s_is_running = true;

  wifi_promiscuous_filter_t filter = {.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT};
  wifi_service_promiscuous_start(sniffer_callback, &filter);
  wifi_service_start_channel_hopping();

  ESP_LOGI(TAG, "Probe Request Monitor started.");
  return true;
}

void probe_monitor_stop(void) {
  if (!s_is_running)
    return;
  wifi_service_promiscuous_stop();
  wifi_service_stop_channel_hopping();
  s_is_running = false;
  ESP_LOGI(TAG, "Probe Request Monitor stopped.");
}

probe_monitor_record_t *probe_monitor_get_results(uint16_t *out_count) {
  *out_count = s_scan_count;
  return s_scan_results;
}

const uint16_t *probe_monitor_get_count_ptr(void) {
  return &s_scan_count;
}

void probe_monitor_free_results(void) {
  probe_monitor_stop();
  if (s_scan_results != NULL) {
    free(s_scan_results);
    s_scan_results = NULL;
  }
  s_scan_count = 0;
}

bool probe_monitor_save_results_to_internal_flash(void) {
  return save_results_to_path(FLASH_STORAGE_WIFI_PROBES, false);
}

bool probe_monitor_save_results_to_sd_card(void) {
  return save_results_to_path("/probe_monitor.json", true);
}

static void add_or_update_probe(const uint8_t *mac, const char *ssid, int8_t rssi) {
  if (s_scan_results == NULL)
    return;

  for (int i = 0; i < s_scan_count; i++) {
    if (memcmp(s_scan_results[i].mac, mac, BSSID_LEN) == 0 &&
        strcmp(s_scan_results[i].ssid, ssid) == 0) {
      s_scan_results[i].rssi = rssi;
      s_scan_results[i].last_seen_timestamp = (uint32_t)(esp_timer_get_time() / US_PER_S);
      return;
    }
  }

  if (s_scan_count < MAX_SCAN_RESULTS) {
    memcpy(s_scan_results[s_scan_count].mac, mac, BSSID_LEN);
    strncpy(s_scan_results[s_scan_count].ssid, ssid, SSID_MAX_LEN);
    s_scan_results[s_scan_count].ssid[SSID_MAX_LEN] = '\0';
    s_scan_results[s_scan_count].rssi = rssi;
    s_scan_results[s_scan_count].last_seen_timestamp = (uint32_t)(esp_timer_get_time() / US_PER_S);
    s_scan_count++;

    ESP_LOGI(TAG,
             "New Probe: [%02x:%02x:%02x:%02x:%02x:%02x] searching for '%s' (RSSI: %d)",
             mac[0],
             mac[1],
             mac[2],
             mac[3],
             mac[4],
             mac[5],
             ssid,
             rssi);
  }
}

static void sniffer_callback(void *buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
  const wifi_mac_header_t *mac_header = (const wifi_mac_header_t *)ppkt->payload;
  const wifi_frame_control_t *fc = (const wifi_frame_control_t *)&mac_header->frame_control;

  if (fc->type == PROBE_FRAME_TYPE && fc->subtype == PROBE_FRAME_SUBTYPE) {
    const uint8_t *payload = ppkt->payload + MAC_HEADER_LEN;
    int payload_len = ppkt->rx_ctrl.sig_len - MAC_HEADER_LEN;

    if (payload_len < 2)
      return;

    int offset = 0;
    char ssid[33] = {0};
    bool is_ssid_found = false;

    while (offset + 2 <= payload_len) {
      uint8_t tag = payload[offset];
      uint8_t len = payload[offset + 1];

      if (offset + 2 + len > payload_len)
        break;

      if (tag == SSID_TAG_ID) {
        int ssid_len = (len > SSID_MAX_LEN) ? SSID_MAX_LEN : len;
        memcpy(ssid, &payload[offset + 2], ssid_len);
        ssid[ssid_len] = '\0';
        is_ssid_found = true;
        break;
      }
      offset += 2 + len;
    }

    if (is_ssid_found) {
      if (strlen(ssid) == 0) {
        strcpy(ssid, "<Broadcast>");
      }
      add_or_update_probe(mac_header->addr2, ssid, ppkt->rx_ctrl.rssi);
    }
  }
}

static bool save_results_to_path(const char *path, bool use_sd_driver) {
  if (s_scan_results == NULL || s_scan_count == 0) {
    ESP_LOGW(TAG, "No results to save.");
    return false;
  }

  cJSON *root = cJSON_CreateArray();
  for (int i = 0; i < s_scan_count; i++) {
    probe_monitor_record_t *rec = &s_scan_results[i];
    cJSON *entry = cJSON_CreateObject();

    char mac_str[18];
    snprintf(mac_str,
             sizeof(mac_str),
             "%02x:%02x:%02x:%02x:%02x:%02x",
             rec->mac[0],
             rec->mac[1],
             rec->mac[2],
             rec->mac[3],
             rec->mac[4],
             rec->mac[5]);
    cJSON_AddStringToObject(entry, "mac", mac_str);
    cJSON_AddStringToObject(entry, "vendor", mac_vendor_get_name(rec->mac));
    cJSON_AddStringToObject(entry, "ssid", rec->ssid);
    cJSON_AddNumberToObject(entry, "rssi", rec->rssi);
    cJSON_AddNumberToObject(entry, "timestamp", rec->last_seen_timestamp);

    cJSON_AddItemToArray(root, entry);
  }

  char *json_string = cJSON_PrintUnformatted(root);
  if (json_string == NULL) {
    cJSON_Delete(root);
    return false;
  }

  esp_err_t err;
  if (use_sd_driver) {
    if (!sd_is_mounted()) {
      free(json_string);
      cJSON_Delete(root);
      return false;
    }
    err = sd_write_string(path, json_string);
  } else {
    err = storage_write_string(path, json_string);
  }

  free(json_string);
  cJSON_Delete(root);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to save results to %s", path);
    return false;
  }

  ESP_LOGI(TAG, "Results saved to %s", path);
  return true;
}
