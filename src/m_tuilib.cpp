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
//  src/m_tuilib.cpp
//----------------------------------------------------------
#include "g_game.h"
//#define OPENTL_IMPLEMENTATION
//#include "tl.h"

Menu::Menu(dim_t dim, coord_t s_coords, const std::vector<const char*>& arr, nomaduint_t itemcount, WINDOW* win,
	const char* name)
	: dimensions(dim), coords(s_coords), numitems(itemcount), menuwin(win), menu_name(name)
{
	item_ls = (ITEM **)Z_Malloc((numitems + 1) * sizeof(ITEM *), TAG_STATIC, &item_ls);
	for (nomaduint_t i = 0; i < numitems; ++i) {
		item_ls[i] = new_item(NULL, arr[i]);
	}
	item_ls[numitems] = (ITEM *)NULL;
	menu = new_menu((ITEM **)item_ls);
	set_menu_win(menu, menuwin);
	box(menuwin, 0, 0);
	mvwaddstr(menuwin, 0, dimensions.width >> 1, menu_name.c_str());
	post_menu(menu);
	wrefresh(menuwin);
}

Menu::~Menu()
{
	unpost_menu(menu);
	for (nomaduint_t i = 0; i < numitems; ++i) {
		free_item(item_ls[i]);
	}
	Z_Free(item_ls);
	free_menu(menu);
}


void Game::DrawMainWinBorder(void)
{
	nomadenum_t i;
	for (i = 0; i < getmaxx(screen); i++) {
		mvwaddch(screen, 0, i, '#');
		mvwaddch(screen, (getmaxy(screen) - 1), i, '#');
	}
	for (i = 0; i < getmaxy(screen); i++) {
		mvwaddch(screen, i, 0, '#');
		mvwaddch(screen, i, (getmaxx(screen) - 1), '#');
	}
}

static std::string buffer;

void Game::DrawTitleScreen()
{
	std::ifstream file("Files/gamedata/GS/titlescreen.txt", std::ios::in);
	while (std::getline(file, buffer)) {
		wprintw(screen, "%s\n", buffer.c_str());
	}
}

void Game::DrawMenuScreen(nomadshort_t s)
{
	std::ifstream file("Files/gamedata/GS/menumain.txt", std::ios::in);
	nomadenum_t i = 0;
	while (std::getline(file, buffer)) {
		mvwprintw(screen, i, 0, "%s\n", buffer.c_str());
		if (i == (s+6)) {
			mvwaddch(screen, i, 2, ' ');
			mvwaddch(screen, i, 3, '[');
			for (nomadenum_t a = 3; a < buffer.length(); a++) {
				if (buffer[a] == ' '
				&& buffer[a+1] == ' ') {
					mvwaddch(screen, i, a, ']');
					break;
				}
			}
		}
		i++;
	}
}

void Game::DrawPauseMenu(nomadshort_t s)
{
	std::ifstream file("Files/gamedata/GS/pausemenu.txt", std::ios::in);
	nomadenum_t i = 0;
	while (std::getline(file, buffer)) {
		mvwprintw(screen, i, 0, "%s\n", buffer.c_str());
		if (i == (s+1)) {
			mvwaddch(screen, i, 2, ' ');
			mvwaddch(screen, i, 3, '[');
			for (nomadenum_t a = 3; a < buffer.length(); a++) {
				if (buffer[a] == ' '
				&& buffer[a+1] == ' ') {
					mvwaddch(screen, i, a, ']');
					break;
				}
			}
		}
		i++;
	}
}