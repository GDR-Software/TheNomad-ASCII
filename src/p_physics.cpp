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
#include "g_game.h"

collider_t Game::P_RangedCollider(nomadushort_t range, nomaduint_t wpn)
{
	
}

collider_t Game::P_RangedCollider(nomadushort_t range)
{
	
}

collider_t Game::P_CloseCollider(nomadenum_t dir)
{
	nomadshort_t y{};
	nomadshort_t x{};
	collider_t hit;
	switch (dir) {
	case D_NORTH:
		y = -1; x = 0;
		break;
	case D_WEST:
		y = 0; x = -1;
		break;
	case D_SOUTH:
		y = 1; x = 0;
		break;
	case D_EAST:
		y = 0; x = 1;
		break;
	};
	if (c_map[playr->pos.y+y][playr->pos.x+x] == '#') {
		hit.where = {playr->pos.y+y, playr->pos.x};
		hit.what = ET_WALL;
	}
	else {
		hit.where = {playr->pos.y, playr->pos.x};
		hit.what = ET_AIR;
	}
	return hit;
}