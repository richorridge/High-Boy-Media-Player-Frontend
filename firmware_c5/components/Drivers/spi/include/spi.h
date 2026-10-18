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

#ifndef SPI_H
#define SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/spi_master.h"

/**
 * @brief Registered SPI device identifiers.
 */
typedef enum {
  SPI_DEVICE_ST7789,
  SPI_DEVICE_CC1101,
  SPI_DEVICE_SD_CARD,
  SPI_DEVICE_MAX
} spi_device_id_t;

/**
 * @brief Configuration for adding a device to the SPI bus.
 */
typedef struct {
  int cs_pin;
  int clock_speed_hz;
  int mode;
  int queue_size;
} spi_device_config_t;

/**
 * @brief Initialize the SPI bus using pin_def.h pins.
 *
 * @return ESP_OK on success or if already initialized.
 */
esp_err_t spi_init(void);

/**
 * @brief Add a device to the SPI bus.
 *
 * @param id      Device identifier.
 * @param config  Device configuration.
 * @return ESP_OK on success or if device already registered.
 */
esp_err_t spi_add_device(spi_device_id_t id, const spi_device_config_t *config);

/**
 * @brief Get the ESP-IDF handle for a registered device.
 *
 * @param id  Device identifier.
 * @return Handle, or NULL if not registered.
 */
spi_device_handle_t spi_get_handle(spi_device_id_t id);

/**
 * @brief Perform a blocking SPI transmission.
 *
 * @param id    Device identifier.
 * @param data  Pointer to TX data buffer.
 * @param len   Number of bytes to transmit.
 * @return ESP_OK on success, or an error code.
 */
esp_err_t spi_transmit(spi_device_id_t id, const uint8_t *data, size_t len);

/**
 * @brief Deinitialize the SPI bus and free all devices.
 *
 * @return ESP_OK on success.
 */
esp_err_t spi_deinit(void);

#ifdef __cplusplus
}
#endif

#endif // SPI_H
