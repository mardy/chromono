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

#ifndef SHADYPOSTPROC_EFFECT_H
#define SHADYPOSTPROC_EFFECT_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "framebuffer.h"
#include "shaderprogram.h"
#include "vertexbuffer.h"
#include "renderable.h"

class OpenGLRenderer;

class Effect : public Renderable {
    public:
        Effect(OpenGLRenderer *renderer);
        virtual ~Effect();

        enum TransitionFrame {
            FRAME_A,
            FRAME_B,
        };

        void begin_capture(enum TransitionFrame frame);
        void end_capture(enum TransitionFrame frame);

        void bind();
        void unbind();

        void render(float value);

    private:
        Program program;
        VertexBuffer vertex_buffer;
        Framebuffer *framebuffer[2];

        GLint coord_loc;
        GLint subtex_loc;
        GLint size_loc;
        GLint brightness_loc;
};

#endif /* SHADYPOSTPROC_EFFECT_H */
