#include "g_game.h"
#include <termios.h>

static int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

#define NB_ENABLE 1
#define NB_DISABLE 2

static void nonblock(int state)
{
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state == NB_ENABLE) {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state == NB_DISABLE) {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

}

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
	nonblock(NB_ENABLE);
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
				attroff(COLOR_PAIR(0));
				delwin(game->screen);
				endwin();
				system("rm -rf Files/gamedata/RUNTIME/*");
				exit(1);
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
							attroff(COLOR_PAIR(0));
							delwin(game->screen);
							endwin();
							system("rm -rf Files/gamedata/RUNTIME/*");
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
							attroff(COLOR_PAIR(0));
							delwin(game->screen);
							endwin();
							system("rm -rf Files/gamedata/RUNTIME/*");
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
static void M_Looper(Mob* mob);

static void levelLoop(void)
{
	game->hudwin[HL_VMATRIX] = subwin(game->screen, MAX_VERT_FOV, MAX_HORZ_FOV, 4, 7);
	game->ClearMainWin();
	game->G_DisplayHUD();
	wrefresh(game->hudwin[HL_VMATRIX]);
	nomaduint_t i;
	while (game->gamestate == GS_LEVEL) {
		game->ClearMainWin();
		// custom key-binds will be implemented in the future
		game->DrawMainWinBorder();
		game->G_DisplayHUD();
		game->PrintMainWin();
		if (kbhit()) {
			game->P_Ticker(getc(stdin));
		}
		for (i = 0; i < sizeof(game->m_Active); i++) {
//			M_Looper(&game->m_Active[i]);
		}
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
/*
static void M_Looper(Mob* mob)
{
	mob->mticker--;
	switch (mob->mstate.id - mob->c_mob.mtype) {
	case S_MOB_NULL:
		mob->mstate = stateinfo[S_MOB_SPAWN+mob->c_mob.mtype];
		mob->mticker = mstate.numticks;
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
	case S_MOB_SMELLPLAYR:
		mob->M_SmellThink(game);
		break;
	case S_MOB_HEARPLAYR:
		mob->M_HearThink(game);
		break;
	case S_MOB_CHASEPLAYR:
		mob->M_ChaseThink(game);
		break;
	case S_MOB_FIGHT:
		mob->M_FightThink(game);
		break;
	case S_MOB_FLEE:
		game->M_FleeThink(game);
		break;
	case S_MOB_DEAD:
		game->M_DeadThink(game);
		break;
	default:
		N_Error("Unknown/Invalid Mob State!");
		break;
	};
}
*/