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


#include "transition.h"

#include "util.h"

Transition::Transition()
    : m_started(0)
    , m_duration(0)
    , m_listener(NULL)
    , m_running(false)
{
}

Transition::~Transition()
{
}

void
Transition::handle(Circle1DEvent *event)
{
    if (!m_running) {
        return;
    }

    if (event->type == Circle1DEvent::TICK) {
        Circle1DTime now = Util::ticks();
        float value = (float)(now - m_started) / (float)m_duration;
        if (value >= 1.0) {
            m_running = false;
            m_listener->on_finished();
        } else {
            m_listener->on_update(value);
        }
    }
}

void
Transition::start(Circle1DTime duration, TransitionListener *listener)
{
    m_started = Util::ticks();
    m_duration = duration;
    m_listener = listener;
    m_running = true;
}

