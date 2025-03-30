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


#include "icons.h"

#include "opengl_renderer.h"
#include "resources.h"
#include "resources_util.h"
#include "util.h"

Icons::Icons(OpenGLRenderer *renderer)
    : program(SHADER_PROGRAM(renderer, icons))
    , vertex_buffer()
    , texture()
    , vtxcoord_loc(-1)
    , texcoord_loc(-1)
    , color_loc(-1)
{
    prepare();
}

void
Icons::prepare()
{
    int width = 512;
    int height = 512;

    ResourceAccess access(RESOURCE(icons_rgb));
    texture.setrgb(width, height, access.data());

    vtxcoord_loc = program.attrib("vtxcoord");
    texcoord_loc = program.attrib("texcoord");
    color_loc = program.uniform("color");
}

Icons::~Icons()
{
    deactivate();
}

void
Icons::bind()
{
    program.bind();
    texture.bind();

    vertex_buffer.bind();

    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(texcoord_loc);
    glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0,
            (void*)(2*4*sizeof(float)) /* Offset of texcoords in render() */);
}

void
Icons::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    glDisableVertexAttribArray(texcoord_loc);

    vertex_buffer.unbind();

    texture.unbind();
    program.unbind();
}

void
Icons::render(enum Icon icon, int x, int y, int w, int h,
        RGB color, float opacity, float rotation)
{
    activate();

    int row = ((int)icon) / 3;
    int column = ((int)icon) % 3;

    float side = 1. / 3.;

    float vtxdata[] = {
        /* Vertex coordinates */
        float(x), float(y),
        float(x+w), float(y),
        float(x), float(y+h),
        float(x+w), float(y+h),

        /* Texture coordinates */
        column * side, row * side,
        (column + 1) * side, row * side,
        column * side, (row + 1) * side,
        (column + 1) * side, (row + 1) * side,
    };
    Vec2(x + w/2, y+h/2).rotate_around(rotation, vtxdata, 4);

    vertex_buffer.data(vtxdata, sizeof(vtxdata));

    glUniform4f(color_loc, color.r, color.g, color.b, opacity);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

