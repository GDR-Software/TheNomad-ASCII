#include "g_game.h"

void Game::DrawSpriteToMainWin(coord_t pos, sprite_t sprite)
{
	mvwaddch(screen, pos.y, pos.x, sprite);
}

void Game::PrintMainWin(void)
{
	wrefresh(screen);
}

void Game::ClearMainWin(void)
{
	werase(screen);
}

void Game::MainWinPrintf(const char *msg)
{
	wrefresh(screen);
}

void Game::DrawMainWinBorder(void)
{
	nomadenum_t i;
	for (i = 0; i < getmaxx(screen); i++) {
		mvwaddch(screen, 0, i, '#');
		mvwaddch(screen, (getmaxy(screen) - 1), i, '#');
	}
	for (i = 0; i < getmaxy(screen); i++) {
		mvwaddch(screen, i, 0, '#');
		mvwaddch(screen, i, (getmaxx(screen) - 1), '#');
	}
}

static std::string buffer;
/*
void Game::DrawSettingsScreen()
{
	
}

void Game::DrawLevelScreen()
{
	
}*/

void Game::DrawTitleScreen()
{
	std::ifstream file("Files/gamedata/GS/titlescreen.txt", std::ios::in);
	while (std::getline(file, buffer)) {
		wprintw(screen, "%s\n", buffer.c_str());
	}
}

void Game::DrawMenuScreen(nomadshort_t s)
{
	std::ifstream file("Files/gamedata/GS/menumain.txt", std::ios::in);
	nomadenum_t i = 0;
	while (std::getline(file, buffer)) {
		mvwprintw(screen, i, 0, "%s\n", buffer.c_str());
		if (i == (s+6)) {
			mvwaddch(screen, i, 2, ' ');
			mvwaddch(screen, i, 3, '[');
			for (nomadenum_t a = 3; a < buffer.length(); a++) {
				if (buffer[a] == ' '
				&& buffer[a+1] == ' ') {
					mvwaddch(screen, i, a, ']');
					break;
				}
			}
		}
		i++;
	}
}

void Game::DrawPauseMenu(nomadshort_t s)
{
	std::ifstream file("Files/gamedata/GS/pausemenu.txt", std::ios::in);
	nomadenum_t i = 0;
	while (std::getline(file, buffer)) {
		mvwprintw(screen, i, 0, "%s\n", buffer.c_str());
		if (i == (s+1)) {
			mvwaddch(screen, i, 2, ' ');
			mvwaddch(screen, i, 3, '[');
			for (nomadenum_t a = 3; a < buffer.length(); a++) {
				if (buffer[a] == ' '
				&& buffer[a+1] == ' ') {
					mvwaddch(screen, i, a, ']');
					break;
				}
			}
		}
		i++;
	}
}