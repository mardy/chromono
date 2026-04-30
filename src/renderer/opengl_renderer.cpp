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


#include "opengl_renderer.h"
#include "constants.h"
#include "util.h"

#include "game.h"

#include <string.h>

// Should be in line with "enum FontSize" in circle1d/renderer.h
static const float FONT_SIZE_FACTORS[] = {
    0.5,
    0.75,
    0.9,
    1.0,
    0.3,
};

class LateInitializeTask : public LoadingTask {
    public:
        LateInitializeTask(OpenGLRenderer *renderer, int phase)
            : LoadingTask()
            , renderer(renderer)
            , phase(phase)
        {
        }

        virtual void
        loading_task_run()
        {
            renderer->late_initialize(phase);
        }

    private:
        OpenGLRenderer *renderer;
        int phase;
};

class LevelLoadingTask : public LoadingTask {
    public:
        LevelLoadingTask(LevelPreview **level_preview, LevelManager *level_manager,
                std::vector<Framebuffer*> *previews, int level)
            : LoadingTask()
            , level_preview(level_preview)
            , level_manager(level_manager)
            , previews(previews)
            , level(level)
        {
        }

        virtual void
        loading_task_run()
        {
            level_func lv = level_manager->get(level)->constructor;
            Framebuffer *fb = (*level_preview)->generate(lv);
            previews->push_back(fb);
        }

    private:
        LevelPreview **level_preview;
        LevelManager *level_manager;
        std::vector<Framebuffer*> *previews;
        int level;
};

OpenGLRenderer::OpenGLRenderer(Game *game, int width, int height, int rotation_degrees)
    : m_game(game)
    , m_rotation_degrees(rotation_degrees)
    , m_projection(width, height)
    , m_width(width)
    , m_height(height)
    , m_programs()
    , m_programs_time()
    , m_projection_mat(m_projection.matrix(m_rotation_degrees))
    , m_loading_tasks()
    , m_loading_tasks_total(0)
    , m_loading(this)
    , m_sphere(NULL)
    , m_line(NULL)
    , m_effect(NULL)
    , m_font(NULL)
    , m_page_indicator(NULL)
    , m_effect_overlay(NULL)
    , m_background(NULL)
    , m_icons(NULL)
    , m_decal(NULL)
    , m_shadow(NULL)
    , m_level_preview(NULL)
    , m_cached_screen(NULL)
    , m_previews()
    , m_rendered_texts()
    , m_cached()
    , m_cached_current_fb(NULL)
    , m_is_offscreen(false)
    , m_old_rotation_degrees(0)
{
    game->set_offset(Vec2(m_projection.offset().x, m_projection.offset().y));

    /* Enqueue late-initialization tasks */
    for (int i=0; i<=11; i++) {
        m_loading_tasks.push_back(new LateInitializeTask(this, i));
    }
    m_loading_tasks_total = m_loading_tasks.size();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setup_viewport();
    glClearColor(0., 0., 0., 0.);
}

void OpenGLRenderer::setup_viewport()
{
    if (m_rotation_degrees == 0 || m_rotation_degrees == 180) {
        glViewport(0, 0, m_width, m_height);
    } else {
        glViewport(0, 0, m_height, m_width);
    }
}

OpenGLRenderer::~OpenGLRenderer()
{
    Renderable::unbind_active();
    Program::unbind_active();

    std::vector<Framebuffer*>::iterator it;
    for (it=m_previews.begin(); it != m_previews.end(); ++it) {
        delete (*it);
    }

    std::list<RenderedText*>::iterator it2;
    for (it2=m_rendered_texts.begin(); it2 != m_rendered_texts.end(); ++it2) {
        delete (*it2);
    }

    delete m_cached_screen;
    delete m_level_preview;
    delete m_shadow;
    delete m_decal;
    delete m_icons;
    delete m_background;
    delete m_effect_overlay;
    delete m_page_indicator;
    delete m_font;
    delete m_effect;
    delete m_line;
    delete m_sphere;
}

void
OpenGLRenderer::late_initialize(int phase)
{
    switch (phase) {
        case  0: m_sphere = new Sphere(this); break;
        case  1: m_line = new Line(this); break;
        case  2: m_effect = new Effect(this); break;
        case  3: m_font = new Font(this); break;
        case  4: m_page_indicator = new PageIndicator(this); break;
        case  5: m_effect_overlay = new EffectOverlay(this); break;
        case  6: m_background = new Background(this); break;
        case  7: m_icons = new Icons(this); break;
        case  8: m_decal = new Decal(this); break;
        case  9: m_shadow = new Shadow(this); break;
        case 10: m_level_preview = new LevelPreview(this); break;
        case 11: m_cached_screen = new CachedScreen(this); break;
        default: SHADY_ASSERT(0); break;
    }
}

bool
OpenGLRenderer::ready()
{
    if (Constants::RENDERED_LEVEL_PREVIEWS && !m_previews.size()) {
        LevelManager *level_manager = m_game->get_level_manager();
        int levels = level_manager->count();
        for (int i=0; i<levels; i++) {
            m_loading_tasks.push_back(new LevelLoadingTask(&m_level_preview,
                        level_manager, &m_previews, i));
        }
        m_loading_tasks_total = m_loading_tasks.size();
    }

    if (m_loading_tasks_total) {
        int tasks_left = 5;

        // Process at most tasks_left loading tasks per iteration
        while (tasks_left > 0 && m_loading_tasks.size()) {
            LoadingTask *task = m_loading_tasks.front();
            m_loading_tasks.pop_front();
            task->loading_task_run();
            delete task;

            tasks_left--;
        }


        // Display current loading progress on-screen
        glClear(GL_COLOR_BUFFER_BIT);
        m_loading.render(1.0 - (float)(m_loading_tasks.size()) /
                (float)(m_loading_tasks_total));

        if (!m_loading_tasks.size()) {
            m_loading_tasks_total = 0;
        }

        return false;
    }

    return true;
}

void
OpenGLRenderer::begin()
{
    /* Update time uniform float in all shader programs */
    float time_now = (float)(Util::ticks())/1000.f;
    std::list< std::pair<Program*,GLint> >::iterator it;
    for (it=m_programs_time.begin(); it != m_programs_time.end(); ++it) {
        Program *program = (*it).first;
        GLint time_loc = (*it).second;

        program->bind();
        glUniform1f(time_loc, time_now);
    }
    Program::unbind_active();

    glClear(GL_COLOR_BUFFER_BIT);
}

void
OpenGLRenderer::background(RGB color)
{
    m_background->render(color);
}

void
OpenGLRenderer::circle(Object *o)
{
    m_sphere->render(o);
}

void
OpenGLRenderer::flush_circles()
{
    m_sphere->flush();
}

void
OpenGLRenderer::line(Joint *j)
{
    m_line->render(j);
}

void
OpenGLRenderer::flush_lines()
{
    m_line->flush();
}

void
OpenGLRenderer::overlay()
{
    m_effect_overlay->render();
}

void
OpenGLRenderer::transition(float value)
{
    m_effect->render(value);
}

void
OpenGLRenderer::finish()
{
    // After finishing rendering, clean up by
    // unbinding all active renderables
    Renderable::unbind_active();
}

void
OpenGLRenderer::page_indicator(int x, int y, int page, int pages)
{
    m_page_indicator->render(x, y, page, pages);
}

void
OpenGLRenderer::icon(enum Icons::Icon icon, int x, int y, int w, int h, RGB color, float opacity, float rotation)
{
    m_icons->render(icon, x, y, w, h, color, opacity, rotation);
}

RenderedText *
OpenGLRenderer::get_rendered_text(const char *text)
{
    std::list<RenderedText*>::iterator it;
    for (it=m_rendered_texts.begin(); it != m_rendered_texts.end(); ++it) {
        RenderedText *rendered_text = *it;
        if (strcmp(text, rendered_text->m_text.c_str()) == 0) {
            rendered_text->m_age = 0;
            return rendered_text;
        }
    }

    RenderedText *result = new RenderedText(m_font, text);
    m_rendered_texts.push_back(result);
    return result;
}

void
OpenGLRenderer::text_measure(const char *text, float *width, float *height, enum FontSize size)
{
    if (*text == '\0') {
        *width = *height = 0;
        return;
    }

    RenderedText *rendered_text = get_rendered_text(text);
    *width = (float)rendered_text->m_width * FONT_SIZE_FACTORS[size];
    *height = (float)rendered_text->m_height * FONT_SIZE_FACTORS[size];
}

void
OpenGLRenderer::text_render(const char *text, float x, float y, enum FontSize size, float opacity, RGB color)
{
    if (*text == '\0') {
        return;
    }

    RenderedText *rendered_text = get_rendered_text(text);
    m_font->render(rendered_text, x, y, FONT_SIZE_FACTORS[size], opacity, color);
}

void
OpenGLRenderer::text_gc()
{
    // Free cached rendered text that hasn't been used for a while

    std::list<RenderedText*>::iterator it;
    for (it=m_rendered_texts.begin(); it != m_rendered_texts.end(); ++it) {
        RenderedText *rendered_text = *it;
        rendered_text->m_age++;
        if (rendered_text->m_age == Constants::RENDERED_TEXT_CACHE_GC_FRAMES) {
            delete rendered_text;
            it = m_rendered_texts.erase(it);
        }
    }
}

void
OpenGLRenderer::decal(int decal, int x, int y, float opacity)
{
    m_decal->render((enum Circle1D::Decal)decal, x, y, 0.0, 1.0, opacity);
}

void
OpenGLRenderer::decal(int decal, float x, float y, float rotation, float scale, float opacity)
{
    m_decal->render((enum Circle1D::Decal)decal, x, y, rotation, scale, opacity);
}

void
OpenGLRenderer::shadow(Vec2 light, Vec2 sphere, float size)
{
    m_shadow->render(light, sphere, size);
}

void
OpenGLRenderer::flush_shadows()
{
    m_shadow->flush();
}


void
OpenGLRenderer::register_program(Program *program)
{
    m_programs.push_back(program);

    program->bind();

    /* Projection matrix */
    GLint projection_loc = program->uniform("projection");
    if (projection_loc != -1) {
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, m_projection_mat.m);
    }

    /* Some programs need time for animation */
    GLint time_loc = program->uniform("time");
    if (time_loc != -1) {
        m_programs_time.push_back(std::make_pair(program, time_loc));
    }

    program->unbind();
}

void
OpenGLRenderer::set_rotation(int rotation_degrees)
{
    if (m_rotation_degrees != rotation_degrees) {
        std::list<Program*>::iterator it;

        m_projection_mat = m_projection.matrix(rotation_degrees);
        for (it=m_programs.begin(); it != m_programs.end(); ++it) {
            Program *program = *it;

            /* Projection matrix (for texture rendering, without rotation) */
            GLint projection_loc = program->uniform("projection");
            if (projection_loc != -1) {
                program->bind();
                glUniformMatrix4fv(projection_loc, 1, GL_FALSE, m_projection_mat.m);
                program->unbind();
            }
        }

        m_rotation_degrees = rotation_degrees;
    }
}

void
OpenGLRenderer::unregister_program(Program *program)
{
    m_programs.remove(program);

    std::list< std::pair<Program*,GLint> >::iterator it;
    for (it=m_programs_time.begin(); it != m_programs_time.end(); ++it) {
        Program *it_program = (*it).first;
        if (program == it_program) {
            it = m_programs_time.erase(it);
        }
    }
}

void
OpenGLRenderer::level_preview(int level, float x, float y, float w, float h)
{
    if (Constants::RENDERED_LEVEL_PREVIEWS) {
        m_level_preview->render(m_previews[level], x, y, w, h);
    }
}

void
OpenGLRenderer::prepare_cached(void *handle, size_t len,
        render_cached_func_t render_cached, void *user_data)
{
    if (begin_cached(handle, len)) {
        render_cached(this, handle, user_data);
        end_cached();
    }
}

bool
OpenGLRenderer::begin_cached(void *handle, size_t len)
{
    std::list< std::pair<ChangeWatch*,Framebuffer*> >::iterator it;
    for (it=m_cached.begin(); it != m_cached.end(); ++it) {
        ChangeWatch *watch = (*it).first;
        if (watch->is(handle, len)) {
            if (watch->needs_update()) {
                m_cached_current_fb = (*it).second;

                m_cached_current_fb->bind();
                start_offscreen(false);
                begin();
                return true;
            } else {
                return false;
            }
        }
    }

    m_cached_current_fb = new Framebuffer(this, m_width, m_height);
    m_cached.push_back(std::pair<ChangeWatch*,Framebuffer*>(new ChangeWatch(handle, len), m_cached_current_fb));

    m_cached_current_fb->bind();
    start_offscreen(false);
    begin();
    return true;
}

void
OpenGLRenderer::end_cached()
{
    finish();
    end_offscreen();
    m_cached_current_fb->unbind();

    m_cached_current_fb = NULL;
}

void
OpenGLRenderer::draw_cached(void *handle, size_t len, Vec2 offset_world)
{
    std::list< std::pair<ChangeWatch*,Framebuffer*> >::iterator it;
    for (it=m_cached.begin(); it != m_cached.end(); ++it) {
        ChangeWatch *watch = (*it).first;
        if (watch->is(handle, len)) {
            Framebuffer *fb = (*it).second;

            offset_world.x /= Constants::WORLD_WIDTH;
            offset_world.y /= Constants::WORLD_HEIGHT;

            m_cached_screen->render(fb->texture(), m_rotation_degrees, offset_world);
            return;
        }
    }

    SHADY_DEBUG_PRINTF("draw_cached called on unknown handle: %p\n", handle);
}

