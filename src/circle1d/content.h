/**
 * chro.mono: A very circular color puzzle game
 * https://thp.io/2013/chromono/
 * Copyright (C) 2013-2021 Thomas Perl <m@thp.io>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **/

#ifndef CARPHONE_CONTENT_H
#define CARPHONE_CONTENT_H

#include "shadypostproc.h"

#include "model.h"

typedef void (*level_func)(Scene*);

struct LevelInfo {
    long stars_time[3]; // time goal for achieving stars
    int required_stars; // stars required to unlock this level
    level_func constructor; // constructor for this level
};

typedef void (*level_pack_behavior_func)(Object *o);

struct LevelPack {
    const char *name; // name of the level pack
    int first_level; // first level in this level pack
    int last_level; // last level in this level pack
    level_pack_behavior_func behavior_func; // animation in menu

    struct {
        int x;
        int y;
        int size;
        RGB color;
        bool connected_to_previous;
    } layout;
};

typedef void (*level_pack_select_func)(LevelPack *pack, void *user_data);

extern LevelInfo levels[];
extern LevelPack packs[];

#endif /* CARPHONE_CONTENT_H */
