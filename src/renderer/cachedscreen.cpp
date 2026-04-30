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


#include "cachedscreen.h"

#include "opengl_renderer.h"
#include "resources.h"


CachedScreen::CachedScreen(OpenGLRenderer *renderer)
    : program(SHADER_PROGRAM(renderer, cachedscreen))
    , vertex_buffer()
    , vtxcoord_loc(-1)
    , offset_loc(-1)
    , subsize_loc(-1)
    , rotated_loc(-1)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    offset_loc = program.uniform("offset");
    subsize_loc = program.uniform("subsize");
    rotated_loc = program.uniform("rotated");

    float vertices[] = {
        -1, -1,
        -1, 1,
        1, -1,
        1, 1,
    };
    vertex_buffer.data(vertices, sizeof(vertices), true);
}

CachedScreen::~CachedScreen()
{
    deactivate();
}

void
CachedScreen::bind()
{
    program.bind();

    vertex_buffer.bind();
    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void
CachedScreen::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    vertex_buffer.unbind();

    program.unbind();
}

void
CachedScreen::render(Texture *texture, int rotation, Vec2 offset)
{
    activate();

    if (rotation == 90) {
        glUniform2f(offset_loc, -offset.y, offset.x);
    } else if (rotation == 270) {
        glUniform2f(offset_loc, offset.y, -offset.x);
    } else if (rotation == 180) {
        glUniform2f(offset_loc, -offset.x, -offset.y);
    } else {
        glUniform2f(offset_loc, offset.x, offset.y);
    }
    glUniform2f(subsize_loc, texture->m_subwidth, texture->m_subheight);

    texture->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    texture->unbind();
}

