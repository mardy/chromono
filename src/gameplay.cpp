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


#include "gameplay.h"

#include "game.h"

Gameplay::Gameplay(Game *game)
    : Page(game, Icons::PAUSE)
{
}


void
Gameplay::handle(Circle1DEvent *event)
{
    Scene *scene = game->get_scene();
    if (event->type == Circle1DEvent::TICK && scene->objectiveReached()) {
        ScoreManager *score_manager = game->get_score_manager();
        LevelManager *level_manager = game->get_level_manager();
        ResultsScreen *results_screen = game->get_results_screen();

        int level = level_manager->current();
        bool new_highscore = score_manager->register_score(level, scene->time);
        results_screen->set_results(level, scene->time, new_highscore);

        game->to_results();
    }

    scene->handle(event);
}

void
Gameplay::render(OpenGLRenderer *renderer)
{
    game->get_scene()->render(renderer);
}

void
Gameplay::render_background(OpenGLRenderer *renderer)
{
    renderer->background(game->get_scene()->background_color);
}

bool
Gameplay::on_back_button()
{
    game->to_pause();
    return true;
}

void
Gameplay::on_paused()
{
    game->to_pause();
}

