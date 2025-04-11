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


#include "font.h"

#include "resources.h"

#include <string.h>

#include "stb_font_SourceSansProSemiBold.inl"

#define USED_FONT_PIXEL_HEIGHT 50

class FontPriv {
    public:
        FontPriv();
        ~FontPriv();

        stb_fontchar fontdata[STB_SOMEFONT_NUM_CHARS];
        Texture texture;
};

FontPriv::FontPriv()
    : fontdata()
    , texture()
{
    auto fontpixels = new unsigned char[STB_SOMEFONT_BITMAP_HEIGHT_POW2][STB_SOMEFONT_BITMAP_WIDTH];
    STB_SOMEFONT_CREATE(fontdata, fontpixels, STB_SOMEFONT_BITMAP_HEIGHT_POW2);
    texture.setalpha(STB_SOMEFONT_BITMAP_WIDTH, STB_SOMEFONT_BITMAP_HEIGHT_POW2,
            (const unsigned char*)fontpixels);
    delete []fontpixels;
}

FontPriv::~FontPriv()
{
}

Font::Font(OpenGLRenderer *renderer)
    : Renderable()
    , program(SHADER_PROGRAM(renderer, font))
    , vertex_buffer()
    , priv(new FontPriv)
    , vtxcoord_loc(-1)
    , texcoord_loc(-1)
    , transform_loc(-1)
    , color_loc(-1)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    texcoord_loc = program.attrib("texcoord");
    transform_loc = program.uniform("transform");
    color_loc = program.uniform("color");
}

Font::~Font()
{
    delete priv;

    deactivate();
}

void
Font::bind()
{
    program.bind();
    vertex_buffer.bind();

    priv->texture.bind();

    glEnableVertexAttribArray(vtxcoord_loc);
    glEnableVertexAttribArray(texcoord_loc);
}

void
Font::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    glDisableVertexAttribArray(texcoord_loc);

    priv->texture.unbind();

    vertex_buffer.unbind();
    program.unbind();
}

void
Font::render(RenderedText *text, int x, int y, float scale, float opacity, RGB color)
{
    activate();

    glUniform4f(color_loc, color.r, color.g, color.b, opacity);
    glUniform4f(transform_loc, x, y, scale, 0.0);

    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0,
        (void*)(sizeof(float)*2*6*text->m_count) /* = offset of texcoords in vtxdata */);
    vertex_buffer.data(text->m_vertices, sizeof(float) * 2 * 2 * 6 * text->m_count);

    glDrawArrays(GL_TRIANGLES, 0, 6 * text->m_count);
}

RenderedText::RenderedText(Font *font, const char *text)
    : m_text(text)
    , m_vertices(NULL)
    , m_count(0)
    , m_width(0)
    , m_height(0)
    , m_age(0)
{
    m_count = strlen(text);

    // 6 corners per quad,
    // 2 attrs (vtx, tex) per corner,
    // 2 coords (x, y) per attr
    m_vertices = new float[m_count * 6 * 2 * 2];

    // Font offset in distance field font
    Vec2 offset(5, 5);

    m_height = USED_FONT_PIXEL_HEIGHT;
    m_width = 0;
    for (int i=0; i<m_count; i++) {
        int char_codepoint = m_text[i];

        stb_fontchar *cd = &(font->priv->fontdata[char_codepoint -
                STB_SOMEFONT_FIRST_CHAR]);

        int vtx_offset = 6 * 2 * i;
        int tex_offset = (m_count * 6 * 2) + 6 * 2 * i;

        m_vertices[vtx_offset +  0] = m_width + cd->x0f - offset.x;
        m_vertices[vtx_offset +  1] = cd->y0f - offset.y;
        m_vertices[vtx_offset +  2] = m_width + cd->x1f - offset.x;
        m_vertices[vtx_offset +  3] = cd->y0f - offset.y;
        m_vertices[vtx_offset +  4] = m_width + cd->x0f - offset.x;
        m_vertices[vtx_offset +  5] = cd->y1f - offset.y;

        m_vertices[vtx_offset +  6] = m_width + cd->x1f - offset.x;
        m_vertices[vtx_offset +  7] = cd->y0f - offset.y;
        m_vertices[vtx_offset +  8] = m_width + cd->x0f - offset.x;
        m_vertices[vtx_offset +  9] = cd->y1f - offset.y;
        m_vertices[vtx_offset + 10] = m_width + cd->x1f - offset.x;
        m_vertices[vtx_offset + 11] = cd->y1f - offset.y;

        m_vertices[tex_offset +  0] = cd->s0f;
        m_vertices[tex_offset +  1] = cd->t0f;
        m_vertices[tex_offset +  2] = cd->s1f;
        m_vertices[tex_offset +  3] = cd->t0f;
        m_vertices[tex_offset +  4] = cd->s0f;
        m_vertices[tex_offset +  5] = cd->t1f;

        m_vertices[tex_offset +  6] = cd->s1f;
        m_vertices[tex_offset +  7] = cd->t0f;
        m_vertices[tex_offset +  8] = cd->s0f;
        m_vertices[tex_offset +  9] = cd->t1f;
        m_vertices[tex_offset + 10] = cd->s1f;
        m_vertices[tex_offset + 11] = cd->t1f;

        m_width += cd->advance;
    }
}

RenderedText::~RenderedText()
{
    delete [] m_vertices;
}

