//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_mob.h
//----------------------------------------------------------
#ifndef _G_MOB_
#define _G_MOB_

#pragma once

#define INITIAL_MOBS_ACTIVE 200
#define MAX_MOBS_ACTIVE 200

// used for locating vars in the stateinfo array
enum : nomaduint_t
{
	MT_HULK,
	MT_RAVAGER,
	MT_GRUNT,

	/* mythical/fantasy-style mobs */
	
	/* humanoid mobs */
	MT_MERC,
	// galakas-soldiers
	MT_SHOTTY,
	MT_PISTOL, // technically the police
	MT_GUNNER,
	
	NUMMOBS,

	MT_NULL
};

// 20
typedef struct mobj_s
{
	const char name[80];
	sprite_t sprite;
	nomadshort_t health;
	nomadushort_t armor;
	nomaduint_t mtype;
	entitytype_t etype;
	nomadenum_t rng;
	nomadenum_t chance_to_spawn;
	nomadenum_t sight_range;
	nomadbool_t hasmelee;
	nomadbool_t hashitscan;
	nomadbool_t hasprojectile;
	nomadshort_t melee_dmg, melee_range;
	nomadshort_t hitscan_dmg, hitscan_range;
	nomadshort_t projectile_dmg, projectile_range;
	const char* mlore;
	const std::vector<nomaduint_t> mdrops;
	inline mobj_s& operator=(const mobj_s& m) {
		memcpy(&(*this), &m, sizeof(mobj_s));
		return *this;
	}
	inline mobj_s() = default;
	inline mobj_s(const mobj_s &) = default;
	inline mobj_s(mobj_s &&) = default;
	inline ~mobj_s() = default;
} mobj_t;

typedef struct
{
	nomadint_t health;
	nomaduint_t armor;
	nomaduint_t minion_type;
	mobj_t base_mob;
} bossj_t;

extern mobj_t mobinfo[NUMMOBS];
extern mobj_t mobbackup[NUMMOBS];
extern const bossj_t bossinfo[];

class Mob
{
public:
	mobj_t c_mob;
	nomadbool_t is_boss;
	bossj_t c_boss;
	
	nomadlong_t mticker;
	nomadint_t health;
	nomadushort_t armor;
	entityflag_t mobflags;

	nomadenum_t mdir;
	sprite_t sprite;
	coord_t mpos;
	entitystate_t mstate;
	nomadshort_t stepcounter;
public:
	Mob() = default;
	~Mob() = default;
	Mob& operator=(const Mob &mob) {
		memcpy(&(*this), &mob, sizeof(Mob));
		return *this;
	}
};

nomadbool_t M_FindMobAt(coord_t pos);
Mob* M_MobAt(coord_t pos);
void M_GenMob(Mob* const mob);
void M_CheckMobs();

// s_mthink functions
void M_RunThinker(linked_list<Mob*>::iterator it);
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
void M_KillMob(linked_list<Mob*>::iterator mob);
void M_KillMob(Mob* mob);

//extern const mstate_t mstates[NUMMOBSTATES];

#endif
