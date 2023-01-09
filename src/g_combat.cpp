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
//  src/g_combat.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "g_playr.h"
#include "g_mob.h"
#include "g_items.h"
#include "g_rng.h"

static Game* game;
static Playr* playr;

void CombatAssigner(Game* const gptr)
{
	game = gptr;
	playr = game->playr;
}

void P_ShootShotty(Weapon* const wpn)
{
	nomadenum_t spread = wpn->c_wpn.spread;
	nomadenum_t numpellets = wpn->c_wpn.numpellets;
	nomadenum_t a{};

	coord_t maxspread[2]; // 0 -> left, 1 -> right
	switch (playr->pdir) {
	case D_NORTH:
		maxspread[0].y = maxspread[1].y = playr->pos.y;
		maxspread[0].x = playr->pos.x - (spread >> 1);
		maxspread[1].x = playr->pos.x + (spread >> 1);
		break;
	case D_WEST:
		maxspread[0].x = maxspread[1].x = playr->pos.x;
		maxspread[0].y = playr->pos.y + (spread >> 1);
		maxspread[1].y = playr->pos.y - (spread >> 1);
		break;
	case D_SOUTH:
		maxspread[0].y = maxspread[1].y = playr->pos.y;
		maxspread[0].x = playr->pos.x + (spread >> 1);
		maxspread[1].x = playr->pos.x + (spread >> 1);
		break;
	case D_EAST:
		maxspread[0].x = maxspread[1].x = playr->pos.x;
		maxspread[0].y = playr->pos.y - (spread >> 1);
		maxspread[1].y = playr->pos.y + (spread >> 1);
		break;
	default:
		N_Error("Unknown/Invalid Player Direction: %hu", playr->pdir);
		break;
	};

	nomadshort_t offset;
	for (nomadushort_t o = 0; o < numpellets; ++o) {
		a = P_Random() & 4;
		if (a > 2)
			offset = -P_Random() & -2;
		else
			offset = P_Random() & 2;


	}
}