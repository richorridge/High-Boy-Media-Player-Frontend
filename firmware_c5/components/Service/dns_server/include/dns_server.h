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

#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the captive-portal DNS server task.
 *
 * Listens on UDP port 53 and responds to all A-record queries with
 * the AP interface IP address. Used for evil twin captive portal.
 */
void start_dns_server(void);

/**
 * @brief Stop the DNS server task.
 */
void stop_dns_server(void);

#ifdef __cplusplus
}
#endif

#endif // DNS_SERVER_H
