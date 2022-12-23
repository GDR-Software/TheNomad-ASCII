#include "g_game.h"

collider_t Game::P_RangedCollider(nomadushort_t range, nomaduint_t wpn)
{
	
}

collider_t Game::P_RangedCollider(nomadushort_t range)
{
	
}

collider_t Game::P_CloseCollider(nomadenum_t dir)
{
	nomadshort_t y{};
	nomadshort_t x{};
	collider_t hit;
	switch (dir) {
	case D_NORTH:
		y = -1; x = 0;
		break;
	case D_WEST:
		y = 0; x = -1;
		break;
	case D_SOUTH:
		y = 1; x = 0;
		break;
	case D_EAST:
		y = 0; x = 1;
		break;
	};
	if (c_map[playr->pos.y+y][playr->pos.x+x] == '#') {
		hit.where = {playr->pos.y+y, playr->pos.x};
		hit.what = ET_WALL;
	}
	else {
		hit.where = {playr->pos.y, playr->pos.x};
		hit.what = ET_AIR;
	}
	return hit;
}