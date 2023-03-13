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
#include "g_rng.h"

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

void Level::G_LoadSpawners(std::shared_ptr<BFF>& bff, char c_map[9][120][120])
{
	std::copy(bff->spawners.begin(), bff->spawners.end(), std::back_inserter(spawners));
	nomadshort_t y{}, x{};

	struct marker
	{
		coord_t pos;
		char *spr;
		sprite_t replacement;
		inline marker(coord_t _pos, char *_spr, sprite_t _replacement)
			: pos(_pos), spr(_spr), replacement(_replacement)
		{
		}
	};

	// game->m_Active is expected to be completely empty at this time
	if (gptr->m_Active.size() > 0) {
		LOG_WARN("G_StartupCampaign hasn't yet cleared game->m_Active, doing so now");
		for (std::vector<Mob*>::iterator it = gptr->m_Active.begin(); it != gptr->m_Active.end(); ++it) {
			M_KillMob(it);
		}
		gptr->m_Active.clear();
	}
	std::vector<marker> markers;
	for (auto &i : spawners) {
		for (y = 0; y < SECTOR_MAX_Y; ++y) {
			for (x = 0; x < SECTOR_MAX_X; ++x) {
				if (lvl_map[y][x] == i->marker) {
					marker mark{ {y, x}, (char *)&c_map[8][y][x], i->replacement };
					markers.push_back(mark);
					LOG_INFO("found marker at %hu, %hu, sprite is %c, replacement is %c", y, x, *mark.spr, i->replacement);
				}
			}
		}
		for (auto& m : markers) {
			if (i->et_type == ET_MOB) {
				gptr->m_Active.emplace_back();
				gptr->m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &gptr->m_Active.back());
				Mob* const mob = gptr->m_Active.back();
				i->et_ptr = (void *)mob;
				for (auto& s : mobinfo) {
					if (i->et_name == s.name) {
						mob->c_mob = s;
					}
				}
				mob->mpos = m.pos;
				mob->mpos.y += 201;
				mob->mpos.x += 201;
				mob->health = mob->c_mob.health;
				mob->armor = mob->c_mob.armor;
				mob->mdir = P_Random() & 3;
				mob->sprite = mob->c_mob.sprite;
				mob->stepcounter = (P_Random() & 18)+5;
				mob->mstate = stateinfo[S_MOB_WANDER];
				mob->mticker = mob->mstate.numticks;
			}
			*m.spr = i->replacement;
		}

		markers.clear();
	}
}

void G_LoadBFF(const char* bffname, Game* const game)
{
	gptr = game;
	printf("G_LoadBFF: loading bff file into memory...\n");
	LOG_INFO("Loading bff directory %s into memory", bffname);
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
	file->BFF_LinkSpawners(data);
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
		for (x = 0; x < MAP_MAX_X+160; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) { fprintf(fp, "#"); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[0][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[7][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[6][y][x]); }
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) { fprintf(fp, "#"); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[1][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[8][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[5][y][x]); }
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[2][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[3][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", bffmap->map_buffer[4][y][x]); }
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
    for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
    fclose(fp);
	LOG_INFO("closing write-only mapfile.txt");
	game->I_InitHUD();
}