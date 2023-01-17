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
	W_SHOTTY_ADB,     // Asturion Double-Barreled
	W_SHOTTY_QS,      // quick loader, pump-action single-shot
	W_SHOTTY_FAB,     // full-auto-b****, that's its name

	W_SIDE_AUP,       // all-use-pistol
	W_SIDE_BOS,       // browning old-school
	W_SIDE_FUSION,    // heavy-hitting pistol, slow firerate though
	W_SIDE_ION,
	W_SIDE_PLASMA,

	W_HSIDE_A8SHOT,   // Asturion 8-shot
	W_HSIDE_SADB,     // sawed-off Asturion DB

	W_PRIM_M23C5,     // yes, the AR-15 platform still exists in the 91st millennium
	W_PRIM_AK77,      // thanks to Ben Pavlovic (Milotus) for this one
	W_PRIM_RAG15,     // why did they even approve the manufacturing of this horrible thing people call a weapon?
	W_PRIM_RAG14,     // better than the 13, but what isn't?
	W_PRIM_PLASMASMG,

	W_HPRIM_FUSION,   // fusion-cannon
	W_HPRIM_HOSIG,    // heavy optics sniper, forged by Ismalda Gurnovil
	W_HPRIM_DR,       // desert rifle
	W_HPRIM_RAG13,    // ranged assault rifle, heaviest but slowest of the three

	W_LAUNCHER_MGL,   // magnetic grenade launcher

	W_ARM_HB,         // hidden blade, assassin's creed but more brutal and actually pratical
	W_ARM_GRAPPLE,    // grappling hook, nuff said
	W_ARM_HC,         // hand-cannon, slightly less powerful 8-shot but you put it on your forearm, fucks the user up
	W_ARM_SB,         // sound-blaster, nope, not the MS-DOS thing, but instead a hand cannon that make craters with sound waves
	W_ARM_FT,         // people are crazy enough the actually attach a flame-thrower onto their arms in Bellatum Terrae, very effective croud control

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
	// only ever used for shotguns
	nomadenum_t numpellets;
	nomadenum_t tbs; // time between shots (in ticks)
	// reloading time, emptied (in ticks)
	nomadenum_t tfrf;
	// reloading time, with still unfired bullets
	nomadenum_t tfrs;
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
