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

#ifndef KERNEL_H
#define KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all kernel subsystems and start background tasks.
 */
void kernel_init(void);

/**
 * @brief Log a safeguard alert.
 *
 * @param title    Alert title.
 * @param message  Alert message.
 */
void safeguard_alert(const char *title, const char *message);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_H
