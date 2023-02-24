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
	bff_file_t file;
	BFF_Parse(&file, bffname);
	bff_map_t* const bffmap = file.bffmap;
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
			fprintf(fp, "%c", bffmap->mapbuffer[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->mapbuffer[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->mapbuffer[6][y][x]);
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
			fprintf(fp, "%c", bffmap->mapbuffer[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->mapbuffer[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->mapbuffer[5][y][x]);
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
			fprintf(fp, "%c", bffmap->mapbuffer[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->mapbuffer[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->mapbuffer[4][y][x]);
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
	LOG_INFO("reserving memory for bff level chunks");
	if (file.levels.size() < 1) {
		LOG_INFO("no level chunks to reserve");
	}
	else {
	//	G_AllocLevelList(file.levels);
	}

	BFF_Free(&file);
	game->I_InitHUD();
}

#define CONTEXT_NO_CHUNK 0
#define CONTEXT_MAP_CHUNK 1
#define CONTEXT_DATA_CHUNK 2
#define CONTEXT_LVL_CHUNK 3

static int context;
static bff_file_t* c_file;
static bff_lvl_t* c_lvl;

#define BFF_SystemErr(...) fprintf(stderr,"Internal Error: ");fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");exit(-1)

static void BFF_ThrowError(uint64_t line, const char* buf, const char* err, ...)
{
    fprintf(stderr, "(ERROR)\nAt Line: %lu\nContents: %s\nMessage: ", line+1, buf);
    va_list argptr;
    va_start(argptr, err);
    vfprintf(stderr, err, argptr);
    va_end(argptr);
    fprintf(stderr, "\n");
    fflush(stderr);
    BFF_Free(c_file);
    exit(-1);
}

static void BFF_GetTokens(bff_file_t* file, bff_runtime_t* runtime)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	size_t line = 0;
	for (size_t i = 0; i < runtime->strbuf.size(); ++i) {
		runtime->tokbuf.emplace_back();
		token_t* const tok = &runtime->tokbuf.back();
		tok->ftok = runtime->strbuf[i].front();
		tok->btok = runtime->strbuf[i].back();
		tok->line = line;
		tok->buffer = runtime->strbuf[i];
		++line;
	}
}

BFF_API void BFF_Free(bff_file_t* file)
{
    if (!file)
        return;
    
    for (auto* i : file->levels) {
        for (int y = 0; y < i->map_height; ++y) {
            free(i->lvl_map[y]);
        }
        free(i->lvl_map);
        free(i);
    }
    free(file->run);
    free(file->bffmap);
}

void parse_properties(bff_file_t* file);
void parse_chunk_dec(bff_file_t* file);
void parse_dumbchunk(bff_file_t* file);
void parse_subchunk(bff_file_t* file);
void parse_list(bff_file_t* file);
void parse_map_sector(bff_file_t* file);
void parse_lvl_map(bff_file_t* file);

BFF_API void BFF_Parse(bff_file_t* file, const std::string& filename)
{
	if (!file) return;
    file->run = (bff_runtime_t*)malloc(sizeof(bff_runtime_t));
	context = CONTEXT_NO_CHUNK;
	std::ifstream in(filename, std::ios::in);
    if (in.fail()) {
        BFF_SystemErr("std::ifstream in.open failed!");
    }
    std::vector<std::string>& strbuf = file->run->strbuf;
    std::string line;
    while (std::getline(in, line)) { strbuf.push_back(line); }
    in.close();
	LOG("successfully opened bff file, parsing");
    BFF_GetTokens(file, file->run);
	LOG("fetched bff tokens");
	context = CONTEXT_NO_CHUNK;
    LOG("parsing file...");
	for (file->run->i = 0; file->run->i < file->run->tokbuf.size(); ++file->run->i) {
		switch (file->run->tokbuf[file->run->i].ftok) {
		case tokens[NULL_TOK]:
		case tokens[NEWLINE]:
			break;
		case tokens[PROP_DECLARE]:
			parse_properties(file);
			break;
		case tokens[SUBCHUNK_OPEN]:
			parse_subchunk(file);
			break;
		case tokens[ELEMENT_DECLARE]:
//			BFF_ParseElement(file);
			break;
		case tokens[CHUNK_DECLARE]:
			parse_chunk_dec(file);
			break;
		};
	}
}

void parse_subchunk(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	bff_runtime_t* r = file->run;
	LOG("found a subchunk at line: %lu", r->i);
	switch (context) {
	case CONTEXT_NO_CHUNK:
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(), "cannot declare subchunk outside of a context");
		break;
	case CONTEXT_MAP_CHUNK:
		parse_map_sector(file);
		break;
	case CONTEXT_LVL_CHUNK:
		parse_lvl_map(file);
		break;
	case CONTEXT_DATA_CHUNK:
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(), "subchunks within data chunks must be declared within elements");
	default:
		BFF_SystemErr("unknown/invalid parser context!");
		break;
	};
}

void parse_lvl_map(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	bff_runtime_t* r = file->run;
	if (c_lvl->map_height == 0 || c_lvl->map_width == 0) {
		BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(),
			"must declare level map height and width in chunk properties before anything else");
	}
	LOG("subchunk type: lvl_map");
	c_lvl->lvl_map = (char **)malloc(sizeof(char *)*c_lvl->map_height);
	if (!c_lvl->lvl_map) {
		BFF_SystemErr("malloc failed!");
	}
	uint16_t y{};
	const std::vector<token_t>& tokbuf = r->tokbuf;
	for (y = 0; y < c_lvl->map_height; ++y) {
		if (tokbuf[y+r->i].buffer.size() < c_lvl->map_width) {
			BFF_ThrowError(y+r->i, tokbuf[y+r->i].buffer.c_str(),
				"level map width is less than declared width of %hu pixels", c_lvl->map_width);
		}
		if (tokbuf[y+r->i].buffer.size() == 1 && tokbuf[y+r->i].ftok == tokens[SUBCHUNK_CLOSE]) {
			BFF_ThrowError(y+r->i, tokbuf[y+r->i].buffer.c_str(),
				"premature ending to level map");
		}
		c_lvl->lvl_map[y] = (char *)malloc(sizeof(char)*c_lvl->map_width);
		if (!c_lvl->lvl_map[y]) {
			BFF_SystemErr("malloc failed!");
		}
		for (uint16_t x = 0; x < c_lvl->map_width; ++x) {
			c_lvl->lvl_map[y][x] = tokbuf[y+r->i].buffer[x];
		}
	}
	if (tokbuf[y+r->i].ftok != tokens[SUBCHUNK_CLOSE] && tokbuf[y+r->i].buffer.size() > 1) {
		BFF_ThrowError(y+r->i, tokbuf[y+r->i].buffer.c_str(),
			"expected level subchunk to end here");
	}
}
static int sectorcounter = 0;
void parse_map_sector(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	LOG("subchunk type: mapsector");
	++file->run->i;
	uint16_t y{};
	for (y = 0; y < 120; ++y) {
        if (file->run->tokbuf[file->run->i+y].buffer.front() == tokens[SUBCHUNK_CLOSE]) {
            BFF_ThrowError(y+file->run->i, file->run->tokbuf[y+file->run->i].buffer.c_str(),
                "premature ending to map-sector subchunk!");
        }
		for (uint16_t x = 0; x < 120; ++x) {
            if (file->run->tokbuf[file->run->i+y].buffer.size() < 120) {
                BFF_ThrowError(y+file->run->i, file->run->tokbuf[y+file->run->i].buffer.c_str(),
					"map-sector subchunk line is less than 120 pixels!"); // pints, techincally
            }
			file->bffmap->mapbuffer[sectorcounter][y][x] = file->run->tokbuf[file->run->i+y].buffer[x];
		}
	}
	++sectorcounter;
	file->run->i += y;
}

void parse_properties(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	LOG("found bff properties");
	bff_runtime_t *r = file->run;
	for (;; ++r->i) {
		if (r->tokbuf[r->i].ftok == tokens[PROP_DECLARE]
		&& r->tokbuf[r->i].btok == tokens[DATA_CLOSE]) { break; }
		const std::string strbuf = r->tokbuf[r->i].buffer;
		if (r->tokbuf[r->i].ftok == tokens[VAR_DECLARE]) {
			if (strbuf.find("NAME") != NotFound) {
				if (context == CONTEXT_NO_CHUNK)
					file->bffname = r->tokbuf[r->i+1].buffer;
				else if (context == CONTEXT_MAP_CHUNK)
					file->bffmap->mapname = r->tokbuf[r->i+1].buffer;
				else if (context == CONTEXT_LVL_CHUNK)
					c_lvl->lvl_name = r->tokbuf[r->i+1].buffer;
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			}
			else if (strbuf.find("ID") != NotFound) {
				if (context == CONTEXT_MAP_CHUNK)
					file->bffmap->mapid = r->tokbuf[r->i+1].buffer;
				else if (context == CONTEXT_LVL_CHUNK)
					file->levels.back()->lvl_id = r->tokbuf[r->i+1].buffer;
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			}
			else if (strbuf.find("MAP_HEIGHT") != NotFound) {
				if (context == CONTEXT_LVL_CHUNK) {
					int i = stoi(r->tokbuf[r->i].buffer);
					if (i > 120) {
						BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(),
							"bff level maps cannot be greater height or width than a 120x120 sector/biome");
					}
					file->levels.back()->map_height = (uint8_t)i;
				}
				else
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
			}
			else if (strbuf.find("MAP_WIDTH") != NotFound) {
				if (context == CONTEXT_LVL_CHUNK) {
					int i = stoi(r->tokbuf[r->i].buffer);
					if (i > 120) {
						BFF_ThrowError(r->i, r->tokbuf[r->i].buffer.c_str(),
							"bff level maps cannot be greater height or width than a 120x120 sector/biome");
					}
					file->levels.back()->map_width = (uint8_t)i;
				}
				else {
					BFF_ThrowError(r->i, strbuf.c_str(), "invalid property var");
				}
			}
        }
	}
}

void parse_chunk_dec(bff_file_t* file)
{
	ASSERT(file, "Error: bff_file_t* file == NULL!");
	bff_runtime_t* r = file->run;
	size_t i = r->i;
	LOG("found a chunk declaration at line: %lu", i);
	bool opening = r->tokbuf[i].btok == tokens[DATA_OPEN];
	bool closing = r->tokbuf[i].btok == tokens[DATA_CLOSE];
	if (!opening && !closing)
		BFF_ThrowError(i, r->tokbuf[i].buffer.c_str(), "invalid chunk syntax");
    
	const std::string& strbuf = r->tokbuf[i].buffer;
	if (strbuf.find("MAP") != NotFound) {
		switch (context) {
		case CONTEXT_NO_CHUNK:
			if (opening && !closing) {
				context = CONTEXT_MAP_CHUNK;
                if (file->bffmap) {
                    BFF_ThrowError(i, strbuf.c_str(), "there cannot be more than one map!");
                }
                file->bffmap = (bff_map_t*)malloc(sizeof(bff_map_t));
                if (!file->bffmap) {
                    BFF_SystemErr("malloc failed!");
                }
			}
			else if (!opening && closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot close map chunk without opening one");
			break;
		case CONTEXT_MAP_CHUNK:
			if (opening && !closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot open map chunk within another one");
			else if (!opening && closing)
				context = CONTEXT_NO_CHUNK;
			break;
		case CONTEXT_DATA_CHUNK:
			BFF_ThrowError(i, strbuf.c_str(), "cannot open or close data chunks within map chunks");
			break;
		default:
			BFF_SystemErr("unknown/invalid parser context!");
			break;
		};
	}
	else if (strbuf.find("LVL") != NotFound) {
		file->levels.emplace_back();
		file->levels.back() = (bff_lvl_t *)malloc(sizeof(bff_lvl_t));
		if (!file->levels.back()) {
			BFF_SystemErr("malloc failed!");
		}
		context = CONTEXT_LVL_CHUNK;
		c_lvl = file->levels.back();
	}
	else if (strbuf.find("DATA") != NotFound) {
		switch (context) {
		case CONTEXT_NO_CHUNK:
			if (opening && !closing) {
				context = CONTEXT_DATA_CHUNK;
			}
			else if (!opening && closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot close data chunk without opening one");
			break;
		case CONTEXT_MAP_CHUNK:
			BFF_ThrowError(i, strbuf.c_str(), "cannot open or close map chunks within data chunks");
			break;
		case CONTEXT_DATA_CHUNK:
			if (opening && !closing)
				BFF_ThrowError(i, strbuf.c_str(), "cannot open data chunk within another one");
			else if (!opening && closing)
				context = CONTEXT_NO_CHUNK;
			break;
		default:
			BFF_SystemErr("unknown/invalid parser context!");
			break;
		};
	}
	else
		BFF_ThrowError(i, strbuf.c_str(), "invalid chunk type/declaration");
}