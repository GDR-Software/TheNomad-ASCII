#include "g_game.h"
#include <fcntl.h>

void Game::I_InitHUD(void)
{
	printf("I_InitHUD(): Initializing HUD...\n");
	playr->pos = {260, 260};
/*	nomaduint_t starty = playr->pos.y - (MAX_VERT_FOV / 2);
	nomaduint_t startx = playr->pos.x - (MAX_HORZ_FOV / 2);
	nomaduint_t endy = playr->pos.y + (MAX_VERT_FOV / 2);
	nomaduint_t endx = playr->pos.x + (MAX_HORZ_FOV / 2); */
	std::ifstream file("Files/gamedata/RUNTIME/mapfile.txt", std::ios::in);
	std::string line;
	std::vector<std::string> buffer;
	while (std::getline(file, line)) {
		buffer.push_back(line);
	};
	memset(&playr->vmatrix, '#', sizeof(playr->vmatrix));
	nomadenum_t vert_fov = MAX_VERT_FOV;
	nomadenum_t horz_fov = MAX_HORZ_FOV;
	nomadushort_t starty = playr->pos.y - (vert_fov >> 1);
	nomadushort_t startx = playr->pos.x - (horz_fov >> 1);
	nomadushort_t endy = playr->pos.y + (vert_fov >> 1);
	nomadushort_t endx = playr->pos.x + (horz_fov >> 1);
	nomadushort_t u = 0, c = 0, y = 0, x = 0;
	y = x = 0;
	for (u = starty; u < endy; u++) {
		for (c = startx; c < endx; c++) {
			playr->vmatrix[y][x] = buffer[u][c];
			x++;
		}
		y++;
	}
	for (y = 0; y < MAP_MAX_Y+160; y++) {
		for (x = 0; x < MAP_MAX_X+160; x++) {
			c_map[y][x] = buffer[y][x];
		}
	}
	file.close();
}
/*
static void G_SetSndPerim(Game *game, nomadenum_t by, nomadenum_t num, coord_t &start)
{
	game->mapbuffer[start.y - by][start.x].snd_lvl = num;
	game->mapbuffer[start.y][start.x - by].snd_lvl = num;
	game->mapbuffer[start.y + by][start.x].snd_lvl = num;
	game->mapbuffer[start.y][start.x + by].snd_lvl = num;
	game->mapbuffer[start.y + by][start.x + by].snd_lvl = num;
	game->mapbuffer[start.y + by][start.x - by].snd_lvl = num;
	game->mapbuffer[start.y - by][start.x + by].snd_lvl = num;
	game->mapbuffer[start.y - by][start.x + by].snd_lvl = num;
}

static void G_SetSmellPerim(Game *game, nomadenum_t by, nomadenum_t num, coord_t &start)
{
	game->mapbuffer[start.y - by][start.x].smell_lvl = num;
	game->mapbuffer[start.y][start.x - by].smell_lvl = num;
	game->mapbuffer[start.y + by][start.x].smell_lvl = num;
	game->mapbuffer[start.y][start.x + by].smell_lvl = num;
	game->mapbuffer[start.y + by][start.x + by].smell_lvl = num;
	game->mapbuffer[start.y + by][start.x - by].smell_lvl = num;
	game->mapbuffer[start.y - by][start.x + by].smell_lvl = num;
	game->mapbuffer[start.y - by][start.x + by].smell_lvl = num;
}
*/
void Game::G_ResetMap(void)
{
	nomaduint_t i;
	nomadenum_t a;
	G_CopyMapBuffer();
	mapbuffer[playr->pos.y][playr->pos.x].snd_lvl = SND_MEDIUM;
	mapbuffer[playr->pos.y][playr->pos.x].smell_lvl = SMELL_MEDIUM;
	mapbuffer[playr->pos.y][playr->pos.x].ptype = ET_PLAYR;
}

static std::vector<std::string> buffer;

void Game::G_DisplayHUD(void)
{
	nomadenum_t i, a;
	G_DisplayVitals();
	G_DisplayCompass();
	G_DisplayLocation();
	G_DisplayConsole();
	G_DisplayWeapons();
	G_DisplayVMatrix();
	for (a = 1; a < 7; a++) {
		for (i = 5; i < 28; i++) {
			mvwaddch(screen, i, a, '#');
		}
	}
	for (i = 7; i < 95; i++) { 
		mvwaddch(screen, 3, i, '#');
	}
}

inline void Game::G_InsertSprites(void)
{
	nomadushort_t i;
	for (i = 0; i < b_Active.size(); i++) {
		c_map[b_Active[i]->pos.y][b_Active[i]->pos.x] = b_Active[i]->c_npc.sprite;
	}
	for (i = 0; i < m_Active.size(); i++) {
		c_map[m_Active[i]->mpos.y][m_Active[i]->mpos.x] = m_Active[i]->c_mob.sprite;
	}
}

inline void Game::G_DisplayConsole(void)
{
	mvwaddch(screen, 32, 33, '>');
	mvwaddch(screen, 32, 32, '/');
}

nomaduint_t B_GetSector(coord_t pos)
{
	// in sectors 0, 7, or 6
	if (pos.y > 80 && pos.y < 199) {
		if (pos.x > 80 && pos.x < 199) {
			return 0;
		} else if (pos.x > 199 && pos.x < 319) {
			return 7;
		} else if (pos.x > 319 && pos.x < 519) {
			return 6;
		}
	}
	// in sectors 1, 8, or 5
	else if (pos.y > 199 && pos.y < 319) {
		if (pos.x > 80 && pos.x < 199) {
			return 1;
		} else if (pos.x > 199 && pos.x < 319) {
			return 8;
		} else if (pos.x > 319 && pos.x < 519) {
			return 5;
		}
	}
	// in sectors 2, 3, or 4
	else if (pos.y > 319 && pos.y < 519) {
		if (pos.x > 80 && pos.x < 199) {
			return 2;
		} else if (pos.x > 199 && pos.x < 319) {
			return 3;
		} else if (pos.x > 319 && pos.x < 519) {
			return 4;
		}
	}
	else {
		return 0; // TODO: make errors
	}
}

inline void Game::G_DisplayLocation(void)
{
	//	biome_t biome = B_GetBiome();
	//	mvwprintw(stdscr, 1, 5, "Location/Biome: %s", biome.name);
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
	// ok, so you may need to adjust this if you're using _NOMAD_16
	mvwprintw(
		screen, 2, 8, "Coords: (y) %i, (x) %i", playr->pos.y, playr->pos.x);
//	mvwprintw(screen, 1, 65, "Ticcount: %ld\n", ticcount);
}

inline void Game::G_DisplayCompass(void)
{
	nomadenum_t i;
	for (i = 0; i < 7; i++) {
		mvwaddch(screen, 4, i, '#');
	}
	for (i = 3; i > 0; i--) {
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

inline void Game::G_DisplayVitals(void)
{
	nomadenum_t i;
	// displaying the lower-left-hand corner of the vitals
	for (i = 1; i < 128; i++) {
		mvwaddch(screen, 28, i, '#');
	}
	mvwaddch(screen, 29, 30, '#');
	mvwaddch(screen, 30, 30, '#');
	mvwaddch(screen, 31, 30, '#');
	mvwaddch(screen, 32, 30, '#');
	mvwaddch(screen, 29, 1, '[');
	mvwaddch(screen, 29, 29, ']');
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	attron(COLOR_PAIR(1));
	for (i = 2; i < 29; i++) {
		mvwaddch(screen, 29, i, ':');
	}
	attroff(COLOR_PAIR(1));
	mvwaddstr(screen, 30, 12, "<HEALTH>");

	mvwaddch(screen, 31, 1, '[');
	attron(COLOR_PAIR(1));
	for (i = 2; i < 29; i++) {
		mvwaddch(screen, 31, i, ':');
	}
	attroff(COLOR_PAIR(1));
	mvwaddch(screen, 31, 29, ']');
	mvwaddstr(screen, 32, 11, " <ARMOUR>");

	// now displaying the body parts
	for (i = 28; i > 0; i--) { // drawing columns
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
	for (i = 96; i < 128; i++) {
		mvwaddch(screen, 6, i, '#');
	}
	mvwaddstr(screen, 1, 105, "HEAD");
	mvwaddstr(screen, 2, 105, "ARMS");
	mvwaddstr(screen, 3, 105, "LEGS");
	mvwaddstr(screen, 4, 105, "BODY");

	for (i = 1; i < 5; i++) {
		mvwaddch(screen, i, 111, '[');
		for (nomadenum_t a = 112; a < 123; a++) {
			mvwaddch(screen, i, a, ':');
		}
		mvwaddch(screen, i, 123, ']');
	}
}

inline void Game::G_DisplayWeapons(void)
{
	nomadenum_t num = 1;
	for (nomadenum_t i = 97; num < 9; i += 4) {
		mvwaddch(screen, 7, i, '[');
		mvwaddch(screen, 7, i + 1, ((char)num + '0'));
		mvwaddch(screen, 7, i + 2, ']');
		num++;
	}
}

inline void Game::G_DisplayVMatrix(void)
{
	std::ifstream file("Files/gamedata/RUNTIME/mapfile.txt", std::ios::in);
	std::string line;
	std::vector<std::string> buffer;
	while (std::getline(file, line)) {
		buffer.push_back(line);
	};
	nomadenum_t vert_fov = MAX_VERT_FOV;
	nomadenum_t horz_fov = MAX_HORZ_FOV;
	nomadushort_t starty = playr->pos.y - (vert_fov >> 1);
	nomadushort_t startx = playr->pos.x - (horz_fov >> 1);
	nomadushort_t endy = playr->pos.y + (vert_fov >> 1);
	nomadushort_t endx = playr->pos.x + (horz_fov >> 1);
	nomadushort_t u = 0, c = 0, y = 0, x = 0;
	for (y = 0; y < MAP_MAX_Y+160; y++) {
		for (x = 0; x < MAP_MAX_X+160; x++) {
			c_map[y][x] = buffer[y][x];
		}
	}
	G_InsertSprites();
	memset(&playr->vmatrix, '#', sizeof(playr->vmatrix));
	y = x = 0;
	for (u = starty; u < endy; u++) {
		for (c = startx; c < endx; c++) {
			playr->vmatrix[y][x] = c_map[u][c];
			x++;
		}
		y++;
	}
	u = 0, c = 0;
	for (y = 0; y < MAX_VERT_FOV; y++) {
		for (x = 0; x < MAX_HORZ_FOV; x++) {
			if (y == 12 && x == 44) {
				mvwaddch(hudwin[HL_VMATRIX], y, x, playr->sprite);
			} else {
				mvwaddch(hudwin[HL_VMATRIX], y, x, playr->vmatrix[u][c]);
			}
			c++;
		}
		u++;
	}
}

void Game::G_CopyMapBuffer(void)
{
	std::ifstream file("Files/gamedata/RUNTIME/mapfile.txt", std::ios::in);
	std::string line;
	std::vector<std::string> buf;
	while (std::getline(file, line)) {
		buf.push_back(line);
	};
//	memmove(&c_map, &buf, sizeof(buf));
}
