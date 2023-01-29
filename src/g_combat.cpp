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

static inline Mob* G_GetHitMob(nomadshort_t y, nomadshort_t x)
{
	for (auto* i : game->m_Active) {
		if (i->mpos.y == y && i->mpos.x == x) { return i; }
	}
	return nullptr;
}

static inline NPC* G_GetHitNPC(nomadshort_t y, nomadshort_t x)
{
	for (auto* i : game->b_Active) {
		if (i->pos.y == y && i->pos.x == x) { return i; }
	}
	return nullptr;
}

//
// G_CastRay(): the general-use combat function that casts a "ray" from a line or slope,
// and determines what it first collides with. This is really just a hitscan collider,
// but I couldn't think of a better name for it.
//
// returns if it hit a wall, and another function (ideally the shooting function) will be
// calling it in a for every bullet in a shot loop. If it hits an entity, it deals damage to
// that entity
//
static inline void G_CastRay(coord_t c1, coord_t c2, Weapon* const wpn) {
	coord_t d, s;
	d.x = abs(c2.x - c1.x), s.x = c1.x < c2.x ? 1 : -1;
	d.y = abs(c2.y - c1.y), s.y = c1.y < c2.y ? 1 : -1;
	nomadint_t err = (d.x > d.y ? d.x : -d.y) >> 1, e2;

	while (c1.x != c2.x || c1.y != c2.y) {
		e2 = err;
		if (e2 > -d.x) { err -= d.y; c1.x += s.x; }
		if (e2 < d.y) { err += d.x; c1.y += s.y; }
	}
}

static inline void G_GetSpread(nomadenum_t spread, nomadenum_t dir, coord_t pos, coord_t* maxspread)
{
	switch (dir) {
	case D_NORTH:
		maxspread[0].y = pos.y;
		maxspread[1].y = pos.y;
		maxspread[0].x = pos.x - (spread >> 1);
		maxspread[1].x = pos.x + (spread >> 1);
		break;
	case D_WEST:
		maxspread[0].x = pos.x;
		maxspread[1].x = pos.x;
		maxspread[0].y = pos.y + (spread >> 1);
		maxspread[1].y = pos.y - (spread >> 1);
		break;
	case D_SOUTH:
		maxspread[0].y = pos.y;
		maxspread[1].y = pos.y;
		maxspread[0].x = pos.x + (spread >> 1);
		maxspread[1].x = pos.x + (spread >> 1);
		break;
	case D_EAST:
		maxspread[0].x = pos.x;
		maxspread[1].x = pos.x;
		maxspread[0].y = pos.y - (spread >> 1);
		maxspread[1].y = pos.y + (spread >> 1);
		break;
	default:
		N_Error("Unknown/Invalid Entity Direction: %hu", dir);
		break;
	};
}

void P_ShootShotty(Weapon* const wpn)
{
	nomadenum_t spread = wpn->c_wpn.spread;
	nomadenum_t numpellets = wpn->c_wpn.numpellets;
	nomaduint_t range = wpn->c_wpn.range;
	nomadshort_t a{};
	coord_t slope, endpoint{};

	coord_t maxspread[2]; // 0 -> left, 1 -> right
	G_GetSpread(spread, playr->pdir, playr->pos, maxspread);

	nomadshort_t offset;
	nomadushort_t o;
	for (o = 0; o < numpellets; ++o) {
		a = P_Random() & 4;
		if (a > 2)
			offset = -P_Random() & -2;
		else
			offset = P_Random() & 2;
		
		slope.y = maxspread[(P_Random() & 1)].y + offset;
		slope.x = maxspread[(P_Random() & 1)].x + offset;
		switch (playr->pdir) {
		case D_NORTH:
			endpoint.y = slope.y - range;
			endpoint.x = slope.x;
			break;
		case D_WEST:
			endpoint.y = slope.y;
			endpoint.x = slope.x - range;
			break;
		case D_SOUTH:
			endpoint.y = slope.y + range;
			endpoint.x = slope.x;
			break;
		case D_EAST:
			endpoint.y = slope.y;
			endpoint.x = slope.x + range;
			break;
		};
		G_CastRay(playr->pos, endpoint, wpn);
	}
}

// gonna need a ticker for this one, y'know, for delays between shots
/*
void P_ShootSingle(const Weapon& wpn)
{
	if (playr->pticker != -1 && playr->pstate == S_PLAYR_SHOOT) {
		nomadenum_t spread = wpn.c_wpn.spread;
		nomaduint_t range = wpn.c_wpn.range;
		nomadshort_t a{};
		coord_t slope;
		
		coord_t maxspread[2];
		G_GetSpread(spread, playr->pdir, playr->pos, maxspread);
	
		nomadshort_t offset;
		a = P_Random() & 4;
		if (playr->pstate == S_PLAYR_SHOOT) {
			if (a > 2)
				offset = (-P_Random() & -2) + -2;
			else
				offset = (P_Random() & 2) + 2;
		}
		else {
			if (a > 2)
				offset = -P_Random() & -2;
			else
				offset = P_Random() & 2;
			playr->pstate = S_PLAYR_SHOOT;
		}
		slope.y = maxspread[(P_Random() & 1)].y + offset;
		slope.x = maxspread[(P_Random() & 1)].x + offset;
		G_CastRay(slope, range, wpn);
	}
}
*/
