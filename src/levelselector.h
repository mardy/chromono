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

#ifndef SHADYPOSTPROC_LEVELSELECTOR_H
#define SHADYPOSTPROC_LEVELSELECTOR_H

#include "shadypostproc.h"

#include "circle1d.h"
#include "levelmanager.h"
#include "scoremanager.h"
#include "opengl_renderer.h"
#include "page.h"

class Game;
class LevelSelectorPack;
class LevelSelectorGrid;

class LevelSelector : public Page {
    public:
        LevelSelector(Game *game);
        virtual ~LevelSelector();

        virtual void before_render(OpenGLRenderer *renderer);
        virtual void render(OpenGLRenderer *renderer);
        virtual void handle(Circle1DEvent *event);
        virtual void render_background(OpenGLRenderer *renderer);
        virtual bool on_back_button();

        void scroll_to(int level);
        void select_pack(LevelPack *pack, bool transition=true);

        void
        go_to_pack()
        {
            state = PACK;
        }

        void
        go_to_grid()
        {
            state = GRID;
        }

        bool is_grid() { return state == GRID; }

        int indicator_y() { return star_count_indicator_y; }

        bool current_pack_contains(int level)
        {
            SHADY_ASSERT(current_pack != NULL);
            return (level >= current_pack->first_level &&
                    level <= current_pack->last_level);
        }

        RGB current_pack_color()
        {
            return current_pack->layout.color;
        }

    private:
        LevelSelectorPack *pack;
        LevelSelectorGrid *grid;
        LevelPack *current_pack;

        int star_count_indicator_y;

        enum State {
            PACK = 0,
            GRID,
        } state;
};

#endif /* SHADYPOSTPROC_LEVELSELECTOR_H */
