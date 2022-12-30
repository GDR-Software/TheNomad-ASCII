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

typedef enum : nomaduint_t
{
	S_PLAYR_NULL, // in the menus
	S_PLAYR_SPAWN, // just spawned in
	S_PLAYR_MOVE, // player is moving
	S_PLAYR_IDLE, // player is standing still
	S_PLAYR_SHOOT,
	S_PLAYR_MELEE,
	S_PLAYR_INTERACT,
	S_PLAYR_LOWHEALTH,
	S_PLAYR_DEAD,

	S_MOB_NULL,
	S_MOB_SPAWN,
	S_MOB_WANDER,
	S_MOB_IDLE,
	S_MOB_CHASEPLAYR,
	S_MOB_FIGHT,
	S_MOB_FLEE,
	S_MOB_DEAD,
	
	NUMSTATES
} state_t;

typedef struct entitystate_s
{
	state_t id;
	nomaduint_t numticks;
} entitystate_t;

extern const entitystate_t stateinfo[NUMSTATES];

#endif
