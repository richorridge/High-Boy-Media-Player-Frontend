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

#include "boot_ui.h"

#include "lvgl.h"

#define BOOT_BG_COLOR      0x000000
#define BOOT_TEXT_COLOR    0xFFFFFF
#define BOOT_TEXT          "Loading Media"
#define BOOT_PULSE_MIN_OPA 110
#define BOOT_PULSE_MS      1100
#define BOOT_FADE_IN_MS    500

static void boot_text_opa_cb(void *var, int32_t v) {
  lv_obj_set_style_text_opa((lv_obj_t *)var, (lv_opa_t)v, 0);
}

static void boot_fade_done_cb(lv_anim_t *fade) {
  lv_anim_t pulse;
  lv_anim_init(&pulse);
  lv_anim_set_var(&pulse, fade->var);
  lv_anim_set_values(&pulse, LV_OPA_COVER, BOOT_PULSE_MIN_OPA);
  lv_anim_set_duration(&pulse, BOOT_PULSE_MS);
  lv_anim_set_playback_duration(&pulse, BOOT_PULSE_MS);
  lv_anim_set_repeat_count(&pulse, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_path_cb(&pulse, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&pulse, boot_text_opa_cb);
  lv_anim_start(&pulse);
}

void ui_boot_show(void) {
  lv_obj_t *boot_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(boot_screen, lv_color_hex(BOOT_BG_COLOR), 0);
  lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
  lv_obj_remove_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_clean(boot_screen);
  lv_screen_load(boot_screen);

  lv_obj_t *label = lv_label_create(boot_screen);
  lv_label_set_text(label, BOOT_TEXT);
  lv_obj_set_style_text_color(label, lv_color_hex(BOOT_TEXT_COLOR), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_opa(label, LV_OPA_TRANSP, 0);
  lv_obj_center(label);

  // Fade in once, then breathe between dim and full white until the home hub loads.
  lv_anim_t fade;
  lv_anim_init(&fade);
  lv_anim_set_var(&fade, label);
  lv_anim_set_values(&fade, LV_OPA_TRANSP, LV_OPA_COVER);
  lv_anim_set_duration(&fade, BOOT_FADE_IN_MS);
  lv_anim_set_exec_cb(&fade, boot_text_opa_cb);
  lv_anim_set_completed_cb(&fade, boot_fade_done_cb);
  lv_anim_start(&fade);

  lv_refr_now(NULL);
}
