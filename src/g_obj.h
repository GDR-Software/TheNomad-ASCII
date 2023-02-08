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
	S_MOB_FIGHT,
	S_MOB_FLEE,
	S_MOB_DEAD,
	
	S_HULK_NULL,
	S_HULK_SPAWN,
	S_HULK_WANDER,
	S_HULK_IDLE,
	S_HULK_CHASEPLAYR,
	S_HULK_FIGHT,
	S_HULK_FLEE,
	S_HULK_DEAD,
	
	S_RAV_NULL,
	S_RAV_SPAWN,
	S_RAV_WANDER,
	S_RAV_IDLE,
	S_RAV_CHASEPLAYR,
	S_RAV_FIGHT,
	S_RAV_FLEE,
	S_RAV_DEAD,
	
	S_GRUNT_NULL,
	S_GRUNT_SPAWN,
	S_GRUNT_WANDER,
	S_GRUNT_IDLE,
	S_GRUNT_CHASEPLAYR,
	S_GRUNT_FIGHT,
	S_GRUNT_FLEE,
	S_GRUNT_DEAD,
	
	S_DRUID_NULL,
	S_DRUID_SPAWN,
	S_DRUID_WANDER,
	S_DRUID_IDLE,
	S_DRUID_CHASEPLAYR,
	S_DRUID_FIGHT,
	S_DRUID_FLEE,
	S_DRUID_DEAD,
	
	S_SANDWURM_NULL,
	S_SANDWURM_SPAWN,
	S_SANDWURM_WANDER,
	S_SANDWURM_IDLE,
	S_SANDWURM_CHASEPLAYR,
	S_SANDWURM_FIGHT,
	S_SANDWURM_FLEE,
	S_SANDWURM_DEAD,
	
	S_MERC_NULL,
	S_MERC_SPAWN,
	S_MERC_WANDER,
	S_MERC_IDLE,
	S_MERC_CHASEPLAYR,
	S_MERC_FIGHT,
	S_MERC_FLEE,
	S_MERC_DEAD,
	
	S_MERC_LEADER_NULL,
	S_MERC_LEADER_SPAWN,
	S_MERC_LEADER_WANDER,
	S_MERC_LEADER_IDLE,
	S_MERC_LEADER_CHASEPLAYR,
	S_MERC_LEADER_FIGHT,
	S_MERC_LEADER_FLEE,
	S_MERC_LEADER_DEAD,
	
	S_SHOTTY_NULL,
	S_SHOTTY_SPAWN,
	S_SHOTTY_WANDER,
	S_SHOTTY_IDLE,
	S_SHOTTY_CHASEPLAYR,
	S_SHOTTY_FIGHT,
	S_SHOTTY_FLEE,
	S_SHOTTY_DEAD,
	
	S_PISTOL_NULL,
	S_PISTOL_SPAWN,
	S_PISTOL_WANDER,
	S_PISTOL_IDLE,
	S_PISTOL_CHASEPLAYR,
	S_PISTOL_FIGHT,
	S_PISTOL_FLEE,
	S_PISTOL_DEAD,
	
	S_GUNNER_NULL,
	S_GUNNER_SPAWN,
	S_GUNNER_WANDER,
	S_GUNNER_IDLE,
	S_GUNNER_CHASEPLAYR,
	S_GUNNER_FIGHT,
	S_GUNNER_FLEE,
	S_GUNNER_DEAD,
	
	S_NOMAD_LEADER_NULL,
	S_NOMAD_LEADER_SPAWN,
	S_NOMAD_LEADER_WANDER,
	S_NOMAD_LEADER_IDLE,
	S_NOMAD_LEADER_CHASEPLAYR,
	S_NOMAD_LEADER_FIGHT,
	S_NOMAD_LEADER_FLEE,
	S_NOMAD_LEADER_DEAD,
	
	S_NOMAD_WARRIOR_NULL,
	S_NOMAD_WARRIOR_SPAWN,
	S_NOMAD_WARRIOR_WANDER,
	S_NOMAD_WARRIOR_IDLE,
	S_NOMAD_WARRIOR_CHASEPLAYR,
	S_NOMAD_WARRIOR_FIGHT,
	S_NOMAD_WARRIOR_FLEE,
	S_NOMAD_WARRIOR_DEAD,
	
	S_BOT_NULL,
	S_BOT_SPAWN,
	S_BOT_WANDER,
	S_BOT_IDLE,
	S_BOT_INTERACTING,
	S_BOT_DEAD,
	
	NUMSTATES
} state_t;

typedef void (* actionp_t)();

typedef struct entitystate_s
{
	state_t id;
	nomadlong_t numticks;
	state_t next;
	actionp_t actionf;
} entitystate_t;

extern const entitystate_t stateinfo[NUMSTATES];

#endif
