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
// DESCRIPTION: src/p_playrs.cpp
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_obj.h"
#include "g_zone.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_items.h"
#include "s_world.h"
#include "g_map.h"
#include "s_scripted.h"
#include "g_playr.h"
#include "g_game.h"

static Playr* playr;
static Game* game;

// pauses all the other threads
static void CommandConsole(Game* const game);

// random interaction code/functions
static void P_ChairInteract(nomadint_t input)
{
	Hud_Printf("System", "There is a chair there, would you like to sit down? [y/n]");
	nomadshort_t in = wgetch(game->screen);
	if (in != 'y') {
		Hud_Printf("System", "Well, that's a shame, see you later");
		return;
	}
	Hud_Printf("System", "You are now sitting");
	playr->pmode = P_MODE_SITTING;
	switch (input) {
	case KEY_w:
		--playr->pos.y;
		break;
	case KEY_a:
		--playr->pos.x;
		break;
	case KEY_s:
		++playr->pos.y;
		break;
	case KEY_d:
		++playr->pos.x;
		break;
	default:
		N_Error("Player-To-Chair Interaction Called, But Movement Vector Is Invalid");
		break;
	};
}
static void P_DoorInteract(nomadint_t input)
{
	Hud_Printf("System", "There's a door in your way, would you like to open it? [y/n]");
	nomadshort_t in = wgetch(game->screen);
	if (in != 'y') {
		Hud_Printf("System", "Ok then, how're you going to get into that building?");
		return;
	}
	// TODO: add in locked doors
	Hud_Printf("System", "You successfully opened the door, bravo");
	switch (input) {
	case KEY_w:
		--playr->pos.y;
		break;
	case KEY_a:
		--playr->pos.x;
		break;
	case KEY_s:
		++playr->pos.y;
		break;
	case KEY_d:
		++playr->pos.x;
		break;
	default:
		N_Error("Player-To-Door Interaction Called, But Movement Vector Is Invalid");
		break;
	};
}

#define ITEM_AMMO   0
#define ITEM_WEAPON 1
#define ITEM_HEALTH 2
#define ITEM_ARMOR  3

static inline nomadbool_t P_HasWpn(nomaduint_t id)
{
	for (const auto& i : playr->P_wpns) {
		if (i.c_wpn.id == id) {
			return true;
		}
	}
	return false;
}

static void P_ItemInteract(nomadint_t input)
{
	nomadbool_t done = false;
	item_t *ptr = NULL;
	linked_list<item_t*>::iterator i;
	for (i = game->items.begin(); i != game->items.end(); i = i->next) {
		item_t *it = i->val;
		if (input == KEY_w && game->c_map[playr->pos.y - 1][playr->pos.x] == '!') {
			ptr = it;
			break;
		}
		else if (input == KEY_a && game->c_map[playr->pos.y][playr->pos.x - 1] == '!') {
			ptr = it;
			break;
		}
		else if (input == KEY_s && game->c_map[playr->pos.y + 1][playr->pos.x] == '!') {
			ptr = it;
			break;
		}
		else if (input == KEY_d && game->c_map[playr->pos.y][playr->pos.x + 1] == '!') {
			ptr = it;
			break;
		}
	}
	if (!ptr) {
		LOG_WARN("P_ItemInteract called but could not pair item_t* with an element in game->items, aborting");
		return;
	}
	if (ptr->wpn != NULL) {
		switch (ptr->wpn->id) {
		case W_SHOTTY_ADB:
			if (P_HasWpn(ptr->wpn->id))
				playr->ammunition[AT_SHELL] += 8;
			playr->ammunition[AT_SHELL] = playr->ammunition[AT_SHELL] > PLAYR_SHELL_MAX ? PLAYR_SHELL_MAX : playr->ammunition[AT_SHELL];
			break;
		case W_SHOTTY_FAB:
			if (P_HasWpn(ptr->wpn->id))
				playr->ammunition[AT_SHELL] += 12;
			playr->ammunition[AT_SHELL] = playr->ammunition[AT_SHELL] > PLAYR_SHELL_MAX ? PLAYR_SHELL_MAX : playr->ammunition[AT_SHELL];
			break;
		case W_SHOTTY_QS:
			if (P_HasWpn(ptr->wpn->id))
				playr->ammunition[AT_SHELL] += 16;
			playr->ammunition[AT_SHELL] = playr->ammunition[AT_SHELL] > PLAYR_SHELL_MAX ? PLAYR_SHELL_MAX : playr->ammunition[AT_SHELL];
			break;
		case W_PRIM_AK77:
			if (P_HasWpn(ptr->wpn->id))
				playr->ammunition[AT_BULLET] += 30;
			playr->ammunition[AT_BULLET] = playr->ammunition[AT_BULLET] > PLAYR_BULLET_MAX ? PLAYR_BULLET_MAX : playr->ammunition[AT_BULLET];
			break;
		case W_PRIM_M23C5:
			if (P_HasWpn(ptr->wpn->id))
				playr->ammunition[AT_BULLET] += 36;
			playr->ammunition[AT_BULLET] = playr->ammunition[AT_BULLET] > PLAYR_BULLET_MAX ? PLAYR_BULLET_MAX : playr->ammunition[AT_BULLET];
			break;
		};
	}
	else {
		switch (ptr->item_id) {
		case I_SHELL_BOX:
			Hud_Printf("System", "You picked up a Box of Shotgun Shells");
			playr->ammunition[AT_SHELL] += SHELL_BOX_NUM;
			playr->ammunition[AT_SHELL] =
				playr->ammunition[AT_SHELL] > PLAYR_SHELL_MAX ? PLAYR_SHELL_MAX : playr->ammunition[AT_SHELL];
			break;
		case I_SHELL_PACK:
			Hud_Printf("System", "You picked up a Pack of Shotgun Shells");
			playr->ammunition[AT_SHELL] += SHELL_PACK_NUM;
			playr->ammunition[AT_SHELL] =
				playr->ammunition[AT_SHELL] > PLAYR_SHELL_MAX ? PLAYR_SHELL_MAX : playr->ammunition[AT_SHELL];
			break;
		case I_BULLET_BOX:
			Hud_Printf("System", "You picked up a Box of Bullets");
			playr->ammunition[AT_BULLET] += BULLET_BOX_NUM;
			playr->ammunition[AT_BULLET] =
				playr->ammunition[AT_BULLET] > PLAYR_BULLET_MAX ? PLAYR_BULLET_MAX : playr->ammunition[AT_BULLET];
			break;
		case I_BULLET_PACK:
			Hud_Printf("System", "You picked up a Pack of Bullets");
			playr->ammunition[AT_BULLET] += BULLET_PACK_NUM;
			playr->ammunition[AT_BULLET] =
				playr->ammunition[AT_BULLET] > PLAYR_BULLET_MAX ? PLAYR_BULLET_MAX : playr->ammunition[AT_BULLET];
			break;
		case I_BANDAGE:
			Hud_Printf("System", "You picked up a Bandage");
			playr->health += 10;
			playr->health = playr->health > 100 ? 100 : playr->health;
			break;
		case I_HEALTH_SMALL:
			Hud_Printf("System", "You picked up a Small Health Pack");
			playr->health += 20;
			playr->health = playr->health > 100 ? 100 : playr->health;
			break;
		case I_HEALTH_NORM:
			Hud_Printf("System", "You picked up a Health Pack");
			playr->health += 45;
			playr->health = playr->health > 100 ? 100 : playr->health;
			break;
		case I_HEALTH_LARGE:
			Hud_Printf("System", "You picked up a Large Health Pack");
			playr->health += 75;
			playr->health = playr->health > 100 ? 100 : playr->health;
			break;
		case I_ARMOR_STREET:
			Hud_Printf("System", "You picked up Street-Grade Armor");
			playr->armor = ARMOR_STREET;
			break;
		case I_ARMOR_MILITARY:
			Hud_Printf("System", "You picked up Military-Grade Armor");
			playr->armor = ARMOR_MILITARY;
			break;
		case I_ARMOR_MERC:
			Hud_Printf("System", "You picked up Mercenary-Grade Armor");
			playr->armor = ARMOR_MERC;
			break;
		};
	}
	char mapbuffer[9][120][120];
	N_memset(mapbuffer, 0, sizeof(mapbuffer));
	N_memcpy(mapbuffer[8], game->lvlptr->lvl_map, sizeof(game->lvlptr->lvl_map));
	for (nomaduint_t y = 0; y < 120; ++y) {
		for (nomaduint_t x = 0; x < 120; ++x) {
			if (mapbuffer[8][y][x] == '!') {
				mapbuffer[8][y][x] = '.';
				goto done;
			}
		}
	}
done:
	nomaduint_t y{}, x{};
	FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	NOMAD_ASSERT(fp, "failed to open Files/gamedata/RUNTIME/mapfile.txt!");
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) { fprintf(fp, "#"); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[0][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[7][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[6][y][x]); }
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) { fprintf(fp, "#"); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[1][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[8][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[5][y][x]); }
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[2][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[3][y][x]); }
		for (x = 0; x < SECTOR_MAX_X; ++x) { fprintf(fp, "%c", mapbuffer[4][y][x]); }
		for (x = 0; x < 80; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
    for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X+160; ++x) { fprintf(fp, "#"); }
		fprintf(fp, "\n");
	}
    fclose(fp);
}
static void P_RugInteract();
static void P_BedInteract();
static void P_ChestInteract();
static void P_DeskInteract();

void PlayrAssigner(Game* const gptr)
{
	game = gptr;
}

void Playr::P_Init()
{
	LOG_INFO("initializing player stats");
	playr = this;
	CombatAssigner(game);
	ItemAssigner(game);
//	name = "Test";
	sprite = SPR('@');
	health = 100;
	armor = ARMOR_STREET;
	pdir = D_NORTH;
	lvl = 0;
	coin = 0;
	pos = {0, 0};
	sector_id = 0;
	pstate = stateinfo[S_PLAYR_NULL];
	pticker = 0;
	
	pmode = P_MODE_ROAMING;
	wpn_slot_current = 1;
	p_rightarm.c_wpn = wpninfo[W_ARM_HB];
    p_leftarm.c_wpn = wpninfo[W_ARM_GRAPPLE];
    p_sidearm.c_wpn = wpninfo[W_SIDE_PLASMA];
    p_heavyside.c_wpn = wpninfo[W_HSIDE_SADB];
    p_primary.c_wpn = wpninfo[W_PRIM_AK77];
    p_heavyprimary.c_wpn = wpninfo[W_HPRIM_RAG13];
    p_shotty.c_wpn = wpninfo[W_SHOTTY_ADB];
	c_wpn = &p_shotty;
	ammunition[AT_SHELL] = PLAYR_SHELL_MAX;
	ammunition[AT_BULLET] = PLAYR_BULLET_MAX;
	ammunition[AT_PLASMA] = 0;
	ammunition[AT_FUSION] = 0;
	N_memset(&body_health.front(), 100, body_health.size());
	for (auto& i : inv) {
		i.name = (const char*)NULL;  
	}
}

void P_Interact()
{

}

void P_UseLeftArm()
{

}
void P_UseRightArm()
{
	
}
void P_ShowWeapons()
{

}

void P_PauseMenu()
{
	game->gamestate = GS_PAUSE;
	game->gamescreen = MENU_PAUSE;
}

void P_UseWeapon()
{
	LOG_PROFILE();
	switch (playr->c_wpn->c_wpn.id) {
	case W_SHOTTY_ADB:
	case W_SHOTTY_FAB:
	case W_SHOTTY_QS:
		P_ShootShotty(playr->c_wpn);
//		Hud_Printf("System", "You shoot your shotgun");
		break;
	case W_HPRIM_FUSION:
	case W_HPRIM_HOSIG:
	case W_HPRIM_RAG13:
	case W_HPRIM_DR:
	case W_PRIM_AK77:
	case W_PRIM_M23C5:
		P_ShootSingle(playr->c_wpn);
		break;
	default: {
		LOG_WARN("P_UseWeapon unknown/invalid weapon id %hu, returning without using weapon", playr->c_wpn->c_wpn.id);
		break; }
	};
}


static boost::mutex main_mutex;
void Game::P_Ticker(nomadint_t input)
{
	boost::unique_lock<boost::mutex> lock{main_mutex};
	--playr->pticker;
//	playr->P_GetMode();
	playr->P_RunTicker(input);
	lock.unlock();
}

static nomadint_t input;

void ctrl_c_handle(int signum)
{
	signal(SIGINT, ctrl_c_handle);
	input = CTRL_c;
}

void Playr::P_RunTicker(nomadint_t finput)
{
	input = finput;
	for (const auto& i : scf::kb_binds) {
		if (input == i.button) {
			(*i.actionp)();
			break;
		}
	}
}

void P_SlideN()
{
	P_DashN();
}
void P_SlideW()
{
	P_DashW();
}
void P_SlideS()
{
	P_DashS();
}
void P_SlideE()
{
	P_DashE();
}

void P_ChangeWeapon1()
{
	playr->c_wpn = &rightarm;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon2()
{
	playr->c_wpn = &leftarm;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon3()
{
	playr->c_wpn = &sidearm;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon4()
{
	playr->c_wpn = &heavyside;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon5()
{
	playr->c_wpn = &primary;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon6()
{
	playr->c_wpn = &heavyprimary;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon7()
{
	playr->c_wpn = &shotty;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon8()
{
	playr->c_wpn = &melee1;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon9()
{
	playr->c_wpn = &melee2;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}
void P_ChangeWeapon10()
{
	playr->c_wpn = &melee3;
	wmove(game->screen, 9, 97);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwaddch(game->screen, 9, 128, '#');
}

void Playr::P_GetMode()
{
	coord_t mercpos = botpos[0];
	if ((pos.y == (mercpos.y - 1) && pos.x == mercpos.x)
	|| (pos.x == (mercpos.x + 1) && pos.y == mercpos.y)) {
		pmode = P_MODE_MERCMASTER;
	}
	else {
		pmode = P_MODE_ROAMING;
	}
}

void P_MoveN()
{
	switch (game->c_map[playr->pos.y - 1][playr->pos.x]) {
	case '_':
		P_DoorInteract(input);
		break;
	case '!':
		P_ItemInteract(input);
		break;
	case ' ':
	case '.':
		--playr->pos.y;
		break;
	case ':': { // a chair/seat sprite
		if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
		else { --playr->pos.y; playr->pmode = P_MODE_ROAMING; }
		break; }
	default:
		break;
	};
}
void P_MoveW()
{
	switch (game->c_map[playr->pos.y][playr->pos.x - 1]) {
	case '_':
		P_DoorInteract(input);
		break;
	case '!':
		P_ItemInteract(input);
		break;
	case '.':
	case ' ':
		--playr->pos.x;
		break;
	case ':': { // a chair/seat sprite
		if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
		else { --playr->pos.x; playr->pmode = P_MODE_ROAMING; }
		break; }
	default:
		break;
	};
}
void P_MoveS()
{
	switch (game->c_map[playr->pos.y + 1][playr->pos.x]) {
	case '_':
		P_DoorInteract(input);
		break;
	case '!':
		P_ItemInteract(input);
		break;
	case '.':
	case ' ':
		++playr->pos.y;
		break;
	case ':': { // a chair/seat sprite
		if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
		else { ++playr->pos.y; playr->pmode = P_MODE_ROAMING; }
		break; }
	default:
		break;
	};
}
void P_MoveE()
{
	switch (game->c_map[playr->pos.y][playr->pos.x + 1]) {
	case '_':
		P_DoorInteract(input);
		break;
	case '!':
		P_ItemInteract(input);
		break;
	case '.':
	case ' ':
		++playr->pos.x;
		break;
	case ':': { // a chair/seat sprite
		if (playr->pmode != P_MODE_SITTING) { P_ChairInteract(input); }
		else { ++playr->pos.x; playr->pmode = P_MODE_ROAMING; }
		break; }
	default:
		break;
	};
}

static nomadbool_t P_CanMove(coord_t& pos)
{
	switch (game->c_map[playr->pos.y+pos.y][playr->pos.x+pos.x]) {
	case '.':
		return true;
		break;
	case ' ':
		return true;
		break;
	case '!':
		P_ItemInteract(input);
		return false;
		break;
	default:
		return false;
		break;
	};
	return false;
}

static inline nomadenum_t P_GetInputDir()
{
	switch (input) {
	case KEY_AT:	return D_NORTH;
	case KEY_EP:	return D_WEST;
	case KEY_HASH:	return D_SOUTH;
	case KEY_DOLLAR:return D_EAST;
	};
	return D_NULL;
}

void P_DashN()
{
//	nomadshort_t range = playr->pos.y - RDASH_SPEED;
	nomadenum_t dir = P_GetInputDir();
	coord_t pos = game->E_GetDir(dir);
	if (!P_CanMove(pos))
		return;
	for (nomadshort_t i = 0; i < RDASH_SPEED; ++i) {
		if (!P_CanMove(pos))
			break;
		else
			--playr->pos.y;
	}
	playr->pstate = stateinfo[S_PLAYR_DASH];
	playr->pticker = playr->pstate.numticks;
}
void P_DashW()
{
//	nomadshort_t range = playr->pos.x - RDASH_SPEED;
	nomadenum_t dir = P_GetInputDir();
	coord_t pos = game->E_GetDir(dir);
	if (!P_CanMove(pos))
		return;
	for (nomadshort_t i = 0; i < RDASH_SPEED; ++i) {
		if (!P_CanMove(pos))
			break;
		else
			--playr->pos.x;
	}
	playr->pstate = stateinfo[S_PLAYR_DASH];
	playr->pticker = playr->pstate.numticks;
}
void P_DashS()
{
//	nomadshort_t range = playr->pos.y - RDASH_SPEED;
	nomadenum_t dir = P_GetInputDir();
	coord_t pos = game->E_GetDir(dir);
	if (!P_CanMove(pos))
		return;
	for (nomadshort_t i = 0; i < RDASH_SPEED; ++i) {
		if (!P_CanMove(pos))
			break;
		else
			++playr->pos.y;
	}
	playr->pstate = stateinfo[S_PLAYR_DASH];
	playr->pticker = playr->pstate.numticks;
}
void P_DashE()
{
//	nomadshort_t range = playr->pos.y - RDASH_SPEED;
	nomadenum_t dir = P_GetInputDir();
	coord_t pos = game->E_GetDir(dir);
	if (!P_CanMove(pos))
		return;
	for (nomadshort_t i = 0; i < RDASH_SPEED; ++i) {
		if (!P_CanMove(pos))
			break;
		else
			++playr->pos.x;
	}
	playr->pstate = stateinfo[S_PLAYR_DASH];
	playr->pticker = playr->pstate.numticks;
}

void P_ChangeDirL()
{
	if (playr->pdir > NUMDIRS) {
		LOG_WARN("P_ChangeDirL playr->pdir > NUMDIRS! assigning it to D_NORTH");
		playr->pdir = D_NORTH;
	}
	for (nomadshort_t y = 1; y < 4; ++y) {
		for (nomadshort_t x = 1; x < 6; ++x) {
			mvwaddch(game->screen, y, x, ' ');
		}
	}
	if (playr->pdir == D_EAST) {
		playr->pdir = D_NORTH;
	}
	else {
		++playr->pdir;
	}
}

void P_ChangeDirR()
{
	if (playr->pdir > NUMDIRS) {
		LOG_WARN("P_ChangeDirR playr->pdir > NUMDIRS! assigning it to D_NORTH");
		playr->pdir = D_NORTH;
	}
	for (nomadshort_t y = 1; y < 4; ++y) {
		for (nomadshort_t x = 1; x < 6; ++x) {
			mvwaddch(game->screen, y, x, ' ');
		}
	}
	if (playr->pdir == D_NORTH) {
		playr->pdir = D_EAST;
	}
	else {
		--playr->pdir;
	}
}

void P_UseMelee()
{

}

void P_QuickShot()
{

}

void P_KillPlayr()
{
	LOG_INFO("freeing up game->playr pointer memory");
	Z_Free(playr);
}

void CommandConsole()
{
	char buf[1024];
	mvwscanw(game->screen, 20, 126, "/> %s", buf);
	if (strstr(buf, "fyia") != NULL) {
		if (scf::launch::infinite_ammo)
			scf::launch::infinite_ammo = false;
		else
			scf::launch::infinite_ammo = true;
	}
	else if (strstr(buf, "iwtbag") != NULL) {
		if (scf::launch::godmode)
			scf::launch::godmode = false;
		else
			scf::launch::godmode = true;
	}
	else if (strstr(buf, "tgdnec") != NULL) {
		if (scf::launch::bottomless_clip)
			scf::launch::bottomless_clip = false;
		else
			scf::launch::bottomless_clip = true;
	}
	else if (strstr(buf, "blindmobs") != NULL) {
		if (scf::launch::blindmobs)
			scf::launch::blindmobs = false;
		else
			scf::launch::blindmobs = true;
	}
	else if (strstr(buf, "deafmobs") != NULL) {
		if (scf::launch::deafmobs)
			scf::launch::deafmobs = false;
		else
			scf::launch::deafmobs = true;
	}
	else if (strstr(buf, "fastmobs=1") != NULL) {
		if (scf::launch::fastmobs3)
			scf::launch::fastmobs3 = false;
		if (scf::launch::fastmobs2)
			scf::launch::fastmobs2 = false;
		if (scf::launch::fastmobs1)
			scf::launch::fastmobs1 = false;
		else
			scf::launch::fastmobs1 = true;
	}
	else if (strstr(buf, "fastmobs=2") != NULL) {
		if (scf::launch::fastmobs1)
			scf::launch::fastmobs1 = false;
		if (scf::launch::fastmobs3)
			scf::launch::fastmobs3 = false;
		if (scf::launch::fastmobs2)
			scf::launch::fastmobs2 = false;
		else
			scf::launch::fastmobs2 = true;
	}
}