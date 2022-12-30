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
#include "g_rng.h"
#include "p_npc.h"
#include "g_obj.h"

Game* game;

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
__CFUNC__ void B_SpawnShopBots(void)
{
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
/*__CFUNC__ void B_CivilianThink(NPC* const npc)
{
	// move at random
	if (P_Random() > 75) {
		coord_t pos = game->E_GetDir(npc->ndir);
		if (E_CloseCollider(npc->ndir, npc->pos, game)) {
		//	npc->pos.y += pos.y;
		//	npc->pos.x += pos.x;
		}
		else {
			npc->ndir = P_Random() & 3;
		}
	}
	// TODO: you know what to do
} */

void Game::I_InitNPCs(void)
{
	game = this;
	b_Active.reserve(npcinfo.size());
	B_SpawnShopBots();
}

NPC::~NPC()
{
	Z_Free(this);
}

static nomadbool_t B_IsScared(NPC* const npc)
{
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
	if (!B_IsScared(npc)) {
		return;
	}

	// extra-scared flag
	nomadbool_t panic =
	(P_Random()&100)>75||npc->c_npc.btype==BOT_CIVILIAN ? true : false;
	
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
	npc->~NPC();
}