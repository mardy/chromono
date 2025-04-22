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

#include "shadypostproc.h"

#include "game.h"
#include "util.h"

#if defined(__wii__) || defined(__gamecube__)
#include "opengx_shaders.h"
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <SDL.h>

class PlatformPriv {
    public:
        PlatformPriv();
        ~PlatformPriv();
        bool process(Circle1DEventHandler *handler);
        void add_controller(int joystick_index);

        static void audio_callback(void *userdata, Uint8 *stream, int len);

        SDL_Window *window;
        int width;
        int height;

    private:
        audio_func_t audio_func;
        void *audio_func_user_data;
        SDL_GameController *controller;

        friend class Platform;
};

static PlatformPriv *platform_priv = NULL;

void
PlatformPriv::audio_callback(void *userdata, Uint8 *stream, int len)
{
    PlatformPriv *priv = (PlatformPriv*)userdata;
    if (priv->audio_func) {
        priv->audio_func((short*)stream, len/sizeof(short),
                priv->audio_func_user_data);
    }
}

PlatformPriv::PlatformPriv()
    : audio_func(NULL)
    , audio_func_user_data(NULL)
    , controller(NULL)
{
#if defined(__wii__) || defined(__gamecube__)
    setup_opengx_shaders();
    setenv("OPENGX_DEBUG", "warnings", 1);
#endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

    SDL_AudioSpec desired;
    memset(&desired, 0, sizeof(desired));
    desired.freq = 22050;
    desired.format = SDL_BYTEORDER == SDL_BIG_ENDIAN ?
        AUDIO_S16MSB : AUDIO_S16LSB;
    desired.channels = 1;
    desired.samples = Constants::DEFAULT_AUDIO_BUFFER;
    desired.callback = PlatformPriv::audio_callback;
    desired.userdata = this;
    SDL_OpenAudio(&desired, NULL);

    int num_joysticks = SDL_NumJoysticks();
    for (int i = 0; i < num_joysticks; i++) {
        if (SDL_IsGameController(i)) {
            add_controller(i);
        }
    }

    platform_priv = this;
}

PlatformPriv::~PlatformPriv()
{
    if (controller) {
        SDL_GameControllerClose(controller);
    }

    SDL_CloseAudio();

    SDL_Quit();
}

bool
PlatformPriv::process(Circle1DEventHandler *handler)
{
    static Circle1DEvent evt;
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool valid = true;
            evt.button = 0;
            switch (event.key.keysym.sym) {
                case SDLK_s:
                    evt.finger = Circle1DEvent::SAVE_SCENE;
                    break;
                case SDLK_l:
                    evt.finger = Circle1DEvent::LOAD_SCENE;
                    break;
                case SDLK_e:
                    evt.finger = Circle1DEvent::EDIT;
                    break;
                case SDLK_d:
                    evt.finger = Circle1DEvent::DELETE;
                    break;
                case SDLK_c:
                    evt.finger = Circle1DEvent::TOGGLE_COLLIDER;
                    break;
                case SDLK_g:
                    evt.finger = Circle1DEvent::TOGGLE_GRAVITY;
                    break;
                case SDLK_b:
                    evt.finger = Circle1DEvent::TOGGLE_BORDERBOUNCE;
                    break;
                case SDLK_f:
                    evt.finger = Circle1DEvent::TOGGLE_FIXED;
                    break;
                case SDLK_h:
                    evt.finger = Circle1DEvent::TOGGLE_SHADOWCAST;
                    break;
                case SDLK_q:
                    // Shortcut useful during development
                    exit(0);
                    break;
                case SDLK_0:
                    evt.button++;
                case SDLK_9:
                    evt.button++;
                case SDLK_8:
                    evt.button++;
                case SDLK_7:
                    evt.button++;
                case SDLK_6:
                    evt.button++;
                case SDLK_5:
                    evt.button++;
                case SDLK_4:
                    evt.button++;
                case SDLK_3:
                    evt.button++;
                case SDLK_2:
                    evt.button++;
                case SDLK_1:
                    evt.finger = Circle1DEvent::SET_COLOR;
                    break;
                case SDLK_i: // "inner" color
                    evt.finger = Circle1DEvent::SET_DESIRED_COLOR;
                    break;
                case SDLK_k: // target "kolor"
                    evt.finger = Circle1DEvent::SET_TARGET_COLOR;
                    break;
                default:
                    valid = false;
                    break;
            }

            if (valid) {
                evt.type = (event.type == SDL_KEYDOWN) ? Circle1DEvent::KEYDOWN : Circle1DEvent::KEYUP;
                handler->handle(&evt);
            }
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return false;
                    break;
                default:
                    break;
            }
        } else if (event.type == SDL_QUIT) {
            return false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN ||
                   event.type == SDL_MOUSEBUTTONUP ||
                   event.type == SDL_MOUSEMOTION) {
            evt.finger = 0;

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                evt.type = Circle1DEvent::MOUSEDOWN;
                evt.x = event.button.x;
                evt.y = event.button.y;
                evt.finger = event.button.which;
                evt.button = event.button.button;
            } else if (event.type == SDL_MOUSEMOTION) {
                evt.type = Circle1DEvent::MOUSEMOTION;
                evt.x = event.motion.x;
                evt.y = event.motion.y;
                evt.finger = 0;
                evt.button = 0;
                for (int i=0; i<3; i++) {
                    if (event.motion.state & (1 << i)) {
                        evt.button = i + 1;
                        break;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                evt.type = Circle1DEvent::MOUSEUP;
                evt.x = event.button.x;
                evt.y = event.button.y;
                evt.finger = event.button.which;
                evt.button = event.button.button;
            }

            handler->handle(&evt);
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                width = event.window.data1;
                height = event.window.data2;
                static_cast<Game *>(handler)->resize(width, height);
            }
        } else if (event.type == SDL_CONTROLLERDEVICEADDED) {
            add_controller(event.cdevice.which);
        } else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
            switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_BACK:
                return false;
            }
        }
    }

    return true;
}

void
PlatformPriv::add_controller(int joystick_index)
{
    if (!controller) {
        controller = SDL_GameControllerOpen(joystick_index);
    }
}

bool
Platform::is_big_endian()
{
    return SDL_BYTEORDER == SDL_BIG_ENDIAN;
}

void
Platform::register_audio(audio_func_t audio_func, void *user_data)
{
    platform_priv->audio_func = audio_func;
    platform_priv->audio_func_user_data = user_data;
}

void
Platform::set_playing(bool enabled)
{
    SDL_PauseAudio(!enabled);
}

void
Platform::set_fullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(platform_priv->window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

const char *
Platform::storage_folder()
{
#if defined(__wii__) || defined(__gamecube__)
    const char *folder = "/apps/chromono/";
    ::mkdir(folder, 0777);
    return folder;
#else
    char *folder = NULL;

    if (folder == NULL) {
        // https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
        const char *home = getenv("XDG_CONFIG_HOME");
        if (home != NULL) {
            char *tmp;
            if (asprintf(&tmp, "%s/chromono", home) != -1) {
                folder = tmp;
            }
        }

        if (folder == NULL) {
            home = getenv("HOME");
            if (home != NULL) {
                char *tmp;
                if (asprintf(&tmp, "%s/.config/chromono", home) != -1) {
                    folder = tmp;
                }
            }
        }

        if (folder != NULL) {
            ::mkdir(folder, 0777);
        }
    }

    return folder;
#endif
}

/* We need to define the SDL_mutex structure in order to subclass from it. The
 * actual contents don't matter, since we aren't really allocating it here. */
struct SDL_mutex {};

class Platform::MutexPriv: public SDL_mutex {};

Platform::Mutex::Mutex():
    priv(static_cast<Platform::MutexPriv*>(SDL_CreateMutex()))
{
}

Platform::Mutex::~Mutex()
{
    SDL_DestroyMutex(priv);
}

bool Platform::Mutex::lock()
{
    return SDL_LockMutex(priv) == 0;
}

void Platform::Mutex::unlock()
{
    SDL_UnlockMutex(priv);
}

long
Util::ticks()
{
    return SDL_GetTicks();
}

int
main(int argc, char *argv[])
{
    PlatformPriv priv;

    priv.window = SDL_CreateWindow("chro.mono",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
#if defined(__wii__) || defined(__gamecube__)
            640, 480,
#else
            Constants::WORLD_WIDTH, Constants::WORLD_HEIGHT,
#endif
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GLContext glcontext = SDL_GL_CreateContext(priv.window);

    SDL_GetWindowSize(priv.window, &(priv.width), &(priv.height));

    Game game;
    game.resize(priv.width, priv.height);

    while (true) {
        if (!platform_priv->process(&game)) {
            break;
        }

        game.render();
        SDL_GL_SwapWindow(priv.window);
    }

    SDL_GL_DeleteContext(glcontext);

    return 0;
}
