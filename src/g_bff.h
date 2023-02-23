#ifndef __BFFLANG__
#define __BFFLANG__

#define ASSERT(x, ...) \
{ \
	if (!(x)) { \
		fprintf(stderr, "%s(): ", __func__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
		fflush(stderr); \
		exit(-1); \
	} \
}

#define BFF_API extern "C"

#ifndef BFF_LOG_PATH
#define BFF_LOG_PATH "bffinfo.log"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <list>

#define BFF_LOGFILE logfile

#define BFF_INFO(...) \
{ \
	fprintf(BFF_LOGFILE, "[INFO](%s): ", __func__); \
	fprintf(BFF_LOGFILE, __VA_ARGS__); \
	fprintf(BFF_LOGFILE, "\n"); \
}
#define BFF_WARN(...) \
{ \
	fprintf(BFF_LOGFILE, "WARNING(%s): ", __func__); \
	fprintf(BFF_LOGFILE, __VA_ARGS__); \
	fprintf(BFF_LOGFILE, "\n"); \
}

#ifndef NotFound
#define NotFound std::string::npos
#endif
#define BFFVAR static constexpr

typedef int64_t magicnum_t;

BFFVAR magicnum_t BFF_FILE_HEADER = 0xa49d21f;

BFFVAR uint8_t CONTEXT_NO_CHUNK   = 0b00000000;
BFFVAR uint8_t CONTEXT_MAP_CHUNK  = 0b00000001;
BFFVAR uint8_t CONTEXT_DATA_CHUNK = 0b00000010;

BFFVAR uint32_t BFF_UNDEFINED      = 0x0000;
BFFVAR uint32_t BFF_TYPE_CAMPAIGN  = 0x0001;
BFFVAR uint32_t BFF_TYPE_ROGUELIKE = 0x0002;
BFFVAR uint32_t BFF_TYPE_DOOMCLONE = 0x0003;
BFFVAR uint32_t BFF_DATA_ITEM      = 0x0004;
BFFVAR uint32_t BFF_DATA_WPN       = 0x0005;
BFFVAR uint32_t BFF_DATA_MOB       = 0x0006;
BFFVAR uint32_t BFF_DATA_NPC       = 0x0007;
BFFVAR uint32_t BFF_MAX_ALLOWED    = 0x0008;

#define BFF_COMPILE       1
#define BFF_DECOMPILE     2

#ifndef NOMAD_MAIN
typedef struct coord_s
{
	int16_t y;
	int16_t x;
} coord_t;
#endif

typedef struct _entity_spawner
{
	std::string spawner_type;
	std::string entity_id;
	coord_t spawn_loc;
} entity_spawner_t;

enum
{
	CHUNK_DECLARE,
	PROP_DECLARE,
	LIST_OPEN,
	LIST_CLOSE,
	VAR_DECLARE,
	SPECIAL_VAR_DECLARE,
	DEFAULT_VALUE,
	SUBCHUNK_OPEN,
	SUBCHUNK_CLOSE,
	NEWLINE,
	NULL_TOK,
	ELEMENT_DECLARE,
	DATA_OPEN,
	DATA_CLOSE,
	DUMBCHUNK_OPEN,
	DUMBCHUNK_CLOSE,
	
	
	NUMTOKENS
};

static constexpr char tokens[NUMTOKENS] = {
	'#',
	'%',
	'{',
	'}',
	'$',
	'&',
	':',
	'(',
	')',
	'\n',
	'\0',
	'@',
	'1',
	'0',
	'[',
	']'
};

typedef struct _bff_map bff_map_t;
typedef struct _bff_file bff_file_t;

typedef struct _blf_file
{
	bff_file_t* file;
	bff_map_t* bffmap = (bff_map_t *)NULL;
	std::string lvl_name;
	std::string lvl_id;
	int map_height = 0;
	int map_width = 0;
	char** lvl_map;
	std::vector<int> mobspawners;
	std::vector<int> itemspawners;
	std::vector<int> wpnspawners;
} blf_file_t;

extern blf_file_t* c_blf;

typedef struct _bff_map
{
	std::string mapname;
	std::string mapid;
	char sectormap[9][120][120];
	int8_t sectorcounter = -1;
	int numlvls = 0;
	std::vector<std::string> levels;
} bff_map_t;

typedef struct _bff_data
{
	std::list<entity_spawner_t> spawners;
} bff_data_t;

typedef struct _bff_file
{
	std::string bffname;
	bff_map_t* bffmap;
	std::vector<blf_file_t*> levels;
	
	bff_data_t bffdata;
	
	int nummaps = 0;
} bff_file_t;

BFF_API void BFF_Free(bff_file_t* file);
BFF_API bff_file_t* BFF_Parse(const std::string& filename);

#endif