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

#ifndef ESPNOW_CHAT_H
#define ESPNOW_CHAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

typedef void (*espnow_chat_on_msg_cb_t)(const char *sender_nick,
                                        const char *message,
                                        bool is_system_msg);
typedef void (*espnow_chat_on_device_list_cb_t)(void);

typedef struct {
  uint8_t mac[6];
  char nick[16];
  int8_t rssi;
  bool is_saved;
} espnow_chat_peer_t;

/** @brief Initialize the chat application and underlying ESP-NOW service. */
esp_err_t espnow_chat_init(void);

/** @brief Deinitialize the chat application. */
void espnow_chat_deinit(void);

/** @brief Set the local nickname. */
void espnow_chat_set_nick(const char *nick);

/** @brief Get the current local nickname. */
const char *espnow_chat_get_nick(void);

/** @brief Set online/offline status. */
void espnow_chat_set_online(bool online);

/** @brief Check if the chat is online. */
bool espnow_chat_is_online(void);

/** @brief Set the encryption key for secure messaging. */
void espnow_chat_set_security_key(const char *key);

/** @brief Send a text message to a specific peer. */
esp_err_t espnow_chat_send_message(const uint8_t *target_mac, const char *message);

/** @brief Broadcast a discovery hello packet. */
esp_err_t espnow_chat_broadcast_discovery(void);

/** @brief Initiate a secure pairing with a target peer. */
esp_err_t espnow_chat_secure_pair(const uint8_t *target_mac);

/** @brief Register a callback for incoming messages. */
void espnow_chat_register_msg_cb(espnow_chat_on_msg_cb_t cb);

/** @brief Register a callback for device list refresh events. */
void espnow_chat_register_refresh_cb(espnow_chat_on_device_list_cb_t cb);

/** @brief Get the list of discovered peers. */
int espnow_chat_get_peer_list(espnow_chat_peer_t *peers, int max_count);

/** @brief Save a peer to the persistent configuration. */
esp_err_t espnow_chat_save_peer(const uint8_t *mac, const char *name);

#ifdef __cplusplus
}
#endif

#endif // ESPNOW_CHAT_H
