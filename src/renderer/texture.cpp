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


#include "texture.h"

#include <stdio.h>
#include <string.h>

std::stack<Texture*>
Texture::bindings;

Texture::Texture()
    : m_texture_id(-1),
      m_subwidth(1.),
      m_subheight(1.)
{
    glGenTextures(1, &m_texture_id);

    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    /* http://stackoverflow.com/questions/4760174/ */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    unbind();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture_id);
}

void
Texture::set_nearest_filter()
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unbind();
}

void
Texture::resize(int width, int height)
{
    m_width = width;
    m_height = height;
    unsigned char *data = new unsigned char[m_width*m_height*4];
    memset(data, 0x00, m_width*m_height*4);
    teximage(GL_RGBA, data);
    delete [] data;
}

void
Texture::teximage(GLint format, const unsigned char *data)
{
    int w = 2;
    int h = 2;

    while (m_width > 0 && w < m_width) w *= 2;
    while (m_height > 0 && h < m_height) h *= 2;

    m_subwidth = (float)m_width / (float)w;
    m_subheight = (float)m_height / (float)h;

    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0,
            format, GL_UNSIGNED_BYTE, NULL);
    if (data != NULL) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height,
                format, GL_UNSIGNED_BYTE, data);
    }
    unbind();
}

void
Texture::bind(int unit)
{
    if (unit != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        glActiveTexture(GL_TEXTURE0);
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    bindings.push(this);
}

void
Texture::unbind(int unit)
{
    if (unit != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
        return;
    }

    bindings.pop();
    glBindTexture(GL_TEXTURE_2D, 0);
    if (!bindings.empty()) {
        glBindTexture(GL_TEXTURE_2D, bindings.top()->m_texture_id);
    }
}


void
Texture::save(const char *filename)
{
    bind();

    int width = 0;
    int height = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    int sub_width = width * m_subwidth;
    int sub_height = height * m_subheight;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);

    char *texdata = new char[3 * width * height];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, texdata);
    unbind();

    /**
     * What we get in texdata is the screenshot, vertically flipped
     * and in texture size (not in framebuffer size).
     *
     * +------------------------+
     * |                 |      |
     * |                 |      |
     * +-----------------+ <- (sub_width, sub_height)
     * |                        |
     * +------------------------+ <- (width, height)
     **/
    int scanline_fb = sub_width * 3;
    int scanline_tx = width * 3;

    char *data = new char[3 * sub_width * sub_height];
    for (int y=0; y<sub_height; y++) {
        memcpy(data + y*scanline_fb,
               texdata +  (sub_height-y)*scanline_tx,
               scanline_fb);
    }

    delete [] texdata;

    // data is (width x height) RGB (8-8-8) image data
    FILE *fp = fopen(filename, "wb");
    fwrite(data, 3, sub_width*sub_height, fp);
    fclose(fp);

    delete [] data;
}

