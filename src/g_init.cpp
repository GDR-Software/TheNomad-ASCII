#include "g_game.h"

static char bffname[81];

static inline void M_Init(Game* game);
static inline void I_ProcessArgs(const std::vector<char*>& myargv);

void I_NomadInit(int argc, char* argv[], Game* game)
{
	bffname[80] = '\0';
	char buf[256];
	switch (_NOMAD_VERSION) {
	case 0:
		snprintf(buf, sizeof(buf),
			"+==============================+\n"
			"| The Nomad Pre-Alpha (v%d.%d.%d) |\n"
			"+==============================+\n",
		_NOMAD_VERSION, _NOMAD_VERSION_UPDATE, _NOMAD_VERSION_PATCH);
		break;
	default:
		printf("(ERROR: Fatal) Invalid Version!\n");
		exit(EXIT_FAILURE);
		break;
	};
	printf("%s", buf);
	std::this_thread::sleep_for(std::chrono::milliseconds(750));
	printf("I_NomadInit(): Initializing Game...\n");
	srand(time(NULL));
	std::vector<char*> myargv;
	for (nomadushort_t i = 0; i < argc; i++) {
		myargv.push_back(argv[i]);
	}
	printf("I_ProcessArgs(): Parsing Launch Parameters...\n");
	I_ProcessArgs(myargv);
	game->gamestate = GS_TITLE;
	game->gamescreen = MENU_TITLE;
	game->ticcount = 0;
	printf("I_InitEntities(): Initializing Entities...\n");
	game->I_InitEntities();
	M_Init(game);
	puts("Done.");
	printf("Starting Up Game...\n");
	printf("I_InitCurses(): Initializing NCurses/Curses Libraries...\n");
	if (!game->I_InitCurses()) {
		printf("ERROR(Fatal): Could Not Initialize Curses!\n");
		exit(-1);
	}
}

static inline void M_Init(Game* game)
{
	puts("M_Init(): Initializing Map Data...");
	char secbuffer[NUM_SECTORS][SECTOR_MAX_Y][SECTOR_MAX_X];
	nomaduint_t y, x;
	for (nomadenum_t i = 0; i < NUM_SECTORS; i++) {
		char path[180];
		y = x = 0;
		snprintf(path, sizeof(path), "Files/gamedata/MAP/mapsector_%hu.txt", i);
		std::ifstream file(path, std::ios::in);
		if (file.fail()) {
			fprintf(stderr, "(ERROR) Could Not Open Map File!\n");
			exit(-1);
		}
		std::string line;
		std::vector<std::string> buffer;
		while (std::getline(file, line)) {
			buffer.push_back(line);
		};
		for (y = 0; y < SECTOR_MAX_Y; y++) {
			for (x = 0; x < SECTOR_MAX_X; x++) {
				secbuffer[i][y][x] = buffer[y][x];
			}
		}
		file.close();
	}
	FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	/*
	076
	185
	234
	*/
	if (!fp) {
		N_Error("Could Not Create RUNTIME/mapfile.txt!");
	}
	for (y = 0; y < 80; y++) {
		for (x = 0; x < MAP_MAX_X; x++) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; y++) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[6][y][x]);
		}
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; y++) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[5][y][x]);
		}
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; y++) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; x++) {
			fprintf(fp, "%c", secbuffer[4][y][x]);
		}
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < 80; y++) {
		for (x = 0; x < MAP_MAX_X; x++) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
//	G_CopyBufferToMap();
//	I_InitBiomes();
	game->I_InitHUD();
}

inline int8_t Game::I_InitCurses(void)
{
	setlocale(LC_ALL, "");
	initscr();
	raw();
	cbreak();
	noecho();
	curs_set(0);
	screen = newwin(34, 129, 0, 0);
	keypad(screen, TRUE);
	
	if (getmaxy(screen) < 30 && getmaxx(screen) < 45) {
		endwin();
		std::printf("ERROR(Fatal): Screen Is Too Small For The Nomad (ASCII)\n");
		return 0;
	}
	// change this in the future, this game doesn't "require" colors
	if (!has_colors()) {
		refresh();
		endwin();
		return -1;
	}
	else {
		return 1;
	}
}

inline void Game::I_InitEntities(void)
{
	playr = (Playr*)Z_Malloc(sizeof(Playr), TAG_STATIC, &playr);
	playr->P_Init();
	I_InitNPCs();
	M_GenMobs();
}

static inline void I_ProcessArgs(const std::vector<char*>& myargv)
{
	for (nomaduint_t i = 1; i < myargv.size(); i++) {
		if (strstr(myargv[i], "-fastmobs") != NULL) {
			if (strchr(myargv[i], '1') != NULL) {
				scf::launch::fastmobs1 = true;
				puts("Fast Mobs 1: On");
			}
			else if (strchr(myargv[i], '2') != NULL) {
				scf::launch::fastmobs2 = true;
				puts("Fast Mobs 2: On");
			}
			else if (strchr(myargv[i], '3') != NULL) {
				scf::launch::fastmobs3 = true;
				puts("Fast Mobs 3: On");
			}
		}
		else if (strstr(myargv[i], "-bff") != NULL) {
				scf::launch::ext_bff = true;
				strcpy(bffname, myargv[i+1]);
		}
		else if (strstr(myargv[i], "-deafmobs") != NULL) {
			scf::launch::deafmobs = true;
			puts("Deaf Mobs: On");
		}
		else if (strstr(myargv[i], "-nosmell") != NULL) {
			scf::launch::nosmell = true;
			puts("No Smell: On");
		}
		else if (strstr(myargv[i], "-nomobs") != NULL) {
			scf::launch::nomobs = true;
			puts("No Mobs: On");
		}
		else if (strstr(myargv[i], "-blindmobs") != NULL) {
			scf::launch::blindmobs = true;
			puts("Blind Mobs: On");
		}
		else if (strstr(myargv[i], "-devmode") != NULL) {
			scf::launch::devmode = true;
			puts("Dev Mode: On");
		} /*
		else if (strstr(myargv[i], "-ram ") != NULL) {
			heap_size += atoi(myargv[i++]);
		} */
		/* now we get to the REAL cheat codes */
		// i want to be a god
		else if (strstr(myargv[i], "-iwtbag") != NULL) {
			scf::launch::godmode = true;
			puts("God Mode: On");
		}
		// that goddam never-ending clip
		else if (strstr(myargv[i], "-tgdnec") != NULL) {
			scf::launch::bottomless_clip = true;
			puts("Bottomless Clip: On");
		}
		// fuck yeah! infinite ammo
		else if (strstr(myargv[i], "-fyia") != NULL) {
			scf::launch::infinite_ammo = true;
			puts("Infinite Ammo: On");
		}
		else if (strstr(myargv[i], ".bff") != NULL) { // bff's in the param list
			continue;
		}
		else {
			printf("Unrecognized Launch Parameter: %s, Ignoring\n", myargv[i]);
		}
	}
	// mobs like little kids who've had a little too much sugar
	if (scf::launch::fastmobs1) {
		scf::mobspeed <<= 1;
	}
	// mobs on crack. Serious, hardcore crack
	else if (scf::launch::fastmobs2) {
		scf::mobspeed <<= 3;
	}
	// mobs on black-tar heroin
	else if (scf::launch::fastmobs3) {
		scf::mobspeed <<= 5;
	}
}