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

#ifndef SHADYPOSTPROC_RESULTSSCREEN_H
#define SHADYPOSTPROC_RESULTSSCREEN_H

#include "shadypostproc.h"

#include "choicescreen.h"

#include "levelmanager.h"
#include "scoremanager.h"

struct BackgroundRenderCacheData {
    Game *game;
    int seq;

    BackgroundRenderCacheData(Game *game)
        : game(game)
        , seq(0)
    {}
};

class ResultsScreen : public ChoiceScreen {
    public:
        ResultsScreen(Game *game, LevelManager *level_manager,
                ScoreManager *score_manager);
        virtual ~ResultsScreen();

        virtual void render(OpenGLRenderer *renderer);
        virtual bool on_back_button();

        virtual void before_render(OpenGLRenderer *renderer);
        virtual void on_exposed();

        void set_results(int level, long score, bool new_highscore) {
            m_time = 0;
            m_level = level;
            m_score = score;
            m_new_highscore = new_highscore;
            m_played_stars_sound = false;
        }

    private:
        LevelManager *m_level_manager;
        ScoreManager *m_score_manager;

        int m_level;
        long m_score;
        bool m_new_highscore;
        bool m_played_stars_sound;

        BackgroundRenderCacheData m_background_render_cache_data;
};

#endif /* SHADYPOSTPROC_RESULTSSCREEN_H */
