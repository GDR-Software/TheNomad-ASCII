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
//  src/info.cpp
//  this is where all the boiled-into-the-binaries takes place
//----------------------------------------------------------
#include "g_items.h"
#include "scf.h"
#include "g_game.h"
#include "g_obj.h"
/*
const mstate_t mstates[NUMMOBSTATES] = {
	{S_MOB_NULL,      0,                   NULL},
	{S_MOB_SPAWN,     15,                  &M_SpawnThink},
	{S_MOB_WANDER,    16,                  &M_WanderThink},
	{S_MOB_IDLE,      scf::ticrate_base*2, NULL},
	{S_MOB_CHASEPLAYR,scf::ticrate_base*25,NULL},
	{S_MOB_FIGHT,     scf::ticrate_base*15,NULL},
	{S_MOB_FLEE,      15,                  NULL},
	{S_MOB_DEAD,      65,                  NULL},
}; */

const entitystate_t stateinfo[NUMSTATES] = {
	{S_PLAYR_NULL,      0},
	{S_PLAYR_SPAWN,     2},
	{S_PLAYR_MOVE,      5},
	{S_PLAYR_IDLE,      3},
	{S_PLAYR_SHOOT,     6},
	{S_PLAYR_MELEE,     8},
	{S_PLAYR_INTERACT,  10},
	{S_PLAYR_LOWHEALTH, 24},
	{S_PLAYR_DEAD,      scf::ticrate_base},
	{S_MOB_NULL,        0},
	{S_MOB_SPAWN,       15},
	{S_MOB_WANDER,      16},
	{S_MOB_IDLE,        scf::ticrate_base*2},
	{S_MOB_CHASEPLAYR,  scf::ticrate_base*25},
	{S_MOB_FIGHT,       scf::ticrate_base*15},
	{S_MOB_FLEE,        15},
	{S_MOB_DEAD,        65},
	{S_BOT_NULL,        0},
	{S_BOT_SPAWN,       10},
	{S_BOT_WANDER,      scf::ticrate_base*2},
	{S_BOT_IDLE,        scf::ticrate_base*3},
	{S_BOT_INTERACTING, scf::ticrate_base*6},
	{S_BOT_DEAD,        10},
};

// name, sprite, health, armor, mtype, etype, rng, chance-to-spawn, sndtol, sndarea,
// smelltol, smellarea, sight range, hasmelee, hashitscan. hasprojectile, melee dmg,
// hitscan dmg, hitscan range, projectile dmg, projectile range, mlore, mdrops
const mobj_t mobinfo[NUMMOBS] = {

{"Hulk\0",           'h', 487, 56,  MT_HULK,    ET_MOB, 150, 44, SND_VLOW, 15, SMELL_VLOW, 10, 40},
{"Ravager\0",        'r', 353, 40,  MT_RAVAGER, ET_MOB, 43,  18, SND_LOW,  7,  SMELL_LOW,  10, 40},
{"Grunt\0",          'z', 21,  11,  MT_GRUNT,   ET_MOB, 210, 89, SND_LOW,  10, SMELL_LOW,  10, 44},
{"Druid\0",          'd', 135, 42,  MT_DRUID,        ET_MOB,},
{"Sand Wurm\0",      'W', 3153,35,  MT_SANDWURM,     ET_MOB,},
{"Street Thug\0",    't',},
{"Beggar\0",         'b',},
{"Mercenary\0",      'm', 233, 78,  MT_MERC,         ET_MOB, 30,  41, SND_LOW,  10, SMELL_LOW,  10, 40},
{"Pistol Guy\0",     'p', 49,  18,  MT_PISTOL,       ET_MOB, 160, 95, SND_LOW,  10, SMELL_LOW,  10, 40},
{"Shotgun Dude\0",   's', 111, 55,  MT_SHOTTY,       ET_MOB, 57,  66, SND_LOW,  10, SMELL_LOW,  10, 40},
{"City Guard\0",     'c',},
{"Sharpshooter\0",   's',},
{"Heavy Gunner\0",   'g', 245, 69,  MT_GUNNER,       ET_MOB, 111, 59, SND_LOW,  10, SMELL_LOW,  10, 40},
{"Nomadic Warrior\0",'n', 689, 105, MT_NOMAD_WARRIOR, ET_MOB, 17,  9,  SND_LOW,  10, SMELL_LOW,  10, 40},
{"Nomadic Leader\0", 'l',}

};

// id, dmg, range, rng, spread, magsize
/*
* for shotguns, damage is damage per pellet maximum.
* for anything shooting bursts (besides the burst shotgun),
* the damage is for all the bullets, not per bullet
*/
std::vector<weapon_t> wpninfo = {
	{W_SHOTTY_SFP,  29,  12,  15,  10,  14},
	{W_SHOTTY_SFA,  25,  12,  15,   8,  12},
	{W_SHOTTY_FA,   21,  10,  15,  12,  20}
};
/*
std::vector<item_t> iteminfo = {
	{},
};
*/