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

#ifndef SD_CARD_DIR_H
#define SD_CARD_DIR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#define SD_BASE_PATH "/sdcard"

#define SD_DIR_IR     "/ir"
#define SD_DIR_BADUSB "/badusb"
#define SD_DIR_NFC    "/nfc"
#define SD_DIR_RFID   "/rfid"
#define SD_DIR_SUBGHZ "/subghz"
#define SD_DIR_CONFIG "/config"
#define SD_DIR_LOGS   "/logs"
#define SD_DIR_BACKUP "/backups"

typedef void (*sd_dir_callback_t)(const char *name, bool is_dir, void *user_data);

/**
 * @brief Create a directory on the SD card.
 *
 * @param path Relative path under the SD mount point.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t sd_dir_create(const char *path);

/**
 * @brief Recursively remove a directory and all its contents.
 *
 * @param path Relative path under the SD mount point.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t sd_dir_remove_recursive(const char *path);

/**
 * @brief Check if a directory exists.
 *
 * @param path Relative path under the SD mount point.
 * @return true if the directory exists.
 */
bool sd_dir_exists(const char *path);

/**
 * @brief List directory contents via callback.
 *
 * @param path Relative path under the SD mount point.
 * @param callback Function called for each entry.
 * @param user_data Opaque pointer forwarded to the callback.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t sd_dir_list(const char *path, sd_dir_callback_t callback, void *user_data);

/**
 * @brief Count files and subdirectories in a directory.
 *
 * @param path Relative path under the SD mount point.
 * @param file_count Pointer to receive the file count.
 * @param dir_count Pointer to receive the directory count.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t sd_dir_count(const char *path, uint32_t *file_count, uint32_t *dir_count);

/**
 * @brief Recursively copy a directory tree.
 *
 * @param src Source path relative to SD mount point.
 * @param dst Destination path relative to SD mount point.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t sd_dir_copy_recursive(const char *src, const char *dst);

/**
 * @brief Get the total size of all files in a directory tree.
 *
 * @param path Relative path under the SD mount point.
 * @param total_size Pointer to receive the total size in bytes.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t sd_dir_get_size(const char *path, uint64_t *total_size);

#ifdef __cplusplus
}
#endif

#endif // SD_CARD_DIR_H
