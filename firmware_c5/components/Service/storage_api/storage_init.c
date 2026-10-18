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

#include "storage_init.h"

#include "esp_log.h"

#include "storage_dirs.h"
#include "storage_mkdir.h"
#include "vfs_config.h"
#include "vfs_core.h"

static const char *TAG = "STORAGE_INIT";

extern esp_err_t vfs_init_auto(void);
extern esp_err_t vfs_deinit_auto(void);
extern bool vfs_is_mounted_auto(void);
extern void vfs_print_info(void);

static bool s_is_initialized = false;

const char *const STORAGE_DEFAULT_DIRS[] = {STORAGE_DIR_CONFIG,
                                            STORAGE_DIR_DATA,
                                            STORAGE_DIR_LOGS,
                                            STORAGE_DIR_CACHE,
                                            STORAGE_DIR_TEMP,
                                            STORAGE_DIR_BACKUP,
                                            STORAGE_DIR_CERTS,
                                            STORAGE_DIR_SCRIPTS,
                                            STORAGE_DIR_CAPTIVE,
                                            NULL};

static esp_err_t storage_create_default_dirs(void) {
  ESP_LOGI(TAG, "Creating default directories");

  int count = 0;
  for (int i = 0; STORAGE_DEFAULT_DIRS[i] != NULL; i++) {
    ESP_LOGI(TAG, "Processing directory [%d]: %s", i, STORAGE_DEFAULT_DIRS[i]);
    count++;

    esp_err_t ret = storage_mkdir_recursive(STORAGE_DEFAULT_DIRS[i]);
    if (ret != ESP_OK) {
      ESP_LOGW(TAG,
               "Failed to create directory: %s (error: %s)",
               STORAGE_DEFAULT_DIRS[i],
               esp_err_to_name(ret));
    } else {
      ESP_LOGI(TAG, "Directory processed successfully: %s", STORAGE_DEFAULT_DIRS[i]);
    }
  }

  ESP_LOGI(TAG, "Processed %d directories", count);
  return ESP_OK;
}

esp_err_t storage_init(void) {
  if (s_is_initialized) {
    ESP_LOGW(TAG, "Already initialized");
    return ESP_OK;
  }

  ESP_LOGI(TAG, "Initializing %s", VFS_BACKEND_NAME);

  esp_err_t ret = vfs_init_auto();
  if (ret == ESP_OK) {
    s_is_initialized = true;
    ESP_LOGI(TAG, "Storage ready at %s", VFS_MOUNT_POINT);

    // Criar diretórios padrão
    storage_create_default_dirs();
  } else {
    ESP_LOGE(TAG, "Initialization failed: %s", esp_err_to_name(ret));
  }

  return ret;
}

esp_err_t storage_init_custom(uint8_t max_files, bool format_if_failed) {
  ESP_LOGW(TAG, "Custom initialization not yet implemented, using defaults");
  return storage_init();
}

esp_err_t storage_deinit(void) {
  if (!s_is_initialized) {
    return ESP_ERR_INVALID_STATE;
  }

  ESP_LOGI(TAG, "Deinitializing storage");

  esp_err_t ret = vfs_deinit_auto();
  if (ret == ESP_OK) {
    s_is_initialized = false;
  }

  return ret;
}

bool storage_is_mounted(void) {
  return s_is_initialized && vfs_is_mounted_auto();
}

esp_err_t storage_remount(void) {
  ESP_LOGI(TAG, "Remounting storage");

  if (s_is_initialized) {
    esp_err_t ret = storage_deinit();
    if (ret != ESP_OK) {
      return ret;
    }
  }

  return storage_init();
}

esp_err_t storage_check_health(void) {
  if (!storage_is_mounted()) {
    return ESP_ERR_INVALID_STATE;
  }

  const char *test_file = ".health_check";
  const char *test_data = "OK";
  char full_path[256];

  snprintf(full_path, sizeof(full_path), "%s/%s", VFS_MOUNT_POINT, test_file);

  esp_err_t ret = vfs_write_file(full_path, test_data, 2);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Health check failed: write error");
    return ret;
  }

  char buffer[10];
  size_t read;
  ret = vfs_read_file(full_path, buffer, sizeof(buffer), &read);
  if (ret != ESP_OK || read != 2) {
    ESP_LOGE(TAG, "Health check failed: read error");
    vfs_unlink(full_path);
    return ESP_FAIL;
  }

  vfs_unlink(full_path);
  ESP_LOGI(TAG, "Health check passed");

  return ESP_OK;
}

const char *storage_get_mount_point(void) {
  return VFS_MOUNT_POINT;
}

const char *storage_get_backend_name(void) {
  return VFS_BACKEND_NAME;
}

void storage_print_info(void) {
  vfs_print_info();
}
