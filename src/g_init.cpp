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
//  src/g_init.cpp
//----------------------------------------------------------
#include "g_game.h"

static nomadbool_t ncurses_on;
static Game* gptr;

void N_Error(const char* err, ...)
{
	if (ncurses_on) {
		clear();
		refresh();
		endwin();
	}
	va_list argptr;
	va_start(argptr, err);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, err, argptr);
	fprintf(stderr, "\n");
	va_end(argptr);
	fflush(stderr);

	gptr->~Game();

	exit(-1);
}

#ifndef TESTING

static char bffname[81];
static inline void E_Init(Game* const game);
static inline void TUI_Init(Game* const game);
static inline void I_ProcessArgs(const std::vector<char*>& myargv);

void I_NomadInit(int argc, char* argv[], Game* const game)
{
#ifdef _NOMAD_DEBUG
	assert(game);
#endif
	gptr = game;
	MainAssigner(game);
	PlayrAssigner(game);
	ncurses_on = false;
	pthread_mutex_init(&game->mob_mutex, NULL);
	pthread_mutex_init(&game->npc_mutex, NULL);
	bffname[80] = '\0';
	game->gamestate = GS_TITLE;
	game->gamescreen = MENU_TITLE;
	game->ticcount = 0;
	char buf[256];
	switch (_NOMAD_VERSION) {
	case 0:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  The Nomad Pre-Alpha (v%d.%d.%d)\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
		break;
	case 1:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  The Nomad Alpha (v%d.%d.%d)  \n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
	case 2:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  The Nomad Beta (v%d.%d.%d)\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
	case 3:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  The Nomad (v%d.%d.%d)\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
	default: // should theoretically never happen
		printf("(ERROR: Fatal) Invalid Version!\n");
		exit(EXIT_FAILURE);
		break;
	};
	printf("%s", buf);
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	printf("I_NomadInit(): Initializing Game...\n");
	srand(time(NULL));
	std::vector<char*> myargv;
	for (nomadushort_t i = 0; i < argc; i++) {
		myargv.push_back(argv[i]);
	}
	I_ProcessArgs(myargv);
	E_Init(game);
	W_Init(game);
	TUI_Init(game);
	nomadenum_t counter = 0;
	waddstr(game->screen, companysplash);
	wrefresh(game->screen);
	while (counter != 2) {
		if (getc(stdin) == '\n' || ' ') break;
		std::this_thread::sleep_for(1s);
		++counter;
	}
	counter = 0;
	werase(game->screen);
	waddstr(game->screen, logosplash);
	wrefresh(game->screen);
	while (counter != 4) {
		if (getc(stdin) == '\n' || ' ') break;
		std::this_thread::sleep_for(1s);
		++counter;
	}
}

static inline void TUI_Init(Game* const game)
{
#ifdef _NOMAD_DEBUG
	assert(game);
#endif
	puts("TUI_Init(): Initializing Screen And NCurses/Curses Libraries...");
	setlocale(LC_ALL, "");
	initscr();
	raw();
	cbreak();
	noecho();
	curs_set(0);
	game->screen = newwin(34, 129, 0, 0);
#ifdef _NOMAD_DEBUG
	assert(game->screen);
	LOG("game->screen allocated successfully");
#endif
	keypad(game->screen, TRUE);
	
	if (getmaxy(game->screen) < 30 && getmaxx(game->screen) < 45)
		N_Error("Screen Too Small For nomadascii!");
	// change this in the future, this game doesn't "require" colors
	if (!has_colors())
		N_Error("Must Support 256 Terminal Colors!");
	
#ifdef _NOMAD_DEBUG
	assert(has_colors());
	LOG("has_colors() = true");
#endif
	ncurses_on = true;
}

static inline void E_Init(Game* const game)
{
#ifdef _NOMAD_DEBUG
	assert(game);
#endif
	puts("E_Init(): Initializing Entities...");
	game->playr = (Playr*)Z_Malloc(sizeof(Playr), TAG_STATIC, &game->playr);
#ifdef _NOMAD_DEBUG
	assert(game->playr);
	LOG("Successfully Allocated game->playr");
#endif
	game->playr->P_Init();
	srand(time(NULL));
	game->I_InitNPCs();
	game->M_GenMobs();
}

static inline void I_ProcessArgs(const std::vector<char*>& myargv)
{
#ifdef _NOMAD_DEBUG
	assert(myargv.size() > 0);
#endif
	puts("I_ProcessArgs(): Parsing Command-Line Arguments...");
	for (nomaduint_t i = 1; i < myargv.size(); ++i) {
		if (strstr(myargv[i], "-fastmobs") != NULL) {
			if (strchr(myargv[i], '1') != NULL) {
				scf::launch::fastmobs1 = true;
				puts("Fast Mobs 1: On");
			}
			else if (strchr(myargv[i], '2') != NULL) {
				scf::launch::fastmobs2 = true;
				puts("Fast Mobs 2: On");
			}
			else if (strchr(myargv[i], '3') != NULL) {
				scf::launch::fastmobs3 = true;
				puts("Fast Mobs 3: On");
			}
		}
		else if (strstr(myargv[i], "-bff") != NULL) {
				scf::launch::ext_bff = true;
				strcpy(bffname, myargv[i+1]);
		}
		else if (strstr(myargv[i], "-deafmobs") != NULL) {
			scf::launch::deafmobs = true;
			puts("Deaf Mobs: On");
		}
		else if (strstr(myargv[i], "-nosmell") != NULL) {
			scf::launch::nosmell = true;
			puts("No Smell: On");
		}
		else if (strstr(myargv[i], "-nomobs") != NULL) {
			scf::launch::nomobs = true;
			puts("No Mobs: On");
		}
		else if (strstr(myargv[i], "-blindmobs") != NULL) {
			scf::launch::blindmobs = true;
			puts("Blind Mobs: On");
		}
		else if (strstr(myargv[i], "-devmode") != NULL) {
			scf::launch::devmode = true;
			puts("Dev Mode: On");
		} /*
		else if (strstr(myargv[i], "-ram ") != NULL) {
			heap_size += atoi(myargv[i++]);
		} */
		/* now we get to the REAL cheat codes */
		// i want to be a god
		else if (strstr(myargv[i], "-iwtbag") != NULL) {
			scf::launch::godmode = true;
			puts("God Mode: On");
		}
		// that goddam never-ending clip
		else if (strstr(myargv[i], "-tgdnec") != NULL) {
			scf::launch::bottomless_clip = true;
			puts("Bottomless Clip: On");
		}
		// fuck yeah! infinite ammo
		else if (strstr(myargv[i], "-fyia") != NULL) {
			scf::launch::infinite_ammo = true;
			puts("Infinite Ammo: On");
		}
		else if (strstr(myargv[i], ".bff") != NULL) { // bff's in the param list
			continue;
		}
		else {
			printf("Unrecognized Launch Parameter: %s, Ignoring\n", myargv[i]);
		}
	}
	// mobs like little kids who've had a little too much sugar
	if (scf::launch::fastmobs1) {
		scf::mobspeed <<= 1;
	}
	// mobs on crack. Serious, hardcore crack
	else if (scf::launch::fastmobs2) {
		scf::mobspeed <<= 3;
	}
	// mobs on black-tar heroin
	else if (scf::launch::fastmobs3) {
		scf::mobspeed <<= 5;
	}
}
#endif