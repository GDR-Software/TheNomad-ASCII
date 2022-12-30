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

static Game* game;

static void M_GenMercSquad();
static void M_GenZurgutLegion();

static void M_GenGroup()
{
	coord_t origin;
	srand(time(NULL));
	origin.y = rand() % 400+90;
	origin.x = rand() % 400+90;
	Mob* leader;
	mobj_t mob = mobinfo[rand() % (NUMMOBS - 1)];
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
	nomadenum_t count = P_Random() & 21;
	Mob* minions[count];
	for (nomadenum_t i = 0; i < count; ++i) {
		mob = mobinfo[rand() % (NUMMOBS - 1)];
		game->m_Active.emplace_back();
		game->m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &game->m_Active.back());
		minions[i] = game->m_Active.back();
		Mob* const m = minions[i];
		if ((rand() % 99) >= 49) {
			m->mpos.y = origin.y + (rand() % 10);
		} else {
			m->mpos.y = origin.y - (rand() % 10);
		}
		if ((rand() % 99) >= 49) { 
			m->mpos.x = origin.x + (rand() % 10);
		} else {
			m->mpos.x = origin.x - (rand() % 10);
		}
		m->is_boss = false;
		m->c_mob = mob;
		m->mstate = stateinfo[S_MOB_WANDER];
		m->mticker = m->mstate.numticks;
		m->stepcounter &= 0;
		m->mdir = P_Random() & 3;
	}
}

void Game::M_GenMobs(void)
{
	game = this;
	Assigner(this);
	m_Active.reserve(MAX_MOBS_ACTIVE);
	for (nomaduint_t i = 0; i < MAX_MOBS_ACTIVE; ++i) {
		if (m_Active.size() >= MAX_MOBS_ACTIVE) {
			break;
		}
		else {
			M_GenGroup();
		}
	} /*
	while (m_Active.size() > MAX_MOBS_ACTIVE) {	
		Z_Free(m_Active.back());
		m_Active.pop_back();
	}; */
}

Mob::Mob()
{
}

Mob::~Mob()
{
}