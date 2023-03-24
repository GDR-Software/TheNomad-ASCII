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
// DESCRIPTION: src/g_loop.cpp
//  main gameplay loop
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

#if !defined(TESTING)
static Game* game;

static void G_ShowAbout()
{
	werase(game->screen);
	mvwprintw(game->screen, 0, 55, "[ABOUT]");
	mvwaddstr(game->screen, 1, 0, about_str);
	wrefresh(game->screen);
	char c;
	while (c != 'q') {
		c = getc(stdin);
		sleepfor(ticrate_mil);
	}
	werase(game->screen);
	return;
}
static void G_ShowCredits()
{
	werase(game->screen);
	mvwprintw(game->screen, 0, 55, "[CREDITS]");
	mvwaddstr(game->screen, 1, 0, credits_str);
	wrefresh(game->screen);
	char c;
	while (c != 'q') {
		c = getc(stdin);
		sleepfor(ticrate_mil);
	}
	werase(game->screen);
	return;
}

static void settingsLoop(void);

void mainLoop(int argc, char* argv[])
{
	Z_Init();
	game = (Game *)Z_Malloc(sizeof(Game), TAG_STATIC, &game);
	PTR_CHECK(NULL_CHECK, game);
	I_NomadInit(argc, argv, game);
	nomadushort_t c{};
	while (1) {
		if (game->gamestate == GS_TITLE) {
			G_RunSound();
			werase(game->screen);
			game->DrawTitleScreen();
			nomadshort_t c;
			c = wgetch(game->screen);
			while (game->gamestate == GS_TITLE) {
				if (c == '\n') {
					game->gamestate = GS_MENU;
					P_PlaySFX(scf::sounds::sfx_menu_select);
				}
				else if (c == ctrl('x')) {
					P_PlaySFX(scf::sounds::sfx_menu_select);
					game->~Game();
					exit(1);
				}
				else if (c == 'q') {
					P_PlaySFX(scf::sounds::sfx_menu_select);
					game->~Game();
					exit(1);
				}
			}
		}
		else if (game->gamestate == GS_MENU) {
			nomadshort_t s = 0;
			while (game->gamestate == GS_MENU) {
				G_RunSound();
				werase(game->screen);
				game->DrawMenuScreen(s);
				char f = wgetch(game->screen);
				if (f != ctrl('x')) {
					if (f == 'w') {
						// s behaves in strange and mysterious ways
						s--;
						if (s < 0) {
							s = 4;
						}
						P_PlaySFX(scf::sounds::sfx_menu_updown);
					}
					else if (f == 's') {
						s++;
						if (s > 4) {
							s = 0;
						}
						P_PlaySFX(scf::sounds::sfx_menu_updown);
					}
					else if (f == '\n') {
						switch (s) {
						case 0:
							game->gamestate = GS_CAMPAIGN;
							break;
						case 1:
							game->G_LoadGame("nomadsv.ngd");
							break;
						case 2:
							G_ShowAbout();
							break;
						case 3:
							G_ShowCredits();
							break;
						case 4:
							game->~Game();
							exit(1);
							break;
						default:
							break;
						};
						P_PlaySFX(scf::sounds::sfx_menu_select);
					}
				}
				else {
					game->gamestate = GS_TITLE;
					P_PlaySFX(scf::sounds::sfx_menu_select);
				}
				sleepfor(100);
			}
		}
		else if (game->gamestate == GS_CAMPAIGN) {
			G_RunSound();
			if (G_CampaignSelect() == 1) {
				levelLoop();
			}
		}
		else if (game->gamestate == GS_LEVEL) {
			levelLoop();
		}
		else if (game->gamestate == GS_SETTINGS) {
			settingsLoop();
		}
		else if (game->gamestate == GS_PAUSE) {
			G_RunSound();
			nomadshort_t s = 0;
			Snd_LowerMusic();
			while (game->gamestate == GS_PAUSE) {
				werase(game->screen);
				game->DrawPauseMenu(s);
				char f = wgetch(game->screen);
				if (f != ctrl('x')) {
					if (f == 'w') {
						// s behaves in strange and mysterious ways
						s--;
						if (s < 0) {
							s = 5;
						}
						P_PlaySFX(scf::sounds::sfx_menu_updown);
					}
					else if (f == 's') {
						s++;
						if (s > 5) {
							s = 0;
						}
						P_PlaySFX(scf::sounds::sfx_menu_updown);
					}
					else if (f == '\n') {
						switch (s) {
						case 0:
							game->gamestate = GS_LEVEL;
							break;
						case 1:
							game->G_SaveGame(game->svfile);
							break;
						case 2:
							game->gamestate = GS_SETTINGS;
							game->gamescreen = MENU_SETTINGS;
							break;
						case 3:
							game->G_LoadGame(game->svfile);
							break;
						case 4:
							game->G_SaveGame(game->svfile);
							game->~Game();
							exit(1);
							break;
						case 5:
							game->gamestate = GS_MENU;
							break;
						default:
							break;
						};
						P_PlaySFX(scf::sounds::sfx_menu_select);
					}
					sleepfor(77);
				}
				else {
					game->gamestate = GS_LEVEL;
					P_PlaySFX(scf::sounds::sfx_menu_select);
				}
			}
		}
		else {
			N_Error("Unknown/Invalid Gamestate: %hu", game->gamestate);
		}
	}
}

static nomaduint_t loop_delay = 0;

void LooperDelay(nomaduint_t numsecs)
{
	loop_delay = numsecs;
}

static linked_list<Mob*>::iterator mob_it;
static linked_list<item_t*>::iterator item_it;
static std::vector<std::future<void>> mob_async;
static boost::mutex playr_lock;

void P_Ticker()
{
	char c;
	if ((c = kb_hit()) != -1)
		game->P_Ticker(c);
}

static nomadbool_t playing = false;

void levelLoop(void)
{
//	if (!playing) {
//		S_PlayMusic("MUS01.ogg");
//		playing = true;
//	}
	assert(game && game->playr);
	game->hudwin[HL_VMATRIX] = subwin(game->screen, MAX_VERT_FOV, MAX_HORZ_FOV, 4, 7);
	assert(game->hudwin[HL_VMATRIX]);
	werase(game->screen);
	game->G_DisplayHUD();
	wrefresh(game->hudwin[HL_VMATRIX]);
	Snd_RaiseMusic();
	while (game->gamestate == GS_LEVEL) {
		std::thread snd_thread(G_RunSound);
		game->DrawMainWinBorder();
		game->G_DisplayHUD();
		if (game->playr->health < 1) {
			if (P_Random() > 240)
				P_PlaySFX(scf::sounds::sfx_playr_die_rare);
			else
				P_PlaySFX(scf::sounds::sfx_playr_die);
			
//			G_RunSound();
			werase(game->screen);
			mvwprintw(game->screen, 0, 0, "YOU DIED!");
			mvwprintw(game->screen, 1, 0, "press any key to exit...");
			wrefresh(game->screen);
			wgetch(game->screen);
			game->~Game();
			exit(EXIT_SUCCESS);
		}
		snd_thread.join();
		if (game->m_Active.size() > 0) {
			for (mob_it = game->m_Active.begin(); mob_it != game->m_Active.end(); mob_it = mob_it->next) {
				M_RunThinker(mob_it);
			}
		}
		char c;
		if ((c = kb_hit()) != -1)
			game->P_Ticker(c);
//		if (game->items.size() > 0) {
//			for (item_it = game->items.begin(); item_it != game->items.end(); item_it = item_it->next) {
//				--item_it->val->ticker;
//				if (item_it->val->ticker <= -1) {
//					item_t* item = item_it->val;
//					game->items.erase(item_it);
//					Z_Free(item);
//				}
//			}
//		}
#if 0
		sleepfor(MILLISECONDS(ticrate_mil));
#endif
		boost::this_thread::sleep_for(boost::chrono::milliseconds(ticrate_mil));
		++game->ticcount;
		wrefresh(game->screen);
	};
	delwin(game->hudwin[HL_VMATRIX]);
	return;
}

// currently, the only way to customize controls is to go into the .scf file and edit it yourself,
// instructions on how to do that are in nomadascii_blackbook.txt
static void settingsLoop(void)
{
	std::ifstream file("Files/gamedata/GS/settingsmenu.txt", std::ios::in);
	NOMAD_ASSERT(!file.fail(), ("Failed to load settingsmenu resource!"));
	assert(!file.fail());
	LOG_INFO("Succesfully opened Files/gamedata/GS/settingsmenu.txt");
	std::vector<std::string> filebuf;
	std::string line;
	while (std::getline(file, line)) { filebuf.push_back(line); };
	file.close();
	nomaduint_t i;
	while (game->gamestate == GS_SETTINGS) {
		werase(game->screen);
		for (i = 0; i < filebuf.size(); ++i)
			mvwaddstr(game->screen, i, 0, filebuf[i].c_str());
		mvwaddstr(game->screen, i+1,  0, "[Launch Parameters]");
		mvwprintw(game->screen, i+2,  0, "fastmobs1: %s", booltostr(scf::launch::fastmobs1));
		mvwprintw(game->screen, i+3,  0, "fastmobs2: %s", booltostr(scf::launch::fastmobs2));
		mvwprintw(game->screen, i+4,  0, "fastmobs3: %s", booltostr(scf::launch::fastmobs3));
		mvwprintw(game->screen, i+5,  0, "blindmobs: %s", booltostr(scf::launch::blindmobs));
		mvwprintw(game->screen, i+6,  0, "deafmobs: %s", booltostr(scf::launch::deafmobs));
		mvwprintw(game->screen, i+7,  0, "nosmell: %s", booltostr(scf::launch::nosmell));
		mvwprintw(game->screen, i+8,  0, "godmode: %s", booltostr(scf::launch::godmode));
		mvwprintw(game->screen, i+9,  0, "ext_bff: %s", booltostr(scf::launch::ext_bff));
		mvwprintw(game->screen, i+10, 0, "bottomless_clip: %s", booltostr(scf::launch::bottomless_clip));
		mvwprintw(game->screen, i+11, 0, "devmode: %s", booltostr(scf::launch::devmode));
		mvwprintw(game->screen, i+12, 0, "infinite_ammo: %s", booltostr(scf::launch::infinite_ammo));
		char c;
		if ((c = kb_hit()) != -1) {
			switch (c) {
			case KEY_BSPACE:
			case CTRL_x:
				game->gamestate = GS_PAUSE;
				game->gamescreen = MENU_PAUSE;
				break;
			case CTRL_c:
				game->~Game();
				break;
			default: break;
			};
		}
		sleepfor(ticrate_mil);
		wrefresh(game->screen);
	};
}

#endif
