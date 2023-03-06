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
//  src/g_game.cpp
//----------------------------------------------------------
#include "n_shared.h"
#include "g_obj.h"
#include "s_scripted.h"
#include "g_items.h"
#include "p_npc.h"
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

nomaduint_t G_GetNumBots(const Game* const game)
{
	return game->b_Active.size();
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
	set_block();
	// now we delete any of the runtime-only resources
	remove("Files/gamedata/RUNTIME/mapfile.txt");
	W_KillWorld();
	P_KillPlayr();
}
