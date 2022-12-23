#ifndef _G_ITEMS_
#define _G_ITEMS_

#include "n_shared.h"

enum
{
	// shotgun, semi-auto pump-action
	W_SHOTTY_SFP,
	// shotgun, semi-auto auto-loading
	W_SHOTTY_SFA,
	// shotgun, full-auto beast
	W_SHOTTY_FA,
	// shotgun, 4-round burst, pump-action
	W_SHOTTY_BF4P,
	// shotgun, 3-round burst, auto-loading
	W_SHOTTY_BF3A,

	W_PISTOL_AUP,
	W_PISTOL_PLASMA,

	W_RIFLE_M23C5,
	// credits to Ben Pavlovic (milotus) for this one
	W_RIFLE_AK77,

	// desert rifle, lever-action 4 shot
	W_DMR_DR,
	W_DMR_HOSIG,

	NUMWEAPONS,
	
	W_NULL
};

enum
{
	I_HEALTH_SMALL,
	I_HEALTH_NORM,
	I_HEALTH_LARGE,
	I_BANDAGE,

	I_ARMOR_STREET,
	I_ARMOR_MILITARY,
	I_ARMOR_MERC,
	I_FLAK,

	I_SHELL,
	I_BULLET,
	I_ROCKET,

	I_COIN,
	I_SHOTGUN_SFP,
	I_SHOTGUN_SFA,

	NUMITEMS,

	I_NULL
};

enum
{
	M_METAL,
	M_STRING,
	M_GASOLINE,
	M_CLOTH,
	
	NUMMATERIALS,
	
	M_NULL
};

typedef struct
{
	nomaduint_t id;
	nomaduint_t dmg;
	nomaduint_t range;
	nomadenum_t rng;
	nomadenum_t spread;
	nomadenum_t magsize;
	nomadenum_t tbs; // time between shots (in ticks)
	// reloading time, emptied (in ticks)
	nomadenum_t tfrf;
	// reloading time, with still unfired bullets
	nomadenum_t tfrs;
} weapon_t;

constexpr uint8_t MAXMATERIALS = 15;

typedef struct
{
	const char* name;
	nomaduint_t item_id;
	nomaduint_t item_weight;
	nomaduint_t item_cost;
	std::vector<nomadenum_t> mtl_ls;
} item_t;

extern std::vector<weapon_t> wpninfo;
extern std::vector<item_t> iteminfo;

class Item
{
public: // basic item statistics
	item_t c_item;
public:
	Item();
	~Item();
};

class Weapon : public Item
{
public:
	weapon_t c_wpn;
public:
	Weapon();
	~Weapon();
};


#endif
