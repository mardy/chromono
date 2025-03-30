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


#include "choicescreen.h"

#include "game.h"
#include "colors.h"
#include "constants.h"

ChoiceScreen::ChoiceScreen(Game *game, ChoiceScreenButton *buttons, int count, enum Icons::Icon back)
    : Page(game, back)
    , m_time(0)
    , m_buttons(buttons)
    , m_count(count)
{
    // Lay out buttons
    int total_width = 0;
    for (int i=0; i<count; i++) {
        total_width += m_buttons[i].w;
        if (i > 0) {
            total_width += Constants::CHOICE_SCREEN_BUTTON_SPACING;
        }
    }

    int x = (Constants::WORLD_WIDTH - total_width) / 2;
    for (int i=0; i<count; i++) {
        m_buttons[i].x = x;
        m_buttons[i].y = (Constants::CHOICE_SCREEN_BUTTON_Y -
                m_buttons[i].h / 2);
        x += m_buttons[i].w + Constants::CHOICE_SCREEN_BUTTON_SPACING;
    }
}

ChoiceScreen::~ChoiceScreen()
{
}

void
ChoiceScreen::render(OpenGLRenderer *renderer)
{
    for (int i=0; i<m_count; i++) {
        ChoiceScreenButton *b = &(m_buttons[i]);
        if (!(b->visible)) {
            continue;
        }

        renderer->icon(b->icon, b->x, b->y, b->w, b->h, Colors::CHOICE, 0.5);

#if 0
        float text_w, text_h;
        renderer->text_measure(b->caption, &text_w, &text_h);
        renderer->text_render(b->caption,
                b->x + (b->w - text_w) / 2,
                b->y + b->h);
#endif
    }
}

void
ChoiceScreen::handle(Circle1DEvent *event)
{
    if (event->type == Circle1DEvent::TICK) {
        m_time++;
        return;
    }

    Vec2 pos(event->x, event->y);

    for (int i=0; i<m_count; i++) {
        ChoiceScreenButton *b = &(m_buttons[i]);
        if (!b->visible) {
            continue;
        }

        switch (event->type) {
            case Circle1DEvent::MOUSEDOWN:
                if (b->finger == -1 && b->contains(pos)) {
                    Platform::play(Sound::BUTTON_PRESS);
                    b->finger = event->finger;
                }
                break;
            case Circle1DEvent::MOUSEMOTION:
                break;
            case Circle1DEvent::MOUSEUP:
                if (b->finger == event->finger) {
                    b->finger = -1;

                    if (b->contains(pos)) {
                        Platform::play(Sound::BUTTON_RELEASE);
                        b->action(game);
                    }
                }
                break;
            default:
                /* do nothing */
                break;
        }
    }
}

