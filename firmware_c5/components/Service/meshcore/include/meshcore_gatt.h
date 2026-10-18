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

#ifndef MESHCORE_GATT_H
#define MESHCORE_GATT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

/**
 * @brief Initialize the MeshCore Companion BLE NUS service.
 *
 * Brings up NimBLE with SC + MITM + DISP_ONLY pairing using the static
 * passkey provided by the P4. Advertises a Nordic UART Service-style
 * primary with the MeshCore Companion characteristics (write + notify).
 *
 * Mutually exclusive with the generic bluetooth_service and with the
 * Meshtastic GATT — only one BLE-mesh stack may be active at a time.
 *
 * @param name_prefix  Advertised name prefix; the final name becomes
 *                     "<prefix>-XXXX" (last 2 octets of MAC). Required.
 * @param pin          6-digit static passkey (100000..999999).
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_INVALID_ARG if name_prefix is NULL
 *   - ESP_ERR_INVALID_STATE if BLE is already owned by another module
 *   - ESP_FAIL on NimBLE stack failure
 */
esp_err_t meshcore_gatt_init(const char *name_prefix, uint32_t pin);

/**
 * @brief Tear down the GATT service and stop NimBLE.
 */
void meshcore_gatt_stop(void);

/**
 * @brief Whether NimBLE has been initialized by this module.
 */
bool meshcore_gatt_is_running(void);

/**
 * @brief Whether a phone is currently connected.
 */
bool meshcore_gatt_is_connected(void);

/**
 * @brief Whether the phone has subscribed to the TX notify characteristic.
 */
bool meshcore_gatt_is_subscribed(void);

/**
 * @brief Notify the phone with a Companion frame received from the P4.
 *
 * Drops silently if no phone is connected or has not subscribed.
 *
 * @param frame  Companion frame bytes (1-byte tag + payload).
 * @param len    Length in bytes (<= 240).
 */
void meshcore_gatt_notify(const uint8_t *frame, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // MESHCORE_GATT_H
