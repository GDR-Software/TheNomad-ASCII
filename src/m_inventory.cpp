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
//  src/m_inventory.cpp
//----------------------------------------------------------
#include "g_game.h"

static Playr* playr;
static Game* game;

void InvAssigner(Game* const gptr)
{
    game = gptr;
    playr = game->playr;
}

inline void Inv_DisplayMercMissions(const std::vector<Mission>& m_ls);
inline void Inv_DisplayItems();

void G_DisplayInventory(void)
{

}