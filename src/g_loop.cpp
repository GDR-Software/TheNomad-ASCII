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
//  src/g_loop.cpp
//----------------------------------------------------------
#include "g_game.h"

#if !defined(TESTING)
static Game* game;

static void levelLoop(void);
static void settingsLoop(void);

void mainLoop(int argc, char* argv[])
{
	uint64_t start, end;
	puts("Z_Init(): initializing zone memory allocation daemon...");
	Z_Init(start, end);
	printf("Allocated zone from %p -> %p, %i bytes allocated to zone\n", mainzone, (mainzone+mainzone->size), heapsize);
	game = (Game *)Z_Malloc(sizeof(Game), TAG_STATIC, &game);
	I_NomadInit(argc, argv, game);
	Z_CheckHeap();
#ifdef _NOMAD_DEBUG
	Z_FileDumpHeap();
#endif
	nomadushort_t c{};
	start_color();
	init_pair(0, COLOR_WHITE, COLOR_WHITE);
	attron(COLOR_PAIR(0));
	while (1) {
		if (game->gamestate == GS_TITLE) {
			game->ClearMainWin();
			game->DrawTitleScreen();
			nomadshort_t c;
			c = wgetch(game->screen);
			while (game->gamestate == GS_TITLE) {
				if (c == '\n') {
					game->gamestate = GS_MENU;
				}
				else if (c == ctrl('x')) {
					game->~Game();
					exit(1);
				}
				else if (c == 'q') {
					game->~Game();
					exit(1);
				}
				else {
					continue;
				}
			}
		}
		else if (game->gamestate == GS_MENU) {
			int16_t s = 0; // this thing breaks if its a nomadshort_t, don't know why
			while (game->gamestate == GS_MENU) {
				game->ClearMainWin();
				game->DrawMenuScreen(s);
				char f = wgetch(game->screen);
				if (f != ctrl('x')) {
					if (f == 'w') {
						// s behaves in strange and mysterious ways
						s--;
						if (s < 0) {
							s = 5;
						}
					}
					else if (f == 's') {
						s++;
						if (s > 5) {
							s = 0;
						}
					}
					else if (f == '\n') {
						switch (s) {
						case 0:
							game->gamestate = GS_LEVEL;
							break;
						case 5:
							game->~Game();
							exit(1);
							break;
						default:
							break;
						};
					}
				}
				else {
					game->gamestate = GS_TITLE;
				}
				std::this_thread::sleep_for(100ms);
			}
		}
		else if (game->gamestate == GS_LEVEL) {
			levelLoop();
		}
		else if (game->gamestate == GS_SETTINGS) {
			settingsLoop();
		}
		else if (game->gamestate == GS_PAUSE) {
			nomadshort_t s = 0;
			while (game->gamestate == GS_PAUSE) {
				s = s;
				game->ClearMainWin();
				game->DrawPauseMenu(s);
				char f = wgetch(game->screen);
				if (f != ctrl('x')) {
					if (f == 'w') {
						// s behaves in strange and mysterious ways
						s--;
						if (s < 0) {
							s = 5;
						}
					}
					else if (f == 's') {
						s++;
						if (s > 5) {
							s = 0;
						}
					}
					else if (f == '\n') {
						switch (s) {
						case 0:
							game->gamestate = GS_LEVEL;
							break;
						case 1:
							game->G_SaveGame();
							break;
						case 2:
							game->gamestate = GS_SETTINGS;
							game->gamescreen = MENU_SETTINGS;
							break;
						case 3:
							game->G_LoadGame("nomadsv.ngd");
							break;
						case 4:
							game->G_SaveGame();
							game->~Game();
							exit(1);
							break;
						case 5:
							game->gamestate = GS_MENU;
							break;
						default:
							break;
						};
					}
					std::this_thread::sleep_for(77ms);
				}
				else {
					game->gamestate = GS_LEVEL;
				}
			}
		}
		else {
			N_Error("Unknown/Invalid Gamestate: %hu", game->gamestate);
		}
	}
}

static void levelLoop(void)
{
#ifdef _NOMAD_DEBUG
	assert(game && game->playr);
#endif
	game->hudwin[HL_VMATRIX] = subwin(game->screen, MAX_VERT_FOV, MAX_HORZ_FOV, 4, 7);
#ifdef _NOMAD_DEBUG
	assert(game->hudwin[HL_VMATRIX]);
#endif
	game->G_DisplayHUD();
	werase(game->screen);
	wrefresh(game->hudwin[HL_VMATRIX]);
	while (game->gamestate == GS_LEVEL) {
//		werase(game->screen);
		game->DrawMainWinBorder();
		game->G_DisplayHUD();
		// custom key-binds will be implemented in the future
//		pthread_create(&game->wthread, NULL, W_Loop, NULL);
		pthread_mutex_lock(&game->playr_mutex);
		char c;
		if ((c = kb_hit()) != -1)
			game->P_Ticker(c);
		pthread_mutex_unlock(&game->playr_mutex);
//		pthread_join(game->wthread, NULL);
		std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
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
	DBG_LOG("Succesfully opened Files/gamedata/GS/settingsmenu.txt");
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
		std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
		wrefresh(game->screen);
	};
}

#endif
