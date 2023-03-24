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
// DESCRIPTION: src/info.cpp
//  this is where all the boiled-into-the-binaries takes place
//----------------------------------------------------------
#include "n_shared.h"
#include "g_items.h"
#include "scf.h"
#include "g_zone.h"
#include "s_scripted.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"
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
	{"Civilian\0",         'c', 15,  8,  BOT_CIVILIAN},
	{"Guard\0",            'g', 40,  19, BOT_GUARD},
	{"Merchant\0",         't', 43,  29, BOT_MERCHANT},
	{"Mercenary Master\0", 'M', 200, 50, BOT_MERCMASTER},
	{"The Bartender\0",    'b', 90,  49, BOT_BARTENDER},
	{"Blacksmith\0",       'f', 78,  67, BOT_BLACKSMITH},
	// they're like rippers but more psychopathic and crazy
	{"Stitcher\0",         's', 65,  40, BOT_STITCHER},
	// stitchers who mess around with genes and genetics, named after my dearest Spino
	{"Splicer\0",          'S', 66,  39, BOT_SPLICER},
	{"Weapons Smith\0",    'w', 100, 43, BOT_WEAPONSMITH},
};

std::vector<entitystate_t> stateinfo = {
	{S_MOB_NULL,                          1,     S_MOB_SPAWN}, // S_MOB_NULL
	{S_MOB_SPAWN,            ticrate_base*3,      S_MOB_IDLE}, // S_MOB_SPAWN
	{S_MOB_WANDER,           ticrate_base*2,      S_MOB_IDLE}, // S_MOB_WANDER
	{S_MOB_IDLE,             ticrate_base*2,    S_MOB_WANDER}, // S_MOB_IDLE
	{S_MOB_CHASE,            ticrate_base*3,    S_MOB_WANDER}, // S_MOB_CHASE
	{S_MOB_FIGHT,            ticrate_base*2,     S_MOB_CHASE}, // S_MOB_FIGHT
	{S_MOB_FLEE,             ticrate_base*3,    S_MOB_WANDER}, // S_MOB_FLEE
	{S_MOB_DEAD,             ticrate_base*2,      S_MOB_NULL}, // S_MOB_DEAD
	{S_PLAYR_NULL,                        1,   S_PLAYR_SPAWN}, // S_PLAYR_NULL
	{S_PLAYR_SPAWN,                       2,    S_PLAYR_IDLE}, // S_PLAYR_SPAWN
	{S_PLAYR_MOVE,                        5,    S_PLAYR_IDLE}, // S_PLAYR_MOVE
	{S_PLAYR_DASH,           ticrate_base*2,    S_PLAYR_IDLE}, // S_PLAYR_DASH
	{S_PLAYR_IDLE,                        3,    S_PLAYR_IDLE}, // S_PLAYR_IDLE
	{S_PLAYR_SHOOT,                       6,    S_PLAYR_IDLE}, // S_PLAYR_SHOOT
	{S_PLAYR_MELEE,                       8,    S_PLAYR_IDLE}, // S_PLAYR_MELEE
	{S_PLAYR_INTERACT,                   10,    S_PLAYR_IDLE}, // S_PLAYR_INTERACT
	{S_PLAYR_LOWHEALTH,                  24,    S_PLAYR_IDLE}, // S_PLAYR_LOWHEALTH
	{S_PLAYR_DEAD,             ticrate_base,    S_PLAYR_NULL}, // S_PLAYR_DEAD
};


//static constexpr const char* hulk_lore;
//static constexpr const char* rav_lore;
//static constexpr const char* grunt_lore;


// name, sprite, health, armor, mtype, etype, rng, chance-to-spawn, sight range,
// hasmelee, hashitscan. hasprojectile, melee dmg, melee range, hitscan dmg,
// hitscan range, projectile dmg, projectile range, mlore, mdrops
mobj_t mobinfo[NUMMOBS] = {
{
	"Hulk\0",
	SPR(M_HULK_SPRITE),
	487,
	56,
	MT_HULK,
	ET_MOB,
	150,
	44,
	2, // kinda blind
	true,
	true,
	true,
	44,
	4,
	63,
	14,
	68,
	26,
	(const char *)NULL,
	{
		I_HEALTH_NORM
	}
},
{
	"Ravager\0",
	SPR(M_RAVAGER_SPRITE),
	353,
	40,
	MT_RAVAGER,
	ET_MOB,
	43,
	18,
	25,
	false,
	true,
	true,
	0,
	0,
	21,
	12,
	32,
	16,
	(const char *)NULL,
	{
		I_HEALTH_NORM
	}
},
{
	"Grunt\0",
	SPR(M_GRUNT_SPRITE),
	21,
	11,
	MT_GRUNT,
	ET_MOB,
	210,
	89,
	30,
	true,
	true,
	false,
	16,
	4,
	28,
	14,
	0,
	0,
	(const char *)NULL,
	{
		I_BANDAGE,
		I_FLAK
	}
},
{
	"Pistol Guy\0",
	SPR(M_PISTOL_SPRITE),
	24,
	18,
	MT_PISTOL,
	ET_MOB,
	160,
	95,
	23,
	true,
	true,
	false,
	5,
	1,
	7,
	12,
	0,
	0,
	(const char *)NULL,
	{
		I_BULLET_BOX,
		I_BULLET_PACK,
//		I_PRIM_RAG15
	}
},
{
	"Shotgun Dude\0",
	SPR(M_SHOTTY_SPRITE),
	36,
	55,
	MT_SHOTTY,
	ET_MOB,
	57,
	66,
	22,
	false,
	true,
	false,
	0,
	0,
	14,
	9,
	0,
	0,
	(const char *)NULL,
	{
//		I_SHOTTY_ADB,
		I_SHELL_BOX,
		I_SHELL_PACK,
//		I_SHOTTY_QS
	}
},
{
	"Heavy Gunner\0",
	SPR(M_GUNNER_SPRITE),
	245,
	69,
	MT_GUNNER,
	ET_MOB,
	111,
	59,
	44,
	false,
	true,
	true,
	0,
	0,
	22,
	34,
	46,
	14,
	(const char *)NULL,
	{
		I_BULLET_BOX,
		I_BULLET_PACK,
//		I_PRIM_AK77
	}
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

#define ID(x) #x

// cmd, name, id, dmg, range, rng, spread, magsize, numpellets (shotguns only)
/*
* for shotguns, damage is damage per pellet maximum.
* for anything shooting bursts (besides the burst shotgun),
* the damage is for all the bullets, not per bullet
*/
std::vector<weapon_t> wpninfo = {
{
	ID(W_SHOTTY_ADB),
	SHOTTY_ADB_STRING,
	W_SHOTTY_ADB,
	SHOTTY_ADB_DAMAGE,
	SHOTTY_ADB_RANGE,
	SHOTTY_RNG,
	SHOTTY_ADB_SPREAD,
	SHOTTY_ADB_MAGSIZE,   
	SHOTTY_ADB_NUMPELLETS,
	scf::sounds::sfx_adb_shot,
	scf::sounds::sfx_shotty_dryfire,
	AT_SHELL
},
{
	ID(W_SHOTTY_FAB),
	SHOTTY_FAB_STRING,
	W_SHOTTY_FAB,
	SHOTTY_FAB_DAMAGE,
	SHOTTY_FAB_RANGE,
	SHOTTY_RNG,
	SHOTTY_FAB_SPREAD,
	SHOTTY_FAB_MAGSIZE,
	SHOTTY_FAB_NUMPELLETS,
	scf::sounds::sfx_adb_shot,
	scf::sounds::sfx_shotty_dryfire,
	AT_SHELL
},
{
	ID(W_SHOTTY_QS),
	SHOTTY_QS_STRING,
	W_SHOTTY_QS,
	SHOTTY_QS_DAMAGE,
	SHOTTY_QS_RANGE,
	SHOTTY_RNG,
	SHOTTY_QS_SPREAD,
	SHOTTY_QS_MAGSIZE,
	SHOTTY_QS_NUMPELLETS,
	scf::sounds::sfx_adb_shot,
	scf::sounds::sfx_shotty_dryfire,
	AT_SHELL
},
{
	ID(W_SIDE_AUP),
	SIDE_AUP_STRING,
	W_SIDE_AUP,
	SIDE_AUP_DAMAGE,
	SIDE_AUP_RANGE,
	SIDE_RNG,
	BURST4_SPREAD,
	SIDE_AUP_MAGSIZE,
	3,
	scf::sounds::sfx_mpistol_shot,
	scf::sounds::sfx_pistol_dryfire,
	AT_BULLET
},
{
	ID(W_SIDE_BOS),
	SIDE_BOS_STRING,
	W_SIDE_BOS,
	SIDE_BOS_DAMAGE,
	SIDE_BOS_RANGE,
	SIDE_RNG,
	SINGLE_SPREAD,
	SIDE_BOS_MAGSIZE,
	1,
	scf::sounds::sfx_mpistol_shot,
	scf::sounds::sfx_pistol_dryfire,
	AT_BULLET
},
{
	ID(W_SIDE_FUSION),
	SIDE_FUSION_STRING,
	W_SIDE_FUSION,
	SIDE_FUSION_DAMAGE,
	SIDE_FUSION_RANGE,
	SIDE_RNG,
	BURST2_SPREAD,
	SIDE_FUSION_MAGSIZE,
	1,
	scf::sounds::sfx_mpistol_shot,
	scf::sounds::sfx_pistol_dryfire,
	AT_BULLET
},
{
	ID(W_SIDE_ION),
	SIDE_ION_STRING,
	W_SIDE_ION,
	SIDE_ION_DAMAGE,
	SIDE_ION_RANGE,
	SIDE_RNG,
	SINGLE_SPREAD,
	SIDE_ION_MAGSIZE,
	1,
	scf::sounds::sfx_mpistol_shot,
	scf::sounds::sfx_pistol_dryfire,
	AT_BULLET
},
{
	ID(W_SIDE_PLASMA),
	SIDE_PLASMA_STRING,
	W_SIDE_PLASMA,
	SIDE_PLASMA_DAMAGE,
	SIDE_PLASMA_RANGE,
	SIDE_RNG,
	SINGLE_SPREAD,
	SIDE_PLASMA_MAGSIZE,
	1,
	scf::sounds::sfx_mpistol_shot,
	scf::sounds::sfx_pistol_dryfire,
	AT_BULLET
},
{
	ID(W_HSIDE_A8SHOT),
	HSIDE_A8SHOT_STRING,
	W_HSIDE_A8SHOT,
	HSIDE_A8SHOT_DAMAGE,
	HSIDE_A8SHOT_RANGE,
	HSIDE_RNG,
	SHOTTY_QS_SPREAD, 
	HSIDE_A8SHOT_MAGSIZE,
	1, // slugs
	scf::sounds::sfx_mshotty_fight,
	scf::sounds::sfx_shotty_dryfire,
	AT_SHELL
},
{
	ID(W_HSIDE_SADB),
	HSIDE_SADB_STRING,
	W_HSIDE_SADB,
	HSIDE_SADB_DAMAGE,
	HSIDE_SADB_RANGE,
	HSIDE_RNG,
	SHOTTY_ADB_SPREAD,
	HSIDE_SADB_MAGSIZE,
	2,
	scf::sounds::sfx_adb_shot,
	scf::sounds::sfx_shotty_dryfire,
	AT_SHELL
},
{
	ID(W_PRIM_M23C5),
	PRIM_M23C5_STRING,
	W_PRIM_M23C5,
	PRIM_M23C5_DAMAGE,
	PRIM_M23C5_RANGE,
	PRIM_RNG,
	BURST3_SPREAD,
	PRIM_M23C5_MAGSIZE,
	1,
	scf::sounds::sfx_rifle_shot,
	scf::sounds::sfx_rifle_dryfire,
	AT_BULLET
},
{
	ID(W_PRIM_AK77),
	PRIM_AK77_STRING,
	W_PRIM_AK77,
	PRIM_AK77_DAMAGE,
	PRIM_AK77_RANGE,
	PRIM_RNG,
	AUTO_SPREAD,
	PRIM_AK77_MAGSIZE,
	1,
	scf::sounds::sfx_rifle_shot,
	scf::sounds::sfx_rifle_dryfire,
	AT_BULLET
},
{
	ID(W_PRIM_RAG15),
	PRIM_RAG15_STRING,
	W_PRIM_RAG15,
	PRIM_RAG15_DAMAGE,
	PRIM_RAG15_RANGE,
	PRIM_RNG,
	BURST2_SPREAD,
	PRIM_RAG15_MAGSIZE,
	4,
	scf::sounds::sfx_rifle_shot,
	scf::sounds::sfx_rifle_dryfire,
	AT_BULLET
},
{
	ID(W_PRIM_RAG14),
	PRIM_RAG14_STRING,
	W_PRIM_RAG14,
	PRIM_RAG14_DAMAGE,
	PRIM_RAG14_RANGE,
	PRIM_RNG,
	AUTO_SPREAD,
	PRIM_RAG14_MAGSIZE,
	3,
	scf::sounds::sfx_rifle_shot,
	scf::sounds::sfx_rifle_dryfire,
	AT_BULLET
},
{
	ID(W_PRIM_PLASMASMG),
	PRIM_PLASMASMG_STRING,
	W_PRIM_PLASMASMG,
	PRIM_PLASMASMG_DAMAGE,
	PRIM_PLASMASMG_RANGE,
	PRIM_RNG,
	BURST3_SPREAD,
	PRIM_PLASMASMG_MAGSIZE,
	0,
	"NONE",
	"NONE",
	AT_PLASMA
},
{
	ID(W_HPRIM_FUSION),
	HPRIM_FUSION_STRING,
	W_HPRIM_FUSION,
	HPRIM_FUSION_DAMAGE,
	HPRIM_FUSION_RANGE,
	HPRIM_RNG,
	SINGLE_SPREAD,
	HPRIM_FUSION_MAGSIZE,
	0,
	"NONE",
	"NONE",
	AT_FUSION
},
{
	ID(W_HPRIM_HOSIG),
	HPRIM_HOSIG_STRING,
	W_HPRIM_HOSIG,
	HPRIM_HOSIG_DAMAGE,
	HPRIM_HOSIG_RANGE,
	HPRIM_RNG,
	SINGLE_SPREAD,
	HPRIM_HOSIG_MAGSIZE,
	0,
	"NONE",
	"NONE",
	AT_BULLET
},
{
	ID(W_HPRIM_RAG13),
	HPRIM_RAG13_STRING,
	W_HPRIM_RAG13,
	HPRIM_RAG13_DAMAGE,
	HPRIM_RAG13_RANGE,
	HPRIM_RNG,
	SINGLE_SPREAD,
	HPRIM_RAG13_MAGSIZE,
	0,
	"NONE",
	"NONE",
	AT_BULLET
},
{
	ID(W_ARM_HB),
	ARM_HB_STRING,
	W_ARM_HB,
	ARM_HB_DAMAGE,
	ARM_HB_RANGE,
	ARM_RNG,
	BURST2_SPREAD,
	0,
	0,
	"NONE",
	"NONE",
	AT_NULL
},
{
	ID(W_ARM_GRAPPLE),
	ARM_GRAPPLE_STRING,
	W_ARM_GRAPPLE,
	ARM_GRAPPLE_DAMAGE,
	ARM_GRAPPLE_RANGE,
	ARM_RNG,
	SINGLE_SPREAD,
	0,
	0,
	"NONE",
	"NONE",
	AT_NULL
}, // 6 shots till need a refill (TODO)
{
	ID(W_ARM_HC),
	ARM_HC_STRING,
	W_ARM_HC,
	ARM_HC_DAMAGE,
	ARM_HC_RANGE,
	ARM_RNG,
	SHOTTY_ADB_SPREAD,
	SHOTTY_ADB_MAGSIZE,
	SHOTTY_ADB_NUMPELLETS,
	"NONE",
	"NONE",
	AT_NULL
}, // 2 shots then ticker
{
	ID(W_ARM_SB),
	ARM_SB_STRING,
	W_ARM_SB,
	ARM_SB_DAMAGE,
	ARM_SB_RANGE,
	ARM_RNG,
	BURST4_SPREAD,
	1,
	0,
	"NONE",
	"NONE",
	AT_NULL
}, // 1 shot then ticker
{
	ID(W_ARM_FT),
	ARM_FT_STRING,
	W_ARM_FT,
	ARM_FT_DAMAGE,
	ARM_FT_RANGE,
	ARM_RNG,
	BURST4_SPREAD,
	1,
	0,
	"NONE",
	"NONE",
	AT_NULL
}, // 1 shot then ticker
};

coord_t botpos[] = {
	{263, 283},
	{265, 252},
	{255, 256},
	{259, 283},
};

// name, id, weight, cost
std::vector<item_t> iteminfo = {
	{"Small Health Pack",     I_HEALTH_SMALL,   6,  13, 0, '!'},
	{"Medium Health Pack",    I_HEALTH_NORM,    11, 45, 0, '!'},
	{"Large Health Pack",     I_HEALTH_LARGE,   16, 88, 0, '!'},
	{"Bandage",               I_BANDAGE,        4,  8,  0, '!'},
	{"Street-Grade Armor",    I_ARMOR_STREET,   12, 5,  0, '!'},
	{"Militarty-Grade Armor", I_ARMOR_MILITARY, 24, 26, 0, '!'},
	{"Mercenary's Armor",     I_ARMOR_MERC,     39, 69, 0, '!'},
	{"Piece of Flak",         I_FLAK,           9,  5,  0, '!'},
	{"Shotgun Shells (6)",    I_SHELL_PACK,     4,  16, 0, '!'},
	{"Bullets (15)",          I_BULLET_PACK,    6,  13, 0, '!'},
	{"Box of Shotgun Shells", I_SHELL_BOX,      16, 54, 0, '!'},
	{"Box of Bullets",        I_BULLET_BOX,     18, 43, 0, '!'},
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

const char* about_str =
"The Nomad-ASCII has been a project in the making for a long time now, and its been a hard game to,\n"
"develop. The game's idea originally came from fan-fiction after I played Modern Warfare 2's Remastered\n"
"Campaign for the second time around, it used to take place in right about 2020-2080, but then during\n"
"the development I watched and feel in love with Mad Max: Fury Road. I changed up the setting to fit\n"
"more of a desert-planet style of post-apocalypse, but when I was doing this, I thought: \"How can I\n"
"use the already developed content?\", and that right there is exactly how the notorious mercenary guilds\n"
"of Bellatum Terrae were born.\n"
"\nHave fun,\nYour Resident Fiend, Noah Van Til\n\n(IN COLLABORATION WITH GDR GAMES)\n";
const char* credits_str =
"\n"
"That one piano piece you'll hear often: alpecagrenade\n"
"Programming: Noah Van Til (and most of the music as well)\n"
"Concept Artists & Ideas Contributers: Cooper & Tucker Kemnitz\n"
"A Few of the Guns: Ben Pavlovic\n"
"\n";