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

#include "deauther_detector.h"

#include <string.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi_80211.h"
#include "wifi_service.h"

static const char *TAG = "DEAUTH_DETECTOR";

#define DEAUTH_FRAME_TYPE    0
#define DEAUTH_FRAME_SUBTYPE 0xC
#define DEAUTH_LOG_INTERVAL  10

static uint32_t *s_deauth_counter = NULL;

static void packet_handler(void *buf, wifi_promiscuous_pkt_type_t type);

void deauther_detector_start(void) {
  if (s_deauth_counter == NULL) {
    s_deauth_counter = (uint32_t *)heap_caps_malloc(sizeof(uint32_t), MALLOC_CAP_SPIRAM);
    if (s_deauth_counter == NULL) {
      ESP_LOGE(TAG, "Failed to allocate deauth counter in PSRAM!");
      return;
    }
    *s_deauth_counter = 0;
  }

  ESP_LOGI(TAG, "Starting Deauth Detector...");

  wifi_promiscuous_filter_t filter = {.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT};

  wifi_service_promiscuous_start(packet_handler, &filter);
  wifi_service_start_channel_hopping();
}

void deauther_detector_stop(void) {
  wifi_service_stop_channel_hopping();
  wifi_service_promiscuous_stop();

  if (s_deauth_counter != NULL) {
    free(s_deauth_counter);
    s_deauth_counter = NULL;
  }

  ESP_LOGI(TAG, "Deauth Detector Stopped.");
}

uint32_t deauther_detector_get_count(void) {
  if (s_deauth_counter != NULL) {
    return *s_deauth_counter;
  }
  return 0;
}

static void packet_handler(void *buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT) {
    return;
  }

  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
  const wifi_frame_control_t *frame_control = (const wifi_frame_control_t *)ppkt->payload;

  if (frame_control->type == DEAUTH_FRAME_TYPE && frame_control->subtype == DEAUTH_FRAME_SUBTYPE) {
    if (s_deauth_counter != NULL) {
      (*s_deauth_counter)++;
      if ((*s_deauth_counter) % DEAUTH_LOG_INTERVAL == 0) {
        ESP_LOGW(TAG, "Deauth detected! Total: %lu", *s_deauth_counter);
      }
    }
  }
}
