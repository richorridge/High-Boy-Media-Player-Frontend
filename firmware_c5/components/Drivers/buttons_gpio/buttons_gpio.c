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

#include "buttons_gpio.h"

#include "driver/gpio.h"
#include "pin_def.h"

static const char *TAG = "BUTTONS_GPIO";

#define BUTTON_PRESSED_LEVEL 0

static button_t s_buttons[] = {
    {GPIO_BTN_UP_PIN, true, false},
    {GPIO_BTN_DOWN_PIN, true, false},
    {GPIO_BTN_LEFT_PIN, true, false},
    {GPIO_BTN_RIGHT_PIN, true, false},
    {GPIO_BTN_OK_PIN, true, false},
    {GPIO_BTN_BACK_PIN, true, false},
};

#define NUM_BUTTONS (sizeof(s_buttons) / sizeof(s_buttons[0]))

static bool get_raw_level(uint32_t gpio) {
  return gpio_get_level(gpio) == BUTTON_PRESSED_LEVEL;
}

bool up_button_pressed(void) {
  return s_buttons[0].pressed_flag &&
         (__atomic_test_and_set(&s_buttons[0].pressed_flag, __ATOMIC_RELAXED), false);
}
bool down_button_pressed(void) {
  return s_buttons[1].pressed_flag &&
         (__atomic_test_and_set(&s_buttons[1].pressed_flag, __ATOMIC_RELAXED), false);
}
bool left_button_pressed(void) {
  return s_buttons[2].pressed_flag &&
         (__atomic_test_and_set(&s_buttons[2].pressed_flag, __ATOMIC_RELAXED), false);
}
bool right_button_pressed(void) {
  return s_buttons[3].pressed_flag &&
         (__atomic_test_and_set(&s_buttons[3].pressed_flag, __ATOMIC_RELAXED), false);
}
bool ok_button_pressed(void) {
  return s_buttons[4].pressed_flag &&
         (__atomic_test_and_set(&s_buttons[4].pressed_flag, __ATOMIC_RELAXED), false);
}
bool back_button_pressed(void) {
  return s_buttons[5].pressed_flag &&
         (__atomic_test_and_set(&s_buttons[5].pressed_flag, __ATOMIC_RELAXED), false);
}

bool up_button_is_down(void) {
  return get_raw_level(GPIO_BTN_UP_PIN);
}
bool down_button_is_down(void) {
  return get_raw_level(GPIO_BTN_DOWN_PIN);
}
bool left_button_is_down(void) {
  return get_raw_level(GPIO_BTN_LEFT_PIN);
}
bool right_button_is_down(void) {
  return get_raw_level(GPIO_BTN_RIGHT_PIN);
}
bool ok_button_is_down(void) {
  return get_raw_level(GPIO_BTN_OK_PIN);
}
bool back_button_is_down(void) {
  return get_raw_level(GPIO_BTN_BACK_PIN);
}

void buttons_task(void) {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool current = get_raw_level(s_buttons[i].gpio);

    if (s_buttons[i].last_state == true && current == false) {
      s_buttons[i].pressed_flag = true;
    }

    s_buttons[i].last_state = current;
  }
}
void buttons_init(void) {
  gpio_config_t io_conf = {
      .intr_type = GPIO_INTR_DISABLE,
      .mode = GPIO_MODE_INPUT,
      .pin_bit_mask =
          ((1ULL << GPIO_BTN_UP_PIN) | (1ULL << GPIO_BTN_DOWN_PIN) | (1ULL << GPIO_BTN_LEFT_PIN) |
           (1ULL << GPIO_BTN_RIGHT_PIN) | (1ULL << GPIO_BTN_OK_PIN) | (1ULL << GPIO_BTN_BACK_PIN)),
      .pull_down_en = 0,
      .pull_up_en = 1,
  };
  gpio_config(&io_conf);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    s_buttons[i].last_state = get_raw_level(s_buttons[i].gpio);
    s_buttons[i].pressed_flag = false;
  }
}
