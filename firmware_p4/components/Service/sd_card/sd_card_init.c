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

#include "sd_card_init.h"
#include "pin_def.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"

static const char *TAG = "SD_CARD_INIT";
static sdmmc_card_t *s_card = NULL;
static bool s_is_mounted = false;

esp_err_t sd_init(void) {
  return sd_init_custom(SD_MAX_FILES, false);
}

esp_err_t sd_init_custom(uint8_t max_files, bool format_if_failed) {
  if (s_is_mounted) {
    ESP_LOGW(TAG, "SD already mounted");
    return ESP_OK;
  }

  ESP_LOGI(TAG, "Initializing SD (SDMMC 4-bit)...");

  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
  slot_config.width = 4;
  slot_config.clk = GPIO_SDMMC_CLK_PIN;
  slot_config.cmd = GPIO_SDMMC_CMD_PIN;
  slot_config.d0 = GPIO_SDMMC_D0_PIN;
  slot_config.d1 = GPIO_SDMMC_D1_PIN;
  slot_config.d2 = GPIO_SDMMC_D2_PIN;
  slot_config.d3 = GPIO_SDMMC_D3_PIN;
  slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = format_if_failed,
      .max_files = max_files,
      .allocation_unit_size = SD_ALLOCATION_UNIT,
  };

  esp_err_t ret =
      esp_vfs_fat_sdmmc_mount(VFS_MOUNT_POINT, &host, &slot_config, &mount_config, &s_card);

  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Mount failed: %s", esp_err_to_name(ret));
    return ret;
  }

  s_is_mounted = true;
  ESP_LOGI(TAG,
           "SD mounted (SDMMC): %s, %llu MB",
           s_card->cid.name,
           ((uint64_t)s_card->csd.capacity) * s_card->csd.sector_size / (1024 * 1024));
  return ESP_OK;
}

esp_err_t sd_deinit(void) {
  if (!s_is_mounted) {
    ESP_LOGW(TAG, "SD not mounted");
    return ESP_ERR_INVALID_STATE;
  }

  esp_err_t ret = esp_vfs_fat_sdcard_unmount(VFS_MOUNT_POINT, s_card);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Unmount failed: %s", esp_err_to_name(ret));
    return ret;
  }

  s_is_mounted = false;
  s_card = NULL;

  ESP_LOGI(TAG, "SD unmounted");
  return ESP_OK;
}

bool sd_is_mounted(void) {
  return s_is_mounted;
}

esp_err_t sd_remount(void) {
  if (s_is_mounted) {
    esp_err_t ret = sd_deinit();
    if (ret != ESP_OK)
      return ret;
  }
  return sd_init();
}

esp_err_t sd_check_health(void) {
  if (!s_is_mounted) {
    ESP_LOGE(TAG, "SD not mounted");
    return ESP_ERR_INVALID_STATE;
  }
  if (s_card == NULL) {
    ESP_LOGE(TAG, "Card pointer is null");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "Card healthy");
  return ESP_OK;
}

sdmmc_card_t *sd_get_card_handle(void) {
  return s_card;
}
