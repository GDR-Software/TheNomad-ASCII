#include "n_w32_ncurses.h"

#define dbgprintf(str) \
{ \
	fprintf(stderr, "%s(): %s\n", __FUNC__, str); \
}

constexpr inline WINDOW* newwin(int height, int width, int starty, int startx)
{
	WINDOW* nwin = (WINDOW *)malloc(sizeof(WINDOW));
	if (!nwin) {
		dbgprintf("malloc failed!");
		exit(-1);
	}
}

constexpr inline void initscr()
{
	AllocConsole();
	stdscr = newwin();
	stdscr = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleActiveScreenBuffer(stdscr);
}