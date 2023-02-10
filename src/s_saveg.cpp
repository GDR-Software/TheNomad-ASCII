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

#undef byte
#include "nlohmann/json.hpp"

static constexpr auto svdir = "Files/gamedata/SVFILES/";

enum : int8_t
{
	NGD_CHUNK_PLAYR,
	NGD_CHUNK_MOB,
	NGD_CHUNK_NPC,
	NGD_CHUNK_WORLD
};

#ifdef UNIX_NOMAD
typedef int32_t num_t;
#elif defined(WIN32_NOMAD)
typedef INT32 num_t;
#endif

static constexpr num_t HEADER = 0x5f3759df;

typedef struct _ngd_header
{
	num_t header = HEADER;
	num_t nummobs = 0;
	num_t numnpcs = 0;
	num_t numchunks = 0;
	char svname[256];
} ngd_header_t;

typedef struct _ngd_chunk
{
	num_t chunktype;
	char buffer[BUFFER_SIZE];
} ngd_chunk_t;

// nomadascii game data (.ngd) save-file
typedef struct _ngd_file
{
	ngd_header_t header;
	ngd_chunk_t* chunks;
} ngd_file_t;

#ifdef UNIX_NOMAD
static uint32_t countfiles(const char *path) {
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    char *npath;
    if (!path) return 0;
    if( (dir_ptr = opendir(path)) == NULL ) return 0;

    int count=0;
    while( (direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name,".")==0 ||
            strcmp(direntp->d_name,"..")==0) continue;
        switch (direntp->d_type) {
        case DT_REG: {
            if (!strstr(direntp->d_name, ".dat"))
				++count;
            break; }
        case DT_DIR:            
            npath = (char *)malloc(strlen(path) + strlen(direntp->d_name)+2);
            sprintf(npath,"%s/%s",path, direntp->d_name);
            count += countfiles(npath);
            free(npath);
            break;
        };
    }
    closedir(dir_ptr);
    return count;
}
#endif

#define MAGIC_XOR 300

using json = nlohmann::json;

static void G_ArchivePlayr(const Playr* playr, json& data);
static void G_ArchiveWorld(const World* world, json& data);
static void G_ArchiveWeapon(const Weapon* wpn, json& data);
static void G_ArchiveItem(const Item* item, json& data);
static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, json& data);
static void G_ArchiveBots(const std::vector<NPC*>& b_Active, json& data);

static void G_UnArchivePlayr(Playr* const playr, json& data);
static void G_UnArchiveWeapon(Weapon* const wpn, json& data);
static void G_UnArchiveItem(Item* const item, json& data);
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active, json& data, nomaduint_t nummobs);
static void G_UnArchiveBots(std::vector<NPC*>& b_Active, json& data, nomaduint_t numbots);

void Game::G_SaveGame(void)
{
	constexpr const char* svname = "nomadascii_sv";
	const char* svfile = "nomadsv.ngd";
	std::ofstream file(svfile, std::ios::out | std::ios::trunc);
	json data;
	data["header"] = {
		{"version", NOMAD_VERSION},
		{"version.major", NOMAD_VERSION_MAJOR},
		{"version.update", NOMAD_VERSION_MINOR},
		{"version.patch", NOMAD_VERSION_PATCH},
		{"svname", svname},
		{"nummobs", m_Active.size()},
		{"numbots", b_Active.size()},
		{"slot", 0},
	};
	G_ArchivePlayr(playr, data);
	G_ArchiveMobs(m_Active, data);
	file << data;
	file.close();
}


bool Game::G_LoadGame(const char* svfile)
{
	std::ifstream file(svfile, std::ios::in);
	json data = json::parse(file);
	file.close();
	nomaduint_t nummobs = data["header"]["nummobs"];
	G_UnArchivePlayr(playr, data);
	G_UnArchiveMobs(m_Active, data, nummobs);
	return true;
}

static void G_ArchivePlayr(const Playr* playr, json& data)
{
	auto health = playr->health.load();
	auto armor = playr->armor.load();
	data["playr"] = {
		{"name", playr->name},
		{"pdir", playr->pdir},
		{"health", health},
		{"armor", armor},
		{"pos.y", playr->pos.y},
		{"pos.x", playr->pos.x},
		{"wpn_slot_current", playr->wpn_slot_current},
		{"pstate", playr->pstate.id},
		{"pticker", playr->pticker},
		{"coin", playr->coin},
		{"lvl", playr->lvl},
		{"sprite", playr->sprite},
		{"sector_id", playr->sector_id},
	};
}

static void G_UnArchivePlayr(Playr* const playr, json& data)
{
	playr->health = data["playr"]["health"];
	playr->armor = data["playr"]["armor"];
	playr->name = data["playr"]["name"];
	playr->pos.y = data["playr"]["pos.y"];
	playr->pos.x = data["playr"]["pos.x"];
	playr->wpn_slot_current = data["playr"]["wpn_slot_current"];
	playr->pstate.id = data["playr"]["pstate"];
	playr->pticker = data["playr"]["pticker"];
	playr->coin = data["playr"]["coin"];
	playr->lvl = data["playr"]["lvl"];
	playr->sprite = data["playr"]["sprite"];
	playr->sector_id = data["playr"]["sector_id"];
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, json& data)
{
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = m_Active[i];
		auto health = mob->health.load();
		auto armor = mob->armor.load();
		data[node_name] = {
			{"health", health},
			{"armor", armor},
			{"mpos.y", mob->mpos.y},
			{"mpos.x", mob->mpos.x},
			{"mdir", mob->mdir},
		};
	}
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active, json& data, nomaduint_t nummobs)
{
	nomaduint_t o = m_Active.size();
	while (m_Active.size() > nummobs) {
		Z_Free(m_Active[o]);
		m_Active.pop_back();
		--o;
	}
	while (m_Active.size() < nummobs) {
		m_Active.emplace_back();
		m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
	}
	for (nomaduint_t i = 0; i < nummobs; ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = m_Active[i];
		mob->health = data[node_name]["health"];
		mob->armor = data[node_name]["armor"];
		mob->mpos.y = data[node_name]["mpos.y"];
		mob->mpos.x = data[node_name]["mpos.x"];
		mob->mdir = data[node_name]["mdir"];
	}
}

static void G_ArchiveBots(const std::vector<NPC*>& b_Active, json& data)
{
	for (nomaduint_t i = 0; i < b_Active.size(); ++i) {
		std::string node_name = "bot_"+std::to_string(i);
		NPC* const npc = b_Active[i];
		auto health = npc->health.load();
		auto armor = npc->armor.load();
		data[node_name] = {
			{"health", health},
			{"armor", armor},
			{"sprite", npc->c_npc.sprite},
			{"type", npc->c_npc.type},
			{"pos.y", npc->pos.y},
			{"pos.x", npc->pos.x},
			{"ndir", npc->ndir},
		};
	}
}
static void G_UnArchiveBots(std::vector<NPC*>& b_Active, json& data, nomaduint_t numbots)
{
	nomaduint_t o = b_Active.size();
	while (b_Active.size() > numbots) {
		Z_Free(b_Acitve[o]);
		b_Active.pop_back();
		--o;
	}
	while (b_Active.size() < numbots) {
		b_Active.emplace_back();
		b_Active.back() = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
	}
	for (nomaduint_t i = 0; i < numbots; ++i) {
		std::string node_name = "bot_"+std::to_string(i);
		NPC* const npc = b_Active[i];
		npc->health = data[node_name]["health"];
		npc->armor = data[node_name]["armor"];
		npc->pos.y = data[node_name]["pos.y"];
		npc->pos.x = data[node_name]["pos.x"];
		npc->ndir = data[node_name]["ndir"];
	}
}
