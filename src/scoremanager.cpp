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


#include "scoremanager.h"

#include "platform.h"

#include <string.h>

static bool
on_scores_load(void *buffer, void *user_data)
{
    ScoreManager *score_manager = (ScoreManager*)user_data;

    score_manager->recalculate();

    // Assume always true for now
    return true;
}

ScoreManager::ScoreManager(LevelManager *level_manager, StorageManager *storage_manager)
    : m_level_manager(level_manager)
    , m_best_times(new long[level_manager->count()])
    , m_stars(-1)
    , m_any_played(false)
    , m_storable(NULL)
{
    for (int i=0; i<level_manager->count(); i++) {
        m_best_times[i] = -1;
    }

    m_storable = new StorableStruct(storage_manager,
            m_best_times, sizeof(long)*level_manager->count(),
            on_scores_load, this);

    recalculate();
}

ScoreManager::~ScoreManager()
{
    delete m_storable;

    delete [] m_best_times;
}

bool
ScoreManager::register_score(int level, long ticks)
{
    long current = m_best_times[level];
    if (current == -1 || ticks < current) {
        m_best_times[level] = ticks;
        m_storable->save_data();
        recalculate();
        return true;
    }
    return false;
}

int
ScoreManager::get_stars(int level)
{
    return get_stars_for(level, m_best_times[level]);
}

long
ScoreManager::get_score(int level)
{
    return m_best_times[level];
}

int
ScoreManager::get_stars_for(int level, long ticks)
{
    int stars = 0;

    LevelInfo *info = m_level_manager->get(level);
    for (int i=0; i<3; i++) {
        if (ticks != -1 && ticks <= info->stars_time[i]) {
            stars++;
        }
    }

    return stars;
}

void
ScoreManager::recalculate()
{
    m_stars = 0;
    m_any_played = false;

    for (int i=0; i<m_level_manager->count(); i++) {
        long score = get_score(i);
        if (score != -1) {
            m_any_played = true;
        }
        m_stars += get_stars(i);
    }
}

