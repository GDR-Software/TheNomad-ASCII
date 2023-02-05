#include "g_game.h"
#include "g_loadbff.h"

#define LIB_BFF "./libbffproject.so.0.1"
#define DL_HANDLE void*
#define DL_OPEN(name) dlopen(name, RTLD_NOW | RTLD_LOCAL)
#define DL_GET(handle, name) dlsym(handle, name)
#define DL_ERROR() dlerror()
#define DL_CLOSE(handle) dlclose(handle)

static DL_HANDLE bffhandle;

#define LOAD(user, func) \
{ \
    *(void **)(&user) = DL_GET(bffhandle, func); \
    if (!(user)) { \
        N_Error("Failed to link BFF function %s to %s!", \
            #user, \
            func); \
    } \
}

typedef bff_file_t* (*init_func)(const char*);
typedef void (*bff_func)(bff_file_t*);
typedef unsigned char (*get_type)(bff_file_t*);

void G_LoadBFF(const char* bffname, Game* const game)
{
#ifdef _NOMAD_DEBUG
    assert(bffname && game);
#endif
    bffhandle = DL_OPEN(LIB_BFF);
    if (!bffhandle) {
        N_Error("Failed to open dynamic link library! error message: %s",
            DL_ERROR());
    }
#ifdef _NOMAD_DEBUG
    assert(bffhandle);
    LOG("Successfully opened bff dynamic link library");
#endif
    init_func initfile;
    bff_func parse, kill;
    get_type bfftype;
    LOAD(initfile, "BFF_InitFile");
    LOAD(bfftype, "BFF_GetFileType");
    LOAD(parse, "BFF_Parse");
    LOAD(kill, "BFF_Free");
    bff_file_t* file = (bff_file_t *)(*initfile)(bffname);
    unsigned char ftype = (unsigned char)(*bfftype)(file);
    if (ftype == BFF_COMPILE)
        N_Error("The Nomad-ASCII currently only supports .bffl files");
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
}