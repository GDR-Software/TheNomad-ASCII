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
//  src/s_saveg.cpp
//----------------------------------------------------------
#include "g_game.h"
#include <limits.h>

static FILE* fp;
static constexpr uint64_t HEADER = 0x5f3759df;
static constexpr auto svfile = "nomadsv.ngd";

static void G_ArchiveWeapon(entitytype_t entity, std::ofstream& file, const Weapon& wpn)
{
	file
	<< "    {\n";
	switch (entity) {
	case ET_PLAYR:
		file
	<< "        entity : PLAYR\n";
		break;
	case ET_MOB:
		file
	<< "        entity : MOB\n";
		break;
	case ET_NPC:
		file
	<< "        entity : NPC\n";
		break;
	};
	file
	<< "        damage : " << wpn.c_wpn.dmg << '\n'
	<< "        range : " << wpn.c_wpn.range << '\n'
	<< "        rng : " << wpn.c_wpn.rng << '\n'
	<< "        spread : " << wpn.c_wpn.spread << '\n'
	<< "        id : " << wpn.c_wpn << '\n'
	<< "    }\n";
}
static void G_UnArchiveWeapon(entitytype_t entity, std::ofstream& file, Weapon& wpn);

static void G_ArchivePlayr(const Playr* playr, std::ofstream& file);
static void G_UnArchivePlayr(Playr* const playr);
static void G_ArchiveWorld(const World* world, std::ofstream& file);
static void G_UnArchiveWorld(World* const world);
static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, std::ofstream& file);
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active);
static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active, std::ofstream& file);
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active);

void Game::G_SaveGame(void)
{
	Z_CheckHeap();
	// std::fstreams gave me shit when I tried this stuff,
	// if you can do this better, then pls do. I accept pulls
	std::ofstream file("nomadsv.ngd", std::ios::out);
	if (file.fail())
		return;

	file
	<< "[header]\n"
	<< "{\n"
	<< "\tversion : " << _NOMAD_VERSION << '\n'
	<< "\tversion.update : " << _NOMAD_VERSION_UPDATE << '\n'
	<< "\tversion.patch : " << _NOMAD_VERSION_PATCH << '\n'
	<< "}\n";
	
	G_ArchivePlayr(playr, file);
}

bool Game::G_LoadGame(void)
{
}

static void G_ArchivePlayr(const Playr* playr, std::ofstream& file)
{
	file
	<< "[player data]\n"
	<< "{\n"
	<< "    name : " << playr->name << '\n'
	<< "    sprite : " << playr->sprite << '\n'
	<< "    health : " << playr->health << '\n'
	<< "    armor : " << playr->armor << '\n'
	<< "    coin : " << playr->coin << '\n'
	<< "    pos : " << playr->pos.y << ", " << playr->pos.x << '\n'
	<< "    pstate : " << playr->pstate << '\n'
	<< "    [c_wpn]\n";
	G_ArchiveWeapon(ET_PLAYR, file, *playr->c_wpn);
	file
	<< "    [sidearm]\n";
	G_ArchiveWeapon(ET_PLAYR, file, sidearm);
	file
	<< "    [heavyside]\n";
	G_ArchiveWeapon(ET_PLAYR, file, heavyside);
	file
	<< "    [leftarm]\n";
	G_ArchiveWeapon(ET_PLAYR, file, leftarm);
	file
	<< "    [rightarm]\n";
	G_ArchiveWeapon(ET_PLAYR, file, rightarm);
	file
	<< "    [primary]\n";
	G_ArchiveWeapon(ET_PLAYR, file primary);
	file
	<< "    [heavyprimary]\n";
	G_ArchiveWeapon(ET_PLAYR, file, heavyprimary);
	file
	<< "    [shotty]\n";
	G_ArchiveWeapon(ET_PLAYR, file, shotty);
	file
	<< "    [melee1]\n";
	G_ArchiveWeapon(ET_PLAYR, file, melee1);
	file
	<< "    [melee2]\n";
	G_ArchiveWeapon(ET_PLAYR, file, melee2);
	file
	<< "    [melee3]\n";
	G_ArchiveWeapon(ET_PLAYR, file, melee3);
	file
	<< "}\n";
}
static void G_UnArchivePlayr(Playr* const playr)
{
}

static void G_ArchiveWorld(const World* world)
{
}
static void G_UnArchiveWorld(World* const world)
{
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active)
{
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active)
{
}

static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active)
{
}
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active)
{
}