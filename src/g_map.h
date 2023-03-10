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
// DESCRIPTION: src/g_map.h
//----------------------------------------------------------
#ifndef _G_MAP_
#define _G_MAP_

#pragma once

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

class Map;
class Spawner;
class BFF;

class Level
{
public:
	std::vector<std::string> intro_screen;
	std::vector<std::string> end_screen;
	std::string lvl_name;
	std::string lvl_id;
	std::shared_ptr<Map> map_link;
	char lvl_map[120][120];
	std::vector<std::shared_ptr<Spawner>> pspawners;
	
	std::vector<std::string> mobspawners;
	std::vector<std::string> itemspawners;
	std::vector<std::string> wpnspawners;
	std::vector<std::shared_ptr<Spawner>> spawners;
	
	std::vector<Mob*> mspawners;
	std::vector<item_t*> ispawners;
	std::vector<Weapon*> wspawners;
public:
	Level(){ memset(lvl_map, '.', sizeof(lvl_map)); }
	Level(const Level &) = delete;
	Level(Level &&) = default;
	~Level();
	
	void G_LoadSpawners(std::shared_ptr<BFF>& bff, char c_map[9][120][120]);
	void G_LoadLevel()
	{
		char mapbuffer[520][520];
		memset(mapbuffer, '#', sizeof(mapbuffer));
		for (nomadshort_t y = 0; y < SECTOR_MAX_Y; ++y) {
			for (nomadshort_t x = 0; x < SECTOR_MAX_X; ++x) {
				
			}
		}
		FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
		fclose(fp);
	}
	void G_ShowIntro();
	void G_ShowExit();
};

class Sector
{
public:
	eastl::array<eastl::array<char, 120>, 120> sector_map;
	std::string sector_name;
	std::string sector_id;
	nomadint_t sector_num = 0;
	std::shared_ptr<Map> map_link;
public:
	Sector(){}
	Sector(const Sector &) = delete;
	Sector(Sector &&) = default;
	~Sector(){}
};

class Map
{
public:
	nomadint_t numlvls = 0;
	std::vector<std::string> intro_screen;
	std::vector<std::string> exit_screen;
	std::string map_name;
	std::string map_id;
	char map_buffer[9][120][120];
	std::vector<std::shared_ptr<Level>> levels;
	std::vector<std::shared_ptr<Sector>> sectors;
public:
	Map() = default;
	Map(const Map &) = default;
	Map(Map &&) = default;
	~Map() = default;
};

#define FILE_NAME(x) std::string("Files/gamedata/BFF/nomadmain.bff/")+std::string(x)
#define JSON_DIAGNOSTICS
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include <sndfile.h>
#include <AL/al.h>
#include <AL/alc.h>

class Spawner
{
public:
	void *et_ptr;
	entitytype_t et_type;
	std::string et_name;
	sprite_t marker;
	sprite_t replacement;
	coord_t pos;
public:
	Spawner() = default;
	Spawner(const Spawner &) = default;
	Spawner(Spawner &&) = default;
	~Spawner() = default;
};


class BFF
{
public:
	std::string dirpath;
	std::string bffname;
	std::string bffversion;
	eastl::array<nomadint_t, 3> game_version;
	std::vector<std::shared_ptr<Level>> levels;
	std::vector<std::shared_ptr<Map>> maps;
	std::vector<std::shared_ptr<Spawner>> spawners;
public:
	void Init(const std::string& _dirpath)
	{
		dirpath = _dirpath;
	}
	~BFF() = default;
	void BFF_LinkSpawners(json& data)
	{
		nomadint_t numspawners = data["data"]["spawners"]["numspawners"];
		spawners.reserve(numspawners);
		for (nomadint_t i = 0; i < numspawners; ++i) {
			spawners.emplace_back(new Spawner());
			std::shared_ptr<Spawner>& spawn = spawners.back();
			std::string node_name = "spawner_"+std::to_string(i);
			std::string et_type = data["data"]["spawners"][node_name]["entity"];
			if (et_type == "ET_MOB") {
				spawn->et_type = ET_MOB;
			} else if (et_type == "ET_PLAYR") {
				spawn->et_type = ET_PLAYR;
			}
			spawn->et_name = data["data"]["spawners"][node_name]["name"];
			std::string marker = data["data"]["spawners"][node_name]["marker"];
			if (marker.size() > 1) {
				N_Error("BFF_LinkSpawners: cannot have a spawn marker greater than 1 character");
			}
			spawn->marker = marker[0];
			std::string replace = data["data"]["spawners"][node_name]["replacement"];
			if (replace.size() > 1) {
				N_Error("BFF_LinkSpawners: cannot have a spawn replacement greater than 1 character");
			}
			spawn->replacement = replace[0];
		}
	}
	void BFF_LoadLevels(json& data)
	{
		nomadint_t numlvls = data["levels"]["numlevels"];
		levels.reserve(numlvls);
		for (nomadint_t i = 0; i < numlvls; ++i) {
			levels.emplace_back(new Level());
			std::shared_ptr<Level>& lvl = levels.back();
			std::string node_name = "lvl_"+std::to_string(i);
			const std::string mapfile = data["levels"][node_name]["mapfile"];
			lvl->lvl_id = data["levels"][node_name]["id"];
			lvl->lvl_name = data["levels"][node_name]["name"];
			{
				for (auto& s : data["levels"][node_name]["mobspawners"]) {
					lvl->mobspawners.push_back(s);
				}
			}
			{
				for (auto& s : data["levels"][node_name]["itemspawners"]) {
					lvl->itemspawners.push_back(s);
				}
			}
			{
				for (auto& s : data["levels"][node_name]["wpnspawners"]) {
					lvl->wpnspawners.push_back(s);
				}
			}
			{
				const std::string filename = data["levels"][node_name]["introfile"];
				std::ifstream file(FILE_NAME(filename), std::ios::in);
				NOMAD_ASSERT(file.is_open(), "failed to open level intro file %s!", filename.c_str());
				std::string line;
				while (std::getline(file, line)) { lvl->intro_screen.push_back(line); }
				file.close();
			}
			{
				const std::string filename = data["levels"][node_name]["endfile"];
				std::ifstream file(FILE_NAME(filename), std::ios::in);
				NOMAD_ASSERT(file.is_open(), "failed to open level end file %s!", filename.c_str());
				std::string line;
				while (std::getline(file, line)) { lvl->end_screen.push_back(line); }
				file.close();
			}
			std::ifstream file(FILE_NAME(mapfile), std::ios::in);
			NOMAD_ASSERT(file.is_open(), "failed to open level map file %s!", mapfile.c_str());
			std::string line;
			std::vector<std::string> buffer;
			nomadshort_t y{}, x{};
			y=x=0;
			while (std::getline(file, line)) {
				if (y > 119) {
					break;
				}
				for (x = 0; x < 120; ++x) {
					lvl->lvl_map[y][x] = line[x];
				}
				++y;
			}
			file.close();
		}
	}
	void BFF_LoadMaps(json& data)
	{
		nomadint_t nummaps = data["maps"]["num_maps"];
		maps.reserve(1);
		for (nomadint_t i = 0; i < nummaps; ++i) {
			maps.emplace_back(new Map());
			std::shared_ptr<Map>& ptr = maps.back();
			std::string node_name = "map_"+std::to_string(i);
			ptr->map_name = data["maps"][node_name]["name"];
			ptr->map_id = data["maps"][node_name]["id"];
//  			{
//				const std::string filename = data["maps"][node_name]["introfile"];
//				std::ifstream file(FILE_NAME(filename), std::ios::in);
//				//NOMAD_ASSERT(file.is_open(), "failed to open map intro screen %s!", filename.c_str());
//				std::string line;
//				while (eastl::getline(file, line)) { ptr->intro_screen.push_back(line); }
//				file.close();
//			}
//			{
//				const std::string filename = data["maps"][node_name]["endfile"];
//				std::ifstream file(FILE_NAME(filename), std::ios::in);
//				//NOMAD_ASSERT(file.is_open(), "failed to open map end screen %s!", filename.c_str());
//				std::string line;
//				while (eastl::getline(file, line)) { ptr->exit_screen.push_back(line); }
//				file.close();
//			}
		}
	}
	void BFF_LinkMaps(json& data)
	{
		nomadint_t i{};
		for (i = 0; i < maps.size(); ++i) {
			std::shared_ptr<Map>& ptr = maps[i];
			const std::string node_name = "map_"+std::to_string(i);
			std::vector<std::string> lvl_names;
			for (auto& l : data["maps"][node_name]["levels"]) {
				lvl_names.push_back(l);
			}
			for (const auto& l : lvl_names) {
				std::shared_ptr<Level>* lvl = nullptr;
				for (auto& a : levels) {
					if (a->lvl_id == l) {
						lvl = &a;
					}
				}
				NOMAD_ASSERT(lvl, "could not find level with name %s", l.c_str());
				ptr->levels.emplace_back(*lvl);
			}
		}
		for (i = 0; i < levels.size(); ++i) {
			const std::string node_name = "lvl_"+std::to_string(i);
			const std::string mapname = data["levels"][node_name]["map"];
			std::shared_ptr<Map>* ptr = nullptr;
			std::shared_ptr<Level>& lvl = levels[i];
			for (auto& m : maps) {
				if (m->map_id == mapname) {
					ptr = &m;
				}
			}
			NOMAD_ASSERT(ptr, "could not find map with name %s for level linkage", mapname.c_str());
			lvl->map_link = *ptr;
		}
	}
	void BFF_LoadSectors(json& data)
	{
		for (nomadint_t i = 0; i < maps.size(); ++i) {
			for (nomadint_t s = 0; s < NUM_SECTORS; ++s) {
				std::string node_name = "mapsector_"+std::to_string(s);
				std::string map_node = "map_"+std::to_string(i);
				maps[i]->sectors.emplace_back(new Sector());
				std::shared_ptr<Sector>& ptr = maps[i]->sectors.back();
				ptr->map_link = maps[i];
				ptr->sector_name = data["maps"][map_node][node_name]["name"];
				ptr->sector_id = data["maps"][map_node][node_name]["id"];
				const std::string mapfile = data["maps"][map_node][node_name]["mapfile"];
				std::ifstream file(FILE_NAME(mapfile), std::ios::in);
				NOMAD_ASSERT(file.is_open(), "failed to open map sector file %s!", mapfile.c_str());
				std::string line;
				nomadshort_t y{}, x{};
				y = x = 0;
				while (std::getline(file, line)) {
					if (y > 119) {
						break;
					}
					for (x = 0; x < 120; ++x) {
						ptr->sector_map[y][x] = line[x];
					}
					++y;
				}
				file.close();
				for (y = 0; y < 120; ++y) {
					for (x = 0; x < 120; ++x) {
						maps[i]->map_buffer[s][y][x] = ptr->sector_map[y][x];
					}
				}
			}
		}
	}
};

#endif
