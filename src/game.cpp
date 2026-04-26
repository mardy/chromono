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


#include "shadypostproc.h"

#include "game.h"

#include "transition.h"
#include "constants.h"
#include "util.h"
#include "colors.h"
#include "platform.h"

#include "resources.h"

class GamePriv : public TransitionListener {
    public:
        GamePriv(Game *game)
            : game(game)
            , transition()
            , running(false)
            , target_page(NULL)
            , func(NULL)
            , need_screenshot(false)
            , state(FADE_OUT)
        {
        }

        virtual ~GamePriv()
        {
        }

        void start_transition(Page *target, game_lambda_t func=NULL) {
            target_page = target;
            this->func = func;

            if (!running) {
                state = FADE_OUT;
                running = true;
                transition.start(Constants::TRANSITION_DURATION_MS, this);
            }
        }

        void on_update(float value) {
            if (state == FADE_OUT) {
                transition_value = 0.5 * value;
            } else {
                transition_value = 0.5 + 0.5 * value;
            }
        }

        void on_finished() {
            if (state == FADE_OUT) {
                on_update(1.0);
                state = FADE_IN;
                transition.start(Constants::TRANSITION_DURATION_MS, this);
            } else {
                on_update(1.0);
                running = false;
            }
        }

        Game *game;
        Transition transition;
        bool running;
        float transition_value;

        Page *target_page;
        game_lambda_t func;
        bool need_screenshot;

        enum State {
            FADE_OUT,
            FADE_IN,
        } state;
};

static char *
get_storage_filename()
{
    const char *folder = Platform::storage_folder();

    if (folder != NULL) {
        std::string tmp = Util::format("%s/scores", folder);
        return strdup(tmp.c_str());
    }

    return strdup(".chromono_scores");
}

Game::Game()
    : tick_last(Util::ticks())
    , tick_accumulator(0)
    , renderer(NULL)
    , world_offset(0.0, 0.0)
    , performance()
    , scene()
    , storage_manager(get_storage_filename())
    , level_manager(&scene)
    , score_manager(&level_manager, &storage_manager)
    , mixer()
    , menu(this)
    , level_selector(this)
    , results_screen(this, &level_manager, &score_manager)
    , pause_screen(this)
    , message_box()
    , gameplay(this)
    , highscores_screen(this, &menu, Colors::HELP_BACKGROUND_COLOR,
            "How to Play", FONT_MEDIUM, RESOURCE(howtoplay_markup))
    , about_screen(this)
    , copyright_screen(this, &about_screen,
            Colors::COLOR_MIX_BACKGROUND_COLOR,
            "Legal Notices", FONT_SMALL, RESOURCE(copyrights_markup))
    , options_screen(this, &storage_manager)
    , options(options_screen.get_options())
    , current_page(NULL)
    , priv(new GamePriv(this))
{
    // Load data from disk
    storage_manager.load_data();

    // Begin sound mixing
    Platform::set_playing(true);

    jump_to(&menu);
}

void
Game::resize(int width, int height)
{
    if (renderer) {
        delete renderer;
    }

    renderer = new OpenGLRenderer(this, width, height);
}

Game::~Game()
{
    delete priv;
}

void Game::render() {
    //performance.set_show_fps(true);

    long now = Util::ticks();

    tick_accumulator += (now - tick_last);
    tick_last = now;

    while (tick_accumulator > Constants::TICK_MS) {
        Circle1DEvent evt(Circle1DEvent::TICK);
        handle(&evt);
        tick_accumulator -= Constants::TICK_MS;
    }

    if (!renderer->ready()) {
        // ready() returns false if we are still loading
        return;
    }

    if (priv->running) {
        if (priv->need_screenshot) {
            current_page->before_render(renderer);
            renderer->begin_capture(Effect::FRAME_A);
            renderer->begin();
            current_page->render_page(renderer);
            renderer->finish();
            renderer->end_capture(Effect::FRAME_A);

            if (priv->func != NULL) {
                priv->func(this);
            }
            jump_to(priv->target_page);

            current_page->before_render(renderer);
            renderer->begin_capture(Effect::FRAME_B);
            renderer->begin();
            current_page->render_page(renderer);
            renderer->finish();
            renderer->end_capture(Effect::FRAME_B);

            priv->need_screenshot = false;
        }

        renderer->begin();
        renderer->transition(priv->transition_value);
        renderer->finish();
        performance.frame();
        return;
    }

    current_page->before_render(renderer);

    renderer->begin();

    current_page->render_page(renderer);

    if (message_box.opened()) {
        renderer->overlay();
        message_box.render(renderer);
    }

    /*
    char tmp[1024];
    sprintf(tmp, "%.2f FPS", performance.framerate());
    renderer->text_render(tmp, 700, 10, FONT_SMALL);
    */

    renderer->finish();
    renderer->text_gc();

    performance.frame();
}

void
Game::handle(Circle1DEvent *event)
{
    Vec2 pos = renderer->screen2world(Vec2(event->x, event->y));
    event->x = pos.x;
    event->y = pos.y;

    if (event->type == Circle1DEvent::TICK) {
        mixer.tick();
    }

    if (message_box.opened()) {
        message_box.handle(event);
        return;
    }

    if (priv->running) {
        priv->transition.handle(event);
    } else {
        current_page->handle_page(event);
    }
}

void
Game::restart_level()
{
    level_manager.start(level_manager.current());
    to_gameplay();
}

void
Game::to_menu()
{
    transition_to(&menu);
}

void
Game::to_results()
{
    jump_to(&results_screen);
}

void
Game::to_pause()
{
    Platform::play(Sound::GAMEPLAY_PAUSED);
    jump_to(&pause_screen);
}

void
Game::to_gameplay()
{
    if (current_page == &pause_screen || current_page == &results_screen) {
        jump_to(&gameplay);
    } else {
        transition_to(&gameplay);
    }
}

void
Game::to_selector(bool show_packs)
{
    scroll_level_selector(level_manager.current());
    if (show_packs) {
        level_selector.go_to_pack();
    } else {
        level_selector.go_to_grid();
    }
    transition_to(&level_selector);
}

void
Game::to_next_unplayed()
{
    // How many stars have we already earned
    int have_stars = score_manager.stars();

    // First try: Go to next level that has not been finished yet
    for (int level=0; level<level_manager.count(); level++) {
        LevelInfo *info = level_manager.get(level);
        long score = score_manager.get_score(level);
        if (score == -1 && (info->required_stars <= have_stars ||
                    Constants::UNLOCK_ALL)) {
            start_level(level);
            return;
        }
    }

    // Second try: Go to first level with less than 3 stars
    for (int level=0; level<level_manager.count(); level++) {
        LevelInfo *info = level_manager.get(level);
        int stars = score_manager.get_stars(level);
        if (stars < 3 && (info->required_stars <= have_stars ||
                    Constants::UNLOCK_ALL)) {
            start_level(level);
            return;
        }
    }

    // No unplayed level - go to selector instead
    to_selector(true);
}

void
Game::to_highscores()
{
    transition_to(&highscores_screen);
}

void
Game::to_about()
{
    transition_to(&about_screen);
}

void
Game::to_copyright()
{
    transition_to(&copyright_screen);
}

void
Game::to_options()
{
    transition_to(&options_screen);
}

void
switch_to_next_level(Game *game)
{
    LevelManager *level_manager = game->get_level_manager();
    LevelSelector *level_selector = game->get_level_selector();

    int next_level = level_manager->current() + 1;

    if (!level_selector->current_pack_contains(next_level)) {
        game->to_selector(true);
        return;
    }

    if (next_level < level_manager->count()) {
        game->scroll_level_selector(next_level);
        level_manager->start(next_level);
    }
}

void
Game::to_next_level()
{
    transition_to(&gameplay, switch_to_next_level);
}

bool
Game::can_proceed()
{
    int next_level = level_manager.current() + 1;

    if (next_level < level_manager.count()) {
        LevelInfo *info = level_manager.get(next_level);
        return (score_manager.stars() >= info->required_stars || Constants::UNLOCK_ALL);
    }

    return false;
}

void
Game::start_level(int level)
{
    scroll_level_selector(level);
    level_manager.start(level);
    transition_to(&gameplay);
}

void
Game::paused()
{
    current_page->on_paused();
    Platform::set_playing(false);
}

void
Game::resumed()
{
    tick_last = Util::ticks();
    Platform::set_playing(true);
}

bool
Game::back_button()
{
    if (message_box.opened()) {
        message_box.dismiss();
        return true;
    }

    return current_page->on_back_button();
}

void
Game::transition_to(Page *page, game_lambda_t func)
{
    priv->need_screenshot = true;
    priv->transition_value = 0.0;
    priv->start_transition(page, func);
    Platform::play(Sound::PAGE_TRANSITION);
}

void
Game::jump_to(Page *page)
{
    if (current_page == page) {
        return;
    }

    current_page = page;
    current_page->on_exposed();
    start_stop_music();
}

void
Game::options_changed()
{
    Platform::set_fullscreen(options->fullscreen);
    start_stop_music();
}

void
Game::start_stop_music()
{
    OptionsData *options = options_screen.get_options();
    if (!(options->enable_music)) {
        mixer.loop(Sound::NO_SOUND, Constants::MUSIC_VOLUME);
    } else if (current_page == &gameplay) {
        mixer.loop(Sound::MUSIC_SNAPPER, Constants::MUSIC_VOLUME);
    } else if (current_page == &pause_screen) {
        mixer.loop(Sound::MUSIC_SNAPPER, Constants::MUSIC_VOLUME, 0.0);
    } else if (current_page == &menu) {
        mixer.loop(Sound::MUSIC_ALVEG, Constants::MUSIC_VOLUME);
    } else if (current_page == &level_selector) {
        mixer.loop(Sound::MUSIC_SUBD, Constants::MUSIC_VOLUME);
    } else if (current_page != NULL) {
        mixer.loop(Sound::MUSIC_KASA90, Constants::MUSIC_VOLUME);
    }

    mixer.set_sound_effects_enabled(options->enable_sfx);
}

