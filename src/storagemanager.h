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

#ifndef SHADYPOSTPROC_STORAGEMANAGER_H
#define SHADYPOSTPROC_STORAGEMANAGER_H

#include "shadypostproc.h"

#include <string.h>
#include <list>

class Storable;

class StorageManager {
    public:
        StorageManager(char *filename);
        ~StorageManager();

        // Add new storable to list of storables
        void register_storable(Storable *storable);

        // Load data from disk and send to storables
        void load_data();

        // Save data to disk and read from storables
        void save_data();

    private:
        // On error, read defaults
        void read_defaults();

        std::list<Storable*> m_storables;
        char *m_filename;
};

class Storable {
    public:
        Storable(StorageManager *storage_manager)
            : storage_manager(storage_manager)
        {
            storage_manager->register_storable(this);
        }

        // Amount (in bytes) this storable needs to store
        virtual size_t storage_requirement() = 0;

        // Read storage_requirement() bytes from buffer on load
        // Return true if reading was successful, false if data is invalid
        virtual bool read_from(void *buffer) = 0;

        // Read defaults (e.g. when storage is damaged or newly created)
        virtual void read_defaults() = 0;

        // Write storage_requirement() bytes to buffer on save
        virtual void write_to(void *buffer) = 0;

        // Helper function that requests saving from the storage manager
        void save_data()
        {
            storage_manager->save_data();
        }

    protected:
        StorageManager *storage_manager;
};

/* Lambda function for validating the contents of an arbitrary buffer */
typedef bool (*validate_struct_func_t)(void *buffer, void *user_data);

class StorableStruct : public Storable {
    public:
        /**
         * Helper class that eases storing of arbitrary structs in a storage
         * manager. The default values from the struct are taken from the
         * contents of the buffer at construction-time. An optional validation
         * function can be used to validate data read in from the file.
         **/
        StorableStruct(StorageManager *storage_manager, void *buffer, size_t len,
                validate_struct_func_t validate_struct_func=NULL,
                void *validate_struct_func_user_data=NULL)
            : Storable(storage_manager)
            , buffer(buffer)
            , len(len)
            , validate_struct_func(validate_struct_func)
            , validate_struct_func_user_data(validate_struct_func_user_data)
            , defaults(malloc(len))
        {
            // Defaults are the initial buffer contents
            memcpy(defaults, buffer, len);
        }

        virtual ~StorableStruct()
        {
            free(defaults);
        }

        virtual size_t
        storage_requirement()
        {
            return len;
        }

        virtual bool
        read_from(void *buffer)
        {
            memcpy(this->buffer, buffer, len);
            if (validate_struct_func != NULL) {
                return validate_struct_func(this->buffer,
                        validate_struct_func_user_data);
            }
            return true;
        }

        virtual void
        read_defaults()
        {
            memcpy(buffer, defaults, len);
        }

        virtual void
        write_to(void *buffer)
        {
            memcpy(buffer, this->buffer, len);
        }

    private:
        void *buffer;
        size_t len;

        validate_struct_func_t validate_struct_func;
        void *validate_struct_func_user_data;

        void *defaults;
};

#endif /* SHADYPOSTPROC_STORAGEMANAGER_H */
