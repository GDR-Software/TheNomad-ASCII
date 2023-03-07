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
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"
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

void G_GetShottyArea(area_t* a, nomadenum_t dir, coord_t pos, nomaduint_t range,
	nomadenum_t spread)
{
	coord_t go = game->E_GetDir(dir);
	coord_t tmp = pos;
	coord_t amount = {0, 0};
	nomadbool_t done = false;
	
	// shorten up the blast if it hit a wall
	for (nomadshort_t i = 0; i < range && !done; ++i) {
		tmp += go;
		++amount;
		switch (game->c_map[tmp.y][tmp.x]) {
		case sprites[SPR_FLOOR_INSIDE]:
		case sprites[SPR_FLOOR_OUTSIDE]:
			break;
		default:
			done = true;
			break;
		};
	}
	coord_t& tl = a->tl;
	coord_t& tr = a->tr;
	coord_t& bl = a->bl;
	coord_t& br = a->br;
	switch (dir) {
	case D_NORTH:
		tl = {pos.y - range, pos.x - spread};
		tr = {tl.y, pos.x + spread};
		bl = {pos.y, tl.x};
		br = {pos.y, tr.x};
		break;
	case D_WEST:
		tl = {pos.y - spread, pos.x - range};
		tr = {tl.y, pos.x};
		bl = {pos.y + spread, pos.x - range};
		br = {pos.y + spread, tr.x};
		break;
	case D_SOUTH:
		tl = {pos.y, pos.x - spread};
		tr = {pos.y, pos.x + spread};
		bl = {pos.y + range, pos.x - spread};
		br = {pos.y + range, pos.x + spread};
		break;
	};
	if (disBetweenOBJ(tmp, pos) < range) {
		switch (dir) {
		case D_NORTH:
			a->tl.y += amount.y;
			break;
		case D_WEST:
			a->tl.x += amount.x;
			break;
		case D_SOUTH:
			a->tl.y -= amount.y;
			break;
		case D_EAST:
			a->tl.x -= amount.x;
			break;
		};
	}
}

void P_DoGrenade(Weapon* const wpn)
{
	area_t explosion;
	coord_t& tl = explosion.tl;
	coord_t& tr = explosion.tr;
	coord_t& bl = explosion.bl;
	coord_t& br = explosion.br;
	std::vector<Mob*> m_hit;
	for (auto* i : game->m_Active) {
		if (inArea(explosion, i->mpos)) {
			m_hit.push_back(i);
		}
	}
}

void P_ShootShotty(Weapon* const wpn)
{
	if (playr->pticker > -1)
		return;
	
	switch (wpn->c_wpn.id) {
	case W_SHOTTY_ADB:
		P_PlaySFX(scf::sounds::sfx_adb_shot.c_str());
		break;
	default:
		break;
	};
	nomadenum_t spread = wpn->c_wpn.spread;
	nomaduint_t range = wpn->c_wpn.range;
	
	area_t a;
	G_GetShottyArea(&a, playr->pdir, playr->pos, range, spread);
	std::vector<Mob*> hit;
	for (auto *i : game->m_Active) {
		if (inArea(a, i->mpos))
			hit.push_back(i);
	}
	for (auto* const i : hit)
		i->health -= playr->c_wpn->c_wpn.dmg;
	
	playr->pstate = stateinfo[S_PLAYR_SHOOT];
	playr->pticker = playr->pstate.numticks;
}

// gonna need a ticker for this one, y'know, for delays between shots
void P_ShootSingle(Weapon* const wpn)
{
	if (playr->pticker > -1)
		return;

	nomaduint_t range = wpn->c_wpn.range;
	coord_t endpoint;
	switch (playr->pdir) {
	case D_NORTH:
		endpoint = {playr->pos.y - range, playr->pos.x};
		break;
	case D_WEST:
		endpoint = {playr->pos.y, playr->pos.x - range};
		break;
	case D_SOUTH:
		endpoint = {playr->pos.y + range, playr->pos.x};
		break;
	case D_EAST:
		endpoint = {playr->pos.y, playr->pos.x + range};
		break;
	default:
		LOG_WARN("playr->pdir was invalid value %hu, setting to D_NORTH", playr->pdir);
		playr->pdir = D_NORTH;
		endpoint = {playr->pos.y - range, playr->pos.x};
		break;
	};
	coord_t pos = game->E_GetDir(playr->pdir);
	for (nomadshort_t y = playr->pos.y; y != endpoint.y; y += pos.y) {
		for (nomadshort_t x = playr->pos.x; x != endpoint.x; x += pos.x) {
			for (auto* i : game->m_Active) {
				if (i->mpos == coord_t(y, x)) {
					i->health -= wpn->c_wpn.dmg;
					goto done; // only one hit target allowed (armor piercing in the future'll override this)
				}
			}
		}
	}
done:
	playr->pstate = stateinfo[S_PLAYR_SHOOT];
	playr->pticker = playr->pstate.numticks;
}