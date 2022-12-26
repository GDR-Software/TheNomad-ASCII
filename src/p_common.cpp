#include "p_npc.h"
#include "g_mob.h"
#include "g_rng.h"
#include "g_game.h"

void Game::E_MoveImmediate(coord_t* epos, nomadenum_t edir)
{
	coord_t pos = E_GetDir(edir);
	epos->y += pos.y;
	epos->x += pos.x;
}

coord_t Game::E_GetDir(nomadenum_t dir)
{
	coord_t coords;
	coords.x = coords.y = 0;
	switch (dir) {
	case D_NORTH:
		coords.y = -1;
		break;
	case D_WEST:
		coords.x = -1;
		break;
	case D_SOUTH:
		coords.y = 1;
		break;
	case D_EAST:
		coords.x = 1;
		break;
	default:
		N_Error("Unknown/Invalid Direction For Entity!");
		break;
	};
	return coords;
}

inline coord_t Game::E_TryMove(coord_t* epos, nomadenum_t* edir)
{
	coord_t pos = E_GetDir(*edir);
	if (c_map[epos->y+pos.y][epos->x+pos.x] != '#') {
		return pos;
	}
	else {
		*edir = P_Random() & 3; // might be the same direction rolled
		pos = E_GetDir(*edir);
		if (c_map[epos->y+pos.y][epos->x+pos.x] != '#') {
			return pos;
		}
		else {
			return {0, 0};
		}
	}
}

nomadbool_t Game::E_Move(coord_t* epos, nomadenum_t* edir)
{
	coord_t pos = E_TryMove(epos, edir);
	if (pos.x == 0 && pos.y == 0) {
		return false;
	}
	else {
		epos->y += pos.y;
		epos->x += pos.x;
		return true;
	}
}