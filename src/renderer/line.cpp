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


#include "line.h"

#include "resources.h"

#include <algorithm>

Line::Line(OpenGLRenderer *renderer)
    : Renderable()
    , program(SHADER_PROGRAM(renderer, line))
    , vertex_buffer()
    , vertex_accumulator()
    , vtxcoord_loc(-1)
    , color_loc(-1)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    color_loc = program.attrib("color");

    vertex_accumulator.add_attrib(vtxcoord_loc, 2);
    vertex_accumulator.add_attrib(color_loc, 4);
}

void
Line::bind()
{
    program.bind();

    vertex_buffer.bind();
    vertex_accumulator.enable_arrays();
}

void
Line::unbind()
{
    vertex_accumulator.disable_arrays();
    vertex_buffer.unbind();

    program.unbind();
}

void
Line::render(Joint *j)
{
    float x1 = j->a->pos.x;
    float y1 = j->a->pos.y;
    float x2 = j->b->pos.x;
    float y2 = j->b->pos.y;
    float ra = j->a->size;
    float rb = j->b->size;

    Vec2 a(x1, y1), b(x2, y2);
    Vec2 a_to_b = b - a;

    // Move points closer to each other (depending on the radius of the
    // adjacent object), but let them overlap with the object a little
    float overlap = 2.0;
    if (a_to_b.length() < (ra + rb - overlap)) {
        // Spheres are overlapping - don't draw line
        return;
    }

    a_to_b = a_to_b.normalize();
    a += a_to_b * (ra - overlap);
    b -= a_to_b * (rb - overlap);

    RGB color(1.0, 1.0, 1.0);
    if (j->flags & Joint::UNTANGLE) {
        // Untangle objects re-use the knot color for indicator color
        color = j->knot_color;
    } else if (j->flags & Joint::IS_RAIL) {
        color = RGB(0.3, 0.3, 0.3);
    }

    float width = 3.8;
    float border = 1.0;
    Vec2 perp(a_to_b.y, -a_to_b.x);

    Vec2 v[8];
    for (int i=0; i<8; i++) {
        if (i % 2 == 0) {
            // A side
            v[i] = a;
        } else {
            // B side
            v[i] = b;
        }

        float distance = width / 2.0;
        if (i < 2 || i > 5) {
            // Outside (including border)
            distance += border;
        }

        if (i < 4) {
            // Top
            v[i] += perp * distance;
        } else {
            // Bottom
            v[i] -= perp * distance;
        }
    }

    float vertices[] = {
        v[0].x, v[0].y, color.r, color.g, color.b, 0.0,
        v[1].x, v[1].y, color.r, color.g, color.b, 0.0,
        v[2].x, v[2].y, color.r, color.g, color.b, 1.0,
        v[3].x, v[3].y, color.r, color.g, color.b, 1.0,
        v[4].x, v[4].y, color.r, color.g, color.b, 1.0,
        v[5].x, v[5].y, color.r, color.g, color.b, 1.0,
        v[6].x, v[6].y, color.r, color.g, color.b, 0.0,
        v[7].x, v[7].y, color.r, color.g, color.b, 0.0,
    };
    vertex_accumulator.append_triangle_strip(vertices, 8);
}

void
Line::flush()
{
    activate();
    int count = vertex_accumulator.upload(&vertex_buffer);
    glDrawArrays(GL_TRIANGLES, 0, count);
    vertex_accumulator.clear();
}

Line::~Line()
{
    deactivate();
}

