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

#define SORT_ABC      0b0000000000 // sort the inventory alphabetically
#define SORT_PRICEY   0b0000000001 // sort based on monetary value
#define SORT_WEIGHT   0b0000000010 // sort based on high-to-low weight

inline void Inv_DisplayMercMissions(const std::vector<Mission>& m_ls);
inline void Inv_DisplayItems();
/*
static inline void Inv_SortItemsABC(void)
{
    std::sort(playr->inv.begin(), playr->inv.end(),
        [](const Item* a, const Item* b){ return a->c_item.name < b->c_item.name; } );
}
static inline void Inv_SortItemPricey(void)
{
    std::sort(playr->inv.begin(), playr->inv.end(),
        [](const Item* a, const Item* b){ return a->c_item.item_cost < b->c_item.item_cost; } );
}
static inline void Inv_SortItemsWeight(void)
{
    std::sort(playr->inv.begin(), playr->inv.end(),
        [](const Item* a, const Item* b){ return a->c_item.item_weight < b->c_item.item_cost; });
}
inline void Inv_SortItems(nomadenum_t sorter)
{
    
    switch (sorter) {
    case SORT_ABC:
        Inv_SortItemsABC();
        break;
    case SORT_PRICEY:
        Inv_SortItemPricey();
        break;
    case SORT_WEIGHT:
        Inv_SortItemsWeight();
        break;
    default:
        N_Error("Unknown/Invalid sorter value for Inv_SortItems! sorter = %hu", sorter);
        break;
    };
}
*/

void G_DisplayInventory(void)
{

}