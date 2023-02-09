#include "g_game.h"
#define BFF_IMPLEMENTATION
#include "g_loadbff.h"
#define DL_ERROR() dlerror()

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

void G_LoadBFF(const char* bffname, Game* const game)
{
#ifdef _NOMAD_DEBUG
    assert(bffname && game);
#endif
	typedef void (*bff_func)(bff_file_t*);
	typedef unsigned char (*filetype)(bff_file_t*);
	typedef bff_file_t*(*init_file)(const char*);
	bffhandle = dlopen("./libbffproject.so.0.1", RTLD_NOW | RTLD_LOCAL);
	if (!bffhandle)
		N_Error("Could not load libbffproject.so.0.1 into memory! system error: %s", DL_ERROR());
	init_file init;
	LOAD(init, "BFF_InitFile");
	bff_func parse;
	LOAD(parse, "BFF_Parse");
	bff_file_t* file = (bff_file_t*)(*init)(bffname);
	filetype getfiletype;
	bff_func kill;
	LOAD(kill, "BFF_Free");
	LOAD(getfiletype, "BFF_GetFileType");
	unsigned char bfftype = (unsigned char)(*getfiletype)(file);
	if (bfftype == BFF_COMPILE)
		N_Error(".bffc (compiled bff's) aren't yet compatible with The Nomad-ASCII");
	(*parse)(file);
	FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	/*
	076
	185
	234
	*/
	if (!fp)
		N_Error("Could Not Create RUNTIME/mapfile.txt!");
#ifdef _NOMAD_DEBUG
	assert(fp);
	LOG("Successfully created RUNTIME/mapfile.txt");
#endif
    nomadshort_t y{}, x{};
    for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X; ++x) {
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
		for (x = 0; x < MAP_MAX_X; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
    fclose(fp);
	(*kill)(file);
	dlclose(bffhandle);
}