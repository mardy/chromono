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

#ifndef SHADYPOSTPROC_SOUNDS_H
#define SHADYPOSTPROC_SOUNDS_H

namespace Sound {
    enum Effect {
        BUTTON_PRESS,
        BUTTON_RELEASE,

        TOGGLE_SWITCH_ON,
        TOGGLE_SWITCH_OFF,

        MOVABLE_SPHERE_PRESS,
        MOVABLE_SPHERE_RELEASE,

        MAIN_MENU_STREAM_SPEEDUP,
        //MAIN_MENU_STREAM_RUNNING,
        MAIN_MENU_STREAM_SLOWDOWN,

        LEVEL_COMPLETE_NO_STARS,
        LEVEL_COMPLETE_1_STAR,
        LEVEL_COMPLETE_2_STARS,
        LEVEL_COMPLETE_3_STARS,

        //LEVEL_COMPLETE_NEW_HIGHSCORE,

        PAGE_TRANSITION,

        GAMEPLAY_PAUSED,
        GAMEPLAY_RESUMED,

        UNLOCK_LEVEL,
        LEVEL_LOCKED_MESSAGE_BOX,

        //SPHERE_HITS_SPHERE,
        SPHERE_HITS_WALL,

        COLOR_CHANGES_TO_WRONG,
        COLOR_CHANGES_TO_RIGHT,

        CHAIN_BREAKS,

        MUSIC_ALVEG,
        MUSIC_KASA90,
        MUSIC_SNAPPER,
        MUSIC_SUBD,

        NO_SOUND, // will never be a valid sound

        COUNT, // last element = number of sounds
    };
};

#endif /* SHADYPOSTPROC_SOUNDS_H */
