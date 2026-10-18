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

#ifndef MEDIA_LIST_UI_H
#define MEDIA_LIST_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "media_catalog.h"

/** @brief Most rows a list can hold (rows beyond this are not shown). */
#define MEDIA_LIST_MAX_ITEMS 32

/**
 * @brief Choose which list ui_media_list_open() shows. Call BEFORE switching to
 *        SCREEN_MEDIA_LIST. Invalid values are ignored.
 */
void ui_media_list_set_category(media_category_t category);

/**
 * @brief Open the Zune-style media list: lowercase title, then rows of thumbnail,
 *        title, subtitle and duration/date. Up/Down moves, Back returns home.
 */
void ui_media_list_open(void);

#ifdef __cplusplus
}
#endif

#endif // MEDIA_LIST_UI_H
