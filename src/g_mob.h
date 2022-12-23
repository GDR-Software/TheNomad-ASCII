#ifndef _G_MOB_
#define _G_MOB_

#include "n_shared.h"
#include "g_playr.h"
#include "g_obj.h"

#ifdef MAX_MOBS_ACTIVE
#undef MAX_MOBS_ACTIVE
#endif

#define MAX_MOBS_ACTIVE 200

// used for locating vars in the stateinfo array
enum
{
	MT_ZHULK = 9,
	MT_ZRAVAGER = 18,
	MT_ZGRUNT = 27,
	MT_PISTOL = 36,
	MT_SHOTTY = 45,
	MT_GUNNER = 54,
	MT_MERC = 63,
	MT_NOMAD = 70
};

typedef struct mobj_s
{
	const char* name;
	sprite_t sprite;
	nomadshort_t health;
	nomadushort_t armor;
	nomaduint_t mtype;
	entitytype_t etype;
	nomadenum_t rng;
	nomadenum_t chance_to_spawn;
	nomadenum_t snd_tol, snd_area;
	nomadenum_t smell_tol, smell_area;
	nomadenum_t sight_range;
	bool hasmelee;
	bool hashitscan;
	bool hasprojectile;
	nomadushort_t melee_dmg;
	nomadushort_t hitscan_dmg, hitscan_range;
	nomadushort_t projectile_dmg, projectile_range;
	const char** mlore;
	const nomaduint_t** mdrops;
} mobj_t;

typedef struct
{
	nomadint_t health;
	nomaduint_t armor;
	nomaduint_t minion_type;
	mobj_t base_mob;
} bossj_t;

extern const mobj_t mobinfo[NUMMOBS];
extern bossj_t bossinfo;

class Mob
{
public:
	mobj_t c_mob;
	nomadbool_t is_boss;
	bossj_t c_boss;
	
	nomadenum_t sector_id;
	nomadlong_t mticker;
	nomadint_t health;
	nomadint_t armor;
	nomadenum_t mdir;
	coord_t mpos;
	entitystate_t mstate;
public:
	Mob() = default;
	~Mob();
/*
	nomadbool_t M_SmellImmediate(const std::unique_ptr<Game>& map);
	nomadbool_t M_SmellPlayr(const std::unique_ptr<Game>& map);
	nomadbool_t M_SeePlayr(const std::unique_ptr<Game>& map);
	nomadbool_t M_HearImmediate(const std::unique_ptr<Game>& map);
	nomadbool_t M_HearPlayr(const std::unique_ptr<Game>& map);

	void M_SpawnThink(std::unique_ptr<Game>& game);
	void M_IdleThink(std::unique_ptr<Game>& game);
	void M_SmellThink(std::unique_ptr<Game>& game);
	void M_HearThink(std::unique_ptr<Game>& game);
	void M_ChasePlayr(std::unique_ptr<Game>& game);
	void M_FightThink(std::unique_ptr<Game>& game);
	void M_FleeThink(std::unique_ptr<Game>& game);
	void M_WanderThink(std::unique_ptr<Game>& game);
	void M_DeadThink(std::unique_ptr<Game>& game); */
};

//void M_AngryPunch(Mob* mob, std::unique_ptr<Game>& game);

#endif
