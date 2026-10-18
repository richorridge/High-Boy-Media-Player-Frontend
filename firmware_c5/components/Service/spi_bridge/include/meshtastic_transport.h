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

#ifndef MESHTASTIC_TRANSPORT_H
#define MESHTASTIC_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#include "spi_protocol.h"

/**
 * @brief Initialize the Meshtastic transport core.
 *
 * Allocates internal mutexes and clears reassembly state. Idempotent.
 *
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_NO_MEM if mutex allocation fails
 */
esp_err_t meshtastic_transport_init(void);

/**
 * @brief Inject a FromRadio chunk received from the P4 over SPI.
 *
 * Reassembles by seq. When the last chunk arrives, increments the
 * fromnum counter and fans out the assembled frame to BLE GATT
 * (FROMRADIO + FROMNUM notify) and TCP (framed StreamAPI write).
 *
 * @param payload  Raw SPI command payload (chunk header + data).
 * @param len      Length of payload in bytes.
 */
void meshtastic_transport_inject_fromradio_chunk(const uint8_t *payload, uint8_t len);

/**
 * @brief Inject a Logradio chunk received from the P4 over SPI.
 *
 * Reassembles by seq. When the last chunk arrives, dispatches the
 * assembled bytes to BLE GATT LOGRADIO notify if subscribed.
 *
 * @param payload  Raw SPI command payload (chunk header + data).
 * @param len      Length of payload in bytes.
 */
void meshtastic_transport_inject_log_chunk(const uint8_t *payload, uint8_t len);

/**
 * @brief Forward a ToRadio frame received from a phone to the P4.
 *
 * Splits the frame into chunks of up to SPI_MESH_CHUNK_PAYLOAD_MAX bytes,
 * tags each with a per-pipe seq, and pushes them via spi_bridge stream
 * SPI_ID_MESH_TORADIO_STREAM for the P4 to reassemble and feed phoneapi.
 *
 * @param frame  ToRadio protobuf bytes (no transport framing).
 * @param len    Length of frame in bytes.
 * @return true if all chunks were enqueued, false if the stream queue
 *         filled up.
 */
bool meshtastic_transport_send_toradio(const uint8_t *frame, uint16_t len);

/**
 * @brief Fill a status snapshot consumed by SPI_ID_MESH_STATUS responses.
 *
 * Aggregates connection state from the BLE GATT and TCP transports plus
 * the current fromnum counter.
 *
 * @param[out] out_status  Destination struct. Must not be NULL.
 */
void meshtastic_transport_get_status(spi_mesh_status_t *out_status);

/**
 * @brief Get and bump the fromnum counter.
 *
 * Internal helper used by GATT to seed FROMNUM notifications.
 *
 * @return The new counter value (post-increment).
 */
uint32_t meshtastic_transport_bump_fromnum(void);

/**
 * @brief Reset reassembly state and the fromnum counter.
 *
 * Called when both transports go offline so the next session starts
 * with a clean handshake.
 */
void meshtastic_transport_reset(void);

#ifdef __cplusplus
}
#endif

#endif // MESHTASTIC_TRANSPORT_H
