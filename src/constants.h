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

#ifndef SHADYPOSTPROC_CONSTANTS_H
#define SHADYPOSTPROC_CONSTANTS_H

namespace Constants {
    extern int WORLD_WIDTH;
    extern int WORLD_HEIGHT;
    extern int TICK_MS;

    extern int PAGE_INDICATOR_RADIUS;

    extern int TRANSITION_DURATION_MS;

    extern int RENDERED_TEXT_CACHE_GC_FRAMES;

    extern int BACK_BUTTON_MARGIN;
    extern int BACK_BUTTON_SIZE;
    extern int CHOICE_SCREEN_BUTTON_SIZE;
    extern int CHOICE_SCREEN_BUTTON_SPACING;
    extern int CHOICE_SCREEN_BUTTON_Y;

    extern float SHADOW_LENGTH;

    extern int MENU_PARTICLES;

    extern bool RENDERED_LEVEL_PREVIEWS;
    extern bool UNLOCK_ALL;

    extern float MUSIC_VOLUME;
    extern float EFFECT_VOLUME;

    extern int DEFAULT_AUDIO_BUFFER;
};

#endif /* SHADYPOSTPROC_CONSTANTS_H */
