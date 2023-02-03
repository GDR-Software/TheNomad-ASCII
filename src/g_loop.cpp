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

void mainLoop(int argc, char* argv[])
{
	Z_Init();
	
	// allocating 1 mb to the chunkiest class of them all
	game = (Game *)Z_Malloc(sizeof(Game), TAG_STATIC, &game);

#ifdef _NOMAD_DEBUG
	assert(game);
#endif
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
				sleep_for(100);
			}
		}
		else if (game->gamestate == GS_LEVEL) {
			levelLoop();
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
						case 2: {
							bool real = game->G_LoadGame();
							if (!real) {
								mvwprintw(game->screen, getmaxy(game->screen), 0, "(ERROR) Invalid Save File");
								wrefresh(game->screen);
								wgetch(game->screen);
							}
							break; }
						case 4:
							game->G_SaveGame();
							game->~Game();
							exit(1);
							break;
						case 5:
							game->gamestate = GS_MENU;
						default:
							break;
						};
					}
					sleep_for(77);
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

static void* P_Loop(void *arg)
{
	pthread_mutex_lock(&game->playr_mutex);
	nomadenum_t c = getc(stdin);
	game->P_Ticker(c);
	pthread_mutex_unlock(&game->playr_mutex);
	return NULL;
}

static void levelLoop(void)
{
	game->hudwin[HL_VMATRIX] = subwin(game->screen, MAX_VERT_FOV, MAX_HORZ_FOV, 4, 7);
#ifdef _NOMAD_DEBUG
	assert(game->hudwin[HL_VMATRIX]);
#endif
	game->G_DisplayHUD();
	werase(game->screen);
	wrefresh(game->hudwin[HL_VMATRIX]);
	while (game->gamestate == GS_LEVEL) {
		werase(game->screen);
		game->DrawMainWinBorder();
		game->G_DisplayHUD();
		// custom key-binds will be implemented in the future
		pthread_create(&game->pthread, NULL, P_Loop, NULL);
		pthread_create(&game->wthread, NULL, W_Loop, NULL);
		pthread_join(game->pthread, NULL);
		pthread_join(game->wthread, NULL);
		sleep_for(ticrate_mil);
		++game->ticcount;
		wrefresh(game->screen);
	};
	delwin(game->hudwin[HL_VMATRIX]);
	return;
}
#endif