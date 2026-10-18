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

#ifndef SERVICE_ESP_NOW_H
#define SERVICE_ESP_NOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_now.h"

#define ESP_NOW_NICK_MAX_LEN   16
#define ESP_NOW_TEXT_MAX_LEN   201
#define ESP_NOW_BROADCAST_ADDR {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

typedef enum {
  ESP_NOW_MSG_TYPE_HELLO = 0x01,
  ESP_NOW_MSG_TYPE_MSG = 0x02,
  ESP_NOW_MSG_TYPE_KEY_SHARE = 0x03
} service_esp_now_msg_type_t;

typedef struct __attribute__((packed)) {
  uint8_t type;
  char nick[ESP_NOW_NICK_MAX_LEN];
  char text[ESP_NOW_TEXT_MAX_LEN];
} service_esp_now_packet_t;

typedef void (*service_esp_now_recv_cb_t)(const uint8_t *mac_addr,
                                          const service_esp_now_packet_t *data,
                                          int8_t rssi);

typedef void (*service_esp_now_send_cb_t)(const uint8_t *mac_addr, esp_now_send_status_t status);

typedef struct {
  uint8_t mac[6];
  char nick[ESP_NOW_NICK_MAX_LEN];
  int8_t rssi;
  bool is_permanent;
} service_esp_now_peer_info_t;

/** @brief Initialize the ESP-NOW service. */
esp_err_t service_esp_now_init(void);

/** @brief Deinitialize the ESP-NOW service. */
void service_esp_now_deinit(void);

/** @brief Set the local nickname. */
esp_err_t service_esp_now_set_nick(const char *nick);

/** @brief Get the current local nickname. */
const char *service_esp_now_get_nick(void);

/** @brief Set online/offline status. */
esp_err_t service_esp_now_set_online(bool online);

/** @brief Check if the service is online. */
bool service_esp_now_is_online(void);

/** @brief Broadcast a hello/discovery packet. */
esp_err_t service_esp_now_broadcast_hello(void);

/** @brief Send a text message to a specific peer. */
esp_err_t service_esp_now_send_msg(const uint8_t *target_mac, const char *text);

/** @brief Initiate a secure key-sharing pairing with a peer. */
esp_err_t service_esp_now_secure_pair(const uint8_t *target_mac);

/** @brief Register a callback for received messages. */
void service_esp_now_register_recv_cb(service_esp_now_recv_cb_t cb);

/** @brief Register a callback for send status. */
void service_esp_now_register_send_cb(service_esp_now_send_cb_t cb);

/** @brief Check if a peer is already registered. */
bool service_esp_now_is_peer_exist(const uint8_t *mac_addr);

/** @brief Add a peer by MAC address. */
esp_err_t service_esp_now_add_peer(const uint8_t *mac_addr);

/** @brief Save a peer to the persistent configuration file. */
esp_err_t service_esp_now_save_peer_to_conf(const uint8_t *mac_addr, const char *name);

/** @brief Get the list of peers discovered in this session. */
int service_esp_now_get_session_peers(service_esp_now_peer_info_t *out_peers, int max_peers);

/** @brief Set the encryption key for message ciphering. */
esp_err_t service_esp_now_set_key(const char *key);

/** @brief Get the current encryption key. */
const char *service_esp_now_get_key(void);

#ifdef __cplusplus
}
#endif

#endif // SERVICE_ESP_NOW_H
