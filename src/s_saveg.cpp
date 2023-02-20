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

#define MAGIC_XOR 300

using json = nlohmann::json;

static void G_ArchivePlayr(const Playr* playr, json& data);
static void G_ArchiveWorld(const World* world, json& data);
static void G_ArchiveWeapon(const Weapon* wpn, json& data);
static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, json& data);
static void G_ArchiveBots(const std::vector<NPC*>& b_Active, json& data);

static void G_UnArchivePlayr(Playr* const playr, json& data);
static void G_UnArchiveWeapon(Weapon* const wpn, json& data);
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active, json& data, nomaduint_t nummobs);
static void G_UnArchiveBots(std::vector<NPC*>& b_Active, json& data, nomaduint_t numbots);

void Game::G_SaveGame(const char* svfile)
{
	struct stat svstat;
	stat(svfile, &svstat);
	LOG_SAVEFILE();
	json data;
	data["header"] = {
		{"version", NOMAD_VERSION},
		{"version.major", NOMAD_VERSION_MAJOR},
		{"version.update", NOMAD_VERSION_MINOR},
		{"version.patch", NOMAD_VERSION_PATCH},
		{"nummobs", m_Active.size()},
		{"numbots", b_Active.size()},
	};
	G_ArchivePlayr(playr, data);
	G_ArchiveMobs(m_Active, data);
	G_ArchiveBots(b_Active, data);
	std::ofstream file(svfile, std::ios::out | std::ios::trunc);
	NOMAD_ASSERT(!file.fail(), "Failed to open save file %s!", svfile);
	file << data;
	file.close();
}


bool Game::G_LoadGame(const char* svfile)
{
	struct stat svstat;
	if (stat(svfile, &svstat) == -1) {
		N_Error("Failed to stat() save file!");
	}
	LOG_SAVEFILE();
	std::ifstream file(svfile, std::ios::in);
	json data = json::parse(file);
	file.close();
	nomaduint_t nummobs = data["header"]["nummobs"];
	nomaduint_t numbots = data["header"]["numbots"];
	G_UnArchivePlayr(playr, data);
	G_UnArchiveMobs(m_Active, data, nummobs);
	G_UnArchiveBots(b_Active, data, numbots);
	return true;
}

static void G_ArchivePlayr(const Playr* playr, json& data)
{
	LOG_INFO("Archiving game->playr data");
	nomadlong_t health = playr->health.load();
	nomadlong_t armor = playr->armor.load();
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
		{"pdir", playr->pdir},
	};
}

static void G_UnArchivePlayr(Playr* const playr, json& data)
{
	LOG_INFO("Unarchiving game->playr data");
	playr->health = (nomadlong_t)data["playr"]["health"];
	playr->armor = (nomadlong_t)data["playr"]["armor"];
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
	playr->pdir = data["playr"]["pdir"];
}

static void G_ArchiveMobs(const std::vector<Mob*>& m_Active, json& data)
{
	LOG_INFO("Archiving m_Active mob data");
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = m_Active[i];
		nomadlong_t health = mob->health.load();
		nomadlong_t armor = mob->armor.load();
		data[node_name] = {
			{"sprite", (int8_t)mob->sprite},
			{"health", health},
			{"armor", armor},
			{"mstate", mob->mstate.id},
			{"mticker", mob->mticker},
			{"mpos.y", mob->mpos.y},
			{"mpos.x", mob->mpos.x},
			{"mdir", mob->mdir},
		};
	}
}
static void G_UnArchiveMobs(std::vector<Mob*>& m_Active, json& data, nomaduint_t nummobs)
{
	LOG_INFO("unarchiving mob data from save file, nummobs: %iu", nummobs);
	if (nummobs != m_Active.size()) {
		LOG_INFO("nummobs != m_Active.size(), re-doing m_Active vector");
		for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
			Z_Free(m_Active.back());
			m_Active.pop_back();
		}
		m_Active.reserve(nummobs);
		for (nomaduint_t i = 0; i < nummobs; ++i) {
			m_Active.emplace_back();
			m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		}
	}
	for (nomaduint_t i = 0; i < nummobs; ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = m_Active[i];
		mob->sprite = (int8_t)data[node_name]["sprite"];
		mob->health = data[node_name]["health"];
		mob->armor = data[node_name]["armor"];
		mob->mstate = stateinfo[data[node_name]["mstate"]];
		mob->mticker = data[node_name]["mticker"];
		mob->mpos.y = data[node_name]["mpos.y"];
		mob->mpos.x = data[node_name]["mpos.x"];
		mob->mdir = data[node_name]["mdir"];
	}
}

static void G_ArchiveBots(const std::vector<NPC*>& b_Active, json& data)
{
	LOG_INFO("Archiving b_Active bot data");
	for (nomaduint_t i = 0; i < b_Active.size(); ++i) {
		std::string node_name = "bot_"+std::to_string(i);
		NPC* const npc = b_Active[i];
		nomadlong_t health = npc->health.load();
		nomadlong_t armor = npc->armor.load();
		data[node_name] = {
			{"health", health},
			{"armor", armor},
			{"sprite", (int8_t)npc->sprite},
			{"btype", npc->c_npc.btype},
			{"pos.y", npc->pos.y},
			{"pos.x", npc->pos.x},
			{"ndir", npc->ndir},
		};
	}
}
static void G_UnArchiveBots(std::vector<NPC*>& b_Active, json& data, nomaduint_t numbots)
{
	LOG_INFO("unarchiving bot data from save file, numbots %iu", numbots);
	if (numbots != b_Active.size()) {
		LOG_INFO("numbots != b_Active.size(), re-doing b_Active vector");
		for (nomaduint_t i = 0; i < b_Active.size(); ++i) {
			Z_Free(b_Active.back());
			b_Active.pop_back();
		}
		b_Active.reserve(numbots);
		for (nomaduint_t i = 0; i < numbots; ++i) {
			b_Active.emplace_back();
			b_Active.back() = (NPC *)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
		}
	}
	for (nomaduint_t i = 0; i < numbots; ++i) {
		std::string node_name = "bot_"+std::to_string(i);
		NPC* const npc = b_Active[i];
		npc->sprite = (int8_t)data[node_name]["sprite"];
		npc->health = data[node_name]["health"];
		npc->armor = data[node_name]["armor"];
		npc->pos.y = data[node_name]["pos.y"];
		npc->pos.x = data[node_name]["pos.x"];
		npc->ndir = data[node_name]["ndir"];
		npc->c_npc.btype = data[node_name]["btype"];
	}
}
