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
//  mob spawner code, balancer, no mob thinker/action functions
//  here, that's s_mthink
//----------------------------------------------------------
#include "g_mob.h"
#include "g_game.h"
#include "scf.h"
#include "g_rng.h"
#include "g_obj.h"

typedef struct mobgroup_s
{
	Mob** m_Active;
	Mob* leader;
	area_t area;
	nomadenum_t sector;
} mobgroup_t;

static Game* game;
static Mob* mob; // the current mob being iterated over

void MobAssigner(Game* const gptr)
{
	game = gptr;
}

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
	leader = M_SpawnMob();
	if (!leader) return; // max mob count has been met
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
		minions[i] = M_SpawnMob();
		if (!minions[i]) { // out of space
			break;
		}
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

static void M_GenMob(Mob* const mob)
{
	mob->c_mob = mobinfo[rand() % NUMMOBS];
	mob->mpos.y = (rand() % 480)+20;
	mob->mpos.x = (rand() % 480)+20;
	mob->mstate = stateinfo[S_MOB_SPAWN+mob->c_mob.stateoffset];
	mob->mdir = P_Random() & 3;
	mob->is_boss = false;
	mob->stepcounter = P_Random() & 10;
}

void Game::M_GenMobs(void)
{
	game = this;
	MobAssigner(this);
	NomadAssigner(this);
	m_Active.reserve(MAX_MOBS_ACTIVE);
	for (nomaduint_t i = 0; i < MAX_MOBS_ACTIVE; ++i) {
		m_Active.emplace_back();
		m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		m_Active.back()->alive = false;
		M_GenMob(m_Active.back());
	}
}

Mob* M_SpawnMob(void)
{
	nomaduint_t index = 0;
	if ((index = G_GetFreeMob(game)) == MAX_MOBS_ACTIVE) {
		return nullptr;
	}
	game->m_Active[index]->alive = true;
	return game->m_Active[index];
}

//
// M_KillMob(): deallocates/kills the current mob being iterated over
//
void M_KillMob(Mob* const actor)
{
#ifdef _NOMAD_DEBUG
	assert(actor);
#endif
	// this memory can now be overwritten
	actor->alive = false;
}
const char* MobTypeToStr(nomaduint_t mtype)
{
	switch (mtype) {
	case MT_DRUID: return VAR_TO_STR(MT_DRUID);
	case MT_SANDWURM: return VAR_TO_STR(MT_SANDWURM);
	case MT_RAVAGER: return VAR_TO_STR(MT_RAVAGER);
	case MT_PISTOL: return VAR_TO_STR(MT_PISTOL);
	case MT_SHOTTY: return VAR_TO_STR(MT_SHOTTY);
	case MT_GUNNER: return VAR_TO_STR(MT_GUNNER);
	case MT_GRUNT: return VAR_TO_STR(MT_GRUNT);
	case MT_HULK: return VAR_TO_STR(MT_HULK);
	case MT_MERC: return VAR_TO_STR(MT_MERC);
	case MT_MERC_LEADER: return VAR_TO_STR(MT_MERC_LEADER);
	case MT_NOMAD_LEADER: return VAR_TO_STR(MT_NOMAD_LEADER);
	case MT_NOMAD_WARRIOR: return VAR_TO_STR(MT_NOMAD_WARRIOR);
	};
	if (!false)
		N_Error("Unknown/Invalid Mob Type %iu!", mtype);
	return nullptr;
}
