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

#ifndef SHADYPOSTPROC_LEVELMANAGER_H
#define SHADYPOSTPROC_LEVELMANAGER_H

#include "shadypostproc.h"

#include "circle1d.h"

#include <vector>

class LevelManager {
    public:
        LevelManager(Scene *scene);
        virtual ~LevelManager();

        void start(int level);
        int current() { return m_current_level; }
        void set_current(int level) { m_current_level = level; }
        int count() { return m_levels.size(); }

        LevelInfo *get(int level);

    private:
        Scene *m_scene;
        std::vector<LevelInfo*> m_levels;
        int m_current_level;
};

#endif /* SHADYPOSTPROC_LEVELMANAGER_H */
