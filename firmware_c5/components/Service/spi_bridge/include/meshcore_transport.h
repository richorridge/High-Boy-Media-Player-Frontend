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

#ifndef MESHCORE_TRANSPORT_H
#define MESHCORE_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#include "spi_protocol.h"

/**
 * @brief Initialize the MeshCore transport core.
 *
 * Allocates internal mutexes and clears reassembly state. Idempotent.
 *
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_NO_MEM if mutex allocation fails
 */
esp_err_t meshcore_transport_init(void);

/**
 * @brief Inject a TX chunk received from the P4 over SPI.
 *
 * Reassembles by seq. When the last chunk arrives, fans out the
 * assembled frame to the BLE NUS TX characteristic via
 * meshcore_gatt_notify.
 *
 * @param payload  Raw SPI command payload (chunk header + data).
 * @param len      Length of payload in bytes.
 */
void meshcore_transport_inject_tx_chunk(const uint8_t *payload, uint8_t len);

/**
 * @brief Forward a Companion frame received from a phone to the P4.
 *
 * Splits the frame into chunks of up to SPI_MESH_CHUNK_PAYLOAD_MAX bytes
 * and pushes them via spi_bridge stream SPI_ID_MCORE_RX_STREAM for the
 * P4 to reassemble and feed meshcore_phoneapi_on_inbound.
 *
 * @param frame  Companion frame bytes (1-byte tag + payload).
 * @param len    Length in bytes.
 * @return true if all chunks were enqueued, false if the stream queue
 *         filled up.
 */
bool meshcore_transport_send_to_p4(const uint8_t *frame, uint16_t len);

/**
 * @brief Fill a status snapshot consumed by SPI_ID_MCORE_STATUS responses.
 *
 * Aggregates connection state from the BLE GATT module.
 *
 * @param[out] out_status  Destination struct. Must not be NULL.
 */
void meshcore_transport_get_status(spi_mcore_status_t *out_status);

/**
 * @brief Reset reassembly state and the seq counter.
 *
 * Called when the BLE transport goes offline so the next session starts
 * with a clean handshake.
 */
void meshcore_transport_reset(void);

#ifdef __cplusplus
}
#endif

#endif // MESHCORE_TRANSPORT_H
