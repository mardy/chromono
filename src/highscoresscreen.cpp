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


#include "highscoresscreen.h"

#include "game.h"
#include "colors.h"
#include "util.h"

HighScoresScreen::HighScoresScreen(Game *game, Page *parent, RGB color,
        const char *title, enum FontSize size, Resource *resource)
    : Page(game, Icons::BACK)
    , parent(parent)
    , color(color)
    , title(title)
    , size(size)
    , pressed(false)
    , last_mouse_pos(0.0, 0.0)
    , offset(0.0, 0.0)
    , max_offset_y(0.0)
    , velocity()
    , textfile(resource)
    , textlines()
    , ticks_since_last_move(0)
{
    char *p = (char*)textfile.data();
    char *end = p + textfile.size();
    while (p < end) {
        char *end = strchr(p, '\n');
        if (end != NULL) {
            *end = '\0';
            textlines.push_back(p);
        }
        p = end + 1;
    }
}


void
HighScoresScreen::render(OpenGLRenderer *renderer)
{
    render_page_title(renderer, title);

    RGB heading_color = RGB::mix(RGB(0xffffff), color);

    float border = 60.0;
    float y = border;

    y += offset.y;

    float h = 30.0;

    std::vector<char*>::iterator it;
    for(it=textlines.begin(); it != textlines.end(); ++it) {
        bool do_render = (y >= -h && y <= Constants::WORLD_HEIGHT);

        float opacity = 1.0;
        if (y < 0.0) {
            opacity = 0.0;
        } else if (y < 120.0) {
            float target_opacity = y / 120.0;
            float alpha = 1.0;
            if (offset.y < -20.0) {
                alpha = 0.0;
            } else if (offset.y < 0.0) {
                alpha = 1.0 - (-offset.y / 20.0);
            }

            opacity = opacity * alpha + target_opacity * (1.0 - alpha);
        } else if (y > Constants::WORLD_HEIGHT - 120.0) {
            float target_opacity = (Constants::WORLD_HEIGHT - y) / 120.0;
            float alpha = 1.0;
            if (offset.y > max_offset_y + 20.0) {
                alpha = 0.0;
            } else if (offset.y < 0.0) {
                alpha = 1.0 - (-(max_offset_y-offset.y) / 20.0);
            }

            opacity = opacity * alpha + target_opacity * (1.0 - alpha);
        }

        char *line = *it;

        if (line[0] == '=') {
            // A poor man's markup language ;)
            if (do_render) {
                renderer->text_render(line+1, 120.0, y,
                        (enum FontSize)(size + 1),
                        opacity, heading_color);
            }
            y -= 10;
        } else {
            if (do_render) {
                renderer->text_render(line, 120.0, y, size,
                        opacity);
            }
        }

        y += h;
    }

    float content_height = (y - offset.y);
    max_offset_y = (Constants::WORLD_HEIGHT - content_height) - border / 2.0;
}

void
HighScoresScreen::handle(Circle1DEvent *event)
{
    if (event->type == Circle1DEvent::TICK) {
        if (pressed) {
            ticks_since_last_move++;
        } else {
            offset += velocity;
            velocity *= 0.95;

            float alpha = 0.5;
            if (offset.y > 0.0) {
                offset.y = alpha * offset.y + (1.0 - alpha) * 0.0;
                velocity = Vec2(0.f, 0.f);
            } else if (offset.y < max_offset_y) {
                offset.y = alpha * offset.y + (1.0 - alpha) * max_offset_y;
                velocity = Vec2(0.f, 0.f);
            }
        }
    }

    if (event->finger != 0) {
        return;
    }

    Vec2 pos(event->x, event->y);

    switch (event->type) {
        case Circle1DEvent::MOUSEDOWN:
            if (!pressed) {
                last_mouse_pos = pos;
                pressed = true;
            }
            break;
        case Circle1DEvent::MOUSEMOTION:
            if (pressed) {
                velocity = (pos - last_mouse_pos);
                offset += velocity;
                last_mouse_pos = pos;
                ticks_since_last_move = 0;
            }
            break;
        case Circle1DEvent::MOUSEUP:
            if (pressed) {
                if (ticks_since_last_move > 10) {
                    velocity = Vec2(0.0, 0.0);
                }
                pressed = false;
            }
            break;
        default:
            break;
    }
}

bool
HighScoresScreen::on_back_button()
{
    game->transition_to(parent);
    return true;
}

void
HighScoresScreen::render_background(OpenGLRenderer *renderer)
{
    renderer->background(color);
}

void
HighScoresScreen::on_exposed()
{
    velocity = offset = Vec2(0.0, 0.0);
}
