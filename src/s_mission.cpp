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
    Z_FreeTags(TAG_MISSION, TAG_MISSION); // only free the mission memory
}

static Game* game;

void MissionAssigner(Game* const gptr)
{
    game = gptr;
}

void G_GenMissionLs(std::vector<Mission>& m_ls)
{
    srand(time(NULL));
    nomadenum_t nummissions = (rand() % 15)+5;
    m_ls.reserve(nummissions);
    for (nomadenum_t i = 0; i < nummissions; ++i) {
        if (((P_Random() & 99)+1) > 24) {
            m_ls.emplace_back();
            Mission& m = m_ls.back();
            const missionseed_t* seed = &missionseeds[(P_Random() & NUMMISSIONTYPES)];
            if (seed->difficulty[0] > game->difficulty)
                m.mindif = game->difficulty;
            while (seed->difficulty[1] > game->difficulty || seed->difficulty[0] < game->difficulty)
                seed = &missionseeds[(rand() % NUMMISSIONTYPES)];
            
            m.type = seed->type;
            m.mobtypes = seed->mobtypes;
            m.minbounty = seed->bounty[0];
            m.maxbounty = seed->bounty[1];
            m.mindif = seed->difficulty[0];
            m.maxdif = seed->difficulty[1];
        }
    }
}