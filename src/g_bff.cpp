#include "g_game.h"
//#define BFF_IMPLEMENTATION
static FILE* logfile;
#include "g_bff.h"

//static void *bffhandle;

#define LOAD(user, func)                                               \
{                                                                      \
	*(void **)(&user) = dlsym(bffhandle, func);                        \
	if (!user) {                                                       \
		N_Error("Failed to link function %s to %s! system error: %s",  \
			#user                                                      \
			func, dlerror());                                          \
	}                                                                  \
}


//#define NOMAD_RTLD RTLD_NOW | RTLD_LOCAL

//fixme - segfaults cuz bffmap gets all f'ed up (sector 0 is completely NULL)
void G_LoadBFF(const char* bffname, Game* const game)
{
	printf("G_LoadBFF: loading bff file into memory...\n");
	bff_file_t* file = BFF_Parse(bffname);
	bff_map_t& bffmap = *file->bffmap;
	FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	/*
	076
	185
	234
	*/
	NOMAD_ASSERT(fp, "Could not create RUNTIME/mapfile.txt!");
	PTR_CHECK(NULL_CHECK, fp);
	LOG_INFO("Successfully created RUNTIME/mapfile.txt");
	nomadshort_t y{}, x{};
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[6][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[5][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap.sectormap[4][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
    for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
    fclose(fp);
	LOG_INFO("closing write-only mapfile.txt");
	BFF_Free(file);
	while (getc(stdin) != 'q');
	game->I_InitHUD();
}

static int context;
static size_t i;
static bff_data_t* c_data;
static bff_map_t* c_map;
static std::vector<std::string> strbuf;

#ifdef NOMAD_MAIN
#define Malloc(size,ptr) Z_Malloc(size,TAG_STATIC,ptr)
#define Realloc(ptr,nsize) Z_Realloc(ptr,nsize,TAG_STATIC)
#define Free(ptr) Z_Free(ptr)
#define Calloc(ptr,nelem,elemsize) Z_Calloc(ptr,nelem,elemsize)
#else
#define Malloc(size,ptr) malloc(size)
#define Realloc(ptr,nsize) realloc(ptr,nsize)
#define Free(ptr) free(ptr)
#define Calloc(ptr,nelem,elemsize) calloc(nelem,elemsize)
#endif

#define ftok(str) str.front()
#define btok(str) str.back()

void BFF_ParseChunkDec(bff_file_t* file);
void BFF_ParseSubChunk(bff_file_t* file);
void BFF_ParseMapSector(bff_file_t* file);
void BFF_ParseProperties(bff_file_t* file);
void BFF_ParseElement(bff_file_t* file);
void BFF_ParseList(bff_file_t* file, std::vector<std::string>& bfflist);
void BFF_ParseSpawner(bff_file_t* file, std::list<entity_spawner_t>& spawners);
blf_file_t* BLF_Parse(bff_file_t* file, const std::vector<std::string>& strbuf, size_t& i);

#define BFF_SystemErr(...) fprintf(stderr, "Internal Error: ");fprintf(stderr,__VA_ARGS__);fprintf(stderr, "\n");exit(-1)
static bff_file_t* c_file;

void BFF_ThrowError(size_t line, const char* buffer, const char* err, ...)
{
	fprintf(stderr, "(ERROR)\nAt Line: %lu\nContents: %s\nMessage: ", line+1, buffer);
	va_list argptr;
	va_start(argptr, err);
	vfprintf(stderr, err, argptr);
	va_end(argptr);
	fprintf(stderr, "\n");
	fflush(stderr);
	if (c_file->bffmap) {
		Free(c_file->bffmap);
	}
	BFF_Free(c_file);
	exit(-1);
}

template<typename T>
static void BFF_GetNum(T& var, const std::string& buffer)
{
	var = (T)stoi(buffer);
}

BFF_API bff_file_t* BFF_Parse(const std::string& filename)
{
	BFF_LOGFILE = fopen(BFF_LOG_PATH, "w");
	bff_file_t* file = (bff_file_t *)Malloc(sizeof(bff_file_t), &file);
	if (!file) {
		fprintf(stderr, "Error: malloc failed!\n");
		exit(-1);
	}
	c_file = file;
	context = CONTEXT_NO_CHUNK;
	std::ifstream in(filename, std::ios::in);
	ASSERT(in.is_open(), "Error: failed to open bffl file!");
	std::string line;
	while (std::getline(in, line)) { strbuf.push_back(line); }
	in.close();
	
	for (i = 0; i < strbuf.size(); ++i) {
		if (i == (strbuf.size() - 1)) { break; }
		switch (ftok(strbuf[i])) {
		case tokens[NULL_TOK]:
		case tokens[NEWLINE]:
			break;
		case tokens[PROP_DECLARE]:
			BFF_ParseProperties(file);
			break;
		case tokens[SUBCHUNK_OPEN]:
			BFF_ParseSubChunk(file);
			break;
		case tokens[ELEMENT_DECLARE]:
			BFF_ParseElement(file);
			break;
		case tokens[CHUNK_DECLARE]:
			BFF_ParseChunkDec(file);
			break;
		};
	}
	fclose(BFF_LOGFILE);
	return file;
}
BFF_API void BFF_Free(bff_file_t* file)
{
	if (!file)
		return;
	
	for (auto* i : file->levels) {
		for (int y = 0; y < i->map_height; ++y) {
			Free(i->lvl_map[y]);
		}
		Free(i->lvl_map);
		Free(i);
	}
	Free(file);
}

void BFF_ParseList(bff_file_t* file, std::vector<std::string>& bfflist)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	for (;; ++i) {
		if (ftok(strbuf[i]) == tokens[ELEMENT_DECLARE]
		&& btok(strbuf[i]) == tokens[LIST_CLOSE]) { break; }
		
		bfflist.push_back(strbuf[i]);
	}
}

void BFF_ParseSpawner(bff_file_t* file, std::list<entity_spawner_t>& spawners)
{
	entity_spawner_t spawn;
	if (strbuf[i].find("MOB") != NotFound) {
		spawn.spawner_type = "MOB";
	} else if (strbuf[i].find("BOT") != NotFound) {
		spawn.spawner_type = "BOT";
	}
	for (;; ++i) {
		if (ftok(strbuf[i]) == tokens[ELEMENT_DECLARE]
		&& btok(strbuf[i]) == tokens[DATA_CLOSE]) { break; }
		
		const std::string& str = strbuf[i];
		if (ftok(str) == tokens[VAR_DECLARE]) {
			if (str.find("COORDS") != NotFound
			|| str.find("COORDINATES") != NotFound) {
				for (;; ++i) {
					if (ftok(strbuf[i]) == tokens[VAR_DECLARE]
					&& btok(strbuf[i]) == tokens[DATA_CLOSE]) { break; }
					
					if (ftok(strbuf[i]) == tokens[DEFAULT_VALUE]) {
						if (strbuf[i].find('Y') != NotFound) {
							spawn.spawn_loc.y = (uint16_t)stoi(strbuf[i+1]);
						}
						else if (strbuf[i].find('X') != NotFound) {
							spawn.spawn_loc.x = (uint16_t)stoi(strbuf[i+1]);
						}
						else {
							BFF_ThrowError(i, strbuf[i].c_str(),
							"invalid coordinates given with entity spawner!");
						}
					}
				}
			}
			else if (str.find("ENTITY") != NotFound) {
				spawn.entity_id = strbuf[i+1];
			}
		}
	}
	spawners.push_back(spawn);
}

void BFF_ParseElement(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	if (strbuf[i].find("MAP_LVLS") != NotFound
	|| strbuf[i].find("MAP_LEVELS") != NotFound) {
		if (context != CONTEXT_MAP_CHUNK) {
			BFF_ThrowError(i, strbuf[i].c_str(),
			"the MAP_LVLS/MAP_LEVELS element is specific to map chunks!");
		}
		else {
			BFF_ParseList(file, c_map->levels);
		}
	}
	else if (strbuf[i].find("MOB_LIST") != NotFound
	|| strbuf[i].find("MOB_LS") != NotFound) {
		if (context != CONTEXT_DATA_CHUNK) {
			BFF_ThrowError(i, strbuf[i].c_str(),
			"the MOB_LS/MOB_LIST element is specific to data chunks!");
		}
		else {
			BFF_ParseList(file, c_map->levels); //fixme
		}
	}
	else if (strbuf[i].find("SPAWNER") != NotFound) {
		if (context != CONTEXT_DATA_CHUNK) {
			BFF_ThrowError(i, strbuf[i].c_str(),
			"the SPAWNER element is specific to data chunks!");
		}
		else {
			BFF_ParseSpawner(file, file->bffdata.spawners);
		}
	}
}

void BFF_ParseSubChunk(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	switch (context) {
	case CONTEXT_NO_CHUNK:
		BFF_ThrowError(i, strbuf[i].c_str(), "cannot declare subchunk outside of a context");
		break;
	case CONTEXT_MAP_CHUNK:
		BFF_ParseMapSector(file);
		break;
	case CONTEXT_DATA_CHUNK:
		BFF_ThrowError(i, strbuf[i].c_str(), "subchunks within data chunks must be declared within elements");
		break;
	default:
		BFF_SystemErr("unknown/invalid context!");
		break;
	};
}

void BFF_ParseMapSector(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	++c_map->sectorcounter;
	++i;
	uint16_t y{};
	for (y = 0; y < 120; ++y) {
		for (uint16_t x = 0; x < 120; ++x) {
			c_map->sectormap[c_map->sectorcounter][y][x] = strbuf[i+y][x];
		}
	}
	i += y;
}

void BFF_ParseProperties(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	for (;; ++i) {
		if (ftok(strbuf[i]) == tokens[PROP_DECLARE]
		&& btok(strbuf[i]) == tokens[DATA_CLOSE]) { break; }
		const std::string& str = strbuf[i];
		
		if (ftok(strbuf[i]) == tokens[VAR_DECLARE]) {
			if (str.find("NAME") != NotFound) {
				if (context == CONTEXT_NO_CHUNK) {
					file->bffname = strbuf[i+1];
				}
				else if (context == CONTEXT_MAP_CHUNK) {
					c_map->mapname = strbuf[i+1];
				}
				else {
					BFF_ThrowError(i, str.c_str(), "invalid property var");
				}
			}
		}
	}
}
void BFF_ParseChunkDec(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	bool opening = btok(strbuf[i]) == tokens[DATA_OPEN];
	bool closing = btok(strbuf[i]) == tokens[DATA_CLOSE];
	if (!opening && !closing)
		BFF_ThrowError(i, strbuf[i].c_str(), "invalid chunk syntax");
	
	const std::string& str = strbuf[i];
	if (str.find("MAP") != NotFound) {
		switch (context) {
		case CONTEXT_NO_CHUNK:
			if (opening && !closing) {
				if (file->bffmap) {
					BFF_ThrowError(i, strbuf[i].c_str(), "cannot declare more than one map chunk");
				}
				context = CONTEXT_MAP_CHUNK;
				file->bffmap = (bff_map_t *)Malloc(sizeof(bff_map_t), &file->bffmap);
				c_map = file->bffmap;
			}
			else if (!opening && closing)
				BFF_ThrowError(i, str.c_str(), "cannot close map chunk without opening one");
			break;
		case CONTEXT_MAP_CHUNK:
			if (opening && !closing)
				BFF_ThrowError(i, str.c_str(), "cannot open map chunk within another one");
			else if (!opening && closing)
				context = CONTEXT_NO_CHUNK;
			break;
		case CONTEXT_DATA_CHUNK:
			BFF_ThrowError(i, str.c_str(), "cannot open or close data chunks within map chunks");
			break;
		default:
			BFF_SystemErr("unknown/invalid context");
			break;
		};
	}
	else if (str.find("LVL") != NotFound) {
		file->levels.push_back(BLF_Parse(file, strbuf, i));
	}
	else if (str.find("DATA") != NotFound) {
		switch (context) {
		case CONTEXT_NO_CHUNK:
			if (opening && !closing) {
				context = CONTEXT_DATA_CHUNK;
			}
			else if (!opening && closing)
				BFF_ThrowError(i, str.c_str(), "cannot close data chunk without opening one");
			break;
		case CONTEXT_MAP_CHUNK:
			BFF_ThrowError(i, str.c_str(), "cannot open or close map chunks within data chunks");
			break;
		case CONTEXT_DATA_CHUNK:
			if (opening && !closing)
				BFF_ThrowError(i, str.c_str(), "cannot open data chunk within another one");
			else if (!opening && closing)
				context = CONTEXT_NO_CHUNK;
			break;
		default:
			BFF_SystemErr("unknown/invalid context");
			break;
		};
	}
	else
		BFF_ThrowError(i, str.c_str(), "invalid chunk type/declaration");
}

#define BLF_Error(line,msg,...) \
{ \
	fprintf(stderr, "(ERROR)\nAt Line: %li\nMessage: %s\nContents: ", line, msg); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	fflush(stderr); \
	exit(-1); \
}

#define BLF_Warn(line,...) \
{ \
	fprintf(stderr, "(WARNING)\nAt Line: %li\n  ", line); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	fflush(stderr); \
}

static void BLF_ParseProperties(blf_file_t* level, const std::vector<std::string>& buf, size_t& i)
{
	for (;; ++i) {
		const std::string& str = buf[i];
		if (str.front() == '%'
		&& str.back() == '0') { break; }
		
		if (str.front() == '$') {
			if (str.find("NAME") != NotFound) {
				level->lvl_name = buf[i+1];
			}
			else if (str.find("MAP") != NotFound) {
				std::string tmpstr = buf[i+1];
				if (tmpstr != level->file->bffmap->mapid) {
					BLF_Error(i, buf[i+1].c_str(), "invalid parent mapid given for level chunk");
				}
				if (!level->bffmap) {
					BLF_Error(i, buf[i+1].c_str(), "invalid parent mapid given for level chunk");
				}
			}
			else if (str.find("ID") != NotFound) {
				level->lvl_id = buf[i+1];
			}
			else if (str.find("MAP_HEIGHT") != NotFound) {
				int height = stoi(buf[i+1]);
				if (height > 120) { // hard limit of a sector
					BLF_Error(i, buf[i+1].c_str(), "level map height must be less than 120 pints!");
				}
				level->map_height = height;
			}
			else if (str.find("MAP_WIDTH") != NotFound) {
				int width = stoi(buf[i+1]);
				if (width > 120) { // also hard limit
					BLF_Error(i, buf[i+1].c_str(), "level map width must be less than 120 pints!");
				}
				level->map_width = width;
			}
		}
		else {
			BLF_Error(i, str.c_str(), "invalid property var: %s", str.c_str());
		}
	}
}

void BLF_ParseDumbChunk(blf_file_t* level, const std::vector<std::string>& buf, size_t& i)
{
	if (level->map_height == 0 || level->map_width == 0) {
		BLF_Error(i, buf[i].c_str(), "must define level properties before dumbchunk!");
	}
	++i;
	size_t y{};
	level->lvl_map = (char **)Malloc((sizeof(char*)*level->map_height)+2, &level->lvl_map);
	for (y = 1; y < level->map_height; ++y) {
		if (strbuf[y].size() == 1 && buf[y].front() == ']') {
			BLF_Error(y, buf[y].c_str(), "premature ending to level map!");
		}
		level->lvl_map[y] = (char *)Malloc((sizeof(char)*level->map_width)+2, &level->lvl_map[y]);
		if (buf[y].size() > level->map_width) {
			BLF_Warn(y, "level map width at this line is greater than the assigned width,\n"
						"  anything within the dumbchunk exceeding the limit will be ignored\n");
		}
		for (int x = 1; x < level->map_width; ++x) {
			level->lvl_map[y][0] = '#';
			level->lvl_map[y][level->map_width+1] = '#';
			level->lvl_map[y][x] = buf[i+y][x];
		}
	}
	memset(&level->lvl_map[0], '#', level->map_width+2);
	memset(&level->lvl_map[level->map_height+1], '#', level->map_width+2);
	if (buf[y].front() != ']') {
		BLF_Warn(y, "level map height within this dumbchunk is greater than the assigned height\n"
					"  anything within the dumbchunk exceeding the limit will be ignored\n");
		while (buf[y].front() != ']') {
			++y;
		}
	}
	i += y;
}

blf_file_t* BLF_Parse(bff_file_t* file, const std::vector<std::string>& buf, size_t& i)
{
	if (!file)
		return (blf_file_t *)NULL;
	blf_file_t* level = (blf_file_t *)Malloc(sizeof(blf_file_t), &level);
	level->file = file;
	for (;; ++i) {
		if (buf[i].front() == '#'
		&& buf[i].back() == '0') { break; }
		
		switch (buf[i].front()) {
		case '[':
			BLF_ParseDumbChunk(level, buf, i);
			break;
		case '%':
			BLF_ParseProperties(level, buf, i);
			break;
		};
	}
	bool linked = false;
	for (const auto& i : c_map->levels) {
		if (i == level->lvl_id) {
			linked = true;
		}
	}
	if (!linked) {
		fprintf(stderr, "ERROR: Failed to link LVL chunk with a MAP chunk, check the casing/spelling?\n");
		BFF_Free(file);
		exit(-1);
	}
	return level;
}