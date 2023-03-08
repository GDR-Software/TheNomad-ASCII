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
#include "n_shared.h"
#include "scf.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "s_scripted.h"
#include "g_items.h"
#include "s_world.h"
#include "g_map.h"
#include "g_zone.h"
#include "g_playr.h"
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

static inline void E_Init(Game* const game);
static inline void I_ProcessArgs(const std::vector<char*>& myargv);

void I_NomadInit(int argc, char* argv[], Game* game)
{
	assert(game);
	gptr = game;
	MainAssigner(game);
	PlayrAssigner(game);
	ncurses_on = false;
	game->gamestate = GS_TITLE;
	game->gamescreen = MENU_TITLE;
	game->difficulty = DIF_NOOB;
	game->ticcount = 0;
	char buf[256];
	strncpy(game->bffname, "nomadmain.bff", sizeof(game->bffname));
	strncpy(game->scfname, "default.scf", sizeof(game->scfname));
	strncpy(game->svfile, "nomadsv.ngd", sizeof(game->svfile));
	switch (_NOMAD_VERSION) {
	case 0:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  Version: Pre-Alpha (v%i.%i.%i)\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
		break;
	case 1:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  Version: Alpha (v%i.%i.%i)  \n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
	case 2:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  Version: Beta (v%i.%i.%i)\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
	case 3:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"  Version: (v%i.%i.%i)\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
	default: // should theoretically never happen
		printf("(ERROR: Fatal) Invalid Version!\n");
		exit(EXIT_FAILURE);
		break;
	};
	TUIAssigner(game);
	CampaignAssigner(game);
	puts(buf);
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	printf("I_NomadInit(): Initializing Game...\n");
	puts("Snd_Init(): Initializing OpenAL and libsndfile audio libraries for audio...");
	srand(time(NULL));
	std::vector<char*> myargv;
	for (nomadushort_t i = 0; i < argc; i++) {
		myargv.push_back(argv[i]);
	}
	I_ProcessArgs(myargv);
	E_Init(game);
	Snd_Init(game);
	G_LoadBFF(game->bffname, game);
	scf::G_LoadSCF(game->scfname);
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

void TUI_Init(Game* const game)
{
//	PTR_CHECK(NULL_CHECK, game);
	puts("TUI_Init(): Initializing Screen And NCurses/Curses Libraries...");
	setlocale(LC_ALL, "");
	initscr();
	raw();
	cbreak();
	noecho();
	curs_set(0);
	game->screen = newwin(34, 129, 0, 0);
	stdscr = game->screen;
//	PTR_CHECK(NULL_CHECK, game->screen);
//	LOG_INFO("game->screen allocated successfully");
	keypad(game->screen, TRUE);
	
	if (getmaxy(game->screen) < 30 && getmaxx(game->screen) < 45)
		N_Error("Screen Too Small For nomadascii!");
	// change this in the future, this game doesn't "require" colors
	if (has_colors() == FALSE)
		N_Error("Terminal Must Support Colors!");
	
	start_color();
//	LOG_INFO("has_colors() = true");
	ncurses_on = true;
}

static inline void E_Init(Game* const game)
{
//	PTR_CHECK(NULL_CHECK, game);
	puts("E_Init(): Initializing Entities...");
	if (!mainzone)
		N_Error("failed!");
	game->playr = (Playr*)Z_Malloc(sizeof(Playr), TAG_STATIC, &game->playr);
//	PTR_CHECK(NULL_CHECK, game->playr);
//	LOG_INFO("Successfully Allocated game->playr");
	game->playr->P_Init();
	srand(time(NULL));
	game->M_GenMobs();
	MobtAssigner(game);
}

static inline void I_ProcessArgs(const std::vector<char*>& myargv)
{
	assert(myargv.size() > 0);
	puts("I_ProcessArgs(): Parsing Command-Line Arguments...");
	for (nomaduint_t i = 1; i < myargv.size(); ++i) {
		if (strstr(myargv[i], "-fastmobs")) {
			if (strchr(myargv[i], '1')) {
				scf::launch::fastmobs1 = true;
				puts("Fast Mobs 1: On");
			}
			else if (strchr(myargv[i], '2')) {
				scf::launch::fastmobs2 = true;
				puts("Fast Mobs 2: On");
			}
			else if (strchr(myargv[i], '3')) {
				scf::launch::fastmobs3 = true;
				puts("Fast Mobs 3: On");
			}
		}
		else if (strstr(myargv[i], "-dif")) {
			++i;
			if (!strstr(myargv[i], "DIF_")) {
				gptr->difficulty = atoi(myargv[i]);
				printf("New Game Difficulty Assigned: %s\n", difftostr(gptr->difficulty));
			} else {
				gptr->difficulty = strtodiff(myargv[i]);
				printf("New Game Difficulty Assigned: %s\n", difftostr(gptr->difficulty));
			}
		}
		else if (strstr(myargv[i], "-bff")) {
			scf::launch::ext_bff = true;
			++i;
			strncpy(gptr->bffname, myargv[i], sizeof(gptr->bffname));
			if (!strstr(gptr->bffname, "nomadmain.bff")) {
				fprintf(stdout, "Using non-default BFF file: %s\n", gptr->bffname);
			}
		}
		else if (strstr(myargv[i], "-scf")) {
			scf::launch::ext_scf = true;
			++i;
			strncpy(gptr->scfname, myargv[i], sizeof(gptr->scfname));
			if (!strstr(gptr->scfname, "default.scf")) {
				fprintf(stdout, "Using non-default SACE Configuration File: %s\n", gptr->scfname);
			}
		}
		else if (strstr(myargv[i], "-save")) {
			++i;
			strncpy(gptr->svfile, myargv[i], sizeof(gptr->svfile));
			if (!strstr(gptr->svfile, "nomadsv.ngd")) {
				fprintf(stdout, "Using non-default nomad game data save file: %s\n", gptr->svfile);
			}
		}
		else if (strstr(myargv[i], "-deafmobs")) {
			scf::launch::deafmobs = true;
			puts("Deaf Mobs: On");
		}
		else if (strstr(myargv[i], "-nosmell")) {
			scf::launch::nosmell = true;
			puts("No Smell: On");
		}
		else if (strstr(myargv[i], "-nomobs")) {
			scf::launch::nomobs = true;
			puts("No Mobs: On");
		}
		else if (strstr(myargv[i], "-blindmobs")) {
			scf::launch::blindmobs = true;
			puts("Blind Mobs: On");
		}
		else if (strstr(myargv[i], "-devmode")) {
			scf::launch::devmode = true;
			puts("Dev Mode: On");
		} /*
		else if (strstr(myargv[i], "-ram ") != NULL) {
			heap_size += atoi(myargv[i++]);
		} */
		/* now we get to the REAL cheat codes */
		// i want to be a god
		else if (strstr(myargv[i], "-iwtbag")) {
			scf::launch::godmode = true;
			puts("God Mode: On");
		}
		// that goddam never-ending clip
		else if (strstr(myargv[i], "-tgdnec")) {
			scf::launch::bottomless_clip = true;
			puts("Bottomless Clip: On");
		}
		// fuck yeah! infinite ammo
		else if (strstr(myargv[i], "-fyia")) {
			scf::launch::infinite_ammo = true;
			puts("Infinite Ammo: On");
		}
		else if (strstr(myargv[i], ".bff")
		|| strstr(myargv[i], ".scf")
		|| strstr(myargv[i], ".ngd")) { // bff/scf/ngd in the param list
			continue;
		}
		else {
			printf("Unrecognized Launch Parameter: %s, Ignoring\n", myargv[i]);
		}
	}
	// mobs like little kids who've had a little too much sugar
	if (scf::launch::fastmobs1) {
		scf::mobspeed = pow(1, 2);
	}
	// mobs on crack. Serious, hardcore crack
	else if (scf::launch::fastmobs2) {
		scf::mobspeed = pow(3, 2);
	}
	// mobs on black-tar heroin
	else if (scf::launch::fastmobs3) {
		scf::mobspeed = pow(5, 2);
	}
}
#endif
