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

#ifndef SHADYPOSTPROC_LEVELSELECTORGRID_H
#define SHADYPOSTPROC_LEVELSELECTORGRID_H

#include "shadypostproc.h"

#include "levelselector.h"

#include "game.h"
#include "constants.h"
#include "colors.h"

struct LevelPreviewInfo {
    int level;
    bool locked;
    int required_stars;
    int stars;
};

#define MAX_PREVIEWS_PER_PAGE 6

struct PagePreviewInfo {
    int page;
    LevelPreviewInfo previews[MAX_PREVIEWS_PER_PAGE];
};

class LevelSelectorGrid {
    public:

    LevelSelectorGrid(Game *game)
        : game(game)
        , level_manager(game->get_level_manager())
        , score_manager(game->get_score_manager())
        , current_page(0)
        , title("Select a Level")
        , size(170, 170)
        , spacing(10, 10)
        , columns(3)
        , first_level(0)
        , last_level(0)
        , top_left(0, 0)
        , rows(0)
        , pages(0)
        , mousedown_pos(0, 0)
        , scrolling(false)
        , pressed(false)
        , scroll_offset_x(0)
    {
        Vec2 available = Vec2(Constants::WORLD_WIDTH, Constants::WORLD_HEIGHT);

        //columns = (available.x - spacing.x) / (spacing.x + size.x);

        top_left.x = (available.x - (columns*(spacing.x+size.x) + spacing.x)) / 2.0;
        rows = (available.y - spacing.y) / (spacing.y + size.y);
        top_left.y = (available.y - (rows*(spacing.y+size.y) + spacing.y)) / 2.0;

        for (int i=0; i<3; i++) {
            preview_info[i].page = -1;
            SHADY_ASSERT(ARRAY_LENGTH(preview_info[i].previews) >= (size_t)per_page());
            memset(&(preview_info[i].previews), 0, sizeof(preview_info[i].previews));
        }

        // At first, show all levels
        set_range(0, level_manager->count());
    }

    void set_title(const char *title)
    {
        this->title = title;
    }

    ~LevelSelectorGrid()
    {
    }

    void set_range(int first_level, int last_level)
    {
        this->first_level = first_level;
        this->last_level = last_level;

        pages = ((last_level - first_level) + per_page() - 1) / per_page();
        current_page = 0;
    }

    void prepare_pages(OpenGLRenderer *renderer);

    void render_page(OpenGLRenderer *renderer, int offset);
    void render_page_internal(OpenGLRenderer *renderer, PagePreviewInfo *info);

    void render(OpenGLRenderer *renderer);
    void handle(Circle1DEvent *event);

    void scroll_to(int level);

    int per_page() { return columns * rows; }

    int first_of(int page) { return first_level + page * per_page(); }
    int last_of(int page) { return first_of(page+1) - 1; }

    int first() { return first_of(current_page); }
    int last() { return last_of(current_page); }

    bool previous_page() {
        if (current_page > 0) {
            current_page--;
            return true;
        }
        return false;
    }

    bool next_page() {
        if (current_page < pages-1) {
            current_page++;
            return true;
        }
        return false;
    }

    int
    trace(int x, int y)
    {
        Vec2 pos(x, y);
        pos = pos - top_left;
        if (pos.x < spacing.x || pos.y < spacing.y) {
            // clicked outside (top/left)
            if (previous_page()) {
                scroll_offset_x -= Constants::WORLD_WIDTH;
            } else {
                // Wrap over
                scroll_offset_x += Constants::WORLD_WIDTH;
                current_page = pages - 1;
            }
            return -1;
        }

        int column = pos.x / (spacing.x + size.x);
        int row = pos.y / (spacing.y + size.y);
        if (column >= columns || row >= rows) {
            // clicked outside (right/bottom)
            if (next_page()) {
                scroll_offset_x += Constants::WORLD_WIDTH;
            } else {
                // Wrap over
                scroll_offset_x -= Constants::WORLD_WIDTH;
                current_page = 0;
            }
            return -1;
        }

        if ((fmod(pos.x, (spacing.x + size.x)) < spacing.x) ||
            (fmod(pos.y, (spacing.y + size.y)) < spacing.y)) {
            // clicked inside spacing
            return -1;
        }

        return first() + column + columns * row;
    }

    Vec2
    position(int index)
    {
        int column = index % columns;
        int row = index / columns;

        return top_left + Vec2(spacing.x + (size.x + spacing.x) * column,
                spacing.y + (size.y + spacing.y) * row);
    }

    Game *game;
    LevelManager *level_manager;
    ScoreManager *score_manager;

    // State variables
    int current_page;
    const char *title;

    // Parameters that can be configured
    Vec2 size;
    Vec2 spacing;
    int columns;

    // Subset of levels to be displayed
    int first_level;
    int last_level;

    // Calculate from parameters above
    Vec2 top_left;
    int rows;
    int pages;

    // Offset for scrolling
    Vec2 mousedown_pos;
    bool scrolling;
    bool pressed;
    int scroll_offset_x;

    // Level Preview Info
    PagePreviewInfo preview_info[3 /* left, current, right */];
};

#endif /* SHADYPOSTPROC_LEVELSELECTORGRID_H */
