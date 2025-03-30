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


#include "shadow.h"

#include "opengl_renderer.h"

#include "resources.h"

Shadow::Shadow(OpenGLRenderer *renderer)
    : Renderable()
    , program(SHADER_PROGRAM(renderer, shadow))
    , vertex_buffer()
    , vertex_accumulator()
    , vtxcoord_loc(-1)
    , opacity_loc(-1)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    opacity_loc = program.attrib("opacity");

    vertex_accumulator.add_attrib(vtxcoord_loc, 2);
    vertex_accumulator.add_attrib(opacity_loc, 1);
}

void
Shadow::bind()
{
    program.bind();

    vertex_buffer.bind();
    vertex_accumulator.enable_arrays();
}

void
Shadow::unbind()
{
    vertex_accumulator.disable_arrays();
    vertex_buffer.unbind();

    program.unbind();
}

void
Shadow::render(Vec2 light, Vec2 sphere, float size)
{
    // Distance-based attenuation of shadow intensity
    float attenuation_start = 400.0;
    float attenuation_end = 800.0;

    float opacity = 1.0;
    float distance = (sphere - light).length();
    if (distance > attenuation_end) {
        // If it's too far away, we do't need to render anything
        return;
    } else if (distance > attenuation_start) {
        opacity = 1.0 - (distance - attenuation_start) / (attenuation_end - attenuation_start);
    }

    Vec2 delta = (sphere - light).normalize() * size;
    delta = Vec2(delta.y, -delta.x); // rotate 90 degrees
    Vec2 a = sphere + delta;
    Vec2 b = sphere - delta;
    Vec2 c = light + (a - light) * Constants::SHADOW_LENGTH;
    Vec2 d = light + (b - light) * Constants::SHADOW_LENGTH;

    float vertices[] = {
        a.x, a.y, opacity,
        b.x, b.y, opacity,
        c.x, c.y, 0.0,
        d.x, d.y, 0.0,
    };
    vertex_accumulator.append_triangle_strip(vertices, 4);
}

void
Shadow::flush()
{
    activate();

    int count = vertex_accumulator.upload(&vertex_buffer);
    glDrawArrays(GL_TRIANGLES, 0, count);

    vertex_accumulator.clear();
}

