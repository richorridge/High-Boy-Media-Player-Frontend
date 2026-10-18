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

#include "home_ui.h"

#include <stddef.h>

#include "esp_log.h"
#include "lvgl.h"

#include "dropdown_ui.h"
#include "header_ui.h"
#include "lv_port_indev.h"
#include "ui_feedback.h"
#include "ui_manager.h"
#include "ui_metrics.h"

static const char *TAG = "HOME_UI";

// Zune-style hub: one column of big lowercase titles on black. The selected title is
// white, the rest fade to grey, and the list scrolls so the last entry ("high boy",
// which hides every original High Boy tool) starts below the fold.
#define HOME_HEADER_HEIGHT_PCT 9
#define HOME_HEADER_HEIGHT     ((ui_screen_h() * HOME_HEADER_HEIGHT_PCT) / 100)

#define HOME_BG_COLOR     0x000000
#define HOME_ACCENT_COLOR 0xA23BEC
#define HOME_SEL_COLOR    0xFFFFFF
#define HOME_NEAR_COLOR   0x7A7A7A
#define HOME_FAR_COLOR    0x4D4D4D

#define HOME_ITEM_H        68
#define HOME_ITEM_PAD_L    20
#define HOME_BAR_W         4
#define HOME_BAR_H         34
#define HOME_BAR_X         8
#define HOME_SCROLL_LEAD   1
#define HOME_COLOR_FADE_MS 220

typedef struct {
  const char *name;
  screen_id_t target;
} home_item_t;

static const home_item_t s_items[] = {
    {"music", SCREEN_PLAYER},
    {"videos", SCREEN_FILES},
    {"pictures", SCREEN_FILES},
    {"settings", SCREEN_SETTINGS},
    {"high boy", SCREEN_MENU},
};

#define HOME_ITEM_COUNT ((int)(sizeof(s_items) / sizeof(s_items[0])))

static lv_obj_t *s_screen_home = NULL;
static lv_obj_t *s_list = NULL;
static lv_obj_t *s_labels[HOME_ITEM_COUNT];
static lv_obj_t *s_bars[HOME_ITEM_COUNT];
static int s_sel = 0;

static lv_style_t s_fade_style;
static lv_style_transition_dsc_t s_fade_trans;
static bool s_style_ready = false;

static void home_event_cb(lv_event_t *e);

static void home_style_init(void) {
  if (s_style_ready)
    return;
  static const lv_style_prop_t props[] = {LV_STYLE_TEXT_COLOR, LV_STYLE_BG_OPA, LV_STYLE_PROP_INV};
  lv_style_transition_dsc_init(
      &s_fade_trans, props, lv_anim_path_ease_out, HOME_COLOR_FADE_MS, 0, NULL);
  lv_style_init(&s_fade_style);
  lv_style_set_transition(&s_fade_style, &s_fade_trans);
  s_style_ready = true;
}

static int32_t item_scroll_target(int sel) {
  int lead = sel - HOME_SCROLL_LEAD;
  return lead > 0 ? lead * HOME_ITEM_H : 0;
}

static lv_color_t item_color(int idx) {
  int dist = idx > s_sel ? idx - s_sel : s_sel - idx;
  if (dist == 0)
    return lv_color_hex(HOME_SEL_COLOR);
  return lv_color_hex(dist == 1 ? HOME_NEAR_COLOR : HOME_FAR_COLOR);
}

static void home_refresh(bool animate) {
  for (int i = 0; i < HOME_ITEM_COUNT; i++) {
    lv_obj_set_style_text_color(s_labels[i], item_color(i), 0);
    lv_obj_set_style_bg_opa(s_bars[i], i == s_sel ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
  }
  lv_obj_scroll_to_y(s_list, item_scroll_target(s_sel), animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

static void home_move(int step) {
  int next = s_sel + step;
  if (next < 0 || next >= HOME_ITEM_COUNT)
    return;
  s_sel = next;
  ui_feedback(UI_FB_NAV);
  home_refresh(true);
}

void ui_home_open(void) {
  ESP_LOGI(TAG, "home open: selected=%d", s_sel);

  if (s_screen_home != NULL) {
    lv_obj_del(s_screen_home);
    s_screen_home = NULL;
  }

  home_style_init();

  s_screen_home = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(s_screen_home, lv_color_hex(HOME_BG_COLOR), 0);
  lv_obj_set_style_bg_opa(s_screen_home, LV_OPA_COVER, 0);
  lv_obj_remove_flag(s_screen_home, LV_OBJ_FLAG_SCROLLABLE);

  int32_t header_h = HOME_HEADER_HEIGHT;
  int32_t view_h = ui_screen_h() - header_h;

  s_list = lv_obj_create(s_screen_home);
  lv_obj_set_size(s_list, ui_screen_w(), view_h);
  lv_obj_align(s_list, LV_ALIGN_TOP_MID, 0, header_h);
  lv_obj_remove_flag(s_list, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_scrollbar_mode(s_list, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_scroll_dir(s_list, LV_DIR_VER);
  lv_obj_set_style_bg_opa(s_list, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(s_list, 0, 0);
  lv_obj_set_style_radius(s_list, 0, 0);
  lv_obj_set_style_pad_all(s_list, 0, 0);
  lv_obj_set_style_pad_row(s_list, 0, 0);
  lv_obj_set_style_pad_bottom(s_list, view_h, 0);
  lv_obj_set_flex_flow(s_list, LV_FLEX_FLOW_COLUMN);

  const lv_font_t *font = &lv_font_montserrat_40;

  for (int i = 0; i < HOME_ITEM_COUNT; i++) {
    lv_obj_t *row = lv_obj_create(s_list);
    lv_obj_set_size(row, LV_PCT(100), HOME_ITEM_H);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_radius(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);

    // The accent marker lives in its row so it scrolls with the list.
    lv_obj_t *bar = lv_obj_create(row);
    lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(bar, HOME_BAR_W, HOME_BAR_H);
    lv_obj_align(bar, LV_ALIGN_LEFT_MID, HOME_BAR_X, 0);
    lv_obj_set_style_bg_color(bar, lv_color_hex(HOME_ACCENT_COLOR), 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_add_style(bar, &s_fade_style, 0);
    s_bars[i] = bar;

    lv_obj_t *l = lv_label_create(row);
    lv_label_set_text(l, s_items[i].name);
    lv_label_set_long_mode(l, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_font(l, font, 0);
    lv_obj_align(l, LV_ALIGN_LEFT_MID, HOME_ITEM_PAD_L, 0);
    lv_obj_add_style(l, &s_fade_style, 0);
    s_labels[i] = l;
  }

  // Status cluster (clock / wifi / ble / sd / battery) sits over a black bar.
  header_ui_create(s_screen_home);
  lv_obj_t *header = lv_obj_get_child(s_screen_home, -1);
  lv_obj_set_style_bg_color(header, lv_color_hex(HOME_BG_COLOR), 0);

  lv_obj_update_layout(s_screen_home);
  home_refresh(false);

  lv_obj_add_event_cb(s_screen_home, home_event_cb, LV_EVENT_KEY, NULL);

  if (main_group != NULL) {
    lv_group_add_obj(main_group, s_screen_home);
    lv_group_focus_obj(s_screen_home);
  }

  ui_screen_load_owned(&s_screen_home, s_screen_home);
}

static void home_event_cb(lv_event_t *e) {
  if (dropdown_ui_is_open())
    return;

  if (ui_input_is_locked())
    return;

  if (lv_event_get_code(e) != LV_EVENT_KEY)
    return;

  switch (lv_event_get_key(e)) {
    case LV_KEY_UP:
    case LV_KEY_LEFT:
      home_move(-1);
      break;
    case LV_KEY_DOWN:
    case LV_KEY_RIGHT:
      home_move(1);
      break;
    case LV_KEY_ENTER:
      ui_feedback(UI_FB_SELECT);
      ui_switch_screen_from_home(s_items[s_sel].target);
      break;
    default:
      break;
  }
}
