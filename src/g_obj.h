#ifndef _G_OBJ_
#define _G_OBJ_

#include "n_shared.h"
/*
enum : nomadenum_t
{
	S_MOB_NULL,
	S_MOB_SPAWN,
	S_MOB_WANDER,
	S_MOB_IDLE,
	S_MOB_CHASEPLAYR,
	S_MOB_FIGHT,
	S_MOB_FLEE,
	S_MOB_DEAD
}; */

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

typedef struct
{
	state_t id;
	nomaduint_t numticks;
//	sprite_t sprite;
//	nomadushort_t color;
} entitystate_t;

extern const entitystate_t stateinfo[NUMSTATES];

typedef enum
{
	M_ZHULK,
	M_ZRAVAGER,
	M_ZGRUNT,

	M_HPISTOL,
	M_HSHOTTY,
	M_HGUNNER,
	M_HROCKET,
	M_HMERC,
	M_HNOMAD,

	NUMMOBS
} mob_t;

#endif
