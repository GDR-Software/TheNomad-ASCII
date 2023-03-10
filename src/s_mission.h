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
//  src/s_mission.h
//----------------------------------------------------------
#ifndef _S_MISSION_
#define _S_MISSION_

#pragma once


class Mob;

typedef enum : nomadenum_t
{
    M_ASSASSINATION,
    M_KIDNAPPING,
    M_EXTORTION,
    M_EXTRACTION,
    M_BODYGUARDING,
    M_CONTRACT, // will almost never come up unless there's an in-game war (could be anything from a gang war to a full-blown world war), the longest and toughest of all the missions
    M_INFILTRATION, // kinda-sorta spy-work, can also just be a grab-information-and-go, varies
//    M_HACKING, // will only come up if the player has some stats into tech - needs further development

    NUMMISSIONTYPES
} missiontype_t;

typedef struct
{
    missiontype_t type;
    nomadenum_t rng;
    std::vector<nomadenum_t> mobtypes; // a list of all the mobs allowed to be spawned for the specific mission seed, for lore's sake and balance (kinda)
    nomadenum_t difficulty[2]; // min, max
    nomaduint_t bounty[2]; // min, max (how much money can be awarded as a bounty of the mission)
} missionseed_t;

class Target
{
public:
    nomadint_t health;
    nomadushort_t armor;
    std::string name;
};

class Mission
{
public: // stuff determined before/during the mission
    area_t area;
    nomadenum_t sector;
    missiontype_t type;
    nomadenum_t rng;
    nomadenum_t maxdif, mindif;
    nomaduint_t maxbounty, minbounty;
    std::vector<nomadenum_t> mobtypes;
public: // during the mission stuff
    std::vector<Mob*> m_Active;
    std::vector<NPC*> b_Active;
public: // bounty/rewards after completion of the mission
    std::vector<nomaduint_t> m_coins; // bounty of said mission
    std::vector<nomaduint_t> m_itemloot; // item-specific loot gained after completion
    std::vector<nomaduint_t> m_wpnloot; // same thing as the item vector, but its weapons
public:
    Mission() = default;
    ~Mission();
    Mission& operator=(const Mission &m);
};

void MissionAssigner(Game* const gptr);
void G_GenMissionLs(std::vector<Mission>& m_ls);

extern const char* missionstrings[NUMMISSIONTYPES];
extern const missionseed_t missionseeds[NUMMISSIONTYPES];

#endif
