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
//  src/g_combat.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "g_playr.h"
#include "g_mob.h"
#include "g_items.h"

typedef struct
{
	coord_t pos;
	uint8_t etype;
} entity_loc_t;

void Playr::P_DamageMobYX(Game& game, coord_t& m_pos, uint_fast8_t dmg)
{
	for (uint_fast8_t i = 0; i < game.m_Active.size(); i++) {
		if (game.m_Active[i].pos == m_pos) {
		//	game.
		}
	}
}

static entity_loc_t HitscanCollider(uint8_t& dir, coord_t& pos, 
	uint16_t& wpnid, const std::unique_ptr<Map>& pmap)
{
	entity_loc_t hit;
	hit.pos.y = NULL;
	hit.pos.x = NULL;
	hit.etype = NULL;
	int8_t y, x, d;
	switch (dir) {
	case D_NORTH:
		y = -1; x = 0; d = -1;
		break;
	case D_EAST:
		y = 0; x = -1; d = -1;
		break;
	case D_SOUTH:
		y = 1; x = 0; d = 1;
		break;
	case D_WEST:
		y = 0; x = 1; d = 1;
		break;
	default:
#ifdef DEVMODE
		gameError(VAR_IS_INVALID_VALUE);
#endif
		return hit;
	};
	if (dir == D_NORTH || D_WEST) {
		for (int16_t y = 0; y < wpninfo[wpnid + WI_RANGE]; y++) {
			for (int16_t x =)
				if (pmap->c_map[])
		}
	}
	else if (dir == D_SOUTH || D_EAST) {
		
	}
}

void Playr::ShootShotty(const std::unique_ptr<Map>& pmap)
{
	entity_loc_t hit = HitscanCollider(pdir, pos, wpn_in_hand[0], pmap);
	int16_t spread = wpninfo[wpn_in_hand[0] + WI_SPREAD];
	uint16_t pellets = wpninfo[wpn_in_hand[0] + WI_NUMBULLETS];
	uint16_t rng = wpninfo[wpn_in_hand[0] + WI_RNG];
	int16_t nspread[2];
	if (pdir == D_NORTH || D_SOUTH) {
		nspread[0] = px - (spread >> 1); // west
		nspread[1] = px + (spread >> 1); // east
	}
	else if (pdir == D_EAST || D_WEST) {
		nspread[0] = py - (spread >> 1); // north
		nspread[1] = py + (spread >> 1); // south
	}
#ifdef EXPERIMENTAL // doesn't work rn, don't use
	coord_t dspread[2];
	else if (pdir == D_NORTHEAST) {
		// left
		dspread[0].y = py - (spread >> 1);
		dspread[0].x = px - (spread >> 1);
		// right
		dspread[1].y = py + (spread >> 1);
		dspread[1].x = px + (spread >> 1);
	}
	else if (pdir == D_SOUTHEAST) {
		// left
	//	dspread[0]
		// right
		
	}
	else if (pdir == D_NORTHWEST) {
		
	}
	else if (pdir == D_SOUTHWEST) {
		
	}
#endif
#ifdef DEVMODE
	else {
		gameError(HOW_DID_YOU_GET_HERE);
	}
#endif
	for (uint8_t o = 0; o < pellets; o++) {
		uint8_t hashit = (P_random() + rng) & 110;
		int8_t offset;
		int8_t which = nspread[rand() % 1];
		if ((rand() % 2) == 2) {
			offset = P_random() & 2;
		}
		else if ((rand() % 2) == 1) {
			// NOTE: this might need fixing, its
			// meant to be a negative offset
			offset = -P_random() & 2;
		}
		else {
			offset = 0;
		}
		switch (pdir) {
		case D_NORTH:
			if (pmap->c_map[pos.y][which] == ET_MOB) {
				P_DamageMobYX({pos.y, which});
			}
		case D_EAST:
		case D_SOUTH:
		case D_WEST:
		default:
#ifdef DEVMODE
			gameError(OK_HOW_THE_FUCK_DID_YOU_GET_HERE);
#endif
			break;
		};
	}
}

// used for any semi-auto weapon except shotguns
void Playr::ShootSingle(const std::unique_ptr<Map>& pmap)
{
	
}

void Playr::ShootBurst3(const std::unique_ptr<Map>& pmap)
{
	
}

void Playr::ShootBurst4(const std::unique_ptr<Map>& pmap)
{
	
}