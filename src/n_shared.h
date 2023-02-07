//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/n_shared.h
//  the big, chunky header that's supposed to be the first include
//  in every header file within the main /src directory.
//  DO NOT MODIFY UNLESS YOU KNOW WHAT YOU ARE DOING!!!!
//----------------------------------------------------------

// fast32 isn't size 4, its a long, shit lies
#ifndef _N_SHARED_
#define _N_SHARED_

// if you got this, well then port it
#if !defined(__unix__)
#   error CURRENT OS NOT COMPATIBLE WITH THE NOMAD ASCII!
#endif

#ifndef __cplusplus
#   error COMPILE WITH C++!
#endif

#if __cplusplus < 201703L
#   error COMPILE WITH C++17 OR HIGHER!
#endif

#if defined(__GNUG__) || defined(__clang__)
#   if defined(__x86_64__) || defined(__amd64) || defined(__i686__)
#       define _NOMAD_64
#   elif defined(__i586__) || defined(__i486__) || defined(__i386__)
#       define _NOMAD_32
#   else
#       error UNKNOWN ARCHITECTURE!
#   endif
#elif defined(_MSVC_VER)
#   if defined(_M_X64)
#       define _NOMAD_64
#   elif defined(_M_X86)
#       define _NOMAD_32
#   else
#       error UNKNOWN ARCHITECTURE!
#   endif
#else
#   error UNSUPPORTED COMPILER!
#endif

#include "nomaddef.h"

#define __CFUNC__ extern "C"

class Game;

#ifdef __unix__
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/stat.h>
#   include <sys/mman.h>
#   include <termios.h>
#   include <signal.h>
#   include <dlfcn.h>
#endif

#include <ncurses.h>
#include <menu.h>
#include <mpg123.h>
#include <out123.h>
#include <syn123.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <sstream>
#include <atomic>
#include <vector>
#include <algorithm>
#include <string>
#include <assert.h>
#include <fstream>
#include <pthread.h>

#ifdef _NOMAD_DEBUG
#include "n_debug.h"
#endif

#define byte uint_fast8_t

void N_Error(const char* err, ...);

#ifndef _N_TYPES_
#define _N_TYPES_

// these types depend on your arch/OS
#ifdef _NOMAD_64
typedef __int128_t nomadllong_t;
typedef int64_t nomadlong_t;
typedef int32_t nomadint_t;
typedef int16_t nomadshort_t;

typedef __uint128_t nomadullong_t;
typedef uint64_t nomadulong_t;
typedef uint32_t nomaduint_t;
typedef uint16_t nomadushort_t;
//typedef double nomadfloat_t;
#elif _NOMAD_32
typedef int64_t nomadllong_t;
typedef int32_t nomadlong_t;
typedef int16_t nomadint_t;
typedef int8_t nomadshort_t;

typedef uint64_t noamdullong_t;
typedef uint32_t nomadulong_t;
typedef uint16_t nomaduint_t;
typedef uint8_t nomadushort_t;
#endif

#define NOMAD_VERSION_MAJOR _NOMAD_VERSION
#define NOMAD_VERSION_MINOR _NOMAD_VERSION_UPDATE
#define NOMAD_VERSION_PATCH _NOMAD_VERSION_PATCH
#define NOMAD_UPDATE_NUM _NOMAD_VERSION_UPDATE

#define NOMAD_VERSION (NOMAD_VERSION_MAJOR * 10000 \
                     + NOMAD_VERSION_MINOR * 100 \
                     + NOMAD_VERSION_PATCH)

// these types don't depend on the arch
typedef uint_fast8_t nomadenum_t;
typedef nomadenum_t sprite_t;
typedef float nomadfloat_t;

#define chtype chtype_small
typedef sprite_t chtype;

#ifdef __cplusplus
typedef bool nomadbool_t;
#else
typedef enum{false, true} nomadbool_t;
#endif

#endif

constexpr uint8_t DIF_NOOB               = 0;
constexpr uint8_t DIF_RECRUIT            = 1;
constexpr uint8_t DIF_MERC               = 2;
constexpr uint8_t DIF_NOMAD              = 3;
constexpr uint8_t DIF_BLACKDEATH         = 4;
constexpr uint8_t DIF_MINORINCONVENIENCE = 5;
constexpr uint8_t NUMDIFS                = 6;

inline char kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	if ((FD_ISSET(STDIN_FILENO, &fds))) {
		return getc(stdin);
	}
	else {
		return -1;
	}
}

inline const char* booltostr(bool b) { return b ? "true" : "false"; }


namespace std {
	size_t filelength(const char* filename);
	size_t filelength(const std::string& filename);
};

enum : nomadenum_t
{
	D_NORTH,
	D_WEST,
	D_SOUTH,
	D_EAST,
	NUMDIRS,
	D_NULL
};

enum : nomadenum_t
{
	A_TN, // true neutral
	A_LG, // lawful good
	A_LE, // lawful evil
	A_CG, // chaotic good
	A_CE, // chaotic evil
	A_LN, // lawful neutral
	A_CN, // chaotic neutral

	NUMALIGNMENTS
};

typedef struct coord_s
{
#ifdef _NOMAD_64
	nomadshort_t y, x;
#elif _NOMAD_32
	nomadint_t y, x;
#endif
    auto& operator[](nomadenum_t i) {
        switch (i) {
        case 0: return y;
        case 1: return x;
        };
    }
    inline bool operator==(struct coord_s c) const;
    inline bool operator>(struct coord_s c) const;
	inline bool operator<(struct coord_s c) const;
	inline bool operator>=(struct coord_s c) const;
	inline bool operator<=(struct coord_s c) const;
	inline bool operator!=(struct coord_s c) const;
} coord_t, vec2_t;

typedef struct
{
    coord_t tl;
    coord_t tr;
    coord_t bl;
    coord_t br;
    coord_t& operator[](nomadenum_t i) {
        switch (i) {
        case 0: return tl;
        case 1: return tr;
        case 2: return bl;
        case 3: return br;
        };
        assert(false);
    }
} area_t;

#define ctrl(x) (x & 0x1F)
#define VAR_TO_STR(x) #x
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static const char* DirToStr(nomadenum_t dir)
{
	switch (dir) {
	case D_NORTH: return VAR_TO_STR(D_NORTH);
	case D_WEST: return VAR_TO_STR(D_WEST);
	case D_SOUTH: return VAR_TO_STR(D_SOUTH);
	case D_EAST: return VAR_TO_STR(D_EAST);
	};
	if (!false)
		N_Error("Unknown/Invalid Entity Direction %hu!", dir);
}

typedef struct
{
	nomadenum_t height, width;
} dim_t;

typedef int32_t nomadfixed_t;

inline nomadfloat_t Q_root(nomadfloat_t x);
nomadint_t disBetweenOBJ(coord_t src, coord_t tar);
coord_t closestOBJ(const std::vector<coord_t>& coords, const coord_t src);

using namespace std::literals::chrono_literals;

#endif
