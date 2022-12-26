// fast32 isn't size 4, its a long, shit lies
#ifndef _N_SHARED_
#define _N_SHARED_

#ifdef __unix__
#    include <unistd.h>
#    include <signal.h>
#    include <poll.h>
#    include <sys/stat.h>
#else // this should never happen, but if it does, then oh hell, you'll have to port it over
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
/*
#elif _MSVC_VER
#   ifdef _M_X86
#       define _NOMAD_32
#   elif defined(_M_X64)
#       define _NOMAD_64
#   endif
*/
#endif

class Game;

#ifndef _N_TYPES_
#define _N_TYPES_

#include <stdint.h>

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
typedef double nomadfloat_t;
#elif _NOMAD_32
typedef int64_t nomadllong_t;
typedef int32_t nomadlong_t;
typedef int16_t nomadint_t;
typedef int8_t nomadshort_t;
typedef uint64_t noamdullong_t;
typedef uint32_t nomadulong_t;
typedef uint16_t nomaduint_t;
typedef uint8_t nomadushort_t;
typedef float nomadfloat_t;
#endif

// these types don't depend on the arch
typedef uint_fast8_t nomadenum_t;
typedef nomadenum_t sprite_t;

#define chtype chtype_small
typedef sprite_t chtype;

#ifdef __cplusplus
typedef bool nomadbool_t;
#else
typedef enum{false, true} nomadbool_t;
#endif

#endif

#include <pthread.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
//#include <atomic>
#include <ctime>
#include <cmath>
#include <fstream>

/*
#ifdef _WIN32
#ifdef __cplusplus
nomadbool_t kbhit(nomadushort_t& in);
#else
nomadbool_t kbhit(nomadushort_t* in);
#endif
#endif
*/

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

inline nomaduint_t disBetweenOBJ(coord_t src, coord_t tar);
float Q_root(float x);

using namespace std::literals::chrono_literals;

#endif
