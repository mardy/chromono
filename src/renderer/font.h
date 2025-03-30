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

#ifndef SHADYPOSTPROC_FONT_H
#define SHADYPOSTPROC_FONT_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "circle1d.h"

#include "texture.h"

#include "renderable.h"
#include "shaderprogram.h"
#include "vertexbuffer.h"
#include "platform.h"

#include <string>

class OpenGLRenderer;

class FontPriv;
class RenderedText;

class Font : public Renderable {
    public:
        Font(OpenGLRenderer *renderer);
        virtual ~Font();

        void render(RenderedText *text, int x, int y, float scale, float opacity, RGB color);

    private:
        void bind();
        void unbind();

        Program program;
        VertexBuffer vertex_buffer;
        FontPriv *priv;

        GLint vtxcoord_loc;
        GLint texcoord_loc;
        GLint transform_loc;
        GLint color_loc;

        friend class RenderedText;
};

class RenderedText {
    public:
        RenderedText(Font *font, const char *text);
        ~RenderedText();

        std::string m_text;
        float *m_vertices;
        int m_count;

        int m_width;
        int m_height;

        int m_age;
};

#endif /* SHADYPOSTPROC_FONT_H */
