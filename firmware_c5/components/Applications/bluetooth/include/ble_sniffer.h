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

#ifndef BLE_SNIFFER_H
#define BLE_SNIFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "esp_err.h"

#include "spi_protocol.h"

/**
 * @brief Start the BLE packet sniffer.
 *
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_NO_MEM if allocation fails
 */
esp_err_t ble_sniffer_start(void);

/**
 * @brief Stop the BLE packet sniffer and free resources.
 */
void ble_sniffer_stop(void);

/** Bind a session ID to the running sniffer (called by dispatcher). */
void ble_sniffer_bind_session(uint32_t session_id);

/** Session-watchdog kill callback. */
void ble_sniffer_session_killed(spi_id_t op_id);

#ifdef __cplusplus
}
#endif

#endif // BLE_SNIFFER_H
