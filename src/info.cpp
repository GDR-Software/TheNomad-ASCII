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
#include "s_mission.h"

scf::keybind_t scf::kb_binds[scf::NUMBINDS] = {
	{"Move Forward",   KEY_w,       scf::kbMove_n,     P_MoveN},
	{"Strafe Left",    KEY_a,       scf::kbStrafe_l,   P_MoveW},
	{"Move Backwards", KEY_s,       scf::kbMove_s,     P_MoveS},
	{"Strafe Right",   KEY_d,       scf::kbStrafe_r,   P_MoveE},
	{"Turn Left",      KEY_q,       scf::kbTurn_l,     P_ChangeDirL},
	{"Turn Right",     KEY_e,       scf::kbTurn_r,     P_ChangeDirR},
	{"Weapon 1",       KEY_1,       scf::kbSwapWpn_1,  P_ChangeWeapon1},
	{"Weapon 2",       KEY_2,       scf::kbSwapWpn_2,  P_ChangeWeapon2},
	{"Weapon 3",       KEY_3,       scf::kbSwapWpn_3,  P_ChangeWeapon3},
	{"Weapon 4",       KEY_4,       scf::kbSwapWpn_4,  P_ChangeWeapon4},
	{"Weapon 5",       KEY_5,       scf::kbSwapWpn_5,  P_ChangeWeapon5},
	{"Weapon 6",       KEY_6,       scf::kbSwapWpn_6,  P_ChangeWeapon6},
	{"Weapon 7",       KEY_7,       scf::kbSwapWpn_7,  P_ChangeWeapon7},
	{"Weapon 8",       KEY_8,       scf::kbSwapWpn_8,  P_ChangeWeapon8},
	{"Weapon 9",       KEY_9,       scf::kbSwapWpn_9,  P_ChangeWeapon9},
	{"Weapon 10",      KEY_0,       scf::kbSwapWpn_10, P_ChangeWeapon10},
	{"Dash North",     KEY_AT,      scf::kbDash_n,     P_DashN},
	{"Dash West",      KEY_EP,      scf::kbDash_w,     P_DashW},
	{"Dash South",     KEY_HASH,    scf::kbDash_s,     P_DashS},
	{"Dash East",      KEY_DOLLAR,  scf::kbDash_e,     P_DashE},
	{"Slide North",    KEY_W,       scf::kbSlide_n,    P_SlideN},
	{"Slide West",     KEY_A,       scf::kbSlide_w,    P_SlideW},
	{"Slide South",    KEY_S,       scf::kbSlide_s,    P_SlideS},
	{"Slide East",     KEY_D,       scf::kbSlide_e,    P_SlideE},
	{"Interact",       KEY_i,       scf::kbInteract,   P_Interact}
};

std::vector<npc_t> npcinfo = {
	{"The Bartender\0",    'b', 90,  49},
	{"Mercenary Master\0", 'M', 200, 50},
	{"Weapons Smith\0",    'w', 100, 43}
};

const entitystate_t pstateinfo[NUMPSTATES] = {
	{S_PLAYR_NULL, 0, 0, NULL},
	{S_PLAYR_SPAWN, 2, 0, NULL},
	{S_PLAYR_MOVE, 5, 0, NULL},
	{S_PLAYR_IDLE, 3, 0, NULL},
	{S_PLAYR_SHOOT, 6, 0, NULL},
	{S_PLAYR_MELEE, 8, 0, NULL},
	{S_PLAYR_INTERACT, 10, 0, NULL},
	{S_PLAYR_LOWHEALTH, 24, 0, NULL},
	{S_PLAYR_DEAD, ticrate_base, S_PLAYR_NULL, NULL},
	
	// mob states
	{S_HULK_NULL,              0,               S_HULK_SPAWN,         NULL}, // S_HULK_NULL
	{S_HULK_SPAWN,             5,               S_HULK_WANDER,        M_SpawnThink}, // S_HULK_SPAWN
	{S_HULK_WANDER,            ticrate_base*4,  S_HULK_IDLE,          M_WanderThink}, // S_HULK_WANDER
	{S_HULK_IDLE,              ticrate_base*3,  S_HULK_WANDER,        M_IdleThink}, // S_HULK_IDLE
	{S_HULK_CHASEPLAYR,        ticrate_base*5,  S_HULK_FIGHT,         M_ChasePlayr}, // S_HULK_CHASEPLAYR
	{S_HULK_FIGHT,             ticrate_base*2,  S_HULK_WANDER,        M_FightThink}, // S_HULK_FIGHT
	{S_HULK_FLEE,              ticrate_base*4,  S_HULK_WANDER,        M_FleeThink}, // S_HULK_FLEE
	{S_HULK_DEAD,              ticrate_base*10, S_HULK_NULL,          M_DeadThink}, // S_HULK_DEAD
	{S_RAV_NULL,               0,               S_RAV_SPAWN,          NULL}, // S_RAV_NULL
	{S_RAV_SPAWN,              5,               S_RAV_WANDER,         M_SpawnThink}, // S_RAV_SPAWN
	{S_RAV_WANDER,             ticrate_base,    S_RAV_IDLE,           M_WanderThink}, // S_RAV_WANDER
	{S_RAV_IDLE,               ticrate_base,    S_RAV_WANDER,         M_IdleThink}, // S_RAV_IDLE
	{S_RAV_CHASEPLAYR,         ticrate_base,    S_RAV_FIGHT,          M_ChasePlayr}, // S_RAV_CHASEPLAYR
	{S_RAV_FIGHT,              ticrate_base,    S_RAV_WANDER,         M_FightThink}, // S_RAV_FIGHT
	{S_RAV_FLEE,               ticrate_base,    S_RAV_WANDER,         M_FleeThink}, // S_RAV_FLEE
	{S_RAV_DEAD,               ticrate_base,    S_RAV_NULL,           M_DeadThink}, // S_RAV_DEAD
	{S_GRUNT_NULL,             0,               S_GRUNT_SPAWN,        NULL}, // S_GRUNT_NULL
	{S_GRUNT_SPAWN,            5,               S_GRUNT_WANDER,       M_SpawnThink}, // S_GRUNT_SPAWN
	{S_GRUNT_WANDER,           ticrate_base,    S_GRUNT_IDLE,         M_WanderThink}, // S_GRUNT_WANDER
	{S_GRUNT_IDLE,             ticrate_base,    S_GRUNT_WANDER,       M_IdleThink}, // S_GRUNT_IDLE
	{S_GRUNT_CHASEPLAYR,       ticrate_base,    S_GRUNT_FIGHT,        M_ChasePlayr}, // S_GRUNT_CHASEPLAYR
	{S_GRUNT_FIGHT,            ticrate_base,    S_GRUNT_WANDER,       M_FightThink}, // S_GRUNT_FIGHT
	{S_GRUNT_FLEE,             ticrate_base,    S_GRUNT_WANDER,       M_FleeThink}, // S_GRUNT_FLEE
	{S_GRUNT_DEAD,             ticrate_base,    S_GRUNT_NULL,         M_DeadThink}, // S_GRUNT_DEAD
	{S_DRUID_NULL,             0,               S_DRUID_SPAWN,        NULL}, // S_DRUID_NULL
	{S_DRUID_SPAWN,            5,               S_DRUID_WANDER,       M_SpawnThink}, // S_DRUID_SPAWN
	{S_DRUID_WANDER,           ticrate_base,    S_DRUID_IDLE,         M_WanderThink}, // S_DRUID_WANDER
	{S_DRUID_IDLE,             ticrate_base,    S_DRUID_WANDER,       M_IdleThink}, // S_DRUID_IDLE
	{S_DRUID_CHASEPLAYR,       ticrate_base,    S_DRUID_FIGHT,        M_ChasePlayr}, // S_DRUID_CHASEPLAYR
	{S_DRUID_FIGHT,            ticrate_base,    S_DRUID_WANDER,       M_FightThink}, // S_DRUID_FIGHT
	{S_DRUID_FLEE,             ticrate_base,    S_DRUID_WANDER,       M_FleeThink}, // S_DRUID_FLEE
	{S_DRUID_DEAD,             ticrate_base,    S_DRUID_NULL,         M_DeadThink}, // S_DRUID_DEAD
	{S_SANDWURM_NULL,          0,               S_SANDWURM_SPAWN,     NULL}, // S_RAV_NULL
	{S_SANDWURM_SPAWN,         5,               S_SANDWURM_WANDER,    M_SpawnThink}, // S_SANDWURM_SPAWN
	{S_SANDWURM_WANDER,        ticrate_base,    S_SANDWURM_IDLE,      M_WanderThink}, // S_SANDWURM_WANDER
	{S_SANDWURM_IDLE,          ticrate_base,    S_SANDWURM_WANDER,    M_IdleThink}, // S_SANDWURM_IDLE
	{S_SANDWURM_CHASEPLAYR,    ticrate_base,    S_SANDWURM_FIGHT,     M_ChasePlayr}, // S_SANDWURM_CHASEPLAYR
	{S_SANDWURM_FIGHT,         ticrate_base,    S_SANDWURM_WANDER,    M_FightThink}, // S_SANDWURM_FIGHT
	{S_SANDWURM_FLEE,          ticrate_base,    S_SANDWURM_WANDER,    M_FleeThink}, // S_SANDWURM_FLEE
	{S_SANDWURM_DEAD,          ticrate_base,    S_SANDWURM_NULL,      M_DeadThink}, // S_SANDWURM_DEAD
	{S_MERC_NULL,              0,               S_MERC_SPAWN,         NULL}, // S_MERC_NULL
	{S_MERC_SPAWN,             5,               S_MERC_WANDER,        M_SpawnThink}, // S_MERC_SPAWN
	{S_MERC_WANDER,            ticrate_base,    S_MERC_IDLE,          M_WanderThink}, // S_MERC_WANDER
	{S_MERC_IDLE,              ticrate_base,    S_MERC_WANDER,        M_IdleThink}, // S_MERC_IDLE
	{S_MERC_CHASEPLAYR,        ticrate_base,    S_MERC_FIGHT,         M_ChasePlayr}, // S_MERC_CHASEPLAYR
	{S_MERC_FIGHT,             ticrate_base,    S_MERC_WANDER,        M_FightThink}, // S_MERC_FIGHT
	{S_MERC_FLEE,              ticrate_base,    S_MERC_WANDER,        M_FleeThink}, // S_MERC_FLEE
	{S_MERC_DEAD,              ticrate_base,    S_MERC_NULL,          M_DeadThink}, // S_MERC_DEAD
	{S_MERC_LEADER_NULL,       0,               S_MERC_LEADER_SPAWN,  NULL}, // S_MERC_LEADER_NULL
	{S_MERC_LEADER_SPAWN,      5,               S_MERC_LEADER_WANDER, M_SpawnThink}, // S_MERC_LEADER_SPAWN
	{S_MERC_LEADER_WANDER,     ticrate_base,    S_MERC_LEADER_IDLE,   M_WanderThink}, // S_MERC_LEADER_WANDER
	{S_MERC_LEADER_IDLE,       ticrate_base,    S_MERC_LEADER_WANDER, M_IdleThink}, // S_MERC_LEADER_IDLE
	{S_MERC_LEADER_CHASEPLAYR, ticrate_base,    S_MERC_LEADER_FIGHT,  M_ChasePlayr}, // S_MERC_LEADER_CHASEPLAYR
	{S_MERC_LEADER_FIGHT,      ticrate_base,    S_MERC_LEADER_WANDER, M_FightThink}, // S_MERC_LEADER_FIGHT
	{S_MERC_LEADER_FLEE,       ticrate_base,    S_MERC_LEADER_WANDER, M_FleeThink}, // S_MERC_LEADER_FLEE
	{S_MERC_LEADER_DEAD,       ticrate_base,    S_MERC_LEADER_NULL,   M_DeadThink}, // S_MERC_LEADER_DEAD
	{S_PISTOL_NULL,            0,               S_PISTOL_SPAWN,       NULL}, // S_PISTOL_NULL
	{S_PISTOL_SPAWN,           5,               S_PISTOL_WANDER,      M_SpawnThink}, // S_PISTOL_SPAWN
	{S_PISTOL_WANDER,          ticrate_base,    S_PISTOL_IDLE,        M_WanderThink}, // S_PISTOL_WANDER
	{S_PISTOL_IDLE,            ticrate_base,    S_PISTOL_WANDER,      M_IdleThink}, // S_PISTOL_IDLE
	{S_PISTOL_CHASEPLAYR,      ticrate_base,    S_PISTOL_FIGHT,       M_ChasePlayr}, // S_PISTOL_CHASEPLAYR
	{S_PISTOL_FIGHT,           ticrate_base,    S_PISTOL_WANDER,      M_FightThink}, // S_PISTOL_FIGHT
	{S_PISTOL_FLEE,            ticrate_base,    S_PISTOL_WANDER,      M_FleeThink}, // S_PISTOL_FLEE
	{S_PISTOL_DEAD,            ticrate_base,    S_PISTOL_NULL,        M_DeadThink}, // S_PISTOL_DEAD
	{S_SHOTTY_NULL,            0,               S_SHOTTY_SPAWN,       NULL}, // S_SHOTTY_NULL
	{S_SHOTTY_SPAWN,           5,               S_SHOTTY_WANDER,      M_SpawnThink}, // S_SHOTTY_SPAWN
	{S_SHOTTY_WANDER,          ticrate_base,    S_SHOTTY_IDLE,        M_WanderThink}, // S_SHOTTY_WANDER
	{S_SHOTTY_IDLE,            ticrate_base,    S_SHOTTY_WANDER,      M_IdleThink}, // S_SHOTTY_IDLE
	{S_SHOTTY_CHASEPLAYR,      ticrate_base,    S_SHOTTY_FIGHT,       M_ChasePlayr}, // S_SHOTTY_CHASEPLAYR
	{S_SHOTTY_FIGHT,           ticrate_base,    S_SHOTTY_WANDER,      M_FightThink}, // S_SHOTTY_FIGHT
	{S_SHOTTY_FLEE,            ticrate_base,    S_SHOTTY_WANDER,      M_FleeThink}, // S_SHOTTY_FLEE
	{S_SHOTTY_DEAD,            ticrate_base,    S_SHOTTY_NULL,        M_DeadThink}, // S_SHOTTY_DEAD
	{S_GUNNER_NULL,            0,               S_GUNNER_SPAWN,       NULL}, // S_GUNNER_NULL
	{S_GUNNER_SPAWN,           5,               S_GUNNER_WANDER,      M_SpawnThink}, // S_GUNNER_SPAWN
	{S_GUNNER_WANDER,          ticrate_base,    S_GUNNER_IDLE,        M_WanderThink}, // S_GUNNER_WANDER
	{S_GUNNER_IDLE,            ticrate_base,    S_GUNNER_WANDER,      M_IdleThink}, // S_GUNNER_IDLE
	{S_GUNNER_CHASEPLAYR,      ticrate_base,    S_GUNNER_FIGHT,       M_ChasePlayr}, // S_GUNNER_CHASEPLAYR
	{S_GUNNER_FIGHT,           ticrate_base,    S_GUNNER_WANDER,      M_FightThink}, // S_GUNNER_FIGHT
	{S_GUNNER_FLEE,            ticrate_base,    S_GUNNER_WANDER,      M_FleeThink}, // S_GUNNER_FLEE
	{S_GUNNER_DEAD,            ticrate_base,    S_GUNNER_NULL,        M_DeadThink}, // S_GUNNER_DEAD
};

// name, sprite, health, armor, mtype, etype, rng, chance-to-spawn, sndtol, sndarea,
// smelltol, smellarea, sight range, hasmelee, hashitscan. hasprojectile, melee dmg,
// hitscan dmg, hitscan range, projectile dmg, projectile range, mlore, mdrops
const mobj_t mobinfo[NUMMOBS] = {
{
	"Hulk\0",
	'h',
	487,
	56,
	MT_HULK,
	S_HULK_NULL,
	ET_MOB,
	150,
	44,
	SND_VLOW,
	15,
	SMELL_VLOW,
	10,
	40
},
{
	"Ravager\0",
	'r',
	353,
	40,
	MT_RAVAGER,
	S_RAV_NULL,
	ET_MOB,
	43,
	18,
	SND_LOW,
	7,
	SMELL_LOW, 
	10,
	40
},
{
	"Grunt\0",
	'z',
	21,
	11,
	MT_GRUNT,
	S_GRUNT_NULL,
	ET_MOB,
	210,
	89,
	SND_LOW,
	10,
	SMELL_LOW,
	10,
	44
},
{
	"Druid\0",
	'd',
	135,
	42,
	MT_DRUID,
	S_DRUID_NULL,
	ET_MOB,
},
{
	"Sand Wurm\0",
	'W',
	3153,
	35,
	MT_SANDWURM,
	S_SANDWURM_NULL,
	ET_MOB,
},
{
	"Mercenary\0",
	'm',
	233,
	78,
	MT_MERC,
	S_MERC_NULL,
	ET_MOB,
	30,
	41,
	SND_LOW,
	10,
	SMELL_LOW,
	10,
	40
},
{
	"Pistol Guy\0",
	'p',
	49,
	18,
	MT_PISTOL,
	S_PISTOL_NULL,
	ET_MOB,
	160,
	95,
	SND_LOW,
	10,
	SMELL_LOW,
	10,
	40
},
{
	"Shotgun Dude\0",
	's',
	111,
	55,
	MT_SHOTTY,
	S_SHOTTY_NULL,
	ET_MOB,
	57,
	66,
	SND_LOW,
	10,
	SMELL_LOW,
	10,
	40
},
{
	"Heavy Gunner\0",
	'g',
	245,
	69,
	MT_GUNNER,
	S_GUNNER_NULL,
	ET_MOB,
	111,
	59,
	SND_LOW,
	10,
	SMELL_LOW,
	10,
	40
},
{
	"Nomadic Warrior\0",
	'n',
	389,
	79,
	MT_NOMAD_WARRIOR,
	S_NOMAD_WARRIOR_NULL,
	ET_MOB,
	17,
	9,
	SND_LOW,
	10,
	SMELL_LOW,
	10,
	40
},
{
	"Nomadic Leader\0",
	'l',
	644,
	96,
	MT_NOMAD_LEADER,
	S_NOMAD_LEADER_NULL,
	ET_MOB,
	8
}
};

/*
* mission type, rng, mob types, difficulty (min, max), bounty (min, max)
*
* the lower the rng, the easier the mission will be to complete, and the higher the harder
*
* the higher the mission is on the table, the more beginner-friendly the mission'll be, just so that we don't get an RNG lock
*/
const missionseed_t missionseeds[NUMMISSIONTYPES] = {
	{M_ASSASSINATION, 48, {MT_PISTOL, MT_SHOTTY, MT_GUNNER, MT_MERC}, {DIF_RECRUIT, DIF_MINORINCONVENIENCE}, {241, 3042}},
	{M_KIDNAPPING,    11, {MT_PISTOL, MT_SHOTTY, MT_GUNNER},                          {DIF_NOOB, DIF_NOMAD}, {13,   543}},
	{M_EXTORTION,     24, {MT_PISTOL, MT_SHOTTY, MT_GUNNER},                          {DIF_NOOB, DIF_NOMAD}, {153, 2424}},
	{M_EXTRACTION,    10, {MT_PISTOL},                                                {DIF_NOOB, DIF_NOMAD}, {153, 2424}},
	{M_BODYGUARDING,  10, {MT_PISTOL},                                                {DIF_NOOB, DIF_NOMAD}, {153, 2424}},
	{M_CONTRACT,      10, {MT_PISTOL},                                                {DIF_NOOB, DIF_NOMAD}, {153, 2424}},
	{M_INFILTRATION,  10, {MT_PISTOL},                                                {DIF_NOOB, DIF_NOMAD}, {153, 2424}},
};

const char* missionstrings[NUMMISSIONTYPES] = {
	"Assassination",
	"Kidnapping",
	"Extortion",
	"Extraction",
	"Body Guarding",
	"Contract",
	"Infiltration",
};

// name, id, dmg, range, rng, spread, magsize, numpellets (shotguns only)
/*
* for shotguns, damage is damage per pellet maximum.
* for anything shooting bursts (besides the burst shotgun),
* the damage is for all the bullets, not per bullet
*/
const weapon_t wpninfo[NUMWEAPONS] = {
	{SHOTTY_ADB_STRING,    W_SHOTTY_ADB,    SHOTTY_ADB_DAMAGE,    SHOTTY_ADB_RANGE,    SHOTTY_RNG,SHOTTY_ADB_SPREAD, SHOTTY_ADB_MAGSIZE,    SHOTTY_ADB_NUMPELLETS},
	{SHOTTY_FAB_STRING,    W_SHOTTY_FAB,    SHOTTY_FAB_DAMAGE,    SHOTTY_FAB_RANGE,    SHOTTY_RNG,SHOTTY_FAB_SPREAD, SHOTTY_FAB_MAGSIZE,    SHOTTY_FAB_NUMPELLETS},
	{SHOTTY_QS_STRING,     W_SHOTTY_QS,     SHOTTY_QS_DAMAGE,     SHOTTY_QS_RANGE,     SHOTTY_RNG,SHOTTY_QS_SPREAD,  SHOTTY_QS_MAGSIZE,     SHOTTY_QS_NUMPELLETS},
	{SIDE_AUP_STRING,      W_SIDE_AUP,      SIDE_AUP_DAMAGE,      SIDE_AUP_RANGE,      SIDE_RNG,  BURST4_SPREAD,     SIDE_AUP_MAGSIZE,      0},
	{SIDE_BOS_STRING,      W_SIDE_BOS,      SIDE_BOS_DAMAGE,      SIDE_BOS_RANGE,      SIDE_RNG,  SINGLE_SPREAD,     SIDE_BOS_MAGSIZE,      0},
	{SIDE_FUSION_STRING,   W_SIDE_FUSION,   SIDE_FUSION_DAMAGE,   SIDE_FUSION_RANGE,   SIDE_RNG,  BURST2_SPREAD,     SIDE_FUSION_MAGSIZE,   0},
	{SIDE_ION_STRING,      W_SIDE_ION,      SIDE_ION_DAMAGE,      SIDE_ION_RANGE,      SIDE_RNG,  SINGLE_SPREAD,     SIDE_ION_MAGSIZE,      0},
	{SIDE_PLASMA_STRING,   W_SIDE_PLASMA,   SIDE_PLASMA_DAMAGE,   SIDE_PLASMA_RANGE,   SIDE_RNG,  SINGLE_SPREAD,     SIDE_PLASMA_MAGSIZE,   0},
	{HSIDE_A8SHOT_STRING,  W_HSIDE_A8SHOT,  HSIDE_A8SHOT_DAMAGE,  HSIDE_A8SHOT_RANGE,  HSIDE_RNG, SHOTTY_QS_SPREAD,  HSIDE_A8SHOT_MAGSIZE,  1}, // slugs
	{HSIDE_SADB_STRING,    W_HSIDE_SADB,    HSIDE_SADB_DAMAGE,    HSIDE_SADB_RANGE,    HSIDE_RNG, SHOTTY_ADB_SPREAD, HSIDE_SADB_MAGSIZE,    4},
	{PRIM_M23C5_STRING,    W_PRIM_M23C5,    PRIM_M23C5_DAMAGE,    PRIM_M23C5_RANGE,    PRIM_RNG,  BURST3_SPREAD,     PRIM_M23C5_MAGSIZE,    0},
	{PRIM_AK77_STRING,     W_PRIM_AK77,     PRIM_AK77_DAMAGE,     PRIM_AK77_RANGE,     PRIM_RNG,  AUTO_SPREAD,       PRIM_AK77_MAGSIZE,     0},
	{PRIM_RAG15_STRING,    W_PRIM_RAG15,    PRIM_RAG15_DAMAGE,    PRIM_RAG15_RANGE,    PRIM_RNG,  BURST2_SPREAD,     PRIM_RAG15_MAGSIZE,    0},
	{PRIM_RAG14_STRING,    W_PRIM_RAG14,    PRIM_RAG14_DAMAGE,    PRIM_RAG14_RANGE,    PRIM_RNG,  AUTO_SPREAD,       PRIM_RAG14_MAGSIZE,    0},
	{PRIM_PLASMASMG_STRING,W_PRIM_PLASMASMG,PRIM_PLASMASMG_DAMAGE,PRIM_PLASMASMG_RANGE,PRIM_RNG,  BURST3_SPREAD,     PRIM_PLASMASMG_MAGSIZE,0},
	{HPRIM_FUSION_STRING,  W_HPRIM_FUSION,  HPRIM_FUSION_DAMAGE,  HPRIM_FUSION_RANGE,  HPRIM_RNG, SINGLE_SPREAD,     HPRIM_FUSION_MAGSIZE,  0},
	{HPRIM_HOSIG_STRING,   W_HPRIM_HOSIG,   HPRIM_HOSIG_DAMAGE,   HPRIM_HOSIG_RANGE,   HPRIM_RNG, SINGLE_SPREAD,     HPRIM_HOSIG_MAGSIZE,   0},
	{HPRIM_RAG13_STRING,   W_HPRIM_RAG13,   HPRIM_RAG13_DAMAGE,   HPRIM_RAG13_RANGE,   HPRIM_RNG, SINGLE_SPREAD,     HPRIM_RAG13_MAGSIZE,   0},
	{ARM_HB_STRING,        W_ARM_HB,        ARM_HB_DAMAGE,        ARM_HB_RANGE,        ARM_RNG,   BURST2_SPREAD,     0,                     0},
	{ARM_GRAPPLE_STRING,   W_ARM_GRAPPLE,   ARM_GRAPPLE_DAMAGE,   ARM_GRAPPLE_RANGE,   ARM_RNG,   SINGLE_SPREAD,     0,                     0}, // 6 shots till need a refill (TODO)
	{ARM_HC_STRING,        W_ARM_HC,        ARM_HC_DAMAGE,        ARM_HC_RANGE,        ARM_RNG,   SHOTTY_ADB_SPREAD, SHOTTY_ADB_MAGSIZE,    SHOTTY_ADB_NUMPELLETS}, // 2 shots then ticker
	{ARM_SB_STRING,        W_ARM_SB,        ARM_SB_DAMAGE,        ARM_SB_RANGE,        ARM_RNG,   BURST4_SPREAD,     1,                     0}, // 1 shot then ticker
	{ARM_FT_STRING,        W_ARM_FT,        ARM_FT_DAMAGE,        ARM_FT_RANGE,        ARM_RNG,   BURST4_SPREAD,     1,                     0}, // 1 shot then ticker
};

coord_t botpos[] = {
	{263, 283},
	{265, 252},
	{255, 256},
	{259, 283},
};

const item_t iteminfo[NUMITEMS] = {
	{"Small Health Pack",     },
	{"Medium Health Pack",    },
	{"Large Health Pack",     },
	{"Bandage",               },
	{"Street-Grade Armor",    },
	{"Militarty-Grade Armor", },
	{"Mercenary's Armor",     },
	{""},
};

const char* logosplash =
"                                    /-----------\\    __                                        \n"
"   <----|---->  |      |  |----->  /      |      \\   | \\   | /-------| |\\      /|     /| |===\\ \n"
"        |       |      |  |       |       |       |  |  \\  | |       | | \\    / |    / | |    |\n"
"        |       |------|  |---->  |       |       |      \\ | |       | |  \\  /  |   /--| |    |\n"
"        |       |      |  |        \\      |      /  /]    \\| |       | |   ==   |  /   | |   / \n"
"        |       |      |  |----->   \\-----|-----/  /       |  \\------| |        | /    | |==/  \n"
"                           <<------|------|--------|--->                                      \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                 --|------+--------|----^                                     \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                   |      |        |                                          \n"
"                                  -|------=----\\---|--                                        \n"
"                                   |      |        |                                          \n"
"                                   /      |                                                   \n"
"                                  /      /|\\                                                  \n"
"                                [/      \\ | /                                                 \n"
"                                        \\ | /                                                 \n"
"                                         \\*/                                                  \n";
const char* companysplash =
" /--------\\   |----\\    |-----\\        /--------\\         /--\\        |\\            /|  |=======>  /======> \n"
" |            |     \\   |      \\       |                 /    \\       | \\          / |  |          |        \n"
" |            |      |  |      |       |                /      \\      |  \\        /  |  |          |        \n"
" |   <-----\\  |      |  |------/       |   <-----\\     /========\\     |   \\      /   |  |=======>  \\======\\ \n"
" |        |   |      |  |     |        |        |     /          \\    |    \\    /    |  |                 | \n"
" |        |   |     /   |      \\       |        |    /            \\   |     \\  /     |  |                 | \n"
" \\--------/   |----/    |       \\      \\--------/   /              \\  |      \\/      |  |=======>  <======/ \n"
"                                                                                                            \n"
" 2021-2023                                                                                                  \n";
