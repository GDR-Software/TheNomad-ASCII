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
//  src/g_playr.h
//----------------------------------------------------------
#ifndef _G_PLAYR_
#define _G_PLAYR_

#include "n_shared.h"
#include "scf.h"
#include "g_items.h"
#include "g_map.h"

#define MAX_PLAYR_WPNS 11

enum : nomadenum_t
{
	P_MODE_ROAMING,
	P_MODE_MERCMASTER,
	P_MODE_SHOPPING,
	P_MODE_MISSION
};

class Playr {
public:
	std::string name;
	sprite_t sprite;
	nomadint_t health;
	nomadushort_t armor;
	nomadenum_t pdir;
	nomadushort_t lvl;
	nomadint_t coin;
	coord_t pos;
	nomadenum_t sector_id;
	Weapon* P_wpns[MAX_PLAYR_WPNS];
	std::vector<Item*> inv;
	nomadint_t body_health[4];
	nomaduint_t pstate;
	std::atomic<nomadenum_t> pmode;
	nomadulong_t pticker;
	char vmatrix[MAX_VERT_FOV*2][MAX_HORZ_FOV*2];
public:
	Playr() = default;
	~Playr();
	void P_Init();
	// TODO: add in collision checks
	void P_MoveN();
	void P_MoveW();
	void P_MoveS();
	void P_MoveE();

	void P_DashN();
	void P_DashW();
	void P_DashS();
	void P_DashE();

	void P_ChangeDirL();
	void P_ChangeDirR();
};

#endif