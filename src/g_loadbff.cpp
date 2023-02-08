#include "g_game.h"
#define BFF_IMPLEMENTATION
#include "g_loadbff.h"
#ifdef UNIX_NOMAD
#define DL_ERROR() dlerror()
#elif defined(WIN32_NOMAD)
#define DL_ERROR() GetLastError()
#endif

void G_LoadBFF(const char* bffname, Game* const game)
{
#ifdef _NOMAD_DEBUG
    assert(bffname && game);
#endif
#ifdef UNIX_NOMAD
	typedef void (*bff_func)(bff_file_t*);
	typedef unsigned char (*filetype)(bff_file_t*);
	typedef bff_file_t*(*init_file)(const char*);
	void *bffhandle = dlopen("./libbffproject.so.0.1", RTLD_NOW | RTLD_LOCAL);
	if (!bffhandle)
		N_Error("Could not load libbffproject.so.0.1 into memory! system error: %s", DL_ERROR());
	init_file init;
	*(void **)(&init) = dlsym(bffhandle, "BFF_InitFile");
	bff_func parse;
	*(void **)(&parse) = dlsym(bffhandle, "BFF_Parse");
	bff_file_t* file = (bff_file_t*)(*init)(bffname);
	filetype getfiletype;
	bff_func kill;
	*(void **)(&kill) = dlsym(bffhandle, "BFF_Free");
	*(void **)(&getfiletype) = dlsym(bffhandle, "BFF_GetFileType");
	unsigned char bfftype = (*getfiletype)(file);
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
#elif defined(WIN32_NOMAD)
	typedef void (__stdcall *bff_func)(bff_file_t*);
	typedef unsigned char (__stdcall *filetype)(bff_file_t*);
	typedef bff_file_t*(__stdcall *init_file)(const char*);
	HINSTANCE bffhandle = LoadLibrary("libbffproject.dll");
    if (!bffhandle) {
        N_Error("Failed to open dynamic link library! error message: %s",
            DL_ERROR());
    }
#ifdef _NOMAD_DEBUG
    assert(bffhandle);
    LOG("Successfully opened bff dynamic link library");
#endif
    bff_func parse, kill;
	filetype getfiletype;
	init_file initfile;
	initfile = (init_file)GetProcAddress(bffhandle, "BFF_InitFile");
	getfiletype = (filetype)GetProcAddress(bffhandle, "BFF_GetFileType");
	kill = (bff_func)GetProcAddress(bffhandle, "BFF_Free");
	bff_file_t* file = initfile(bffname);
    unsigned char ftype = getfiletype(file);
    if (ftype == BFF_COMPILE)
        N_Error("The Nomad-ASCII currently only supports .bffl files");
//    DL_DO(parse, file);

//    FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	/*
	076
	185
	234
	*/
/*	if (!fp)
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
    fclose(fp); */
	kill(file);
	FreeLibrary(bffhandle);
#endif
}