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

typedef enum
{
	demo, // demo - the first version, most buggy, will always be free
	shareware, // the stuff that's distributed for free but with much more content than the demo
	commercial, // the official paid-for version

	unknown // invalid type
} bfftype_t;

// only the official nomadascii bffs use this struct
typedef struct defined_bff_s
{
	const char* fname;
	bfftype_t type;
	const char* rname;
} defined_bff_t;

#define MAIN       0
#define SHAREWARE  1
#define DEMO       2
#define BLOODBURNE 3
#define PHANTOMSIX 4
#define BBSHARE    5
#define PSSHARE    6

static defined_bff_t defined_bffs[] = {
	{"nomadmain.bff",  commercial, "The Nomad: Tales From a Lost Land"},
	{"nomadshare.bff", shareware,  "The Nomad (Shareware)"},
	{"nomaddemo.bff",  demo,       "The Nomad (Demo)"},
	{"bloodburne.bff", commercial, "Bloodburne: World War 3"},
	{"phantomsix.bff", commercial, "The Phantom Six"},
	{"bbshare.bff",    shareware,  "Bloodburne (Shareware Version)"},
	{"psshare.bff",    shareware,  "The Phantom Six (Shareware Version)"},
};

static Game* gptr;

Level::~Level()
{
	mspawners.clear();
	wspawners.clear();
	ispawners.clear();
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
		for (linked_list<Mob*>::iterator it = gptr->m_Active.begin(); it != gptr->m_Active.end(); it = it->next) {
			Z_Free(it->val);
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
				gptr->m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, NULL);
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
	nomadenum_t bff_num = MAIN;
	if (in.fail() && !N_strcmp(bffname, defined_bffs[MAIN].fname)) {
		nomadbool_t good = false;
		for (nomadenum_t i = 1; i < ARRAY_SIZE(defined_bffs) && !good; i++) {
			strncpy(game->bffname, defined_bffs[i].fname, sizeof(game->bffname));
			in.open(std::string("Files/gamedata/BFF/"+std::string(bffname)+"/entries.json"), std::ios::in);
			if (in.is_open()) {
				good = true;
			}
		}
	}
	bffname = game->bffname;
	if (bff_num == MAIN || bff_num == SHAREWARE || bff_num == DEMO) {
		printf("G_LoadBFF: BFF is an official/pre-defined bff: %s\n", defined_bffs[bff_num].rname);
	}
	else {
		printf("G_LoadBFF: BFF is custom\n");
	}
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
	const std::shared_ptr<Map>& bffmap = file->maps[0];
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

static const char* BFF_GetString(const std::string in)
{
	return in.c_str();
}

void BFF::BFF_LoadSectors(json& data)
{
	for (nomadint_t i = 0; i < maps.size(); ++i) {
		for (nomadint_t s = 0; s < NUM_SECTORS; ++s) {
			std::string node_name = "mapsector_"+std::to_string(s);
			std::string map_node = "map_"+std::to_string(i);
			maps[i]->sectors.emplace_back(static_cast<Sector*>(Z_Malloc(sizeof(Sector), TAG_STATIC, NULL)));
			std::shared_ptr<Sector>& ptr = maps[i]->sectors.back();
			ptr->map_link = maps[i];
			ptr->sector_name = BFF_GetString(data["maps"][map_node][node_name]["name"]);
			ptr->sector_id = BFF_GetString(data["maps"][map_node][node_name]["id"]);
			const std::string mapfile = data["maps"][map_node][node_name]["mapfile"];
			std::ifstream file(FILE_NAME(mapfile), std::ios::in);
			NOMAD_ASSERT(file.is_open(), "failed to open map sector file %s!", mapfile.c_str());
			std::string line;
			nomadshort_t y{}, x{};
			y = x = 0;
			while (std::getline(file, line)) {
				if (y > 119) {
					break;
				}
				for (x = 0; x < 120; ++x) {
					ptr->sector_map[y][x] = line[x];
				}
				++y;
			}
			file.close();
			for (y = 0; y < 120; ++y) {
				for (x = 0; x < 120; ++x) {
					maps[i]->map_buffer[s][y][x] = ptr->sector_map[y][x];
				}
			}
		}
	}
}

void BFF::BFF_LinkMaps(json& data)
{
	nomadint_t i{};
	for (i = 0; i < maps.size(); ++i) {
		std::shared_ptr<Map>& ptr = maps[i];
		const std::string node_name = "map_"+std::to_string(i);
		std::vector<std::string> lvl_names;
		for (auto& l : data["maps"][node_name]["levels"]) {
			lvl_names.push_back(BFF_GetString(l));
		}
		for (const auto& l : lvl_names) {
			std::shared_ptr<Level>* lvl = nullptr;
			for (auto& a : levels) {
				if (a->lvl_id == l) {
					lvl = &a;
				}
			}
			NOMAD_ASSERT(lvl, "could not find level with name %s", l.c_str());
			ptr->levels.emplace_back(*lvl);
		}
	}
	for (i = 0; i < levels.size(); ++i) {
		const std::string node_name = "lvl_"+std::to_string(i);
		const std::string mapname = BFF_GetString(data["levels"][node_name]["map"]);
		std::shared_ptr<Map>* ptr = nullptr;
		std::shared_ptr<Level>& lvl = levels[i];
		for (auto& m : maps) {
			if (m->map_id == mapname) {
				ptr = &m;
			}
		}
		NOMAD_ASSERT(ptr, "could not find map with name %s for level linkage", mapname.c_str());
		lvl->map_link = *ptr;
	}
}

void BFF::BFF_LoadMaps(json& data)
{
	nomadint_t nummaps = data["maps"]["num_maps"];
	maps.reserve(1);
	for (nomadint_t i = 0; i < nummaps; ++i) {
		maps.emplace_back(new Map());
		std::shared_ptr<Map>& ptr = maps.back();
		std::string node_name = "map_"+std::to_string(i);
		ptr->map_name = BFF_GetString(data["maps"][node_name]["name"]);
		ptr->map_id = BFF_GetString(data["maps"][node_name]["id"]);
// 		{
//			const std::string filename = data["maps"][node_name]["introfile"];
//			std::ifstream file(FILE_NAME(filename), std::ios::in);
//			//NOMAD_ASSERT(file.is_open(), "failed to open map intro screen %s!", filename.c_str());
//			std::string line;
//			while (eastl::getline(file, line)) { ptr->intro_screen.push_back(line); }
//			file.close();
//		}
//		{
//			const std::string filename = data["maps"][node_name]["endfile"];
//			std::ifstream file(FILE_NAME(filename), std::ios::in);
//			//NOMAD_ASSERT(file.is_open(), "failed to open map end screen %s!", filename.c_str());
//			std::string line;
//			while (eastl::getline(file, line)) { ptr->exit_screen.push_back(line); }
//			file.close();
//		}
	}
}

void BFF::BFF_LoadLevels(json& data)
{
	nomadint_t numlvls = data["levels"]["numlevels"];
	levels.reserve(numlvls);
	for (nomadint_t i = 0; i < numlvls; ++i) {
		levels.emplace_back(new Level());
		std::shared_ptr<Level>& lvl = levels.back();
		std::string node_name = "lvl_"+std::to_string(i);
		const std::string mapfile = data["levels"][node_name]["mapfile"];
		lvl->lvl_id = BFF_GetString(data["levels"][node_name]["id"]);
		lvl->lvl_name = BFF_GetString(data["levels"][node_name]["name"]);
		{
			for (auto& s : data["levels"][node_name]["mobspawners"]) {
				lvl->mobspawners.push_back(s);
			}
		}
		{
			for (auto& s : data["levels"][node_name]["itemspawners"]) {
				lvl->itemspawners.push_back(s);
			}
		}
		{
			for (auto& s : data["levels"][node_name]["wpnspawners"]) {
				lvl->wpnspawners.push_back(s);
			}
		}
		{
			const std::string filename = data["levels"][node_name]["introfile"];
			std::ifstream file(FILE_NAME(filename), std::ios::in);
			NOMAD_ASSERT(file.is_open(), "failed to open level intro file %s!", filename.c_str());
			std::string line;
			while (std::getline(file, line)) { lvl->intro_screen.push_back(line); }
			file.close();
		}
		{
			const std::string filename = data["levels"][node_name]["endfile"];
			std::ifstream file(FILE_NAME(filename), std::ios::in);
			NOMAD_ASSERT(file.is_open(), "failed to open level end file %s!", filename.c_str());
			std::string line;
			while (std::getline(file, line)) { lvl->end_screen.push_back(line); }
			file.close();
		}
		std::ifstream file(FILE_NAME(mapfile), std::ios::in);
		NOMAD_ASSERT(file.is_open(), "failed to open level map file %s!", mapfile.c_str());
		std::string line;
		std::vector<std::string> buffer;
		nomadshort_t y{}, x{};
		y=x=0;
		while (std::getline(file, line)) {
			if (y > 119) {
				break;
			}
			for (x = 0; x < line.size(); ++x) {
				lvl->lvl_map[y][x] = line[x];
			}
			++y;
		}
		file.close();
	}
}

void BFF::BFF_LinkSpawners(json& data)
{
	nomadint_t numspawners = data["data"]["spawners"]["numspawners"];
	spawners.reserve(numspawners);
	for (nomadint_t i = 0; i < numspawners; ++i) {
		spawners.emplace_back(new Spawner());
		std::shared_ptr<Spawner>& spawn = spawners.back();
		std::string node_name = "spawner_"+std::to_string(i);
		std::string et_type = data["data"]["spawners"][node_name]["entity"];
		if (et_type == "ET_MOB") {
			spawn->et_type = ET_MOB;
		} else if (et_type == "ET_PLAYR") {
			spawn->et_type = ET_PLAYR;
		}
		spawn->et_name = BFF_GetString(data["data"]["spawners"][node_name]["name"]);
		std::string marker = data["data"]["spawners"][node_name]["marker"];
		if (marker.size() > 1) {
			N_Error("BFF_LinkSpawners: cannot have a spawn marker greater than 1 character");
		}
		spawn->marker = marker[0];
		std::string replace = data["data"]["spawners"][node_name]["replacement"];
		if (replace.size() > 1) {
			N_Error("BFF_LinkSpawners: cannot have a spawn replacement greater than 1 character");
		}
		spawn->replacement = replace[0];
	}
}