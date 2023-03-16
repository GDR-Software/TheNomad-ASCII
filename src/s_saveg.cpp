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
// DESCRIPTION: src/s_saveg.cpp
//  save-game I/O, uses nlohmann-json
//----------------------------------------------------------
#include "n_shared.h"
#include "g_obj.h"
#include "g_zone.h"
#include "s_scripted.h"
#include "g_mob.h"
#include "scf.h"
#include "p_npc.h"
#include "g_items.h"
#include "g_map.h"
#include "g_playr.h"
#include "s_world.h"
#include "g_game.h"

#undef assert
#undef byte
#include "nlohmann/json.hpp"

static constexpr auto svdir = "Files/gamedata/SVFILES/";

#define MAGIC_XOR 300

using json = nlohmann::json;

static void G_ArchivePlayr(const Playr* playr, json& data);
static void G_ArchiveWeapon(const Weapon* wpn, json& data);
static void G_ArchiveMobs(const linked_list<Mob*>& m_Active, json& data);

static void G_UnArchivePlayr(Playr* const playr, json& data);
static void G_UnArchiveWeapon(Weapon* const wpn, json& data);
static void G_UnArchiveMobs(linked_list<Mob*>& m_Active, json& data, nomaduint_t nummobs);

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
		{"bffname", bffname},
	};
	G_ArchivePlayr(playr, data);
	G_ArchiveMobs(m_Active, data);
	std::ofstream file(svfile, std::ios::out | std::ios::trunc);
	NOMAD_ASSERT(file.is_open(), "failed to open save file %s!", svfile);
	file << data;
	file.close();
}


bool Game::G_LoadGame(const char* svfile)
{
	struct stat svstat;
	if (stat(svfile, &svstat) == -1) {
		N_Error("failed to stat() save file!");
	}
	LOG_SAVEFILE();
	std::ifstream file(svfile, std::ios::in);
	json data = json::parse(file);
	file.close();
	const std::string &svbff = data["header"]["bffname"];
	if (!N_strcmp(svbff.c_str(), bffname)) {
		echo();
		noraw();
		nocbreak();
		char c;
		mvwaddstr(screen, getmaxy(screen) - 5, 0, "BFF file found in save file is different from current save file, this could lead to a crash");
		mvwaddstr(screen, getmaxy(screen) - 4, 0, "Continue? [y/n]");
		wrefresh(screen);
		c = getc(stdin);
		if (c != 'y') {
			return false;
		}
	}
	nomaduint_t nummobs = data["header"]["nummobs"];
	G_UnArchivePlayr(playr, data);
	G_UnArchiveMobs(m_Active, data, nummobs);
	return true;
}

static void G_ArchivePlayr(const Playr* playr, json& data)
{
	LOG_INFO("Archiving game->playr data");
	data["playr"] = {
//		{"name", playr->name},
		{"pdir", playr->pdir},
		{"health", playr->health},
		{"armor", playr->armor},
		{"pos.y", playr->pos.y},
		{"pos.x", playr->pos.x},
		{"wpn_slot_current", playr->wpn_slot_current},
		{"pstate", playr->pstate.id},
		{"pticker", playr->pticker},
		{"coin", playr->coin},
		{"lvl", playr->lvl},
		{"sprite", playr->sprite},
		{"sector_id", playr->sector_id},
		{"pdir", playr->pdir}
	};
}

static void G_UnArchivePlayr(Playr* const playr, json& data)
{
	LOG_INFO("Unarchiving game->playr data");
	playr->health = (nomadlong_t)data["playr"]["health"];
	playr->armor = (nomadlong_t)data["playr"]["armor"];
//	playr->name = data["playr"]["name"];
	playr->pos.y = data["playr"]["pos.y"];
	playr->pos.x = data["playr"]["pos.x"];
	playr->wpn_slot_current = data["playr"]["wpn_slot_current"];
	playr->pstate.id = data["playr"]["pstate"];
	playr->pticker = data["playr"]["pticker"];
	playr->coin = data["playr"]["coin"];
	playr->lvl = data["playr"]["lvl"];
	playr->sprite = (sprite_t)data["playr"]["sprite"];
	playr->sector_id = data["playr"]["sector_id"];
	playr->pdir = data["playr"]["pdir"];
}

static void G_ArchiveMobs(const linked_list<Mob*>& m_Active, json& data)
{
	LOG_INFO("Archiving m_Active mob data");
	linked_list<Mob*>::const_iterator it = m_Active.begin();
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = it->val;
		data[node_name] = {
			{"sprite", (int8_t)mob->sprite},
			{"health", mob->health},
			{"armor", mob->armor},
			{"mstate", mob->mstate.id},
			{"mticker", mob->mticker},
			{"mpos.y", mob->mpos.y},
			{"mpos.x", mob->mpos.x},
			{"mdir", mob->mdir}
		};
		it = it->next;
	}
}
static void G_UnArchiveMobs(linked_list<Mob*>& m_Active, json& data, nomaduint_t nummobs)
{
	LOG_INFO("unarchiving mob data from save file, nummobs: %iu", nummobs);
	if (nummobs != m_Active.size()) {
		LOG_INFO("nummobs != m_Active.size(), re-doing m_Active vector");
		for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
			Z_Free(m_Active.back());
			m_Active.pop_node();
		}
		for (nomaduint_t i = 0; i < nummobs; ++i) {
			m_Active.emplace_back();
			m_Active.back() = (Mob *)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
		}
	}
	linked_list<Mob*>::iterator it = m_Active.begin();
	for (nomaduint_t i = 0; i < m_Active.size(); ++i) {
		std::string node_name = "mob_"+std::to_string(i);
		Mob* const mob = it->val;
		mob->sprite = (int8_t)data[node_name]["sprite"];
		mob->health = data[node_name]["health"];
		mob->armor = data[node_name]["armor"];
		mob->mstate = stateinfo[data[node_name]["mstate"]];
		mob->mticker = data[node_name]["mticker"];
		mob->mpos.y = data[node_name]["mpos.y"];
		mob->mpos.x = data[node_name]["mpos.x"];
		mob->mdir = data[node_name]["mdir"];
		it = it->next;
	}
}