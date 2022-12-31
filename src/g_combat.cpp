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
	weapon_t* w = &wpn->c_wpn;
	coord_t origin;
	nomadshort_t spos;
	switch (playr->pdir) {
	case D_NORTH:
		spos = playr->pos.x;
		break;
	case D_WEST:
		spos = playr->pos.y;
		break;
	case D_SOUTH:
		spos = playr->pos.x;
		break;
	case D_EAST:
		spos = playr->pos.y;
		break;
	default: /*
#ifdef _NOMAD_DEBUG
// TODO: add in debug log
#endif*/
		N_Error("Unknown/Invalid Player Direction: %i", playr->pdir); // this should never happen
		break;
	};
	nomadshort_t offset;
	nomadenum_t numpellets = w->numpellets;
	for (nomadshort_t o = numpellets; o > -1; --o) {
		nomadbool_t s = (rand() % 1) == 1 ? offset = -P_Random() & -2 : offset = P_Random() & 2;
		nomadshort_t spread[2];
		spread[0] = spos - ((w->spread >> 1) + offset);
		spread[1] = spos + ((w->spread >> 1) + offset);
		
		// first, cast a straight line ray whence the player is facing
		// TODO: THIS
	}
}