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


#include "mixer.h"

#include "platform.h"
#include "constants.h"
#include "resources.h"

#include <list>
#include <algorithm>
#include <pthread.h>

#define MIXER_MAX_CHANNELS 4
#define MIXER_BACKGROUNDS 2

#include <vorbis/vorbisfile.h>

static size_t
decode_ogg_in_memory(const unsigned char *encoded_ogg, size_t encoded_ogg_length_bytes,
        int *channels, int *sample_rate, short **decoded_samples)
{
    OggVorbis_File ovf;
    memset(&ovf, 0, sizeof(ovf));

    ov_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    if (ov_open_callbacks(NULL, &ovf, (char *)encoded_ogg, encoded_ogg_length_bytes, callbacks) == 0) {
        vorbis_info *info = ov_info(&ovf, -1);

        *channels = info->channels;
        *sample_rate = info->rate;

        size_t pos = 0;
        size_t allocated_size = 0;
        *decoded_samples = NULL;

        while (true) {
            if (allocated_size == pos) {
                allocated_size += allocated_size + 1024;
                *decoded_samples = (short *)realloc(*decoded_samples, allocated_size);
            }

            long res = ov_read(&ovf, ((char *)*decoded_samples) + pos, allocated_size - pos,
                               Platform::is_big_endian(), 2, 1, 0);
            if (res < 0) {
                free(*decoded_samples);
                return 0;
            } else if (res > 0) {
                pos += res;
            } else {
                break;
            }
        }

        *decoded_samples = (short *)realloc(*decoded_samples, pos);

        ov_clear(&ovf);

        return pos / sizeof(short);
    }

    return 0;
}

class Buffer {
    public:
        Buffer(Resource *resource)
        {
            ResourceAccess access(resource);
            len = decode_ogg_in_memory(access.data(),
                    access.size(), &channels, &sample_rate, &data);
        }

        ~Buffer()
        {
            free(data);
        }

        short get(float pos) {
            if (pos >= len || pos < 0.0) {
                return 0.0;
            }

            float alpha = fmod(pos, 1.0);
            int cur = (int)pos;
            if (alpha == 0.0) { return data[cur]; }

            int nxt = (cur + 1 < len) ? (cur + 1) : cur;
            return (data[cur] * (1.0 - alpha) +
                    data[nxt] * alpha);
        }

        short *data;
        int len;
        int channels;
        int sample_rate;
};

class Playback {
    public:
        Playback();
        ~Playback();

        Playback(const Playback &other);
        Playback &operator=(const Playback &other);

        void start(Buffer *buffer, float rate=1.0,
                float volume=1.0, bool loop=false);
        void stop();

        void tick();

        void fill(short *stream, int len);

        void set_rate(float rate) { this->rate = rate; }
        float get_rate() { return rate; }

        void set_volume(float volume) { this->volume = volume; }
        float get_volume() { return volume; }

        void set_loop(bool loop) { this->loop = loop; }
        float get_loop() { return loop; }

        long position() {
            return current_pos * 1000 / 22050;
        }

        long duration() {
            return buffer->len * 1000 / 22050;
        }

        void start_fade_out() {
            if (fadeout == -1) {
                fadeout = 50;
            }
        }

        void stop_fade_out() {
            fadeout = -1;
        }

        bool finished() {
            return (buffer == NULL || current_pos > buffer->len || fadeout == 0);
        }

        bool is_buffer(Buffer *buffer) {
            return this->buffer == buffer;
        }

    private:
        Buffer *buffer;

        float current_pos;
        float current_rate;
        float current_volume;

        float rate;
        float volume;
        bool loop;
        int fadeout;
};

class Player {
    public:
        Player();
        ~Player();

        void music(Buffer *buffer, float rate=1.0, float volume=1.0);
        void sfx(Buffer *buffer, float rate=1.0, float volume=1.0);

        void tick();

        // current amplitude, used for visualization
        float amplitude;

    private:
        static void
        audio_func(short *stream, int len, void *user_data);

        void fill(short *stream, int len);
        float mix(short *dst, short *src, int len);

        Platform::Mutex mutex;

        // Two backgrounds for fading between them
        Playback background[MIXER_BACKGROUNDS];
        Playback effects[MIXER_MAX_CHANNELS];
};

class MixerPriv {
    public:
        MixerPriv();
        ~MixerPriv();

        float get_amplitude() { return player.amplitude; }

        Buffer *buffers[Sound::COUNT];
        Player player;
        bool sound_effects_enabled;
};

static struct {
    enum Sound::Effect effect;
    Resource *resource;
} RESOURCE_MAPPING[] = {
    {Sound::LEVEL_LOCKED_MESSAGE_BOX, RESOURCE(sound_level_locked_message_box_ogg)},
    {Sound::BUTTON_RELEASE, RESOURCE(sound_button_release_ogg)},
    {Sound::LEVEL_COMPLETE_NO_STARS, RESOURCE(sound_level_complete_ogg)},
    {Sound::LEVEL_COMPLETE_1_STAR, RESOURCE(sound_level_complete_ogg)},
    {Sound::LEVEL_COMPLETE_2_STARS, RESOURCE(sound_level_complete_ogg)},
    {Sound::LEVEL_COMPLETE_3_STARS, RESOURCE(sound_level_complete_ogg)},
    {Sound::TOGGLE_SWITCH_OFF, RESOURCE(sound_toggle_switch_off_ogg)},
    {Sound::PAGE_TRANSITION, RESOURCE(sound_page_transition_ogg)},
    {Sound::COLOR_CHANGES_TO_WRONG, RESOURCE(sound_color_changes_to_wrong_ogg)},
    {Sound::MOVABLE_SPHERE_RELEASE, RESOURCE(sound_movable_sphere_release_ogg)},
    {Sound::MOVABLE_SPHERE_PRESS, RESOURCE(sound_movable_sphere_press_ogg)},
    {Sound::COLOR_CHANGES_TO_RIGHT, RESOURCE(sound_color_changes_to_right_ogg)},
    {Sound::TOGGLE_SWITCH_ON, RESOURCE(sound_toggle_switch_on_ogg)},
    {Sound::CHAIN_BREAKS, RESOURCE(sound_chain_breaks_ogg)},
    {Sound::SPHERE_HITS_WALL, RESOURCE(sound_sphere_hits_wall_ogg)},
    {Sound::BUTTON_PRESS, RESOURCE(sound_button_press_ogg)},
    {Sound::MUSIC_ALVEG, RESOURCE(music_alveg_ogg)},
    {Sound::MUSIC_KASA90, RESOURCE(music_kasa90_ogg)},
    {Sound::MUSIC_SNAPPER, RESOURCE(music_snapper_ogg)},
    {Sound::MUSIC_SUBD, RESOURCE(music_subd_ogg)},
};

MixerPriv::MixerPriv()
    : player()
    , sound_effects_enabled(true)
{
    for (int i=0; i<Sound::COUNT; i++) {
        buffers[i] = NULL;
    }

    for (size_t i=0; i<ARRAY_LENGTH(RESOURCE_MAPPING); i++) {
        buffers[RESOURCE_MAPPING[i].effect] =
            new Buffer(RESOURCE_MAPPING[i].resource);
    }
}

MixerPriv::~MixerPriv()
{
    for (int i=0; i<Sound::COUNT; i++) {
        if (buffers[i]) {
            delete buffers[i];
        }
    }
}

void
mixer_effect(enum Sound::Effect sound, void *user_data)
{
    Mixer *mixer = (Mixer*)user_data;
    mixer->play(sound, Constants::EFFECT_VOLUME);
}

Mixer::Mixer()
    : priv(new MixerPriv)
{
    Platform::register_effect(mixer_effect, this);
}

Mixer::~Mixer()
{
    Platform::register_effect(NULL, NULL);

    delete priv;
}

void
Mixer::play(enum Sound::Effect effect, float volume)
{
    if (priv->sound_effects_enabled) {
        float rate = 1.0;
        priv->player.sfx(priv->buffers[effect], rate, volume);
    }
}

void
Mixer::loop(enum Sound::Effect music, float volume, float rate)
{
    priv->player.music(priv->buffers[music], rate, volume);
}

void
Mixer::tick()
{
    priv->player.tick();
}

Playback::Playback()
    : buffer(NULL)
    , current_pos(0.0)
    , current_rate(1.0)
    , current_volume(1.0)
    , rate(1.0)
    , volume(1.0)
    , loop(false)
    , fadeout(0)
{
}

Playback::Playback(const Playback &other)
    : buffer(other.buffer)
    , current_pos(other.current_pos)
    , current_rate(other.current_rate)
    , current_volume(other.current_volume)
    , rate(other.rate)
    , volume(other.volume)
    , loop(other.loop)
    , fadeout(other.fadeout)
{
}

Playback &
Playback::operator=(const Playback &other)
{
    this->buffer = other.buffer;
    this->current_pos = other.current_pos;
    this->current_rate = other.current_rate;
    this->current_volume = other.current_volume;
    this->rate = other.rate;
    this->volume = other.volume;
    this->loop = other.loop;
    this->fadeout = other.fadeout;

    return *this;
}

void
Playback::start(Buffer *buffer, float rate,
        float volume, bool loop)
{
    this->buffer = buffer;
    this->current_pos = 0.0;
    this->current_rate = this->rate = rate;
    this->current_volume = this->volume = volume;
    this->loop = loop;
    this->fadeout = -1;
}

void
Playback::stop()
{
    this->loop = false;
    this->fadeout = 0;
}

Playback::~Playback()
{
}

void
Playback::fill(short *stream, int len)
{
    float factor = volume;
    if (fadeout >= 0) {
        factor *= (float)fadeout / 100.0;
    }

    for (int i=0; i<len; i++) {
        stream[i] = buffer->get(current_pos) * factor;
        current_pos += current_rate;
        if (loop) {
            if (current_pos >= buffer->len - 1) {
                current_pos = 0.0;
            }
        }
    }
}

void
Playback::tick()
{
    float alpha = 0.95;

    current_rate = alpha * current_rate + (1.0 - alpha) * rate;
    current_volume = alpha * current_volume + (1.0 - alpha) * volume;

    if (fadeout > 0) {
        fadeout--;
    }
}

void
Player::audio_func(short *stream, int len, void *user_data)
{
    Player *player = (Player*)user_data;
    player->fill(stream, len);
}

Player::Player()
    : amplitude(0)
{
    Platform::register_audio(Player::audio_func, this);
}

Player::~Player()
{
    Platform::register_audio(NULL, NULL);
}

void
Player::sfx(Buffer *buffer, float rate, float volume)
{
    if (buffer == NULL) {
        return;
    }

    int i = 0;

    if (mutex.lock()) {
        for (i=0; i<MIXER_MAX_CHANNELS; i++) {
            if (effects[i].finished()) {
                effects[i].start(buffer, rate, volume);
                break;
            }
        }
        mutex.unlock();
    }

    if (i == MIXER_MAX_CHANNELS) {
        SHADY_DEBUG_PRINTF("No free slot for sound effect - skipping\n");
    }
}

void
Player::music(Buffer *buffer, float rate, float volume)
{
    Playback *a = &background[0];
    Playback *b = &background[1];

    // "a" is always the current (main) music,
    // "b" is the last (fading out) music

    if (mutex.lock()) {
        if (a->is_buffer(buffer)) {
            // Music is already playing - just update rate/volume
            a->stop_fade_out();
            a->set_loop(true);
            a->set_rate(rate);
            a->set_volume(volume);
        } else {
            // Move old background music from "a" to "b" and fade out,
            // then start new music on "a" (or stop "a" if fading out)
            background[1] = background[0];
            b->start_fade_out();

            if (buffer != NULL) {
                a->start(buffer, rate, volume, true);
            } else {
                a->stop();
            }
        }
        mutex.unlock();
    }
}

void
Player::tick()
{
    if (mutex.lock()) {
        for (int i=0; i<MIXER_MAX_CHANNELS; i++) {
            effects[i].tick();
        }

        for (int i=0; i<MIXER_BACKGROUNDS; i++) {
            background[i].tick();
            if (background[i].finished()) {
                background[i].stop();
            }
        }
        mutex.unlock();
    }
}

void
Player::fill(short *stream, int len)
{
    short tmp[MIXER_BACKGROUNDS+MIXER_MAX_CHANNELS][len];
    bool filled[MIXER_BACKGROUNDS+MIXER_MAX_CHANNELS];

    memset(stream, 0, sizeof(short) * len);

    if (mutex.lock()) {
        for (int i=0; i<MIXER_BACKGROUNDS; i++) {
            int index = i;
            if (!background[i].finished()) {
                background[i].fill(tmp[index], len);
                filled[index] = true;
            } else {
                filled[index] = false;
            }
        }

        for (int i=0; i<MIXER_MAX_CHANNELS; i++) {
            int index = MIXER_BACKGROUNDS + i;
            if (!effects[i].finished()) {
                effects[i].fill(tmp[index], len);
                filled[index] = true;
            } else {
                filled[index] = false;
            }
        }
        mutex.unlock();
    }

    float amplitude_new = 0.0;
    for (int i=0; i<MIXER_BACKGROUNDS+MIXER_MAX_CHANNELS; i++) {
        if (filled[i]) {
            float ampl = mix(stream, tmp[i], len);
            if (ampl > amplitude_new) {
                amplitude_new = ampl;
            }
        }
    }

    amplitude = amplitude_new;
}

float
Player::mix(short *dst, short *src, int len)
{
    const int max = (1 << 15) - 1;
    const int min = -(1 << 15);

    int amplitude = 0;
    for (int i=0; i<len; i++) {
        int s = dst[i] + src[i];

        if (s < min) {
            s = min;
        } else if (s > max) {
            s = max;
        }

        if (s > amplitude) {
            amplitude = s;
        }

        dst[i] = s;
    }

    return (float)amplitude / (float)max;
}

void
Mixer::set_sound_effects_enabled(bool enabled)
{
    priv->sound_effects_enabled = enabled;
}

float
Mixer::get_amplitude()
{
    return priv->get_amplitude();
}

