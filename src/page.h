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

#ifndef SHADYPOSTPROC_PAGE_H
#define SHADYPOSTPROC_PAGE_H

#include "shadypostproc.h"

#include "circle1d.h"
#include "icons.h"
#include "pressable.h"

class OpenGLRenderer;
class Game;

class Page : public Circle1DEventHandler {
    public:
        Page(Game *game, enum Icons::Icon back=Icons::NONE);

        /* Used by game */
        void handle_page(Circle1DEvent *event);
        void render_page(OpenGLRenderer *renderer);

        /* Implemented/overridden by subclasses */
        virtual void handle(Circle1DEvent *event) = 0;
        virtual void render(OpenGLRenderer *renderer) = 0;
        virtual void before_render(OpenGLRenderer *renderer) {}

        virtual void on_exposed() { }
        virtual void on_paused() { }
        virtual bool on_back_button() { return false; }
        virtual void render_background(OpenGLRenderer *renderer);

        /* Helper function */
        void render_text_center(OpenGLRenderer *renderer,
                const char *text, enum FontSize size, float *y,
                float opacity=1.0);
        void render_page_title(OpenGLRenderer *renderer,
                const char *text);

    protected:
        Game *game;

    private:
        enum Icons::Icon back;
        PressableRect back_pressable;
};

#endif /* SHADYPOSTPROC_PAGE_H */
