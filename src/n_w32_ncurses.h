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

WINDOW* stdscr;

constexpr inline WINDOW* newwin(int height, int width, int starty, int startx);
constexpr inline void initscr();
constexpr inline void endwin();
constexpr inline void move(int y, int x);

constexpr inline void waddch(WINDOW* window, chtype ch);
constexpr inline void addch(chtype ch);
constexpr inline void mvaddch(int y, int x, chtype ch);
constexpr inline void mvwaddch(WINDOW* window, int y, int x, chtype ch);

constexpr inline void addstr(const char* message);
constexpr inline void printw(const char* message, ...);
constexpr inline void scanw(const char* str, ...);
constexpr inline char getch(void);

constexpr inline void wmove(WINDOW* win, int y, int x);

#endif

#endif
