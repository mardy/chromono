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


#include "framebuffer.h"

#include "opengl_renderer.h"

Framebuffer::Framebuffer(OpenGLRenderer *renderer, int width, int height, float scale)
    : m_framebuffer_id(-1),
      m_width(width),
      m_height(height),
      m_scale(scale),
      m_oldFB0(0),
      m_texture(),
      m_renderer(renderer)
{
    // iOS: http://stackoverflow.com/questions/11617013/
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFB0);

    m_texture.resize(width*scale, height*scale);
    glGenFramebuffers(1, &m_framebuffer_id);
    SHADY_ASSERT(m_framebuffer_id != (GLuint)-1);

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, m_texture.id(), 0);
    unbind();
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_framebuffer_id);
}

void
Framebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id);
    glViewport(0, 0, m_width*m_scale, m_height*m_scale);
}

void
Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_oldFB0);
    glViewport(0, 0, m_renderer->m_width, m_renderer->m_height);
}

