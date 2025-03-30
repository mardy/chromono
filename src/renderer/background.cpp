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


#include "background.h"

#include "opengl_renderer.h"
#include "resources.h"
#include "util.h"

Background::Background(OpenGLRenderer *renderer)
    : Renderable()
    , renderer(renderer)
    , program(SHADER_PROGRAM(renderer, background))
    , vertex_buffer()
    , random_pattern()
    , template_program(SHADER_PROGRAM(renderer, backgroundtemplate))
    , template_framebuffer(renderer, renderer->m_width, renderer->m_height)
    , template_rendering(true)
    , vtxcoord_loc(-1)
    , screensize_loc(-1)
    , randomsize_loc(-1)
    , color_loc(-1)
{
    prepare();
}

void
Background::prepare()
{
    float vertices[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1,
    };
    vertex_buffer.data(vertices, sizeof(vertices), true);

    // First, render template
    vtxcoord_loc = template_program.attrib("vtxcoord");
    screensize_loc = template_program.uniform("screensize");
    randomsize_loc = template_program.uniform("randomsize");

    int random_size = 128;
    unsigned char data[3*random_size*random_size];
    Random random(123, 456);
    for (size_t i=0; i<sizeof(data)/3; i++) {
        // In backgroundtemplate, we only read the red channel,
        // so no need to set green and blue channels as well
        data[i*3] = /*data[i*3+1] = data[i*3+2] = */random.next() % 256;
    }
    random_pattern.setrgb(random_size, random_size, data);

    template_framebuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    activate();
    glUniform2f(screensize_loc, renderer->m_width, renderer->m_height);
    glUniform2f(randomsize_loc, random_size, random_size);
    render(RGB());
    deactivate();
    template_framebuffer.unbind();
    template_rendering = false;

    //template_framebuffer.texture()->save("background.rgb");

    // Next, set up normal rendering
    vtxcoord_loc = program.attrib("vtxcoord");
    screensize_loc = program.uniform("screensize");
    color_loc = program.uniform("color");
    program.bind();
    glUniform2f(screensize_loc, template_framebuffer.texture()->m_subwidth,
            template_framebuffer.texture()->m_subheight);
    program.unbind();
}

Background::~Background()
{
    deactivate();
}

void
Background::bind()
{
    if (template_rendering) {
        template_program.bind();
        random_pattern.bind();
    } else {
        program.bind();
        template_framebuffer.texture()->bind();
    }

    vertex_buffer.bind();
    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void
Background::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    vertex_buffer.unbind();

    if (template_rendering) {
        random_pattern.unbind();
        program.unbind();
    } else {
        template_framebuffer.texture()->unbind();
        template_program.unbind();
    }
}

void
Background::render(RGB color)
{
    activate();

    if (!template_rendering) {
        glUniform4f(color_loc, color.r, color.g, color.b, 1.0);
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

