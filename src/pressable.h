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

#ifndef SHADYPOSTPROC_PRESSABLE_H
#define SHADYPOSTPROC_PRESSABLE_H

#include "shadypostproc.h"

#include "circle1d.h"

class Pressable {
    public:
        Pressable();
        virtual ~Pressable();

        bool handle(Circle1DEvent *event);

        virtual bool contains(Vec2 pos) = 0;

        /* Return true here to avoid release sound */
        virtual bool on_pressed() = 0;

    private:
        int finger;
};

class GamePressable : public Pressable {
    public:
        GamePressable(Game *game, game_lambda_t callback)
            : Pressable()
            , game(game)
            , callback(callback)
        {
        }

        virtual ~GamePressable()
        {
        }

        virtual bool on_pressed()
        {
            callback(game);
            return false;
        }

    protected:
        Game *game;
        game_lambda_t callback;
};

class PressableRect : public GamePressable {
    public:
        PressableRect(Game *game, game_lambda_t callback,
                float x, float y, float w, float h)
            : GamePressable(game, callback)
            , x(x)
            , y(y)
            , w(w)
            , h(h)
        {
        }

        PressableRect(Game *game, game_lambda_t callback,
                Vec2 pos, float size)
            : GamePressable(game, callback)
            , x(pos.x)
            , y(pos.y)
            , w(size)
            , h(size)
        {
        }

        virtual ~PressableRect()
        {
        }

        virtual bool contains(Vec2 pos)
        {
            return (pos.x >= x && pos.x <= (x + w) &&
                    pos.y >= y && pos.y <= (y + h));
        }

        Vec2 pos() { return Vec2(x, y); }
        void setPos(Vec2 pos) { x = pos.x; y = pos.y; }

    private:
        float x;
        float y;
        float w;
        float h;
};

#endif /* SHADYPOSTPROC_PRESSABLE_H */
