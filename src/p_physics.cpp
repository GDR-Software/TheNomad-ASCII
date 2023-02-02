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

static Game* game;

void PhysicsAssigner(Game* const gptr)
{
	game = gptr;
}

// false if entity can walk, true if cannot
nomadbool_t E_CloseCollider(nomadenum_t dir, coord_t from, Game* const game)
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

void G_CheckCollider(coord_t point)
{
	switch (game->c_map[point.y][point.x]) {
	case '#':
	case '_':
		return;
		break;
	case '.':
	case ' ':
		return;
		break;
	};
}
