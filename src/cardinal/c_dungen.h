#ifndef _C_DUNGEON_
#define _C_DUNGEON_

#include "../g_game.h"

namespace cardinal {

struct droom
{
	dim_t rdim;
	coord_t rcoords[4];
	char description[256];
};

struct ddoor
{
	nomaduint_t health;
	char description[256];
};

struct dhall
{
	coord_t hcoords;
	dim_t hdim;
	char description[256];
};

class Dungeon
{
public:
	Game* game;
	std::vector<struct droom*> rooms;
	std::vector<struct ddoor*> doors;
	std::vector<struct dhall*> halls;
	std::vector<std::vector<nomadushort_t>> tile_map;
public:
	Dungeon(Game* const game);
	~Dungeon();

	struct droom* D_GenRoom();
	struct ddoor* D_GenDoor();
	struct dhall* D_GenHall();
};

};

#endif