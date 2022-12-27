#ifdef _WIN32
// ncurses is mostly linux, fuck it
#ifndef __N_W32_NCURSES__
#define __N_W32_NCURSES__

#include "n_shared.h"

typedef CHAR_INFO chtype;

typedef struct _win32_ncurses_window
{
	HANDLE h_Win;
	int height;
	int width;
	int starty;
	int startx;
} WINDOW;

constexpr WINDOW* stdscr;

constexpr inline WINDOW* newwin(int height, int width, int starty, int startx);
constexpr inline void initscr();
constexpr inline void endwin();
constexpr inline void move();
constexpr inline void waddch();
constexpr inline void addch();
constexpr inline void mvaddch();
constexpr inline void mvwaddch();
constexpr inline void addstr();
constexpr inline void printw();
constexpr inline void scanw(const char* str, ...);
constexpr inline char getch();

constexpr inline void wmove(WINDOW* win);;

#endif

#endif
