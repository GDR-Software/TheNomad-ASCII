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

static void G_ArchivePlayr(const Playr* playr);
static void G_UnArchivePlayr(Playr* const playr);
static void G_ArchiveWorld(const World* world);
static void G_UnArchiveWorld(World* const world);
static void G_ArchiveMobs(const std::vector<Mob*>& m_Active);
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active);
static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active);
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active);

void Game::G_SaveGame(void)
{
	Z_CheckHeap();
	// std::fstreams gave me shit when I tried this stuff,
	// if you can do this better, then pls do. I accept pulls
	fp = fopen(svfile, "wb");
	if (!fp) {
		return;
	}
	fwrite(&HEADER, 1, sizeof(HEADER), fp);
	G_ArchivePlayr(playr);
	G_ArchiveWorld(world);
	G_ArchiveMobs(m_Active);
	G_ArchiveNPCs(b_Active);
	fclose(fp);
}

bool Game::G_LoadGame(void)
{
	struct stat fdata;
	if (stat(svfile, &fdata) == -1)
		return false;
	fp = fopen(svfile, "rb");
	if (!fp)
		return false;
#ifdef _NOMAD_DEBUG
	assert(fp && fdata.st_size > 0);
	LOG("Successfully Opened Save File, and Size is Greater Than 0");
#endif
	uint64_t header;
	fread(&header, 1, sizeof(uint64_t), fp);
	
	// not a valid save file
	if (!(header & HEADER))
		return false;
#ifdef _NOMAD_DEBUG
	assert(header & HEADER);
	LOG("Save File is of The Correct Format (.NGD), Header is Correct Magic Number");
#endif

	G_UnArchivePlayr(playr);
	G_UnArchiveWorld(world);
	G_UnArchiveMobs(m_Active);
	G_UnArchiveNPCs(b_Active);
	fclose(fp);
	Z_CheckHeap();
	return true;
}

static void G_ArchivePlayr(const Playr* playr)
{
#ifdef _NOMAD_DEBUG
	assert(playr);
	LOG("Archiving Player Data");
#endif
	short size = (short)playr->name.size();
	fwrite(&size, sizeof(short), 1, fp);
	fwrite((char*)&playr->name, sizeof(char), size, fp);
	fwrite(&playr->health, sizeof(playr->health), 1, fp);
	fwrite(&playr->armor, sizeof(playr->armor), 1, fp);
	fwrite(&playr->pos.y, sizeof(playr->pos.y), 1, fp);
	fwrite(&playr->pos.x, sizeof(playr->pos.x), 1, fp);
	fwrite(&playr->coin, sizeof(playr->coin), 1, fp);
	fwrite(&playr->lvl, sizeof(playr->lvl), 1, fp);
	fwrite(&playr->vmatrix, sizeof(char), sizeof(playr->vmatrix), fp);
	fwrite(&playr->P_wpns, sizeof(Weapon*), sizeof(playr->P_wpns), fp);
}
static void G_UnArchivePlayr(Playr* const playr)
{
#ifdef _NOMAD_DEBUG
	assert(playr);
	LOG("Unarchiving Player Data");
#endif
	short size;
	fread(&size, sizeof(short), 1, fp);
	fread((char*)&playr->name, sizeof(char), size, fp);
	fread(&playr->health, sizeof(playr->health), 1, fp);
	fread(&playr->armor, sizeof(playr->armor), 1, fp);
	fread(&playr->pos.y, sizeof(playr->pos.y), 1, fp);
	fread(&playr->pos.x, sizeof(playr->pos.x), 1, fp);
	fread(&playr->coin, sizeof(playr->coin), 1, fp);
	fread(&playr->lvl, sizeof(playr->lvl), 1, fp);
	fread(&playr->vmatrix, sizeof(char), sizeof(playr->vmatrix), fp);
	fread(&playr->P_wpns, sizeof(Weapon*), sizeof(playr->P_wpns), fp);
}

static void G_ArchiveWorld(const World* world)
{
#ifdef _NOMAD_DEBUG
	assert(world);
#endif
	fwrite(&world->time.month, sizeof(world->time.month), 1, fp);
	fwrite(&world->time.year, sizeof(world->time.year), 1, fp);
	fwrite(&world->time.day, sizeof(world->time.day), 1, fp);
	fwrite(&world->day, sizeof(world->day), 1, fp);
}
static void G_UnArchiveWorld(World* const world)
{
#ifdef _NOMAD_DEBUG
	assert(world);
#endif
	fread(&world->time.month, sizeof(world->time.month), 1, fp);
	fread(&world->time.year, sizeof(world->time.year), 1, fp);
	fread(&world->time.day, sizeof(world->time.day), 1, fp);
	fread(&world->day, sizeof(world->day), 1, fp);
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active)
{
#ifdef _NOMAD_DEBUG
	assert(m_Active.size() > 0); // there will always be active mobs whilst the player is in the campaign mode
	LOG("Archiving Mob Data");
#endif
	short size = (short)m_Active.size();
	fwrite(&size, sizeof(short), 1, fp);
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		Mob* mob = m_Active[i];
#ifdef _NOMAD_DEBUG
		assert(mob);
#endif
		fwrite(&mob->c_mob.mtype, sizeof(mob->c_mob.mtype), 1, fp);
		fwrite(&mob->health, sizeof(mob->health), 1, fp);
		fwrite(&mob->armor, sizeof(mob->armor), 1, fp);
		fwrite(&mob->mpos.y, sizeof(mob->mpos.y), 1, fp);
		fwrite(&mob->mpos.x, sizeof(mob->mpos.x), 1, fp);
		fwrite(&mob->mstate.id, sizeof(mob->mstate.id), 1, fp);
		fwrite(&mob->mticker, sizeof(mob->mticker), 1, fp);
	}
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active)
{
#ifdef _NOMAD_DEBUG
	assert(m_Active.size() > 0);
	LOG("Unarchiving Mob Data");
#endif
	nomadshort_t i;
	for (i = 0; i < m_Active.size(); ++i) {
		Z_Free(m_Active[i]);
#ifdef _NOMAD_DEBUG
		assert(!m_Active[i]);
#endif
	}
	m_Active.clear();
#ifdef _NOMAD_DEBUG
	assert(m_Active.size());
#endif
	short size;
	fread(&size, sizeof(short), 1, fp);
	m_Active.reserve(size);
	for (i = 0; i < size; ++i) {
		m_Active.emplace_back();
		m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		Mob* mob = m_Active.back();
#ifdef _NOMAD_DEBUG
		assert(mob);
#endif
		fread(&mob->c_mob.mtype, sizeof(mob->c_mob.mtype), 1, fp);
		fread(&mob->health, sizeof(mob->health), 1, fp);
		fread(&mob->armor, sizeof(mob->armor), 1, fp);
		fread(&mob->mpos.y, sizeof(mob->mpos.y), 1, fp);
		fread(&mob->mpos.x, sizeof(mob->mpos.x), 1, fp);
		fread(&mob->mstate.id, sizeof(mob->mstate.id), 1, fp);
		fread(&mob->mticker, sizeof(mob->mticker), 1, fp);
	}
}

static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active)
{
#ifdef _NOMAD_DEBUG
	assert(b_Active.size() > 0);
	LOG("Archiving NPC Data");
#endif
	short size = (short)b_Active.size();
	fwrite(&size, sizeof(short), 1, fp);
	for (nomadshort_t i = 0; i < b_Active.size(); i++) {
		NPC* npc = b_Active[i];
#ifdef _NOMAD_DEBUG
		assert(npc);
#endif
		fwrite(&npc->c_npc.sprite, sizeof(npc->c_npc.sprite), 1, fp);
		fwrite(&npc->ndir, sizeof(npc->ndir), 1, fp);
		fwrite(&npc->pos.y, sizeof(npc->pos.y), 1, fp);
		fwrite(&npc->pos.x, sizeof(npc->pos.x), 1, fp);
		fwrite(&npc->health, sizeof(npc->health), 1, fp);
		fwrite(&npc->armor, sizeof(npc->armor), 1, fp);
		fwrite(&npc->nstate.id, sizeof(npc->nstate.id), 1, fp);
		fwrite(&npc->nticker, sizeof(npc->nticker), 1, fp);
	}
}
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active)
{
#ifdef _NOMAD_DEBUG
	assert(b_Active.size() > 0);
#endif
	nomaduint_t i;
	for (i = 0; i < b_Active.size(); i++) {
		Z_Free(b_Active[i]);
	}
	b_Active.clear();
#ifdef _NOMAD_DEBUG
	assert(b_Active.size() == 0);
#endif
	short size;
	fread(&size, sizeof(short), 1, fp);
	b_Active.reserve(size);
	for (i = 0; i < size; i++) {
		b_Active.emplace_back();
		b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		NPC* npc = b_Active.back();
#ifdef _NOMAD_DEBUG
		assert(npc);
#endif
		fread(&npc->c_npc.sprite, sizeof(npc->c_npc.sprite), 1, fp);
		fread(&npc->ndir, sizeof(npc->ndir), 1, fp);
		fread(&npc->pos.y, sizeof(npc->pos.y), 1, fp);
		fread(&npc->pos.x, sizeof(npc->pos.x), 1, fp);
		fread(&npc->health, sizeof(npc->health), 1, fp);
		fread(&npc->armor, sizeof(npc->armor), 1, fp);
		fread(&npc->nstate.id, sizeof(npc->nstate.id), 1, fp);
		fread(&npc->nticker, sizeof(npc->nticker), 1, fp);
	}
}