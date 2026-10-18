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

#include "media_catalog.h"

#include "lvgl.h"

static const media_item_t s_music[] = {
    {"Glass Tides", "Neon Harbor - Low Season", "3:42", 0x3A6EA5},
    {"Paper Satellites", "The Quiet Engines - Orbit Notes", "4:05", 0x7A4FA3},
    {"Midnight Arcade", "Vera Lund - Pixel Heart", "3:18", 0xC2457A},
    {"Slow Lightning", "Coast Static - Weather Maps", "5:01", 0x2F8F83},
    {"Violet Hour", "Neon Harbor - Low Season", "3:27", 0x3A6EA5},
    {"Cassette Summer", "Mara & The Moths - Tape Hiss", "2:56", 0xD08A2E},
    {"Overpass", "Coast Static - Weather Maps", "4:12", 0x2F8F83},
    {"Honey Circuit", "Vera Lund - Pixel Heart", "3:49", 0xC2457A},
    {"Lanterns", "The Quiet Engines - Orbit Notes", "4:33", 0x7A4FA3},
    {"Static Bloom", "Mara & The Moths - Tape Hiss", "3:08", 0xD08A2E},
    {"Night Bus Home", "Neon Harbor - After Hours", "3:51", 0x44507A},
    {"Parallel Sun", "Juno Park - Parallel Sun", "4:20", 0xB5523B},
};

static const media_item_t s_videos[] = {
    {"Signals from the Deep", "Documentary - 2022", "52 min", 0x1F5F8B},
    {"Cardboard Kingdom", "Animation - 2020", "24 min", 0xD08A2E},
    {"Last Train to Elmwood", "Drama - 2019", "1h 38m", 0x6B4A3A},
    {"The Soldering Club", "Comedy - 2023", "22 min", 0x3E8E5A},
    {"Moonlit Heist", "Thriller - 2021", "1h 52m", 0x44507A},
    {"Slow Mountain", "Nature - 2022", "44 min", 0x2F8F83},
    {"Retro Reboot", "Documentary - 2018", "57 min", 0xB5523B},
    {"Pocket Universe", "Sci-fi - 2024", "1h 21m", 0x7A4FA3},
};

static const media_item_t s_pictures[] = {
    {"Pier at Dusk", "Brighton Hollow", "12 Aug", 0xC2693A},
    {"Market Morning", "Old Town", "03 Sep", 0xD0A02E},
    {"Rooftop Garden", "Home", "21 Jun", 0x3E8E5A},
    {"Snow Day", "Fell Road", "08 Jan", 0x6E8FB5},
    {"Lighthouse", "Cape Wren", "30 Jul", 0x3A6EA5},
    {"Birthday Cake", "Home", "17 May", 0xC2457A},
    {"Desert Road", "Salt Flats", "02 Apr", 0xB5523B},
    {"City Lights", "Harbour Bridge", "25 Nov", 0x44507A},
    {"Harbour Cat", "Brighton Hollow", "13 Aug", 0x7A4FA3},
    {"Trail Marker", "Ridge Path", "09 Oct", 0x2F8F83},
};

typedef struct {
  const char *title;
  const char *glyph;
  const media_item_t *items;
  size_t count;
} media_table_t;

#define TABLE_SIZE(table) (sizeof(table) / sizeof((table)[0]))

static const media_table_t s_tables[MEDIA_CATEGORY_COUNT] = {
    [MEDIA_CATEGORY_MUSIC] = {"music", LV_SYMBOL_AUDIO, s_music, TABLE_SIZE(s_music)},
    [MEDIA_CATEGORY_VIDEOS] = {"videos", LV_SYMBOL_VIDEO, s_videos, TABLE_SIZE(s_videos)},
    [MEDIA_CATEGORY_PICTURES] = {"pictures", LV_SYMBOL_IMAGE, s_pictures, TABLE_SIZE(s_pictures)},
};

static const media_table_t *table_for(media_category_t category) {
  if ((int)category < 0 || category >= MEDIA_CATEGORY_COUNT)
    return NULL;
  return &s_tables[category];
}

const char *media_catalog_title(media_category_t category) {
  const media_table_t *t = table_for(category);
  return t != NULL ? t->title : NULL;
}

const char *media_catalog_glyph(media_category_t category) {
  const media_table_t *t = table_for(category);
  return t != NULL ? t->glyph : NULL;
}

size_t media_catalog_count(media_category_t category) {
  const media_table_t *t = table_for(category);
  return t != NULL ? t->count : 0;
}

const media_item_t *media_catalog_item(media_category_t category, size_t index) {
  const media_table_t *t = table_for(category);
  if (t == NULL || index >= t->count)
    return NULL;
  return &t->items[index];
}
