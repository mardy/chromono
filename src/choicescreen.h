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

#ifndef SHADYPOSTPROC_CHOICESCREEN_H
#define SHADYPOSTPROC_CHOICESCREEN_H

#include "shadypostproc.h"

#include "circle1d.h"
#include "opengl_renderer.h"
#include "icons.h"
#include "constants.h"
#include "page.h"

class Game;

struct ChoiceScreenButton {
    ChoiceScreenButton(const char *caption, enum Icons::Icon icon, game_lambda_t action)
        : x(0)
        , y(0)
        , w(Constants::CHOICE_SCREEN_BUTTON_SIZE)
        , h(Constants::CHOICE_SCREEN_BUTTON_SIZE)
        , visible(true)
        , finger(-1)
        , caption(caption)
        , icon(icon)
        , action(action)
    {
    }

    bool contains(Vec2 pos) {
        return ((x <= pos.x) && ((x + w) >= pos.x) &&
                (y <= pos.y) && ((y + h) >= pos.y));
    }

    int x;
    int y;
    int w;
    int h;
    bool visible;
    int finger;
    const char *caption;
    enum Icons::Icon icon;
    game_lambda_t action;
};

class ChoiceScreen : public Page {
    public:
        ChoiceScreen(Game *game, ChoiceScreenButton *buttons, int count, enum Icons::Icon back=Icons::NONE);
        virtual ~ChoiceScreen();

        virtual void render(OpenGLRenderer *renderer);
        virtual void handle(Circle1DEvent *event);
        
    protected:
        long m_time;

    private:
        ChoiceScreenButton *m_buttons;
        int m_count;
};

#endif /* SHADYPOSTPROC_CHOICESCREEN_H */
