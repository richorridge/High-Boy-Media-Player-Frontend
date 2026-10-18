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

#ifndef BT_DISPATCHER_H
#define BT_DISPATCHER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "spi_protocol.h"

/**
 * @brief Execute a Bluetooth command received via SPI.
 *
 * @param id               Command identifier.
 * @param payload          Pointer to the command payload.
 * @param len              Payload length in bytes.
 * @param out_resp_payload Buffer to fill with the response data.
 * @param out_resp_len     Pointer to store the response length.
 * @return SPI status code indicating the operation result.
 */
spi_status_t bt_dispatcher_execute(spi_id_t id,
                                   const uint8_t *payload,
                                   uint8_t len,
                                   uint8_t *out_resp_payload,
                                   uint8_t *out_resp_len);

#ifdef __cplusplus
}
#endif

#endif // BT_DISPATCHER_H
