#ifndef _G_BFF_
#define _G_BFF_

#pragma once

#define BFF_VERSION_MAJOR 0
#define BFF_VERSION_MINOR 2
#define BFF_VERSION_STR \
	"BFFProject version "#BFF_VERSION_MAJOR"."#BFF_VERSION_MINOR""

#define BFF_COMPILE       1
#define BFF_DECOMPILE     2
#define BFF_STR_SIZE      256
#define BFF_INIT_STR(str) memset(&str,'\0',BFF_STR_SIZE)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <functional>

#if defined(__GNUG__)
#   define BFF_API extern "C"
#   define BFF_WARN_RETN __attribute__((warn_unused_result))
#   define BFF_NORETN __attribute__((noreturn))
#   define BFF_DEPRECATED __attribute__((deprecated))
#elif defined(_MSVC_VER)
#   ifdef BFF_IMPLEMENTATION
#       define BFF_API extern "C" __declspec(dllimport)
#   else
#       define BFF_API extern "C" __declspec(dllexport)
#   endif
#   define BFF_WARN_RETN _Check_return
#   define BFF_NORETN __declspec(noreturn)
#   define BFF_DEPRECATED __declspec(deprecated("*** this function is deprecated, older version? ***"))
#else
#   define BFF_API extern "C"
#   define BFF_WARN_RETN
#   define BFF_DEPRECATED
#   define BFF_NORETN
#   define BFF_DEPRECATED
#endif

#ifndef NOMAD_MAIN
typedef uint_fast8_t byte;
#define NotFound std::string::npos
typedef struct coord_s
{
	int16_t y;
	int16_t x;
} coord_t;
#endif

#define ASSERT(x, ...) \
{ \
	if (!(x)) { \
		fprintf(stderr, "%s(): ", __func__); \
		fprintf(stderr, __VA_ARGS__); \
		exit(-1); \
	} \
}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _entity_spawner
{
	std::string entity_id;
	coord_t pos;
} entity_spawner_t;

#ifndef BFF_LOGFILE
#define BFF_LOGFILE "bffinfo.log"
#endif
#ifndef BFF_LOGSTREAM
#define BFF_LOGSTREAM bfflog
extern FILE* BFF_LOGSTREAM;
#endif

typedef struct token_s
{
	char ftok;
	char btok;
	size_t line;
	std::string buffer;
	token_s()
	{
	}
} token_t;

enum
{
	CHUNK_DECLARE,
	PROP_DECLARE,
	LIST_OPEN,
	LIST_CLOSE,
	VAR_DECLARE,
	SPECIAL_VAR_DECLARE,
	DEFAULT_VALUE,
	CODE_OPEN,
	CODE_CLOSE,
	SUBCHUNK_OPEN,
	SUBCHUNK_CLOSE,
	NEWLINE,
	NULL_TOK,
	DATA_OPEN,
	DATA_CLOSE,
	
	NUMTOKENS
};

#ifdef __cplusplus
}
#endif

static constexpr char tokens[NUMTOKENS] = {
	'#',
	'%',
	'[',
	']',
	'$',
	'&',
	':',
	'{',
	'}',
	'(',
	')',
	'\n',
	'\0',
	'1',
	'0'
};

typedef struct _bff_runtime
{
	uint64_t i;
	std::vector<token_t> tokbuf;
	std::vector<std::string> strbuf;
} bff_runtime_t, bff_runtime;

typedef struct _bff_file bff_file_t;
typedef struct _bff_map bff_map_t;

typedef struct _bff_lvl
{
    bff_file_t* file = NULL;
    bff_map_t* bffmap = NULL;
    std::string lvl_name;
    std::string lvl_id;
    uint8_t map_height = 0;
    uint8_t map_width = 0;
    std::vector<std::vector<char>> lvl_map;
    std::vector<std::string> itemspawners;
    std::vector<std::string> mobspawners;
    std::vector<std::string> wpnspawners;
} bff_lvl_t;

typedef struct _bff_map
{
	std::string mapname;
	std::string mapid;
    char mapbuffer[9][120][120];
	int sectorcounter = 0;
    std::vector<std::string> levels;
} bff_map_t;

typedef struct _bff_mob
{
	std::string mobname;
	int health;
	int armor;
	entity_spawner_t spawner;
	std::string mtemplate;
} bff_mob_t;

typedef struct _bff_bot
{
	std::string botname;
	int health;
	int armor;
	entity_spawner_t spawner;
	std::string btemplate;
} bff_bot_t;

typedef struct _bff_item
{
	std::string itemname;
	entity_spawner_t spawner;
	std::string itemplate;
} bff_item_t;

typedef struct _bff_wpn
{
	std::string wpnname;
	entity_spawner_t spawner;
	std::string wtemplate;
} bff_wpn_t;

typedef struct _bff_data
{
	std::vector<std::string> mobspawners;
	std::vector<std::string> itemspawners;
	std::vector<std::string> wpnspawners;
	std::vector<std::string> botspawners;
	std::vector<bff_mob_t> mobs;
	std::vector<bff_item_t> items;
	std::vector<bff_wpn_t> wpns;
	std::vector<bff_bot_t> bots;
} bff_data_t;

typedef struct _bff_file
{
	bff_runtime_t run;
	bff_data_t bffdata;
	std::string bffname;
	std::vector<bff_map_t> maps;
	
	std::vector<bff_lvl_t> levels;
} bff_file_t;


BFF_API void BFF_Parse(bff_file_t& file, const std::string& filename);
BFF_API void BFF_Compile(bff_file_t& file, const std::string& outfile);
BFF_API void BFF_Decompile(bff_file_t& file, const std::string& infile);

#define parse_spawner_coords(pos) \
{ \
	if (r.tokbuf[i].btok != tokens[DATA_OPEN]) { \
		BFF_ThrowError(i, r.tokbuf[i].buffer.c_str(), \
			"expected '1' (data initializer) at end of the line"); \
	} \
	for (;; ++i) { \
		const std::string& buf = r.tokbuf[i].buffer; \
		if (buf.front() == tokens[SPECIAL_VAR_DECLARE] \
		&& buf.back() == tokens[DATA_CLOSE]) { break; } \
		if (buf.front() == tokens[DEFAULT_VALUE]) { \
			if (buf.find('Y') != NotFound) { \
				int c = stoi(r.tokbuf[i+1].buffer); \
				if (c > 360 || c < 1) { \
					BFF_ThrowError(i, r.tokbuf[i+1].buffer.c_str(), \
						"coordinate integer must be between 1-360"); \
				} \
				pos.y = c - 1; \
			} \
			else if (buf.find('X') != NotFound) { \
				int c = stoi(r.tokbuf[i+1].buffer); \
				if (c > 360 || c < 1) { \
					BFF_ThrowError(i, r.tokbuf[i+1].buffer.c_str(), \
						"coordinate integer must be between 1-360"); \
				} \
				pos.x = c - 1; \
			} \
		} \
	} \
}

void G_AllocLevelList(std::vector<bff_lvl_t>& bfflvls, bff_lvl_t** lvl_list);
void G_FreeLevel(bff_lvl_t* lvl);
bff_lvl_t* G_LoadLevel(const std::string& name, bff_lvl_t* old);

#endif