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

Playr::Playr()
{
}

void Playr::P_Init()
{
	name = "Test";
	sprite = '@';
	health = 100;
	armor = 12, 
	pdir = D_NORTH;
	lvl = 0;
	coin = 0;
	pos = {0, 0};
	sector_id = 0;
	for (auto* i : P_wpns)
		i = (Weapon *)Z_Malloc(sizeof(Weapon), TAG_STATIC, &i);
	memset(&body_health, 100, sizeof(body_health));
	pstate &= stateinfo[S_PLAYR_NULL].id;
}

Playr::~Playr()
{
}

static nomadbool_t P_MoveTicker(Playr* playr)
{
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
	playr->pticker--;
	switch (input) {
	case KEY_q: {
		if (!P_MoveTicker(playr)) {
			playr->P_ChangeDirL();
		}
		break; }
	case KEY_e: {
		if (!P_MoveTicker(playr)) {
			playr->P_ChangeDirR();
		}
		break; }
	case KEY_w: {
		if (!P_MoveTicker(playr)) {
			switch (c_map[playr->pos.y - 1][playr->pos.x]) {
			case ' ':
			case '_':
			case '.':
				playr->P_MoveN();
				wrefresh(hudwin[HL_VMATRIX]);
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_a: {
		if (!P_MoveTicker(playr)) {
			switch (c_map[playr->pos.y][playr->pos.x - 1]) {
			case ' ':
			case '_':
			case '.':
				playr->P_MoveW();
				wrefresh(hudwin[HL_VMATRIX]);
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_s: {
		if (!P_MoveTicker(playr)) {
			switch (c_map[playr->pos.y + 1][playr->pos.x]) {
			case ' ':
			case '_':
			case '.':
				playr->P_MoveS();
				wrefresh(hudwin[HL_VMATRIX]);
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_d: {
		if (!P_MoveTicker(playr)) {
			switch (c_map[playr->pos.y][playr->pos.x + 1]) {
			case ' ':
			case '_':
			case '.':
				playr->P_MoveE();
				wrefresh(hudwin[HL_VMATRIX]);
				break;
			default:
				break;
			};
		}
		break; }
	case KEY_TILDA:
		
		break;
	case ctrl('x'):
		pthread_cancel(mthread);
		pthread_cancel(nthread);
		gamestate = GS_PAUSE;
		break;
	case ctrl('z'):
		pthread_cancel(mthread);
		pthread_cancel(nthread);
		this->~Game();
		exit(1);
		break;
	default: break;
	};
}

void Playr::P_Ticker(void)
{
	
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
	if (pdir == D_EAST) {
		pdir = D_NORTH;
	}
	else {
		pdir++;
	}
}

void Playr::P_ChangeDirR()
{
	if (pdir == D_NORTH) {
		pdir = D_EAST;
	}
	else {
		pdir--;
	}
}