#ifndef _G_LOADBFF_
#define _G_LOADBFF_

#define ASSERT(x, ...) \
{ \
	if (!(x)) { \
		fprintf(stderr, "%s(): ", __func__); \
		fprintf(stderr, __VA_ARGS__); \
		exit(-1); \
	} \
}

#ifdef __unix__
#define BFF_API extern "C"
#elif defined(_WIN32)
#ifdef BFF_IMPLEMENTATION
#define BFF_API extern "C" __declspec(dllimport) __stdcall
#else
#define BFF_API extern "C" __declspec(dllexport) __stdcall
#endif
#endif

#define BFF_INTERNAL extern "C"

typedef uint_fast8_t bffnum_t;

#ifdef __cplusplus
extern "C" {
#endif

#define BFF_COMPILE       1
#define BFF_DECOMPILE     2
#define BFF_VERSION_MAJOR 0
#define BFF_VERSION_MINOR 1

typedef struct _entity_spawner
{
	uint32_t entity_id;
	coord_t spawn_loc;
} entity_spawner_t;

#define bffenum unsigned char

#ifdef __unix__
typedef __int128_t magicnum_t;
#elif defined(_WIN32)
typedef LONGLONG magicnum_t;
#endif
typedef char buffer[1024];

#define NotFound std::string::npos
#define BFFVAR static constexpr

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

#define ERR_INVALID_CONTEXT 0xe1

typedef struct bffmap_s
{
	std::string mapname;
	char sectormap[9][120][120];
	int8_t sectorcounter;
} bffmap_t;


typedef struct bffitem_s
{
	uint32_t itemtemplate;
	entity_spawner_t* spawner;
} bffitem_t;

typedef struct bffmob_s
{
	uint32_t mobtemplate;
	entity_spawner_t* spawner;
	
	const char* mobname;
	uint32_t health;
	uint16_t armor;
	bool hasmelee;
	bool hashitscan;
	bool hasprojectile;
	uint32_t meleedmg, meleerng;
	uint32_t hitscandmg, hitscanrange, hitscanrng;
	uint32_t projectiledmg, projectilerange, projectilerng;
} bffmob_t;

typedef struct bffwpn_s
{
	uint32_t wpntemplate;
	entity_spawner_t* spawner;
} bffwpn_t;

typedef struct bffnpc_s
{
	uint32_t npctemplate;
	entity_spawner_t* spawner;
	
	std::string npcname;
	uint32_t health;
	uint16_t armor;
} bffnpc_t;

typedef struct _bffdata
{
	uint32_t dtype;
	std::vector<bffnpc_t> npc_data;
	std::vector<bffmob_t> mob_data;
	std::vector<bffitem_t> item_data;
	std::vector<bffwpn_t> wpn_data;
} bffdata_t;


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

typedef struct _bff_file
{
	std::ifstream* file;
	bff_runtime_t* run;
	std::string filename;
	std::string output;
	const char* bffname;
	uint32_t numbiomes;
	uint32_t numwpns;
	uint32_t numitems;
	uint32_t nummobs;
	uint32_t numnpcs;
	uint32_t bfftype;
	uint8_t context;
	bffmap_t mchunk;
	bffdata_t* c_data;
	std::vector<bffdata_t> dchunks;
	std::vector<entity_spawner_t> spawners;
} bff_file_t;

BFF_API bff_file_t* BFF_InitFile(const char* filename);
BFF_API bff_file_t* BFF_Init(int argc, char** argv);
BFF_API void BFF_Free(bff_file_t* file);
BFF_API std::vector<bffdata_t>& BFF_GetDataChunks(bff_file_t* file);
BFF_API bffmap_t* BFF_GetMapChunk(bff_file_t* file);
BFF_API std::vector<bffwpn_t>& BFF_GetWpnElements(bff_file_t* file);
BFF_API std::vector<bffitem_t>& BFF_GetItemElements(bff_file_t* file);
BFF_API std::vector<bffmob_t>& BFF_GetMobElements(bff_file_t* file);
BFF_API void BFF_InitMapChunk(bffmap_t* mchunk);
BFF_API unsigned char BFF_GetFileType(bff_file_t* file);
BFF_API void BFF_Parse(bff_file_t* file);
BFF_API void BFF_Compile(bff_file_t* file, const char* outfile);
BFF_API void BFF_Decompile(bff_file_t* file, const char* infile);
BFF_API void BFF_ParseMapElement(bff_file_t* file);
BFF_API void BFF_ParseItemElement(bff_file_t* file);
BFF_API void BFF_ParseWpnElement(bff_file_t* file);
BFF_API void BFF_ParseMobElement(bff_file_t* file);
BFF_API void BFF_ParseNPCElement(bff_file_t* file);
BFF_API void BFF_ParseElement(bff_file_t* file);
BFF_API void BFF_ParseProperties(bff_file_t* file);
BFF_API void BFF_ParseChunkDec(bff_file_t* file);
BFF_API void BFF_ParseSubChunk(bff_file_t* file);
BFF_API void BFF_ParseMapSector(bff_file_t* file);
BFF_API void BFF_ParseDataSubChunk(bff_file_t* file);
BFF_API void BFF_GetNumU32(uint32_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNumU16(uint16_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNumU8(uint8_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNum32(int32_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNum16(int16_t* var, const std::string& str, size_t i);
BFF_API void BFF_GetNum8(int8_t* var, const std::string& str, size_t i);

BFF_INTERNAL void BFF_SystemErr(size_t err);
BFF_INTERNAL void BFF_ThrowError(size_t line, const char* buffer, const char* err, ...);

#endif