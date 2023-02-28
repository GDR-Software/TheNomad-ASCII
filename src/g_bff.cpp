//#include "g_bff.h"
#include "n_shared.h"

#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"

void G_LoadBFF(const char* bffname, Game* const game)
{
	printf("G_LoadBFF: loading bff file into memory...\n");
	std::ifstream in(std::string("Files/gamedata/BFF/"+std::string(bffname)+"/entries.json"), std::ios::in);
	NOMAD_ASSERT(in.is_open(), "failed to open bff file %s!", bffname);
	std::shared_ptr<BFF> file = std::make_shared<BFF>();
	json data = json::parse(in);
	file->Init(std::string("Files/gamedata/BFF/"+std::string(bffname)+"/entries.json"));
	file->game_version[0] = data["properties"]["nomad_version"]["version.major"];
	file->game_version[1] = data["properties"]["nomad_version"]["version.update"];
	file->game_version[2] = data["properties"]["nomad_version"]["version.patch"];
	if (file->game_version[0] > _NOMAD_VERSION) {
		fprintf(stderr, "Warning: BFF game's version is not equal to this game's version major, proceed? [y\n]\n");
		char c = getc(stdin);
		if (c != 'y') {
			exit(1);
		}
		fprintf(stdout, "Proceeding with loading.\n");
	}
	file->BFF_LoadLevels(data);
	file->BFF_LoadMaps(data);
	file->BFF_LinkMaps(data);
	file->BFF_LoadSectors(data);
	game->bff = file;
	in.close();
	game->playr->c_lvl = file->levels[0];
	std::shared_ptr<Map>& bffmap = file->maps[0];
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
			fprintf(fp, "%c", bffmap->map_buffer[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->map_buffer[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->map_buffer[6][y][x]);
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
			fprintf(fp, "%c", bffmap->map_buffer[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->map_buffer[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->map_buffer[5][y][x]);
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
			fprintf(fp, "%c", bffmap->map_buffer[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->map_buffer[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", bffmap->map_buffer[4][y][x]);
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
	game->I_InitHUD();
}