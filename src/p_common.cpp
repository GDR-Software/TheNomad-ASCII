#include "p_npc.h"
#include "g_mob.h"
#include "g_rng.h"
#include "g_game.h"

static coord_t E_GetDir(nomadenum_t dir)
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

nomadbool_t Game::E_Move(coord_t* epos, nomadenum_t* edir)
{
	coord_t pos = E_GetDir(*edir);
	nomadbool_t trymove = false;
	if (c_map[epos->y+pos.y][epos->x+pos.x] == '#') {
		trymove = true;
	}
	// now try and move the mob
	if (trymove) {
		*edir = P_Random() & 3; // this might end up in the same direction
		pos = E_GetDir(*edir);
		if (c_map[epos->y+pos.y][epos->x+pos.x] == '#') {
			return false;
		}
		else {
			epos->y += pos.y;
			epos->x += pos.x;
			return true;
		}
	}
	else {
		epos->y += pos.y;
		epos->x += pos.x;
		return true;
	}
}