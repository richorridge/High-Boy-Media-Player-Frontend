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

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the RGB LED.
 */
void led_rgb_init(void);

/**
 * @brief Blink the LED red (error indication).
 */
void led_blink_red(void);

/**
 * @brief Blink the LED green (success indication).
 */
void led_blink_green(void);

/**
 * @brief Blink the LED blue (info indication).
 */
void led_blink_blue(void);

/**
 * @brief Blink the LED purple (info indication).
 */
void led_blink_purple(void);

#ifdef __cplusplus
}
#endif

#endif // LED_CONTROL_H
