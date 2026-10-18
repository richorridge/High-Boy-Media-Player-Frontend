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

/**
 * @file storage_impl.h
 * @brief File, directory, and storage info operations — backend agnostic.
 */

#ifndef STORAGE_IMPL_H
#define STORAGE_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "esp_err.h"

typedef struct {
  char path[256];
  size_t size;
  time_t modified_time;
  time_t created_time;
  bool is_directory;
  bool is_hidden;
  bool is_readonly;
} storage_file_info_t;

typedef void (*storage_dir_callback_t)(const char *name, bool is_dir, void *user_data);

typedef struct {
  char backend_name[32];
  char mount_point[64];
  bool is_mounted;
  uint64_t total_bytes;
  uint64_t free_bytes;
  uint64_t used_bytes;
  uint32_t block_size;
} storage_info_t;

/**
 * @brief Check if a file exists.
 *
 * @param path  File path.
 * @return true if the file exists.
 */
bool storage_file_exists(const char *path);

/**
 * @brief Check if a file is empty (zero bytes).
 *
 * @param path   File path.
 * @param[out] empty  Set to true when the file has zero size.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_is_empty(const char *path, bool *empty);

/**
 * @brief Get detailed information about a file.
 *
 * @param path  File path.
 * @param[out] info  Populated on success.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_get_info(const char *path, storage_file_info_t *info);

/**
 * @brief Get the size of a file in bytes.
 *
 * @param path  File path.
 * @param[out] size  File size.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_get_size(const char *path, size_t *size);

/**
 * @brief Extract the file extension from a path.
 *
 * @param path  File path.
 * @param[out] ext   Buffer to receive the extension string.
 * @param size  Size of the ext buffer.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_get_extension(const char *path, char *ext, size_t size);

/**
 * @brief Delete a file.
 *
 * @param path  File path.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_delete(const char *path);

/**
 * @brief Rename (move) a file.
 *
 * @param old_path  Current path.
 * @param new_path  New path.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_rename(const char *old_path, const char *new_path);

/**
 * @brief Copy a file.
 *
 * @param src  Source path.
 * @param dst  Destination path.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_copy(const char *src, const char *dst);

/**
 * @brief Move a file (alias for rename).
 *
 * @param src  Source path.
 * @param dst  Destination path.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_move(const char *src, const char *dst);

/**
 * @brief Truncate a file to a given size.
 *
 * @param path  File path.
 * @param size  New size in bytes.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_truncate(const char *path, size_t size);

/**
 * @brief Clear a file (truncate to zero).
 *
 * @param path  File path.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_clear(const char *path);

/**
 * @brief Compare two files byte-by-byte.
 *
 * @param path1  First file path.
 * @param path2  Second file path.
 * @param[out] equal  Set to true if files are identical.
 * @return ESP_OK on success.
 */
esp_err_t storage_file_compare(const char *path1, const char *path2, bool *equal);

/**
 * @brief Create a directory.
 *
 * @param path  Directory path.
 * @return ESP_OK on success.
 */
esp_err_t storage_dir_create(const char *path);

/**
 * @brief Remove an empty directory.
 *
 * @param path  Directory path.
 * @return ESP_OK on success.
 */
esp_err_t storage_dir_remove(const char *path);

/**
 * @brief Remove a directory and all its contents recursively.
 *
 * @param path  Directory path.
 * @return ESP_OK on success.
 */
esp_err_t storage_dir_remove_recursive(const char *path);

/**
 * @brief Check if a directory exists.
 *
 * @param path  Directory path.
 * @return true if the directory exists.
 */
bool storage_dir_exists(const char *path);

/**
 * @brief Check if a directory is empty.
 *
 * @param path   Directory path.
 * @param[out] empty  Set to true when the directory has no entries.
 * @return ESP_OK on success.
 */
esp_err_t storage_dir_is_empty(const char *path, bool *empty);

/**
 * @brief List directory entries via a callback.
 *
 * @param path       Directory path.
 * @param callback   Called once per entry.
 * @param user_data  Opaque context forwarded to the callback.
 * @return ESP_OK on success.
 */
esp_err_t storage_dir_list(const char *path, storage_dir_callback_t callback, void *user_data);

/**
 * @brief Count files and sub-directories inside a directory.
 *
 * @param path        Directory path.
 * @param[out] file_count  Number of files.
 * @param[out] dir_count   Number of sub-directories.
 * @return ESP_OK on success.
 */
esp_err_t storage_dir_count(const char *path, uint32_t *file_count, uint32_t *dir_count);

/**
 * @brief Copy a directory recursively (not yet implemented).
 *
 * @param src  Source directory.
 * @param dst  Destination directory.
 * @return ESP_ERR_NOT_SUPPORTED.
 */
esp_err_t storage_dir_copy_recursive(const char *src, const char *dst);

/**
 * @brief Get total size of a directory (not yet implemented).
 *
 * @param path         Directory path.
 * @param[out] total_size  Total size in bytes.
 * @return ESP_ERR_NOT_SUPPORTED.
 */
esp_err_t storage_dir_get_size(const char *path, uint64_t *total_size);

/**
 * @brief Get aggregate storage information.
 *
 * @param[out] info  Populated on success.
 * @return ESP_OK on success.
 */
esp_err_t storage_get_info(storage_info_t *info);

/**
 * @brief Print detailed storage information to the log.
 */
void storage_print_info_detailed(void);

/**
 * @brief Get the amount of free space.
 *
 * @param[out] free_bytes  Free space in bytes.
 * @return ESP_OK on success.
 */
esp_err_t storage_get_free_space(uint64_t *free_bytes);

/**
 * @brief Get total storage capacity.
 *
 * @param[out] total_bytes  Total space in bytes.
 * @return ESP_OK on success.
 */
esp_err_t storage_get_total_space(uint64_t *total_bytes);

/**
 * @brief Get the amount of used space.
 *
 * @param[out] used_bytes  Used space in bytes.
 * @return ESP_OK on success.
 */
esp_err_t storage_get_used_space(uint64_t *used_bytes);

/**
 * @brief Get the storage usage percentage.
 *
 * @param[out] percentage  Usage as 0..100.
 * @return ESP_OK on success.
 */
esp_err_t storage_get_usage_percent(float *percentage);

/**
 * @brief Get the backend type name string.
 *
 * @return Null-terminated backend name.
 */
const char *storage_get_backend_type(void);

/**
 * @brief Get the VFS mount point string.
 *
 * @return Null-terminated mount point path.
 */
const char *storage_get_mount_point_str(void);

#ifdef __cplusplus
}
#endif

#endif // STORAGE_IMPL_H