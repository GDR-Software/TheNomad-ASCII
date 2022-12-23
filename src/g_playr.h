#ifndef _G_PLAYR_
#define _G_PLAYR_

#include "n_shared.h"
#include "scf.h"
#include "g_map.h"

#define MAX_PLAYR_WPNS 11

class Playr {
public:
	std::string name;
	sprite_t sprite;
	nomadint_t health;
	nomadushort_t armor;
	nomadenum_t pdir;
	nomadushort_t lvl;
	nomadint_t coin;
	coord_t pos;
	nomadenum_t sector_id;
	nomaduint_t P_wpns[MAX_PLAYR_WPNS];
	nomaduint_t** inv;
	nomadint_t body_health[4];
	nomaduint_t pstate;
	nomadulong_t pticker;
	char vmatrix[MAX_VERT_FOV*2][MAX_HORZ_FOV*2];
//	pint_t vperim[MAX_VERT_FOV+20*2][MAX_HORZ_FOV+20*2];
	// anything within this perimeter is an active entity, anything not is a static entity
public:
	Playr();
	~Playr();
	void P_Init();
	// TODO: add in collision checks
	void P_MoveN();
	void P_MoveW();
	void P_MoveS();
	void P_MoveE();

	void P_DashN();
	void P_DashW();
	void P_DashS();
	void P_DashE();

	void P_ChangeDirL();
	void P_ChangeDirR();

	void P_Ticker(void);
	/*
		void ShootShotty(const Map& map);
		void ShootSingle(const Map& map);
		void ShootBF3(const Map& map);
		void ShootBF4(const Map& map); */
};

#endif