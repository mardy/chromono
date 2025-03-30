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


#include "effect.h"

#include "platform.h"
#include "opengl_renderer.h"

#include "resources.h"

Effect::Effect(OpenGLRenderer *renderer)
    : program(SHADER_PROGRAM(renderer, effect))
    , vertex_buffer()
    , framebuffer()
    , coord_loc(-1)
    , subtex_loc(-1)
    , size_loc(-1)
    , brightness_loc(-1)
{
    framebuffer[0] = new Framebuffer(renderer, renderer->m_width, renderer->m_height);
    framebuffer[1] = new Framebuffer(renderer, renderer->m_width, renderer->m_height);

    coord_loc = program.attrib("coord");
    subtex_loc = program.uniform("subtex");
    size_loc = program.uniform("size");
    brightness_loc = program.uniform("brightness");

    program.bind();

    glUniform2f(size_loc, (float)renderer->m_width, (float)renderer->m_height);
    glUniform1f(brightness_loc, 0.0);
    glUniform1i(program.uniform("portrait"), renderer->m_width < renderer->m_height);
    glUniform1i(program.uniform("frame_a"), 0);
    glUniform1i(program.uniform("frame_b"), 1);

    Texture *texture = framebuffer[0]->texture();
    glUniform2f(subtex_loc, texture->m_subwidth, texture->m_subheight);

    float vertices[] = {
        0, 0,
        1, 0,
        0, 1,
        1, 1,
    };
    vertex_buffer.data(vertices, sizeof(vertices), true);
}

Effect::~Effect()
{
    delete framebuffer[1];
    delete framebuffer[0];

    deactivate();
}

void
Effect::begin_capture(enum TransitionFrame frame)
{
    framebuffer[frame]->bind();
}

void
Effect::end_capture(enum TransitionFrame frame)
{
    framebuffer[frame]->unbind();
}

void
Effect::bind()
{
    program.bind();

    vertex_buffer.bind();
    glEnableVertexAttribArray(coord_loc);
    glVertexAttribPointer(coord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

    framebuffer[0]->texture()->bind();
    framebuffer[1]->texture()->bind(1);
}

void
Effect::unbind()
{
    framebuffer[0]->texture()->unbind();
    framebuffer[1]->texture()->unbind(1);

    glDisableVertexAttribArray(coord_loc);
    vertex_buffer.unbind();

    program.unbind();
}

void
Effect::render(float value)
{
    activate();

    glUniform1f(brightness_loc, value);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

