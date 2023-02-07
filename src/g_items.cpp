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

static Game* game;
static Playr* playr;

void ItemAssigner(Game* const gptr)
{
    game = gptr;
    playr = game->playr;
}

const char* GetWeaponNameFromId(nomaduint_t id)
{
    switch (id) {
    case W_SHOTTY_ADB: return VAR_TO_STR(W_SHOTTY_ADB);
    case W_SHOTTY_FAB: return VAR_TO_STR(W_SHOTTY_FAB);
    case W_SHOTTY_QS: return VAR_TO_STR(W_SHOTTY_QS);
    case W_SIDE_AUP: return VAR_TO_STR(W_SIDE_AUP);
    case W_SIDE_BOS: return VAR_TO_STR(W_SIDE_BOS);
    case W_SIDE_FUSION: return VAR_TO_STR(W_SIDE_FUSION);
    case W_SIDE_ION: return VAR_TO_STR(W_SIDE_ION);
    case W_SIDE_PLASMA: return VAR_TO_STR(W_SIDE_PLASMA);
    case W_HSIDE_A8SHOT: return VAR_TO_STR(W_HSIDE_A8SHOT);
    case W_HSIDE_SADB: return VAR_TO_STR(W_HSIDE_SADB);
    case W_PRIM_AK77: return VAR_TO_STR(W_PRIM_AK77);
    case W_PRIM_M23C5: return VAR_TO_STR(W_PRIM_M23C5);
    case W_PRIM_PLASMASMG: return VAR_TO_STR(W_PRIM_PLASMASMG);
    case W_PRIM_RAG14: return VAR_TO_STR(W_PRIM_RAG14);
    case W_PRIM_RAG15: return VAR_TO_STR(W_PRIM_RAG15);
    case W_HPRIM_RAG13: return VAR_TO_STR(W_HPRIM_RAG13);
    case W_HPRIM_DR: return VAR_TO_STR(W_HPRIM_DR);
    case W_HPRIM_FUSION: return VAR_TO_STR(W_HPRIM_FUSION);
    case W_HPRIM_HOSIG: return VAR_TO_STR(W_HPRIM_HOSIG);
    case W_ARM_FT: return VAR_TO_STR(W_ARM_FT);
    case W_ARM_GRAPPLE: return VAR_TO_STR(W_ARM_GRAPPLE);
    case W_ARM_HB: return VAR_TO_STR(W_ARM_HB);
    case W_ARM_HC: return VAR_TO_STR(W_ARM_HC);
    case W_ARM_SB: return VAR_TO_STR(W_ARM_SB);
    case W_MELEE_BALLISTA: return VAR_TO_STR(W_MELEE_BALLISTA);
    case W_MELEE_BASTARD: return VAR_TO_STR(W_MELEE_BASTARD);
    case W_MELEE_CROSSBOW: return VAR_TO_STR(W_MELEE_CROSSBOW);
    case W_MELEE_DAGGER: return VAR_TO_STR(W_MELEE_DAGGER);
    case W_MELEE_KANTANA: return VAR_TO_STR(W_MELEE_KANTANA);
    case W_MELEE_LONG: return VAR_TO_STR(W_MELEE_LONG);
    case W_MELEE_MACE: return VAR_TO_STR(W_MELEE_MACE);
    case W_MELEE_MORNING: return VAR_TO_STR(W_MELEE_MORNING);
    case W_MELEE_SHORT: return VAR_TO_STR(W_MELEE_SHORT);
    case W_MELEE_TANTO: return VAR_TO_STR(W_MELEE_TANTO);
    };
    if (!false)
        N_Error("Invalid weapon id %iu!", id);
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
void P_GiveDefault(void)
{
    rightarm.c_wpn = wpninfo[W_ARM_HB];
    leftarm.c_wpn = wpninfo[W_ARM_GRAPPLE];
    sidearm.c_wpn = wpninfo[W_SIDE_PLASMA];
    heavyside.c_wpn = wpninfo[W_HSIDE_SADB];
    primary.c_wpn = wpninfo[W_PRIM_AK77];
    heavyprimary.c_wpn = wpninfo[W_HPRIM_RAG13];
    shotty.c_wpn = wpninfo[W_SHOTTY_QS];
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