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
#include "g_game.h"
#include "scf.h"

nomaduint_t G_GetNumMobs(const Game* const game)
{
	nomaduint_t count = 0;
	for (const auto* i : game->m_Active) {
		if (i != nullptr) ++count;
	}
	return count;
}

nomaduint_t G_GetNumBots(const Game* const game)
{
	nomaduint_t count = 0;
	for (const auto* i : game->b_Active) {
		if (i != nullptr) ++count;
	}
	return count;
}

nomaduint_t G_GetFreeBot(const Game* const game)
{
	for (nomaduint_t i = 0; i < ARRAY_SIZE(game->b_Active); ++i) {
		if (game->b_Active[i] != nullptr) {
			return i;
		}
	}
	return MAX_NPC_ACTIVE;
}

nomaduint_t G_GetFreeMob(const Game* const game)
{
	for (nomaduint_t i = 0; i < ARRAY_SIZE(game->m_Active); ++i) {
		if (game->m_Active[i] != nullptr) {
			return i;
		}
	}
	return MAX_MOBS_ACTIVE;
}

Game::Game()
{
}

#ifdef __unix__
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
#endif

Game::~Game()
{
	pthread_mutex_destroy(&playr_mutex);
	pthread_mutex_destroy(&mob_mutex);
	pthread_mutex_destroy(&npc_mutex);
	if (gamestate == GS_LEVEL) {
		pthread_kill(wthread, SIGTERM);
		pthread_kill(mthread, SIGTERM);
		pthread_kill(nthread, SIGTERM);
	}
	delwin(screen);
	attroff(COLOR_PAIR(0));
	endwin();
#ifdef UNIX_NOMAD
	set_block();
#endif
	// now we delete any of the runtime-only resources
	remove("Files/gamedata/RUNTIME/mapfile.txt");
	W_KillWorld();
	P_KillPlayr();
}
