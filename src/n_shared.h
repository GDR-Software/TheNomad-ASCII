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
#if !defined(__unix__) && !defined(_WIN32)
#   error CURRENT OS NOT COMPATIBLE WITH THE NOMAD ASCII!
#endif

#ifndef __cplusplus
#   error COMPILE WITH C++!
#endif

#if __cplusplus < 201703L
#   error COMPILE WITH C++17 OR HIGHER!
#endif

#ifdef _WIN32
#define _NOMAD_32
#elif defined(_WIN64)
#define _NOMAD_64
#endif

#if defined(__GNUG__) || defined(__clang__) && !defined(_WIN32)
#   if defined(__x86_64__) || defined(__amd64) || defined(__i686__)
#       define _NOMAD_64
#   elif defined(__i586__) || defined(__i486__) || defined(__i386__)
#       define _NOMAD_32
#   else
#       error UNKNOWN ARCHITECTURE!
#   endif
#elif defined(_MSVC_VER) && !defined(_NOMAD_64) && !defined(_NOMAD_32)
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
#elif defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   pragma comment(lib, "libloaderapi2")
#   include <libloaderapi2.h>
#   include <conio.h>
#endif

#ifdef __unix__
#   define UNIX_NOMAD
#   ifdef REPLIT
#   include <ncurses/ncurses.h>
#   include <ncurses/menu.h>
#   else
#   include <ncurses.h>
#   include <menu.h>
#   endif
#elif defined(_WIN32)
#   define WIN32_NOMAD
#   include <ncursesw/ncurses.h>
#   include <ncursesw/menu.h>
#endif
#ifndef REPLIT
#include <mpg123.h>
#include <out123.h>
#include <syn123.h>
#endif

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
#include <iomanip>
#include <fstream>
#include <pthread.h>

/*
 * ISC License
 * 
 * Copyright 2022 Michael M.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * colors.h - single header colors in C
 * https://github.com/michaelfm1211/colors.h
 */

#pragma once

/*
 * Select Graphic Rendition parameters
 */

// use a SGR parameter not supported by colors.h. ex: alternate fonts
#define SGR_N(n) "\x1b["#n"m"

#define SGR_RESET "\x1b[m"
#define SGR_BOLD "\x1b[1m"
#define SGR_FAINT "\x1b[2m"
#define SGR_ITALIC "\x1b[3m"
#define SGR_UNDER "\x1b[4m"
#define SGR_BLINK "\x1b[5m"
#define SGR_BLINK_RAPID "\x1b[6m"
#define SGR_INVERT "\x1b[7m"
#define SGR_HIDE "\x1b[8m"
#define SGR_STRIKE "\x1b[9m"
#define SGR_DEF_FONT "\x1b[10m"
#define SGR_GOTHIC "\x1b[20m"
#define SGR_2UNDER "\x1b[21m"
#define SGR_DEF_WEIGHT "\x1b[22m"
#define SGR_NO_ITALIC "\x1b[23m"
#define SGR_NO_GOTHIC "\x1b[23m"
#define SGR_NO_UNDER "\x1b[24m"
#define SGR_NO_BLINK "\x1b[25m"
#define SGR_MONOSPACE "\x1b[26m"
#define SGR_NO_INVERT "\x1b[27m"
#define SGR_NO_HIDE "\x1b[28m"
#define SGR_NO_STRIKE "\x1b[29m"
// there's a bunch of color related SGRs. they're under the C_ instead, but
// everyting under here is already obscure anyway
#define SGR_NO_MONOSPACE "\x1b[50m"
#define SGR_FRAME "\x1b[51m"
#define SGR_CIRCLE "\x1b[52m"
#define SGR_OVER "\x1b[53m"
#define SGR_NO_FRAME "\x1b[54m"
#define SGR_NO_CIRCLE "\x1b[54m"
#define SGR_NO_OVER "\x1b[55m"
#define SGR_NO_FRAME "\x1b[54m"
#define SGR_IDEO_UNDER "\x1b[60m"
#define SGR_R_SIDE "\x1b[60m"
#define SGR_IDEO_2UNDER "\x1b[61m"
#define SGR_R_2SIDE "\x1b[61m"
#define SGR_IDEO_OVER "\x1b[62m"
#define SGR_L_SIDE "\x1b[62m"
#define SGR_IDEO_2OVER "\x1b[63m"
#define SGR_L_2SIDE "\x1b[63m"
#define SGR_IDEO_STRESS "\x1b[64m"
#define SGR_NO_IDEO "\x1b[65m"
#define SGR_NO_SIDE "\x1b[65m"
#define SGR_SUPER "\x1b[73m"
#define SGR_SUB "\x1b[74m"
#define SGR_NO_SUPER "\x1b[75m"
#define SGR_NO_SUB "\x1b[75m"


/*
 * Colors
 * Key:
 * C_{color} = text color
 * C_BR_{color} = bright text color
 * C_BG_{color} = background color
 * C_BG_BR_{color} = bright background color
 */

#define C_DEF "\x1b[39m"
#define C_BG_DEF "\x1b[49m"

#define C_BLACK "\x1b[30m"
#define C_RED "\x1b[31m"
#define C_GREEN "\x1b[32m"
#define C_YELLOW "\x1b[33m"
#define C_BLUE "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN "\x1b[36m"
#define C_WHITE "\x1b[37m"
#define C_RESET "\x1b[0m"

#define C_BR_BLACK "\x1b[90m"
#define C_GRAY "\x1b[90m"
#define C_BR_RED "\x1b[91m"
#define C_BR_GREEN "\x1b[92m"
#define C_BR_YELLOW "\x1b[93m"
#define C_BR_BLUE "\x1b[94m"
#define C_BR_MAGENTA "\x1b[95m"
#define C_BR_CYAN "\x1b[96m"
#define C_BR_WHITE "\x1b[97m"

#define C_BG_BLACK "\x1b[40m"
#define C_BG_RED "\x1b[41m"
#define C_BG_GREEN "\x1b[42m"
#define C_BG_YELLOW "\x1b[43m"
#define C_BG_BLUE "\x1b[44m"
#define C_BG_MAGENTA "\x1b[45m"
#define C_BG_CYAN "\x1b[46m"
#define C_BG_WHITE "\x1b[47m"

#define C_BG_BR_BLACK "\x1b[100m"
#define C_BG_GRAY "\x1b[100m"
#define C_BG_BR_RED "\x1b[101m"
#define C_BG_BR_GREEN "\x1b[102m"
#define C_BG_BR_YELLOW "\x1b[103m"
#define C_BG_BR_BLUE "\x1b[104m"
#define C_BG_BR_MAGENTA "\x1b[105m"
#define C_BG_BR_CYAN "\x1b[106m"
#define C_BG_BR_WHITE "\x1b[107m"

// select from the 256-color table. see table at:
// https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
#define C_256(n) "\x1b[38;5;"#n"m"
#define C_BG_256(n) "\x1b[48;5;"#n"m"

// enter a RGB value
#define C_RGB(r, g, b) "\x1b[38;2;"#r";"#g";"#b"m"
#define C_BG_RGB(r, g, b) "\x1b[48;2;"#r";"#g";"#b"m"


#if !defined(NDEBUG) && !defined(RELEASE)
#include <assert.h>
#define _NOMAD_DEBUG
#endif

extern FILE* dbg_file;
#define LOGGER_OUTFILE dbg_file 
#include "n_debug.h"

#define byte unsigned char

void N_Error(const char* err, ...);

#ifndef _N_TYPES_
#define _N_TYPES_

// these types depend on your OS
#ifdef UNIX_NOMAD
typedef __int128_t nomadllong_t;
typedef int64_t nomadlong_t;
typedef int32_t nomadint_t;
typedef int16_t nomadshort_t;

typedef uint_fast8_t nomadenum_t;

typedef __uint128_t nomadullong_t;
typedef uint64_t nomadulong_t;
typedef uint32_t nomaduint_t;
typedef uint16_t nomadushort_t;
#elif defined(WIN32_NOMAD)
typedef long long nomadllong_t;
typedef INT64 nomadlong_t;
typedef INT32 nomadint_t;
typedef INT16 nomadshort_t;

typedef UINT8 nomadenum_t;

typedef unsigned long long nomadullong_t;
typedef UINT64 nomadulong_t;
typedef UINT32 nomaduint_t;
typedef UINT16 nomadushort_t;
#endif

#define NOMAD_VERSION_MAJOR _NOMAD_VERSION
#define NOMAD_VERSION_MINOR _NOMAD_VERSION_UPDATE
#define NOMAD_VERSION_PATCH _NOMAD_VERSION_PATCH
#define NOMAD_UPDATE_NUM _NOMAD_VERSION_UPDATE

#define NOMAD_VERSION (NOMAD_VERSION_MAJOR * 10000 \
                     + NOMAD_VERSION_MINOR * 100 \
                     + NOMAD_VERSION_PATCH)

// these types don't depend on the arch
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

inline char kb_hit()
{
#ifdef __unix__
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
#elif defined(_WIN32)
	if (_kbhit()) return _getch();
	else return -1;
#endif
}


namespace std {
	size_t filelength(const char* filename);
	size_t filelength(const std::string& filename);
};

enum dir : nomadenum_t
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
	nomadshort_t y, x;
    auto& operator[](nomadenum_t i) {
        switch (i) {
        case 0: return y;
        case 1: return x;
        };
		LOG_WARN("Invalid call to coord_s operator[], returning y");
		return y;
    }
	inline bool operator==(struct coord_s c) const {
		return (x == c.x && y == c.y);
	}
	inline bool operator!=(struct coord_s c) const {
		return (x != c.x && y != c.y);
	}
	inline bool operator>(struct coord_s c) const {
		return (x > c.x && y > c.y);
	}
	inline bool operator<(struct coord_s c) const {
		return (x < c.x && y < c.y);
	}
	inline bool operator>=(struct coord_s c) const {
		return (x >= c.x && y >= c.y);
	}
	inline bool operator<=(struct coord_s c) const {
		return (x <= c.x && y <= c.y);
	}
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
		LOG_WARN("Invalid call to area operator[], returning tl");
		return tl;
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
	return nullptr;
}
static nomadenum_t StrToDir(const char* str)
{
	if (strcmp(str, "D_NORTH")) return D_NORTH;
	else if (strcmp(str, "D_WEST")) return D_WEST;
	else if (strcmp(str, "D_SOUTH")) return D_SOUTH;
	else if (strcmp(str, "D_EAST")) return D_EAST;
	else return D_NULL;
}

typedef struct
{
	nomadenum_t height, width;
} dim_t;

typedef int32_t nomadfixed_t;

typedef struct
{
	coord_t where;
	nomaduint_t what;
	void *ptr = nullptr;
} collider_t;

inline auto dirtostr(nomadenum_t dir) -> const char* { return DirToStr(dir); };
inline auto strtodir(const char* str) -> nomadenum_t { return StrToDir(str); };
inline auto strtobool(const char* str) -> nomadbool_t { return strcmp(str, "true") ? true : false; };
inline auto booltostr(bool b) -> const char* { return b ? "true" : "false"; };
collider_t G_CastRay(coord_t endpoint, coord_t startpoint, Game* const game);
nomadbool_t G_CheckCollider(coord_t point, Game* const game, collider_t& c);
inline nomadfloat_t Q_root(nomadfloat_t x);
nomadint_t disBetweenOBJ(coord_t src, coord_t tar);
coord_t closestOBJ(const std::vector<coord_t>& coords, const coord_t src);

using namespace std::literals::chrono_literals;

#endif
