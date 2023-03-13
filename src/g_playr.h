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
// DESCRIPTION: src/g_playr.h
//  player class
//----------------------------------------------------------
#ifndef _G_PLAYR_
#define _G_PLAYR_

#pragma once

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
//	std::string name;
	sprite_t sprite;
	nomadint_t health;
	nomadenum_t armor;
	nomadenum_t pdir = D_NORTH;
	nomadushort_t lvl = 0;
	nomadulong_t xp = 0;
	nomadint_t coin = 0;
	coord_t pos;
	nomadenum_t sector_id;
	Weapon P_wpns[MAX_PLAYR_WPNS];
	Weapon* c_wpn = (Weapon *)NULL;
	nomadbool_t wpn_select = false;
	nomadenum_t wpn_slot_current = 1;
	std::shared_ptr<Level> c_lvl;
	std::array<item_t, MAX_PLAYR_ITEMS> inv;
	std::array<nomadint_t, 4> body_health;
	entitystate_t pstate;
	nomadenum_t pmode;
	nomadenum_t lastmoved;
	nomadlong_t pticker = 0;
	char vmatrix[MAX_VERT_FOV*2][MAX_HORZ_FOV*2];
	std::array<nomadint_t, NUMAMMOTYPES> ammunition;
public:
	Playr() = default;
	~Playr() = default;
	Playr(const Playr &) = default;
	Playr(Playr &&) = default;
	Playr& operator=(const Playr& playr) {
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
