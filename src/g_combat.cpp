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
static inline void G_CastRay(const coord_t slope, nomadshort_t range, Weapon* const wpn)
{
	nomadshort_t y{}, x{};
	nomadshort_t* rptr;
	nomadshort_t mrange;
	switch (playr->pdir) {
	case D_NORTH:
		mrange = range - playr->pos.y;
		rptr = &y;
		break;
	case D_WEST:
		mrange = range - playr->pos.x;
		rptr = &x;
		break;
	case D_SOUTH:
		mrange = range + playr->pos.y;
		rptr = &y;
		break;
	case D_EAST:
		mrange = range + playr->pos.x;
		rptr = &x;
		break;
	default:
		N_Error("Unknown/Invalid Player Direction: %hu", playr->pdir);
		break;
	};
	for (y = playr->pos.y;; y += slope.y) {
			for (x = playr->pos.x;; x += slope.x) {
				// fixed
				if (*rptr >= mrange)
					break;
				
				switch (game->c_map[y][x]) {
				case ' ':
				case '.':
					break;
				case '#':
				case '_':
					return; // hit a wall, the ray is finished
					break;
				default: {
					if (P_Random() > wpn->c_wpn.rng) {
						Mob* const mob = G_GetHitMob(y, x);
						NPC* npc;
						if (!mob) {
							npc = G_GetHitNPC(y, x);
							npc->health -= wpn->c_wpn.dmg;
						}
						else {
							mob->health -= wpn->c_wpn.dmg;
						}
						if (!mob && !npc)
							N_Error("Hit An Invalid Entity (both pointers were NULL, but collided with a char not meant to be there), Corrupt Memory?");
					}
					break; }
				};
			}
		}
}

void P_ShootShotty(Weapon* const wpn)
{
	nomadenum_t spread = wpn->c_wpn.spread;
	nomadenum_t numpellets = wpn->c_wpn.numpellets;
	nomaduint_t range = wpn->c_wpn.range;
	nomadshort_t a{};
	coord_t slope;

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
	nomadushort_t o;
	for (o = 0; o < numpellets; ++o) {
		a = P_Random() & 4;
		if (a > 2)
			offset = -P_Random() & -2;
		else
			offset = P_Random() & 2;
		
		slope.y = maxspread[(P_Random() & 1)].y + offset;
		slope.x = maxspread[(P_Random() & 1)].x + offset;
		G_CastRay(slope, range, wpn);
	}
}

// gonna need a ticker for this one, y'know, for delays between shots
void P_ShootSingle(Weapon* const wpn)
{
	nomadenum_t spread = wpn->c_wpn.spread;
}