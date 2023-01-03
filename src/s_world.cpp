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
//  src/s_world.cpp
//----------------------------------------------------------
#include "g_game.h"

static pthread_mutex_t world_mutex;

class World
{
private:
    nomadulong_t time;
    nomadbool_t day;
public:
    World() = default;
    World()
    {
        Z_Free(this);
        pthread_mutex_destroy(&world_mutex);
    }
};

static Game* game;
static World* world;
static nomadulong_t* gametics;

void World_Init(Game* const gptr)
{
    game = gptr;
    gametics = &game->ticcount;
    pthread_mutex_init(&world_mutex, NULL);
}

void* W_Loop(void *arg)
{
    pthread_mutex_lock(&world_mutex);

    pthread_mutex_unlock(&world_mutex);
    return NULL;
}