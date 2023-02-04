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
//  src/p_playr.cpp
//----------------------------------------------------------
#include "g_playr.h"
#include "g_obj.h"
#include "g_game.h"

static Playr* playr;
static Game* game;

// pauses all the other threads
static void CommandConsole(Game* const game);

// random interaction code/functions
static void P_ChairInteract(nomadint_t input)
{
	Hud_Printf("System", "There is a chair there, would you like to sit down? [y/n]");
	nomadshort_t in = wgetch(game->screen);
	if (in != 'y') {
		Hud_Printf("System", "Well, that's a shame, see you later");
		return;
	}
	Hud_Printf("System", "You are now sitting");
	playr->pmode = P_MODE_SITTING;
	switch (input) {
	case KEY_w:
		--playr->pos.y;
		break;
	case KEY_a:
		--playr->pos.x;
		break;
	case KEY_s:
		++playr->pos.y;
		break;
	case KEY_d:
		++playr->pos.x;
		break;
	default:
		N_Error("Player-To-Chair Interaction Called, But Movement Vector Is Invalid");
		break;
	};
}
static void P_DoorInteract(nomadint_t input)
{
	Hud_Printf("System", "There's a door in your way, would you like to open it? [y/n]");
	nomadshort_t in = wgetch(game->screen);
	if (in != 'y') {
		Hud_Printf("System", "Ok then, how're you going to get into that building?");
		return;
	}
	// TODO: add in locked doors
	Hud_Printf("System", "You successfully opened the door, bravo");
	switch (input) {
	case KEY_w:
		--playr->pos.y;
		break;
	case KEY_a:
		--playr->pos.x;
		break;
	case KEY_s:
		++playr->pos.y;
		break;
	case KEY_d:
		++playr->pos.x;
		break;
	default:
		N_Error("Player-To-Door Interaction Called, But Movement Vector Is Invalid");
		break;
	};
}
static void P_NPCInteract();
static void P_ItemInteract();
static void P_RugInteract();
static void P_BedInteract();
static void P_ChestInteract();
static void P_DeskInteract();

void PlayrAssigner(Game* const gptr)
{
	game = gptr;
}

void Playr::P_Init()
{
	playr = this;
	CombatAssigner(game);
	ItemAssigner(game);
	name = "Test";
	sprite = '@';
	health = 100;
	armor = 12;
	pdir = D_NORTH;
	lvl = 0;
	coin = 0;
	pos = {0, 0};
	sector_id = 0;
	pmode = P_MODE_ROAMING;
	p_rightarm.c_wpn = wpninfo[W_ARM_HB];
    p_leftarm.c_wpn = wpninfo[W_ARM_GRAPPLE];
    p_sidearm.c_wpn = wpninfo[W_SIDE_PLASMA];
    p_heavyside.c_wpn = wpninfo[W_HSIDE_SADB];
    p_primary.c_wpn = wpninfo[W_PRIM_AK77];
    p_heavyprimary.c_wpn = wpninfo[W_HPRIM_RAG13];
    p_shotty.c_wpn = wpninfo[W_SHOTTY_ADB];
	c_wpn = &p_shotty;

	memset(&body_health, 100, sizeof(body_health));
	pstate &= stateinfo[S_PLAYR_NULL].id;
}

static nomadbool_t P_MoveTicker(Playr* playr)
{
#ifdef _NOMAD_DEBUG
	assert(playr);
#endif
	if (playr->pstate == stateinfo[S_PLAYR_NULL].id) {
		playr->pstate = stateinfo[S_PLAYR_SPAWN].id;
		playr->pticker = stateinfo[S_PLAYR_SPAWN].numticks;
		return true;
	}
	else if (playr->pstate == stateinfo[S_PLAYR_MOVE].id) {
		playr->pticker = stateinfo[S_PLAYR_MOVE].numticks;
		return false;
	}
	else {
		if (!playr->pticker) {
			playr->pstate = stateinfo[S_PLAYR_MOVE].id;
			playr->pticker = stateinfo[S_PLAYR_MOVE].numticks;
			return false;
		}
		else {
			return true;
		}
	}
}

void P_UseLeftArm()
{

}
void P_UseRightArm()
{
	
}
void P_ShowWeapons()
{

}

void P_PauseMenu()
{
	pthread_join(game->wthread, NULL);
	game->gamestate = GS_PAUSE;
	game->gamescreen = MENU_PAUSE;
}

void P_UseWeapon()
{
	switch (playr->c_wpn->c_wpn.id) {
	case W_SHOTTY_ADB:
	case W_SHOTTY_FAB:
	case W_SHOTTY_QS:
		P_ShootShotty(playr->c_wpn);
		break;
	case W_HPRIM_FUSION:
	case W_HPRIM_HOSIG:
	case W_HPRIM_RAG13:
	case W_HPRIM_DR:
//		P_ShootSingle(*wpn);
		break;
	default:
		N_Error("Unknown/Invalid Weapon ID! id: %iu", playr->c_wpn->c_wpn.id);
		break;
	};
}

void Game::P_Ticker(nomadint_t input)
{
#ifdef _NOMAD_DEBUG
	LOG("gamestate = %hu", gamestate);
	LOG("gamescreen = %hu", gamescreen);
#endif
	--playr->pticker;
//	playr->P_GetMode();
	playr->P_RunTicker(input);
}

static nomadint_t input;

void Playr::P_RunTicker(nomadint_t finput)
{
	input = finput;
	for (const auto& i : scf::kb_binds) {
		if (input == i.button) {
			(*i.actionp)();
			break;
		}
	}
}

void P_SlideN()
{
	P_DashN();
}
void P_SlideW()
{
	P_DashW();
}
void P_SlideS()
{
	P_DashS();
}
void P_SlideE()
{
	P_DashE();
}

void P_ChangeWeapon1()
{
	playr->c_wpn = &rightarm;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon2()
{
	playr->c_wpn = &leftarm;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon3()
{
	playr->c_wpn = &sidearm;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon4()
{
	playr->c_wpn = &heavyside;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon5()
{
	playr->c_wpn = &primary;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon6()
{
	playr->c_wpn = &heavyprimary;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon7()
{
	playr->c_wpn = &shotty;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon8()
{
	playr->c_wpn = &melee1;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon9()
{
	playr->c_wpn = &melee2;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon10()
{
	playr->c_wpn = &melee3;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}

void Playr::P_GetMode()
{
	coord_t mercpos = botpos[0];
	if ((pos.y == (mercpos.y - 1) && pos.x == mercpos.x)
	|| (pos.x == (mercpos.x + 1) && pos.y == mercpos.y)) {
		pmode = P_MODE_MERCMASTER;
	}
	else {
		pmode = P_MODE_ROAMING;
	}
}

void P_MoveN()
{
	if (!P_MoveTicker(playr)) {
		switch (game->c_map[playr->pos.y - 1][playr->pos.x]) {
		case '_':
			P_DoorInteract(input);
			break;
		case '.':
		case ' ':
			playr->pos.y--;
			break;
		case 'M':
			B_MercMasterInteract();
			break;
		case '(':
			B_BartenderInteract();
			break;
		case ':': { // a chair/seat sprite
			if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
			else { playr->pos.y--; playr->pmode = P_MODE_ROAMING; }
			break; }
		case '=': // weapons smith table
			B_WeaponSmithInteract();
			break;
		default:
			break;
		};
	}
}
void P_MoveW()
{
	if (!P_MoveTicker(playr)) {
		switch (game->c_map[playr->pos.y][playr->pos.x - 1]) {
		case '_':
			P_DoorInteract(input);
			break;
		case '.':
		case ' ':
			playr->pos.x--;
			break;
		case 'M': // the merc master's custom sprite
			B_MercMasterInteract();
			break;
		case '(': // a bar sprite
			B_BartenderInteract();
			break;
		case ':': { // a chair/seat sprite
			if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
			else { playr->pos.x--; playr->pmode = P_MODE_ROAMING; }
			break; }
		case '=': // weapons smith table
			B_WeaponSmithInteract();
			break;
		default:
			break;
		};
	}
}
void P_MoveS()
{
	if (!P_MoveTicker(playr)) {
		switch (game->c_map[playr->pos.y + 1][playr->pos.x]) {
		case '_':
			P_DoorInteract(input);
			break;
		case '.':
		case ' ':
			playr->pos.y++;
			break;
		case 'M':
			B_MercMasterInteract();
			break;
		case '(':
			B_BartenderInteract();
			break;
		case ':': { // a chair/seat sprite
			if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
			else { playr->pos.y++; playr->pmode = P_MODE_ROAMING; }
			break; }
		case '=': // weapons smith table
			B_WeaponSmithInteract();
			break;
		default:
			break;
		};
	}
}
void P_MoveE()
{
	if (!P_MoveTicker(playr)) {
		switch (game->c_map[playr->pos.y][playr->pos.x + 1]) {
		case '_':
			P_DoorInteract(input);
			break;
		case '.':
		case ' ':
			playr->pos.x++;
			break;
		case 'M':
			B_MercMasterInteract();
			break;
		case '(':
			B_BartenderInteract();
			break;
		case ':': { // a chair/seat sprite
			if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
			else { playr->pos.x++; playr->pmode = P_MODE_ROAMING; }
			break; }
		case '=': // weapons smith table
			B_WeaponSmithInteract();
			break;
		default:
			break;
		};
	}
}

void P_DashN()
{
	nomadshort_t range = playr->pos.y - RDASH_SPEED;
	nomadbool_t hit = false;
	while (!hit) {
		if (playr->pos.y == range) break;
		switch (game->c_map[playr->pos.y][playr->pos.x]) {
		case '#': // destructible environments will be added in the future
		case '_':
			hit = true;
			break;
		case ' ':
		case '.':
			break;
		default:
			break;
		};
		--playr->pos.y;
	}
}
void P_DashW()
{
	nomadshort_t range = playr->pos.x - RDASH_SPEED;
	nomadbool_t hit = false;
	while (!hit) {
		if (playr->pos.x == range) break;
		switch (game->c_map[playr->pos.y][playr->pos.x]) {
		case '#': // destructible environments will be added in the future
		case '_':
			hit = true;
			break;
		case ' ':
		case '.':
			break;
		default:
			break;
		};
		--playr->pos.x;
	}
}
void P_DashS()
{
	nomadshort_t range = playr->pos.y + RDASH_SPEED;
	nomadbool_t hit = false;
	while (!hit) {
		if (playr->pos.y == range) break;
		switch (game->c_map[playr->pos.y][playr->pos.x]) {
		case '#': // destructible environments will be added in the future
		case '_':
			hit = true;
			break;
		case ' ':
		case '.':
			break;
		default:
			break;
		};
		++playr->pos.y;
	}
}
void P_DashE()
{
	nomadshort_t range = playr->pos.x + RDASH_SPEED;
	nomadbool_t hit = false;
	while (!hit) {
		if (playr->pos.x == range) break;
		switch (game->c_map[playr->pos.y][playr->pos.x]) {
		case '#': // destructible environments will be added in the future
		case '_':
			hit = true;
			break;
		case ' ':
		case '.':
			break;
		default:
			break;
		};
		++playr->pos.x;
	}
}

void P_ChangeDirL()
{
#ifdef _NOMAD_DEBUG
	assert(playr->pdir < NUMDIRS);
	LOG("pdir = %hu", playr->pdir);
#endif
	if (playr->pdir == D_EAST) {
		playr->pdir = D_NORTH;
	}
	else {
		++playr->pdir;
	}
}

void P_ChangeDirR()
{
#ifdef _NOMAD_DEBUG
	assert(playr->pdir < NUMDIRS);
	LOG("pdir = %hu", playr->pdir);
#endif
	if (playr->pdir == D_NORTH) {
		playr->pdir = D_EAST;
	}
	else {
		--playr->pdir;
	}
}

void P_UseMelee()
{

}

void P_QuickShot()
{

}

void P_KillPlayr()
{
	Z_Free(playr);
}

void CommandConsole()
{
	char buf[1024];
//	mvwscanw(game->screen, , , "%s", buf);
	if (strstr(buf, "fyia") != NULL) {
		if (scf::launch::infinite_ammo)
			scf::launch::infinite_ammo = false;
		else
			scf::launch::infinite_ammo = true;
	}
	else if (strstr(buf, "iwtbag") != NULL) {
		if (scf::launch::godmode)
			scf::launch::godmode = false;
		else
			scf::launch::godmode = true;
	}
	else if (strstr(buf, "tgdnec") != NULL) {
		if (scf::launch::bottomless_clip)
			scf::launch::bottomless_clip = false;
		else
			scf::launch::bottomless_clip = true;
	}
	else if (strstr(buf, "blindmobs") != NULL) {
		if (scf::launch::blindmobs)
			scf::launch::blindmobs = false;
		else
			scf::launch::blindmobs = true;
	}
	else if (strstr(buf, "deafmobs") != NULL) {
		if (scf::launch::deafmobs)
			scf::launch::deafmobs = false;
		else
			scf::launch::deafmobs = true;
	}
	else if (strstr(buf, "fastmobs=1") != NULL) {
		if (scf::launch::fastmobs3)
			scf::launch::fastmobs3 = false;
		if (scf::launch::fastmobs2)
			scf::launch::fastmobs2 = false;
		if (scf::launch::fastmobs1)
			scf::launch::fastmobs1 = false;
		else
			scf::launch::fastmobs1 = true;
	}
	else if (strstr(buf, "fastmobs=2") != NULL) {
		if (scf::launch::fastmobs1)
			scf::launch::fastmobs1 = false;
		if (scf::launch::fastmobs3)
			scf::launch::fastmobs3 = false;
		if (scf::launch::fastmobs2)
			scf::launch::fastmobs2 = false;
		else
			scf::launch::fastmobs2 = true;
	}
}