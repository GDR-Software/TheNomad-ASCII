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
	name = "Test";
	sprite = '@';
	health = 100;
	armor = 12, 
	pdir = D_NORTH;
	lvl = 0;
	coin = 0;
	pos = {0, 0};
	sector_id = 0;
	pmode = P_MODE_ROAMING;
#ifndef _NOMAD_DEBUG
	for (auto* i : P_wpns)
		i = (Weapon *)Z_Malloc(sizeof(Weapon), TAG_STATIC, &i);
#else
	for (auto* i : P_wpns) {
		i = (Weapon *)Z_Malloc(sizeof(Weapon), TAG_STATIC, &i);
		assert(i);
	}
#endif
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

void Playr::P_RunTicker(nomadint_t input)
{
	switch (input) {
	case KEY_EP:
		break;
	case KEY_AT:
		break;
	case KEY_HASH:
		break;
	case KEY_DOLLAR:
		break;
	case KEY_q: {
		if (!P_MoveTicker(this)) {
			P_ChangeDirL();
		}
		break; }
	case KEY_e: {
		if (!P_MoveTicker(this)) {
			P_ChangeDirR();
		}
		break; }
	case KEY_w: {
		if (!P_MoveTicker(this)) {
			switch (game->c_map[pos.y - 1][pos.x]) {
			case '_':
				P_DoorInteract(input);
				break;
			case '.':
			case ' ':
				P_MoveN();
				break;
			case 'M':
				B_MercMasterInteract();
				break;
			case '(':
				B_BartenderInteract();
				break;
			case ':': { // a chair/seat sprite
				if (pmode != P_MODE_SITTING) { P_ChairInteract(input); }
				else { P_MoveN(); playr->pmode = P_MODE_ROAMING; }
				break; }
			case '=': // weapons smith table
				B_WeaponSmithInteract();
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_a: {
		if (!P_MoveTicker(this)) {
			switch (game->c_map[pos.y][pos.x - 1]) {
			case '_':
				P_DoorInteract(input);
				break;
			case '.':
			case ' ':
				P_MoveW();
				break;
			case 'M': // the merc master's custom sprite
				B_MercMasterInteract();
				break;
			case '(': // a bar sprite
				B_BartenderInteract();
				break;
			case ':': { // a chair/seat sprite
				if (pmode != P_MODE_SITTING) { P_ChairInteract(input); }
				else { P_MoveW(); playr->pmode = P_MODE_ROAMING; }
				break; }
			case '=': // weapons smith table
				B_WeaponSmithInteract();
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_s: {
		if (!P_MoveTicker(this)) {
			switch (game->c_map[pos.y + 1][pos.x]) {
			case '_':
				P_DoorInteract(input);
				break;
			case '.':
			case ' ':
				P_MoveS();
				break;
			case 'M':
				B_MercMasterInteract();
				break;
			case '(':
				B_BartenderInteract();
				break;
			case ':': { // a chair/seat sprite
				if (pmode != P_MODE_SITTING) { P_ChairInteract(input); }
				else { P_MoveS(); playr->pmode = P_MODE_ROAMING; }
				break; }
			case '=': // weapons smith table
				B_WeaponSmithInteract();
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_d: {
		if (!P_MoveTicker(this)) {
			switch (game->c_map[pos.y][pos.x + 1]) {
			case '_':
				P_DoorInteract(input);
				break;
			case '.':
			case ' ':
				P_MoveE();
				break;
			case 'M':
				B_MercMasterInteract();
				break;
			case '(':
				B_BartenderInteract();
				break;
			case ':': { // a chair/seat sprite
				if (pmode != P_MODE_SITTING) { P_ChairInteract(input); }
				else { P_MoveE(); playr->pmode = P_MODE_ROAMING; }
				break; }
			case '=': // weapons smith table
				B_WeaponSmithInteract();
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_TILDA:
		
		break;
	case KEY_GRAVE:
		CommandConsole(game);
		break;
	case ctrl('x'):
//		pthread_join(game->mthread, NULL);
//		pthread_join(game->nthread, NULL);
		pthread_join(game->wthread, NULL);
		game->gamestate = GS_PAUSE;
		break;
	case ctrl('z'):
		delwin(game->hudwin[HL_VMATRIX]);
		game->~Game();
		exit(1);
		break;
	default: break;
	};
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

void Playr::P_MoveN()
{
	pos.y--;
}

void Playr::P_MoveW()
{
	pos.x--;
}

void Playr::P_MoveS()
{
	pos.y++;
}

void Playr::P_MoveE()
{
	pos.x++;
}

void Playr::P_DashN()
{
	pos.y -= 5;
}

void Playr::P_DashW()
{
	pos.x -= 5;
}

void Playr::P_DashS()
{
	pos.y += 5;
}

void Playr::P_DashE()
{
	pos.x += 5;
}

void Playr::P_ChangeDirL()
{
#ifdef _NOMAD_DEBUG
	assert(pdir < NUMDIRS);
	LOG("pdir = %hu", pdir);
#endif
	if (pdir == D_EAST) {
		pdir = D_NORTH;
	}
	else {
		++pdir;
	}
}

void Playr::P_ChangeDirR()
{
#ifdef _NOMAD_DEBUG
	assert(pdir < NUMDIRS);
	LOG("pdir = %hu", pdir);
#endif
	if (pdir == D_NORTH) {
		pdir = D_EAST;
	}
	else {
		--pdir;
	}
}

void P_KillPlayr()
{
	Z_Free(playr);
}

static void CommandConsole(Game* const game)
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