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


#include "spheretemplate.h"

#include "opengl_renderer.h"
#include "resources.h"

SphereTemplate::SphereTemplate(OpenGLRenderer *renderer)
    : program(SHADER_PROGRAM(renderer, spheretemplate))
    , vertex_buffer()
    , vtxcoord_loc(-1)
    , center_loc(-1)
    , size_loc(-1)
{
}

SphereTemplate::~SphereTemplate()
{
    deactivate();
}

void
SphereTemplate::render_to_texture(Framebuffer *framebuffer)
{
    float vertices[] = {
        -1, -1,
        -1, +1,
        +1, -1,
        +1, +1,
    };
    vertex_buffer.data(vertices, sizeof(vertices), true);

    vtxcoord_loc = program.attrib("vtxcoord");
    center_loc = program.uniform("center");
    size_loc = program.uniform("size");

    program.bind();
    glUniform2f(program.uniform("framebuffer_size"),
            framebuffer->m_width, framebuffer->m_height);

    // Rendering of the template goes into the framebuffer texture
    framebuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    activate();

    /**
     * Layout of our framebuffer texture:
     *
     * +----------+----------+
     * |          | 64 |32|16|
     * |  radius  | px |     |
     * |  =128px  +----+     |
     * |          |          |
     * |          |          |
     * +----------+----------+
     *
     * y = 0 always
     * 128px radius = 256x256 @ x=0
     * 64px radius = 128x128 @ x=256
     * 32px radius = 64x64 @ x=256 + 128
     * 16px radius = 32x32 @ x=256 + 128 + 64
     *
     * This will be rendered below. The choice which template is used
     * depends on the size of the rendered sphere. The choice is made
     * in sphere's vertex shader (aka sphere.vsh).
     **/

    // Full-size sphere at left half of framebuffer
    render(128, 128, 128);
    // Half-size sphere at right half of framebuffer
    render(256 + 64, 64, 64);
    // Quad-size sphere at right half of right half of framebuffer
    render(256 + 128 + 32, 32, 32);
    // Octo(?)-size sphere at (right half of)*3 framebuffer
    render(256 + 128 + 64 + 16, 16, 16);

    deactivate();
    framebuffer->unbind();

    //framebuffer->texture()->save("sphere.rgb");
}

void
SphereTemplate::get_transform(float size, float *scale, float *offset)
{
    *scale = 1.f;
    *offset = 0.f;

    if (size > 64.0) {
        // Use the biggest image - left half of texture
    } else if (size > 32.0) {
        // Use the half-size image - located on right half
        *scale = 2.0;
        *offset = 0.5;
    } else if (size > 16.0) {
        // Use the quad-size image - located even more to the right
        *scale = 4.0;
        *offset = 0.75;
    } else {
        // Use the octo(?)-size image - located even more to the right ;)
        *scale = 8.0;
        *offset = 0.875;
    }
}

void
SphereTemplate::bind()
{
    program.bind();
    vertex_buffer.bind();
    glEnableVertexAttribArray(vtxcoord_loc);
    glVertexAttribPointer(vtxcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void
SphereTemplate::unbind()
{
    glDisableVertexAttribArray(vtxcoord_loc);
    vertex_buffer.unbind();
    program.unbind();
}

void
SphereTemplate::render(float x, float y, float size)
{
    glUniform2f(center_loc, x, y);
    glUniform1f(size_loc, size);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

