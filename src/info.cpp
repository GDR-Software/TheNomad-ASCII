// this is where all the boiled-into-the-binaries takes place
#include "g_items.h"
#include "g_mob.h"
#include "scf.h"
#include "p_npc.h"
#include "g_obj.h"

const entitystate_t stateinfo[NUMSTATES] = {
	{S_PLAYR_NULL,      0},
	{S_PLAYR_SPAWN,     2},
	{S_PLAYR_MOVE,      5},
	{S_PLAYR_IDLE,      3},
	{S_PLAYR_SHOOT,     6},
	{S_PLAYR_MELEE,     8},
	{S_PLAYR_INTERACT,  10},
	{S_PLAYR_LOWHEALTH, 24},
	{S_PLAYR_DEAD,      16},
	{S_ZHULK_NULL,      0},
	{S_ZHULK_SPAWN,     90},
	{S_ZHULK_WANDER,    60},
	{S_ZHULK_IDLE,      50},
	{S_ZHULK_SNIFF,     60},
	{S_ZHULK_HEARPLAYR, 60},
	{S_ZHULK_CHASEPREY, 60},
	{S_ZHULK_FIGHT,     60},
	{S_ZHULK_RUNAWAY,   60},
	{S_ZHULK_DEAD,      15},
};

// name, sprite, health, armor, mtype, etype, rng, chance-to-spawn, sndtol, sndarea,
// smelltol, smellarea, sight range, hasmelee, hashitscan. hasprojectile, melee dmg,
// hitscan dmg, hitscan range, projectile dmg, projectile range, mlore, mdrops
const mobj_t mobinfo[NUMMOBS] = {

{"Zurgut Hulk",   'H', 487, 56,  MT_ZHULK,   ET_MOB, 150, },
{"Zurgut Ravager",'R', 353, 40,  MT_ZRAVAGER,ET_MOB, 43},
{"Zurgut Grunt",  'Z', 21,  11,  MT_ZGRUNT,  ET_MOB, 210},
{"Pistol Guy",    'P', 49,  18,  MT_PISTOL,  ET_MOB, 160},
{"Shotgun Dude",  'S', 111, 55,  MT_SHOTTY,  ET_MOB, 57},
{"Heavy Gunner",  'G', 245, 69,  MT_GUNNER,  ET_MOB, 111},
{"Mercenary",     'M', 233, 78,  MT_MERC,    ET_MOB, 30},
{"Nomad",         'N', 689, 105, MT_NOMAD,   ET_MOB, 17}

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

// forewarning: this array is really chunky, and you may find some interesting stuff here...
// also, since this is a free game, and only really the bff's are the payment, I've included some
// characters from fiction
npc_t npcinfo[9] = {
	{"Rinfur Hummain",    'R',  533, 24, {0, 0, 0, 0, 0}},
	{"Asturion Gurnovil", 'A',  543, 53, {0, 0, 0, 0, 0}},
	{"Ismalda Gurnovil",  'I',  453, 66, {0, 0, 0, 0, 0}},
	{"Farant Kirosla",    'F',   54, 10, {0, 0, 0, 0, 0}},
	// just cuz
	{"Darth Vader",       'D',  704, 76, {0, 0, 0, 0, 0}},
	// thanks ben
	{"Yukko The Clown",   'Y',  321, 14, {0, 0, 0, 0, 0}},
	// im a tolkien nerd at heart
	{"Sauron",            'S', 1094, 89, {0, 0, 0, 0, 0}},
	// ...
	{"Chizuru",           'C',  540, 34, {0, 0, 0, 0, 0}}
};