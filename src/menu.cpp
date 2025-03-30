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


#include "menu.h"

#include "opengl_renderer.h"

#include "game.h"
#include "util.h"
#include "colors.h"

Button::Button(Menu *menu, Object *object, enum Icons::Icon icon, const char *text, game_lambda_t listener)
    : Pressable()
    , x(0.0)
    , y(0.0)
    , w(0.0)
    , h(0.0)
    , m_menu(menu)
    , m_object(object)
    , m_text(text)
    , m_listener(listener)
{
    m_menu->queue_relayout();
}

Button::~Button()
{
    //delete m_text;
}

void
Button::set_text(const char *text)
{
    m_text = text;
    w = h = 0.0;
    m_menu->queue_relayout();
}

void
Button::render(OpenGLRenderer *renderer, float opacity)
{
    renderer->text_render(m_text, x, y, FONT_MEDIUM, opacity);
    //float size = m_object->size * 1.5;
    //Vec2 pos = m_object->pos - Vec2(size, size) / 2.0;
    //renderer->icon(m_icon, pos.x, pos.y, size, size, RGB(1.0, 1.0, 1.0), 0.1);
}

bool
Button::contains(Vec2 pos)
{
    return (((pos - m_object->pos).length() < m_object->size) ||
            (pos.x >= x && pos.x <= (x + w) &&
             pos.y >= y && pos.y <= (y + h)));
}

bool
Button::on_pressed()
{
    if (m_listener != NULL) {
        m_listener(m_menu->get_game());
    }

    return false;
}

void
go_to_selector(Game *game)
{
    game->to_selector(true);
}

void
go_to_next_unplayed(Game *game)
{
    game->to_next_unplayed();
}

void
go_to_highscores(Game *game)
{
    game->to_highscores();
}

void
go_to_about(Game *game)
{
    game->to_about();
}

void
go_to_options(Game *game)
{
    game->to_options();
}

class FallingObjectsBehavior : public Circle1DBehavior {
    public:
        CIRCLE1D_BEHAVIOR_BODY(FallingObjectsBehavior)

        FallingObjectsBehavior(Scene *scene, bool *pressed)
            : Circle1DBehavior(scene)
            , m_objects()
            , m_random(1337, 4711)
            , m_pressed(pressed)
            , m_stream_strength(0.0)
            , m_border(0.0)
            , m_populated(false)
        {
        }

        void populate()
        {
            for (int i=0; i<Constants::MENU_PARTICLES; i++) {
                Vec2 pos = Vec2(m_random.next() % Constants::WORLD_WIDTH,
                                m_random.next() % (int)(Constants::WORLD_HEIGHT + m_border * 2.0) - m_border);
                Object *oo = new Object(scene, pos.x, pos.y, 8 + (m_random.next() % 8),
                            Object::COLLIDER | Object::COLORABLE | Object::SLOW | Object::SILENT);
                m_objects.push_back(oo);
            }
        }

        virtual void simulate()
        {
            float wiggle = 0.0;
            
            if (*m_pressed) {
                float alpha = 0.99;
                m_stream_strength = alpha * m_stream_strength + (1.0 - alpha) * 1.0;
                wiggle = sinf(scene->time * Constants::TICK_MS * 0.04);
            } else {
                float alpha = 0.95;
                m_stream_strength = alpha * m_stream_strength + (1.0 - alpha) * 0.0;
            }

            Vec2 force(10.0 * m_stream_strength * wiggle, - 60.0 * m_stream_strength);

            std::vector<Object*>::iterator it;
            for (it=m_objects.begin(); it != m_objects.end(); ++it) {
                Object *o = *it;
                if (o->pos.y > Constants::WORLD_HEIGHT + o->size + m_border) {
                    o->pos = Vec2(m_random.next() % Constants::WORLD_WIDTH,
                            -(o->size + m_border));
                } else if (o->pos.y < -(o->size + m_border)) {
                    o->pos = Vec2(m_random.next() % Constants::WORLD_WIDTH,
                            Constants::WORLD_HEIGHT + o->size + m_border);
                }
                if (m_stream_strength < 0.5) {
                    o->target_color = RGB::mix(RGB(0.0, 0.0, 0.0), RGB::background());
                }
                o->desired = o->color;
                o->apply_force(force);
            }
        }

        void set_border(float border) {
            m_border = border;
            if (!m_populated) {
                populate();
                scene->simulate(60); // preroll
                m_populated = true;
            }
        }

    private:
        std::vector<Object*> m_objects;
        Random m_random;
        bool *m_pressed;
        float m_stream_strength;
        float m_border;
        bool m_populated;
};


static Vec2
button_about_pos(Game *game)
{
    Vec2 pos = game->get_offset();
    pos.x = Constants::BACK_BUTTON_MARGIN - pos.x;
    pos.y += Constants::WORLD_HEIGHT -
        Constants::BACK_BUTTON_MARGIN - Constants::BACK_BUTTON_SIZE;
    return pos;
}

static Vec2
button_options_pos(Game *game)
{
    Vec2 pos = game->get_offset();
    pos.y += Constants::WORLD_HEIGHT -
        Constants::BACK_BUTTON_MARGIN - Constants::BACK_BUTTON_SIZE;
    pos.x += Constants::WORLD_WIDTH -
        Constants::BACK_BUTTON_MARGIN - Constants::BACK_BUTTON_SIZE;
    return pos;
}


Menu::Menu(Game *game)
    : Page(game)
    , m_scene()
    , m_falling_objects(NULL)
    , m_buttons()
    , m_must_relayout(false)
    , m_time(0)
    , play_button(NULL)
    , m_pressed(false)
    , m_button_about(game, go_to_about, button_about_pos(game), Constants::BACK_BUTTON_SIZE)
    , m_button_options(game, go_to_options, button_options_pos(game), Constants::BACK_BUTTON_SIZE)
{
    m_falling_objects = new FallingObjectsBehavior(&m_scene, &m_pressed);

    play_button =
    add_button("Play", Icons::PLAY, RGB(0.4, 0.8, 0.4), go_to_next_unplayed);
    add_button("Chapters", Icons::GRID, Colors::SELECTOR_BACKGROUND_COLOR, go_to_selector);
    add_button("Help", Icons::STAR, Colors::HELP_BACKGROUND_COLOR, go_to_highscores);
}

Menu::~Menu()
{
    std::vector<Button*>::iterator it;
    for (it=m_buttons.begin(); it != m_buttons.end(); ++it) {
        delete (*it);
    }
}

Button *
Menu::add_button(const char *text, enum Icons::Icon icon, RGB color, game_lambda_t listener)
{
    Object *o = new Object(&m_scene, 300, 300, 64, Object::FIXED | Object::COLLIDER, color);
    Button *button = new Button(this, o, icon, text, listener);
    m_buttons.push_back(button);
    return button;
}

void
Menu::before_render(OpenGLRenderer *renderer)
{
    m_button_about.setPos(button_about_pos(game));
    m_button_options.setPos(button_options_pos(game));
}

void
Menu::render(OpenGLRenderer *renderer)
{
    if (m_must_relayout) {
        relayout(renderer);
        m_must_relayout = false;
    }

    Vec2 pos = game->get_offset();
    m_falling_objects->set_border(pos.y + 10.0);

    m_scene.render(renderer);

    std::vector<Button*>::iterator it;
    for (it=m_buttons.begin(); it != m_buttons.end(); ++it) {
        (*it)->render(renderer, 1.0);
    }

    RGB color(0.4, 0.6, 0.7);

    pos = m_button_about.pos();
    renderer->icon(Icons::ABOUT, pos.x, pos.y, Constants::BACK_BUTTON_SIZE,
            Constants::BACK_BUTTON_SIZE, color, 0.3);

    pos = m_button_options.pos();
    renderer->icon(Icons::OPTIONS, pos.x, pos.y, Constants::BACK_BUTTON_SIZE,
            Constants::BACK_BUTTON_SIZE, color, 0.3);
}

void
Menu::handle(Circle1DEvent *event)
{
    if (event->type == Circle1DEvent::TICK) {
        m_scene.handle(event);
        m_time++;
    }

    if (m_button_about.handle(event)) {
        return;
    } else if (m_button_options.handle(event)) {
        return;
    }

    std::vector<Button*>::iterator it;
    for (it=m_buttons.begin(); it != m_buttons.end(); ++it) {
        if ((*it)->handle(event)) {
            return;
        }
    }

    if (event->finger == 0) {
        if (event->type == Circle1DEvent::MOUSEDOWN) {
            Platform::play(Sound::MAIN_MENU_STREAM_SPEEDUP);
            m_pressed = true;
        } else if (event->type == Circle1DEvent::MOUSEUP) {
            Platform::play(Sound::MAIN_MENU_STREAM_SLOWDOWN);
            m_pressed = false;
        }
    }
}

void
Menu::relayout(OpenGLRenderer *renderer)
{
    Vec2 spacing(50, 10);
    float height = 0;
    float width = 0;

    float button_width = 120;

    // Measure total width
    std::vector<Button*>::iterator it;
    for (it=m_buttons.begin(); it != m_buttons.end(); ++it) {
        Button *b = *it;
        renderer->text_measure(b->m_text, &(b->w), &(b->h), FONT_MEDIUM);

        width += button_width;
        height = std::max(spacing.y + b->m_object->size * 2.f, height);
    }
    width += spacing.x * (m_buttons.size() - 1);

    // Layout buttons
    int x = (Constants::WORLD_WIDTH - width) / 2;
    for (it=m_buttons.begin(); it != m_buttons.end(); ++it) {
        Button *b = *it;
        b->x = x + (button_width - b->w) / 2.0;
        b->m_object->pos = Vec2(b->x + b->w / 2, Constants::WORLD_HEIGHT / 2);
        b->y = b->m_object->pos.y + spacing.y + b->m_object->size;
        x += button_width + spacing.x;
    }
}

void
Menu::on_exposed()
{
    ScoreManager *score_manager = game->get_score_manager();

    if (score_manager->any_played()) {
        play_button->set_text("Continue");
    }
}

void
Menu::render_background(OpenGLRenderer *renderer)
{
    RGB color = RGB::mix(RGB(0xFFFFFF), RGB::background(),
            0.3 * game->get_sound_opacity());
    renderer->background(color);
}
