//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_items.cpp
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
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
	return nullptr;
}

void G_SpawnItem(nomaduint_t itemid, nomadlong_t ticker, coord_t& pos)
{
    game->items.emplace_back();
    game->items.back() = (item_t *)Z_Malloc(sizeof(item_t), TAG_STATIC, NULL);
    *game->items.back() = iteminfo[itemid];
    game->items.back()->ticker = ticker;
    game->items.back()->pos = pos;
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