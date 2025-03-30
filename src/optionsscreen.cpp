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



#include "optionsscreen.h"

#include "game.h"
#include "circle1d.h"
#include "colors.h"

class ToggleOption : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(ToggleOption)

        ToggleOption(Game *game, Scene *scene, Vec2 pos,
                const char *label, int *target,
                Storable *storable)
            : Circle1DBehavior(scene)
            , game(game)
            , pos(pos)
            , label(label)
            , target(target)
            , storable(storable)
            , old_grabbed(-1)
            , switch_object(NULL)
        {
            switch_object = new Object(scene, pos.x, pos.y, 30,
                    Object::FIXED | Object::COLLIDER | Object::CANGRAB,
                    (*target) ? Colors::SWITCH_ON : Colors::SWITCH_OFF);
        }

        virtual ~ToggleOption()
        {
        }

        void update_switch(bool force=false)
        {
            if (*target) {
                switch_object->target_color = Colors::SWITCH_ON;
            } else {
                switch_object->target_color = Colors::SWITCH_OFF;
            }

            if (force) {
                switch_object->color = switch_object->target_color;
            } else {
                game->options_changed();

                if (*target) {
                    Platform::play(Sound::TOGGLE_SWITCH_ON);
                } else {
                    Platform::play(Sound::TOGGLE_SWITCH_OFF);
                }
            }
        }

        virtual void simulate()
        {
            if (switch_object->grabbed != old_grabbed) {
                if (switch_object->grabbed != -1) {
                    if (*target) {
                        *target = 0;
                    } else {
                        *target = 1;
                    }

                    storable->save_data();
                    update_switch();
                }
                old_grabbed = switch_object->grabbed;
            }
        }

        Game *game;
        Vec2 pos;
        const char *label;
        int *target;
        Storable *storable;
        int old_grabbed;
        Object *switch_object;
};

class OptionsScreenPriv {
    public:
        OptionsScreenPriv(Game *game, StorageManager *storage_manager);
        ~OptionsScreenPriv();

        void render(OpenGLRenderer *renderer);
        void handle(Circle1DEvent *event);

        bool on_options_loaded();

        Game *m_game;
        OptionsData m_options_data;
        StorableStruct *m_storable;
        Scene m_scene;
        std::list<ToggleOption*> m_toggle_options;
};

static bool
on_options_load(void *buffer, void *user_data)
{
    OptionsScreenPriv *priv = (OptionsScreenPriv*)user_data;
    return priv->on_options_loaded();
}

OptionsScreenPriv::OptionsScreenPriv(Game *game,
        StorageManager *storage_manager)
    : m_game(game)
    , m_options_data()
    , m_storable(NULL)
    , m_scene()
    , m_toggle_options()
{
    m_storable = new StorableStruct(storage_manager,
            &m_options_data, sizeof(m_options_data),
            on_options_load, this);

    int items = 3;
    float spacing = 80.0;

    Vec2 pos(200.0, (Constants::WORLD_HEIGHT - (items - 1) * spacing) / 2.f);

    m_toggle_options.push_back(new ToggleOption(game, &m_scene, pos,
                "Fullscreen", &(m_options_data.fullscreen), m_storable));
    pos.y += spacing;

    m_toggle_options.push_back(new ToggleOption(game, &m_scene, pos,
                "Enable music", &(m_options_data.enable_music), m_storable));
    pos.y += spacing;

    m_toggle_options.push_back(new ToggleOption(game, &m_scene, pos,
                "Enable sound effects", &(m_options_data.enable_sfx), m_storable));
    pos.y += spacing;
}

OptionsScreenPriv::~OptionsScreenPriv()
{
    delete m_storable;
}

void
OptionsScreenPriv::render(OpenGLRenderer *renderer)
{
    std::list<ToggleOption*>::iterator it;
    for (it=m_toggle_options.begin(); it != m_toggle_options.end(); ++it) {
        ToggleOption *top = (*it);

        float w, h;
        renderer->text_measure(top->label, &w, &h, FONT_MEDIUM);

        Vec2 pos = top->switch_object->pos;
        pos.y -= h / 2.0;
        pos.x += top->switch_object->size + 15.0;

        renderer->text_render(top->label, pos.x, pos.y, FONT_MEDIUM);
    }

    m_scene.render(renderer);
}

void
OptionsScreenPriv::handle(Circle1DEvent *event)
{
    m_scene.handle(event);
}

bool
OptionsScreenPriv::on_options_loaded()
{
    std::list<ToggleOption*>::iterator it;
    for (it=m_toggle_options.begin(); it != m_toggle_options.end(); ++it) {
        ToggleOption *top = (*it);
        top->update_switch(true);
    }

    m_game->options_changed();

    // Assume always true for now
    return true;
}


OptionsScreen::OptionsScreen(Game *game, StorageManager *storage_manager)
    : Page(game, Icons::BACK)
    , priv(new OptionsScreenPriv(game, storage_manager))
{
}

OptionsScreen::~OptionsScreen()
{
    delete priv;
}

void
OptionsScreen::render(OpenGLRenderer *renderer)
{
    render_page_title(renderer, "Settings");
    priv->render(renderer);
}

void
OptionsScreen::handle(Circle1DEvent *event)
{
    priv->handle(event);
}

bool
OptionsScreen::on_back_button()
{
    game->to_menu();
    return true;
}

OptionsData *
OptionsScreen::get_options()
{
    return &(priv->m_options_data);
}

