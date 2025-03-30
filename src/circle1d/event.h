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

#ifndef CIRCLE1D_EVENT_H
#define CIRCLE1D_EVENT_H

class Circle1DEvent {
    public:
        enum Type {
            NONE = 0,
            MOUSEDOWN,
            MOUSEUP,
            MOUSEMOTION,
            TICK,

            KEYDOWN,
            KEYUP,
        };

        enum Key {
            IGNORE = 0,
            SAVE_SCENE,
            LOAD_SCENE,
            EDIT,
            DELETE,
            TOGGLE_COLLIDER,
            TOGGLE_GRAVITY,
            TOGGLE_BORDERBOUNCE,
            TOGGLE_FIXED,
            TOGGLE_SHADOWCAST,
            SET_COLOR,
            SET_TARGET_COLOR,
            SET_DESIRED_COLOR,
        };

        Circle1DEvent(enum Type type=NONE, float x=0, float y=0, int finger=0)
            : type(type)
            , x(x)
            , y(y)
            , finger(finger)
            , button(0)
        {
        }

        enum Type type;
        float x;
        float y;
        int finger; // multi-touch support, for key events an enum Key
        int button; // mouse button, for key events extra data

        bool is_mouse_event() {
            return type == MOUSEDOWN || type == MOUSEUP || type == MOUSEMOTION;
        }

        bool is_key_event() {
            return type == KEYDOWN || type == KEYUP;
        }
};

class Circle1DEventHandler {
    public:
        virtual void handle(Circle1DEvent *event) = 0;
};

#endif /* CIRCLE1D_EVENT_H */
