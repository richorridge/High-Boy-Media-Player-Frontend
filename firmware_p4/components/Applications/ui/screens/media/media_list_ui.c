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

#include "media_list_ui.h"

#include <stddef.h>

#include "esp_log.h"
#include "lvgl.h"

#include "dropdown_ui.h"
#include "header_ui.h"
#include "lv_port_indev.h"
#include "ui_manager.h"
#include "ui_metrics.h"
#include "zune_style.h"

static const char *TAG = "MEDIA_LIST";

#define MEDIA_HEADER_HEIGHT_PCT 9
#define MEDIA_HEADER_HEIGHT     ((ui_screen_h() * MEDIA_HEADER_HEIGHT_PCT) / 100)

#define MEDIA_TITLE_H     40
#define MEDIA_TITLE_PAD_L 16
#define MEDIA_COUNT_PAD_R 12
#define MEDIA_ROW_H       56
#define MEDIA_BAR_W       4
#define MEDIA_BAR_H       36
#define MEDIA_BAR_X       6
#define MEDIA_ART_SIZE    40
#define MEDIA_ART_X       16
#define MEDIA_ART_RADIUS  4
#define MEDIA_TEXT_X      (MEDIA_ART_X + MEDIA_ART_SIZE + 10)
#define MEDIA_TEXT_PAD_R  8
#define MEDIA_TITLE_Y     8
#define MEDIA_SUB_Y       29
#define MEDIA_DETAIL_Y    11
#define MEDIA_DETAIL_W    48
#define MEDIA_SCROLL_LEAD 1

#define MEDIA_SUB_SEL_COLOR  0xA0A0A0
#define MEDIA_SUB_DIM_COLOR  0x555555
#define MEDIA_DETAIL_COLOR   0x6A6A6A
#define MEDIA_ART_DARKEN_OPA LV_OPA_50

#define MEDIA_COUNT_BUF 24

static lv_obj_t *s_screen = NULL;
static lv_obj_t *s_list = NULL;
static lv_obj_t *s_titles[MEDIA_LIST_MAX_ITEMS];
static lv_obj_t *s_subs[MEDIA_LIST_MAX_ITEMS];
static lv_obj_t *s_bars[MEDIA_LIST_MAX_ITEMS];
static media_category_t s_category = MEDIA_CATEGORY_MUSIC;
static size_t s_count = 0;
static size_t s_sel = 0;

static void media_event_cb(lv_event_t *e);

void ui_media_list_set_category(media_category_t category) {
  if ((int)category < 0 || category >= MEDIA_CATEGORY_COUNT)
    return;
  s_category = category;
}

static int32_t view_height(void) {
  return ui_screen_h() - MEDIA_HEADER_HEIGHT - MEDIA_TITLE_H;
}

// Keep the selected row one row below the top, but never scroll past the last row.
static int32_t scroll_target(size_t sel) {
  int32_t content = (int32_t)s_count * MEDIA_ROW_H;
  int32_t max_scroll = content > view_height() ? content - view_height() : 0;
  int32_t lead = (int32_t)sel - MEDIA_SCROLL_LEAD;
  int32_t target = lead > 0 ? lead * MEDIA_ROW_H : 0;
  return target > max_scroll ? max_scroll : target;
}

static void media_refresh(bool animate) {
  for (size_t i = 0; i < s_count; i++) {
    bool is_sel = (i == s_sel);
    lv_obj_set_style_text_color(
        s_titles[i], lv_color_hex(is_sel ? ZUNE_SEL_COLOR : ZUNE_NEAR_COLOR), 0);
    lv_obj_set_style_text_color(
        s_subs[i], lv_color_hex(is_sel ? MEDIA_SUB_SEL_COLOR : MEDIA_SUB_DIM_COLOR), 0);
    lv_obj_set_style_bg_opa(s_bars[i], is_sel ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
  }
  lv_obj_scroll_to_y(s_list, scroll_target(s_sel), animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

static void media_move(int step) {
  if (step < 0 && s_sel == 0)
    return;
  if (step > 0 && s_sel + 1 >= s_count)
    return;
  s_sel = (size_t)((int)s_sel + step);
  media_refresh(true);
}

// One-line label: the fixed height is what makes LV_LABEL_LONG_DOT truncate instead of wrap.
static lv_obj_t *make_label(lv_obj_t *parent,
                            const char *text,
                            const lv_font_t *font,
                            int32_t w,
                            lv_label_long_mode_t mode) {
  lv_obj_t *l = lv_label_create(parent);
  lv_label_set_text(l, text);
  lv_label_set_long_mode(l, mode);
  lv_obj_set_size(l, w, lv_font_get_line_height(font));
  lv_obj_set_style_text_font(l, font, 0);
  return l;
}

static void build_row(lv_obj_t *list, size_t idx, const media_item_t *item) {
  int32_t text_w = ui_screen_w() - MEDIA_TEXT_X - MEDIA_TEXT_PAD_R;

  lv_obj_t *row = lv_obj_create(list);
  lv_obj_set_size(row, LV_PCT(100), MEDIA_ROW_H);
  lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  lv_obj_set_style_radius(row, 0, 0);
  lv_obj_set_style_pad_all(row, 0, 0);

  lv_obj_t *bar = lv_obj_create(row);
  lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(bar, MEDIA_BAR_W, MEDIA_BAR_H);
  lv_obj_align(bar, LV_ALIGN_LEFT_MID, MEDIA_BAR_X, 0);
  lv_obj_set_style_bg_color(bar, lv_color_hex(ZUNE_ACCENT_COLOR), 0);
  lv_obj_set_style_bg_opa(bar, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(bar, 0, 0);
  lv_obj_set_style_radius(bar, 0, 0);
  lv_obj_add_style(bar, zune_style_fade(), 0);
  s_bars[idx] = bar;

  lv_color_t art = lv_color_hex(item->art_color);
  lv_obj_t *tile = lv_obj_create(row);
  lv_obj_remove_flag(tile, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_size(tile, MEDIA_ART_SIZE, MEDIA_ART_SIZE);
  lv_obj_align(tile, LV_ALIGN_LEFT_MID, MEDIA_ART_X, 0);
  lv_obj_set_style_radius(tile, MEDIA_ART_RADIUS, 0);
  lv_obj_set_style_border_width(tile, 0, 0);
  lv_obj_set_style_pad_all(tile, 0, 0);
  lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, 0);
  lv_obj_set_style_bg_color(tile, art, 0);
  lv_obj_set_style_bg_grad_color(tile, lv_color_darken(art, MEDIA_ART_DARKEN_OPA), 0);
  lv_obj_set_style_bg_grad_dir(tile, LV_GRAD_DIR_VER, 0);
  lv_obj_t *glyph = lv_label_create(tile);
  lv_label_set_text(glyph, media_catalog_glyph(s_category));
  lv_obj_set_style_text_color(glyph, lv_color_white(), 0);
  lv_obj_set_style_text_font(glyph, &lv_font_montserrat_16, 0);
  lv_obj_center(glyph);

  lv_obj_t *title = make_label(
      row, item->title, &lv_font_montserrat_16, text_w - MEDIA_DETAIL_W, LV_LABEL_LONG_DOT);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, MEDIA_TEXT_X, MEDIA_TITLE_Y);
  lv_obj_add_style(title, zune_style_fade(), 0);
  s_titles[idx] = title;

  lv_obj_t *sub =
      make_label(row, item->subtitle, &lv_font_montserrat_12, text_w, LV_LABEL_LONG_DOT);
  lv_obj_align(sub, LV_ALIGN_TOP_LEFT, MEDIA_TEXT_X, MEDIA_SUB_Y);
  lv_obj_add_style(sub, zune_style_fade(), 0);
  s_subs[idx] = sub;

  lv_obj_t *detail =
      make_label(row, item->detail, &lv_font_montserrat_12, MEDIA_DETAIL_W, LV_LABEL_LONG_CLIP);
  lv_obj_set_style_text_align(detail, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_set_style_text_color(detail, lv_color_hex(MEDIA_DETAIL_COLOR), 0);
  lv_obj_align(detail, LV_ALIGN_TOP_RIGHT, -MEDIA_TEXT_PAD_R, MEDIA_DETAIL_Y);
}

void ui_media_list_open(void) {
  s_count = media_catalog_count(s_category);
  if (s_count > MEDIA_LIST_MAX_ITEMS)
    s_count = MEDIA_LIST_MAX_ITEMS;
  s_sel = 0;
  ESP_LOGI(TAG, "open: %s (%u items)", media_catalog_title(s_category), (unsigned)s_count);

  if (s_screen != NULL) {
    lv_obj_del(s_screen);
    s_screen = NULL;
  }

  s_screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(s_screen, lv_color_hex(ZUNE_BG_COLOR), 0);
  lv_obj_set_style_bg_opa(s_screen, LV_OPA_COVER, 0);
  lv_obj_remove_flag(s_screen, LV_OBJ_FLAG_SCROLLABLE);

  int32_t header_h = MEDIA_HEADER_HEIGHT;

  lv_obj_t *title = lv_label_create(s_screen);
  lv_label_set_text(title, media_catalog_title(s_category));
  lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(ZUNE_SEL_COLOR), 0);
  lv_obj_align(title, LV_ALIGN_TOP_LEFT, MEDIA_TITLE_PAD_L, header_h);

  char count_text[MEDIA_COUNT_BUF];
  lv_snprintf(count_text, sizeof(count_text), "%u items", (unsigned)s_count);
  lv_obj_t *count = lv_label_create(s_screen);
  lv_label_set_text(count, count_text);
  lv_obj_set_style_text_font(count, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(count, lv_color_hex(ZUNE_FAR_COLOR), 0);
  lv_obj_align(count, LV_ALIGN_TOP_RIGHT, -MEDIA_COUNT_PAD_R, header_h + 18);

  s_list = lv_obj_create(s_screen);
  lv_obj_set_size(s_list, ui_screen_w(), view_height());
  lv_obj_align(s_list, LV_ALIGN_TOP_MID, 0, header_h + MEDIA_TITLE_H);
  lv_obj_remove_flag(s_list, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_scrollbar_mode(s_list, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_scroll_dir(s_list, LV_DIR_VER);
  lv_obj_set_style_bg_opa(s_list, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(s_list, 0, 0);
  lv_obj_set_style_radius(s_list, 0, 0);
  lv_obj_set_style_pad_all(s_list, 0, 0);
  lv_obj_set_style_pad_row(s_list, 0, 0);
  lv_obj_set_flex_flow(s_list, LV_FLEX_FLOW_COLUMN);

  for (size_t i = 0; i < s_count; i++)
    build_row(s_list, i, media_catalog_item(s_category, i));

  // Header last so its status cluster draws over the list; black to match the hub.
  header_ui_create(s_screen);
  lv_obj_t *header = lv_obj_get_child(s_screen, -1);
  lv_obj_set_style_bg_color(header, lv_color_hex(ZUNE_BG_COLOR), 0);

  lv_obj_update_layout(s_screen);
  if (s_count > 0)
    media_refresh(false);

  lv_obj_add_event_cb(s_screen, media_event_cb, LV_EVENT_KEY, NULL);

  if (main_group != NULL) {
    lv_group_add_obj(main_group, s_screen);
    lv_group_focus_obj(s_screen);
  }

  ui_screen_load_owned(&s_screen, s_screen);
}

static void media_event_cb(lv_event_t *e) {
  if (dropdown_ui_is_open())
    return;

  if (ui_input_is_locked())
    return;

  if (lv_event_get_code(e) != LV_EVENT_KEY)
    return;

  switch (lv_event_get_key(e)) {
    case LV_KEY_UP:
    case LV_KEY_LEFT:
      media_move(-1);
      break;
    case LV_KEY_DOWN:
    case LV_KEY_RIGHT:
      media_move(1);
      break;
    case LV_KEY_ESC:
      ui_switch_screen(SCREEN_HOME);
      break;
    default:
      break;
  }
}
