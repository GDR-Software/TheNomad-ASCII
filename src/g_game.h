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
//  src/g_game.h
//----------------------------------------------------------
#ifndef _G_GAME_
#define _G_GAME_

#pragma once

#include "g_sound.h"

#ifndef NOMAD_MAIN
#define NOMAD_MAIN
#endif

class Playr;
class Level;

// used for loading the bff, tracking the progress // nope, not anymore
constexpr auto NotFound = std::string::npos;

typedef enum : nomadenum_t
{
	GS_TITLE,
	GS_MENU,
	GS_LEVEL,
	GS_SETTINGS,
	GS_PAUSE,
	GS_CAMPAIGN,
} gamestate_t;

typedef enum : nomadenum_t
{
	HL_HUD,
	HL_VMATRIX,
	HL_VITALS,
	HL_WPNS,
	HL_PROMPT,

	NUMHUDLAYERS
} hudlayer_t;

constexpr auto MENU_TITLE     = 0x01;
constexpr auto MENU_MAIN      = 0x02;
constexpr auto MENU_SETTINGS  = 0x03;
constexpr auto MENU_LOADING   = 0x04;
constexpr auto HUD            = 0x05;
constexpr auto INVENTORY      = 0x06;
constexpr auto MAP            = 0x07;
constexpr auto MENU_SAVEFILES = 0x08;
constexpr auto MENU_PAUSE     = 0x09;

#if 0
template<auto fn>
struct ZoneDeleter {
	template<typename _Tp>
	constexpr void operator()(_Tp* arg) const { fn(arg); }
};
inline void free_playr(Playr *p) { Z_Free(p); }
inline void free_world(World *w) { Z_Free(w); }

namespace std {
template<typename _Tp, auto fn>
using zone_ptr = std::unique_ptr<_Tp, ZoneDeleter<fn>>;
};
#endif

enum
{
	PF_POS_OWNED = 0xa0, // the position on the map of the projectile is the same as the owner
	PF_KILL_TICKER = 0xa1, // free the projectile after x amount of base_ticker loops have occured
	PF_WITH_TICKER = 0xa2 // only move the projectile by its speed after x amount of ticks
};

typedef struct proj_s
{
	void *owner;
	entitytype_t et_owner;
	coord_t pos;
	nomadenum_t type;
	nomadenum_t speed = 1;
	nomadint_t flags;
	nomadlong_t ticker = 0;
	nomadlong_t base_ticker = 0;
	nomadint_t killticker = 0;

	inline proj_s& operator=(const proj_s& p) {
		owner = p.owner;
		et_owner = p.et_owner;
		pos = p.pos;
		type = p.type;
		speed = p.speed;
		return *this;
	}

	inline proj_s(void *_owner, entitytype_t _et_owner, coord_t _pos, nomadenum_t _type, nomadenum_t _speed)
		: owner(_owner), et_owner(_et_owner), pos(_pos), type(_type), speed(_speed)
	{
	}
	inline proj_s(void *_owner, entitytype_t _et_owner, const coord_t& _pos, nomadenum_t _type, nomadenum_t _speed,
		nomadint_t _flags)
		: owner(_owner), et_owner(_et_owner), pos(_pos), type(_type), speed(_speed), flags(_flags)
	{
	}
	inline proj_s(void *_owner, entitytype_t _et_owner, const coord_t& _pos, nomadenum_t _type, nomadenum_t _speed,
		nomadint_t _flags, nomadlong_t _ticker)
		: owner(_owner), et_owner(_et_owner), pos(_pos), type(_type), speed(_speed), flags(_flags), ticker(_ticker),
		base_ticker(_ticker)
	{
	}
	inline proj_s(void *_owner, entitytype_t _et_owner, const coord_t& _pos, nomadenum_t _type, nomadenum_t _speed,
		nomadint_t _flags, nomadlong_t _ticker, nomadint_t _killticker)
		: owner(_owner), et_owner(_et_owner), pos(_pos), type(_type), speed(_speed), flags(_flags), ticker(_ticker),
		base_ticker(_ticker), killticker(_killticker)

	{
	}
	inline proj_s() = default;
	inline proj_s(proj_s &&) = default;
	inline proj_s(const proj_s &) = delete;
	inline ~proj_s() = default;
} proj_t;

inline std::array<coord_t, 2> G_DrawRay(const coord_t& from, nomadenum_t dir, nomaduint_t range)
{
	std::array<coord_t, 2> ray;
	coord_t* start = &ray[0];
	coord_t& end = ray[1];
	switch (dir) {
	case D_NORTH:
		end = {from.y - range, from.x};
		break;
	case D_WEST:
		end = {from.y, from.x - range};
		break;
	case D_SOUTH:
		end = {from.y + range, from.x};
		break;
	case D_EAST:
		end = {from.y, from.x + range};
		break;
	default:
		LOG_WARN("invalid entity dir given to G_DrawRay: %hu, assigning default value of D_NORTH", dir);
		end = {from.y - range, from.x};
		break;
	};
	return ray;
}

void levelLoop();

class Game
{
public:
	nomadulong_t ticcount;
	gamestate_t gamestate;
	nomadenum_t gamescreen;
	char bffname[256];
	char scfname[256];
	char svfile[256];
	nomadenum_t difficulty;
	Playr* playr;
	World* world;
	linked_list<Mob*> m_Active{};
	linked_list<proj_t*> proj_list{};
	linked_list<item_t*> items{};
//	std::vector<NPC*> b_Active;
	char* biomenames[9];
public: // map stuff
	int8_t c_map[MAP_MAX_Y+160][MAP_MAX_X+160];
	std::shared_ptr<BFF> bff;
	std::shared_ptr<Map> map_ptr;
	std::shared_ptr<Level> lvlptr;
public:
	WINDOW* screen;
	WINDOW* hudwin[NUMHUDLAYERS];
public:
	Game();
	~Game();

	void DrawMainWinBorder(void);
	void DrawCompass(void);
	void DrawTitleScreen(void);
	void DrawPauseMenu(nomadshort_t s);
	void DrawMenuScreen(nomadshort_t s);
	
	void I_InitHUD(void);
	void G_ResetMap(void);
	void G_DisplayHUD(void);
	void G_CopyBufferToMap(void);
	void G_CopyMapBuffer(void);
	void P_Ticker(nomadint_t input);

	void M_GenMobs(void);

	bool G_LoadGame(const char* svfile);
	void G_SaveGame(const char* svfile);

//	npc_t N_MakeNPC(void);
//	void I_InitNPCs(void);

	inline coord_t E_TryMove(coord_t* epos, nomadenum_t* edir);
	nomadenum_t E_GetFacing(coord_t current, coord_t orig);
	void E_MoveImmediate(coord_t* epos, nomadenum_t edir); // the checks have already been performed by said entity
	coord_t E_GetDir(nomadenum_t dir);
	nomadbool_t E_Move(coord_t* epos, nomadenum_t* edir);
	void G_SaveState(void);
	void G_SaveRecentSlot(void);
};

void PhysicsAssigner(Game* const gptr);
void G_SpawnProjectile(proj_t& proj);
void G_SpawnProjectile(proj_t proj);
void G_RunProjectiles(void);
void TUIAssigner(Game* const gptr);
void LooperDelay(nomaduint_t numsecs);
void MobtAssigner(Game* const gptr);
void MobAssigner(Game* const gptr);
//void NPCAssigner(Game* const gptr);
//void NomadAssigner(Game* const gptr);
void CampaignAssigner(Game* const gptr);
void ctrl_c_handle(int signum);

nomaduint_t G_GetNumMobs(const Game* const game);
nomaduint_t G_GetNumBots(const Game* const game);

void G_GetShottyArea(area_t* a, nomadenum_t dir, coord_t pos, nomaduint_t range,
	nomadenum_t spread);
void TUI_Init(Game* const game);
nomadint_t G_CampaignSelect();
void G_LoadBFF(const char* bffname, Game* const game);
void I_NomadInit(int argc, char* argv[], Game* game);
//void W_Init(Game* const gptr);
//void W_Loop();
void mainLoop(int argc, char* argv[]);
void Hud_Printf(const char* from, const char* msg, ...);
void Hud_DisplayWpnSlot(nomadenum_t wpn_slot);
void MainAssigner(Game* const gptr);
void N_Error(const char* err, ...);
nomadbool_t E_CloseCollider(nomadenum_t dir, coord_t from, Game* const game);
std::vector<collider_t>& E_RangedCollider(nomadenum_t dir, coord_t from,
	nomadshort_t range, Game* const game);

void Snd_Init(Game* const gptr);

extern const char* logosplash;
extern const char* companysplash;
extern const char* about_str;
extern const char* credits_str;

#endif
