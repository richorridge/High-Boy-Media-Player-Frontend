// Stubs for everything the Fake Zune screens reach outside LVGL: the hardware
// services, the shared header bar, and the screens the hub opens.
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "lvgl.h"

#include "dropdown_ui.h"
#include "header_ui.h"
#include "lvgl_glue.h"
#include "menu_ui.h"
#include "ui_chrome.h"
#include "ui_feedback.h"
#include "ui_manager.h"
#include "wav_library_ui.h"

#include "harness.h"

lv_group_t *main_group = NULL;

// ── Screen opens: each just paints a labelled black screen ───────────────────

static lv_obj_t *stub_screen(const char *name) {
  lv_obj_t *scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x101820), 0);
  lv_obj_t *l = lv_label_create(scr);
  lv_label_set_text(l, name);
  lv_obj_set_style_text_color(l, lv_color_white(), 0);
  lv_obj_center(l);
  return scr;
}

static void open_stub(const char *name) {
  lv_obj_t *scr = stub_screen(name);
  lv_screen_load(scr);
  harness_note_open(name);
}

void ui_menu_open(void) { open_stub("HIGH BOY MENU"); }
void ui_wav_library_open(void) { open_stub("MUSIC LIBRARY"); }
void ui_files_open(void) { open_stub("FILES"); }
void ui_settings_open(void) { open_stub("SETTINGS"); }
void ui_mp3_player_open(void) { open_stub("MP3 PLAYER"); }

// ── Shared services ──────────────────────────────────────────────────────────

void header_ui_create(lv_obj_t *parent) {
  lv_obj_t *bar = lv_obj_create(parent);
  lv_obj_set_size(bar, lv_pct(100), 28);
  lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_bg_color(bar, lv_color_hex(0x202020), 0);
  lv_obj_set_style_border_width(bar, 0, 0);
  lv_obj_set_style_radius(bar, 0, 0);
  lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_t *t = lv_label_create(bar);
  lv_label_set_text(t, "12:34");
  lv_obj_set_style_text_color(t, lv_color_white(), 0);
  lv_obj_align(t, LV_ALIGN_LEFT_MID, 6, 0);
}

bool dropdown_ui_is_open(void) { return false; }
void ui_feedback(ui_feedback_kind_t kind) { harness_note_feedback(kind); }

bool lvgl_glue_lock(int timeout_ms) { return true; }
void lvgl_glue_unlock(void) {}

void ui_chrome_set_breadcrumb_root(const char *root) {}
void ui_chrome_set_status_enabled(bool enabled) {}
void screen_tips_hook(screen_id_t id) {}
bool screen_tips_active(void) { return false; }
void sx1262_stop_rx(void) {}
int sx1262_receive_continuous(void) { return 0; }
