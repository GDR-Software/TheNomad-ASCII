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
//  src/s_mmisc.cpp
//----------------------------------------------------------
#include "g_mob.h"
#include "g_game.h"
#include "scf.h"
#include "g_rng.h"
#include "g_obj.h"

typedef struct mobgroup_s
{
	std::vector<Mob*> m_Active;
	Mob* leader;
	area_t area[4];
	nomadenum_t sector;
} mobgroup_t;

static Game* game;

static void M_GenMercSquad();
static void M_GenZurgutLegion();

static void M_GenGroup()
{
	coord_t origin;
	srand(time(NULL));
	origin.y = (rand() % 300)+190;
	origin.x = (rand() % 300)+190;
	Mob* leader;
	mobj_t mob = mobinfo[rand() % (NUMMOBS - 2)];
	game->m_Active.emplace_back();
	game->m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &game->m_Active.back());
	leader = game->m_Active.back();
	leader->is_boss = false;
	leader->c_mob = mob;
	leader->mpos.y = origin.y;
	leader->mpos.x = origin.x;
	leader->mstate = stateinfo[S_MOB_WANDER];
	leader->mticker = leader->mstate.numticks;
	leader->stepcounter &= 0;
	leader->mdir = P_Random() & 3;
	nomadenum_t count = P_Random() & 6;
	Mob* minions[count];
	for (nomadenum_t i = 0; i < count; ++i) {
		mob = mobinfo[rand() % (NUMMOBS - 2)];
		game->m_Active.emplace_back();
		game->m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &game->m_Active.back());
		minions[i] = game->m_Active.back();
		Mob* const m = minions[i];
		if ((rand() % 99) >= 49) {
			m->mpos.y = origin.y + ((rand() % 10)+15);
		} else {
			m->mpos.y = origin.y - ((rand() % 10)+15);
		}
		if ((rand() % 99) >= 49) { 
			m->mpos.x = origin.x + ((rand() % 10)+15);
		} else {
			m->mpos.x = origin.x - ((rand() % 10)+15);
		}
		m->is_boss = false;
		m->c_mob = mob;
		m->mstate = stateinfo[S_MOB_WANDER];
		m->mticker = m->mstate.numticks;
		m->stepcounter = P_Random() & 10;
		m->mdir = P_Random() & 3;
	}
}

void Game::M_GenMobs(void)
{
	game = this;
	Assigner(this);
	NomadAssigner(this);
	
	m_Active.reserve(INITIAL_MOBS_ACTIVE * 2);
	nomadenum_t numgroups = P_Random() & 15;
	for (nomadenum_t i = 0; i < numgroups; ++i) {
		M_GenGroup();
	}
}

void M_KillMob(Mob* const mob)
{
#ifdef _NOMAD_DEBUG
	assert(mob);
#endif
	Z_Free(mob);
}