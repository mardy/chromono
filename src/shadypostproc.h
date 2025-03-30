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

#ifndef SHADYPOSTPROC_SHADYPOSTPROC_H
#define SHADYPOSTPROC_SHADYPOSTPROC_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <algorithm>

#define USE_DEBUG_PRINTF

#if defined(USE_DEBUG_PRINTF)
#  define SHADY_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#  define SHADY_ASSERT(x) do { if (!(x)) { SHADY_DEBUG_PRINTF("assertion failed: %s\n", #x); exit(1); } } while(0)
#else
#  define SHADY_DEBUG_PRINTF(...)
#  define SHADY_ASSERT(x) (void)(x)
#endif


#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

class Game;
typedef void (*game_lambda_t)(Game *);

#endif /* SHADYPOSTPROC_SHADYPOSTPROC_H */
