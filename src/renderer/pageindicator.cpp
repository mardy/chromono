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


#include "pageindicator.h"

#include "opengl_renderer.h"
#include "constants.h"
#include "resources.h"

PageIndicator::PageIndicator(OpenGLRenderer *renderer)
    : Renderable()
    , program(SHADER_PROGRAM(renderer, pageindicator))
    , vertex_buffer()
    , size(Constants::PAGE_INDICATOR_RADIUS*2.0)
    , spacing(size*0.3)
    , vtxcoord_loc(-1)
    , center_loc(-1)
    , size_loc(-1)
    , filled_loc(-1)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    center_loc = program.uniform("center");
    size_loc = program.uniform("size");
    filled_loc = program.uniform("filled");

    program.bind();
    glUniform1f(size_loc, size);
    program.unbind();

    float vertices[] = {
        -0.5, -0.5,
        0.5, -0.5,
        -0.5, 0.5,
        0.5, 0.5,
    };
    vertex_buffer.data(vertices, sizeof(vertices), true);
}

PageIndicator::~PageIndicator()
{
    deactivate();
}

void
PageIndicator::bind()
{
    program.bind();
    vertex_buffer.bind();

    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void
PageIndicator::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);

    vertex_buffer.unbind();
    program.unbind();
}

void
PageIndicator::render(int x, int y, int page, int pages)
{
    activate();

    x -= (pages * (size+spacing) - spacing) / 2;
    y -= size / 2.0;

    for (int i=0; i<pages; i++) {
        glUniform2f(center_loc, x, y);
        glUniform1i(filled_loc, i == page);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        x += size + spacing;
    }
}

