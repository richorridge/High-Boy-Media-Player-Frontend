// Scripted test run for the Fake Zune boot screen and home hub.
//
// Usage: fakezune_sim <frame-output-dir>
// Writes boot_*.ppm / home_*.ppm frames there and exits non-zero on any failed check.

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "lvgl.h"

#include "boot_ui.h"
#include "home_ui.h"
#include "ui_manager.h"

#include "harness.h"

#define PANEL_W 240
#define PANEL_H 320
#define STEP_MS 5

// Logical screen size; swapped when run with FZ_LANDSCAPE=1 (the firmware's rotated mode).
static int SCREEN_W = PANEL_W;
static int SCREEN_H = PANEL_H;

extern lv_group_t *main_group;
extern screen_id_t current_screen_id;

static uint32_t s_now_ms = 0;
static uint16_t s_fb[PANEL_W * PANEL_H];
static const char *s_out_dir = ".";

static int s_failures = 0;
static int s_checks = 0;
static char s_last_open[64] = "";
static int s_feedback_count = 0;

static uint32_t tick_cb(void) {
  return s_now_ms;
}

void harness_note_open(const char *name) {
  snprintf(s_last_open, sizeof(s_last_open), "%s", name);
}

void harness_note_feedback(int kind) {
  (void)kind;
  s_feedback_count++;
}

#define CHECK(cond, ...)                                  \
  do {                                                    \
    s_checks++;                                           \
    if (!(cond)) {                                        \
      s_failures++;                                       \
      printf("  FAIL %s:%d: ", __FILE__, __LINE__);       \
      printf(__VA_ARGS__);                                \
      printf("\n");                                       \
    }                                                     \
  } while (0)

// ── Display + keypad ─────────────────────────────────────────────────────────

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  lv_display_flush_ready(disp);
}

static uint32_t s_pending_key = 0;
static int s_key_phase = 0;

static void keypad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  static uint32_t last_key = 0;
  if (s_key_phase == 1) {
    last_key = s_pending_key;
    data->key = last_key;
    data->state = LV_INDEV_STATE_PRESSED;
    s_key_phase = 2;
  } else {
    data->key = last_key;
    data->state = LV_INDEV_STATE_RELEASED;
    s_key_phase = 0;
  }
}

static void advance(uint32_t ms) {
  for (uint32_t t = 0; t < ms; t += STEP_MS) {
    s_now_ms += STEP_MS;
    lv_timer_handler();
  }
}

static void press(uint32_t key) {
  s_pending_key = key;
  s_key_phase = 1;
  advance(60);
}

static void dump_frame(const char *name) {
  advance(STEP_MS);
  lv_refr_now(NULL);
  char path[512];
  snprintf(path, sizeof(path), "%s/%s.ppm", s_out_dir, name);
  FILE *f = fopen(path, "wb");
  if (f == NULL) {
    printf("  cannot write %s: %s\n", path, strerror(errno));
    return;
  }
  fprintf(f, "P6\n%d %d\n255\n", SCREEN_W, SCREEN_H);
  for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
    uint16_t p = s_fb[i];
    uint8_t rgb[3] = {(uint8_t)(((p >> 11) & 0x1F) * 255 / 31),
                      (uint8_t)(((p >> 5) & 0x3F) * 255 / 63),
                      (uint8_t)((p & 0x1F) * 255 / 31)};
    fwrite(rgb, 1, 3, f);
  }
  fclose(f);
}

// ── Widget lookup ────────────────────────────────────────────────────────────

static lv_obj_t *find_label(lv_obj_t *root, const char *text) {
  uint32_t n = lv_obj_get_child_count(root);
  for (uint32_t i = 0; i < n; i++) {
    lv_obj_t *c = lv_obj_get_child(root, (int32_t)i);
    if (lv_obj_check_type(c, &lv_label_class) && strcmp(lv_label_get_text(c), text) == 0)
      return c;
    lv_obj_t *hit = find_label(c, text);
    if (hit != NULL)
      return hit;
  }
  return NULL;
}

static int count_labels(lv_obj_t *root) {
  int count = 0;
  uint32_t n = lv_obj_get_child_count(root);
  for (uint32_t i = 0; i < n; i++) {
    lv_obj_t *c = lv_obj_get_child(root, (int32_t)i);
    if (lv_obj_check_type(c, &lv_label_class))
      count++;
    count += count_labels(c);
  }
  return count;
}

static const char *HUB_NAMES[] = {"music", "videos", "pictures", "settings", "high boy"};
#define HUB_COUNT 5

// The selected hub entry is the one drawn in the selection colour (white).
static int hub_selected(void) {
  int found = -1;
  for (int i = 0; i < HUB_COUNT; i++) {
    lv_obj_t *l = find_label(lv_screen_active(), HUB_NAMES[i]);
    if (l == NULL)
      return -2;
    lv_color_t c = lv_obj_get_style_text_color(l, 0);
    if (lv_color_eq(c, lv_color_hex(0xFFFFFF))) {
      if (found != -1)
        return -3;
      found = i;
    }
  }
  return found;
}

static int32_t hub_label_bottom(int idx) {
  lv_obj_t *l = find_label(lv_screen_active(), HUB_NAMES[idx]);
  lv_area_t a;
  lv_obj_get_coords(l, &a);
  return a.y2;
}

// ── Tests ────────────────────────────────────────────────────────────────────

static void test_boot(void) {
  printf("boot screen\n");
  ui_boot_show();
  advance(100);
  dump_frame("boot_0_start");

  lv_obj_t *scr = lv_screen_active();
  CHECK(find_label(scr, "Loading Media") != NULL, "boot screen must say 'Loading Media'");
  CHECK(count_labels(scr) == 1, "boot screen must show only one label, found %d", count_labels(scr));

  lv_obj_t *label = find_label(scr, "Loading Media");
  lv_area_t a;
  lv_obj_get_coords(label, &a);
  CHECK(a.x1 >= 0 && a.x2 < SCREEN_W, "label overflows the screen: x %d..%d", a.x1, a.x2);
  CHECK(a.y1 >= SCREEN_H / 3 && a.y2 <= SCREEN_H * 2 / 3, "label is not centred: y %d..%d", a.y1, a.y2);

  advance(700);
  dump_frame("boot_1_faded_in");
  advance(1100);
  dump_frame("boot_2_pulse_dim");
  advance(1100);
  dump_frame("boot_3_pulse_bright");
  CHECK(find_label(lv_screen_active(), "Loading Media") != NULL, "label survives the pulse loop");
}

static void open_hub(void) {
  ui_home_open();
  current_screen_id = SCREEN_HOME;
  advance(600);
}

static void test_hub(void) {
  printf("home hub\n");
  open_hub();
  dump_frame("home_0_music");

  lv_obj_t *scr = lv_screen_active();
  for (int i = 0; i < HUB_COUNT; i++)
    CHECK(find_label(scr, HUB_NAMES[i]) != NULL, "hub entry '%s' missing", HUB_NAMES[i]);
  CHECK(hub_selected() == 0, "music should start selected, got %d", hub_selected());

  // Every original High Boy tool must be behind "high boy": nothing else on the hub.
  CHECK(find_label(scr, "WIFI") == NULL && find_label(scr, "NFC") == NULL, "old menu items leaked onto the hub");
  CHECK(hub_label_bottom(4) > SCREEN_H, "'high boy' should start below the fold (bottom=%d)", hub_label_bottom(4));

  const char *frames[] = {"home_0_music", "home_1_videos", "home_2_pictures", "home_3_settings", "home_4_high_boy"};
  for (int i = 1; i < HUB_COUNT; i++) {
    press(LV_KEY_DOWN);
    advance(450);
    CHECK(hub_selected() == i, "after %d DOWN presses selected=%d", i, hub_selected());
    dump_frame(frames[i]);
  }
  CHECK(hub_label_bottom(4) <= SCREEN_H, "scrolling down should reveal 'high boy' (bottom=%d)", hub_label_bottom(4));

  press(LV_KEY_DOWN);
  advance(450);
  CHECK(hub_selected() == 4, "DOWN past the end must stay on the last entry, got %d", hub_selected());

  for (int i = 3; i >= 0; i--) {
    press(LV_KEY_UP);
    advance(450);
    CHECK(hub_selected() == i, "UP back to %d, got %d", i, hub_selected());
  }
  dump_frame("home_back_at_top");
  CHECK(hub_label_bottom(4) > SCREEN_H, "'high boy' should be hidden again at the top (bottom=%d)", hub_label_bottom(4));

  press(LV_KEY_UP);
  advance(450);
  CHECK(hub_selected() == 0, "UP past the start must stay on music, got %d", hub_selected());

  int fb = s_feedback_count;
  press(LV_KEY_DOWN);
  advance(450);
  CHECK(s_feedback_count > fb, "navigation should give UI feedback");
  press(LV_KEY_UP);
  advance(450);
}

static void select_and_open(int index) {
  // Bounded: if the hub is not on screen hub_selected() is negative and would never converge.
  for (int guard = 0; guard < HUB_COUNT && hub_selected() >= 0 && hub_selected() != index; guard++) {
    press(hub_selected() < index ? LV_KEY_DOWN : LV_KEY_UP);
    advance(450);
  }
  CHECK(hub_selected() == index, "could not select hub entry %d (selected=%d)", index, hub_selected());
  s_last_open[0] = '\0';
  press(LV_KEY_ENTER);
  advance(100);
}

static void test_launch_targets(void) {
  printf("launch targets\n");
  struct {
    int index;
    const char *expect;
  } cases[] = {{0, "MUSIC LIBRARY"}, {1, "FILES"}, {2, "FILES"}, {3, "SETTINGS"}, {4, "HIGH BOY MENU"}};

  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    open_hub();
    select_and_open(cases[i].index);
    CHECK(strcmp(s_last_open, cases[i].expect) == 0, "'%s' opened '%s', expected '%s'", HUB_NAMES[cases[i].index], s_last_open, cases[i].expect);
    if (cases[i].index == 4) {
      dump_frame("launch_high_boy");
    }
  }
}

static void test_back_navigation(void) {
  printf("back navigation\n");

  // A tool opened from the hub: its BACK (ui_switch_screen(SCREEN_MENU)) returns to the hub
  // with the same entry still selected.
  open_hub();
  select_and_open(3);
  CHECK(ui_current_screen() == SCREEN_SETTINGS, "settings should be current, got %d", ui_current_screen());
  advance(600);
  ui_switch_screen(SCREEN_MENU);
  CHECK(ui_current_screen() == SCREEN_HOME, "BACK from a hub-launched screen must land on the hub, got %d", ui_current_screen());
  advance(100);
  CHECK(hub_selected() == 3, "hub should remember 'settings', got %d", hub_selected());

  // Deeper navigation keeps the redirect: music -> (player) -> BACK -> library -> BACK -> hub.
  select_and_open(0);
  advance(600);
  ui_switch_screen(SCREEN_MP3_PLAYER);
  advance(600);
  ui_switch_screen(SCREEN_PLAYER);
  advance(600);
  ui_switch_screen(SCREEN_MENU);
  CHECK(ui_current_screen() == SCREEN_HOME, "BACK from the music library must land on the hub, got %d", ui_current_screen());

  // A tool opened from the high boy carousel still goes back to the carousel.
  advance(600);
  select_and_open(4);
  CHECK(ui_current_screen() == SCREEN_MENU, "high boy should open the carousel, got %d", ui_current_screen());
  advance(600);
  ui_switch_screen(SCREEN_FILES);
  advance(600);
  ui_switch_screen(SCREEN_MENU);
  CHECK(ui_current_screen() == SCREEN_MENU, "BACK from a carousel-launched screen must return to the carousel, got %d", ui_current_screen());

  // The carousel's own BACK goes home, and clears any stale redirect.
  advance(600);
  ui_switch_screen(SCREEN_HOME);
  CHECK(ui_current_screen() == SCREEN_HOME, "carousel BACK goes home");
  advance(600);
  ui_switch_screen(SCREEN_MENU);
  CHECK(ui_current_screen() == SCREEN_MENU, "plain SCREEN_MENU switch must not be redirected, got %d", ui_current_screen());
}

int main(int argc, char **argv) {
  if (argc > 1)
    s_out_dir = argv[1];
  if (mkdir(s_out_dir, 0755) != 0 && errno != EEXIST) {
    printf("cannot create %s: %s\n", s_out_dir, strerror(errno));
    return 2;
  }

  lv_init();
  lv_tick_set_cb(tick_cb);

  const char *landscape = getenv("FZ_LANDSCAPE");
  if (landscape != NULL && landscape[0] == '1') {
    SCREEN_W = PANEL_H;
    SCREEN_H = PANEL_W;
    printf("landscape %dx%d\n", SCREEN_W, SCREEN_H);
  }

  lv_display_t *disp = lv_display_create(SCREEN_W, SCREEN_H);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
  lv_display_set_buffers(disp, s_fb, NULL, sizeof(s_fb), LV_DISPLAY_RENDER_MODE_DIRECT);
  lv_display_set_flush_cb(disp, flush_cb);

  main_group = lv_group_create();
  lv_group_set_default(main_group);
  lv_indev_t *kp = lv_indev_create();
  lv_indev_set_type(kp, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(kp, keypad_read);
  lv_indev_set_group(kp, main_group);

  test_boot();
  test_hub();
  test_launch_targets();
  test_back_navigation();

  printf("\n%d checks, %d failed\n", s_checks, s_failures);
  return s_failures == 0 ? 0 : 1;
}
