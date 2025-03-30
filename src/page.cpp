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


#include "page.h"

#include "game.h"

static void
game_back_button(Game *game)
{
    game->back_button();
}

/**
 * About the back_pressable position:
 *
 * Consider everything to the left and top of the back button
 * area to also be a feedback area for the back button (this
 * allows easier navigation by just pressing at the top left
 * corner of the screen) - especially important in selector
 *
 * for event handling:
 * pressable_pos = -game->get_offset()
 * pressable_size = (button margin + button size)
 *
 * for rendering:
 * render_pos = (pressable_pos + button margin)
 * render_size = button size
 **/

Page::Page(Game *game, enum Icons::Icon back)
    : game(game)
    , back(back)
    , back_pressable(game, game_back_button,
            -game->get_offset(),
            Constants::BACK_BUTTON_SIZE +
            Constants::BACK_BUTTON_MARGIN)
{
}

void
Page::handle_page(Circle1DEvent *event)
{
    if (back != Icons::NONE) {
        if (back_pressable.handle(event)) {
            return;
        }
    }

    handle(event);
}

void
Page::render_page(OpenGLRenderer *renderer)
{
    back_pressable.setPos(-game->get_offset());

    render_background(renderer);
    render(renderer);

    if (back != Icons::NONE) {
        Vec2 pos = back_pressable.pos() +
            Vec2(Constants::BACK_BUTTON_MARGIN,
                    Constants::BACK_BUTTON_MARGIN);

        renderer->icon(back, pos.x, pos.y,
                Constants::BACK_BUTTON_SIZE,
                Constants::BACK_BUTTON_SIZE,
                RGB(1.0, 1.0, 1.0), 0.1);
    }
}

void
Page::render_background(OpenGLRenderer *renderer)
{
    renderer->background(RGB::background());
}

void
Page::render_text_center(OpenGLRenderer *renderer,
        const char *text, enum FontSize size, float *y,
        float opacity)
{
    float text_w, text_h;
    renderer->text_measure(text, &text_w, &text_h, size);
    float x = (Constants::WORLD_WIDTH - text_w) / 2.0;
    renderer->text_render(text, x, *y, size, opacity);

    (*y) += text_h;
}

void
Page::render_page_title(OpenGLRenderer *renderer, const char *text)
{
    Vec2 offset = game->get_offset();

    float w, h;
    renderer->text_measure(text, &w, &h, FONT_LARGE);
    renderer->text_render(text, (Constants::WORLD_WIDTH - w) / 2.0,
            10.0 - offset.y, FONT_LARGE);
}

