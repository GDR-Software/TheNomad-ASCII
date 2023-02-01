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

static inline void G_MarkWall(coord_t pos)
{
	for (nomadenum_t i = D_NORTH; i < 4; ++i) {
		switch (i) {
		case D_NORTH:
			switch (game->c_map[pos.y - 1][pos.x]) {
			case ' ':
			case '.':
				game->c_map[pos.y - 1][pos.x] = '~';
				break;
			default:
				break;
			};
			break;
		case D_WEST:
			switch (game->c_map[pos.y][pos.x - 1]) {
			case ' ':
			case '.':
				game->c_map[pos.y][pos.x - 1] = '~';
				break;
			default:
				break;
			};
			break;
		case D_SOUTH:
			switch (game->c_map[pos.y + 1][pos.x]) {
			case ' ':
			case '.':
				game->c_map[pos.y + 1][pos.x] = '~';
				break;
			default:
				break;
			};
			break;
		case D_EAST:
			switch (game->c_map[pos.y][pos.x + 1]) {
			case ' ':
			case '.':
				game->c_map[pos.y][pos.x + 1] = '~';
				break;
			default:
				break;
			};
			break;
		};
	}
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
		switch (game->c_map[c1.y][c1.x]) {
		case ' ':
		case '.':
			break;
//		case '_':
		case '#':
//		case ':':
//		case '(':
//		case '+':
//		case '*':
//		case '<':
//		case '>':
//		case '[':
			G_MarkWall(c1);
			Hud_Printf("System", "You hit a wall");
			break;
		};
	}
}

static inline void G_GetEndpoint(coord_t& end, coord_t start, Weapon* const wpn, nomadenum_t dir, coord_t spread[2])
{
	if (dir == D_NORTH) {
		nomadshort_t left = spread[0].x;
		nomadshort_t right = spread[1].x;
		for (nomadshort_t y = start.y; y > wpn->range; --y) {
			nomadbool_t d = P_Random() & 1;
			for (nomadshort_t )
		}
	}
	/*
	// Calculate the angle from the slope
    nomadint_t angle = atan2(slope.y, slope.x) * 180 / M_PI;

    // Calculate the endpoint considering the spread
    angle += spread * (rand() / (RAND_MAX + 1.0) * 2 - 1);

    // Convert angle to radians
    angle = angle * M_PI / 180;

    // Calculate the endpoint considering the offset
    end.x = start.x + range * cos(angle) + offset;
    end.y = start.y + range * sin(angle) + offset;
	*/
}

static inline void G_GetSpread(nomadenum_t spread, nomadenum_t dir, coord_t pos, coord_t* maxspread)
{
	// use bit-shifts if this gets too slow
	rspread = spread / 2;
	switch (dir) {
	case D_NORTH:
		coord_t& left = maxspread[0];
		coord_t& right = maxspread[1];
		left.y = pos.y;
		left.x = pos.x - rspread;
		right.y = pos.y;
		right.x = pos.x + rspread;
		break;
	case D_WEST:
		coord_t& up = maxspread[0];
		coord_t& down = maxspread[1];
		up.y = pos.y - rspread;
		up.x = pos.x;
		down.y = pos.y + rspread;
		down.x = pos.x;
		break;
	case D_SOUTH:
		coord_t& left = maxspread[0];
		coord_t& right = maxspread[1];
		left.y = pos.y;
		left.x = pos.x - rspread;
		right.y = pos.y;
		right.x = pos.x + rspread;
		break;
	case D_EAST:
		coord_t& up = maxspread[0];
		coord_t& down = maxspread[1];
		up.y = pos.y - rspread;
		up.x = pos.x;
		down.y = pos.y + rspread;
		down.x = pos.x;
		break;
	default:
		N_Error("Unknown/Invalid Entity Direction: %hu", dir);
		break;
	};
}

static inline area_t G_DoShottyCollider(nomaduint_t range, coord_t maxspread[2], nomadenum_t dir, coord_t start)
{
	area_t collider;
	coord_t& tl = collider[0];
	coord_t& tr = collider[1];
	coord_t& br = collider[2];
	coord_t& bl = collider[3];
	switch (dir) {
	case D_NORTH:
		tl.y = pos.y - range;
		tl.x = pos.x - maxspread[0].x;
		tr.y = pos.y - range;
		tr.x = pos.x + maxspread[1].x;
		br.y = pos.y;
		br.x = pos.x + maxspread[1].x;
		bl.y = pos.y;
		bl.x = pos.x - maxspread[0].x;
		break;
	case D_WEST:
		tl.y = pos.y - maxspread[0].y;
		tl.x = pos.x - range;
		tr.y = pos.y - maxspread[0].y;
		tr.x = pos.x;
		br.y = pos.y + maxspread[1].y;
		br.x = pos.x;
		bl.y = pos.y - maxspread[1].y;
		bl.x = pos.x - range;
		break;
	case D_SOUTH:
		tl.y = pos.y;
		tl.x = pos.x - maxspread[0].x;
		tr.y = pos.y;
		tr.x = pos.x + maxspread[1].x;
		bl.y = pos.y + range;
		bl.x = pos.x - maxspread[0].x;
		br.y = pos.y + range;
		br.x = pos.x + maxspread[1].x;
		break;
	case D_EAST:
		break;
	};
	return collider
}

void P_ShootShotty(Weapon* const wpn)
{
	nomadenum_t spread = wpn->c_wpn.spread;
	nomadenum_t numpellets = wpn->c_wpn.numpellets;
	nomaduint_t range = wpn->c_wpn.range;
	nomadshort_t a{};
	coord_t start{};

	coord_t maxspread[2]; // for north and south: 0 -> left, 1 -> right. For east and west: 0 -> up, 1 -> down
	G_GetSpread(spread, playr->pdir, playr->pos, maxspread);

	nomadshort_t offset;
	nomadushort_t o;
	nomadbool_t left{};
	left = P_Random() & 1;
	nomadbool_t upper{};
	upper = P_Random() & 1;
	
	area_t collider = G_DoShottyCollider(range, maxspread, playr->pdir, playr->pos);
	for (o = 0; o < numpellets; ++o) {
		for (auto* i : game->m_Active) {
			if ((i->mpos.y > collider[0].y && i->mpos.y < collider[3].y)
			 && (i->mpos.x > collider[0].x && i->mpos.x < collider[3].x)) {
				// hit a mob
				break;
			}
		}
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
