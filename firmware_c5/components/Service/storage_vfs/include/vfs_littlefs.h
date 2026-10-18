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
 * @file vfs_littlefs.h
 * @brief LittleFS backend interface.
 */
#ifndef VFS_LITTLEFS_H
#define VFS_LITTLEFS_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Initialize the LittleFS backend. */
esp_err_t vfs_littlefs_init(void);

/** @brief Deinitialize the LittleFS backend. */
esp_err_t vfs_littlefs_deinit(void);

/** @brief Check if LittleFS is currently mounted. */
bool vfs_littlefs_is_mounted(void);

/** @brief Print LittleFS partition info to the log. */
void vfs_littlefs_print_info(void);

/** @brief Format the LittleFS partition and remount. */
esp_err_t vfs_littlefs_format(void);

/** @brief Register the LittleFS backend with the VFS layer. */
esp_err_t vfs_register_littlefs_backend(void);

/** @brief Unregister the LittleFS backend from the VFS layer. */
esp_err_t vfs_unregister_littlefs_backend(void);

#ifdef __cplusplus
}
#endif

#endif // VFS_LITTLEFS_H
