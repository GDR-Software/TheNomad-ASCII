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

// if you got this, well then, port it
#if !defined(__unix__) || (_WIN32)
#   error CURRENT OS NOT COMPATIBLE WITH THE NOMAD ASCII!
#endif

#if __cplusplus < 201703L
#   error COMPILE WITH C++17 OR HIGHER!
#endif

// dependencies
#include "ncurses/ncurses.h"

#ifdef __GNUG__
#   if defined(__x86_64__) || (__amd64)
#       define _NOMAD_64
#   elif defined(__i586__) || (__i386__) || (__i486__)
#       define _NOMAD_32
#   endif
#elif _MSVC_VER
#   ifdef _M_X86
#       define _NOMAD_32
#   elif defined(_M_X64)
#       define _NOMAD_64
#   endif
#else
#    error UNSUPPORTED COMPILER!
#endif

#define __CFUNC__ extern "C"

class Game;

#include <fstream>
#include <vector>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <exception>
#include <thread>
#include <chrono>
#include <atomic>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <stdarg.h>

// OS-specific headers
#if defined(__unix__)
#   include <unistd.h>
#   include <sys/stat.h>
#   include <termios.h>
#   include <fcntl.h>
#   include <signal.h>
#elif defined(_WIN32)
#   include <conio.h>
#   include <windows.h>
#   include <dos.h>
#   include <bios.h>
#endif

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

inline nomadenum_t kbhit()
{
/*#ifdef _WIN32 // broken - FIXME
	if (_kbhit()) {
		in = (nomadushort_t)_getch();
		return true;
	}
	else {
		return false;
	} */
//#elif defined(__unix__)
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
		return 0;
	}
//#endif
}

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

typedef struct
{
#ifdef _NOMAD_64
	nomadshort_t y, x;
#elif _NOMAD_32
	nomadint_t y, x;
#endif
} coord_t;

typedef struct
{
	nomadenum_t height, width;
} dim_t;

nomaduint_t disBetweenOBJ(const coord_t src, const coord_t tar);
coord_t closestOBJ(const std::vector<coord_t>& coords, const coord_t src);
nomadfloat_t Q_root(nomadfloat_t x);

using namespace std::literals::chrono_literals;

#endif
