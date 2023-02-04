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
//  src/g_map.h
//----------------------------------------------------------
#ifndef _G_MAP_
#define _G_MAP_

#include "n_shared.h"

// these vars will most likely change with the version
#if _NOMAD_VERSION == 0
constexpr uint16_t MAP_MAX_Y          = 360;
constexpr uint16_t MAP_MAX_X          = 360;
constexpr uint_fast8_t SECTOR_MAX_Y   = 120;
constexpr uint_fast8_t SECTOR_MAX_X   = 120;
constexpr uint_fast8_t NUM_SECTORS    = 9;
constexpr uint_fast8_t SECTOR_DOD     = 0; // dunes of desolation
constexpr uint_fast8_t SECTOR_AW      = 1; // ancient wastes
constexpr uint_fast8_t SECTOR_SW      = 2; // salt wastes
constexpr uint_fast8_t SECTOR_AP      = 3; // ashen plains
constexpr uint_fast8_t SECTOR_BH      = 4; // burnt hills
constexpr uint_fast8_t SECTOR_SOS     = 5; // sea of sands
constexpr uint_fast8_t SECTOR_DC      = 6; // demagel canyons
constexpr uint_fast8_t SECTOR_FN      = 7; // frozen north
constexpr uint_fast8_t SECTOR_TECOG   = 8; // the eternal city of galakas
#endif

typedef struct
{
	coord_t where;
	nomaduint_t what;
	void *ptr = nullptr;
} collider_t;

// sound level is only ever checked by non-player entities
typedef enum : nomadenum_t
{
	SND_NULL, // sound is blocked
	SND_NONE, // little to no sound
	SND_VLOW, // a little whisper here and there
	SND_LOW, // the normal bit of abience/background sound
	SND_SOME, // a bit more than usual sound
	SND_MEDIUM, // unnatural sound amount
	SND_HIGH, // high amounts of sound
	SND_VHIGH // deafening amounts of sound
} sndlvl_t;

typedef enum : nomadenum_t
{
	SMELL_NULL, // sound is blocked
	SMELL_NONE, // little to no sound
	SMELL_VLOW, // a little whisper here and there
	SMELL_LOW, // the normal bit of abience/background sound
	SMELL_SOME, // a bit more than usual sound
	SMELL_MEDIUM, // unnatural sound amount
	SMELL_HIGH, // high amounts of sound
	SMELL_VHIGH // deafening amounts of sound
} smelllvl_t;

// all entities are pints
typedef struct
{
	nomadushort_t occupied; // entity type
	nomadushort_t ptype; // pint type
//	nomadenum_t color;
	sndlvl_t snd_lvl;
	smelllvl_t smell_lvl;
} __attribute__((packed))pint_t;

typedef struct
{
	coord_t coords;
	nomadushort_t spawn_chance;
	nomaduint_t etype;
	sprite_t sprite;
	nomadenum_t sector_id;
} spawn_t;

#endif
