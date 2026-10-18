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

#ifndef STORAGE_DIRS_H
#define STORAGE_DIRS_H

#include "vfs_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_DIR_CONFIG  VFS_MOUNT_POINT "/config"
#define STORAGE_DIR_DATA    VFS_MOUNT_POINT "/data"
#define STORAGE_DIR_LOGS    VFS_MOUNT_POINT "/logs"
#define STORAGE_DIR_CACHE   VFS_MOUNT_POINT "/cache"
#define STORAGE_DIR_TEMP    VFS_MOUNT_POINT "/temp"
#define STORAGE_DIR_BACKUP  VFS_MOUNT_POINT "/backup"
#define STORAGE_DIR_CERTS   VFS_MOUNT_POINT "/certs"
#define STORAGE_DIR_SCRIPTS VFS_MOUNT_POINT "/scripts"
#define STORAGE_DIR_CAPTIVE VFS_MOUNT_POINT "/captive_portal"

#ifdef __cplusplus
}
#endif

#endif // STORAGE_DIRS_H
