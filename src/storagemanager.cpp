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


#include "storagemanager.h"


StorageManager::StorageManager(char *filename)
    : m_storables()
    , m_filename(filename)
{
}

StorageManager::~StorageManager()
{
    free(m_filename);
}

void
StorageManager::register_storable(Storable *storable)
{
    m_storables.push_back(storable);
}

void
StorageManager::load_data()
{
    FILE *fp = fopen(m_filename, "rb");
    if (fp) {
        std::list<Storable*>::iterator it;
        for (it=m_storables.begin(); it != m_storables.end(); ++it) {
            size_t len = (*it)->storage_requirement();
            void *buffer = malloc(len);

            if (fread(buffer, len, 1, fp) == 1) {
                if (((*it)->read_from(buffer))) {
                    free(buffer);
                    continue;
                }
            }

            SHADY_DEBUG_PRINTF("Cannot read from %s\n", m_filename);
            read_defaults();
            free(buffer);
            break;
        }
        fclose(fp);
    } else {
        read_defaults();
    }
}

void
StorageManager::read_defaults()
{
    std::list<Storable*>::iterator it;
    for (it=m_storables.begin(); it != m_storables.end(); ++it) {
        (*it)->read_defaults();
    }
}

void
StorageManager::save_data()
{
    FILE *fp = fopen(m_filename, "wb");
    if (fp != NULL) {
        std::list<Storable*>::iterator it;
        for (it=m_storables.begin(); it != m_storables.end(); ++it) {
            size_t len = (*it)->storage_requirement();
            void *buffer = malloc(len);
            (*it)->write_to(buffer);
            if (fwrite(buffer, len, 1, fp) != 1) {
                SHADY_DEBUG_PRINTF("Cannot write to %s\n", m_filename);
                break;
            }
            free(buffer);
        }
        fclose(fp);
    }
}

