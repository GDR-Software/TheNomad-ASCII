#include "g_game.h"
#include "g_rng.h"
#include "p_npc.h"
#include "g_obj.h"

static constexpr coord_t bot_zones[] = {
	{254, 245},
	{256, 246},
	{271, 235}
};
static std::vector<auto> bot_names[] = {
	"KEK"
};

Game* game;

/*
typedef struct
{
	coord_t bpos[4];
	std::vector<NPC*> npcs;
	nomadenum_t btype;
} building_t;

std::vector<building_t*> buildings;
static char c_map[MAP_MAX_Y][MAP_MAX_X];

static inline coord_t G_FindBuilding(nomadshort_t y, nomadshort_t x)
{
	for (;; ++y) {
		for (;; ++x) {
			if (c_map[y][x] == '#') { return {y, x}; }
		}
	}
}

static inline void G_GetBuildingCoords(building_t* building,
	nomadshort_t& y, nomadshort_t& x)
{
	nomadbool_t done = false;
	for (;; ++y) {
		switch (c_map[y][x]) {
		case '#':
		case '_':
			break;
		default:
			building->bpos[2] = {y, x};
			done = true;
			break;
		};
		if (done) { break; }
	}
	done = false;
	for (;; ++x) {
		switch (c_map[y][x]) {
		case '#':
		case '_':
			break;
		default:
			building->bpos[3] = {y, x};
			done = true;
			break;
		};
		if (done) { break; }
	}
	done = false;
	for (;; --y) {
		switch (c_map[y][x]) {
		case '#':
		case '_':
			break;
		default:
			building->bpos[1] = {y, x};
			done = true;
			break;
		};
		if (done) { break; }
	}
}

static void G_GenBuilding(building_t *building)
{
	building->btype = 5;
}

static void G_MapBuildings(void)
{
	nomadshort_t y, x;
	for (y = 0; y < MAP_MAX_Y; ++y) {
		memcpy(&c_map[y][0], &game->c_map[y+80][80], MAP_MAX_X);
	}

	for (y = 0; y < MAP_MAX_Y; ++y) {
		for (x = 0; x < MAP_MAX_X; ++x) {
			building_t building;
			building.bpos[0] = G_FindBuilding(y, x);
			G_GetBuildingCoords(&building, y, x);
		//	if ((disBetweenOBJ(building.bpos[0], building.bpos[2])) < 81
		//	&& (disBetweenOBJ(building.bpos[0], building.bpos[1])) < 81) {
	//			G_GenBuilding(building);
//				buildings.push_back(&building);
		//	}
		//	else { // forbid massive f'ing buildings
		//		break;
		//	}
		}
	}
}
*/

static constexpr coord_t botpos[] = {
	{263, 283},
	{265, 252},
	{255, 256},
	{259, 283},
};

static std::vector<npc_t> npcinfo = {
	{"The Bartender\0",    'b', 90,  49},
	{"Mercenary Master\0", 'M', 200, 50},
	{"Weapons Smith\0",    'w', 100, 43}
};
static void B_SpawnShopBots(void)
{
	// hardcoded until the BFFs roll around
	std::vector<NPC*>& b_Active = game->b_Active;
	NPC* npc;
	
	b_Active.emplace_back(); // creating the guns 'n' grenades bartender
	b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	npc = b_Active.back();
	npc->pos = botpos[0];
	npc->c_npc = npcinfo[0];
	npc->ndir = D_WEST;

	b_Active.emplace_back(); // creating the guns 'n' grenades mercenary master
	b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	npc = b_Active.back();
	npc->pos = botpos[1];
	npc->c_npc = npcinfo[1];
	npc->ndir = D_NORTH;

	b_Active.emplace_back(); // creating the guns 'n' grenades weapons smith
	b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	npc = b_Active.back();
	npc->pos = botpos[2];
	npc->c_npc = npcinfo[2];
	npc->ndir = D_EAST;
}
static void B_SpawnCivilianBots(void)
{
	std::vector<NPC*>& b_Active = game->b_Active;
	for (nomaduint_t i = 0; i < 3; ++i) {
		b_Active.emplace_back();
		b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		NPC* npc = b_Active.back();
		npc->pos = bot_zones[i];
//		npc->c_npc.name = bot_names[rand() % bot_names.size()];
		npc->c_npc.sprite = 'c';
		npc->c_npc.health = 100;
		npc->c_npc.armor = 12;
	}
}

void Game::I_InitNPCs(void)
{
	game = this;
	b_Active.reserve(npcinfo.size()+3);
	B_SpawnShopBots();
	B_SpawnCivilianBots();
}

NPC::~NPC()
{
	Z_Free(this);
}

void B_FleeArea(NPC* const npc)
{
	if (npc->c_npc.sprite != 'c') { return; }

	coord_t pos = game->E_GetDir(npc->ndir);
	npc->pos.y += pos.y;
	npc->pos.x += pos.x;
}