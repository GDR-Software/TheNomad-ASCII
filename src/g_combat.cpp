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
	nomadbool_t negative = P_Random() & 1;
	nomadbool_t right{};
	if (negative)
		offset = (P_Random() & 2) * -1;
	else
		offset = P_Random() & 2;
	start.y = playr->pos.y;
	start.x = playr->pos.x;
//	area_t collider = G_DoShottyCollider(range, maxspread, playr->pdir, playr->pos);
	for (o = 0; o < numpellets; ++o) {
		right = P_Random() & 1;
		switch (playr->pdir) {
		case D_NORTH: {
			endpoint.y = playr->pos.y - range;
			if (right)
				endpoint.x = playr->pos.x + (P_Random() & maxspread[right].x);
			else
				endpoint.x = playr->pos.x - (P_Random() & maxspread[right].x);
			break; }
		case D_SOUTH: {
			endpoint.y = playr->pos.y + range;
			if (right)
				endpoint.x = playr->pos.x + (P_Random() & maxspread[right].x);
			else
				endpoint.x = playr->pos.x - (P_Random() & maxspread[right].x);
			break; }
		case D_WEST: {
			endpoint.x = playr->pos.x - range;
			if (right)
				endpoint.y = playr->pos.y - (P_Random() & maxspread[right].y);
			else
				endpoint.x = playr->pos.x + (P_Random() & maxspread[right].y);
			break; }
		case D_EAST: {
			endpoint.x = playr->pos.x + range;
			if (right)
				endpoint.y = playr->pos.y - (P_Random() & maxspread[right].y);
			else
				endpoint.x = playr->pos.y + (P_Random() & maxspread[right].y);
			break; }
		};
		collider_t c = G_CastRay(endpoint, start, game);
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
