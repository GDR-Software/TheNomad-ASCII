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
//  src/s_nomads.cpp
//----------------------------------------------------------
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

static Game* game;

class Tribe;

#define MAX_TRIBES_ACTIVE 15
static Tribe** tribes; // a list of all the tribes active in the game at a time
static nomadenum_t numtribes = 0;

typedef struct nomad_s
{
	npc_t npc;
	mobj_t mob;
	std::string name;
	coord_t pos;
	nomadint_t health;
	nomadushort_t armor;
	Tribe* tribe; // ptr to the tribe it belongs to
} nomad_t;

class Tribe
{
public:
	std::string name; // the tribe's name
 	nomadenum_t sector_id; // really just the biome
	nomadenum_t numwarriors;
	nomadenum_t numtraders;
	nomadenum_t numbots;
	nomad_t** warriors; // the assigned warriors of the tribe
	nomad_t** traders; // the assigned traders/merchants of the tribe
	nomad_t** bots; // the civilains of said tribe
	nomad_t* leader; // called "Tarsin" in the common nomadic tongue, the tribe's commander/leader
	nomaduint_t size; // number of members in the tribe
	nomadenum_t alignment; // the average alignment of the tribe
	nomadenum_t direction; // the direction the tribe is facing in, used for setting a route and moving the entirety of the tribe as one
	area_t area; // to keep collision faster and easier
	nomadenum_t index = 0;
public:
	Tribe(){}
	~Tribe(){}
	void T_SetRoute();
	void T_FollowRoute();
};

void T_KillTribe(Tribe* const tribe)
{
	nomaduint_t i{};
	for (i = 0; i < tribe->numwarriors; ++i) {
		Z_Free(tribe->warriors[i]);
	}
	for (i = 0; i < tribe->numtraders; ++i) {
		Z_Free(tribe->traders[i]);
	}
	for (i = 0; i < tribe->numbots; ++i) {
		Z_Free(tribe->bots[i]);
	}
	Z_Free(tribe->leader);
	Z_Free(tribe);
	--numtribes;
	tribes = (Tribe **)Z_Realloc(&tribes, ARRAY_SIZE(tribes) * numtribes, TAG_STATIC);
}

void NomadAssigner(Game* const gptr)
{
#ifdef _NOMAD_DEBUG
	assert(gptr);
#endif
	game = gptr;
}

// T_Interact(): handles most player and nomad-tribe interactions
void T_Interact()
{
	
}

void B_GenNomadTribe()
{
//	if (ARRAY_SIZE(tribes) == MAX_TRIBES_ACTIVE) return;
	nomadushort_t i;
	std::vector<Mob*>& m_Active = game->m_Active;
//	std::vector<NPC*>& b_Active = game->b_Active;
	tribes = (Tribe **)Z_Realloc(&tribes, ARRAY_SIZE(tribes) + sizeof(Tribe*), TAG_STATIC);
	++numtribes;
	tribes[numtribes] = (Tribe *)Z_Malloc(sizeof(Tribe), TAG_STATIC, &tribes[numtribes]);
	Tribe* const tribe = tribes[numtribes];
	tribe->numwarriors = P_Random() & 2;
	tribe->numbots = (P_Random() & 4)+3;
	area_t& area = tribe->area;
	coord_t& tl = area[0];
	coord_t& tr = area[1];
	coord_t& bl = area[2];
	coord_t& br = area[3];
	tl.y = (rand() % 300)+190;
	tl.x = (rand() % 300)+190;
	tr.y = tl.y;
	tr.x = tl.x + ((rand() % 10)+15);
	bl.y = tl.y + ((rand() % 10)+15);
	bl.x = tl.x + ((rand() % 10)+15);
	br.y = bl.y;
	br.x = tr.x;
	nomad_t** warriors = tribe->warriors;
//	std::vector<nomad_t*>& bots = tribe->bots;
	tribe->leader = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &tribe->leader);
	nomad_t* leader = tribe->leader;
	leader->pos.y = tl.y + ((rand() % 13));
	leader->pos.x = tl.x + ((rand() % 13));
//	b_Active.emplace_back();
//	b_Active.back() = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	// nomadic leaders are the only ones in a tribe who can be both diplomats as well as soldiers
//	leader->npc = npcinfo[BOT_NOMAD_CIVILIAN]; // need data in this array or SEGGY
	leader->mob = mobinfo[MT_NOMAD_LEADER];
	Mob* mob = M_SpawnMob();
	mob->c_mob = leader->mob;
//	b_Active.back()->c_npc = leader->npc;

	warriors = (nomad_t **)Z_Malloc(sizeof(nomad_t *) * tribe->numwarriors, TAG_STATIC, &warriors);
//	bots.reserve(numbots);
/*
	for (i = 0; i < numbots; ++i) {
		bots.emplace_back();
		b_Active.emplace_back();
		b_Active.back() = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		bots.back() = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &bots.back());
		nomad_t* bot = bots.back();
		bot->npc = npcinfo[BOT_NOMAD_CIVILIAN];
		bot->pos.y = tl.y + ((rand() % 13));
		bot->pos.x = tl.x + ((rand() % 13));
		b_Active.back()->c_npc = bot->npc;
	}*/
	for (i = 0; i < tribe->numwarriors; ++i) {
		Mob* mob = M_SpawnMob();
		if (!mob) {
			break;
		}
		warriors[i] = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &warriors[i]);
		nomad_t* n = warriors[i];
		n->mob = mobinfo[MT_NOMAD_WARRIOR];
		n->pos.y = tl.y + ((rand() % 13));
		n->pos.x = tl.x + ((rand() % 13));
		mob->c_mob = n->mob;
	}
}

void Tribe::T_SetRoute()
{
	direction = P_Random() & 3;
	for (nomadshort_t i = 0; i < numbots; ++i) {
		nomad_t* b = bots[i];
		
	}
}

void Tribe::T_FollowRoute()
{

}