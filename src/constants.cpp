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


#include "constants.h"

namespace Constants {
    int WORLD_WIDTH = 800;
    int WORLD_HEIGHT = 480;
    int TICK_MS = 20;

    int PAGE_INDICATOR_RADIUS = 8;
    int TRANSITION_DURATION_MS = 100;

    int RENDERED_TEXT_CACHE_GC_FRAMES = 200;

    int BACK_BUTTON_MARGIN = 10;
    int BACK_BUTTON_SIZE = 70;
    int CHOICE_SCREEN_BUTTON_SIZE = 100;
    int CHOICE_SCREEN_BUTTON_SPACING = 40;
    int CHOICE_SCREEN_BUTTON_Y = 390;

    float SHADOW_LENGTH = 9.0;

    int MENU_PARTICLES = 120;

    bool RENDERED_LEVEL_PREVIEWS = true;
    bool UNLOCK_ALL = false;

    float MUSIC_VOLUME = 0.05;
    float EFFECT_VOLUME = 0.3;

    int DEFAULT_AUDIO_BUFFER = 2048;
};

