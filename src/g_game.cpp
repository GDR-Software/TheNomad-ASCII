#include "g_game.h"
#include "scf.h"
#include <stdarg.h>

Game::Game()
{
}

Game::~Game()
{/*
	attroff(COLOR_PAIR(0));
	delwin(screen);
	endwin();
	// unique_ptrs'll do what they do best here
	
	// now we delete any of the runtime-only resources
	system("rm -rf Files/gamedata/RUNTIME"); */
}

void N_Error(const char* err, ...)
{
	endwin();
	va_list	argptr;

    // Message first.
	va_start(argptr, err);
	fprintf(stderr, "(ERROR) ");
	vfprintf(stderr, err, argptr);
	fprintf(stderr, "\n\n");
	va_end(argptr);
	
	fflush(stderr);

	// cleanup, cleanup, everybody cleanup!! get the fuck outta here!
	exit(-1);
}