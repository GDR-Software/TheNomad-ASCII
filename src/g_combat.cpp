//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_combat.cpp
//  general-use combat functions
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

#if 0
static inline Mob* G_GetHitMob(nomadshort_t y, nomadshort_t x)
{
	for (auto* i : game->m_Active) {
		if (i->mpos.y == y && i->mpos.x == x) { return i; }
	}
	return nullptr;
}
#endif

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
	linked_list<Mob*> m_hit;
	m_hit.init();
	for (linked_list<Mob*>::iterator it = game->m_Active.begin(); it->next != game->m_Active.end(); it = it->next) {
		Mob* const i = it->val;
		if (inArea(explosion, i->mpos)) {
			m_hit.push_node(i);
		}
	}
}

static nomadenum_t P_GetWpnIndex(Weapon* const wpn)
{
	for (nomadenum_t i = 0; i < MAX_PLAYR_WPNS; ++i) {
		if (wpn == &playr->P_wpns[i]) {
			return i;
		}
	}
	return 0;
}

void P_ShootShotty(Weapon* const wpn)
{
	if (playr->pticker > -1)
		return;
	
	if (playr->ammunition[AT_SHELL] < 1) {
		P_PlaySFX(scf::sounds::sfx_dry_fire[0]);
		return;
	}
	switch (wpn->c_wpn.id) {
	case W_SHOTTY_ADB:
		P_PlaySFX(scf::sounds::sfx_adb_shot);
		playr->ammunition[AT_SHELL] -= 2;
		if (playr->ammunition[AT_SHELL] < 0)
			playr->ammunition[AT_SHELL] = 0;
		break;
//	case W_SHOTTY_FAB:
//	case W_SHOTTY_QS:
//		P_PlaySFX(scf::sounds::sfx_fab_shot);
//		--playr->ammunition[AT_SHELL].pool;
	default:
		break;
	};
	nomadenum_t spread = wpn->c_wpn.spread;
	nomaduint_t range = wpn->c_wpn.range;
	
	area_t a;
	G_GetShottyArea(&a, playr->pdir, playr->pos, range, spread);
	linked_list<Mob*> hit;
	hit.init();
	linked_list<Mob*>::iterator it;
	for (it = game->m_Active.begin(); it->next != game->m_Active.end(); it = it->next) {
		Mob* const actor = it->val;
		if (inArea(a, actor->mpos)) {
			hit.push_node(actor);
		}
	}
	for (it = hit.begin(); it->next != hit.end(); it = it->next)
		it->val->health -= wpn->c_wpn.dmg;
	
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
	nomadshort_t y, x;
	for (y = playr->pos.y; y != endpoint.y; y += pos.y) {
		for (x = playr->pos.x; x != endpoint.x; x += pos.x) {
			for (linked_list<Mob*>::iterator it = game->m_Active.begin(); it->next != game->m_Active.end(); it = it->next) {
				Mob* const actor = it->val;
				if (actor->mpos == coord_t(y, x)) {
					actor->health -= wpn->c_wpn.dmg;
					goto done;
				}
			}
		}
	}
done:
	playr->pstate = stateinfo[S_PLAYR_SHOOT];
	playr->pticker = playr->pstate.numticks;
}