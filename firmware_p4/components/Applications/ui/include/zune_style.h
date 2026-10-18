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

#ifndef ZUNE_STYLE_H
#define ZUNE_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/** @name Fake Zune palette shared by the home hub and the media lists. */
///@{
#define ZUNE_BG_COLOR     0x000000
#define ZUNE_ACCENT_COLOR 0xA23BEC
#define ZUNE_SEL_COLOR    0xFFFFFF
#define ZUNE_NEAR_COLOR   0x7A7A7A
#define ZUNE_FAR_COLOR    0x4D4D4D
///@}

/**
 * @brief Style that fades text colour and background opacity (220 ms) when they change.
 *
 * Add it to selectable labels and accent markers so selection changes cross-fade.
 * Created on first use; the returned style lives for the life of the program.
 */
const lv_style_t *zune_style_fade(void);

#ifdef __cplusplus
}
#endif

#endif // ZUNE_STYLE_H
