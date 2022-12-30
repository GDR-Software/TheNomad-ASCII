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
private:
	std::string name; // the tribe's name
 	nomadenum_t sector_id; // really just the biome
	std::vector<nomad_t*> warriors; // the assigned warriors of the tribe
	std::vector<nomad_t*> traders; // the assigned traders/merchants of the tribe
	std::vector<nomad_t*> bots; // the civilains of said tribe
	nomad_t* leader; // called "Tarsin" in the common nomadic tongue, the tribe's commander/leader
	nomaduint_t size; // number of members in the tribe
	nomadenum_t alignment; // the average moral of the tribe
public:
	Tribe(){}
	~Tribe()
	{
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

void Tribe::T_SetRoute()
{
	
}