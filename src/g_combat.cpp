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

#define FIXED_POINT_SCALE 1000

nomadfixed_t int_to_fixed_point(nomadint_t x) {
    return x * FIXED_POINT_SCALE;
}

nomadint_t fixed_point_to_int(nomadfixed_t x) {
    return x / FIXED_POINT_SCALE;
}

nomadfixed_t multiply_fixed_point(nomadfixed_t x, nomadfixed_t y) {
    return ((int64_t) x) * y / FIXED_POINT_SCALE;
}

nomadfixed_t divide_fixed_point(nomadfixed_t x, nomadfixed_t y) {
    return ((int64_t) x) * FIXED_POINT_SCALE / y;
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
static inline void G_CastRay(coord_t c1, coord_t c2) {
	nomadshort_t dx = fixed_point_to_int(c2[1] - c1[1]);
    nomadshort_t dy = fixed_point_to_int(c2[0] - c1[0]);
    nomadshort_t d = 2 * dy - dx;
    nomadshort_t incrE = 2 * dy;
    nomadshort_t incrNE = 2 * (dy - dx);
	nomadshort_t x = fixed_point_to_int(c1[1]);
    nomadshort_t y = fixed_point_to_int(c1[0]);

    while (x < fixed_point_to_int(c1[1])) {
        x++;

        if (d <= 0) {
            d += incrE;
        } else {
            d += incrNE;
            y++;
		}
    }
}
static inline void G_GetEndpoint(vec2_t& end, vec2_t start, Weapon* const wpn, nomadenum_t dir, nomadenum_t spread,
	vec2_t slope)
{

	// Calculate the length of the line
    nomadfixed_t length = to_fixed(Q_root(to_float(slope[1] * slope[1] + slope[0] * slope[0])));
    
    // Calculate the spread factor
    nomadfixed_t spread_factor = (length * spread) / 100;
    
    // Calculate the endpoint coordinates
    end[1] = start[1] + (slope[1] * spread_factor) / length;
    end[0] = start[0] + (slope[0] * spread_factor) / length;
	/*
	// Calculate the angle from the slope
    nomadint_t angle = atan2(slope.y, slope.x) * 180 / M_PI;

    // Calculate the endpoint considering the spread
    angle += spread * (rand() / (RAND_MAX + 1.0) * 2 - 1);

    // Convert angle to radians
    angle = angle * M_PI / 180;

    // Calculate the endpoint considering the offset
    end.x = start.x + range * cos(angle) + offset;
    end.y = start.y + range * sin(angle) + offset; */
}

static inline void G_GetSpread(nomadenum_t spread, nomadenum_t dir, coord_t pos, coord_t* maxspread)
{
	// use bit-shifts if this gets too slow
	nomadshort_t rspread = spread / 2;
	coord_t& left = maxspread[0];
	coord_t& right = maxspread[1];
	coord_t& up = maxspread[0];
	coord_t& down = maxspread[1];
	switch (dir) {
	case D_NORTH:
		left.y = pos.y;
		left.x = pos.x - rspread;
		right.y = pos.y;
		right.x = pos.x + rspread;
		break;
	case D_WEST:
		up.y = pos.y - rspread;
		up.x = pos.x;
		down.y = pos.y + rspread;
		down.x = pos.x;
		break;
	case D_SOUTH:
		left.y = pos.y;
		left.x = pos.x - rspread;
		right.y = pos.y;
		right.x = pos.x + rspread;
		break;
	case D_EAST:
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
	coord_t& tl = collider.tl;
	coord_t& tr = collider.tr;
	coord_t& br = collider.br;
	coord_t& bl = collider.bl;
	switch (dir) {
	case D_NORTH:
		tl.y = start.y - range;
		tl.x = start.x - maxspread[0].x;
		tr.y = start.y - range;
		tr.x = start.x + maxspread[1].x;
		br.y = start.y;
		br.x = start.x + maxspread[1].x;
		bl.y = start.y;
		bl.x = start.x - maxspread[0].x;
		break;
	case D_WEST:
		tl.y = start.y - maxspread[0].y;
		tl.x = start.x - range;
		tr.y = start.y - maxspread[0].y;
		tr.x = start.x;
		br.y = start.y + maxspread[1].y;
		br.x = start.x;
		bl.y = start.y - maxspread[1].y;
		bl.x = start.x - range;
		break;
	case D_SOUTH:
		tl.y = start.y;
		tl.x = start.x - maxspread[0].x;
		tr.y = start.y;
		tr.x = start.x + maxspread[1].x;
		bl.y = start.y + range;
		bl.x = start.x - maxspread[0].x;
		br.y = start.y + range;
		br.x = start.x + maxspread[1].x;
		break;
	case D_EAST:

		break;
	};
	return collider;
}

static inline void G_DoShotty(coord_t maxspread[2], nomaduint_t range, coord_t pos, nomadbool_t left, nomadbool_t right)
{
	nomadshort_t y;
	nomadshort_t x;

	y = maxspread[0].y; // always starts from left/up
	x = maxspread[0].x; 
	for (; y != )
}

void P_ShootShotty(Weapon* const wpn)
{
	nomadenum_t spread = wpn->c_wpn.spread;
	nomadenum_t numpellets = wpn->c_wpn.numpellets;
	nomaduint_t range = wpn->c_wpn.range;
	nomadshort_t a{};
	coord_t start{}, endpoint{};

	coord_t maxspread[2]; // for north and south: 0 -> left, 1 -> right. For east and west: 0 -> up, 1 -> down
	G_GetSpread(spread, playr->pdir, playr->pos, maxspread);

	nomadshort_t offset{};
	nomadenum_t o{};
	nomadbool_t left{};
	nomadbool_t upper{};
	left = P_Random() & 1;
	upper = P_Random() & 1;
	area_t collider = G_DoShottyCollider(range, maxspread, playr->pdir, playr->pos);
	for (o = 0; o < numpellets; ++o) {
		a = P_Random() & 4;
		if (a > 2)
			offset = -P_Random() & -2;
		else
			offset = P_Random() & 2;
		G_DoShotty(maxspread, range, playr->pos, left, upper);
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
