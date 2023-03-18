//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_game.cpp
//  game class constructor/destructor
//----------------------------------------------------------
#include "n_shared.h"
#include "g_obj.h"
#include "s_scripted.h"
#include "g_items.h"
#include "p_npc.h"
#include "g_zone.h"
#include "g_mob.h"
#include "g_map.h"
#include "s_world.h"
#include "scf.h"
#include "g_playr.h"
#include "g_game.h"

nomaduint_t G_GetNumMobs(const Game* const game)
{
	return game->m_Active.size();
}

Game::Game()
{
}

static void set_block(void)
{
#ifdef __unix__
	struct termios ttystate;
	
	// get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	//turn on canonical mode
	ttystate.c_lflag |= ICANON;

	// set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
#endif
}

Game::~Game()
{
	delwin(screen);
	endwin();
//	set_block();
	// now we delete any of the runtime-only resources
	remove("Files/gamedata/RUNTIME/mapfile.txt");
}
