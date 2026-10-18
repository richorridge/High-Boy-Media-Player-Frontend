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

#ifndef SIGNAL_MONITOR_H
#define SIGNAL_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Start monitoring the signal strength of a specific BSSID.
 *
 * @param bssid    BSSID to monitor (6 bytes).
 * @param channel  Wi-Fi channel to listen on.
 */
void signal_monitor_start(const uint8_t *bssid, uint8_t channel);

/**
 * @brief Stop the signal monitor and free resources.
 */
void signal_monitor_stop(void);

/**
 * @brief Get the last observed RSSI value.
 *
 * @return RSSI in dBm, or -127 if no signal detected.
 */
int8_t signal_monitor_get_rssi(void);

/**
 * @brief Get time elapsed since the last packet was seen.
 *
 * @return Milliseconds since last packet, or UINT32_MAX if never seen.
 */
uint32_t signal_monitor_get_last_seen_ms(void);

#ifdef __cplusplus
}
#endif

#endif // SIGNAL_MONITOR_H
