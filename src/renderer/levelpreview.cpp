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


#include "levelpreview.h"

#include "resources.h"
#include "opengl_renderer.h"
#include "platform.h"

LevelPreview::LevelPreview(OpenGLRenderer *renderer)
    : Renderable()
    , renderer(renderer)
    , program(SHADER_PROGRAM(renderer, levelpreview))
    , vertex_buffer()
    , aspect((float)(renderer->m_width) /
            (float)(renderer->m_height))
    , vtxcoord_loc(-1)
    , texcoord_loc(-1)
{
    /* Aspect ratio must always be >= 1 (portrait->landscape rotation) */
#if 0
    if (aspect < 1.f) {
        aspect = 1.f / aspect;
    }
#endif

    vtxcoord_loc = program.attrib("vtxcoord");
    texcoord_loc = program.attrib("texcoord");
}

LevelPreview::~LevelPreview()
{
    deactivate();
}

Framebuffer *
LevelPreview::generate(level_func level)
{
    int side = 256;
    Framebuffer *fb = new Framebuffer(renderer, side, side);

    // Render level
    Scene scene;
    level(&scene);
    scene.simulate(60); // preroll

    fb->bind();

    renderer->start_offscreen();
    renderer->begin();
    scene.render(renderer);
    renderer->finish();
    renderer->end_offscreen();

    fb->unbind();
    return fb;
}

void
LevelPreview::bind()
{
    program.bind();
    vertex_buffer.bind();
    glEnableVertexAttribArray(vtxcoord_loc);
    glEnableVertexAttribArray(texcoord_loc);
}

void
LevelPreview::render(Framebuffer *framebuffer, int x, int y, int w, int h)
{
    activate();

    framebuffer->texture()->bind();

    Texture *texture = framebuffer->texture();

    // part of the picture (< 1.0 means zoom)
    float part = 0.9;

    float height = part;
    float width = part / aspect * ((float)(w) / (float)(h));

    float xx = (1.0 - width) / 2.0;
    float yy = (1.0 - height) / 2.0;

    float x1 = xx * texture->m_subwidth;
    float y1 = yy * texture->m_subheight;
    float x2 = (1.0 - xx) * texture->m_subwidth;
    float y2 = (1.0 - yy) * texture->m_subheight;

    float vertices[] = {
        // Vertex coordinates
        float(x), float(y),
        float(x+w), float(y),
        float(x), float(y+h),
        float(x+w), float(y+h),

        // Texture coordinates
        x1, y2,
        x2, y2,
        x1, y1,
        x2, y1,
    };
    vertex_buffer.data(vertices, sizeof(vertices));
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*2*4));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    framebuffer->texture()->unbind();
}

void
LevelPreview::unbind()
{
    glDisableVertexAttribArray(texcoord_loc);
    glDisableVertexAttribArray(vtxcoord_loc);
    vertex_buffer.unbind();
    program.unbind();
}

