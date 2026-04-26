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

#ifndef QW_TEXTURE_H
#define QW_TEXTURE_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include <stack>

class Texture {
    public:
        Texture();
        ~Texture();

        void resize(int width, int height);

        void bind(int unit=0);
        void unbind(int unit=0);

        void set_nearest_filter();

        GLuint id() { return m_texture_id; }

        void setalpha(int w, int h, const unsigned char *data) {
            resize(w, h);
            teximage(GL_ALPHA, data);
        }

        void setrgb(int w, int h, const unsigned char *data) {
            resize(w, h);
            teximage(GL_RGB, data);
        }

    private:
        void teximage(GLint format, const unsigned char *data);

        static std::stack<Texture*> bindings;

        GLuint m_texture_id;
        int m_width;
        int m_height;

    public:
        float m_subwidth;
        float m_subheight;
};

#endif /* QW_TEXTURE_H */
