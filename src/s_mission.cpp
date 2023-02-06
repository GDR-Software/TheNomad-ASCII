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

Mission& Mission::operator=(const Mission& m)
{
    Mission& m1 = *this;
    m1.area = m.area;
    std::copy(m.b_Active.begin(), m.b_Active.end(), std::back_inserter(m1.b_Active));
    std::copy(m.m_Active.begin(), m.m_Active.end(), std::back_inserter(m1.m_Active));
    std::copy(m.m_coins.begin(), m.m_coins.end(), std::back_inserter(m1.m_coins));
    std::copy(m.m_itemloot.begin(), m.m_itemloot.end(), std::back_inserter(m1.m_itemloot));
    std::copy(m.m_wpnloot.begin(), m.m_wpnloot.end(), std::back_inserter(m1.m_wpnloot));
    std::copy(m.mobtypes.begin(), m.mobtypes.end(), std::back_inserter(m1.mobtypes));
    m1.maxbounty = m.maxbounty;
    m1.maxdif = m.maxdif;
    m1.minbounty = m.minbounty;
    m1.mindif = m.mindif;
    m1.type = m.type;
    m1.rng = m.rng;
    m1.sector = m.sector;
    return m1;
}

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
//        if (((P_Random() & 99)+1) > 24) {
            m_ls.emplace_back();
            Mission& m = m_ls.back();
            const missionseed_t* seed = &missionseeds[(P_Random() & NUMMISSIONTYPES)];
            if (seed->difficulty[0] > game->difficulty) {
                m.mindif = game->difficulty;
            }
            while (seed->difficulty[1] > game->difficulty || seed->difficulty[0] < game->difficulty) {
                seed = &missionseeds[(rand() % NUMMISSIONTYPES)];
            }
            
            m.type = seed->type;
            m.mobtypes = seed->mobtypes;
            m.minbounty = seed->bounty[0];
            m.maxbounty = seed->bounty[1];
            m.mindif = seed->difficulty[0];
            m.maxdif = seed->difficulty[1];
//        }
    }
}