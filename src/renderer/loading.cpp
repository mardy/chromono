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


#include "loading.h"

#include "opengl_renderer.h"
#include "constants.h"

#include "resources.h"

Loading::Loading(OpenGLRenderer *renderer)
    : Renderable()
    , program(SHADER_PROGRAM(renderer, loading))
    , vertex_buffer()
    , vtxcoord_loc(-1)
    , color_loc(-1)
    , counter(0)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    color_loc = program.uniform("color");
}

Loading::~Loading()
{
    deactivate();
}

void
Loading::bind()
{
    program.bind();
    vertex_buffer.bind();

    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void
Loading::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);

    vertex_buffer.unbind();
    program.unbind();
}

void
Loading::render(float percentage)
{
    activate();

    float w, h;

    float intensity = 1.0 - percentage;

    if (percentage < 0.2) {
        intensity = percentage * 5.0;
    }

    glUniform4f(color_loc, intensity, intensity, intensity, 1.0);

    for (int i=0; i<6; i++) {
        w = 10.0;

        if ((counter % 6) == i) {
            w = 20.0;
        }

        h = w;

        float x = (Constants::WORLD_WIDTH - 30 * 7) / 2.0;
        x += i * 30;
        x -= w / 2.0;

        float y = Constants::WORLD_HEIGHT / 2.0;
        y -= h / 2.0;

        float vertices[] = {
            x, y,
            x+w, y,
            x, y+h,
            x+w, y+h,
        };
        vertex_buffer.data(vertices, sizeof(vertices));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    counter++;
}

