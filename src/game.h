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

#ifndef SHADYPOSTPROC_GAME_H
#define SHADYPOSTPROC_GAME_H

#include "shadypostproc.h"

#include "circle1d.h"
#include "opengl_renderer.h"
#include "menu.h"
#include "storagemanager.h"
#include "levelmanager.h"
#include "scoremanager.h"
#include "performance.h"
#include "levelselector.h"
#include "resultsscreen.h"
#include "pausescreen.h"
#include "messagebox.h"
#include "gameplay.h"
#include "highscoresscreen.h"
#include "aboutscreen.h"
#include "optionsscreen.h"
#include "mixer.h"

class GamePriv;

class Game : public Circle1DEventHandler {
    public:
        Game();
        virtual ~Game();


        /**
         * "Public API" to be used by (all/most) frontends
         **/

        /* OpenGL viewport resized */
        void resize(int width, int height);

        /* Orientation changed; angle is in degrees */
        void set_orientation(int angle);

        /* Render a single frame */
        void render();

        /* Handle an input event */
        void handle(Circle1DEvent *event);

        /* Game has been paused / put in background */
        void paused();

        /* Game has been resumed / put to foreground */
        void resumed();

        /* Back button was pressed, returns true if handled */
        bool back_button();

        /* Returns true if gameplay is currently active (for Swipe Lock) */
        bool is_gameplay() { return current_page == &gameplay; }


        void show_message(const char *message) {
            message_box.show(message);
        }

        bool is_selector() { return current_page == &level_selector && level_selector.is_grid(); }

        void start_level(int level);

        void restart_level();
        void to_menu();
        void to_results();
        void to_pause();
        void to_gameplay();
        void to_selector(bool show_packs);
        void to_next_unplayed();
        void to_highscores();
        void to_about();
        void to_copyright();
        void to_options();
        void to_next_level();
        bool can_proceed();

        void options_changed();
        void start_stop_music();

        void scroll_level_selector(int level) {
            level_selector.scroll_to(level);
        }

        OpenGLRenderer *get_renderer() {
            return renderer;
        }

        ScoreManager *get_score_manager() {
            return &score_manager;
        }

        LevelManager *get_level_manager() {
            return &level_manager;
        }

        ResultsScreen *get_results_screen() {
            return &results_screen;
        }

        LevelSelector *get_level_selector() {
            return &level_selector;
        }

        Scene *get_scene() {
            return &scene;
        }

        void set_offset(Vec2 world_offset) {
            this->world_offset = world_offset;
        }

        Vec2 get_offset() {
            return world_offset;
        }

        Page *topmost() { return current_page; }

        void transition_to(Page *page, game_lambda_t func=NULL);
        void jump_to(Page *page);

        float get_sound_opacity() {
            return 0.5 + std::min(0.5, mixer.get_amplitude() * 10.0);
        }

    private:
        void update_renderer();

    private:
        long tick_last;
        long tick_accumulator;

        int screen_width;
        int screen_height;
        int screen_rotation;

        OpenGLRenderer *renderer;
        Vec2 world_offset;

        /* Without any OpenGL state */
        Performance performance;
        Scene scene;
        StorageManager storage_manager;
        LevelManager level_manager;
        ScoreManager score_manager;
        Mixer mixer;

        Menu menu;
        LevelSelector level_selector;
        ResultsScreen results_screen;
        PauseScreen pause_screen;
        MessageBox message_box;
        Gameplay gameplay;
        HighScoresScreen highscores_screen;
        AboutScreen about_screen;
        HighScoresScreen copyright_screen;
        OptionsScreen options_screen;

        OptionsData *options;

        Page *current_page;

        GamePriv *priv;
};

#endif /* SHADYPOSTPROC_GAME_H */
