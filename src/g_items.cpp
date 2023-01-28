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
//  src/g_items.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "g_rng.h"

Item::~Item()
{
    Z_Free(this);
}

static Game* game;
static Playr* playr;

#define rightarm      playr->P_wpns[0]
#define leftarm       playr->P_wpns[1]
#define sidearm       playr->P_wpns[2]
#define heavyside     playr->P_wpns[3]
#define primary       playr->P_wpns[4]
#define heavyprimary  playr->P_wpns[5]
#define shotty        playr->P_wpns[6]
#define launcher      playr->P_wpns[7]
#define melee1        playr->P_wpns[8]
#define meele2        playr->P_wpns[9]
#define melee3        playr->P_wpns[10]

void ItemAssigner(Game* const gptr)
{
    game = gptr;
    playr = game->playr;
}

money_t currency_convert(money_t from, nomadenum_t to)
{
    if (from.type == to)
        return from;
    
    money_t out;
    nomaduint_t mult = 0;
    for (nomadenum_t i = 0; i < to; ++i)
        mult += 100;
    
    out.amount = from.amount / mult;
    out.type = to;
    return out;
}

// called at init stage, but is only meant to give the player the starter weapons and items,
// these will, however, be replaced when the player either loads a save file, or simply plays the game
void P_GiveDefault()
{
    rightarm->c_wpn = wpninfo[W_ARM_HB];
    leftarm->c_wpn = wpninfo[W_ARM_GRAPPLE];
    sidearm->c_wpn = wpninfo[W_SIDE_PLASMA];
    heavyside->c_wpn = wpninfo[W_HSIDE_SADB];
    primary->c_wpn = wpninfo[W_PRIM_AK77];
    heavyprimary->c_wpn = wpninfo[W_HPRIM_RAG13];
    shotty->c_wpn = wpninfo[W_SHOTTY_QS];
    launcher->c_wpn = wpninfo[W_LAUNCHER_MGL];
}

void P_GiveArmor(nomadenum_t armor)
{
    
}

// called during init stage, as this list'll be the rather permanent one
std::vector<Weapon*> G_GenWeaponList()
{
    std::vector<Weapon*> list;
    nomadenum_t numwpns = (P_Random() & 65)+1;
    list.reserve(numwpns);
    for (nomadshort_t i = 0; i < numwpns; ++i) {
        list.emplace_back();
        list.back() = (Weapon *)Z_Malloc(sizeof(Weapon), TAG_STATIC, &list.back());
        Weapon* const wpn = list.back();
    }
    return list;
}

inline void G_RemoveFromList(Weapon* wpn, std::vector<Weapon*>& list)
{
    for (nomadshort_t i = 0; i < list.size(); ++i) {
        if (wpn == list[i]) {
            list.erase(list.begin()+i);
            Z_Free(wpn);
            break;
        }
    }
}

inline void G_RemoveFromList(Item* item, std::vector<Item*>& list)
{
    for (nomadshort_t i = 0; i < list.size(); ++i) {
        if (item == list[i]) {
            list.erase(list.begin()+i);
            Z_Free(item);
            break;
        }
    }
}

inline void G_DeleteList(std::vector<Weapon*>& list)
{
    for (auto* i : list)
        Z_Free(i);
    list.clear();
}

inline void G_DeleteList(std::vector<Item*>& list)
{
    for (auto* i : list)
        Z_Free(i);
    list.clear();
}