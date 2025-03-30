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

#ifndef SHADYPOSTPROC_OPENGL_RENDERER_H
#define SHADYPOSTPROC_OPENGL_RENDERER_H

#include "shadypostproc.h"
#include "chromono_opengl.h"

#include "circle1d.h"

#include "levelmanager.h"

#include "shaderprogram.h"
#include "framebuffer.h"
#include "projection.h"

#include "sphere.h"
#include "line.h"
#include "effect.h"
#include "menu.h"
#include "font.h"
#include "pageindicator.h"
#include "effectoverlay.h"
#include "background.h"
#include "levelpreview.h"
#include "icons.h"
#include "decal.h"
#include "shadow.h"

#include "loading.h"
#include "loadingtask.h"

#include "changewatch.h"
#include "cachedscreen.h"

#include <list>
#include <utility>

class Game;
class OpenGLRenderer;

typedef void (*render_cached_func_t)(OpenGLRenderer *renderer,
        void *prepared, void *user_data);

class OpenGLRenderer : public Circle1DRenderer {
    public:
        OpenGLRenderer(Game *game, int width, int height);
        ~OpenGLRenderer();

        Vec2 screen2world(Vec2 screen) {
            return m_projection.screen2world(screen);
        }

        void late_initialize(int phase); // Don't call this from outside
        bool ready();

        virtual void begin();
        void background(RGB color);
        virtual void circle(Object *o);
        virtual void flush_circles();
        virtual void line(Joint *j);
        virtual void flush_lines();
        virtual void text_measure(const char *text, float *width, float *height, enum FontSize size);
        virtual void text_render(const char *text, float x, float y, enum FontSize size, float opacity=1.0, RGB color=RGB(1.0, 1.0, 1.0));
        virtual void decal(int decal, int x, int y, float opacity=1.0);
        virtual void shadow(Vec2 light, Vec2 sphere, float size);
        virtual void flush_shadows();
        virtual void finish();

        void decal(int decal, float x, float y, float rotation, float scale, float opacity);

        void text_gc();

        void level_preview(int level, float x, float y, float w, float h);
        void page_indicator(int x, int y, int page, int pages);

        void icon(enum Icons::Icon icon, int x, int y, int w, int h, RGB color=RGB(1.0, 1.0, 1.0), float opacity=1.0, float rotation=0.0);
        void overlay();

        void begin_capture(enum Effect::TransitionFrame frame) {
            m_effect->begin_capture(frame);
        }

        void end_capture(enum Effect::TransitionFrame frame) {
            m_effect->end_capture(frame);
        }

        void transition(float value);

        bool rotation_enabled() {
            return m_rotation_enabled;
        }

        void set_rotation_enabled(bool enabled);

        void register_program(Program *program);
        void unregister_program(Program *program);

        void start_offscreen(bool reset_rotation=true) {
            if (!m_is_offscreen) {
                m_old_rotation_enabled = rotation_enabled();
            }

            set_rotation_enabled(!reset_rotation);
            m_is_offscreen = true;
        }

        void end_offscreen() {
            set_rotation_enabled(m_old_rotation_enabled);
            m_is_offscreen = false;
        }

        // Screen caching
        void prepare_cached(void *handle, size_t len,
                render_cached_func_t render_cached, void *user_data);
        void draw_cached(void *handle, size_t len,
                Vec2 offset_world=Vec2(0.0, 0.0));

    private:
        bool begin_cached(void *handle, size_t len);
        void end_cached();

        RenderedText *get_rendered_text(const char *text);

    private:
        Game *m_game;
        bool m_rotation_enabled;
        Projection m_projection;
    public:
        int m_width;
        int m_height;
    private:
        std::list<Program*> m_programs;
        std::list< std::pair<Program*,GLint> > m_programs_time;
        Mat4 m_projection_mat;
        Mat4 m_nprojection_mat;

        std::list<LoadingTask*> m_loading_tasks;
        int m_loading_tasks_total;
        Loading m_loading;

        Sphere *m_sphere;
        Line *m_line;
        Effect *m_effect;
        Font *m_font;
        PageIndicator *m_page_indicator;
        EffectOverlay *m_effect_overlay;
        Background *m_background;
        Icons *m_icons;
        Decal *m_decal;
        Shadow *m_shadow;
        LevelPreview *m_level_preview;
        CachedScreen *m_cached_screen;

        std::vector<Framebuffer*> m_previews;
        std::list<RenderedText*> m_rendered_texts;

        std::list< std::pair<ChangeWatch*,Framebuffer*> > m_cached;
        Framebuffer *m_cached_current_fb;

        bool m_is_offscreen;
        bool m_old_rotation_enabled;
};

#endif /* SHADYPOSTPROC_OPENGL_RENDERER_H */
