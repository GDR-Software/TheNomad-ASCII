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
	state_t stateoffset;
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
extern const bossj_t bossinfo;

constexpr nomadenum_t idle       = 0b00000000;
constexpr nomadenum_t smellplayr = 0b00000001;
constexpr nomadenum_t hearplayr  = 0b00000010;
constexpr nomadenum_t seeplayr   = 0b00000011;

class Mob
{
public:
	mobj_t c_mob;
	nomadbool_t is_boss;
	bossj_t c_boss;
	
	nomadenum_t sector_id;
	nomadlong_t mticker;
	std::atomic<nomadint_t> health;
	std::atomic<nomadint_t> armor;
	nomadenum_t mdir;
	coord_t mpos;
	entitystate_t mstate;
	nomadshort_t stepcounter;
	nomadbool_t alive = false; // if false, then the memory can be overwritten with a new mob, if true, then the memory is safe
public:
	Mob(){}
	~Mob(){}
	Mob& operator=(const Mob &mob) {
		memcpy(&(*this), &mob, sizeof(Mob));
		return *this;
	}
};

// s_mthink functions
void M_ThinkerAssigner(Game* const gptr);
void M_ThinkerCurrent(Mob* const mptr);
void M_SpawnThink();
void M_WanderThink();
void M_IdleThink();
void M_FightThink();
void M_ChasePlayr();
void M_DeadThink();
void M_FleeThink();

void NomadAssigner(Game* const gptr);

// s_mmisc functions
Mob* M_SpawnMob(void);
void MobAssigner(Game* const gptr);
const char* MobTypeToStr(nomaduint_t mtype);
void M_KillMob(void);

//extern const mstate_t mstates[NUMMOBSTATES];

#endif
