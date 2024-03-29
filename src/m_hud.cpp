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
// DESCRIPTION: src/m_hud.cpp
//  HUD rendering and display code
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"

#define HEALTH_COLOR_GOOD 1
#define HEALTH_COLOR_MED  2
#define HEALTH_COLOR_CRIT 3

static constexpr uint8_t vert_fov = MAX_VERT_FOV >> 1;
static constexpr uint8_t horz_fov = MAX_HORZ_FOV >> 1;
#define mapfile "Files/gamedata/RUNTIME/mapfile.txt"
static coord_t origin = coord_t(260, 260);

static inline void P_GetMapBuffer();
static inline void Hud_GetVMatrix();
static inline void Hud_InsertSprites();

static Game* game;
static Playr* playr;

static inline const char* Hud_ArmorToStr(nomadenum_t armor)
{
	switch (armor) {
	case ARMOR_STREET: return "Street-Grade";
	case ARMOR_MILITARY: return "Military-Grade";
	case ARMOR_MERC: return "Mercenary's Armor";
	};
	return "Unknown Armor";
}

static nomadlong_t hudtics{};
static std::string hudbuf;
static constexpr uint16_t BUFFER_SIZE = 90;
static char hudbuffer[BUFFER_SIZE];

static inline void Hud_DisplayConsole();
static inline void Hud_DisplayBarVitals();
static inline void Hud_DisplayBodyVitals();
static inline void Hud_DisplayCompass();
static inline void Hud_DisplayVMatrix();
static inline void Hud_DisplayWeapons();
static inline void Hud_DisplayLocation();
#if 0

static SDL_Renderer* Hud_Renderer;
static inline void Hud_SDL_ClearBuffer()
{
	SDL_RenderClear(game->SDL_renderer);
}
static inline void Hud_SDL_DisplayConsole();
static inline void Hud_SDL_DisplayBarVitals();
static inline void Hud_SDL_DisplayBodyVitals();
static inline void Hud_SDL_DisplayCompass();
static inline void Hud_SDL_DisplayVMartix();
static inline void Hud_SDL_DisplayWeapons();
static inline void Hud_SDL_DisplayLocation();
static inline void Hud_SDL_FlushBuffer()
{
	SDL_RenderPresent(game->SDL_renderer);
}

enum
{
	DISPLAY_CLEAR_BUFFER,
	DISPLAY_LOCATION,
	DISPLAY_BAR_VITALS,
	DISPLAY_BODY_VITALS,
	DISPLAY_COMPASS,
	DISPLAY_CONSOLE,
	DISPLAY_WEAPONS,
	DISPLAY_VMATRIX,
	DISPLAY_FLUSH_BUFFER,
	
	NUM_RENDER_FUNCS
};

static std::function<void()> renderer[NUM_RENDER_FUNCS];
#endif

void Hud_Printf(const char* from, const char* msg, ...)
{
	PTR_CHECK(NULL_CHECK, from);
	PTR_CHECK(NULL_CHECK, msg);
	va_list argptr;
	va_start(argptr, msg);
	vsnprintf(hudbuffer, BUFFER_SIZE, msg, argptr);
	va_end(argptr);
	for (nomadshort_t i = 0; i < BUFFER_SIZE; ++i) {
		mvwaddch(game->screen, 30, i+36, ' ');
	}
	mvwprintw(game->screen, 30, 32, "[%s]: %s", from, hudbuffer);
}

static inline void Hud_DisplayConsole()
{
	if (hudtics > -1) {
		--hudtics;
	}
	else {
		hudtics = ticrate_base*5;
		for (nomadshort_t i = 32; i < getmaxx(game->screen) - 2; ++i) {
			mvwaddch(game->screen, 30, i, ' ');
		}
		// fixed the FIXME
	}
}

static void HudAssigner(Game* const gptr)
{
	game = gptr;
	playr = game->playr;
}

void Game::I_InitHUD(void)
{
	puts("I_InitHud(): Initializing HUD data and view matrix...");
	LOG_INFO("Initializing HUD data");
	HudAssigner(this);
	playr->pos = origin;
	hudtics = 0;
	memset(hudbuffer, 0, sizeof(hudbuffer));
	Hud_GetVMatrix();
}

void Game::G_DisplayHUD(void)
{
	nomadenum_t i, a;
	Hud_DisplayLocation();
	Hud_DisplayBarVitals();
	Hud_DisplayBodyVitals();
	Hud_DisplayCompass();
	Hud_DisplayConsole();
	Hud_DisplayWeapons();
	Hud_DisplayVMatrix();
	for (a = 1; a < 7; ++a) {
		for (i = 5; i < 28; ++i) {
			mvwaddch(game->screen, i, a, '#');
		}
	}
	for (i = 7; i < 95; ++i) { 
		mvwaddch(game->screen, 3, i, '#');
	}
}

static inline void Hud_InsertSprites()
{
//	for (const auto* i : game->b_Active) {
//		game->c_map[i->pos.y][i->pos.x] = i->sprite;
//	}
//	for (std::vector<Mob*>::const_iterator it = game->m_Active.begin(); it != game->m_Active.end(); ++it) {
//		game->c_map[(*it)->mpos.y][(*it)->mpos.x] = (*it)->sprite;
//	}
}

static inline nomaduint_t B_GetSector(const coord_t& pos)
{
	// in sectors 0, 7, or 6
	if (pos.y > 79 && pos.y < 200) {
		if (pos.x > 79 && pos.x < 200) { return 0; }
		else if (pos.x > 200 && pos.x < 320) { return 7; }
		else if (pos.x > 320 && pos.x < 520) { return 6; }
	}
	// in sectors 1, 8, or 5
	else if (pos.y > 200 && pos.y < 320) {
		if (pos.x > 79 && pos.x < 200) { return 1; }
		else if (pos.x > 200 && pos.x < 320) { return 8; }
		else if (pos.x > 320 && pos.x < 520) { return 5; }
	}
	// in sectors 2, 3, or 4
	else if (pos.y > 320 && pos.y < 520) {
		if (pos.x > 79 && pos.x < 200) { return 2; }
		else if (pos.x > 200 && pos.x < 320) { return 3; }
		else if (pos.x > 320 && pos.x < 520) { return 4; }
	}
	else { return 0; } // TODO: make errors
	return 0; // compiler warning if this ain't here
}

static inline void Hud_DisplayLocation()
{
	char name[81];
	switch (B_GetSector(playr->pos)) {
	case 0:
		strncpy(name, "Dunes of Desolation", sizeof(name));
		break;
	case 1:
		strncpy(name, "The Ancient Wastes", sizeof(name));
		break;
	case 2:
		strncpy(name, "The Salt Wastes", sizeof(name));
		break;
	case 3:
		strncpy(name, "The Ashen Plains", sizeof(name));
		break;
	case 4:
		strncpy(name, "The Burnt Hills", sizeof(name));
		break;
	case 5:
		strncpy(name, "Sea of Sands", sizeof(name));
		break;
	case 6:
		strncpy(name, "Demagel Canyons", sizeof(name));
		break;
	case 7:
		strncpy(name, "The Frozen North", sizeof(name));
		break;
	case 8:
		strncpy(name, "The Eternal City of Galakas", sizeof(name));
		break;
	};
	wmove(game->screen, 1, 8);
	wclrtoeol(game->screen);
	mvwprintw(game->screen, 1, 8, "Location/Biome: %s", name);
	mvwprintw(game->screen, 2, 8, "Coords: (y) %li, (x) %li", playr->pos.y, playr->pos.x);
	mvwaddch(game->screen, 1, (getmaxx(game->screen) - 1), '#');
}

static inline void Hud_DisplayCompass()
{
	nomadenum_t i;
	for (i = 0; i < 7; ++i) {
		mvwaddch(game->screen, 4, i, '#');
	}
	for (i = 3; i > 0; --i) {
		mvwaddch(game->screen, i, 6, '#');
	}
	switch (playr->pdir) {
	case D_NORTH:
		mvwaddch(game->screen, 2, 3, '*');
		mvwaddch(game->screen, 1, 3, '|');
		break;
	case D_WEST:
		mvwaddch(game->screen, 2, 3, '*');
		mvwaddch(game->screen, 2, 2, '-');
		break;
	case D_SOUTH:
		mvwaddch(game->screen, 2, 3, '*');
		mvwaddch(game->screen, 3, 3, '|');
		break;
	case D_EAST:
		mvwaddch(game->screen, 2, 3, '*');
		mvwaddch(game->screen, 2, 4, '-');
		break;
	};
}

static inline void Hud_DisplayBodyVitals()
{
	nomadenum_t i;
	// now displaying the body parts
	for (i = 28; i > 0; --i) { // drawing columns
		mvwaddch(game->screen, i, 95, '#');
	}
	mvwaddch(game->screen, 2, 100, '[');
	mvwaddch(game->screen, 2, 101, ']');
	mvwaddch(game->screen, 3, 100, '[');
	mvwaddch(game->screen, 3, 101, ']');
	mvwaddch(game->screen, 3, 99, '/');
	mvwaddch(game->screen, 3, 102, '\\');
	mvwaddch(game->screen, 4, 100, ':');
	mvwaddch(game->screen, 4, 99, '_');
	mvwaddch(game->screen, 4, 101, ':');
	mvwaddch(game->screen, 4, 102, '_');
	for (i = 96; i < 128; ++i) {
		mvwaddch(game->screen, 6, i, '#');
	}
	mvwaddstr(game->screen, 1, 105, "HEAD");
	mvwaddstr(game->screen, 2, 105, "ARMS");
	mvwaddstr(game->screen, 3, 105, "LEGS");
	mvwaddstr(game->screen, 4, 105, "BODY");

	for (i = 1; i < 5; ++i) {
		mvwaddch(game->screen, i, 111, '[');
		for (nomadenum_t a = 112; a < 123; ++a) {
			mvwaddch(game->screen, i, a, ':');
		}
		mvwaddch(game->screen, i, 123, ']');
	}
}

static inline void Hud_DisplayBarVitals()
{
	nomadenum_t i;
	// displaying the lower-left-hand corner of the vitals
	for (i = 1; i < 128; ++i) {
		mvwaddch(game->screen, 28, i, '#');
	}
	mvwaddch(game->screen, 29, 30, '#');
	mvwaddch(game->screen, 30, 30, '#');
	mvwaddch(game->screen, 31, 30, '#');
	mvwaddch(game->screen, 32, 30, '#');
	mvwaddch(game->screen, 29, 1, '[');
	mvwaddch(game->screen, 29, 29, ']');
	
	init_pair(HEALTH_COLOR_GOOD, COLOR_GREEN, COLOR_BLACK);
	init_pair(HEALTH_COLOR_MED, COLOR_YELLOW, COLOR_BLACK);
	init_pair(HEALTH_COLOR_CRIT, COLOR_RED, COLOR_BLACK);
	nomadenum_t health_color;
	if (playr->health > 75)
		health_color = HEALTH_COLOR_GOOD;
	else if (playr->health < 76 && playr->health > 24)
		health_color = HEALTH_COLOR_MED;
	else if (playr->health < 25)
		health_color = HEALTH_COLOR_CRIT;

	wattron(game->screen, COLOR_PAIR(health_color));
	for (i = 2; i < 29; ++i) {
		mvwaddch(game->screen, 29, i, ':');
	}
	wattroff(game->screen, COLOR_PAIR(health_color));
	mvwaddstr(game->screen, 30, 12,  "<HEALTH>");
	mvwprintw(game->screen, 32, 2, "ARMOR: %s", Hud_ArmorToStr(playr->armor));
}

static char wpnbuffer[120]={0};

static inline const char* getstring(ammotype_t type)
{
	if (playr->ammunition[type] < 10) {
		switch (playr->ammunition[type]) {
		case 0: return "0";
		case 1: return "1";
		case 2: return "2";
		case 3: return "3";
		case 4: return "4";
		case 5: return "5";
		case 6: return "6";
		case 7: return "7";
		case 8: return "8";
		case 9: return "9";
		};
	}
	else
		return std::to_string(playr->ammunition[type]).c_str();
	
	LOG_WARN("ammotype_t passed to getstring held an invalid integer within playr->ammuntion[type], returning \"unknown\"");
	return "unknown";
}

static inline void Hud_DisplayWeapons()
{
	nomadenum_t num = 1;
	for (nomadenum_t i = 97; num < 10; i += 3) {
		mvwaddch(game->screen, 7, i, '[');
		mvwaddch(game->screen, 7, i + 1, ((char)num + '0'));
		mvwaddch(game->screen, 7, i + 2, ']');
		++num;
	}
	mvwaddch(game->screen, 7, 124, '[');
	mvwaddch(game->screen, 7, 125, '0');
	mvwaddch(game->screen, 7, 126, ']');
	mvwaddstr(game->screen, 8,  97, "[Weapon In Hand]:");
	mvwaddstr(game->screen, 9,  97, playr->c_wpn->c_wpn.name);
	mvwaddstr(game->screen, 11, 97, "[Ammunition]:");
#ifdef _WIN32
	mvwprintw(game->screen, 12, 97, "Shotgun Shells: %s",  itoa(playr->ammunition[AT_SHELL], wpnbuffer, 10));
	mvwprintw(game->screen, 13, 97, "Bullets: %s", itoa(playr->ammunition[AT_BULLET], wpnbuffer, 10));
	mvwprintw(game->screen, 14, 97, "Fusion Metals: %s", itoa(playr->ammunition[AT_FUSION], wpnbuffer, 10));
	mvwprintw(game->screen, 15, 97, "Plasma Batteries: %s", itoa(playr->ammunition[AT_PLASMA], wpnbuffer, 10));
#elif defined(__unix__)
	mvwprintw(game->screen, 12, 97, "Shotgun Shells: %03i", playr->ammunition[AT_SHELL]);
	mvwprintw(game->screen, 13, 97, "Bullets: %03i", playr->ammunition[AT_BULLET]);
	mvwprintw(game->screen, 14, 97, "Fusion Metals: %03i", playr->ammunition[AT_FUSION]);
	mvwprintw(game->screen, 15, 97, "Plasma Batteries: %03i", playr->ammunition[AT_PLASMA]);
#endif
}

void Hud_DisplayWpnSlot(nomadenum_t wpn_slot)
{
	switch (wpn_slot) {
	case 0:
		return;
		break;
	case 1:
		mvwprintw(game->screen, 8,  97, "1) %s", leftarm.c_wpn.name);
		mvwprintw(game->screen, 9,  97, "2) %s", rightarm.c_wpn.name);
		mvwprintw(game->screen, 10, 97, "3) %s", melee1.c_wpn.name);
		mvwprintw(game->screen, 11, 97, "4) %s", melee2.c_wpn.name);
		mvwprintw(game->screen, 12, 97, "5) %s", melee3.c_wpn.name);
		break;
	case 2:
		mvwprintw(game->screen, 8,  97, "1) %s", sidearm.c_wpn.name);
		mvwprintw(game->screen, 9,  97, "2) %s", heavyside.c_wpn.name);
		break;
	case 3:
		mvwprintw(game->screen, 8,  97, "1) %s", primary.c_wpn.name);
		mvwprintw(game->screen, 9,  97, "2) %s", heavyprimary.c_wpn.name);
		mvwprintw(game->screen, 10, 97, "3) %s", shotty.c_wpn.name);
		break;
	default:
		N_Error("Invalid wpn_slot selected: %hu", wpn_slot);
		break;
	};
}

static inline void R_DrawPixel(nomadshort_t y, nomadshort_t x, nomadshort_t u, nomadshort_t c)
{
//	if ((y < 23 && y > 2) && (x < 74 && x > 14)) {
//		wattroff(game->hudwin[HL_VMATRIX], A_DIM | A_INVIS);
//		wattron(game->hudwin[HL_VMATRIX], A_NORMAL);
//	}
//	else {
//		wattroff(game->hudwin[HL_VMATRIX], A_NORMAL);
//		wattron(game->hudwin[HL_VMATRIX], A_DIM | A_INVIS);
//	}
	if (y == 12 && x == 44) {
		mvwaddch(game->hudwin[HL_VMATRIX],
			y, x, playr->sprite);
	}
	else {
		mvwaddch(game->hudwin[HL_VMATRIX],
			y, x, playr->vmatrix[u][c]);
	}
}


static inline void Hud_DisplayVMatrix()
{
	Hud_GetVMatrix();
	nomaduint_t u = 0, c = 0;
	werase(game->hudwin[HL_VMATRIX]);
	for (nomaduint_t y = 0; y < MAX_VERT_FOV; ++y) {
		for (nomaduint_t x = 0; x < MAX_HORZ_FOV; ++x) {
			R_DrawPixel(y, x, u, c);
			c++;
		}
		u++;
	}
	wrefresh(game->hudwin[HL_VMATRIX]);
}

static inline void P_GetMapBuffer()
{
#if 0
	std::ifstream file(mapfile, std::ios::in);
	if (file.fail()) {
		N_Error("Could Not Open File %s", mapfile);
	}
	std::string line;
#endif
	filestream fp(mapfile, "r");
	nomaduint_t y = 0, x = 0;
	char c = fp.getc();
	while (c != EOF) {
		if (c == '\n') { ++y; x = 0; }
		else {
			game->c_map[y][x] = c;
			++x;
		}
		c = fp.getc();
	}
//	while (std::getline(file, line)) {
//		for (nomaduint_t x = 0; x < line.size(); ++x) {
//			game->c_map[y][x] = line[x];
//		}
//		y++;
//	};
//	file.close();
}

static inline nomadbool_t inbuilding(nomadshort_t y, nomadshort_t x)
{
	nomadenum_t counter = 0;
	// check up
	for (nomadshort_t u = y; u > -1; --u) {
		switch (playr->vmatrix[u][x]) {
		case '#':
			u = -1;
			++counter;
			break;
		case '.':
			u = -1;
			break;
		default: break;
		};
	}
	// check down
	for (nomadshort_t u = y; u < MAX_VERT_FOV; ++u) {
		switch (playr->vmatrix[u][x]) {
		case '#':
			u = MAX_VERT_FOV;
			++counter;
			break;
		case '.':
			u = MAX_VERT_FOV;
			break;
		default: break;
		};
	}
	// check left
	for (nomadshort_t c = x; c > -1; --c) {
		switch (playr->vmatrix[y][c]) {
		case '#':
			c = -1;
			++counter;
			break;
		case '.':
			c = -1;
			break;
		default: break;
		};
	}
	// check right
	for (nomadshort_t c = x; c < MAX_HORZ_FOV; ++c) {
		switch (playr->vmatrix[y][c]) {
		case '#':
			c = MAX_HORZ_FOV;
			++counter;
			break;
		case '.':
			c = -1;
			break;
		default: break;
		};
	}
	return counter >= 3 ? true : false;
}

static inline void Hud_FilterVMatrix()
{
	for (nomadshort_t y = MAX_VERT_FOV - 1; y > -1; --y) {
		for (nomadshort_t x = MAX_HORZ_FOV - 1; x > -1; --x) {
			switch (playr->vmatrix[y][x]) {
			case ' ': {
				if (!inbuilding(y, x)) { playr->vmatrix[y][x] = '#'; }
				break; }
			default: break;
			};
		}
	}
}

static inline void Hud_GetVMatrix()
{
//	LOG_PROFILE();
	P_GetMapBuffer();
	game->c_map[playr->pos.y][playr->pos.x] = playr->sprite;
	coord_t startc;
	coord_t endc;
	startc.y = playr->pos.y - vert_fov;
	startc.x = playr->pos.x - horz_fov;
	endc.y = playr->pos.y + vert_fov;
	endc.x = playr->pos.x + horz_fov;
	memset(playr->vmatrix, '#', sizeof(playr->vmatrix));
	
	nomadlong_t u, c;
	u = c = 0;
	if (game->m_Active.size() > 0) {
		for (linked_list<Mob*>::iterator it = game->m_Active.begin(); it != game->m_Active.end(); it = it->next) {
			game->c_map[it->val->mpos.y][it->val->mpos.x] = it->val->sprite;
		}
	}
	if (game->items.size() > 0) {
		for (linked_list<item_t*>::iterator it = game->items.begin(); it != game->items.end(); it = it->next) {
			game->c_map[it->val->pos.y][it->val->pos.x] = '!';
		}
	}
//	for (const auto* const i : game->items) {
//		game->c_map[i->pos.y][i->pos.x] = sprites[SPR_PICKUP];
//	}
//	Hud_InsertSprites();
	for (nomadshort_t y = startc.y; y < endc.y; ++y) {
		for (nomadshort_t x = startc.x; x < endc.x; ++x) {
			playr->vmatrix[u][c] = game->c_map[y][x];
			c++;
		}
		u++;
	}
}
