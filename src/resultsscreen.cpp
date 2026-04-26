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


#include "resultsscreen.h"

#include "game.h"
#include "colors.h"
#include "constants.h"
#include "util.h"

static void action_restart_level(Game *g) { g->restart_level(); }
static void action_to_selector(Game *g) { g->to_selector(false); }
static void action_to_next_level(Game *g) { g->to_next_level(); }

static ChoiceScreenButton buttons[] = {
    ChoiceScreenButton("Restart", Icons::RESTART, action_restart_level),
    ChoiceScreenButton("To Menu", Icons::GRID, action_to_selector),
    ChoiceScreenButton("Next Level", Icons::PLAY, action_to_next_level),
};

ResultsScreen::ResultsScreen(Game *game, LevelManager *level_manager,
            ScoreManager *score_manager)
    : ChoiceScreen(game, buttons, ARRAY_LENGTH(buttons))
    , m_level_manager(level_manager)
    , m_score_manager(score_manager)
    , m_level(-1)
    , m_score(-1)
    , m_new_highscore(-1)
    , m_played_stars_sound(false)
    , m_background_render_cache_data(game)
{
}

ResultsScreen::~ResultsScreen()
{
}

void
ResultsScreen::render(OpenGLRenderer *renderer)
{
    renderer->draw_cached(&m_background_render_cache_data,
            sizeof(m_background_render_cache_data));

    float x, y;

    LevelInfo *info = m_level_manager->get(m_level);

    int stars_count = m_score_manager->get_stars_for(m_level, m_score);

    int stars_size = 180;
    float stars_rotation = 0.f;
    float stars_factor = 1.f;
    if (m_time < 20) {
        stars_factor = (float)m_time / 20.f;
        stars_size += 600 * (1.f - stars_factor);
        stars_rotation = M_PI / 4. * (1.f - stars_factor);
    }

    if (!m_played_stars_sound) {
        switch (stars_count) {
            case 0:
                Platform::play(Sound::LEVEL_COMPLETE_NO_STARS);
                break;
            case 1:
                Platform::play(Sound::LEVEL_COMPLETE_1_STAR);
                break;
            case 2:
                Platform::play(Sound::LEVEL_COMPLETE_2_STARS);
                break;
            case 3:
                Platform::play(Sound::LEVEL_COMPLETE_3_STARS);
                break;
            default:
                SHADY_ASSERT(!"unreachable");
                break;
        }
        m_played_stars_sound = true;
    }

    int stars_y = 130;
    y = stars_y - stars_size / 2;
    for (int i=0; i<3; i++) {
        x = (Constants::WORLD_WIDTH - stars_size*3) / 2 + stars_size * i;
        float opacity = stars_factor;
        RGB color;
        if (stars_count > i) {
            // star color
            float phase = std::min(1., m_time / (20.0 + 1. * (i+1)));
            color = RGB::mix(color, Colors::STAR, 1.f - phase);
            opacity *= 0.9;
        } else {
            opacity *= 0.2;
        }
        renderer->icon(Icons::STAR, x, y, stars_size, stars_size, color, opacity, stars_rotation);
    }

    y = 220;
    float text_spacing = -4.0;
    char tmp[1024+44];
    char score_time_str[1024];
    snprintf(tmp, sizeof(tmp), "Level %d complete", (m_level + 1));
    render_text_center(renderer, tmp, FONT_MEDIUM, &y);
    y += text_spacing;

    Util::format_score_time(m_score, score_time_str, sizeof(score_time_str));
    snprintf(tmp, sizeof(tmp), "Time taken: %s", score_time_str);
    render_text_center(renderer, tmp, FONT_MEDIUM, &y);
    y += text_spacing;

    if (stars_count < 3) {
        // Show requirement for next better star count
        Util::format_score_time(info->stars_time[stars_count],
                score_time_str, sizeof(score_time_str));
        snprintf(tmp, sizeof(tmp), "Time needed for %d %s: %s",
                stars_count + 1,
                ((stars_count + 1) == 1) ? "star" : "stars",
                score_time_str);
        render_text_center(renderer, tmp, FONT_MEDIUM, &y);
        y += text_spacing;
    }

    if (m_new_highscore && stars_count == 3) {
        snprintf(tmp, sizeof(tmp), "New highscore!");
        render_text_center(renderer, tmp, FONT_MEDIUM, &y);
        y += text_spacing;
    }

    // Set visibility of "next level" button
    buttons[2].visible = game->can_proceed();

    ChoiceScreen::render(renderer);
}

bool
ResultsScreen::on_back_button()
{
    game->to_selector(false);
    return true;
}

void
render_scene_with_overlay_callback(OpenGLRenderer *renderer, void *prepared, void *user_data)
{
    BackgroundRenderCacheData *data = (BackgroundRenderCacheData*)prepared;

    data->game->get_scene()->render(renderer);
    renderer->overlay();
}

void
ResultsScreen::before_render(OpenGLRenderer *renderer)
{
    renderer->prepare_cached(&m_background_render_cache_data,
            sizeof(m_background_render_cache_data),
            render_scene_with_overlay_callback, NULL);
}

void
ResultsScreen::on_exposed()
{
    // Invalidate sequence, so background render cache is redrawn
    m_background_render_cache_data.seq++;
}

