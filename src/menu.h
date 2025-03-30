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

#ifndef SHADYPOSTPROC_MENU_H
#define SHADYPOSTPROC_MENU_H

#include "shadypostproc.h"
#include "circle1d.h"
#include "font.h"
#include "icons.h"
#include "pressable.h"

#include "page.h"

#include <vector>

class Menu;
class OpenGLRenderer;
class Game;

class Button : public Pressable {
    public:
        Button(Menu *menu, Object *object, enum Icons::Icon icon, const char *text, game_lambda_t listener);
        ~Button();

        void render(OpenGLRenderer *renderer, float opacity);
        void set_text(const char *text);

        virtual bool contains(Vec2 pos);
        virtual bool on_pressed();

    private:
        float x;
        float y;
        float w;
        float h;

        Menu *m_menu;
        Object *m_object;
        const char *m_text;
        game_lambda_t m_listener;

        friend class Menu;
};

class FallingObjectsBehavior;

class Menu : public Page {
    public:
        Menu(Game *game);
        ~Menu();

        Game *get_game() { return game; }

        Button *add_button(const char *text, enum Icons::Icon icon, RGB color, game_lambda_t listener=NULL);

        virtual void before_render(OpenGLRenderer *renderer);
        virtual void render(OpenGLRenderer *renderer);
        virtual void handle(Circle1DEvent *event);
        virtual void on_exposed();
        virtual void render_background(OpenGLRenderer *renderer);

        void relayout(OpenGLRenderer *renderer);

        void queue_relayout() {
            m_must_relayout = true;
        }

    private:
        Scene m_scene;
        FallingObjectsBehavior *m_falling_objects;

        std::vector<Button*> m_buttons;
        bool m_must_relayout;
        long m_time;
        Button *play_button;
        bool m_pressed;

        PressableRect m_button_about;
        PressableRect m_button_options;
};

#endif /* SHADYPOSTPROC_MENU_H */
