#ifndef _G_MOB_
#define _G_MOB_

#include "n_shared.h"
#include "g_playr.h"
#include "g_obj.h"

#ifdef MAX_MOBS_ACTIVE
#undef MAX_MOBS_ACTIVE
#endif

#define MAX_MOBS_ACTIVE 400

// used for locating vars in the stateinfo array
enum
{
	MT_HULK,
	MT_RAVAGER,
	MT_GRUNT,
	MT_PISTOL,
	MT_SHOTTY,
	MT_GUNNER,
	MT_MERC,
	MT_NOMAD
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
extern const bossj_t bossinfo;

constexpr nomadenum_t idle       = 0b00000000;
constexpr nomadenum_t smellplayr = 0b00000001;
constexpr nomadenum_t hearplayr  = 0b00000010;
constexpr nomadenum_t seeplayr   = 0b00000011;

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
	nomadshort_t stepcounter;
public:
	Mob();
	~Mob();

	nomadbool_t M_SmellImmediate();
	nomadbool_t M_SmellPlayr();
	nomadbool_t M_SeePlayr();
	nomadbool_t M_HearImmediate();
	nomadbool_t M_HearPlayr();

	void M_SpawnThink();
	void M_IdleThink();
	void M_ChasePlayr();
	void M_FightThink();
	void M_FleeThink();
	void M_WanderThink();
	void M_DeadThink();
};

extern void M_FollowPlayr(Mob* const mob, Game* const game);
extern void M_GetLeaders(Game* const game);
extern void M_FollowLeader(Mob* const mob, Game* const game);

#endif
