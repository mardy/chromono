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


#include "levelmanager.h"

LevelManager::LevelManager(Scene *scene)
    : m_scene(scene)
    , m_levels()
    , m_current_level(0)
{
    int i = 0;
    while (levels[i].constructor != NULL) {
        m_levels.push_back(&(levels[i]));
        i++;
    }

    start(0);
}

void
LevelManager::start(int level)
{
    SHADY_ASSERT(level >= 0 && level < count());

    m_scene->reset();
    m_levels[level]->constructor(m_scene);
    m_scene->background_color_from_content();
    m_current_level = level;
}


LevelInfo *
LevelManager::get(int level)
{
    SHADY_ASSERT(level >= 0 && level < count());

    return m_levels[level];
}

LevelManager::~LevelManager()
{
}

