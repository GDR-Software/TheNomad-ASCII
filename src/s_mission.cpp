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
//  src/s_mission.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "g_rng.h"
#include "s_mission.h"

Mission::~Mission()
{
    for (auto* i : m_itemloot)
        Z_Free(i);
    for (auto* i : m_wpnloot)
        Z_Free(i);
    Z_Free(this);
}

static Game* game;

void MissionAssigner(Game* const gptr)
{
    game = gptr;
}

Mission* G_GenMission()
{
    srand(time(NULL)); // set a brand-new random seed
    Mission* m = (Mission*)Z_Malloc(sizeof(Mission), TAG_STATIC, &m);
    const missionseed_t* seed;
    if (((P_Random() & 99)+1) > 24) { // use a mission seed if > 24, this way, its a rare occurance to get a weird/funky/unique mission
        seed = &missionseeds[rand() % NUMMISSIONTYPES];
        m->type = seed->type;
        m->rng = seed->rng;
        m->mobtypes = seed->mobtypes;
        m->mindif = &seed->difficulty[0];
        m->maxdif = &seed->difficulty[1];
        m->minbounty = &seed->bounty[0];
        m->maxbounty = &seed->bounty[1];
    }
    else {
        return nullptr; // TODO: this
    }
    return m;
}