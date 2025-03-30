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


#include "effectoverlay.h"

#include "opengl_renderer.h"
#include "texture.h"

#include "resources.h"

EffectOverlay::EffectOverlay(OpenGLRenderer *renderer)
    : Renderable()
    , renderer(renderer)
    , program(SHADER_PROGRAM(renderer, effectoverlay))
    , vertex_buffer()
    , template_framebuffer(renderer, renderer->m_width, renderer->m_height)
    , template_program(SHADER_PROGRAM(renderer, effectoverlaytemplate))
    , template_rendering(true)
    , vtxcoord_loc(-1)
{
    prepare();
}

void
EffectOverlay::prepare()
{
    float vertices[] = {
        0, 0,
        1, 0,
        0, 1,
        1, 1,
    };
    vertex_buffer.data(vertices, sizeof(vertices), true);

    // Prepare the template program
    vtxcoord_loc = template_program.attrib("vtxcoord");
    template_program.bind();
    glUniform2f(template_program.uniform("subtex"),
            template_framebuffer.texture()->m_subwidth,
            template_framebuffer.texture()->m_subheight);
    glUniform1i(template_program.uniform("portrait"),
            renderer->m_width < renderer->m_height);
    template_program.unbind();

    template_framebuffer.bind();
    activate();
    render();
    deactivate();
    template_framebuffer.unbind();
    template_rendering = false;

    // Prepare the normal program
    vtxcoord_loc = program.attrib("vtxcoord");
    program.bind();
    glUniform2f(program.uniform("subtex"),
            template_framebuffer.texture()->m_subwidth,
            template_framebuffer.texture()->m_subheight);
    program.unbind();
}

EffectOverlay::~EffectOverlay()
{
    deactivate();
}

void
EffectOverlay::bind()
{
    if (template_rendering) {
        template_program.bind();
    } else {
        program.bind();
        template_framebuffer.texture()->bind();
    }

    vertex_buffer.bind();
    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void
EffectOverlay::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    vertex_buffer.unbind();

    if (!template_rendering) {
        template_framebuffer.texture()->unbind();
    }
}

void
EffectOverlay::render()
{
    activate();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
