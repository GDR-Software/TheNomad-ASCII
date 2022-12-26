#include "g_playr.h"
#include "g_obj.h"
#include "g_game.h"

Playr::Playr()
{
}

void Playr::P_Init()
{
	name = "Test";
	sprite = '@';
	health = 100;
	armor = 12, 
	pdir = D_NORTH;
	lvl = 0;
	coin = 0;
	pos = {0, 0};
	sector_id = 0;
	memset(&P_wpns, -1, sizeof(P_wpns));
	memset(&body_health, 100, sizeof(body_health));
	pstate &= stateinfo[S_PLAYR_NULL].id;
}

Playr::~Playr()
{
}

static nomadbool_t P_MoveTicker(Playr* playr)
{
	if (playr->pstate == stateinfo[S_PLAYR_NULL].id) {
		playr->pstate = stateinfo[S_PLAYR_SPAWN].id;
		playr->pticker = stateinfo[S_PLAYR_SPAWN].numticks;
		return true;
	}
	else if (playr->pstate == stateinfo[S_PLAYR_MOVE].id) {
		playr->pticker = stateinfo[S_PLAYR_MOVE].numticks;
		return false;
	}
	else {
		if (!playr->pticker) {
			playr->pstate = stateinfo[S_PLAYR_MOVE].id;
			playr->pticker = stateinfo[S_PLAYR_MOVE].numticks;
			return false;
		}
		else {
			return true;
		}
	}
}

void Game::P_Ticker(nomadint_t input)
{
	playr->pticker--;
	switch (input) {
	case KEY_q: {
		if (!P_MoveTicker(playr)) {
			playr->P_ChangeDirL();
		}
		break; }
	case KEY_e: {
		if (!P_MoveTicker(playr)) {
			playr->P_ChangeDirR();
		}
		break; }
	case KEY_w: {
		if (!P_MoveTicker(playr)) {
			if (c_map[playr->pos.y - 1][playr->pos.x] == '#') {break;}
			else {playr->P_MoveN(); wrefresh(hudwin[HL_VMATRIX]);}
		}
		break; }
	case KEY_a: {
		if (!P_MoveTicker(playr)) {
			if (c_map[playr->pos.y][playr->pos.x - 1] ==  '#') {break;}
			else {playr->P_MoveW(); wrefresh(hudwin[HL_VMATRIX]);}
		}
		break; }
	case KEY_s: {
		if (!P_MoveTicker(playr)) {
			if (c_map[playr->pos.y+1][playr->pos.x] == '#') {break;}
			else {playr->P_MoveS(); wrefresh(hudwin[HL_VMATRIX]);}
		}
		break; }
	case KEY_d: {
		if (!P_MoveTicker(playr)) {
			if (c_map[playr->pos.y][playr->pos.x+1] == '#') {break;}
			else {playr->P_MoveE(); wrefresh(hudwin[HL_VMATRIX]);}
		}
		break; }
	case KEY_TILDA:
		
		break;
	case ctrl('x'):
		gamestate = GS_PAUSE;
		break;
	case ctrl('z'):
		this->~Game();
		exit(1);
		break;
	default: break;
	};
}

void Playr::P_Ticker(void)
{
	
}

void Playr::P_MoveN()
{
	pos.y--;
}

void Playr::P_MoveW()
{
	pos.x--;
}

void Playr::P_MoveS()
{
	pos.y++;
}

void Playr::P_MoveE()
{
	pos.x++;
}

void Playr::P_DashN()
{
	pos.y -= 5;
}

void Playr::P_DashW()
{
	pos.x -= 5;
}

void Playr::P_DashS()
{
	pos.y += 5;
}

void Playr::P_DashE()
{
	pos.x += 5;
}

void Playr::P_ChangeDirL()
{
	if (pdir == D_EAST) {
		pdir = D_NORTH;
	}
	else {
		pdir++;
	}
}

void Playr::P_ChangeDirR()
{
	if (pdir == D_NORTH) {
		pdir = D_EAST;
	}
	else {
		pdir--;
	}
}