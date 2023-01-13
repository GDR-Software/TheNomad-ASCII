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
//  src/g_items.h
//----------------------------------------------------------
#ifndef _G_ITEMS_
#define _G_ITEMS_

#include "n_shared.h"
#include "g_map.h"

enum
{
	// shotgun, semi-auto pump-action
	W_SHOTTY_SFP,
	// shotgun, semi-auto auto-loading
	W_SHOTTY_SFA,
	// shotgun, full-auto beast
	W_SHOTTY_FA,
	// shotgun, 4-round burst, pump-action
	W_SHOTTY_BF4P,
	// shotgun, 3-round burst, auto-loading
	W_SHOTTY_BF3A,
	// shotgun, Asturion 8-shot
	W_SHOTTY_A8SHOT,
	// shotgun, Asturion double-barreled
	W_SHOTTY_ADB,

	W_PISTOL_ION,
	// All Use Pistol, overpowered as hell
	W_PISTOL_AUP,
	W_PISTOL_PLASMA,
	// browning pistol, old-school
	W_PISTOL_BOS,

	// the dude actually forged this himself, if you're wondering why there's still
	// the AR-15 platform in existence in the 91st millennium. Some other models still exist,
	// but they're rare, if the player picks this up, well, they've got one hell of a
	// rifle
	W_RIFLE_M23C5,
	// credits to Ben Pavlovic (milotus) for this one
	W_RIFLE_AK77,
	// Ranged Assault Gun model 15, the most notorius of the 3 models, horrible stats
	W_RIFLE_RAG15,
	// Ranged Assault Gun model 14, not as notorius as the 15
	W_RIFLE_RAG14,
	// fusion rifle, only use iron with this one
	W_RIFLE_FUSION,

	// desert rifle, lever-action 4 shot
	W_DMR_DR,
	// the slapper, y'know
	W_DMR_HOSIG,
	// Ranged Assault Gun model 13, the most notable and respected of the 3, the 13 hits the hardest, but the slowest
	W_DMR_RAG13,
	// the fusion cannon, fucking nuke in a sniper rifle
	W_DMR_FUSION,

	W_SMG_PLASMA,
	W_SMG_MP69K,

	W_ROCKET_LAUNCHER,
	W_ROCKET_MGL,
	W_ROCKET_ION,
	// its. a. cannon. if you say otherwise, then you haven't seen it in action
	W_ROCKET_FUSION,

	NUMWEAPONS,
	
	W_NULL
};

enum
{
	I_HEALTH_SMALL,
	I_HEALTH_NORM,
	I_HEALTH_LARGE,
	I_BANDAGE,

	I_ARMOR_STREET,
	I_ARMOR_MILITARY,
	I_ARMOR_MERC,
	I_FLAK,

	I_SHELL,
	I_BULLET,
	I_ROCKET,

	I_COIN,
	I_SHOTGUN_SFP,
	I_SHOTGUN_SFA,
	I_SHOTGUN_FA,
	I_SHOTGUN_BF4P,
	I_SHOTGUN_BF3A,
	I_PISTOL_AUP,
	I_PISTOL_PLASMA,
	I_RIFLE_M23C5,

	NUMITEMS,

	I_NULL
};

enum
{
	M_METAL,
	M_STRING,
	M_GASOLINE,
	M_CLOTH,
	
	NUMMATERIALS,
	
	M_NULL
};

typedef struct
{
	nomaduint_t id;
	nomaduint_t dmg;
	nomaduint_t range;
	nomadenum_t rng;
	nomadenum_t spread;
	nomadenum_t magsize;
	nomadenum_t tbs; // time between shots (in ticks)
	// reloading time, emptied (in ticks)
	nomadenum_t tfrf;
	// reloading time, with still unfired bullets
	nomadenum_t tfrs;

	// only ever used for shotguns
	nomadenum_t numpellets;
} weapon_t;

constexpr uint8_t MAXMATERIALS = 15;

typedef struct
{
	const char* name;
	nomaduint_t item_id;
	nomaduint_t item_weight;
	nomaduint_t item_cost;
	std::vector<nomadenum_t> mtl_ls;
} item_t;

extern const weapon_t wpninfo[NUMWEAPONS];
extern const item_t iteminfo[NUMITEMS];

class Item
{
public: // basic item statistics
	item_t c_item;
public:
	Item() = default;
	~Item();
};

class Weapon : public Item
{
public:
	weapon_t c_wpn;
public:
	Weapon() = default;
	~Weapon();
};

std::vector<collider_t>& P_GetHitEntities(Weapon* const wpn);
void P_ShootShotty(Weapon* const wpn);
void P_ShootSingle(Weapon* const wpn);
void P_ShootAuto(Weapon* const wpn);


#endif
