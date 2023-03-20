#ifndef _N_SHARED_
#define _N_SHARED_

#pragma once

#define NOMAD_VERSION_MAJOR _NOMAD_VERSION
#define NOMAD_VERSION_MINOR _NOMAD_VERSION_UPDATE
#define NOMAD_VERSION_PATCH _NOMAD_VERSION_PATCH

#define NVER NOMAD_VERSION_MAJOR
#define PATCH_NUM NOMAD_VERSION_PATCH

#define NOMAD_VERSION (NOMAD_VERSION_MAJOR * 10000 \
                     + NOMAD_VERSION_MINOR * 100 \
                     + NOMAD_VERSION_PATCH)

#ifndef NOMAD_EXPERIMENTAL
#if NVER == 0
#define NOMAD_VERSION_STR "The Nomad (Pre-Alpha) v" NOMAD_VERSION_MAJOR "." NOMAD_VERSION_MINOR "." NOMAD_VERSION_PATCH ""
#elif NVER == 1
#define NOMAD_VERSION_STR "The Nomad (Alpha) v" NOMAD_VERSION_MAJOR "." NOMAD_VERSION_MINOR "." NOMAD_VERSION_PATCH ""
#elif NVER == 2
#define NOMAD_VERSION_STR "The Nomad (Beta) v" NOMAD_VERSION_MAJOR "." NOMAD_VERSION_MINOR "." NOMAD_VERSION_PATCH""
#elif NVER == 3
#define NOMAD_VERSION_STR "The Nomad v" NOMAD_VERSION_MAJOR "." NOMAD_VERSION_MINOR "." NOMAD_VERSION_PATCH ""
#endif
#else
#define NOMAD_VERSION_STR "The Nomad (Experimental) v" NOMAD_VERSION_MAJOR "." NOMAD_VERSION_MINOR "." NOMAD_VERSION_PATCH ""
#endif

#if !defined(NDEBUG) && !defined(RELEASE)
#define _NOMAD_DEBUG
#endif

#if !defined(_WIN32) && !defined(__linux__)
#   error UNSUPPORTED OS FOR NOMAD-ASCII!
#endif

#ifndef __cplusplus
#   error COMPILE WITH C++!
#endif

#if __cplusplus < 201703L
#   error COMPILE WITH C++17 OR HIGHER!
#endif

#if defined(__GNUG__) || defined(__clang__)
#   if defined(__x86_64__) || defined(__amd64) || defined(__i686__) || defined(__x64__)
#       define _NOMAD_64
#   elif defined(__i586__) || defined(__i486__) || defined(__i386__)
#       define _NOMAD_32
#   else
#       error UNKNOWN ARCHITECTURE!
#   endif
#elif defined(_MSVC_VER)
#   warning Microsoft Visual Studio is not the main supported compiler for nomadascii
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

#if defined(__GNUG__) || defined(__clang__)
#   define WARN_RETN __attribute__((warn_unused_result))
#elif defined(_MSVC)
#   define WARN_RETN _Check_return
#endif

#include "nomaddef.h"
#include "n_pch.h"

#define __CFUNC__ extern "C"

class Game;

#include "n_stdlib.h"

enum
{
	SPR_WALL,
	SPR_PLAYR,
	SPR_WINDOW,
	SPR_DOOR_STATIC,
	SPR_LCABINET,
	SPR_RCABINET,
	SPR_DOOR_OPEN,
	SPR_DOOR_CLOSE,
	SPR_CHAIR,
	SPR_TABLE,
	SPR_MONEY,
	SPR_PICKUP,
	SPR_SECRET,
	SPR_FLOOR_OUTSIDE,
	SPR_FLOOR_INSIDE,
	SPR_GATE_LEFT_DOOR,
	SPR_GATE_RIGHT_DOOR,
	SPR_COUNTER_LEFT,
	SPR_COUNTER_RIGHT,
	SPR_THIN_WALL,
	SPR_TREE,
	SPR_ROCKS,
	SPR_BED,
	SPR_WATER,
	SPR_ROCKET,
	
	NUM_SPRITES
};

#define is_template_type(var,type) constexpr (std::is_integral<var>::value && !std::is_same<var, type>::value)
#define is_var_type(var,type) constexpr (std::is_integral<decltype(x)>::value && !std::is_same<decltype(x), type>::value)
#if 0
#define is_same_type(a, b)  __builtin_types_compatible_p(typeof(a), typeof(b))
#define is_pointer_or_array(p)  (__builtin_classify_type(p) == 5)
#define decay(p)  (&*__builtin_choose_expr(is_pointer_or_array(p), p, NULL))
#define is_pointer(p)  is_same_type(p, decay(p))
#endif

#define SPR(x) x

constexpr const char sprites[NUM_SPRITES] = {
	SPR('#'), SPR('@'), SPR('~'), SPR('_'), SPR('['), SPR(']'), SPR('>'), SPR('<'),
	SPR(':'), SPR('+'), SPR('$'), SPR('!'), SPR('?'), SPR('.'), SPR('/'), SPR('\\'),
	SPR(')'), SPR('('), SPR('='), SPR('|'), SPR('&'), SPR('%'), SPR(';'), SPR('*'),
};

void N_Error(const char *err, ...);

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

extern FILE* dbg_file;
extern FILE* p_file;
#define LOGGER_OUTFILE dbg_file

#ifdef _NOMAD_DEBUG
#define NOMAD_ASSERT(expr,...) \
	(static_cast<bool>(expr) ? void(0) : N_Error("%s:%s:%lu Assertion '%s' failed.", __FILE__,__func__,__LINE__,#expr))

#define LOG_PROFILE() LOG_INFO("profiling %s:%s:%u",__FILE__,__func__,__LINE__);profiler p(__func__)
#else
#define LOG_PROFILE() profiler p(__func__)

#define NOMAD_ASSERT(expr,...) \
	(static_cast<bool>(expr) ? void(0) : N_Error(__VA_ARGS__))
#endif

#ifndef LOGGER_OUTFILE
#define LOGGER_OUTFILE dbg_file
#endif
#ifndef LOGGER_PROFILE
#define LOGGER_PROFILE p_file
#endif

#define MARKER() puts("marker"); exit(1)

#ifdef _NOMAD_DEBUG
#define LOG_DEBUG(...)                         \
{                                              \
	fprintf(LOGGER_OUTFILE, "%s: ", __func__); \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);      \
	fprintf(LOGGER_OUTFILE, "\n");             \
}
#else
#define LOG_DEBUG(...)
#endif
#define DBG_LOG(...) LOG_DEBUG(__VA_ARGS__)

#define TAG_TO_STR(x) \
({char str[100]; \
	switch (x) { \
	case TAG_FREE: \
		strcpy(str, "TAG_FREE"); \
		break; \
	case TAG_STATIC: \
		strcpy(str, "TAG_STATIC"); \
		break; \
		break; \
	case TAG_PURGELEVEL: \
		strcpy(str, "TAG_PURGELEVEL"); \
		break; \
	case TAG_SCOPE: \
		strcpy(str, "TAG_SCOPE"); \
		break; \
	case TAG_CACHE: \
		strcpy(str, "TAG_CACHE"); \
		break; \
	}; \
str;})

#define LOG_FREETAGS(lowtag, hightag, nblocks, bfreed)   \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"Zone Daemon Log:\n"                                 \
	"\tlog type              => FREETAGS\n"              \
	"\tlowtag                => %s\n"                    \
	"\thightag               => %s\n"                    \
	"\tbytes freed           => %i\n"                    \
	"\tblocks freed          => %i\n",                   \
	TAG_TO_STR(lowtag),                                  \
	TAG_TO_STR(hightag), nblocks, bfreed);               \
}

#define LOG_HEAP()                                       \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"[Zone Daemon Log]\n"                                \
	"\tzone id               => %i\n"                    \
	"\tlog type              => HEAP_CHECK\n"            \
	"\ttotal bytes allocated => %lu\n"                   \
	"\ttotal bytes freed     => %lu\n"                   \
	"\tnumber of memblocks   => %lu\n",                  \
	GET_ZONE_ID(zone), log_size, free_size, numblocks);  \
}

#define LOG_ALLOC(ptr, tag, size)                        \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"[Zone Daemon Log]\n"                                \
	"\tlog type              => ALLOCATION\n"            \
	"\tbytes allocated       => %i\n"                    \
	"\tblock tag             => %i\n"                    \
	"\tuser pointer          => %p\n",                   \
	size, tag, ptr);                                     \
}
#define LOG_DEALLOC(ptr, tag, size)                      \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"[Zone Daemon Log]\n"                                \
	"\tlog type              => DEALLOCATION\n"          \
	"\tbytes freed           => %i\n"                    \
	"\tblock tag             => %i\n"                    \
	"\tuser pointer          => %p\n",                   \
	size, tag, ptr);                                     \
}

#define LOG_INFO(...)                                    \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"%s[INFO]%s(%s): ", C_GREEN, C_RESET, __func__);     \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);                \
	fprintf(LOGGER_OUTFILE, "\n");                       \
}
#define LOG_WARN(...)                                    \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"\x1b[31mWARNING:\x1b[0m\x1b[38;5;11m ");            \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);                \
	fprintf(LOGGER_OUTFILE, "%s\n", C_RESET);            \
}
#define LOG_ERROR(...)                                   \
{                                                        \
	fprintf(LOGGER_OUTFILE,                              \
	"%sERROR:%s%s\n"                                     \
	"\tfunction: %s\n"                                   \
	"\twhat: ",                                          \
		C_RED, C_RESET, "\x1b[38;5;11m", __func__);      \
	fprintf(LOGGER_OUTFILE, __VA_ARGS__);                \
	fprintf(LOGGER_OUTFILE, "%s\n", C_RESET);            \
	exit(EXIT_FAILURE);                                  \
}

#define LOG_TRACE_VAR(var) LOG_DEBUG(#var " = %s (%s:%d)", std::to_string(var).c_str(), __FILE__, __LINE__)


// pointer checks, they be givin' me them seggies
#define NULL_CHECK 0
#define PTR_CHECK(type, ptr)                                            \
{                                                                       \
	if (!ptr) {                                                         \
		LOG_WARN("pointer %s was NULL!", #ptr);                         \
	}                                                                   \
}

#define LOG_SAVEFILE()                                                  \
{                                                                       \
	fprintf(LOGGER_OUTFILE,                                             \
	"<=== Beginning Save/Load Game Procedures ===>\n"                   \
	"\tfile name:     %s\n"                                             \
	"\tfile size:     %lu\n"                                            \
	"\tlast accessed: %lu\n"                                            \
	"\tlast modified: %lu\n",                                           \
	svfile, svstat.st_size, svstat.st_atime, svstat.st_mtime);          \
}

struct profiler
{
	std::clock_t timer;
	const char* function;
	profiler(const char *__restrict func)
	{
		timer = std::clock();
		fprintf(LOGGER_PROFILE, "%s%s[PROFILER START]%s: %s\n", C_BG_MAGENTA, C_WHITE, C_RESET, func);
	}
	~profiler()
	{
		std::clock_t end = std::clock();
		float time = (end - timer)/(float)CLOCKS_PER_SEC;
		fprintf(LOGGER_PROFILE,
			"%s%s[PROFILER END]%s:\n"
			"\tfunction: %s\n"
			"\ttime: %f\n", C_BG_MAGENTA, C_WHITE, C_RESET, function, time);
	}
};

#define HALF           (.50)
#define QUARTER        (.25)
#define THREE_QUARTERS (.33)

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
	if ((FD_ISSET(STDIN_FILENO, &fds)))
		return getc(stdin);
	else
		return -1;
#elif defined(_WIN32)
	if (_kbhit())
		return _getch();
	else
		return -1;
#endif
}

#ifdef __unix__
//typedef __int128_t nomadllong_t;
typedef int64_t nomadlong_t;
typedef int32_t nomadint_t;
typedef int16_t nomadshort_t;
typedef int8_t nomadbyte_t;

typedef uint_fast8_t nomadenum_t;

//typedef __uint128_t nomadullong_t;
typedef uint64_t nomadulong_t;
typedef uint32_t nomaduint_t;
typedef uint16_t nomadushort_t;
typedef uint8_t nomadubyte_t;
#elif defined(_WIN32)
//typedef INT128 nomadllong_t;
typedef INT64 nomadlong_t;
typedef INT32 nomadint_t;
typedef INT16 nomadshort_t;
typedef INT8 nomadbyte_t;

typedef UINT8 nomadenum_t;

//typedef UINT128 nomadullong_t;
typedef UINT64 nomadulong_t;
typedef UINT32 nomaduint_t;
typedef UINT16 nomadushort_t;
typedef UINT8 nomadubyte_t;
#endif

typedef unsigned char byte;
typedef nomadulong_t nomadsize_t;
typedef nomadlong_t nomadssize_t;
typedef const char* nomad_cstr_t;
typedef std::string nomad_str_t;

#define nomadptr_t nomadlong_t huge*
#define nomaduptr_t nomadulong_t huge*

#ifdef __cplusplus
typedef bool nomadbool_t;
#else
typedef enum{false = 0, true = 1} nomadbool_t;
#endif

typedef nomadbyte_t sprite_t;

#define chtype chtype_small
typedef sprite_t chtype;

#ifdef NOMAD_DOUBLE
typedef double nomadfloat_t;
#else
typedef float nomadfloat_t;
#endif

constexpr uint8_t DIF_NOOB               = 0;
constexpr uint8_t DIF_RECRUIT            = 1;
constexpr uint8_t DIF_MERC               = 2;
constexpr uint8_t DIF_NOMAD              = 3;
constexpr uint8_t DIF_BLACKDEATH         = 4;
constexpr uint8_t DIF_MINORINCONVENIENCE = 5;
constexpr uint8_t NUMDIFS                = 6;

#define DIF_HARDEST DIF_MINORINCONVENIENCE

struct filestream
{
	FILE* fp;
	filestream(const std::string& filename, const char* mode)
		: fp(fopen(filename.c_str(), mode))
	{
		if (!fp) {
			N_Error("failed to open file with name %s", filename.c_str());
		}
	}
	filestream(const char* filename, const char* mode)
		: fp(fopen(filename, mode))
	{
		if (!fp) {
			N_Error("failed to open file with name %s", filename);
		}
	}
	~filestream() noexcept { fclose(fp); }
	inline FILE* stream(void) noexcept
	{ return fp; }
	inline char getc(void) const noexcept
	{ return ::getc(fp); }
};

typedef nomadlong_t point_t;
typedef struct coord_s
{
	point_t y, x;
	inline coord_s(const coord_s& c)
		: y(c.y), x(c.x)
	{
	}
	inline coord_s(point_t _y, point_t _x)
		: y(_y), x(_x)
	{
	}
	inline coord_s()
	{
	}
	coord_s(coord_s &&) = default;
	
	inline nomadbool_t operator==(const coord_s& c) const {
		return (y == c.y && x == c.x);
	}
	inline nomadbool_t operator!=(const coord_s& c) const {
		return (y != c.y && x != c.x);
	}
	inline nomadbool_t operator>=(const coord_s& c) const {
		return (y >= c.y && x >= c.x);
	}
	inline nomadbool_t operator<=(const coord_s& c) const {
		return (y <= c.y && x <= c.x);
	}
	inline nomadbool_t operator>(const coord_s& c) const {
		return (y > c.y && x > c.x);
	}
	inline nomadbool_t operator<(const coord_s& c) const {
		return (y < c.y && x < c.x);
	}
	inline nomadbool_t operator==(point_t& p) const {
		return (y == p && x == p);
	}
	inline nomadbool_t operator!=(point_t& p) const {
		return (y != p && x != p);
	}
	inline nomadbool_t operator>=(point_t& p) const {
		return (y >= p && x >= p);
	}
	inline nomadbool_t operator<=(point_t& p) const {
		return (y <= p && x <= p);
	}
	inline nomadbool_t operator>(point_t& p) const {
		return (y > p && x > p);
	}
	inline nomadbool_t operator<(point_t& p) const {
		return (y < p && x < p);
	}
	inline coord_s& operator=(const coord_s& c) {
		y = c.y;
		x = c.x;
		return *this;
	}
	inline coord_s& operator=(point_t& p) {
		y = p;
		x = p;
		return *this;
	}
	inline coord_s& operator++(void) {
		++y;
		++x;
		return *this;
	}
	inline coord_s& operator--(void) {
		--y;
		--x;
		return *this;
	}
	inline coord_s& operator+=(const coord_s& c) {
		y += c.y;
		x += c.x;
		return *this;
	}
	inline coord_s& operator+=(point_t p) {
		y += p;
		x += p;
		return *this;
	}
	inline coord_s& operator-=(const coord_s& c) {
		y -= c.y;
		x -= c.x;
		return *this;
	}
	inline point_t& operator[](nomadenum_t i) {
		switch (i) {
		case 0: return y;
		case 1: return x;
		};
		LOG_WARN("Invalid call to point_t& coord_s::operator[], returning y, i = %hu", i);
		return y;
	}
} coord_t;

extern nomadbool_t ncurses_on;
extern nomadbool_t gui_on;

typedef struct area_s
{
	coord_t tl;
	coord_t tr;
	coord_t bl;
	coord_t br;
	area_s() : tl({0,0}), tr({0,0}), bl({0,0}), br({0,0}) {}
	area_s(coord_t _tl, coord_t _tr, coord_t _bl, coord_t _br)
		: tl(_tl), tr(_tr), bl(_bl), br(_br)
	{
	}
	area_s(const area_s &) = delete;
	area_s(area_s &&) = default;
	
	inline nomadbool_t operator==(const area_s& a) const {
		return (tl == a.tl && tr == a.tl && bl == a.bl && br == a.br);
	}
	inline nomadbool_t operator>=(const area_s& a) const {
		return (tl >= a.tl && tr >= a.tl && bl >= a.bl && br >= a.br);
	}
	inline nomadbool_t operator<=(const area_s& a) const {
		return (tl <= a.tl && tr <= a.tl && bl <= a.bl && br <= a.br);
	}
	inline nomadbool_t operator!=(const area_s& a) const {
		return (tl != a.tl && tr != a.tl && bl != a.bl && br != a.br);
	}
	inline nomadbool_t operator>(const area_s& a) const {
		return (tl > a.tl && tr > a.tl && bl > a.bl && br > a.br);
	}
	inline nomadbool_t operator<(const area_s& a) const {
		return (tl < a.tl && tr < a.tl && bl < a.bl && br < a.br);
	}
	inline area_s& operator++(void) {
		++tl;
		++tr;
		++bl;
		++br;
		return *this;
	}
	inline area_s& operator--(void) {
		--tl;
		--tr;
		--bl;
		--br;
		return *this;
	}
	inline area_s& operator+=(const area_s& a) {
		tl += a.tl;
		tr += a.tr;
		bl += a.bl;
		br += a.br;
		return *this;
	}
	inline area_s& operator-=(const area_s& a) {
		tl -= a.tl;
		tr -= a.tr;
		bl -= a.bl;
		br -= a.br;
		return *this;
	}
	inline coord_t& operator[](nomadenum_t i) {
		switch (i) {
		case 0: return tl;
		case 1: return tr;
		case 2: return bl;
		case 3: return br;
		};
		LOG_WARN("Invalid call to coord_t& area_s::operator[], returning tl, i = %hu", i);
		return tl;
	}
} area_t;

#ifdef __unix__
#define ONE_SECOND      (1000*1000)
#define ONE_MILLISECOND (1000)
#define sleepfor(x)     usleep((x))
#elif defined(_WIN32)
#define ONE_SECOND      (1000)
#define ONE_MILLISECOND (1)
#define sleepfor(x)     Sleep(x)
#endif
#define SECONDS(x)       ((x)*ONE_SECOND)
#define MILLISECONDS(x)  ((x)*ONE_MILLISECOND)

#define CHECK_STD_VALID(arg) if (!arg.valid()) N_Error("%s: memory failure with variable %s", __func__, #arg)

typedef struct collider_s
{
	coord_t where;
	nomaduint_t what;
	void *ptr = (void *)NULL;
	collider_s(coord_t& _where, nomaduint_t _what, void *_ptr)
		: where(_where), what(_what), ptr(_ptr)
	{
	}
	collider_s()
	{
	}
} collider_t;

enum dir : nomadenum_t
{
	D_NORTH,
	D_WEST,
	D_SOUTH,
	D_EAST,
	NUMDIRS,
	D_NULL
};

typedef union floatint_u
{
	nomadint_t i;
	nomaduint_t u;
	float f;
	nomadenum_t b[4];
} floatint_t;

#ifdef NOMAD_FAST_MATH
#define float_to_fixed(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))
#define fixed_to_float(x) \
({ \
  	if (!i) \
  	  	return 0; \
  	unsigned s = i>>31; \
  	unsigned E = (int) (log(i<0 ? -i : i)/log(2)); \
  	unsigned exp = E + 127; \
  	unsigned M= i>0? i : -i; \
  	unsigned frac = M ^ (1<<E); \
  	if (E>=23) \
    	frac >>= E-23; \
	else
		frac <<= 23-E; \
	nomadfloat_t a = s<<31 | exp<<23 | frac; \
a;})
#else
#define float_to_fixed(x) ((nomadfixed_t)x)
#define fixed_to_float(x) ((float)x)
#endif


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
	return (const char *)NULL;
}
static nomadenum_t StrToDir(const char* str)
{
	if (strstr(str, "D_NORTH")) return D_NORTH;
	else if (strstr(str, "D_WEST")) return D_WEST;
	else if (strstr(str, "D_SOUTH")) return D_SOUTH;
	else if (strstr(str, "D_EAST")) return D_EAST;
	else return D_NULL;
}

typedef struct dim_s
{
	nomadenum_t height, width;
	dim_s(nomadenum_t _height, nomadenum_t _width)
		: height(_height), width(_width)
	{
	}
	dim_s()
	{
	}
} dim_t;

typedef int32_t nomadfixed_t;

typedef union 
{
	nomadshort_t data[2];
	nomadbyte_t bytes[4];
} nomadfixed_u;


class Menu
{
public:
	nomaduint_t numitems;
	MENU* menu;
	ITEM** item_ls;
public:
	Menu(const std::vector<std::string>& choices, const char* marker);
	Menu(const char** choices, const char* marker);
	~Menu();
	void UpItem() { menu_driver(menu, REQ_UP_ITEM); }
	void DownItem() { menu_driver(menu, REQ_DOWN_ITEM); }
};

inline auto dirtostr(nomadenum_t dir) -> const char* { return DirToStr(dir); };
inline auto strtodir(const char* str) -> nomadenum_t { return StrToDir(str); };
inline auto difftostr(uint_fast8_t diff) -> const char* {
	switch (diff) {
	case DIF_NOOB: return "I AM A NOOB";
	case DIF_RECRUIT: return "I CAN do stuff";
	case DIF_MERC: return "bring. It .ON!";
	case DIF_NOMAD: return "Can't Touch This";
	case DIF_BLACKDEATH: return "John Wick Junior";
	case DIF_MINORINCONVENIENCE: return "I AM DEATH ITSELF";
	};
	return (const char *)NULL;
};
inline auto strtodiff(const char* str) -> nomadenum_t {
	if (strstr(str, "DIF_NOOB")) return DIF_NOOB;
	else if (strstr(str, "DIF_RECRUIT")) return DIF_RECRUIT;
	else if (strstr(str, "DIF_MERC")) return DIF_MERC;
	else if (strstr(str, "DIF_NOMAD")) return DIF_NOMAD;
	else if (strstr(str, "DIF_BLACKDEATH")) return DIF_BLACKDEATH;
	else if (strstr(str, "DIF_MINORINCONVENIENCE")) return DIF_MINORINCONVENIENCE;
	return NUMDIFS;
};

inline auto strtobool(const char* str) -> nomadbool_t { return N_strcmp(str, "true") ? true : false; };
inline auto strtobool(const std::string& str) -> nomadbool_t { return str == "true" ? true : false; };
inline auto booltostr(bool b) -> const char* { return b ? "true" : "false"; };
inline auto oppositedir(nomadenum_t dir) -> nomadenum_t
{
	switch(dir) {
	case D_NORTH: return D_SOUTH;
	case D_WEST: return D_EAST;
	case D_SOUTH: return D_NORTH;
	case D_EAST: return D_WEST;
	};
	return D_NORTH;
};
inline auto is_oppositedir(nomadenum_t dir, nomadenum_t cmp) -> nomadbool_t
{
	if ((dir == D_NORTH && cmp == D_SOUTH) || (dir == D_SOUTH && dir == D_NORTH)) return true;
	else if ((dir == D_WEST && cmp == D_EAST) || (dir == D_EAST && cmp == D_WEST)) return true;
	return false;
};

using namespace std::literals::chrono_literals;

nomadbool_t G_CheckCollider(coord_t& point, Game* const game, collider_t& c);
nomadbool_t inArea(const area_t& a, const coord_t& pos);
inline nomadfloat_t Q_root(nomadfloat_t x);
coord_t closestOBJ(const std::vector<coord_t>& coords, const coord_t& src);
nomadlong_t disBetweenOBJ(const coord_t& src, const coord_t& tar);
collider_t G_CastRay(coord_t& endpoint, coord_t& startpoint, Game* const game);

#endif
