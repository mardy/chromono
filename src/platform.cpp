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


#include "platform.h"

#include "shadypostproc.h"

effect_func_t
Platform::effect_func = NULL;

void *
Platform::effect_func_user_data = NULL;

void
Platform::play(enum Sound::Effect sound)
{
    if (Platform::effect_func != NULL) {
        Platform::effect_func(sound, effect_func_user_data);
    } else {
        SHADY_DEBUG_PRINTF("Would play sound: %d\n", sound);
    }
}

void
Platform::register_effect(effect_func_t effect_func, void *user_data)
{
    Platform::effect_func_user_data = NULL;
    Platform::effect_func = effect_func;
    Platform::effect_func_user_data = user_data;
}

