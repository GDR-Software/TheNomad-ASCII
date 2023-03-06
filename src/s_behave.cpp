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
//  src/s_behave.cpp
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"
#include "g_rng.h"
#include "s_mission.h"

static Game* game;

void NPCAssigner(Game* const gptr)
{
	game = gptr;
}

void B_BalanceBot(NPC* const npc)
{
	npc->c_npc = npcinfo[rand() % NUMBOTTYPES];
	npc->sprite = npc->c_npc.sprite;
	npc->health = npc->c_npc.health;
	npc->armor = npc->c_npc.armor;
	npc->ndir = P_Random() & 3;
	npc->pos.y = (rand() % 480)+20;
	npc->pos.x = (rand() % 480)+20;
	nomadbool_t done_coords = false;
	while (!done_coords) {
		switch (game->c_map[npc->pos.y][npc->pos.x]) {
		case ' ':
		case '.':
			done_coords = true;
			break;
		default:
			npc->pos.y = (rand() % 480)+20;
			npc->pos.x = (rand() % 480)+20;
			break;
		};
	}
}

NPC* B_SpawnBot(void)
{
	game->b_Active.emplace_back();
	game->b_Active.back() = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &game->b_Active.back());
//	B_BalanceBot(game->b_Active.back());
	return game->b_Active.back();
}

// should only get triggered once, and only ever during the initialize phase
__CFUNC__ void B_SpawnShopBots(void)
{
	PTR_CHECK(NULL_CHECK, game);
	// hardcoded until the BFFs roll around
	NPC* npc;
	
	// creating the guns 'n' grenades bartender
	npc = B_SpawnBot();
	npc->pos = botpos[0];
	npc->c_npc = npcinfo[BOT_BARTENDER];
	npc->sprite = npc->c_npc.sprite;
	npc->c_npc.btype = BOT_BARTENDER;
	npc->ndir = D_WEST;

	// creating the guns 'n' grenades mercenary master
	npc = B_SpawnBot();
	npc->pos = botpos[1];
	npc->c_npc = npcinfo[BOT_MERCMASTER];
	npc->c_npc.btype = BOT_MERCMASTER;
	npc->sprite = npc->c_npc.sprite;
	npc->ndir = D_NORTH;

	// creating the guns 'n' grenades weapons smith
	npc = B_SpawnBot();
	npc->pos = botpos[2];
	npc->c_npc = npcinfo[BOT_WEAPONSMITH];
	npc->c_npc.btype = BOT_WEAPONSMITH;
	npc->sprite = npc->c_npc.sprite;
	npc->ndir = D_EAST;
}
/*
__CFUNC__ void B_GenerateCivilian(NPC* const npc)
{
	npc->pos.y = (P_Random() & 204) + 112;
	npc->pos.x = (P_Random() & 107) + 208;
	switch (game->c_map[npc->pos.y][npc->pos.x]) {
	case ' ':
	case '_':
	case '.':
		break;
	default:
		Z_Free(npc);
		return;
		break;
	};
	npc->c_npc.sprite = 'c';
	npc->c_npc.health = 100;
	npc->c_npc.armor = 12;
	npc->c_npc.btype = BOT_CIVILIAN;
}
__CFUNC__ void B_SpawnCivilianBots(void)
{
	std::vector<NPC*>& b_Active = game->b_Active;
	for (nomaduint_t i = 0; i < MAX_NPC_ACTIVE; ++i) {
		b_Active.emplace_back();
		b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		NPC* const npc = b_Active.back();
		npc->nstate = stateinfo[S_BOT_WANDER];
		npc->nticker = npc->nstate.numticks;
		B_GenerateCivilian(npc);
	}
} */

void Game::I_InitNPCs(void)
{
	game = this;
	PTR_CHECK(NULL_CHECK, game);
	b_Active.reserve(MAX_NPC_ACTIVE);
	LOG_INFO("reserving %i NPC for b_Active", MAX_NPC_ACTIVE);
	B_SpawnShopBots();
//	B_GenNomadTribe();
}

static nomadbool_t B_IsScared(NPC* const npc)
{
	PTR_CHECK(NULL_CHECK, npc);
	if (npc->c_npc.btype == BOT_CIVILIAN) {
		return true; // auto-true if its a civilian
	}
	nomaduint_t rand = (P_Random() & 49) + 1;
	if (rand > 15 && npc->c_npc.btype != BOT_GUARD) {
		return true;
	}
	else if (rand > 30 && npc->c_npc.btype == BOT_GUARD) {
		return true;
	}
	else {
		return false;
	}
}

void B_FleeArea(NPC* const npc)
{
	PTR_CHECK(NULL_CHECK, npc);
	if (!B_IsScared(npc)) {
		return;
	}

	// extra-scared flag
	nomadbool_t panic = (P_Random() & 100) > 75 || npc->c_npc.btype == BOT_CIVILIAN;
	
	if (panic) {
		// run in wild circles
		npc->pos.y += rand() % 444;
	}
	/*
	coord_t pos = game->E_GetDir(npc->ndir);
	npc->pos.y += pos.y;
	npc->pos.x += pos.x; */
}

void B_KillBot(NPC* npc)
{
	for (std::vector<NPC*>::iterator it = game->b_Active.begin(); it != game->b_Active.end(); ++it) {
		if ((*it) == npc) {
			game->b_Active.erase(it);
		}
	}
	Z_Free(npc);
}

void B_KillBot(std::vector<NPC*>::iterator npc)
{
	game->b_Active.erase(npc);
	Z_Free(*npc);
}

void B_WeaponSmithInteract()
{
	Hud_Printf("Weapon Smith", "Greetings, anything I can help you with? [y/n]");
	nomadint_t i = wgetch(game->screen);
	if (i == 'y') {
		Hud_Printf("Weapon Smith", "Splendid, now tell me, what would you like?");
	}
	else {
		Hud_Printf("Weapon Smith", "Whelp then, see you when you need some metal");
	}
}

void B_BartenderInteract()
{
	Hud_Printf("Bartender", "Hello there, anything I can get you today? [y/n]");
	nomadint_t i = wgetch(game->screen);
	if (i == 'y') {
		Hud_Printf("Bartender", "Amazing!, what would ya like?");
	}
	else {
		Hud_Printf("Bartender", "Aight, well, seeya later");
	}
}

static void B_MercDisplayMissions(const std::vector<Mission>& m_ls, Mission* m);

void B_MercMasterInteract()
{
	Hud_Printf("Mercenary Master", "Well hello there, mercenary, how may I help you today? [y/n]");
	wrefresh(game->screen);
	nomadint_t i = getc(stdin);
	if (i == 'y') {
		Hud_Printf("Mercenary Master", "Excellent, here's a list of missions");
		return;
	}
	else {
		Hud_Printf("Mercernary Master", "Oh well, I'll be waiting for you");
		return;
	}
}

static const char* GetMissionNameFromType(const Mission& m)
{
	switch (m.type) {
	case M_ASSASSINATION: return "Assassination";
	case M_BODYGUARDING: return "Bodyguarding";
	case M_EXTORTION: return "Extortion";
	case M_EXTRACTION: return "Extraction";
	case M_CONTRACT: return "Contract";
	case M_INFILTRATION: return "Infiltration";
	case M_KIDNAPPING: return "Kidnapping";
	};
	if (!false)
		N_Error("Unknown/Invalid Mission Type: %i", (int)m.type);
	
	return nullptr;
}

static void B_MercDisplayMissions(const std::vector<Mission>& m_ls, Mission* m)
{
	ITEM **item_ls;
	char c;	
	MENU *menu;
    nomadint_t n_choices, i;
    nomadshort_t selector = 0;
    nomadbool_t done = false;
    werase(game->screen);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	init_pair(5, COLOR_CYAN, COLOR_BLACK);
    std::vector<const char*> choices;
    choices.emplace_back();
    choices.back() = (const char*)NULL;

	/* Create items */
    n_choices = choices.size();
    item_ls = (ITEM **)Z_Malloc(n_choices * sizeof(ITEM *), TAG_STATIC, &item_ls);
    for(i = 0; i < n_choices; ++i) {
        
    }

	/* Crate menu */
	menu = new_menu((ITEM **)item_ls);
     
	/* Set main window and sub window */
    set_menu_win(menu, game->screen);
    set_menu_sub(menu, derwin(game->screen, 10, 38, 3, 1));
	set_menu_format(menu, 10, 1);
			
	/* Set menu mark to the string " * " */
    set_menu_mark(menu, " -> ");

	/* Print a border around the main window and print a title */
    box(game->screen, 0, 0);
    wattron(game->screen, COLOR_PAIR(5));
    mvwprintw(game->screen, 1, 55, "[Inventory]");
    wattroff(game->screen, COLOR_PAIR(5));
	mvwaddch(game->screen, 2, 46, '+');
	mvwhline(game->screen, 2, 47, '-', 33);
	mvwaddch(game->screen, 2, 80, '+');
    mvwaddch(game->screen, 1, 46, '|');
    mvwaddch(game->screen, 1, 80, '|');
    wrefresh(game->screen);
	/* Post the menu */
	post_menu(menu);
	wrefresh(game->screen);
	while (1) {
        c = getc(stdin);
        switch (c) {
        case KEY_w: {
            --selector;
            if (selector < 0)
                selector = choices.size() - 1;
            menu_driver(menu, REQ_UP_ITEM);
            break; }
        case KEY_s: {
            ++selector;
            if (selector >= choices.size())
                selector = 0;
            menu_driver(menu, REQ_DOWN_ITEM);
            break; }
        case ctrl('x'):
        case KEY_q:
            game->gamestate = GS_MENU;
            goto done;
            break;
        default:
            continue;
            break;
        }
        wrefresh(game->screen);
        std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
	}
done:
    unpost_menu(menu);
    free_menu(menu);
    for(i = 0; i < n_choices; ++i)
        free_item(item_ls[i]);
}
