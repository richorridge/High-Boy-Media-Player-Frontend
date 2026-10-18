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

#ifndef PROBE_MONITOR_H
#define PROBE_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Record of a captured probe request.
 */
typedef struct {
  uint8_t mac[6];
  int8_t rssi;
  char ssid[33];
  uint32_t last_seen_timestamp;
} probe_monitor_record_t;

/**
 * @brief Start the probe request monitor.
 *
 * @return true on success, false on failure.
 */
bool probe_monitor_start(void);

/**
 * @brief Stop the probe request monitor.
 */
void probe_monitor_stop(void);

/**
 * @brief Get the captured probe request results.
 *
 * @param[out] out_count  Number of probe records found.
 * @return Pointer to result array.
 */
probe_monitor_record_t *probe_monitor_get_results(uint16_t *out_count);

/**
 * @brief Get a pointer to the live result count.
 *
 * @return Pointer to the internal count variable.
 */
const uint16_t *probe_monitor_get_count_ptr(void);

/**
 * @brief Free probe results and stop monitoring.
 */
void probe_monitor_free_results(void);

/**
 * @brief Save results to internal flash storage.
 *
 * @return true on success, false on failure.
 */
bool probe_monitor_save_results_to_internal_flash(void);

/**
 * @brief Save results to SD card.
 *
 * @return true on success, false on failure.
 */
bool probe_monitor_save_results_to_sd_card(void);

#ifdef __cplusplus
}
#endif

#endif // PROBE_MONITOR_H
