#ifndef _G_GAME_
#define _G_GAME_

#include "n_shared.h"
#include "p_npc.h"
#include "g_playr.h"
#include "g_mob.h"
#include "g_zone.h"
#include "g_map.h"

#ifndef _NOMAD_MAIN_
#define _NOMAD_MAIN_
#endif

// used for loading the bff, tracking the progress // nope, not anymore

#define ctrl(x) (x & 0x1F)
constexpr auto NotFound = std::string::npos;
//#define MAX_SAVES	15

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
	nomaduint_t ticcount;
	gamestate_t gamestate;
	nomadenum_t gamescreen;
	char bffname[80];
	Playr* playr;
	std::vector<Mob*> m_Active;
	std::vector<NPC*> b_Active;
public: // map stuff
	pint_t mapbuffer[MAP_MAX_Y+160][MAP_MAX_X+160];
	char c_map[MAP_MAX_Y+160][MAP_MAX_X+160];
public:
	WINDOW* screen;
	WINDOW* hudwin[NUMHUDLAYERS];
public:
	Game();
	~Game();
	collider_t P_RangedCollider(nomadushort_t range, nomaduint_t wpn);
	collider_t P_RangedCollider(nomadushort_t range);
	collider_t P_CloseCollider(nomadenum_t dir);
	
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
	void E_MoveImmediate(coord_t* epos, nomadenum_t edir); // the checks have already been performed by said entity
	coord_t E_GetDir(nomadenum_t dir);
	nomadbool_t E_Move(coord_t* epos, nomadenum_t* edir);
	void M_FollowPlayr(Mob* const mob, nomadbool_t smell, nomadbool_t hear,
		nomadbool_t see);
};

void I_NomadInit(int argc, char* argv[], Game* game);
void mainLoop(int argc, char* argv[]);
void N_Error(const char* err, ...);

#endif
