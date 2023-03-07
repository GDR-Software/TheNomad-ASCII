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

static Game* gptr;

Level::~Level()
{
	for (auto* i : mspawners)
		Z_Free(i);
	for (auto* i : ispawners)
		Z_Free(i);
	for (auto* i : wspawners)
		Z_Free(i);
}

void Level::G_LoadSpawners(std::shared_ptr<BFF>& bff)
{
	std::copy(bff->spawners.begin(), bff->spawners.end(), std::back_inserter(spawners));
	nomadshort_t y{}, x{};
	for (auto& i : spawners) {
		if (i->et_type == ET_MOB) {
			mspawners.emplace_back();
			mspawners.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &mspawners.back());
			Mob* const mob = mspawners.back();
			i->et_ptr = (void *)mob;
			if (i->et_name == "MT_HULK") {
				mob->c_mob = mobinfo[MT_HULK];
			} else if (i->et_name == "MT_RAVAGER") {
				mob->c_mob = mobinfo[MT_RAVAGER];
			} else if (i->et_name == "MT_GRUNT") {
				mob->c_mob = mobinfo[MT_GRUNT];
			} else if (i->et_name == "MT_PISTOL") {
				mob->c_mob = mobinfo[MT_PISTOL];
			} else if (i->et_name == "MT_SHOTTY") {
				mob->c_mob = mobinfo[MT_SHOTTY];
			} else if (i->et_name == "MT_GUNNER") {
				mob->c_mob = mobinfo[MT_GUNNER];
			}
		} else if (i->et_type == ET_PLAYR) {
			i->et_name = "PLAYR";
			pspawners.push_back(std::move(i));
			i->et_ptr = (void *)gptr->playr;
		}
		char *marker = (char *)NULL;
		for (y = 0; y < SECTOR_MAX_Y; ++y) {
			for (x = 0; x < SECTOR_MAX_X; ++x) {
				if (lvl_map[y][x] == i->marker) {
					marker = &lvl_map[y][x];
				}
			}
		}
		if (!marker) {
			N_Error("Level::G_LoadSpawners: failed to locate marker %c in level %s", i->marker, lvl_name.c_str());
		}
		i->pos = {y, x};
		i->pos.y += 200;
		i->pos.x += 200;
		*marker = i->replacement;
	}
}

void G_LoadBFF(const char* bffname, Game* const game)
{
	gptr = game;
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