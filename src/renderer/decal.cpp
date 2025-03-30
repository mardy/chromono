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


#include "decal.h"

#include "opengl_renderer.h"
#include "resources.h"
#include "resources_util.h"

#include "circle1d.h"

static struct { int w, h; } decal_texture_size = { 1024, 512 };
static struct { int x, y, w, h; } decal_map[Circle1D::DECALS_COUNT] = {
 /* INTRO_ARROW */ {0, 202, 240, 131},
 /* UNTANGLE */ {0, 0, 300, 202},
 /* ON_OFF */ {396, 202, 150, 71},
 /* COLOR_LEFT */ {300, 119, 212, 69},
 /* COLOR_RIGHT */ {240, 202, 156, 73},
 /* PENDULUM */ {300, 0, 333, 119},
 /* EASTEREGG */ {633, 0, 250, 119},
};

Decal::Decal(OpenGLRenderer *renderer)
    : Renderable()
    , program(SHADER_PROGRAM(renderer, decal))
    , vertex_buffer()
    , texture()
    , vtxcoord_loc(-1)
    , texcoord_loc(-1)
    , opacity_loc(-1)
{
    prepare();
}

void
Decal::prepare()
{
    ResourceAccess rgb(RESOURCE(decals_rgb));
    texture.setalpha(decal_texture_size.w, decal_texture_size.h, rgb.data());

    vtxcoord_loc = program.attrib("vtxcoord");
    texcoord_loc = program.attrib("texcoord");
    opacity_loc = program.uniform("opacity");
}

Decal::~Decal()
{
    deactivate();
}

void
Decal::bind()
{
    program.bind();
    texture.bind();

    vertex_buffer.bind();

    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(texcoord_loc);
    glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0,
            (void*)(2*4*sizeof(float)));
}

void
Decal::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    glDisableVertexAttribArray(texcoord_loc);

    vertex_buffer.unbind();

    texture.unbind();
    program.unbind();
}

void
Decal::render(enum Circle1D::Decal decal, float x, float y,
        float rotation, float scale, float opacity)
{
    activate();

    float tx = (float)(decal_map[decal].x) / (float)(decal_texture_size.w);
    float ty = (float)(decal_map[decal].y) / (float)(decal_texture_size.h);
    float tw = (float)(decal_map[decal].w) / (float)(decal_texture_size.w);
    float th = (float)(decal_map[decal].h) / (float)(decal_texture_size.h);

    float vw = decal_map[decal].w * scale;
    float vh = decal_map[decal].h * scale;
    float vx = x - vw / 2.;
    float vy = y - vh / 2.;

    float vertices[] = {
        /* vtxcoords */
        vx, vy,
        vx+vw, vy,
        vx, vy+vh,
        vx+vw, vy+vh,

        /* texcoord */
        tx, ty,
        tx+tw, ty,
        tx, ty+th,
        tx+tw, ty+th,
    };
    Vec2(x, y).rotate_around(rotation, vertices, 4);

    vertex_buffer.data(vertices, sizeof(vertices));

    glUniform1f(opacity_loc, opacity);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

