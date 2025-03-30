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


#include "levelselector.h"

#include "levelselectorgrid.h"
#include "levelselectorpack.h"

static void
on_level_pack_selected(LevelPack *pack, void *user_data)
{
    LevelSelector *selector = (LevelSelector*)user_data;
    selector->select_pack(pack);
}

static void
set_level_selector_state_pack(Game *game)
{
    LevelSelector *selector = game->get_level_selector();
    selector->go_to_pack();
}

static void
set_level_selector_state_grid(Game *game)
{
    LevelSelector *selector = game->get_level_selector();
    selector->go_to_grid();
}

LevelSelector::LevelSelector(Game *game)
    : Page(game, Icons::BACK)
    , pack(new LevelSelectorPack(game, on_level_pack_selected, this))
    , grid(new LevelSelectorGrid(game))
    , current_pack(&(packs[0]))
    , star_count_indicator_y(0)
    , state(PACK)
{
}

LevelSelector::~LevelSelector()
{
    delete grid;
}

void
LevelSelector::scroll_to(int level)
{
    if ((current_pack->first_level < level) ||
        (current_pack->last_level > level)) {
        LevelPack *new_pack = pack->find_pack(level);
        if (new_pack != NULL) {
            select_pack(new_pack, false);
        }
    }

    grid->scroll_to(level);
}

void
LevelSelector::select_pack(LevelPack *pack, bool transition)
{
    current_pack = pack;
    grid->set_range(pack->first_level, pack->last_level);
    grid->set_title(pack->name);
    if (transition) {
        game->transition_to(this, set_level_selector_state_grid);
    }
}

void
LevelSelector::render(OpenGLRenderer *renderer)
{
    /* Stars count */
    int have_stars = game->get_score_manager()->stars();

    char tmp[1024];
    sprintf(tmp, "%d", have_stars);

    int star_w, star_h;
    star_w = star_h = 30;

    float text_w, text_h;
    renderer->text_measure(tmp, &text_w, &text_h, FONT_MEDIUM);

    float line_h = (text_h > star_h) ? text_h : star_h;

    Vec2 offset = game->get_offset();

    float margin = 20.0;
    float x = margin - offset.x;
    star_count_indicator_y = Constants::WORLD_HEIGHT + offset.y - margin - line_h / 2.0;

    if (state == PACK) {
        pack->render(renderer);
    } else if (state == GRID) {
        grid->render(renderer);
    }

    renderer->icon(Icons::STAR,
            x, star_count_indicator_y - star_h / 2,
            star_w, star_h, Colors::STAR, 0.9);

    x += star_w;
    renderer->text_render(tmp, x,
            star_count_indicator_y - text_h / 2.0, FONT_MEDIUM);
}

void
LevelSelector::handle(Circle1DEvent *event)
{
    if (event->finger != 0) {
        /* No multi-touch support in level selector */
        return;
    }

    if (state == PACK) {
        pack->handle(event);
    } else if (state == GRID) {
        grid->handle(event);
    }
}

void
LevelSelector::render_background(OpenGLRenderer *renderer)
{
    if (state == GRID) {
        renderer->background(RGB::mix(current_pack_color(), RGB(0.0, 0.0, 0.0)));
    } else {
        renderer->background(Colors::SELECTOR_BACKGROUND_COLOR);
    }
}

bool
LevelSelector::on_back_button()
{
    if (state == GRID) {
        game->transition_to(this, set_level_selector_state_pack);
        return true;
    }

    game->to_menu();
    return true;
}

void
LevelSelector::before_render(OpenGLRenderer *renderer)
{
    if (state == GRID) {
        grid->prepare_pages(renderer);
    } else {
        pack->before_render(renderer);
    }
}

