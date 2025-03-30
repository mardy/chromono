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


#include "aboutscreen.h"

#include "game.h"

AboutScreen::AboutScreen(Game *game)
    : Page(game, Icons::BACK)
    , scene()
    , a(NULL)
    , b(NULL)
    , c(NULL)
    , opacity(0.f)
{
}

void
AboutScreen::render(OpenGLRenderer *renderer)
{
    float y = 120;

    render_text_center(renderer, "chro.mono", FONT_XLARGE, &y);
    render_text_center(renderer, "Version 1.1.1", FONT_MEDIUM, &y);
    render_text_center(renderer, "Open Source Release", FONT_SMALL, &y);

    y += 30;
    render_text_center(renderer, "Copyright (c) 2021 Thomas Perl", FONT_MEDIUM, &y);
    render_text_center(renderer, "https://thp.io/2013/chromono/", FONT_MEDIUM, &y);

    y += 30;
    render_text_center(renderer, "[Legal notices]", FONT_MEDIUM, &y);

    float easter_egg_y_min = 275.;
    float easter_egg_y_max = 300.;

    if ((a->pos.y >= easter_egg_y_min && a->pos.y <= easter_egg_y_max &&
         b->pos.y >= easter_egg_y_min && b->pos.y <= easter_egg_y_max &&
         c->pos.y >= easter_egg_y_min && c->pos.y <= easter_egg_y_max) &&
        (a->pos.x >= 375. && a->pos.x <= 390. &&
         b->pos.x >= 504. && b->pos.x <= 510. &&
         c->pos.x >= 587. && c->pos.x <= 592.))
    {
        opacity = 1.f - 0.95f * (1.f - opacity);
    } else {
        opacity *= 0.95f;
    }

    if (opacity) {
        // Here comes the easter egg...
        // Cover the copyright line with the spheres so that the text reads:
        // "Copyright Th P" -> Easter egg decal will be shown
        renderer->decal(Circle1D::EASTEREGG, 660., 100., 0.3, 1.0, 0.5 * opacity);
    }

    scene.render(renderer);
}

void
AboutScreen::handle(Circle1DEvent *event)
{
    scene.handle(event);


    if (event->type == Circle1DEvent::MOUSEDOWN) {
        if ((event->x >= 308 && event->x <= 308 + 182 &&
             event->y >= 372 && event->y <= 410)) {
            // "Legal notices" text
            game->to_copyright();
        }
    }
}

bool
AboutScreen::on_back_button()
{
    game->to_menu();
    return true;
}

void
AboutScreen::render_background(OpenGLRenderer *renderer)
{
    renderer->background(RGB(0x666666));
}

void
AboutScreen::on_exposed()
{
    opacity = 0.f;
    scene.reset();
    int flags = Object::NOGRAVITY | Object::COLLIDER | Object::BORDERBOUNCE;
    a = new Object(&scene, 600, 80, 50, flags, RGB(0.9, 0.5, 0.2));
    b = new Object(&scene, 680, 170, 40, flags, RGB(0.2, 0.4, 0.8));
    c = new Object(&scene, 720, 260, 30, flags, RGB(0.3, 0.8, 0.2));
}

