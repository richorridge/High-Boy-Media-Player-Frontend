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

#ifndef WIFI_FLOOD_H
#define WIFI_FLOOD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Start authentication frame flood against a target AP.
 *
 * @param target_bssid  BSSID of the target access point.
 * @param channel       Wi-Fi channel of the target.
 * @return true on success, false on failure.
 */
bool wifi_flood_auth_start(const uint8_t *target_bssid, uint8_t channel);

/**
 * @brief Start association frame flood against a target AP.
 *
 * @param target_bssid  BSSID of the target access point.
 * @param channel       Wi-Fi channel of the target.
 * @return true on success, false on failure.
 */
bool wifi_flood_assoc_start(const uint8_t *target_bssid, uint8_t channel);

/**
 * @brief Start probe request flood against a target AP.
 *
 * @param target_bssid  BSSID of the target access point.
 * @param channel       Wi-Fi channel of the target.
 * @return true on success, false on failure.
 */
bool wifi_flood_probe_start(const uint8_t *target_bssid, uint8_t channel);

/**
 * @brief Stop the flood task.
 */
void wifi_flood_stop(void);

/**
 * @brief Check if the flood task is currently running.
 *
 * @return true if running, false otherwise.
 */
bool wifi_flood_is_running(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_FLOOD_H
