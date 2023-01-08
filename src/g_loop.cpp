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
static void titleLoop(void);
static void menuMLoop(void);
static void menuPLoop(void);

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
		switch (game->gamestate) {
		case GS_TITLE:
			titleLoop();
			break;
		case GS_MENU:
			menuMLoop();
			break;
		case GS_LEVEL:
			levelLoop();
			break;
		case GS_PAUSE:
			menuPLoop();
			break;
		default:
			N_Error("Unknown/Invalid Gamestate: %hu", game->gamestate);
			break;
		};
		if (game->gamestate == GS_TITLE) {
			
		}
		else if (game->gamestate == GS_MENU) {
			
		}
		else if (game->gamestate == GS_LEVEL) {
			levelLoop();
		}
		else if (game->gamestate == GS_PAUSE) {
		}
	}
}

static void titleLoop(void)
{
	game->ClearMainWin();
	game->DrawTitleScreen();
	c = wgetch(game->screen);
	if (c == '\n') {
		game->gamestate = GS_MENU;
	}
	else if (c == ctrl('x')) {
		game->~Game();
		break;
	}
	else {
		continue;
	}
}
static void menuMLoop(void)
{
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
	};
}
static void menuPLoop(void)
{
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
			std::this_thread::sleep_for(77ms);
		}
		else {
			game->gamestate = GS_LEVEL;
		}
	};
		
}

static void levelLoop(void)
{
	game->hudwin[HL_VMATRIX] = subwin(game->screen, MAX_VERT_FOV, MAX_HORZ_FOV, 4, 7);
//	game->hudwin[HL_PROMPT] = subwin(game->screen, 4, 50, 28, 125);
	//game->ClearMainWin();
	game->G_DisplayHUD();
	werase(game->screen);
	wrefresh(game->hudwin[HL_VMATRIX]);
	while (game->gamestate == GS_LEVEL) {
		game->DrawMainWinBorder();
		game->G_DisplayHUD();
		// custom key-binds will be implemented in the future
		pthread_create(&game->wthread, NULL, W_Loop, NULL);
		pthread_mutex_lock(&game->playr_mutex);
		nomadenum_t c;
		if ((c = kbhit()) != 0)
			game->P_Ticker(c);
		pthread_mutex_unlock(&game->playr_mutex);
		pthread_join(game->wthread, NULL);
		std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
		++game->ticcount;
		wrefresh(game->screen);
	};
//	delwin(game->hudwin[HL_PROMPT]);
	delwin(game->hudwin[HL_VMATRIX]);
	return;
}
#endif