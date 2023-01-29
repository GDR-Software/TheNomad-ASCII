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

#include "n_shared.h"
#include "g_zone.h"
#include "p_npc.h"
#include "g_playr.h"
#include "g_mob.h"
#include "g_map.h"
#include "s_world.h"

#ifndef _NOMAD_MAIN_
#define _NOMAD_MAIN_
#endif

// used for loading the bff, tracking the progress // nope, not anymore

#define ctrl(x) (x & 0x1F)
constexpr auto NotFound = std::string::npos;

typedef enum : nomadenum_t
{
	GS_TITLE,
	GS_MENU,
	GS_LEVEL,
	GS_PAUSE
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

class Game
{
public:
	std::atomic<nomadulong_t> ticcount;
	gamestate_t gamestate;
	nomadenum_t gamescreen;
	char bffname[80];
	nomadenum_t difficulty;
	Playr* playr;
	World* world;
	std::vector<Mob*> m_Active;
	std::vector<NPC*> b_Active;
public: // map stuff
	sndlvl_t sndmap[MAP_MAX_Y+160][MAP_MAX_X+160];
	smelllvl_t smellmap[MAP_MAX_Y+160][MAP_MAX_X+160];
	char c_map[MAP_MAX_Y+160][MAP_MAX_X+160];
public:
	WINDOW* screen;
	WINDOW* hudwin[NUMHUDLAYERS];
public: // *** multithreading! ***
	pthread_mutex_t mob_mutex;
	pthread_mutex_t npc_mutex;
	pthread_mutex_t playr_mutex;
	pthread_t mthread;
	pthread_t nthread;
	pthread_t wthread;
	pthread_t pthread;
#ifdef __unix__
	pid_t mobid;
	pid_t npcid;
#endif
	nomadlong_t hudtics;
	std::atomic<nomaduint_t> pdmg; // amount of damage done to the player in a single tic
public:
	Game();
	~Game();

	void DrawSpriteToMainWin(coord_t pos, sprite_t sprite);
	void PrintMainWin(void);
	void MainWinPrintf(const char* msg);
	void ClearMainWin(void);
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
	
	void G_SaveGame(void);
	bool G_LoadGame(void);

	npc_t N_MakeNPC(void);
	void I_InitNPCs(void);

	inline coord_t E_TryMove(coord_t* epos, nomadenum_t* edir);
	nomadenum_t E_GetFacing(coord_t current, coord_t orig);
	void E_MoveImmediate(coord_t* epos, nomadenum_t edir); // the checks have already been performed by said entity
	coord_t E_GetDir(nomadenum_t dir);
	nomadbool_t E_Move(coord_t* epos, nomadenum_t* edir);
};

void I_NomadInit(int argc, char* argv[], Game* const game);
void W_Init(Game* const gptr);
void* W_Loop(void *arg);
void mainLoop(int argc, char* argv[]);
void Hud_Printf(const char* from, const char* msg, ...);
void Hud_DisplayWpnSlot(nomadenum_t wpn_slot);
void MainAssigner(Game* const gptr);
void N_Error(const char* err, ...);
nomadbool_t E_CloseCollider(nomadenum_t dir, coord_t from, Game* const game);
std::vector<collider_t>& E_RangedCollider(nomadenum_t dir, coord_t from,
	nomadshort_t range, Game* const game);

extern const char* logosplash;
extern const char* companysplash;

#endif
