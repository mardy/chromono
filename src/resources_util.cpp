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


#include "resources_util.h"
#include "util.h"

#include <zlib.h>

ResourceAccess::ResourceAccess(Resource *resource)
    : m_resource(resource)
    , m_buffer(0)
    , m_length(0)
{
    m_length = resource->uncompressed_size;
    m_buffer = (unsigned char *)malloc(m_length);
    uLongf destLen = m_length;

    int result = uncompress(m_buffer, &destLen, resource->data, resource->compressed_size);
    SHADY_ASSERT(result == Z_OK);
    SHADY_ASSERT(destLen == resource->uncompressed_size);
}

ResourceAccess::~ResourceAccess()
{
    free(m_buffer);
}

const char *
ResourceAccess::name()
{
    return m_resource->name;
}

unsigned char *
ResourceAccess::data()
{
    return m_buffer;
}

size_t
ResourceAccess::size()
{
    return m_length;
}
