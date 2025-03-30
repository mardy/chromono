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

#ifndef SHADYPOSTPROC_SCOREMANAGER_H
#define SHADYPOSTPROC_SCOREMANAGER_H

#include "levelmanager.h"
#include "storagemanager.h"

class ScoreManager {
    public:
        ScoreManager(LevelManager *level_manager, StorageManager *storage_manager);
        virtual ~ScoreManager();

        // Game registers score after level is finished
        // returns true if this is a new high score, false otherwise
        bool register_score(int level, long ticks);

        // Return number of earned stars (0-3) for a given level
        int get_stars(int level);

        // Return the raw best score for a level
        long get_score(int level);

        // Get the amount of stars the user gets for a given score
        int get_stars_for(int level, long ticks);

        int stars() { return m_stars; }
        bool any_played() { return m_any_played; }

        // Recalculate stars counts based on scores (consider private)
        void recalculate();
    private:

        LevelManager *m_level_manager;
        long *m_best_times;

        /* Cached values */
        int m_stars;
        bool m_any_played;

        /* Storage handle */
        StorableStruct *m_storable;
};

#endif /* SHADYPOSTPROC_SCOREMANAGER_H */
