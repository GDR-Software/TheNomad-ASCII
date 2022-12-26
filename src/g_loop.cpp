#include "g_game.h"

static Game* game;

static void levelLoop(void);

static constexpr nomaduint_t ticrate_mil  = 28;
static constexpr nomaduint_t ticrate_base = 35;

void mainLoop(int argc, char* argv[])
{
	Z_Init();
	
	// allocating 1 mb to the chunkiest class of them all
	game = (Game *)Z_Malloc(sizeof(Game), TAG_STATIC, &game);
	I_NomadInit(argc, argv, game);
	Z_CheckHeap();
	Z_FileDumpHeap();
	nomadushort_t c{};
	start_color();
	init_pair(0, COLOR_WHITE, COLOR_WHITE);
	attron(COLOR_PAIR(0));
	while (1) {
		if (game->gamestate == GS_TITLE) {
			game->ClearMainWin();
			game->DrawTitleScreen();
			c = wgetch(game->screen);
			if (c == '\n') {
				game->gamestate = GS_MENU;
			}
			else if (c == ctrl('x')) {
				game->~Game();
				break;
			}
			else {
				continue;
			}
		}
		else if (game->gamestate == GS_MENU) {
			int16_t s = 0; // this thing breaks if its a nomadshort_t, don't know why
			while (game->gamestate == GS_MENU) {
				game->ClearMainWin();
				game->DrawMenuScreen(s);
				char f = wgetch(game->screen);
				if (f != ctrl('x')) {
					if (f == 'w') {
						// s behaves in strange and mysterious ways
						s--;
						if (s < 0) {
							s = 5;
						}
					}
					else if (f == 's') {
						s++;
						if (s > 5) {
							s = 0;
						}
					}
					else if (f == '\n') {
						switch (s) {
						case 0:
							game->gamestate = GS_LEVEL;
							break;
						case 5:
							game->~Game();
							exit(1);
							break;
						default:
							break;
						};
					}
				}
				else {
					game->gamestate = GS_TITLE;
				}
				std::this_thread::sleep_for(100ms);
			};
		}
		else if (game->gamestate == GS_LEVEL) {
			levelLoop();
		}
		else if (game->gamestate == GS_PAUSE) {
			nomadshort_t s = 0;
			while (game->gamestate == GS_PAUSE) {
				s = s;
				game->ClearMainWin();
				game->DrawPauseMenu(s);
				char f = wgetch(game->screen);
				if (f != ctrl('x')) {
					if (f == 'w') {
						// s behaves in strange and mysterious ways
						s--;
						if (s < 0) {
							s = 5;
						}
					}
					else if (f == 's') {
						s++;
						if (s > 5) {
							s = 0;
						}
					}
					else if (f == '\n') {
						switch (s) {
						case 0:
							game->gamestate = GS_LEVEL;
							break;
						case 1:
							game->G_SaveGame();
							break;
						case 2: {
							bool real = game->G_LoadGame();
							if (!real) {
								mvwprintw(game->screen, getmaxy(game->screen), 0, "(ERROR) Invalid Save File");
								wrefresh(game->screen);
								wgetch(game->screen);
							}
							break; }
						case 4:
							game->G_SaveGame();
							game->~Game();
							exit(1);
							break;
						case 5:
							game->gamestate = GS_MENU;
						default:
							break;
						};
					}
					std::this_thread::sleep_for(77ms);
				}
				else {
					game->gamestate = GS_LEVEL;
				}
			};
		}
	}
}

//static void* N_Looper(void* arg);
static void* M_Looper(void *arg);

static void levelLoop(void)
{
	game->hudwin[HL_VMATRIX] = subwin(game->screen, MAX_VERT_FOV, MAX_HORZ_FOV, 4, 7);
	game->ClearMainWin();
	game->G_DisplayHUD();
	wrefresh(game->hudwin[HL_VMATRIX]);
	nomaduint_t i;
	nomadushort_t c;
	while (game->gamestate == GS_LEVEL) {
		game->ClearMainWin();
		// custom key-binds will be implemented in the future
		game->DrawMainWinBorder();
		game->G_DisplayHUD();
		game->PrintMainWin();
		pthread_mutex_lock(&game->playr_mutex);
		pthread_create(&game->mthread, NULL, M_Looper, NULL);
		if (kbhit(c)) {
			game->P_Ticker(c);
		}
		pthread_join(game->mthread, NULL);
		pthread_mutex_unlock(&game->playr_mutex);
		std::this_thread::sleep_for(std::chrono::milliseconds(ticrate_mil));
		game->ticcount++;
	};
	delwin(game->hudwin[HL_VMATRIX]);
	return;
}
/*
static void* N_Looper(void* arg)
{
	pthread_mutex_lock(&npc_mutex);
	for (nomadenum_t i = 0; i < game->b_Active.size(); i++) {
		NPC* npc = &game->b_Active[i];
		npc->nticker--;
		if (!game->E_Move(npc)) {
			continue;
		}
	}
	pthread_mutex_unlock(&npc_mutex);
	return NULL;
} */

static void* M_Looper(void *arg)
{
	pthread_mutex_lock(&game->mob_mutex);
	for (nomadenum_t i = 0; i < MAX_MOBS_ACTIVE; i++) {
		Mob* mob = game->m_Active[i];
		mob->mticker--;
		switch (mob->mstate.id) {
		/*case S_MOB_IDLE:
			mob->M_IdleThink(game);
			break; */
		case S_MOB_WANDER:
			mob->M_WanderThink(game);
			break;
		};
	}
	pthread_mutex_unlock(&game->mob_mutex);
	return NULL;
	/*
	mob->mticker--;
	switch (mob->mstate.id) {
	case S_MOB_NULL:
		mob->mstate = stateinfo[S_MOB_SPAWN];
		mob->mticker = mob->mstate.numticks;
		break;
	case S_MOB_SPAWN:
		mob->M_SpawnThink(game);
		break;
	case S_MOB_WANDER:
		mob->M_WanderThink(game);
		break;
	case S_MOB_IDLE:
		mob->M_IdleThink(game);
		break;
	case S_MOB_CHASEPLAYR:
		mob->M_ChasePlayr(game);
		break;
	case S_MOB_FIGHT:
		mob->M_FightThink(game);
		break;
	case S_MOB_FLEE:
		mob->M_FleeThink(game);
		break;
	case S_MOB_DEAD:
		mob->M_DeadThink(game);
		break;
	default:
		N_Error("Unknown/Invalid Mob State!");
		break;
	};
	*/
}