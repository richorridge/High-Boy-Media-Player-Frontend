# Copyright (c) 2025 HIGH CODE LLC
#
# TentacleOS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TentacleOS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TentacleOS. If not, see <https://www.gnu.org/licenses/>.


#APPLICATION
file(GLOB_RECURSE BLE_APP_SRCS "bluetooth/*.c")
file(GLOB_RECURSE ESPNOW_CHAT_APP_SRCS "espnow_chat/*.c")
file(GLOB_RECURSE WIFI_APP_SRCS "wifi/*.c")

idf_component_register(SRCS 
  ${WIFI_APP_SRCS}
  ${ESPNOW_CHAT_APP_SRCS}
  ${BLE_APP_SRCS}

  INCLUDE_DIRS 
  "wifi/include"   
  "espnow_chat/include"
  "bluetooth/include"


  REQUIRES 
  driver
  Drivers
  Service
  esp_common
  esp_wifi
  bt
)
target_link_libraries(${COMPONENT_LIB} -Wl,-zmuldefs)
