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

#ifndef SHADYPOSTPROC_ICONS_H
#define SHADYPOSTPROC_ICONS_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "circle1d.h"

#include "renderable.h"
#include "shaderprogram.h"
#include "vertexbuffer.h"
#include "framebuffer.h"
#include "texture.h"

class OpenGLRenderer;

class Icons : public Renderable {
    public:
        Icons(OpenGLRenderer *renderer);
        virtual ~Icons();

        virtual void prepare();

        virtual void bind();
        virtual void unbind();

        enum Icon {
            ABOUT = 0,
            GRID = 1,
            LOCK = 2,
            RESTART = 3,
            OPTIONS = 4,
            STAR = 5,
            BACK = 6,
            PAUSE = 7,
            PLAY = 8,

            NONE = 9,
        };

        void render(enum Icon icon, int x, int y, int w, int h,
                RGB color, float opacity, float rotation);

    private:
        Program program;
        VertexBuffer vertex_buffer;

        Texture texture;

        GLint vtxcoord_loc;
        GLint texcoord_loc;
        GLint color_loc;
};

#endif /* SHADYPOSTPROC_ICONS_H */
