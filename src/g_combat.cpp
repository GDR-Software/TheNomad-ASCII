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
#include "g_animation.h"

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

static inline void G_GetShottyArea(area_t* a, nomadenum_t dir, coord_t pos, nomaduint_t range,
	nomadenum_t spread)
{
	coord_t& tl = a->tl;
	coord_t& tr = a->tr;
	coord_t& bl = a->bl;
	coord_t& br = a->br;
	switch (dir) {
	case D_NORTH:
		tl.y = pos.y - range;
		tl.x = pos.x - spread;
		tr.y = tl.y;
		tr.x = pos.x + spread;
		bl.y = pos.y;
		bl.x = tl.x;
		br.y = pos.y;
		br.x = tr.x;
		break;
	case D_WEST:
		tl.y = pos.y - spread;
		tl.x = pos.x - range;
		tr.y = tl.y;
		tr.x = pos.x;
		bl.y = pos.y + spread;
		bl.x = pos.x - range;
		br.y = pos.y + spread;
		br.x = tr.x;
		break;
	case D_SOUTH:
		tl.y = pos.y;
		tl.x = pos.x - spread;
		tr.y = pos.y;
		tr.x = pos.x + spread;
		bl.y = pos.y + range;
		bl.x = pos.x - spread;
		br.y = pos.y + range;
		br.x = pos.x + spread;
		break;
	};
}

void P_ShootShotty(Weapon* const wpn)
{
	if (playr->pticker > -1)
		return;
	
	nomadenum_t spread = wpn->c_wpn.spread;
	nomaduint_t range = wpn->c_wpn.range;
	
	area_t a;
	G_GetShottyArea(&a, playr->pdir, playr->pos, range, spread);
	std::vector<Mob*> hit;
	for (auto *i : game->m_Active) {
		if ((i->mpos.y >= a.tl.y && i->mpos.y <= a.br.y)
		&&  (i->mpos.x >= a.tl.x && i->mpos.x <= a.br.x)) {
			hit.push_back(i);
		}
	}
	// divide the damage somewhat equally
	nomaduint_t divvy = wpn->c_wpn.dmg / hit.size();
	for (auto* const i : hit) {
		i->health -= divvy;
	}
	playr->pstate = stateinfo[S_PLAYR_SHOOT];
	playr->pticker = playr->pstate.numticks;
}

// gonna need a ticker for this one, y'know, for delays between shots
void P_ShootSingle(Weapon* const wpn)
{
	if (playr->pticker > -1)
		return;

	nomadenum_t spread = wpn->c_wpn.spread;
	nomaduint_t range = wpn->c_wpn.range;
	coord_t endpoint;
	switch (playr->pdir) {
	case D_NORTH:
		endpoint.y = playr->pos.y - range;
		endpoint.x = playr->pos.x;
		break;
	case D_WEST:
		endpoint.y = playr->pos.y;
		endpoint.x = playr->pos.x - range;
		break;
	case D_SOUTH:
		endpoint.y = playr->pos.y + range;
		endpoint.x = playr->pos.x;
		break;
	case D_EAST:
		endpoint.y = playr->pos.y;
		endpoint.x = playr->pos.x + range;
		break;
	default:
		LOG_WARN("playr->pdir was invalid value %hu, setting to D_NORTH", playr->pdir);
		playr->pdir = D_NORTH;
		endpoint.y = playr->pos.y - range;
		endpoint.x = playr->pos.x;
		break;
	};
	coord_t pos = game->E_GetDir(playr->pdir);
	for (nomadshort_t y = playr->pos.y; y != endpoint.y; y += pos.y) {
		for (nomadshort_t x = playr->pos.x; x != endpoint.x; x += pos.x) {
			for (auto* i : game->m_Active) {
				if (i->mpos == coord_t(y, x)) {
					i->health -= wpn->c_wpn.dmg;
				}
			}
		}
	}
	playr->pstate = stateinfo[S_PLAYR_SHOOT];
	playr->pticker = playr->pstate.numticks;
}