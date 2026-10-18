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

#ifndef BUTTONS_GPIO_H
#define BUTTONS_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "pin_def.h"

typedef struct {
  uint32_t gpio;
  bool last_state;
  bool pressed_flag;
} button_t;

/**
 * @brief Initialize all button GPIOs.
 */
void buttons_init(void);

/**
 * @brief Poll button states. Call periodically from a task.
 */
void buttons_task(void);

/**
 * @brief Check if the up button was pressed (clears flag on read).
 * @return true if pressed since last check.
 */
bool up_button_pressed(void);

/**
 * @brief Check if the down button was pressed (clears flag on read).
 * @return true if pressed since last check.
 */
bool down_button_pressed(void);

/**
 * @brief Check if the left button was pressed (clears flag on read).
 * @return true if pressed since last check.
 */
bool left_button_pressed(void);

/**
 * @brief Check if the right button was pressed (clears flag on read).
 * @return true if pressed since last check.
 */
bool right_button_pressed(void);

/**
 * @brief Check if the ok button was pressed (clears flag on read).
 * @return true if pressed since last check.
 */
bool ok_button_pressed(void);

/**
 * @brief Check if the back button was pressed (clears flag on read).
 * @return true if pressed since last check.
 */
bool back_button_pressed(void);

/**
 * @brief Check if the up button is currently held down.
 * @return true if held down.
 */
bool up_button_is_down(void);

/**
 * @brief Check if the down button is currently held down.
 * @return true if held down.
 */
bool down_button_is_down(void);

/**
 * @brief Check if the left button is currently held down.
 * @return true if held down.
 */
bool left_button_is_down(void);

/**
 * @brief Check if the right button is currently held down.
 * @return true if held down.
 */
bool right_button_is_down(void);

/**
 * @brief Check if the ok button is currently held down.
 * @return true if held down.
 */
bool ok_button_is_down(void);

/**
 * @brief Check if the back button is currently held down.
 * @return true if held down.
 */
bool back_button_is_down(void);

#ifdef __cplusplus
}
#endif

#endif // BUTTONS_GPIO_H
