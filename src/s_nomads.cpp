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
	NPC* npc;
	Mob* mob; // the ptr to the mob class it belongs to, nullptr if it's not a warrior/leader
	std::string name;
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
public:
	Tribe(){}
	~Tribe()
	{
		for (auto* const i : bots) {
			Z_Free(i->npc);
			Z_Free(i);
		}
		for (auto* const i : warriors) {
			Z_Free(i->mob);
			Z_Free(i);
		}
		for (auto* const i : traders) {
			Z_Free(i->npc);
			Z_Free(i);
		}
		Z_Free(leader->npc);
		Z_Free(leader->mob);
		Z_Free(leader);
		Z_Free(this);
	}
	void T_SetRoute();
};

static std::vector<Tribe*> tribes; // a list of all the tribes active in the game at a time

void NomadAssigner(Game* const gptr)
{
	game = gptr;
}

// T_Interact(): handles most player and nomad-tribe interactions
void T_Interact()
{
	
}

void B_GenNomadTribe()
{
	nomadenum_t numwarriors = (P_Random() & 4)+1;
	nomadenum_t numbots = (P_Random() & 10)+3;
	nomadenum_t i;
	Tribe* tribe = (Tribe *)Z_Malloc(sizeof(Tribe), TAG_STATIC, &tribe);
	std::vector<nomad_t*>& warriors = tribe->warriors;
	std::vector<nomad_t*>& bots = tribe->bots;
	tribe->leader = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &tribe->leader);
	nomad_t& leader = *tribe->leader;
	// nomadic leaders are the only ones in a tribe who can be both diplomats as well as soldiers
	leader.npc = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &leader.npc);
	leader.mob = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &leader.mob);

	warriors.reserve(numwarriors);
	bots.reserve(numbots);

	for (i = 0; i < numbots; ++i) {
		bots.emplace_back();
		bots.back() = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &bots.back());
		nomad_t* bot = bots.back();
		bot->mob = nullptr;
		bot->npc = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &bot->npc);
	//	bot->npc->c_npc = npcinfo[BOT_NOMAD_CIVILIAN]; // TODO: finish this up
	}
	for (i = 0; i < numwarriors; ++i) {
		warriors.emplace_back();
		warriors.back() = (nomad_t *)Z_Malloc(sizeof(nomad_t), TAG_STATIC, &warriors.back());
		nomad_t* n = warriors.back();
		n->mob = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &n->mob);
		n->npc = nullptr;
		n->mob->c_mob = mobinfo[MT_NOMAD_WARRIOR];
	}
	tribes.push_back(tribe);

}

void Tribe::T_SetRoute()
{
	
}