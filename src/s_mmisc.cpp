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
	area_t area;
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
	MobAssigner(this);
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

const char* MobTypeToStr(nomaduint_t mtype)
{
	switch (mtype) {
	case MT_BEGGAR: return VAR_TO_STR(MT_BEGGAR);
	case MT_DRUID: return VAR_TO_STR(MT_DRUID);
	case MT_SANDWURM: return VAR_TO_STR(MT_SANDWURM);
	case MT_RAVAGER: return VAR_TO_STR(MT_RAVAGER);
	case MT_PISTOL: return VAR_TO_STR(MT_PISTOL);
	case MT_SHOTTY: return VAR_TO_STR(MT_SHOTTY);
	case MT_GUNNER: return VAR_TO_STR(MT_GUNNER);
	case MT_GRUNT: return VAR_TO_STR(MT_GRUNT);
	case MT_GUARD: return VAR_TO_STR(MT_GUARD);
	case MT_HULK: return VAR_TO_STR(MT_HULK);
	case MT_MERC: return VAR_TO_STR(MT_MERC);
	case MT_MERC_LEADER: return VAR_TO_STR(MT_MERC_LEADER);
	case MT_NOMAD_LEADER: return VAR_TO_STR(MT_NOMAD_LEADER);
	case MT_NOMAD_WARRIOR: return VAR_TO_STR(MT_NOMAD_WARRIOR);
	case MT_SNIPER: return VAR_TO_STR(MT_SNIPER);
	case MT_SOLDIER: return VAR_TO_STR(MT_SOLDIER);
	case MT_THUG: return VAR_TO_STR(MT_THUG);
	};
	if (!false)
		N_Error("Unknown/Invalid Mob Type %iu!", mtype);
}