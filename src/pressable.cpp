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


#include "pressable.h"

#include "platform.h"

Pressable::Pressable()
    : finger(-1)
{
}

Pressable::~Pressable()
{
}

bool
Pressable::handle(Circle1DEvent *event)
{
    Vec2 pos(event->x, event->y);

    switch (event->type) {
        case Circle1DEvent::MOUSEDOWN:
            if (finger == -1 && contains(pos)) {
                Platform::play(Sound::BUTTON_PRESS);
                finger = event->finger;
                return true;
            }
            break;
        case Circle1DEvent::MOUSEMOTION:
            if (finger == event->finger) {
                return true;
            }
            break;
        case Circle1DEvent::MOUSEUP:
            if (finger == event->finger) {
                if (contains(pos)) {
                    if (!on_pressed()) {
                        Platform::play(Sound::BUTTON_RELEASE);
                    }
                }

                finger = -1;
                return true;
            }
            break;
        default:
            /* ignore event */
            break;
    }

    return false;
}

