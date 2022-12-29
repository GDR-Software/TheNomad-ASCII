#include "g_game.h"

static constexpr uint8_t vert_fov = MAX_VERT_FOV >> 1;
static constexpr uint8_t horz_fov = MAX_HORZ_FOV >> 1;
static constexpr auto mapfile = "Files/gamedata/RUNTIME/mapfile.txt";
static constexpr coord_t origin = {260, 260};

static inline void P_GetMapBuffer(Game* const game);
static inline void Hud_GetVMatrix(Game* const game);
static inline void Hud_InsertSprites(Game* const game);

static inline void Hud_DisplayConsole(WINDOW* const screen);
static inline void Hud_DisplayBarVitals(Game* const game);
static inline void Hud_DisplayBodyVitals(Game* const game);
static inline void Hud_DisplayCompass(Game* const game);
static inline void Hud_DisplayVMatrix(Game* const game);
static inline void Hud_DisplayWeapons(Game* const game);
static inline void Hud_DisplayLocation(Game* const game);

void Game::I_InitHUD(void)
{
	printf("I_InitHUD(): Initializing HUD...\n");
	playr->pos = origin;
	Hud_GetVMatrix(this);
}

void Game::G_DisplayHUD(void)
{
	nomadenum_t i, a;
	Hud_DisplayBarVitals(this);
	Hud_DisplayBodyVitals(this);
	Hud_DisplayConsole(screen);
	Hud_DisplayCompass(this);
	Hud_DisplayLocation(this);
	Hud_DisplayWeapons(this);
	Hud_DisplayVMatrix(this);
	for (a = 1; a < 7; ++a) {
		for (i = 5; i < 28; ++i) {
			mvwaddch(screen, i, a, '#');
		}
	}
	for (i = 7; i < 95; ++i) { 
		mvwaddch(screen, 3, i, '#');
	}
}

static inline void Hud_InsertSprites(Game* const game)
{
	nomadushort_t i;
	for (i = 0; i < game->b_Active.size(); ++i) {
		const NPC* npc = game->b_Active[i];
		game->c_map[npc->pos.y][npc->pos.x] = npc->c_npc.sprite;
	}
	for (i = 0; i < game->m_Active.size(); ++i) {
		const Mob* mob = game->m_Active[i];
		game->c_map[mob->mpos.y][mob->mpos.x] = mob->c_mob.sprite;
	}
}

static inline nomaduint_t B_GetSector(coord_t pos)
{
	// in sectors 0, 7, or 6
	if (pos.y > 80 && pos.y < 199) {
		if (pos.x > 80 && pos.x < 199) { return 0; }
		else if (pos.x > 199 && pos.x < 319) { return 7; }
		else if (pos.x > 319 && pos.x < 519) { return 6; }
	}
	// in sectors 1, 8, or 5
	else if (pos.y > 199 && pos.y < 319) {
		if (pos.x > 80 && pos.x < 199) { return 1; }
		else if (pos.x > 199 && pos.x < 319) { return 8; }
		else if (pos.x > 319 && pos.x < 519) { return 5; }
	}
	// in sectors 2, 3, or 4
	else if (pos.y > 319 && pos.y < 519) {
		if (pos.x > 80 && pos.x < 199) { return 2; }
		else if (pos.x > 199 && pos.x < 319) { return 3; }
		else if (pos.x > 319 && pos.x < 519) { return 4; }
	}
	else { return 0; } // TODO: make errors
	return 0; // compiler warning if this ain't here
}

static inline void Hud_DisplayLocation(Game* const game)
{
	const Playr* playr = game->playr;
	WINDOW* const screen = game->screen;
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
	mvwprintw(screen, 1, 8, "Location/Biome: %s", name);
	// ok, so you may need to adjust this if you're using _NOMAD_32
	mvwprintw(
		screen, 2, 8, "Coords: (y) %i, (x) %i", playr->pos.y, playr->pos.x);
}

static inline void Hud_DisplayCompass(Game* const game)
{
	WINDOW* const screen = game->screen;
	const Playr* playr = game->playr;
	nomadenum_t i;
	for (i = 0; i < 7; ++i) {
		mvwaddch(screen, 4, i, '#');
	}
	for (i = 3; i > 0; --i) {
		mvwaddch(screen, i, 6, '#');
	}
	switch (playr->pdir) {
	case D_NORTH:
		mvwaddch(screen, 2, 3, '*');
		mvwaddch(screen, 1, 3, '|');
		break;
	case D_WEST:
		mvwaddch(screen, 2, 3, '*');
		mvwaddch(screen, 2, 2, '-');
		break;
	case D_SOUTH:
		mvwaddch(screen, 2, 3, '*');
		mvwaddch(screen, 3, 3, '|');
		break;
	case D_EAST:
		mvwaddch(screen, 2, 3, '*');
		mvwaddch(screen, 2, 4, '-');
		break;
	};
}

static inline void Hud_DisplayConsole(WINDOW* const screen)
{
	mvwaddch(screen, 32, 33, '>');
	mvwaddch(screen, 32, 32, '/');
}

static inline void Hud_DisplayBodyVitals(Game* const game)
{
	WINDOW* const screen = game->screen;
	nomadenum_t i;
	// now displaying the body parts
	for (i = 28; i > 0; --i) { // drawing columns
		mvwaddch(screen, i, 95, '#');
	}
	mvwaddch(screen, 2, 100, '[');
	mvwaddch(screen, 2, 101, ']');
	mvwaddch(screen, 3, 100, '[');
	mvwaddch(screen, 3, 101, ']');
	mvwaddch(screen, 3, 99, '/');
	mvwaddch(screen, 3, 102, '\\');
	mvwaddch(screen, 4, 100, ':');
	mvwaddch(screen, 4, 99, '_');
	mvwaddch(screen, 4, 101, ':');
	mvwaddch(screen, 4, 102, '_');
	for (i = 96; i < 128; ++i) {
		mvwaddch(screen, 6, i, '#');
	}
	mvwaddstr(screen, 1, 105, "HEAD");
	mvwaddstr(screen, 2, 105, "ARMS");
	mvwaddstr(screen, 3, 105, "LEGS");
	mvwaddstr(screen, 4, 105, "BODY");

	for (i = 1; i < 5; ++i) {
		mvwaddch(screen, i, 111, '[');
		for (nomadenum_t a = 112; a < 123; ++a) {
			mvwaddch(screen, i, a, ':');
		}
		mvwaddch(screen, i, 123, ']');
	}
}

static inline void Hud_DisplayBarVitals(Game* const game)
{
	WINDOW* const screen = game->screen;
	nomadenum_t i;
	// displaying the lower-left-hand corner of the vitals
	for (i = 1; i < 128; ++i) {
		mvwaddch(screen, 28, i, '#');
	}
	mvwaddch(screen, 29, 30, '#');
	mvwaddch(screen, 30, 30, '#');
	mvwaddch(screen, 31, 30, '#');
	mvwaddch(screen, 32, 30, '#');
	mvwaddch(screen, 29, 1, '[');
	mvwaddch(screen, 29, 29, ']');
	
	// i think these colors won't display currently,
	// TODO?
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	attron(COLOR_PAIR(1));
	for (i = 2; i < 29; ++i) {
		mvwaddch(screen, 29, i, ':');
	}
	attroff(COLOR_PAIR(1));
	mvwaddstr(screen, 30, 12, "<HEALTH>");

	mvwaddch(screen, 31, 1, '[');
	attron(COLOR_PAIR(1));
	for (i = 2; i < 29; ++i) {
		mvwaddch(screen, 31, i, ':');
	}
	attroff(COLOR_PAIR(1));
	mvwaddch(screen, 31, 29, ']');
	mvwaddstr(screen, 32, 11, " <ARMOUR>");
}

static inline void Hud_DisplayWeapons(Game* const game)
{
	WINDOW* const screen = game->screen;
	
	nomadenum_t num = 1;
	for (nomadenum_t i = 97; num < 9; i += 4) {
		mvwaddch(screen, 7, i, '[');
		mvwaddch(screen, 7, i + 1, ((char)num + '0'));
		mvwaddch(screen, 7, i + 2, ']');
		num++;
	}
}

static inline void Hud_DisplayVMatrix(Game* const game)
{
	Hud_GetVMatrix(game);
	nomaduint_t u = 0, c = 0;
	for (nomaduint_t y = 0; y < MAX_VERT_FOV; ++y) {
		for (nomaduint_t x = 0; x < MAX_HORZ_FOV; ++x) {
			if (y == 12 && x == 44) {
				mvwaddch(game->hudwin[HL_VMATRIX],
					y, x, game->playr->sprite);
			} else {
				mvwaddch(game->hudwin[HL_VMATRIX],
					y, x, game->playr->vmatrix[u][c]);
			}
			c++;
		}
		u++;
	}
}

static inline void P_GetMapBuffer(Game* const game)
{
	std::ifstream file(mapfile, std::ios::in);
	if (file.fail()) {
		N_Error("Could Not Open File %s", mapfile);
	}
	std::string line;
	unsigned int y = 0;
	while (std::getline(file, line)) {
		for (unsigned int x = 0; x < line.size(); ++x) {
			game->c_map[y][x] = line[x];
		}
		y++;
	};
	file.close();
}

static void G_ResetMap(Game* const game);

static inline void Hud_GetVMatrix(Game* const game)
{
	Playr* const playr = game->playr;
	P_GetMapBuffer(game);
	G_ResetMap(game);
	coord_t startc;
	coord_t endc;
	startc.y = playr->pos.y - vert_fov;
	startc.x = playr->pos.x - horz_fov;
	endc.y = playr->pos.y + vert_fov;
	endc.x = playr->pos.x + horz_fov;
	memset(&playr->vmatrix, '#', sizeof(playr->vmatrix));
	
	nomadshort_t u, c;
	u = c = 0;
	Hud_InsertSprites(game);
	for (nomadshort_t y = startc.y; y < endc.y; ++y) {
		for (nomadshort_t x = startc.x; x < endc.x; ++x) {
			playr->vmatrix[u][c] = game->c_map[y][x];
			c++;
		}
		u++;
	}
}

static inline void G_SetSndPerim(Game* const game,
	nomadenum_t by, sndlvl_t num, coord_t from)
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

static inline void G_SetSmellPerim(Game* const game,
	nomadenum_t by, smelllvl_t num, coord_t from)
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

static inline void G_SetMapSnd(Game* const game)
{
	for (nomaduint_t y = 0; y < MAP_MAX_Y+160; ++y) {
		for (nomaduint_t x = 0; x < MAP_MAX_X+160; ++x) {
			game->sndmap[y][x] = SND_LOW;
		}
	}
}

static inline void G_SetMapSmell(Game* const game)
{
	for (nomaduint_t y = 0; y < MAP_MAX_Y+160; ++y) {
		for (nomaduint_t x = 0; x < MAP_MAX_X+160; ++x) {
			game->smellmap[y][x] = SMELL_LOW;
		}
	}
}

static void G_ResetMap(Game* const game)
{
	nomadenum_t i;
//	G_SetMapSnd(game);
//	G_SetMapSmell(game);
//	for (i = 0; i < game->m_Active.size(); ++i) {
//		Mob* mob = game->m_Active[i];
//		game->sndmap[mob->mpos.y][mob->mpos.x] = SND_MEDIUM;
//		game->smellmap[mob->mpos.y][mob->mpos.x] = SMELL_MEDIUM;
//		G_SetSndPerim(game, 1, SND_MEDIUM, mob->mpos);
//		G_SetSndPerim(game, 2, SND_MEDIUM, mob->mpos);
//		G_SetSndPerim(game, 3, SND_SOME, mob->mpos);
//		G_SetSndPerim(game, 4, SND_SOME, mob->mpos);
//	}
//	game->sndmap[game->playr->pos.y][game->playr->pos.x] = SND_MEDIUM;
//	game->smellmap[game->playr->pos.y][game->playr->pos.x] = SMELL_MEDIUM;
	game->c_map[game->playr->pos.y][game->playr->pos.x] = '@';
//	for (i = 1; i < 4; ++i) {
//		G_SetSndPerim(game, i, SND_MEDIUM, game->playr->pos);
//		G_SetSmellPerim(game, i, SMELL_MEDIUM, game->playr->pos);
//	}
//	for (i = 4; i < 6; ++i) {
//		G_SetSndPerim(game, i, SND_SOME, game->playr->pos);
//		G_SetSmellPerim(game, i, SMELL_SOME, game->playr->pos);
//	}
}