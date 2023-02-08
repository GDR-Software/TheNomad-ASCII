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
//  src/s_mthink.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "g_rng.h"

static Game* game;
static Mob* actor;

void M_ThinkerAssigner(Game* const gptr)
{
	game = gptr;
}

void M_ThinkerCurrent(Mob* const mptr)
{
	actor = mptr;
}

static nomadbool_t M_SeePlayr()
{
	coord_t pos = game->E_GetDir(actor->mdir);
	coord_t end{};
	switch (actor->mdir) {
	case D_NORTH:
		end.y = actor->mpos.y - actor->c_mob.sight_range;
		end.x = actor->mpos.x;
		break;
	case D_WEST:
		end.y = actor->mpos.y;
		end.x = actor->mpos.x - actor->c_mob.sight_range;
		break;
	case D_SOUTH:
		end.y = actor->mpos.y + actor->c_mob.sight_range;
		end.x = actor->mpos.x;
		break;
	case D_EAST:
		end.y = actor->mpos.y;
		end.x = actor->mpos.x + actor->c_mob.sight_range;
		break;
	};
	collider_t hit = G_CastRay(actor->mpos, end, game);
	if (!hit.ptr || hit.what != ET_PLAYR)
		return false;
	else if (hit.what == ET_PLAYR)
		return true;
	return false;
}

void M_SpawnThink()
{
	// emulating MTG summoning sickness
	if (actor->mticker > -1) return;
}

void M_WanderThink()
{
	if (!actor->stepcounter) {
		if (((P_Random() & 29)+1) > 10) {
			actor->stepcounter = P_Random() & 10; // get a cardinal number in the future
		
			// and now with a newly set step counter, we change the direction if rng decides it so
			if ((P_Random() & 100) < actor->c_mob.rng) {
				actor->mdir = P_Random() & 3; // might be the same direction
			}
		}
		else {
			actor->mstate = stateinfo[S_MOB_IDLE+actor->c_mob.mtype];
		}
	}
	else {
		--actor->stepcounter;
		coord_t pos = game->E_GetDir(actor->mdir);
		char move = game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x];
		switch (move) {
		case '.':
		case ' ':
			game->E_MoveImmediate(&actor->mpos, actor->mdir);
			break;
		default:
			actor->mdir = P_Random() & 3;
			break;
		};
	}
}

void M_IdleThink()
{
	// hulks have no idle state, too aggressive and angry
	if (actor->c_mob.mtype == MT_HULK) {
		actor->mstate = stateinfo[S_HULK_WANDER];
		actor->mticker = mstate.numticks;
		return;
	}
	if (M_SeePlayr()) {
		actor->mstate = stateinfo[S_MOB_WANDER+actor->c_mob.stateoffset];
	}
	else {
		return;
	}
}

static nomadenum_t M_NewChaseDir()
{
	coord_t pos = game->E_GetDir(actor->mdir);

	// the mob sees the player, so move them directly towards the player
	if (M_SeePlayr()) {
		coord_t& mpos = mob->mpos;
		char move = game->c_map[mpos.y+pos.y][mpos.x+pos.x];
		switch (move) {
		case '.':
		case ' ':
			return 5;
			break;
		default:
			return (P_Random() & 3);
			break;
		};
	}
	else {
		return (P_Random() & 3);
	}
}

void M_SearchForPlayr()
{
	nomadenum_t pursuit = idle;
	if (mob->M_SmellPlayr()) {
		pursuit++;
	}
	if (mob->M_HearPlayr()) {
		pursuit++;
	}
	if (mob->M_SeePlayr()) {
		pursuit++;
	}
	if (pursuit < 1) {
		return;
	}
	nomadenum_t chasedir = M_GetChaseDir(mob, game);
	nomadbool_t changedir = false;
	if (chasedir == 5) {
		// if the mob doesn't need to change direction, then best 
		// thing to do is to go in the direction of player's last pos
		M_FollowPlayr(mob, game);
	}
	else {
		changedir = true;
	}
	// changing direction if needed
	if (changedir) {
		mob->mdir = chasedir;
		// try again after changing
		chasedir = M_GetChaseDir(mob, game);
		if (chasedir == 5) {
			mob->mdir = chasedir;
		}
		// if it fails again, give up
		else {
			return;
		}
	}
}
