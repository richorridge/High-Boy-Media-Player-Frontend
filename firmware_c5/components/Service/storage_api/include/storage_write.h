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
 * @file storage_write.h
 * @brief Write operations — backend agnostic.
 */

#ifndef STORAGE_WRITE_H
#define STORAGE_WRITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

/**
 * @brief Write a null-terminated string to a file (overwrite).
 *
 * @param path  File path.
 * @param data  String to write.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_string(const char *path, const char *data);

/**
 * @brief Append a null-terminated string to a file.
 *
 * @param path  File path.
 * @param data  String to append.
 * @return ESP_OK on success.
 */
esp_err_t storage_append_string(const char *path, const char *data);

/**
 * @brief Write raw binary data to a file (overwrite).
 *
 * @param path  File path.
 * @param data  Data buffer.
 * @param size  Number of bytes to write.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_binary(const char *path, const void *data, size_t size);

/**
 * @brief Append raw binary data to a file.
 *
 * @param path  File path.
 * @param data  Data buffer.
 * @param size  Number of bytes to append.
 * @return ESP_OK on success.
 */
esp_err_t storage_append_binary(const char *path, const void *data, size_t size);

/**
 * @brief Write a line (with trailing newline) to a file (overwrite).
 *
 * @param path  File path.
 * @param line  Line content without newline.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_line(const char *path, const char *line);

/**
 * @brief Append a line (with trailing newline) to a file.
 *
 * @param path  File path.
 * @param line  Line content without newline.
 * @return ESP_OK on success.
 */
esp_err_t storage_append_line(const char *path, const char *line);

/**
 * @brief Write a printf-formatted string to a file (overwrite).
 *
 * @param path    File path.
 * @param format  printf format string.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_formatted(const char *path, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * @brief Append a printf-formatted string to a file.
 *
 * @param path    File path.
 * @param format  printf format string.
 * @return ESP_OK on success.
 */
esp_err_t storage_append_formatted(const char *path, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * @brief Write a raw buffer to a file (overwrite).
 *
 * @param path    File path.
 * @param buffer  Data buffer.
 * @param size    Number of bytes.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_buffer(const char *path, const void *buffer, size_t size);

/**
 * @brief Write an array of bytes to a file (overwrite).
 *
 * @param path   File path.
 * @param bytes  Byte array.
 * @param count  Number of bytes.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_bytes(const char *path, const uint8_t *bytes, size_t count);

/**
 * @brief Write a single byte to a file (overwrite).
 *
 * @param path  File path.
 * @param byte  The byte to write.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_byte(const char *path, uint8_t byte);

/**
 * @brief Write a 32-bit integer as a string to a file (overwrite).
 *
 * @param path   File path.
 * @param value  Integer value.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_int(const char *path, int32_t value);

/**
 * @brief Write a float as a string to a file (overwrite).
 *
 * @param path   File path.
 * @param value  Float value.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_float(const char *path, float value);

/**
 * @brief Write a CSV row to a file (overwrite).
 *
 * @param path         File path.
 * @param columns      Array of column strings.
 * @param num_columns  Number of columns.
 * @return ESP_OK on success.
 */
esp_err_t storage_write_csv_row(const char *path, const char **columns, size_t num_columns);

/**
 * @brief Append a CSV row to a file.
 *
 * @param path         File path.
 * @param columns      Array of column strings.
 * @param num_columns  Number of columns.
 * @return ESP_OK on success.
 */
esp_err_t storage_append_csv_row(const char *path, const char **columns, size_t num_columns);

#ifdef __cplusplus
}
#endif

#endif