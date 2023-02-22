#include "g_game.h"
#define BFF_IMPLEMENTATION
#include "g_bff.h"

static void *bffhandle;

#define LOAD(user, func)                                               \
{                                                                      \
	*(void **)(&user) = dlsym(bffhandle, func);                        \
	if (!user) {                                                       \
		N_Error("Failed to link function %s to %s! system error: %s",  \
			#user                                                      \
			func, dlerror());                                          \
	}                                                                  \
}

nomadbool_t using_slf = false;

#define NOMAD_RTLD RTLD_NOW | RTLD_LOCAL

void G_LoadBFF(const char* bffname, Game* const game)
{
    assert(bffname && game);
	typedef void (*bff_func)(bff_file_t*);
	typedef unsigned char (*filetype)(bff_file_t*);
	typedef bff_file_t*(*init_file)(const char*);
	bffhandle = dlopen("./libbffproject.so.0.1", NOMAD_RTLD);
	NOMAD_ASSERT(bffhandle,
		"Could not load libbffproject.so.0.1 into memory! system error: %s", dlerror());
	LOG_INFO("sucessfully loaded libbffproject.so.0.1 into memory with dlopen()");
	init_file init;
	LOAD(init, "BFF_InitFile");
	LOG_INFO("successfully linked init to BFF_InitFile");
	bff_func parse;
	LOAD(parse, "BFF_Parse");
	LOG_INFO("successfully linked parse to BFF_Parse");
	bff_file_t* file = (bff_file_t*)(*init)(bffname);
	PTR_CHECK(NULL_CHECK, file);
	LOG_INFO("successfully allocated memory to bff_file_t* file");
	filetype getfiletype;
	bff_func kill;
	LOAD(kill, "BFF_Free");
	LOG_INFO("successfully linked kill to BFF_Free");
	LOAD(getfiletype, "BFF_GetFileType");
	LOG_INFO("successfully linked getfiletype to BFF_GetFileType");
	unsigned char bfftype = (unsigned char)(*getfiletype)(file);
	if (bfftype != BFF_COMPILE || BFF_DECOMPILE) {
		LOG_WARN("bfftype was not equal to BFF_COMPILE or BFF_DECOMPILE! assigning default value of BFF_DECOMPILE");
		bfftype = BFF_DECOMPILE;
	}
	NOMAD_ASSERT(bfftype != BFF_COMPILE,
		".bffc (compiled bff's) aren't yet compatible with The Nomad-ASCII");
	(*parse)(file);
	if (using_slf) {
		void *ptr = R_Malloc(64, TAG_STATIC, ptr);
	}
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
			fprintf(fp, "%c", file->mchunk.sectormap[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", file->mchunk.sectormap[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", file->mchunk.sectormap[6][y][x]);
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
			fprintf(fp, "%c", file->mchunk.sectormap[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", file->mchunk.sectormap[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", file->mchunk.sectormap[5][y][x]);
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
			fprintf(fp, "%c", file->mchunk.sectormap[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", file->mchunk.sectormap[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", file->mchunk.sectormap[4][y][x]);
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
	(*kill)(file);
	LOG_INFO("freeing/cleaning up libbffproject memory");
	dlclose(bffhandle);
	LOG_INFO("deallocating dynamic library loader handle");
	game->I_InitHUD();
}