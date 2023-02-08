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

#define MAX_PLAYR_ITEMS 1000
#define MAX_PLAYR_WPNS PLAYR_MAX_WPNS
#define DASH_SPEED 5
#define RDASH_SPEED 6

class Mission;

enum : nomadenum_t
{
	P_MODE_ROAMING,
	P_MODE_MERCMASTER,
	P_MODE_SHOPPING,
	P_MODE_MISSION,
	P_MODE_SITTING
};

class Playr
{
public:
	std::string name;
	sprite_t sprite;
	std::atomic<nomadint_t> health;
	std::atomic<nomadushort_t> armor;
	nomadenum_t pdir = D_NORTH;
	nomadushort_t lvl = 0;
	nomadint_t coin = 0;
	coord_t pos;
	nomadenum_t sector_id;
	Weapon P_wpns[MAX_PLAYR_WPNS];
	Weapon* c_wpn = nullptr;
	nomadbool_t wpn_select = false;
	nomadenum_t wpn_slot_current = 1;
	Item inv[MAX_PLAYR_ITEMS];
	std::atomic<nomadint_t> body_health[4];
	nomaduint_t pstate;
	std::atomic<nomadenum_t> pmode;
	nomadenum_t lastmoved;
	nomadlong_t pticker = 0;
	char vmatrix[MAX_VERT_FOV*2][MAX_HORZ_FOV*2];
	Mission* c_mission = nullptr;
public:
	Playr() = default;
	~Playr() = delete;
	Playr& operator=(const Playr& playr) {
		memcpy(&(*this), &playr, sizeof(Playr));
		return *this;
	}
	const Playr& operator=(const Playr& playr) {
		memcpy(&(*this), &playr, sizeof(Playr));
		return *this;
	}
	
	void P_Init();
	void P_GetMode();
	void P_RunTicker(nomadint_t finput);
};

void P_Interact();
void P_PauseMenu();
void P_UseWeapon();
void P_UseMelee();
void P_QuickShot();
void P_UseLeftArm();
void P_UseRightArm();
void P_ShowWeapons();
void P_MoveN();
void P_MoveW();
void P_MoveS();
void P_MoveE();
void P_DashN();
void P_DashW();
void P_DashS();
void P_DashE();
void P_SlideN();
void P_SlideW();
void P_SlideS();
void P_SlideE();
void P_ChangeDirL();
void P_ChangeDirR();
void P_ChangeWeapon1();
void P_ChangeWeapon2();
void P_ChangeWeapon3();
void P_ChangeWeapon4();
void P_ChangeWeapon5();
void P_ChangeWeapon6();
void P_ChangeWeapon7();
void P_ChangeWeapon8();
void P_ChangeWeapon9();
void P_ChangeWeapon10();
void CommandConsole();

void P_ShootShotty(Weapon* const wpn);
void P_ShootSingle(Weapon* const wpn);
void P_ShootBurst3(Weapon* const wpn);
void P_ShootBurst4(Weapon* const wpn);

void PlayrAssigner(Game* const gptr);
void P_KillPlayr();

#endif
