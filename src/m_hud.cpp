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
//  src/m_hud.cpp
//----------------------------------------------------------
#include "g_game.h"

static constexpr uint8_t vert_fov = MAX_VERT_FOV >> 1;
static constexpr uint8_t horz_fov = MAX_HORZ_FOV >> 1;
static constexpr auto mapfile = "Files/gamedata/RUNTIME/mapfile.txt";
static constexpr coord_t origin = {260, 260};

static inline void P_GetMapBuffer();
static inline void Hud_GetVMatrix();
static inline void Hud_InsertSprites();

static Game* game;
static Playr* playr;

static inline void Hud_DisplayConsole();
static inline void Hud_DisplayBarVitals();
static inline void Hud_DisplayBodyVitals();
static inline void Hud_DisplayCompass();
static inline void Hud_DisplayVMatrix();
static inline void Hud_DisplayWeapons();
static inline void Hud_DisplayLocation();

static nomadlong_t hudtics{};
static std::string hudbuf;
static constexpr uint16_t BUFFER_SIZE = 256;
static char* hudbuffer;

void Hud_Printf(const char* from, const char* msg, ...)
{
#ifdef _NOMAD_DEBUG
	assert(msg && from);
#endif
	va_list argptr;
	va_start(argptr, msg);
	vsnprintf(hudbuffer, BUFFER_SIZE, msg, argptr);
	va_end(argptr);
	wmove(game->screen, 30, 31);
	wclrtoeol(game->screen);
	wrefresh(game->screen);
	mvwprintw(game->screen, 30, 32, "[%s] %s", from, hudbuffer);
	mvwaddch(game->screen, 30, 128, '#');
}

static inline void Hud_DisplayConsole()
{
	if (hudtics > -1) {
		--hudtics;
	}
	else {
		hudtics = ticrate_base*5;
		wmove(game->screen, 30, 31);
		wclrtoeol(game->screen);
		wrefresh(game->screen);
		mvwaddch(game->screen, 30, 128, '#');
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
	printf("I_InitHUD(): Initializing HUD...\n");
	HudAssigner(this);
	playr->pos = origin;
	hudtics = 0;
	hudbuffer =  (char *)Z_Malloc(BUFFER_SIZE, TAG_STATIC, &hudbuffer);
	if (!hudbuffer)
		N_Error("Failed to allocate memory to hudbuffer!");

	Hud_GetVMatrix();
}

void Game::G_DisplayHUD(void)
{
	nomadenum_t i, a;
	Hud_DisplayConsole();
	Hud_DisplayBarVitals();
	Hud_DisplayBodyVitals();
	Hud_DisplayCompass();
	Hud_DisplayLocation();
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
	for (const auto* i : game->b_Active)
		game->c_map[i->pos.y][i->pos.x] = i->c_npc.sprite;
	for (const auto* i : game->m_Active)
		game->c_map[i->mpos.y][i->mpos.x] = i->c_mob.sprite;
}

static inline nomaduint_t B_GetSector(coord_t pos)
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
	mvwprintw(game->screen, 1, 8, "Location/Biome: %s", name);
	// ok, so you may need to adjust this if you're using _NOMAD_32
	mvwprintw(
		game->screen, 2, 8, "Coords: (y) %i, (x) %i", playr->pos.y, playr->pos.x);
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
	
	// i think these colors won't display currently,
	// TODO?
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	attron(COLOR_PAIR(1));
	for (i = 2; i < 29; ++i) {
		mvwaddch(game->screen, 29, i, ':');
	}
	attroff(COLOR_PAIR(1));
	mvwaddstr(game->screen, 30, 12, "<HEALTH>");

	mvwaddch(game->screen, 31, 1, '[');
	attron(COLOR_PAIR(1));
	for (i = 2; i < 29; ++i) {
		mvwaddch(game->screen, 31, i, ':');
	}
	attroff(COLOR_PAIR(1));
	mvwaddch(game->screen, 31, 29, ']');
	mvwaddstr(game->screen, 32, 11, " <ARMOUR>");
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

static inline void Hud_DisplayVMatrix()
{
	Hud_GetVMatrix();
	nomaduint_t u = 0, c = 0;
	werase(game->hudwin[HL_VMATRIX]);
	for (nomaduint_t y = 0; y < MAX_VERT_FOV; ++y) {
		for (nomaduint_t x = 0; x < MAX_HORZ_FOV; ++x) {
			if (y == 12 && x == 44) {
				mvwaddch(game->hudwin[HL_VMATRIX],
					y, x, playr->sprite);
			} else {
				mvwaddch(game->hudwin[HL_VMATRIX],
					y, x, playr->vmatrix[u][c]);
			}
			c++;
		}
		u++;
	}
	wrefresh(game->hudwin[HL_VMATRIX]);
}

static inline void P_GetMapBuffer()
{
	std::ifstream file(mapfile, std::ios::in);
	if (file.fail()) {
		N_Error("Could Not Open File %s", mapfile);
	}
	std::string line;
	nomaduint_t y = 0;
	while (std::getline(file, line)) {
		for (nomaduint_t x = 0; x < line.size(); ++x) {
			game->c_map[y][x] = line[x];
		}
		y++;
	};
	file.close();
}

static inline void G_ResetMap();

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
	P_GetMapBuffer();
	G_ResetMap();
	coord_t startc;
	coord_t endc;
	startc.y = playr->pos.y - vert_fov;
	startc.x = playr->pos.x - horz_fov;
	endc.y = playr->pos.y + vert_fov;
	endc.x = playr->pos.x + horz_fov;
	memset(&playr->vmatrix, '#', sizeof(playr->vmatrix));
	
	nomadshort_t u, c;
	u = c = 0;
	Hud_InsertSprites();
	for (nomadshort_t y = startc.y; y < endc.y; ++y) {
		for (nomadshort_t x = startc.x; x < endc.x; ++x) {
			playr->vmatrix[u][c] = game->c_map[y][x];
			c++;
		}
		u++;
	}
//	Hud_FilterVMatrix();
}

static inline void G_SetSndPerim(nomadenum_t by, sndlvl_t num, coord_t from)
{
	game->sndmap[from.y + by][from.x] = num;
	game->sndmap[from.y][from.x + by] = num;
	game->sndmap[from.y - by][from.x] = num;
	game->sndmap[from.y][from.x - by] = num;
	game->sndmap[from.y + by][from.x + by] = num;
	game->sndmap[from.y + by][from.x - by] = num;
	game->sndmap[from.y - by][from.x + by] = num;
	game->sndmap[from.y - by][from.x - by] = num;
}

static inline void G_SetSmellPerim(nomadenum_t by, smelllvl_t num, coord_t from)
{
	game->smellmap[from.y + by][from.x] = num;
	game->smellmap[from.y][from.x + by] = num;
	game->smellmap[from.y - by][from.x] = num;
	game->smellmap[from.y][from.x - by] = num;
	game->smellmap[from.y + by][from.x + by] = num;
	game->smellmap[from.y + by][from.x - by] = num;
	game->smellmap[from.y - by][from.x + by] = num;
	game->smellmap[from.y - by][from.x - by] = num;
}

static inline void G_SetMapSnd()
{
	for (nomadint_t y = 0; y < MAP_MAX_Y+160; ++y) {
		for (nomadint_t x = 0; x < MAP_MAX_X+160; ++x) {
			game->sndmap[y][x] = SND_LOW;
		}
	}
}

static inline void G_SetMapSmell()
{
	for (nomadint_t y = 0; y < MAP_MAX_Y+160; ++y) {
		for (nomadint_t x = 0; x < MAP_MAX_X+160; ++x) {
			game->smellmap[y][x] = SMELL_LOW;
		}
	}
}

static inline void G_ResetMap()
{
//	nomadenum_t i;
//	G_SetMapSnd(game);
//	G_SetMapSmell(game);
	/*for (i = 0; i < game->m_Active.size(); ++i) {
		Mob* mob = game->m_Active[i];
		game->sndmap[mob->mpos.y][mob->mpos.x] = SND_MEDIUM;
		game->smellmap[mob->mpos.y][mob->mpos.x] = SMELL_MEDIUM;
		G_SetSndPerim(game, 1, SND_MEDIUM, mob->mpos);
		G_SetSndPerim(game, 2, SND_MEDIUM, mob->mpos);
		G_SetSndPerim(game, 3, SND_SOME, mob->mpos);
		G_SetSndPerim(game, 4, SND_SOME, mob->mpos);
	}
	game->sndmap[game->playr->pos.y][game->playr->pos.x] = SND_MEDIUM;
	game->smellmap[game->playr->pos.y][game->playr->pos.x] = SMELL_MEDIUM; */
	game->c_map[playr->pos.y][playr->pos.x] = '@';
/*	for (i = 1; i < 4; ++i) {
		G_SetSndPerim(game, i, SND_MEDIUM, game->playr->pos);
		G_SetSmellPerim(game, i, SMELL_MEDIUM, game->playr->pos);
	}
	for (i = 4; i < 6; ++i) {
		G_SetSndPerim(game, i, SND_SOME, game->playr->pos);
		G_SetSmellPerim(game, i, SMELL_SOME, game->playr->pos);
	}*/
}