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
//  src/g_obj.h
//----------------------------------------------------------
#ifndef _G_OBJ_
#define _G_OBJ_

#pragma once

#include "n_shared.h"

typedef enum : nomadenum_t
{
	ET_PLAYR,
	ET_MOB,
	ET_NPC,
	ET_ITEM,
	ET_AIR,
	ET_WALL,

	ET_NULL
} entitytype_t;

typedef enum : nomadint_t
{
	EF_PLAYR_HIT, // damaged from player's attack
	EF_JUST_HIT, // hit by non-player entity
	EF_KILLABLE
} entityflag_t;

typedef enum : nomaduint_t
{
	S_MOB_NULL,
	S_MOB_SPAWN,
	S_MOB_WANDER,
	S_MOB_IDLE,
	S_MOB_CHASE,
	S_MOB_FIGHT,
	S_MOB_FLEE,
	S_MOB_DEAD,

	S_PLAYR_NULL, // in the menus
	S_PLAYR_SPAWN, // just spawned in
	S_PLAYR_MOVE, // player is moving
	S_PLAYR_IDLE, // player is standing still
	S_PLAYR_SHOOT,
	S_PLAYR_MELEE,
	S_PLAYR_INTERACT,
	S_PLAYR_LOWHEALTH,
	S_PLAYR_DEAD,
	
	S_BOT_NULL,
	S_BOT_SPAWN,
	S_BOT_WANDER,
	S_BOT_IDLE,
	S_BOT_INTERACTING,
	S_BOT_DEAD,
	
	NUMSTATES,

	S_STATE_NONE
} state_t;

typedef struct entitystate_s
{
	state_t id;
	nomadlong_t numticks;
	state_t next;
	inline bool operator==(const entitystate_s state) const {
		return (id == state.id && next == state.next && numticks == state.numticks);
	}
	inline bool operator!=(const entitystate_s state) const {
		return (id != state.id && next != state.next && numticks != state.numticks);
	}
} entitystate_t;

extern std::vector<entitystate_t> stateinfo;

#endif
