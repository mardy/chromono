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

#ifndef QW_FRAMEBUFFER_H
#define QW_FRAMEBUFFER_H

#include "chromono_opengl.h"

#include "texture.h"

class OpenGLRenderer;

class Framebuffer
{
    public:
        Framebuffer(OpenGLRenderer *renderer, int width, int height, float scale=1.);
        ~Framebuffer();

        void bind();
        void unbind();

        Texture *texture() { return &m_texture; }

    private:
        GLuint m_framebuffer_id;
    public:
        int m_width;
        int m_height;
    private:
        float m_scale;
        GLint m_oldFB0;

        Texture m_texture;
        OpenGLRenderer *m_renderer;
};

#endif /* QW_FRAMEBUFFER_H */
