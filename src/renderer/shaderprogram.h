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

#ifndef SHADYPOSTPROC_SHADERPROGRAM_H
#define SHADYPOSTPROC_SHADERPROGRAM_H

#include "shadypostproc.h"
#include "chromono_opengl.h"
#include "resources_util.h"

class OpenGLRenderer;

class Shader {
    public:
        enum Type {
            VERTEX, FRAGMENT,
        };

        Shader(enum Type type, Resource *resource);
        virtual ~Shader();

    private:
        GLuint m_id;
        friend class Program;
};

#define SHADER_PROGRAM(renderer, base) \
    Program(renderer, #base, \
            new Shader(Shader::VERTEX, RESOURCE(base ## _vsh)), \
            new Shader(Shader::FRAGMENT, RESOURCE(base ## _fsh)))

class Program {
    public:
        Program(OpenGLRenderer *renderer, const char *name, Shader *fragment, Shader *vertex);
        virtual ~Program();

        void bind();
        void unbind();

        static void unbind_active() {
            if (active != NULL) {
                active->unbind();
                active = NULL;
            }
        }

        GLint
        uniform(const char *name);

        GLint
        attrib(const char *name);

        const char *
        name() { return m_name; }

    private:
        static Program *active;

        GLuint m_id;
        const char *m_name;
        OpenGLRenderer *m_renderer;
        Shader *m_fragment;
        Shader *m_vertex;
};

#endif /* SHADYPOSTPROC_SHADERPROGRAM_H */
