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


#include "sphere.h"

#include "opengl_renderer.h"
#include "renderable.h"
#include "shaderprogram.h"
#include "resources.h"
#include "vertexaccumulator.h"

#include "spheretemplate.h"

struct SphereRenderInfo {
    Vec2 pos;
    float size;
    RGB color;
    RGB desired;
};

#define SPHERE_RENDER_QUEUE_MAX 64

class SphereRenderQueue {
    public:
        SphereRenderQueue();

        SphereRenderInfo queue_default[SPHERE_RENDER_QUEUE_MAX];
        int count_default;
};

SphereRenderQueue::SphereRenderQueue()
    : count_default(0)
{
}



class SphereDefault : public Renderable {
    public:
        SphereDefault(OpenGLRenderer *renderer, Texture *texture);
        ~SphereDefault();

        virtual void bind();
        virtual void unbind();

        void render(SphereRenderInfo *spheres, int count);

    private:
        Program program;
        VertexBuffer vertex_buffer;
        VertexAccumulator vertex_accumulator;
        Texture *texture;

        GLint vtxcoord_loc;
        GLint texcoord_loc;
        GLint color_loc;
        GLint desired_loc;
};

SphereDefault::SphereDefault(OpenGLRenderer *renderer, Texture *texture)
    : program(SHADER_PROGRAM(renderer, spheredefault))
    , vertex_buffer()
    , vertex_accumulator()
    , texture(texture)
    , vtxcoord_loc(-1)
    , texcoord_loc(-1)
    , color_loc(-1)
    , desired_loc(-1)
{
    vtxcoord_loc = program.attrib("vtxcoord");
    texcoord_loc = program.attrib("texcoord");
    color_loc = program.attrib("color");
    desired_loc = program.attrib("desired");

    vertex_accumulator.add_attrib(vtxcoord_loc, 2);
    vertex_accumulator.add_attrib(texcoord_loc, 2);
    vertex_accumulator.add_attrib(color_loc, 3);
    vertex_accumulator.add_attrib(desired_loc, 3);
}

SphereDefault::~SphereDefault()
{
    deactivate();
}

void
SphereDefault::bind()
{
    texture->bind();
    program.bind();

    vertex_buffer.bind();
    vertex_accumulator.enable_arrays();
}

void
SphereDefault::unbind()
{
    vertex_accumulator.disable_arrays();
    vertex_buffer.unbind();

    program.unbind();
    texture->unbind();
}

void
SphereDefault::render(SphereRenderInfo *spheres, int count)
{
    activate();

    vertex_accumulator.clear();

    for (int i=0; i<count; i++) {
        SphereRenderInfo *info = &(spheres[i]);

        float tscale, toffset;
        SphereTemplate::get_transform(info->size, &tscale, &toffset);

        Vec2 v[4];
        Vec2 t[4];
        for (int j=0; j<4; j++) {
            float x = (j % 2 == 0) ? -1 : +1;
            float y = (j < 2) ? -1 : +1;

            v[j] = info->pos + Vec2(x, y) * info->size;
            t[j] = Vec2(0.5, 0.5) + Vec2(x, y) * 0.5;
            // here, tex is relative to the entire texture
            t[j].x /= 2.0;
            // now, tex is relative to the left half of the texture
            // corresponds to biggest image - 128px radius, 256x256 pixels

            // This choses the right-sized texture (calculated in sphere.cpp)
            t[j] /= tscale;
            t[j] += Vec2(toffset, 0.f);
        }

        RGB c = info->color;
        RGB d = info->desired;

        float data[] = {
            v[0].x, v[0].y, t[0].x, t[0].y, c.r, c.g, c.b, d.r, d.g, d.b,
            v[1].x, v[1].y, t[1].x, t[1].y, c.r, c.g, c.b, d.r, d.g, d.b,
            v[2].x, v[2].y, t[2].x, t[2].y, c.r, c.g, c.b, d.r, d.g, d.b,
            v[3].x, v[3].y, t[3].x, t[3].y, c.r, c.g, c.b, d.r, d.g, d.b,
        };
        vertex_accumulator.append_triangle_strip(data, 4);
    }

    int elements = vertex_accumulator.upload(&vertex_buffer);

    glDrawArrays(GL_TRIANGLES, 0, elements);
}

Sphere::Sphere(OpenGLRenderer *renderer)
    : template_framebuffer(renderer, 512, 256)
    , render_queue(new SphereRenderQueue)
    , sphere_default(new SphereDefault(renderer, template_framebuffer.texture()))
{
    SphereTemplate *tmpl = new SphereTemplate(renderer);
    tmpl->render_to_texture(&template_framebuffer);
    delete tmpl;
}

void
Sphere::render(Object *o)
{
    if (render_queue->count_default == SPHERE_RENDER_QUEUE_MAX - 1) {
        flush();
    }

    SphereRenderInfo *info = &(render_queue->queue_default[render_queue->count_default++]);
    info->pos = o->pos;
    info->size = o->size;
    if (o->flags & Object::COLORABLE) {
        info->color = o->color;
        info->desired = o->desired;
    } else {
        info->color = info->desired = o->color;
    }
}

void
Sphere::flush()
{
    sphere_default->render(render_queue->queue_default, render_queue->count_default);
    render_queue->count_default = 0;
}

Sphere::~Sphere()
{
    delete sphere_default;

    delete render_queue;
}

