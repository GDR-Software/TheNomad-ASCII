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
//  src/g_mob.h
//----------------------------------------------------------
#ifndef _G_MOB_
#define _G_MOB_

#pragma once

#include "n_shared.h"
#include "g_playr.h"
#include "g_obj.h"

#define INITIAL_MOBS_ACTIVE 200
#define MAX_MOBS_ACTIVE 200

// used for locating vars in the stateinfo array
enum : nomaduint_t
{
	MT_HULK,
	MT_RAVAGER,
	MT_GRUNT,

	/* mythical/fantasy-style mobs */
	MT_DRUID,
	MT_SANDWURM,
	
	/* humanoid mobs */
	MT_MERC,
	MT_MERC_LEADER,
	// galakas-soldiers
	MT_SHOTTY,
	MT_PISTOL, // technically the police
	MT_GUNNER,
	// nomads
	MT_NOMAD_LEADER,
	MT_NOMAD_WARRIOR,
	
	NUMMOBS,

	MT_NULL
};

typedef struct mobj_s
{
	const char* name;
	sprite_t sprite;
	nomadshort_t health;
	nomadushort_t armor;
	nomaduint_t mtype;
	entitytype_t etype;
	nomadenum_t rng;
	nomadenum_t chance_to_spawn;
	nomadenum_t snd_tol, snd_area;
	nomadenum_t smell_tol, smell_area;
	nomadenum_t sight_range;
	bool hasmelee;
	bool hashitscan;
	bool hasprojectile;
	nomadushort_t melee_dmg;
	nomadushort_t hitscan_dmg, hitscan_range;
	nomadushort_t projectile_dmg, projectile_range;
	const char* mlore;
	const nomaduint_t* mdrops;
} mobj_t;

typedef struct
{
	nomadint_t health;
	nomaduint_t armor;
	nomaduint_t minion_type;
	mobj_t base_mob;
} bossj_t;

extern const mobj_t mobinfo[NUMMOBS];
extern const bossj_t bossinfo[];

class Mob
{
public:
	mobj_t c_mob;
	nomadbool_t is_boss;
	bossj_t c_boss;
	
	nomadlong_t mticker;
	std::atomic<nomadlong_t> health;
	std::atomic<nomadlong_t> armor;
	entityflag_t mobflags;

	nomadenum_t mdir;
	sprite_t sprite;
	coord_t mpos;
	entitystate_t mstate;
	nomadshort_t stepcounter;
public:
	Mob(){}
	~Mob(){}
	Mob& operator=(const Mob &mob) {
		memcpy(&(*this), &mob, sizeof(Mob));
		return *this;
	}
};

void M_GenMob(Mob* const mob);
void M_CheckMobs();

// s_mthink functions
void M_RunThinker(Mob* const actor);
void M_NullThink(Mob* actor);
void M_SpawnThink(Mob* actor);
void M_WanderThink(Mob* actor);
void M_IdleThink(Mob* actor);
void M_FightThink(Mob* actor);
void M_ChasePlayr(Mob* actor);
void M_DeadThink(Mob* actor);
void M_FleeThink(Mob* actor);

// s_mmisc functions
Mob* M_SpawnMob(void);
const char* MobTypeToStr(nomaduint_t mtype);
void M_KillMob(std::vector<Mob*>::iterator mob);
void M_KillMob(Mob* mob);

//extern const mstate_t mstates[NUMMOBSTATES];

#endif
