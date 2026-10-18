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

#ifndef MEDIA_CATALOG_H
#define MEDIA_CATALOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/** @brief Kinds of media the home hub offers. */
typedef enum {
  MEDIA_CATEGORY_MUSIC,
  MEDIA_CATEGORY_VIDEOS,
  MEDIA_CATEGORY_PICTURES,
  MEDIA_CATEGORY_COUNT
} media_category_t;

/** @brief One row of a media list. All strings are static. */
typedef struct {
  const char *title;
  const char *subtitle; ///< Artist / album, genre / year, or place.
  const char *detail;   ///< Duration or date, shown at the right edge.
  uint32_t art_color;   ///< Base colour of the thumbnail tile (0xRRGGBB).
} media_item_t;

/**
 * @brief Demo media shown by the Fake Zune lists.
 *
 * The catalog is invented placeholder data: nothing on the SD card is read and the
 * items cannot be played. Replace the tables in media_catalog.c to change what is shown.
 */

/** @brief Lowercase list title, e.g. "music". NULL for an invalid category. */
const char *media_catalog_title(media_category_t category);

/** @brief Glyph (an LV_SYMBOL_* string) drawn on each thumbnail tile. */
const char *media_catalog_glyph(media_category_t category);

/** @brief Number of items in the category; 0 for an invalid category. */
size_t media_catalog_count(media_category_t category);

/** @brief Item at @p index, or NULL when out of range. */
const media_item_t *media_catalog_item(media_category_t category, size_t index);

#ifdef __cplusplus
}
#endif

#endif // MEDIA_CATALOG_H
