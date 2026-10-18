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

#include "led_control.h"

#include "led_strip.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "pin_def.h"

static const char *TAG = "LED_CONTROL";

#define RMT_RESOLUTION_HZ (10 * 1000 * 1000)

#define LED_COLOR_RED_R  255
#define LED_COLOR_RED_G  0
#define LED_COLOR_RED_B  0
#define LED_BLINK_RED_MS 500

#define LED_COLOR_GREEN_R  0
#define LED_COLOR_GREEN_G  150
#define LED_COLOR_GREEN_B  0
#define LED_BLINK_GREEN_MS 220

#define LED_COLOR_BLUE_R  0
#define LED_COLOR_BLUE_G  0
#define LED_COLOR_BLUE_B  255
#define LED_BLINK_BLUE_MS 500

#define LED_COLOR_PURPLE_R  200
#define LED_COLOR_PURPLE_G  0
#define LED_COLOR_PURPLE_B  220
#define LED_BLINK_PURPLE_MS 500

static led_strip_handle_t s_led_strip;

void led_rgb_init(void) {
  led_strip_config_t strip_config = {
      .strip_gpio_num = GPIO_LED_RGB_PIN,
      .max_leds = 1,
  };
  led_strip_rmt_config_t rmt_config = {
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = RMT_RESOLUTION_HZ,
      .flags.with_dma = false,
  };
  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip));
  ESP_ERROR_CHECK(led_strip_clear(s_led_strip));
  ESP_LOGI(TAG, "LED RGB inicializado no GPIO %d", GPIO_LED_RGB_PIN);
}

static void led_blink_color(uint8_t r, uint8_t g, uint8_t b, int duration_ms) {
  if (s_led_strip == NULL) {
    return;
  }
  ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, r, g, b));
  ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
  vTaskDelay(duration_ms / portTICK_PERIOD_MS);
  ESP_ERROR_CHECK(led_strip_clear(s_led_strip));
  ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void led_blink_red(void) {
  ESP_LOGI(TAG, "Piscando LED vermelho (erro)");
  led_blink_color(LED_COLOR_RED_R, LED_COLOR_RED_G, LED_COLOR_RED_B, LED_BLINK_RED_MS);
}

void led_blink_green(void) {
  ESP_LOGI(TAG, "Piscando LED verde (sucesso)");
  led_blink_color(LED_COLOR_GREEN_R, LED_COLOR_GREEN_G, LED_COLOR_GREEN_B, LED_BLINK_GREEN_MS);
}

void led_blink_blue(void) {
  ESP_LOGI(TAG, "Piscando LED azul (info)");
  led_blink_color(LED_COLOR_BLUE_R, LED_COLOR_BLUE_G, LED_COLOR_BLUE_B, LED_BLINK_BLUE_MS);
}

void led_blink_purple(void) {
  ESP_LOGI(TAG, "Piscando LED roxo (info)");
  led_blink_color(LED_COLOR_PURPLE_R, LED_COLOR_PURPLE_G, LED_COLOR_PURPLE_B, LED_BLINK_PURPLE_MS);
}
