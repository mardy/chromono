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


#include "pausescreen.h"

#include "game.h"
#include "constants.h"

static void action_restart_level(Game *g) { g->restart_level(); }
static void action_to_selector(Game *g) { g->to_selector(false); }

static ChoiceScreenButton buttons[] = {
    ChoiceScreenButton("Restart", Icons::RESTART, action_restart_level),
    ChoiceScreenButton("To Menu", Icons::GRID, action_to_selector),
};

PauseScreen::PauseScreen(Game *game)
    : ChoiceScreen(game, buttons, ARRAY_LENGTH(buttons), Icons::PAUSE)
{
}

PauseScreen::~PauseScreen()
{
}

void
PauseScreen::render(OpenGLRenderer *renderer)
{
    game->get_scene()->render(renderer);
    renderer->overlay();

    float y = 230;

    render_text_center(renderer, "Game paused", FONT_XLARGE, &y);
    ChoiceScreen::render(renderer);
}

bool
PauseScreen::on_back_button()
{
    Platform::play(Sound::GAMEPLAY_RESUMED);
    game->to_gameplay();
    return true;
}

