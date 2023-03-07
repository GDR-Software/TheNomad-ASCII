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
//  src/p_physics.cpp
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

// false if entity can walk, true if cannot
nomadbool_t E_CloseCollider(nomadenum_t dir, coord_t& from, Game* const game)
{
	coord_t pos = game->E_GetDir(dir);
	switch (game->c_map[from.y+pos.y][from.x+pos.x]) {
	case '.':
	case '_':
	case ' ':
		return false;
		break;
	default:
		return true;
		break;
	};
	return true;
}

void E_RangedCollider(coord_t start, nomaduint_t range, Game* const game, nomadenum_t dir,
	collider_t& collider)
{
	nomadshort_t end{};
	switch (dir) {
	case D_NORTH: {
		end = start.y - range;
		for (nomadshort_t y = start.y; y != end; --y) {
	//		collider = G_CheckCollider({y, start.x}, game);
		}
		break; }
	case D_WEST: {
		end = start.x - range;
		for (nomadshort_t x = start.x; x != end; --x) {
			switch (game->c_map[start.y][x]) {};
		}
		break; }
	case D_SOUTH: {
		end = start.y + range;
		for (nomadshort_t y = start.y; y != end; ++y) {
			switch (game->c_map[y][start.x]) {};
		}
		break; }
	case D_EAST: {
		end = start.x + range;
		for (nomadshort_t x = start.x; x != end; ++x) {
			switch (game->c_map[start.y][x]) {};
		}
		break; }
	};
}

nomadbool_t G_CheckCollider(coord_t& point, Game* const game, collider_t& c)
{
	c.where = point;
	c.what = ET_AIR;
	c.ptr = nullptr;
	
	if (point == game->playr->pos) {
		c.what = ET_PLAYR;
		c.ptr = (void *)&game->playr;
		return true;
	}

	// always check the entity positions before checking the pint
	for (auto* i : game->m_Active) {
		if (i->mpos.y == point.y && i->mpos.x == point.x) {
			c.ptr = (void *)&i;
			c.what = ET_MOB;
			return true;
		}
	}
	switch (game->c_map[point.y][point.x]) {
	case '#':
	case '_':
		c.what = ET_WALL;
		return true;
		break;
	case '.':
	case ' ':
		return false;
		break;
	default:
		break;
	};
	return false;
}
