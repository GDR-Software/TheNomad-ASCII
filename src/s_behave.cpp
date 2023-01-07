//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/s_behave.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "s_mission.h"
#include "g_rng.h"
#include "p_npc.h"
#include "g_obj.h"

static Game* game;

__CFUNC__ void B_SpawnShopBots(void)
{
#ifdef _NOMAD_DEBUG
	assert(game);
#endif
	// hardcoded until the BFFs roll around
	std::vector<NPC*>& b_Active = game->b_Active;
	NPC* npc;
	
	b_Active.emplace_back(); // creating the guns 'n' grenades bartender
	b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	npc = b_Active.back();
	npc->pos = botpos[0];
	npc->c_npc = npcinfo[0];
	npc->c_npc.btype = BOT_BARTENDER;
	npc->ndir = D_WEST;

	b_Active.emplace_back(); // creating the guns 'n' grenades mercenary master
	b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	npc = b_Active.back();
	npc->pos = botpos[1];
	npc->c_npc = npcinfo[1];
	npc->c_npc.btype = BOT_MERCMASTER;
	npc->ndir = D_NORTH;

	b_Active.emplace_back(); // creating the guns 'n' grenades weapons smith
	b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	npc = b_Active.back();
	npc->pos = botpos[2];
	npc->c_npc = npcinfo[2];
	npc->c_npc.btype = BOT_WEAPONSMITH;
	npc->ndir = D_EAST;
}
/*
__CFUNC__ void B_GenerateCivilian(NPC* const npc)
{
	npc->pos.y = (P_Random() & 204) + 112;
	npc->pos.x = (P_Random() & 107) + 208;
	switch (game->c_map[npc->pos.y][npc->pos.x]) {
	case ' ':
	case '_':
	case '.':
		break;
	default:
		Z_Free(npc);
		return;
		break;
	};
	npc->c_npc.sprite = 'c';
	npc->c_npc.health = 100;
	npc->c_npc.armor = 12;
	npc->c_npc.btype = BOT_CIVILIAN;
}
__CFUNC__ void B_SpawnCivilianBots(void)
{
	std::vector<NPC*>& b_Active = game->b_Active;
	for (nomaduint_t i = 0; i < MAX_NPC_ACTIVE; ++i) {
		b_Active.emplace_back();
		b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		NPC* const npc = b_Active.back();
		npc->nstate = stateinfo[S_BOT_WANDER];
		npc->nticker = npc->nstate.numticks;
		B_GenerateCivilian(npc);
	}
} */

void Game::I_InitNPCs(void)
{
#ifdef _NOMAD_DEBUG
	assert(!game);
#endif
	game = this;
	b_Active.reserve(npcinfo.size()+(INITIAL_NPC_ACTIVE*2));
#ifdef _NOMAD_DEBUG
	LOG("reserving %li NPC* for b_Active", npcinfo.size()+(INITIAL_NPC_ACTIVE*2));
#endif
	NomadAssigner(this);
	MissionAssigner(this);
	B_SpawnShopBots();
	B_GenNomadTribe();
}

NPC::~NPC()
{
	Z_Free(this);
}

static nomadbool_t B_IsScared(NPC* const npc)
{
#ifdef _NOMAD_DEBUG
	assert(npc);
#endif
	if (npc->c_npc.btype == BOT_CIVILIAN) {
		return true; // auto-true if its a civilian
	}
	nomaduint_t rand = (P_Random() & 49) + 1;
	if (rand > 15 && npc->c_npc.btype != BOT_GUARD) {
		return true;
	}
	else if (rand > 30 && npc->c_npc.btype == BOT_GUARD) {
		return true;
	}
	else {
		return false;
	}
}

void B_FleeArea(NPC* const npc)
{
#ifdef _NOMAD_DEBUG
	assert(npc);
#endif
	if (!B_IsScared(npc)) {
		return;
	}

	// extra-scared flag
	nomadbool_t panic = (P_Random() & 100) > 75 || npc->c_npc.btype == BOT_CIVILIAN;
	
	if (panic) {
		// run in wild circles
		npc->pos.y += rand() % 444;
	}
	/*
	coord_t pos = game->E_GetDir(npc->ndir);
	npc->pos.y += pos.y;
	npc->pos.x += pos.x; */
}

void B_KillBot(NPC* const npc)
{
#ifdef _NOMAD_DEBUG
	assert(npc);
#endif
	npc->~NPC();
}

static void B_MercMasterInteract(NPC* const npc);

inline void B_MercMasterThink(NPC* const npc)
{
#ifdef _NOMAD_DEBUG
	assert(npc);
#endif
	coord_t mercpos = botpos[0];
	if ((game->playr->pos.y == (mercpos.y - 1) && game->playr->pos.x == mercpos.x)
	|| (game->playr->pos.y == mercpos.y && game->playr->pos.x == (mercpos.x + 1))) {
		game->playr->pmode = P_MODE_MERCMASTER;
		B_MercMasterInteract(npc);
	}
}
//inline void B_BartenderLoop(NPC* const npc);
//inline void B_WeaponSmithLoop(NPC* const npc);
//inline void B_BlackSmithLoop(NPC* const npc);
//inline void B_CivilianLoop(NPC* const npc);

static void B_MercMasterInteract(NPC* const npc)
{
#ifdef _NOMAD_DEBUG
	assert(npc && game->playr);
#endif
	Playr* const playr = game->playr;
	if (playr->pmode != P_MODE_MERCMASTER) {
#ifdef _NOMAD_DEBUG
		LOG("called merc master interaction without player mode being merc master interaction"); // whoopsy
#endif
		return;
	}
	
	Mission* mission = G_GenMission();
}