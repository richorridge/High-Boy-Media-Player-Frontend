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

#ifndef AP_SCANNER_H
#define AP_SCANNER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "esp_wifi_types.h"

/**
 * @brief Start an access point scan.
 *
 * @return true on success, false if a scan is already in progress.
 */
bool ap_scanner_start(void);

/**
 * @brief Get the scan results.
 *
 * @param[out] out_count  Number of access points found.
 * @return Pointer to result array, or NULL if scan is in progress.
 */
wifi_ap_record_t *ap_scanner_get_results(uint16_t *out_count);

/**
 * @brief Free scan results and associated task memory.
 */
void ap_scanner_free_results(void);

/**
 * @brief Save scan results to internal flash storage.
 *
 * @return true on success, false on failure.
 */
bool ap_scanner_save_results_to_internal_flash(void);

/**
 * @brief Save scan results to SD card.
 *
 * @return true on success, false on failure.
 */
bool ap_scanner_save_results_to_sd_card(void);

#ifdef __cplusplus
}
#endif

#endif // AP_SCANNER_H
