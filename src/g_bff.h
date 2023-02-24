#ifndef _G_BFF_
#define _G_BFF_

#pragma once

#ifndef NotFound
#define NotFound std::string::npos
#endif

#define ASSERT(x, ...) \
{ \
	if (!(x)) { \
		fprintf(stderr, "%s(): ", __func__); \
		fprintf(stderr, __VA_ARGS__); \
		exit(-1); \
	} \
}

#if defined(__unix__)
#define BFF_API extern "C" __attribute__((visibilty("default")))
#elif defined(_WIN32)
#ifdef BFF_IMPLEMENTATION
#define BFF_API extern "C" __declspec(dllimport)
#else
#define BFF_API extern "C" __declspec(dllexport)
#endif
#endif

#ifndef NOMAD_MAIN
typedef uint_fast8_t byte;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BFF_COMPILE       1
#define BFF_DECOMPILE     2
#define BFF_VERSION_MAJOR 0
#define BFF_VERSION_MINOR 2

#ifndef NOMAD_MAIN
typedef struct coord_s
{
	int16_t y;
	int16_t x;
} coord_t;
#endif

typedef struct _entity_spawner
{
	uint32_t entity_id;
	coord_t spawn_loc;
} entity_spawner_t;

#define LOG(...)                         \
{                                        \
	fprintf(stdout, "%s(): ", __func__); \
	fprintf(stdout, __VA_ARGS__);        \
	fputc('\n', stdout);                 \
}

typedef struct token_s
{
	char ftok;
	char btok;
	size_t line;
	std::string buffer;
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
	ELEMENT_DECLARE,
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
	'@',
	'1',
	'0'
};

typedef struct _bff_runtime
{
	uint64_t i;
	std::vector<token_t> tokbuf;
	std::vector<std::string> strbuf;
} bff_runtime_t;

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
    char** lvl_map;
} bff_lvl_t;

typedef struct _bff_map
{
    std::string mapname;
	std::string mapid;
    char mapbuffer[9][120][120];
    std::vector<std::string> levels;
} bff_map_t;

typedef struct _bff_file
{
    bff_runtime_t* run;
    std::string bffname;
    bff_map_t* bffmap = NULL;

    bff_lvl_t** levels;
} bff_file_t;

BFF_API void BFF_Parse(bff_file_t* file, const std::string& filename);
BFF_API void BFF_Free(bff_file_t* file);
BFF_API void BFF_Compile(bff_file_t* file);

void G_AllocLevelList(const std::vector<bff_lvl_t*>& bfflvls);
void G_FreeLevel(bff_lvl_t* lvl);
bff_lvl_t* G_LoadLevel(const std::string& name, bff_lvl_t* old);

#endif