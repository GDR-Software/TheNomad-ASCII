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
//  src/s_world.h
//----------------------------------------------------------
#ifndef _S_WORLD_
#define _S_WORLD_

#pragma once

#include "g_game.h"

typedef struct
{
    std::atomic<nomadulong_t>* ticcount;
    nomadulong_t month;
    nomadulong_t year;
    nomadulong_t day;
} worldtime_t;

class World
{
public:
    worldtime_t time;
    nomadbool_t day; // whether its daytime or not
    nomadint_t temperature; // the average temperature of the world
public:
    World(){}
    ~World(){}
    World& operator=(const World& world) {
    	memcpy(&(*this), &world, sizeof(World));
    	return *this;
    }
    void GenDungeon(void);
};

void P_Pickup(Weapon* wpn);
void P_Pickup(Weapon& wpn);
void P_Pickup(Item& item);
void P_Pickup(Item* item);
void W_KillWorld();

#endif
