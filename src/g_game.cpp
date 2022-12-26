#include "g_game.h"
#include "scf.h"
#include <stdarg.h>

Game::Game()
{
}

static void set_block(void)
{
	struct termios ttystate;
	
	// get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	
	//turn on canonical mode
	ttystate.c_lflag |= ICANON;

	// set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

Game::~Game()
{
	attroff(COLOR_PAIR(0));
	delwin(screen);
	erase();
	endwin();
	set_block();
	
	// now we delete any of the runtime-only resources
	system("rm Files/gamedata/RUNTIME/*.txt");
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