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
//  src/g_main.cpp
//----------------------------------------------------------
#include "g_game.h"

static Game* game;

void MainAssigner(Game* const gptr)
{
	game = gptr;
}

void signal_interrupt(int signum)
{
	if (game->gamestate != GS_TITLE && GS_MENU)
		delwin(game->hudwin[HL_VMATRIX]);
	game->~Game();
	puts("Killing Proccess, Exit Success!");
	exit(EXIT_SUCCESS);
}

void signal_seggy(int signum)
{
	if (game->gamestate != GS_TITLE && GS_MENU && GS_PAUSE)
		delwin(game->hudwin[HL_VMATRIX]);
	game->~Game();
#ifndef RELEASE
	printf("Signal Segmentation Fault Received\n");
#else
	puts("Caught A Seggy! Goddamn It!");
#endif
	exit(EXIT_FAILURE);
}

void signal_unnatural_demise(int signum)
{
	if (game->gamestate != GS_TITLE && GS_MENU && GS_PAUSE)
		delwin(game->hudwin[HL_VMATRIX]);
	game->~Game();
#ifndef RELEASE
	puts("Received Signal Abort");
#else
	puts("The Game Object Was KIA'd By Strange & Mysterious Forces Beyond Our Knowledege...");
#endif
	exit(EXIT_FAILURE);
}

void signal_somethins_corrupt(int signum)
{
	if (game->gamestate != GS_TITLE && GS_MENU && GS_PAUSE)
		delwin(game->hudwin[HL_VMATRIX]);
	game->~Game();
#ifndef RELEASE
	printf("Received Signal Corruption\n");
#else
	puts("Somethin's Gotten Corrupt... I Don't Know What, But This Thing is Corrupt, Your Fault, Perhaps?");
#endif
	exit(EXIT_FAILURE);
}

static void set_nonblock(void)
{
	struct termios ttystate;
	
	// get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	
	// turn off canonical mode
	ttystate.c_lflag &= ~ICANON;
	
	// minimum of number input read.
	ttystate.c_cc[VMIN] = 1;
	
	// set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

FILE* dbg_file;
#define DBG_PATH "Files/debug/"
void __attribute__((constructor)) debug_startup(void)
{
	std::string dbg_path = DBG_PATH;
	dbg_path += "debuglog.txt";
	dbg_file = fopen(dbg_path.c_str(), "a");
	fprintf(dbg_file, "\n\n\n%s(APPENDING PREVIOUS LOG)%s\n\n", C_GREEN, C_RESET);
	fprintf(dbg_file, "%s%s<--------------------[START LOG SESSION]-------------------->%s\n", C_BG_BLACK, C_WHITE, C_RESET);
}
void __attribute__((destructor)) debug_kill(void)
{
	fprintf(dbg_file, "\n%s%s<--------------------[END LOG SESSION]-------------------->%s\n", C_BG_BLACK, C_WHITE, C_RESET);
	fclose(dbg_file);
}

void signal_buss(int signum)
{
	if (game->gamestate != GS_TITLE && GS_MENU && GS_PAUSE)
		delwin(game->hudwin[HL_VMATRIX]);
	game->~Game();
#ifdef RELEASE
	LOG_WARN("RECIEVED SIGNAL SIGBUS!");
	puts("Attempted to access invalid memory address (sigbus)");
#else
	puts("signal SIGBUS recieved");
#endif
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
#ifdef UNIX_NOMAD
	signal(SIGINT, signal_interrupt);
	signal(SIGSEGV, signal_seggy);
	signal(SIGTERM, signal_interrupt);
	signal(SIGABRT, signal_unnatural_demise);
	signal(SIGILL, signal_somethins_corrupt);
	signal(SIGQUIT, signal_interrupt);
	signal(SIGKILL, signal_interrupt);
	signal(SIGBUS, signal_buss);
	set_nonblock();
#endif
	mainLoop(argc, argv);
	return 0;
}
