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

#ifndef MESHTASTIC_TCP_H
#define MESHTASTIC_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

/**
 * @brief Bring up the Meshtastic TCP server on the C5.
 *
 * Listens on port 4403 with single-client semantics, registers an mDNS
 * advertisement under _meshtastic._tcp, and uses StreamAPI framing
 * (0x94 0xC3 lenhi lenlo + protobuf) on the wire. Reuses whatever AP
 * the wifi_service has already brought up — does not change SSID.
 *
 * @param node_num  Our node number, used for the mDNS instance name.
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_INVALID_STATE if already running
 *   - Error code from socket/mdns layers otherwise
 */
esp_err_t meshtastic_tcp_init(uint32_t node_num);

/**
 * @brief Tear down the Meshtastic TCP server.
 */
void meshtastic_tcp_stop(void);

/**
 * @brief Whether the listening socket is open.
 */
bool meshtastic_tcp_is_running(void);

/**
 * @brief Number of connected TCP clients (0 or 1).
 */
uint8_t meshtastic_tcp_get_client_count(void);

/**
 * @brief Send a FromRadio frame to the connected TCP client.
 *
 * Adds StreamAPI framing (0x94 0xC3 lenhi lenlo) and writes to the
 * socket. No-op if no client is connected.
 *
 * @param frame  FromRadio protobuf bytes (no framing).
 * @param len    Length of frame in bytes.
 */
void meshtastic_tcp_send_fromradio(const uint8_t *frame, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // MESHTASTIC_TCP_H
