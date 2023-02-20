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

#include "n_shared.h"
#include "g_zone.h"
#include "s_scripted.h"
#include "p_npc.h"
#include "g_playr.h"
#include "g_mob.h"
#include "g_map.h"
#include "s_world.h"

#ifndef _NOMAD_MAIN_
#define _NOMAD_MAIN_
#endif

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

class Game
{
public:
	std::atomic<nomadulong_t> ticcount;
	gamestate_t gamestate;
	nomadenum_t gamescreen;
	char bffname[256];
	char scfname[256];
	char svfile[256];
	nomadenum_t difficulty;
	Playr* playr;
	World* world;
	std::vector<Mob*> m_Active;
	std::vector<NPC*> b_Active;
	char* biomenames[9];
public: // map stuff
	int8_t c_map[MAP_MAX_Y+160][MAP_MAX_X+160];
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
	pthread_t cthread; // thread specific the scripted encounters daemon
	nomadlong_t hudtics;
	std::atomic<nomaduint_t> pdmg; // amount of damage done to the player in a single tic
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

	npc_t N_MakeNPC(void);
	void I_InitNPCs(void);

	inline coord_t E_TryMove(coord_t* epos, nomadenum_t* edir);
	nomadenum_t E_GetFacing(coord_t current, coord_t orig);
	void E_MoveImmediate(coord_t* epos, nomadenum_t edir); // the checks have already been performed by said entity
	coord_t E_GetDir(nomadenum_t dir);
	nomadbool_t E_Move(coord_t* epos, nomadenum_t* edir);
	void G_SaveState(void);
	void G_SaveRecentSlot(void);
};


void LooperDelay(nomaduint_t numsecs);
void MobtAssigner(Game* const gptr);
void MobAssigner(Game* const gptr);
void NPCAssigner(Game* const gptr);
void NomadAssigner(Game* const gptr);
void CampaignAssigner(Game* const gptr);

nomaduint_t G_GetNumMobs(const Game* const game);
nomaduint_t G_GetNumBots(const Game* const game);

void TUI_Init(Game* const game);
void G_CampaignSelect();
void G_LoadBFF(const char* bffname, Game* const game);
void I_NomadInit(int argc, char* argv[], Game* game);
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
extern const char* about_str;
extern const char* credits_str;

#endif
