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

#include "zune_style.h"

#define ZUNE_FADE_MS 220

static lv_style_t s_fade_style;
static lv_style_transition_dsc_t s_fade_trans;
static bool s_is_ready = false;

const lv_style_t *zune_style_fade(void) {
  if (!s_is_ready) {
    static const lv_style_prop_t props[] = {
        LV_STYLE_TEXT_COLOR, LV_STYLE_BG_OPA, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(
        &s_fade_trans, props, lv_anim_path_ease_out, ZUNE_FADE_MS, 0, NULL);
    lv_style_init(&s_fade_style);
    lv_style_set_transition(&s_fade_style, &s_fade_trans);
    s_is_ready = true;
  }
  return &s_fade_style;
}
