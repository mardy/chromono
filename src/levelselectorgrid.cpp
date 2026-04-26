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


#include "levelselectorgrid.h"

void
render_page_internal_callback(OpenGLRenderer *renderer, void *prepared, void *user_data)
{
    LevelSelectorGrid *grid = (LevelSelectorGrid*)user_data;
    grid->render_page_internal(renderer, (PagePreviewInfo*)prepared);
}

void
LevelSelectorGrid::prepare_pages(OpenGLRenderer *renderer)
{
    for (int i=0; i<3; i++) {
        int page = current_page - 1 + i;
        preview_info[i].page = page;
        if (page >= 0 && page < pages) {
            int first_level = first_of(page);
            for (int level=first_level; level<=last_of(page); level++) {
                LevelPreviewInfo *pinfo = &(preview_info[i].previews[level-first_level]);
                if (level < level_manager->count() && level<=last_level) {
                    LevelInfo *info = level_manager->get(level);
                    pinfo->level = level;
                    pinfo->locked = (score_manager->stars() < info->required_stars) && !Constants::UNLOCK_ALL;
                    pinfo->required_stars = info->required_stars;
                    pinfo->stars = score_manager->get_stars(level);
                } else {
                    pinfo->level = -1;
                }
            }

            renderer->prepare_cached(&(preview_info[i]),
                    sizeof(preview_info[i]),
                    render_page_internal_callback, this);
        }
    }
}

void
LevelSelectorGrid::render_page(OpenGLRenderer *renderer, int offset)
{
    int page = preview_info[offset+1].page;

    if (page >= 0 && page < pages) {
        Vec2 pos(scroll_offset_x + Constants::WORLD_WIDTH * offset, 0.0);

        if (offset == 0) {
            // Slow down dragging of center page if it's the first or
            // last page, so that there is some scrolling resistance
            if (page == 0 && scroll_offset_x > 0) {
                pos.x -= scroll_offset_x * 3 / 4;
            } else if (page == (pages - 1) && scroll_offset_x < 0) {
                pos.x -= scroll_offset_x * 3 / 4;
            }
        }

        renderer->draw_cached(&(preview_info[offset+1]), sizeof(preview_info[offset+1]), pos);
    }
}

void
LevelSelectorGrid::render_page_internal(OpenGLRenderer *renderer, PagePreviewInfo *info)
{
    LevelSelector *level_selector = game->get_level_selector();
    RGB color = level_selector->current_pack_color();
    RGB text_color = RGB::mix(RGB(0x000000), color);

    for (int i=0; i<per_page(); i++) {
        LevelPreviewInfo *level = &(info->previews[i]);
        if (level->level == -1) {
            continue;
        }
        Vec2 pos = position(i);

        if (!level->locked) {
            if (Constants::RENDERED_LEVEL_PREVIEWS) {
                renderer->level_preview(level->level, pos.x, pos.y, size.x, size.y);
            } else {
                Vec2 c = pos + size / 2.0;
                Object o(NULL, c.x, c.y, size.length() / 4.0, Object::FIXED, color);
                renderer->circle(&o);
            }
        } else {
            float lock_scale = 0.7;
            float side = std::min(size.x, size.y) * lock_scale;
            renderer->icon(Icons::LOCK,
                    pos.x + (size.x - side) / 2,
                    pos.y + (size.y - side) / 2,
                    side,
                    side,
                    Colors::LOCK, 0.5);

            char tmp[1024];
            snprintf(tmp, sizeof(tmp), "Need %d stars", level->required_stars);

            float w, h;
            renderer->text_measure(tmp, &w, &h, FONT_SMALL);
            renderer->text_render(tmp, pos.x + (size.x - w) / 2.0,
                    pos.y + size.y - h - 15.0, FONT_SMALL);
        }
    }

    if (!Constants::RENDERED_LEVEL_PREVIEWS) {
        renderer->flush_circles();
    }

    for (int i=0; i<per_page(); i++) {
        LevelPreviewInfo *level = &(info->previews[i]);
        if (level->level == -1 || level->locked) {
            continue;
        }
        Vec2 pos = position(i);

        if (!Constants::RENDERED_LEVEL_PREVIEWS) {
            float w, h;
            char tmp[1024];
            snprintf(tmp, sizeof(tmp), "%d", level->level - first_level + 1);
            renderer->text_measure(tmp, &w, &h, FONT_XLARGE);
            renderer->text_render(tmp, pos.x + (size.x - w) / 2.0,
                    pos.y + (size.y - h) / 2.0, FONT_XLARGE,
                    1.0, text_color);
        }

        for (int i=0; i<3; i++) {
            renderer->icon(Icons::STAR,
                    pos.x + size.x / 2 + 40 * (i-1) - 20,
                    pos.y + size.y - 45,
                    40, 40, Colors::STAR, (level->stars > i)?0.9:0.3);
        }
    }
}

void
LevelSelectorGrid::handle(Circle1DEvent *event)
{
    Vec2 pos(event->x, event->y);
    if (event->type == Circle1DEvent::MOUSEDOWN) {
        mousedown_pos = pos;
        pressed = true;
        Platform::play(Sound::BUTTON_PRESS);
        scrolling = false;
    } else if (event->type == Circle1DEvent::MOUSEMOTION) {
        if (pressed) {
            float scroll_threshold = 50;
            if ((pos - mousedown_pos).length() > scroll_threshold) {
                // Start scrolling here
                scrolling = true;
                scroll_offset_x = (pos.x - mousedown_pos.x);
            }
        }
    } else if (event->type == Circle1DEvent::MOUSEUP) {
        if (pressed) {
            if (!scrolling) {
                int level = trace(event->x, event->y);
                if (level != -1 && level < level_manager->count()) {
                    LevelInfo *info = level_manager->get(level);
                    int missing_stars = (info->required_stars - score_manager->stars());
                    if (missing_stars <= 0 || Constants::UNLOCK_ALL) {
                        Platform::play(Sound::BUTTON_RELEASE);
                        game->start_level(level);
                    } else {
                        char tmp[1024];
                        snprintf(tmp, sizeof(tmp), (missing_stars == 1) ? "Need %d more star to unlock" :
                                "Need %d more stars to unlock", missing_stars);
                        game->show_message(tmp);
                        Platform::play(Sound::LEVEL_LOCKED_MESSAGE_BOX);
                    }
                }
            } else {
                if (scroll_offset_x > Constants::WORLD_WIDTH / 4.0) {
                    if (previous_page()) {
                        scroll_offset_x -= Constants::WORLD_WIDTH;
                    }
                } else if (scroll_offset_x < -Constants::WORLD_WIDTH / 4.0) {
                    if (next_page()) {
                        scroll_offset_x += Constants::WORLD_WIDTH;
                    }
                }
            }
            scrolling = false;
            pressed = false;
        }
    } else if (event->type == Circle1DEvent::TICK) {
        if (!pressed) {
            scroll_offset_x *= 0.8;
        }
    }
}

void
LevelSelectorGrid::render(OpenGLRenderer *renderer)
{
    render_page(renderer, 0);

    if (scroll_offset_x < 0.0) {
        render_page(renderer, +1);
    } else if (scroll_offset_x > 0.0) {
        render_page(renderer, -1);
    }

    Vec2 offset = game->get_offset();

    float w, h;
    renderer->text_measure(title, &w, &h, FONT_LARGE);
    renderer->text_render(title, (Constants::WORLD_WIDTH - w) / 2.0, 10.0 - offset.y, FONT_LARGE);

    if (pages > 1) {
        renderer->page_indicator(Constants::WORLD_WIDTH / 2.0,
                game->get_level_selector()->indicator_y() +
                Constants::PAGE_INDICATOR_RADIUS,
                current_page, pages);
    }
}

void
LevelSelectorGrid::scroll_to(int level)
{
    current_page = (level - first_level) / per_page();
}

