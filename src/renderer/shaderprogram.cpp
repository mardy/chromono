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


#include "shaderprogram.h"
#include "opengl_renderer.h"
#include "resources_util.h"

#include <string.h>

Shader::Shader(enum Shader::Type type, Resource *resource)
    : m_id(glCreateShader((type==Shader::FRAGMENT)?
                (GL_FRAGMENT_SHADER):(GL_VERTEX_SHADER)))
{
    ResourceAccess access(resource);
    std::string source((const char*)access.data(), access.size());

    const char *sources[] = {
#if defined(USE_OPENGL_ES)
        "#version 100\n",
        "precision mediump float;\n",
#endif
        source.c_str(),
    };

    glShaderSource(m_id, sizeof(sources)/sizeof(sources[0]), sources, NULL);

    glCompileShader(m_id);
    GLint success = GL_FALSE;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
#if defined(USE_DEBUG_PRINTF)
        GLint size = 0;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &size);
        ++size;
        char *tmp = new char[size];
        memset(tmp, 0, size);
        glGetShaderInfoLog(m_id, size, NULL, tmp);
        printf("== %s ==\n%s\n", access.name(), tmp);
        delete [] tmp;

        int lineno = 1;

#if defined(USE_OPENGL_ES)
        // For OpenGL ES, we prepend two lines to the shader
        lineno += 2;
#endif
        char *current_line = (char *)sources[(sizeof(sources)/sizeof(sources[0]))-1];
        while (true) {
            char *next = current_line;
            while (*next != '\0' && *next != '\n') {
                next++;
            }
            bool done = (*next == '\0');
            *next = '\0';
            if (strlen(current_line) > 0 || !done) {
                printf("%3d  %s\n", lineno, current_line);
            }
            if (done) {
                break;
            }
            current_line = next + 1;
            lineno++;
        }
        printf("\n");
#endif /* defined(USE_DEBUG_PRINTF) */

        exit(EXIT_FAILURE);
    }
}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

Program *
Program::active = NULL;

Program::Program(OpenGLRenderer *renderer, const char *name,
        Shader *fragment, Shader *vertex)
    : m_id(glCreateProgram())
    , m_name(name)
    , m_renderer(renderer)
    , m_fragment(fragment)
    , m_vertex(vertex)
{
    glAttachShader(m_id, m_fragment->m_id);
    glAttachShader(m_id, m_vertex->m_id);
    glLinkProgram(m_id);
    GLint success = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    SHADY_ASSERT(success == GL_TRUE);

    m_renderer->register_program(this);
}

Program::~Program()
{
    unbind();
    delete m_fragment;
    delete m_vertex;
    glDeleteProgram(m_id);
    m_renderer->unregister_program(this);
}

void
Program::bind()
{
    if (active == this) {
        return;
    }

    glUseProgram(m_id);
    active = this;
}

void
Program::unbind()
{
    if (active != this) {
        return;
    }

    glUseProgram(0);
    active = NULL;
}

GLint
Program::uniform(const char *name)
{
    return glGetUniformLocation(m_id, name);
}

GLint
Program::attrib(const char *name)
{
    return glGetAttribLocation(m_id, name);
}

