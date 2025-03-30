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

#ifndef SHADYPOSTPROC_HIGHSCORESSCREEN_H
#define SHADYPOSTPROC_HIGHSCORESSCREEN_H

#include "shadypostproc.h"

#include "page.h"
#include "resources_util.h"

#include <vector>

class HighScoresScreen : public Page {
    public:
        HighScoresScreen(Game *game, Page *parent, RGB color,
                const char *title, enum FontSize size,
                Resource *resource);

        virtual void render(OpenGLRenderer *renderer);
        virtual void handle(Circle1DEvent *event);
        virtual bool on_back_button();
        virtual void render_background(OpenGLRenderer *renderer);
        virtual void on_exposed();

    private:
        Page *parent;
        RGB color;
        const char *title;
        enum FontSize size;
        bool pressed;
        Vec2 last_mouse_pos;
        Vec2 offset;
        float max_offset_y;
        Vec2 velocity;
        ResourceAccess textfile;
        std::vector<char*> textlines;
        int ticks_since_last_move;
};

#endif /* SHADYPOSTPROC_HIGHSCORESSCREEN_H */
