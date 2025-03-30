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


#include "messagebox.h"

#include "constants.h"

#include <string.h>

MessageBox::MessageBox()
    : m_message("")
    , m_opened(false)
{
}

MessageBox::~MessageBox()
{
}

void
MessageBox::show(const char *message)
{
    m_message = message;
    m_opened = true;
}

void
MessageBox::render(OpenGLRenderer *renderer)
{
    float w, h;
    renderer->text_measure(m_message.c_str(), &w, &h, FONT_LARGE);
    renderer->text_render(m_message.c_str(), 
            (Constants::WORLD_WIDTH - w) / 2.0,
            (Constants::WORLD_HEIGHT - h) / 2.0, FONT_LARGE);
}

void
MessageBox::handle(Circle1DEvent *event)
{
    if (event->type == Circle1DEvent::MOUSEUP) {
        m_opened = false;
    }
}

