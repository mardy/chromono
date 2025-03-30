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

#ifndef SHADYPOSTPROC_MIXER_H
#define SHADYPOSTPROC_MIXER_H

#include "shadypostproc.h"

#include "sounds.h"

class MixerPriv;

class Mixer {
    public:
        Mixer();
        virtual ~Mixer();

        // Play a single sound effect once
        void play(enum Sound::Effect effect, float volume=1.0);

        // Set the current background music loop
        void loop(enum Sound::Effect music, float volume=1.0, float rate=1.0);

        // Update properties, clean up things, etc..
        void tick();

        // Called from Game to enable/disable sound effects
        void set_sound_effects_enabled(bool enabled);

        // Get current amplitude for visualization
        float get_amplitude();

    private:
        MixerPriv *priv;
};

#endif /* SHADYPOSTPROC_MIXER_H */
