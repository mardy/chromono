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

#ifndef SHADYPOSTPROC_PLATFORM_H
#define SHADYPOSTPROC_PLATFORM_H

#include "sounds.h"
#include "resources_util.h"

#include <memory>

// len == number of shorts in stream (sizeof(*stream) / sizeof(short))
typedef void (*audio_func_t)(short *stream, int len, void *user_data);
typedef void (*effect_func_t)(enum Sound::Effect sound, void *user_data);

class MutexPriv;

class Platform {
    public:
        static bool is_big_endian();

        /* Storage */
        static const char *storage_folder();

        /* Audio */
        static void register_audio(audio_func_t audio_func, void *user_data);
        static void set_playing(bool enabled);

        /* Sound effects - used by code throughout */
        static void play(enum Sound::Effect sound);

        /* Audio effect handler */
        static void register_effect(effect_func_t effect_func, void *user_data);

        static void set_fullscreen(bool fullscreen);

        /* Threading */
        class Mutex final {
        public:
            Mutex();
            ~Mutex();
            bool lock();
            void unlock();

        private:
            std::unique_ptr<MutexPriv> priv;
        };

    private:
        static effect_func_t effect_func;
        static void *effect_func_user_data;
};

class MutexLock {
public:
    MutexLock(Platform::Mutex &mutex)
        : mutex(mutex)
        , locked(mutex.lock())
    {
    }

    ~MutexLock()
    {
        if (locked) {
            mutex.unlock();
        }
    }

    operator bool() const { return locked; }

private:
    MutexLock(const MutexLock &) = delete;
    MutexLock &operator=(const MutexLock &) = delete;

    Platform::Mutex &mutex;
    bool locked;
};

#endif /* SHADYPOSTPROC_PLATFORM_H */
