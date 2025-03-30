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

#ifndef SHADYPOSTPROC_TRANSITION_H
#define SHADYPOSTPROC_TRANSITION_H

#include "shadypostproc.h"

#include "circle1d.h"

class TransitionListener {
    public:
        virtual void on_update(float value) = 0;
        virtual void on_finished() = 0;
};

class Transition : public Circle1DEventHandler {
    public:
        Transition();
        virtual ~Transition();

        bool running() { return m_running; }

        void handle(Circle1DEvent *event);

        void start(Circle1DTime duration, TransitionListener *listener);

    private:
        Circle1DTime m_started;
        Circle1DTime m_duration;
        TransitionListener *m_listener;
        bool m_running;
};

#endif /* SHADYPOSTPROC_TRANSITION_H */
