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

class Map;
class Spawner;
class BFF;

#if 0
class allocator_zone
{
public:
	allocator_zone(const char* = NULL) noexcept = default;
	allocator_zone(const allocator_zone&) noexcept = default;
	allocator_zone(const allocator_zone&, const char*) noexcept = default;

	allocator_zone& operator=(const allocator_zone &)
	{ return *this; }

	bool operator==(const allocator_zone&)
	{ return true; }
	bool operator!=(const allocator_zone&)
	{ return false; }
	
	void* allocate(size_t n, int /*flags*/ = 0)
	{ return Z_Malloc(n); }
	
	void* allocate(size_t n, size_t alignment, size_t alignmentOffset, int /*flags*/ = 0) const
	{ return Z_Malloc(size, TAG_STATIC, NULL); }
	void deallocate(void* p, size_t /*n*/) const
	{ Z_Free(p); }
	const char* get_name(void) const
	{ return "allocator_mzone"; }
	void set_name(const char*) const
	{ }
};
#endif

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
	std::vector<std::string> playrspawners;
	std::vector<std::shared_ptr<Spawner>>spawners;
	
	linked_list<Mob*> mspawners{};
	linked_list<item_t*> ispawners{};
	linked_list<Weapon*> wspawners{};
public:
	Level(){ memset(lvl_map, '.', sizeof(lvl_map)); }
	Level(const Level &) = delete;
	Level(Level &&) = default;
	~Level();
	
	void G_LoadSpawners(std::shared_ptr<BFF>& bff, char c_map[9][120][120]);
	void G_LoadLevel();
	void G_ShowIntro();
	void G_ShowExit();
};

class Sector
{
public:
	std::array<std::array<char, 120>, 120> sector_map;
	std::string sector_name;
	std::string sector_id;
	nomadint_t sector_num = 0;
	std::shared_ptr<Map> map_link;
public:
	Sector() = default;
	Sector(const Sector &) = delete;
	Sector(Sector &&) = default;
	~Sector() = default;
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

#define FILE_NAME(x) std::string("Files/gamedata/BFF/")+std::string(gptr->bffname)+std::string("/")+std::string(x)
#define JSON_DIAGNOSTICS
#include "nlohmann/json.hpp"
using json = nlohmann::json;

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
	std::array<nomadint_t, 3> game_version;
	std::vector<std::shared_ptr<Level>> levels;
	std::vector<std::shared_ptr<Map>> maps;
	std::vector<std::shared_ptr<Spawner>>spawners;
public:
	void Init(const std::string& _dirpath)
	{
		dirpath = _dirpath;
	}
	~BFF() = default;
	void BFF_LinkSpawners(json& data);
	void BFF_LoadLevels(json& data);
	void BFF_LoadMaps(json& data);
	void BFF_LinkMaps(json& data);
	void BFF_LoadSectors(json& data);
};

#endif
