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
//  src/p_npc.h
//----------------------------------------------------------
#ifndef _P_NPC_
#define _P_NPC_

#include "n_shared.h"
#include "g_obj.h"

#ifdef INITIAL_NPC_ACTIVE
#undef INITIAL_NPC_ACTIVE
#endif

#define INITIAL_NPC_ACTIVE 200

typedef enum : nomadenum_t
{
	BOT_CIVILIAN,
	BOT_GUARD,
	BOT_MERCHANT,
	BOT_MERCMASTER,
	BOT_BARTENDER,
	BOT_NOMAD_CIVILIAN,
	BOT_NOMAD_TRADER,
	BOT_BLACKSMITH,
	BOT_STITCHER,
	BOT_SPLICER,
	BOT_WEAPONSMITH,
	
	NUMBOTTYPES
} bot_t;

typedef struct
{
	nomadushort_t personality;
	nomadushort_t bond;
	nomadushort_t flaw;
	nomadushort_t ideal;
	nomadushort_t goal;
} traits_t;

typedef struct
{
	std::string name;
	sprite_t sprite;
	nomadint_t health;
	nomadushort_t armor;
	traits_t personality;
	bot_t btype;
} npc_t;

extern coord_t botpos[];

extern std::vector<npc_t> npcinfo;

class NPC
{
public:
	npc_t c_npc;

	coord_t pos;
	nomadenum_t ndir;
	entitystate_t nstate;
	nomaduint_t nticker;
	std::atomic<nomadint_t> health;
	std::atomic<nomadushort_t> armor;
public:
	NPC() = default;
	NPC& operator=(const NPC &npc) = delete;
	NPC(const NPC&) = delete;
	NPC(NPC &&) = default;
};

void B_GenNomadTribe();;

void B_GalakasGuardThink(NPC* const npc);
void B_SortiorGuardThink(NPC* const npc);
void B_LatanGuardThink(NPC* const npc);
void B_LatanSmithThink(NPC* const npc);

void B_MercMasterInteract();
void B_BartenderInteract();
void B_WeaponSmithInteract();
void B_BlackSmithInteract();

// for all civilians
void B_CivilianThink(NPC* const npc);
void B_JunkyThink(NPC* const npc);
void B_MerchantThink(NPC* const npc);
void B_NomadThink(NPC* const npc);

#endif
