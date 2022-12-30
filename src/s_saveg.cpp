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

static FILE* fp;
static constexpr uint64_t HEADER = 0x5f3759df;
static constexpr auto svfile = "nomadsv.ngd";

static void G_ArchivePlayr(const Playr* playr);
static void G_UnArchivePlayr(Playr* const playr);
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
	G_ArchiveMobs(m_Active);
	G_ArchiveNPCs(b_Active);
	fclose(fp);
}

bool Game::G_LoadGame(void)
{
	struct stat fdata;
	if (stat(svfile, &fdata) == -1) {
		return false;
	}
	fp = fopen(svfile, "rb");
	if (!fp) {
		return false;
	}
	uint64_t header;
	fread(&header, 1, sizeof(uint64_t), fp);
	
	// not a valid save file
	if (!(header & HEADER)) {
		return false;
	}
	G_UnArchivePlayr(playr);
	G_UnArchiveMobs(m_Active);
	G_UnArchiveNPCs(b_Active);
	fclose(fp);
	Z_CheckHeap();
	return true;
}

static void G_ArchivePlayr(const Playr* playr)
{
	short size = (short)playr->name.size();
	fwrite(&size, 1, sizeof(short), fp);
	fwrite((char*)&playr->name, playr->name.size(), sizeof(char), fp);
	fwrite(&playr->health, 1, sizeof(playr->health), fp);
	fwrite(&playr->armor, 1, sizeof(playr->armor), fp);
	fwrite(&playr->pos.y, 1, sizeof(playr->pos.y), fp);
	fwrite(&playr->pos.x, 1, sizeof(playr->pos.x), fp);
	fwrite(&playr->coin, 1, sizeof(playr->coin), fp);
	fwrite(&playr->lvl, 1, sizeof(playr->lvl), fp);
	fwrite(&playr->vmatrix, sizeof(playr->vmatrix), sizeof(char), fp);
}
static void G_UnArchivePlayr(Playr* const playr)
{
	short size;
	fread(&size, 1, sizeof(short), fp);
	fread((char*)&playr->name, size, sizeof(char), fp);
	fread(&playr->health, 1, sizeof(playr->health), fp);
	fread(&playr->armor, 1, sizeof(playr->armor), fp);
	fread(&playr->pos.y, 1, sizeof(playr->pos.y), fp);
	fread(&playr->pos.x, 1, sizeof(playr->pos.x), fp);
	fread(&playr->coin, 1, sizeof(playr->coin), fp);
	fread(&playr->lvl, 1, sizeof(playr->lvl), fp);
	fread(&playr->vmatrix, sizeof(playr->vmatrix), sizeof(char), fp);
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active)
{
	short size = (short)m_Active.size();
	fwrite(&size, 1, sizeof(short), fp);
	for (nomaduint_t i = 0; i < m_Active.size(); i++) {
		Mob* mob = m_Active[i];
		fwrite(&mob->c_mob.mtype, 1, sizeof(mob->c_mob.mtype), fp);
		fwrite(&mob->health, 1, sizeof(mob->health), fp);
		fwrite(&mob->armor, 1, sizeof(mob->armor), fp);
		fwrite(&mob->mpos.y, 1, sizeof(mob->mpos.y), fp);
		fwrite(&mob->mpos.x, 1, sizeof(mob->mpos.x), fp);
		fwrite(&mob->mstate.id, 1, sizeof(mob->mstate.id), fp);
		fwrite(&mob->mticker, 1, sizeof(mob->mticker), fp);
	}
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active)
{
	nomaduint_t i;
	for (i = 0; i < m_Active.size(); i++) {
		Z_Free(m_Active[i]);
	}
	m_Active.clear();
	short size;
	fread(&size, 1, sizeof(short), fp);
	m_Active.reserve(size);
	for (i = 0; i < size; i++) {
		m_Active.emplace_back();
		m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		Mob* mob = m_Active.back();
		fread(&mob->c_mob.mtype, 1, sizeof(mob->c_mob.mtype), fp);
		fread(&mob->health, 1, sizeof(mob->health), fp);
		fread(&mob->armor, 1, sizeof(mob->armor), fp);
		fread(&mob->mpos.y, 1, sizeof(mob->mpos.y), fp);
		fread(&mob->mpos.x, 1, sizeof(mob->mpos.x), fp);
		fread(&mob->mstate.id, 1, sizeof(mob->mstate.id), fp);
		fread(&mob->mticker, 1, sizeof(mob->mticker), fp);
	}
}

static void G_ArchiveNPCs(const std::vector<NPC*>& b_Active)
{
	short size = (short)b_Active.size();
	fwrite(&size, 1, sizeof(short), fp);
	for (nomaduint_t i = 0; i < b_Active.size(); i++) {
		NPC* npc = b_Active[i];
		fwrite(&npc->c_npc.sprite, 1, sizeof(npc->c_npc.sprite), fp);
		fwrite(&npc->ndir, 1, sizeof(npc->ndir), fp);
		fwrite(&npc->pos.y, 1, sizeof(npc->pos.y), fp);
		fwrite(&npc->pos.x, 1, sizeof(npc->pos.x), fp);
		fwrite(&npc->health, 1, sizeof(npc->health), fp);
		fwrite(&npc->armor, 1, sizeof(npc->armor), fp);
		fwrite(&npc->nstate.id, 1, sizeof(npc->nstate.id), fp);
		fwrite(&npc->nticker, 1, sizeof(npc->nticker), fp);
	}
}
static void G_UnArchiveNPCs(std::vector<NPC*>& b_Active)
{
	nomaduint_t i;
	for (i = 0; i < b_Active.size(); i++) {
		Z_Free(b_Active[i]);
	}
	b_Active.clear();
	short size;
	fread(&size, 1, sizeof(short), fp);
	b_Active.reserve(size);
	for (i = 0; i < size; i++) {
		b_Active.emplace_back();
		b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		NPC* npc = b_Active.back();
		fread(&npc->c_npc.sprite, 1, sizeof(npc->c_npc.sprite), fp);
		fread(&npc->ndir, 1, sizeof(npc->ndir), fp);
		fread(&npc->pos.y, 1, sizeof(npc->pos.y), fp);
		fread(&npc->pos.x, 1, sizeof(npc->pos.x), fp);
		fread(&npc->health, 1, sizeof(npc->health), fp);
		fread(&npc->armor, 1, sizeof(npc->armor), fp);
		fread(&npc->nstate.id, 1, sizeof(npc->nstate.id), fp);
		fread(&npc->nticker, 1, sizeof(npc->nticker), fp);
	}
}