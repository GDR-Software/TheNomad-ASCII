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
#include "g_game.h"
#include "p_npc.h"
#include "g_rng.h"

static Game* game;

class Tribe;

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
	std::vector<nomad_t*> warriors; // the assigned warriors of the tribe
	std::vector<nomad_t*> traders; // the assigned traders/merchants of the tribe
	std::vector<nomad_t*> bots; // the civilains of said tribe
	nomad_t* leader; // called "Tarsin" in the common nomadic tongue, the tribe's commander/leader
	nomaduint_t size; // number of members in the tribe
	nomadenum_t alignment; // the average alignment of the tribe
	nomadenum_t direction; // the direction the tribe is facing in, used for setting a route and moving the entirety of the tribe as one
	area_t area; // to keep collision faster and easier
public:
	Tribe(){}
	~Tribe()
	{
		for (auto* const i : bots)
			Z_Free(i);
		for (auto* const i : warriors)
			Z_Free(i);
		for (auto* const i : traders)
			Z_Free(i);
		Z_Free(leader);
		Z_Free(this);
	}
	void T_SetRoute();
	void T_FollowRoute();
};

static std::vector<Tribe*> tribes; // a list of all the tribes active in the game at a time

void NomadAssigner(Game* const gptr)
{
	assert(gptr);
	game = gptr;
}

// T_Interact(): handles most player and nomad-tribe interactions
void T_Interact()
{
	
}

void B_GenNomadTribe()
{
	Z_CheckHeap();
	nomadenum_t numwarriors = P_Random() & 2;
	nomadenum_t numbots = (P_Random() & 4)+3;
	nomadushort_t i;
	std::vector<Mob*>& m_Active = game->m_Active;
	std::vector<NPC*>& b_Active = game->b_Active;
	Tribe* tribe = (Tribe *)Z_Malloc(sizeof(Tribe), TAG_STATIC, &tribe);
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
	std::vector<nomad_t*>& warriors = tribe->warriors;
	std::vector<nomad_t*>& bots = tribe->bots;
	tribe->leader = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &tribe->leader);
	nomad_t* leader = tribe->leader;
	leader->pos.y = tl.y + ((rand() % 13));
	leader->pos.x = tl.x + ((rand() % 13));
	m_Active.emplace_back();
	m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
	b_Active.emplace_back();
	b_Active.back() = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	// nomadic leaders are the only ones in a tribe who can be both diplomats as well as soldiers
	leader->npc = npcinfo[BOT_NOMAD_CIVILIAN];
	leader->mob = mobinfo[MT_NOMAD_LEADER];
	m_Active.back()->c_mob = leader->mob;
	b_Active.back()->c_npc = leader->npc;

	warriors.reserve(numwarriors);
	bots.reserve(numbots);

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
	}
	for (i = 0; i < numwarriors; ++i) {
		warriors.emplace_back();
		m_Active.emplace_back();
		m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		warriors.back() = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &warriors.back());
		nomad_t* n = warriors.back();
		n->mob = mobinfo[MT_NOMAD_WARRIOR];
		n->pos.y = tl.y + ((rand() % 13));
		n->pos.x = tl.x + ((rand() % 13));
		m_Active.back()->c_mob = n->mob;
	}
	tribes.push_back(tribe);
}

void Tribe::T_SetRoute()
{
	direction = P_Random() & 3;
	for (nomadshort_t i = 0; i < bots.size(); ++i) {
		nomad_t* b = bots[i];
		
	}
}

void Tribe::T_FollowRoute()
{

}