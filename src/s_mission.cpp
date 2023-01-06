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

Mission* G_GenMission()
{
    srand(time(NULL)); // set a brand-new random seed
    Mission* m = (Mission*)Z_Malloc(sizeof(Mission), TAG_MISSION, &m);
    if (((P_Random() & 99)+1) > 24) { // use a mission seed if > 24, this way, its a rare occurance to get a weird/funky/unique mission
        nomadenum_t mission = rand() % NUMMISSIONTYPES;
        const missionseed_t *seed = &missionseeds[mission];
        while (seed->difficulty[0] > game->difficulty) {
            ++mission;
            seed = &missionseeds[mission];
        };
#ifdef _NOMAD_DEBUG
        LOG("rolled greater than/equal to 25, assigning mission seed to the mission, seed: %hu", seed->type);
#endif
        m->type = seed->type;
        m->rng = seed->rng;
        m->mobtypes = seed->mobtypes;
        m->mindif = seed->difficulty[0];
        m->maxdif = seed->difficulty[1];
        m->minbounty = seed->bounty[0];
        m->maxbounty = seed->bounty[1];
    }
    else {
#ifdef _NOMAD_DEBUG
        LOG("rolled less than 25, rolling random values for all mission variables");
#endif
        switch (game->difficulty) {
        case DIF_NOOB:
            m->rng = (P_Random()&25)+2;
            m->minbounty = (P_Random()&100)+m->rng;
            m->maxbounty = ((P_Random()&150)+m->minbounty)+m->rng;
            break;
        case DIF_RECRUIT:
            m->rng = (P_Random()&40)+7;
            m->minbounty = (P_Random()&150)+m->rng;
            m->maxbounty = ((P_Random()&250)+m->minbounty)+m->rng;
            break;
        case DIF_MERC:
            m->rng = (P_Random()&60)+6;
            m->minbounty = (P_Random()&250)+m->rng;
            m->maxbounty = ((P_Random()&500)+m->minbounty)+m->rng;
            break;
        case DIF_NOMAD:
            m->rng = (P_Random()&80)+15;
            m->minbounty = (P_Random()&500)+m->rng;
            m->maxbounty = ((P_Random()&900)+m->minbounty)+m->rng;
            break;
        case DIF_BLACKDEATH:
            m->rng = (P_Random()&90)+10;
            m->minbounty = (P_Random()&1000)+m->rng;
            m->maxbounty = ((P_Random()&2500)+m->minbounty)+m->rng;
            break;
        case DIF_MINORINCONVENIENCE:
            m->rng = (P_Random()&150)+50; // they're asking for it
            m->minbounty = (P_Random()&4000)+m->rng;
            m->maxbounty = ((P_Random()&9000)+m->minbounty)+m->rng;
            break;
        default: // use normal difficulty values, and set the difficulty to medium
#ifdef _NOMAD_DEBUG
            LOG("Unknown/Invalid Game Difficulty: %hu", game->difficulty);
#endif
            m->rng = (P_Random()&60)+6;
            m->minbounty = (P_Random()&250)+m->rng;
            m->maxbounty = ((P_Random()&500)+m->minbounty)+m->rng;
            game->difficulty = DIF_MERC;
            break;
        };
        m->type = missionseeds[(P_Random() & m->rng)].type;
        for (nomadenum_t i = 0;;) {
            if (i == NUMMOBS) break;
            nomadenum_t mtype = rand() % NUMMOBS;
            for (nomadenum_t a = 0; a < m->mobtypes.size(); ++a) {
                if (mtype == m->mobtypes[i]) {
                    mtype = rand() % NUMMOBS; // attempting to get some unique mobs in the mix there, but cant do much without getting an RNG lock
                }
            }
            if ((P_Random() & 100) > 25) {
                m->mobtypes.push_back(mtype);
                ++i;
            }
            else {
                continue;
            }
        }
        m->mindif = m->maxdif = game->difficulty;
    }

    std::vector<Mob*>& m_Active = game->m_Active;
    nomadenum_t nummobs = 0;
    nomadenum_t maxmobs = ((P_Random() & 70) + m->rng)+1;
#ifdef _NOMAD_DEBUG
    LOG("maxmobs: %hu", maxmobs);
#endif
    while (m_Active.size() <= (INITIAL_MOBS_ACTIVE*2) || nummobs < maxmobs) {
        m_Active.emplace_back();
        m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_MISSION, &m_Active.back());
        Mob* mob = m_Active.back();
        mob->c_mob = mobinfo[(rand() % m->mobtypes.size())];
        mob->health += P_Random() & m->rng; // give them either + 0 or a little buff
        mob->armor += P_Random() & m->rng;
    };
    if (m->type == M_ASSASSINATION || M_KIDNAPPING) {
#ifdef _NOMAD_DEBUG
        LOG("mission type = M_ASSASSINATION or M_KIDNAPPING, generatng high-value target mob");
#endif
        switch (game->difficulty) {
        case DIF_NOOB:
            break;
        case DIF_RECRUIT:
            break;
        case DIF_MERC:
            break;
        case DIF_NOMAD:
            break;
        case DIF_BLACKDEATH:
            break;
        case DIF_MINORINCONVENIENCE:
            break;
        default:
            break;
        };
        //mob->c_mob.health = (P_Random() & 200)+m->rng;
    };
    
    return m;
}