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

void signal_interrupt(int signum)
{
	werase(stdscr);
	endwin();
	puts("Killing Proccess, Exit Success!");
	exit(EXIT_SUCCESS);
}

void signal_seggy(int signum)
{
	werase(stdscr);
	endwin();
#ifndef _RELEASE
	printf("Signal Segmentation Fault Received\n");
#else
	puts("Caught A Seggy! Goddamn It!");
#endif
	exit(EXIT_FAILURE);
}

void signal_unnatural_demise(int signum)
{
	werase(stdscr);
	endwin();
#ifndef _RELEASE
	puts("Received Signal Abort");
#else
	puts("The Game Object Was KIA'd By Strange & Mysterious Forces Beyond Our Knowledege...");
#endif
	exit(EXIT_FAILURE);
}

void signal_somethins_corrupt(int signum)
{
	werase(stdscr);
	endwin();
#ifndef _RELEASE
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

int main(int argc, char* argv[])
{
#ifdef __unix__
	signal(SIGINT, signal_interrupt);
	signal(SIGSEGV, signal_seggy);
	signal(SIGTERM, signal_interrupt);
	signal(SIGABRT, signal_unnatural_demise);
	signal(SIGILL, signal_somethins_corrupt);
	signal(SIGQUIT, signal_interrupt);
#endif
	set_nonblock();
	mainLoop(argc, argv);
	return 0;
}