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

#ifndef SHADYPOSTPROC_LEVELSELECTORPACK_H
#define SHADYPOSTPROC_LEVELSELECTORPACK_H

#include "shadypostproc.h"

#include "circle1d.h"

#include "levelselector.h"

#include "game.h"
#include "constants.h"
#include "colors.h"

#define MAX_LEVEL_PACKS 10

class PressablePack;

class LevelSelectorPack {
    public:

    LevelSelectorPack(Game *game, level_pack_select_func select_func,
            void *select_func_user_data);
    ~LevelSelectorPack();

    void before_render(OpenGLRenderer *renderer);
    void render(OpenGLRenderer *renderer);
    void handle(Circle1DEvent *event);

    LevelPack *find_pack(int level);

    bool select_pack(int pack_id);

    Game *game;
    level_pack_select_func select_func;
    void *select_func_user_data;
    LevelManager *level_manager;
    ScoreManager *score_manager;
    Scene scene;

    // State variables
    const char *title;

    Object *level_pack_object[MAX_LEVEL_PACKS];
    PressablePack *pressable_pack[MAX_LEVEL_PACKS];
    int packs_count;

    // Array of ints for each pack, 0 means not yet instantiated and 1 means
    // instantiated (to additionally add behaviors when packs are unlocked
    // during gameplay)
    int *instantiated_behaviors;
};

#endif /* SHADYPOSTPROC_LEVELSELECTORGRID_H */
