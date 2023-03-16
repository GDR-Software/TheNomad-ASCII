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
// DESCRIPTION: src/p_common.cpp
//----------------------------------------------------------
#include "n_shared.h"
#include "g_obj.h"
#include "p_npc.h"
#include "g_zone.h"
#include "g_mob.h"
#include "g_items.h"
#include "g_rng.h"
#include "g_map.h"
#include "scf.h"
#include "g_playr.h"
#include "s_world.h"
#include "g_game.h"

void Game::E_MoveImmediate(coord_t* epos, nomadenum_t edir)
{
	coord_t pos = E_GetDir(edir);
	epos->y += pos.y;
	epos->x += pos.x;
}

// this function only works with functions using algos similar
// to the one used to calculate FOV in m_hud.cpp (P_GetVMatrix)
//	This will return an approximation of what direction the entity is facing in
//	because of the fact that there are no diagonals yet
// FIXME - maybe (testing needed)
nomadenum_t Game::E_GetFacing(coord_t current, coord_t orig)
{
	// east
	if (current.x > orig.x) {
		// south-east
		if (current.y > orig.y) {
			return (orig.y - current.y) >= (orig.x - current.x) ? D_SOUTH : D_EAST;
		}
		// north-east
		else if (current.y < orig.y) {
			return (current.y - orig.y) >= (current.x - orig.x) ? D_NORTH : D_EAST;
		}
	}
	// west
	else if (current.x < orig.x) {
		// south-west
		if (current.y > orig.y) {
			return (orig.y - current.y) >= (orig.x - current.x) ? D_SOUTH : D_WEST;
		}
		else if (current.y < orig.y) {
			return (current.y - orig.y) >= (current.x - orig.x)? D_NORTH : D_WEST;
		}
	}
	return D_NORTH;
}

coord_t Game::E_GetDir(nomadenum_t dir)
{
	coord_t coords;
	coords.x = coords.y = 0;
	switch (dir) {
	case D_NORTH:
		coords.y = -1;
		break;
	case D_WEST:
		coords.x = -1;
		break;
	case D_SOUTH:
		coords.y = 1;
		break;
	case D_EAST:
		coords.x = 1;
		break;
	default:
		N_Error("Unknown/Invalid Direction For Entity!");
		break;
	};
	return coords;
}

inline coord_t Game::E_TryMove(coord_t* epos, nomadenum_t* edir)
{
	coord_t pos = E_GetDir(*edir);
	if (c_map[epos->y+pos.y][epos->x+pos.x] != '#') {
		return pos;
	}
	else {
		*edir = P_Random() & 3; // might be the same direction rolled
		pos = E_GetDir(*edir);
		if (c_map[epos->y+pos.y][epos->x+pos.x] != '#') {
			return pos;
		}
		else {
			return {0, 0};
		}
	}
}

nomadbool_t Game::E_Move(coord_t* epos, nomadenum_t* edir)
{
	coord_t pos = E_TryMove(epos, edir);
	if (pos.x == 0 && pos.y == 0) {
		return false;
	}
	else {
		epos->y += pos.y;
		epos->x += pos.x;
		return true;
	}
}