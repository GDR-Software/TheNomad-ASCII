//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/s_mthink.cpp
//  mob-thinker funcs
//----------------------------------------------------------
#include "n_shared.h"
#include "scf.h"
#include "g_zone.h"
#include "g_items.h"
#include "g_obj.h"
#include "g_mob.h"
#include "p_npc.h"
#include "g_map.h"
#include "s_scripted.h"
#include "s_world.h"
#include "g_playr.h"
#include "g_game.h"
#include "g_animation.h"
#include "g_rng.h"

static Game* game;

typedef void (*actionf_t)(Mob*);

#define SFX_MOB_STATE   0
#define SFX_MOB_WAKEUP  1
#define SFX_MOB_MELEE   2
#define SFX_MOB_HITSCAN 3
#define SFX_MOB_PROJ    4
#define SFX_MOB_DIE     5

typedef struct mobsfx_s
{
	const char* name;
	state_t state;
	nomadenum_t sfx;
	nomaduint_t mobtype;
} mobsfx_t;
typedef struct mobthinker_s
{
	state_t statenum;
	actionf_t funcptr;
} mobthinker_t;

static mobsfx_t sfxinfo[] = {
//	{scf::sounds::sfx_hulk_wakeup,    S_MOB_NULL,   SFX_MOB_WAKEUP, MT_HULK},
//	{scf::sounds::sfx_hulk_wander,    S_MOB_WANDER, SFX_MOB_STATE,  MT_HULK},
//	{scf::sounds::sfx_hulk_melee,     S_MOB_NULL,   SFX_MOB_MELEE,  MT_HULK},
//	{scf::sounds::sfx_hulk_proj,      S_MOB_NULL,   SFX_MOB_PROJ,   MT_HULK},
//	{scf::sounds::sfx_hulk_chase,     S_MOB_CHASE,  SFX_MOB_STATE,  MT_HULK},
//	{scf::sounds::sfx_rav_wakeup,     S_MOB_NULL,   SFX_MOB_WAKEUP, MT_RAVAGER},
//	{scf::sounds::sfx_rav_wander,     S_MOB_WANDER, SFX_MOB_STATE,  MT_RAVAGER},
//	{scf::sounds::sfx_rav_melee,      S_MOB_NULL,   SFX_MOB_MELEE,  MT_RAVAGER},
//	{scf::sounds::sfx_rav_proj,       S_MOB_NULL,   SFX_MOB_PROJ,   MT_RAVAGER},
//	{scf::sounds::sfx_rav_chase,      S_MOB_CHASE,  SFX_MOB_STATE,  MT_RAVAGER},
//	{scf::sounds::sfx_rav_fight,      S_MOB_FIGHT,  SFX_MOB_STATE,  MT_RAVAGER},
	{scf::sounds::sfx_mshotty_wakeup, S_MOB_NULL,   SFX_MOB_WAKEUP, MT_SHOTTY},
	{scf::sounds::sfx_mshotty_fight,  S_MOB_FIGHT,  SFX_MOB_HITSCAN,MT_SHOTTY},
	{scf::sounds::sfx_mshotty_wander, S_MOB_WANDER, SFX_MOB_STATE,  MT_SHOTTY},
	{scf::sounds::sfx_mshotty_die,    S_MOB_NULL,   SFX_MOB_DIE,    MT_SHOTTY},
	{scf::sounds::sfx_mpistol_die,    S_MOB_NULL,   SFX_MOB_DIE,    MT_PISTOL},
	{scf::sounds::sfx_mpistol_shot,   S_MOB_FIGHT,  SFX_MOB_HITSCAN,MT_PISTOL},
	{scf::sounds::sfx_mpistol_punch,  S_MOB_FIGHT,  SFX_MOB_MELEE,  MT_PISTOL},
	{scf::sounds::sfx_mpistol_wakeup, S_MOB_NULL,   SFX_MOB_WAKEUP, MT_PISTOL},
	{scf::sounds::sfx_mpistol_wander, S_MOB_WANDER, SFX_MOB_STATE,  MT_PISTOL},
};

static mobthinker_t thinkers[] = {
	{S_MOB_NULL,   M_NullThink},
	{S_MOB_SPAWN,  M_SpawnThink},
	{S_MOB_IDLE,   M_IdleThink},
	{S_MOB_WANDER, M_WanderThink},
	{S_MOB_CHASE,  M_ChasePlayr},
	{S_MOB_FIGHT,  M_FightThink},
	{S_MOB_DEAD,   M_DeadThink},
};


/* misc. mob functions */
inline nomadenum_t M_GiveStepcounter(Mob* const actor)
{
	if (actor->mstate.id == S_MOB_FIGHT)
		if (game->difficulty == DIF_HARDEST)
			return P_Random() & 5;
		else
			return (P_Random() & 14)+5;
	
	return (P_Random() & 27)+5;
}
inline void M_PlaySFX(const Mob* actor, nomadenum_t sfx);
inline void M_Infight(Mob* const reactor, Mob* const inflictor);
inline void M_FacePlayr(Mob* const actor);
inline void M_DoorInteract(Mob* const actor, const coord_t& pos);
inline coord_t M_GetDir(nomadenum_t dir);
inline void M_ChangeState(Mob* const actor);
inline void M_ChangeState(Mob* const actor, state_t newstate);
inline void M_PunchPlayr(Mob* const actor);
inline void M_PlayrKnockBack(nomadshort_t amount, nomadenum_t mdir);
inline void M_DoPlayrDamage(nomaduint_t damage);
inline nomadbool_t M_SeePlayr(const Mob* actor);
inline nomadbool_t M_HearPlayr(const Mob* actor);
inline nomadbool_t M_CanMove(const Mob* actor);
inline nomadbool_t M_NewMoveDir(Mob* const actor);
inline nomadbool_t M_CheckHitscanRange(const Mob* actor);
inline nomadbool_t M_CheckMeleeRange(const Mob* actor);
inline nomadbool_t M_CheckProjectileRange(const Mob* actor);

/* mob-type specific functions */
void M_HulkCharge(Mob* const actor);
void M_HulkSniff(Mob* const actor);
void M_HulkSlap(Mob* const actor);
void M_RavBoost(Mob* const actor);
void M_RavMissiles(Mob* const actor);
void M_PistolShot(Mob* const actor);
void M_ShottyHitscan(Mob* const actor);
void M_GunnerRapid(Mob* const actor);
void M_GunnerBarrage(Mob* const actor);
void M_HammerDash(Mob* const actor);
void M_HammerSwing(Mob* const actor);

//
// M_PistolShot
//
void M_PistolShot(Mob* const actor)
{
//	P_PlaySFX(scf::sounds::sfx_mpistol_shot);
	M_FacePlayr(actor);
	nomadint_t distance = disBetweenOBJ(actor->mpos, game->playr->pos);
	if (distance <= actor->c_mob.melee_range) {
		M_PunchPlayr(actor);
		return;
	}
	coord_t pos = M_GetDir(actor->mdir);
	nomaduint_t range = actor->c_mob.hitscan_range;
	std::array<coord_t, 2> ray = G_DrawRay(actor->mpos, actor->mdir, range);
	for (nomadshort_t y = ray[0].y; y != ray[1].y; y += pos.y) {
		for (nomadshort_t x = ray[0].x; x != ray[1].x; x += pos.x) {
			if (game->playr->pos == coord_t(y, x))
				M_DoPlayrDamage(actor->c_mob.hitscan_dmg);
			
			for (linked_list<Mob*>::iterator it = game->m_Active.begin();
			it != game->m_Active.end(); it = it->next) {
				if (it->val->mpos == coord_t(y, x)) {
					M_Infight(it->val, actor);
				}
			}
		}
	}
}

//
// M_ShottyHitscan
//
void M_ShottyHitscan(Mob* const actor)
{
//	P_PlaySFX(scf::sounds::sfx_mshotty_fight);
	M_FacePlayr(actor);
	nomadint_t distance = disBetweenOBJ(actor->mpos, game->playr->pos);
	nomadint_t maxdis = actor->c_mob.hitscan_range;
	area_t shotty_area;
	G_GetShottyArea(&shotty_area, actor->mdir, actor->mpos, actor->c_mob.hitscan_range,
		SHOTTY_HITSCAN_WIDTH);
	linked_list<Mob*> hit;
	for (linked_list<Mob*>::iterator it = game->m_Active.begin();
	it != game->m_Active.end(); it = it->next) {
		if (inArea(shotty_area, it->val->mpos)) {
			hit.emplace_back();
			hit.back() = it->val;
		}
	}
	if (hit.size() > 0)
		M_Infight(hit.front(), actor);
	if (inArea(shotty_area, game->playr->pos)) {
		if (distance <= (maxdis * .75)) // 1/4, max damage
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg);
		else if (distance <= (maxdis * .5)) // 1/2, some damage
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg >> 1);
		else if (distance == maxdis) // least damage, max range
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg * .75);
	}
}

//
// M_GunnerBarrage
//
void M_GunnerBarrage(Mob* const actor)
{
    nomadbyte_t numrockets;
    area_t a;
	if (!M_SeePlayr(actor))
        goto nosee;
	
	M_FacePlayr(actor);
	a.tl = {actor->mpos.y - 10, actor->mpos.x - 10};
	a.tr = {actor->mpos.y - 10, actor->mpos.x + 10};
	a.bl = {actor->mpos.y + 10, actor->mpos.x - 10};
	a.br = {actor->mpos.y + 10, actor->mpos.x + 10};
	numrockets = (P_Random() & 6) + 1;
	while (--numrockets) {
		if (inArea(a, game->playr->pos))
			M_DoPlayrDamage(actor->c_mob.projectile_dmg + (P_Random() & 8));
	}
	if (!M_SeePlayr(actor))
		M_ChangeState(actor, S_MOB_IDLE);
	else
		M_ChangeState(actor, S_MOB_CHASE);

nosee:
	a.tl = {actor->mpos.y - 15, actor->mpos.x - 15};
	a.tr = {actor->mpos.y - 15, actor->mpos.x + 15};
	a.bl = {actor->mpos.y + 15, actor->mpos.x - 15};
	a.br = {actor->mpos.y + 15, actor->mpos.x + 15};
	numrockets = (P_Random() & 9) + 1;
	while (--numrockets) {
		// punish the player for not moving quick and smart
		if (inArea(a, game->playr->pos))
			M_DoPlayrDamage(actor->c_mob.projectile_dmg + (P_Random() & 10));
	}
	if (!M_SeePlayr(actor))
		M_ChangeState(actor, S_MOB_IDLE);
	else
		M_ChangeState(actor, S_MOB_CHASE);
}

//
// M_GunnerRapid
//
void M_GunnerRapid(Mob* const actor)
{
    nomadbyte_t numbullets;
	if (!M_SeePlayr(actor))
		goto nosee;
	
	M_FacePlayr(actor);
	numbullets = (P_Random() & 8) + 1;
	while (--numbullets) {
		if (!M_SeePlayr(actor)) {
			M_ChangeState(actor, S_MOB_IDLE);
		}
		if (P_Random() & 15 > 8 && disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.hitscan_range) {
			M_DoPlayrDamage((P_Random() & actor->c_mob.hitscan_dmg) * 6); // massive damage, but really stupid creatures
		}
	}

nosee:
	// even if the gunner can't see the player, they'll still shoot
    numbullets = (P_Random() & 12) + 1;
	while (--numbullets) {
		// if the player gets caught up in a gunner's rapid-fire rampage, they'll be punished
		if (M_SeePlayr(actor))
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg + (P_Random() & 5));
	}
	if (!M_SeePlayr(actor))
		M_ChangeState(actor, S_MOB_IDLE);
	else
		M_ChangeState(actor, S_MOB_CHASE);
}

//
// M_HammerSwing
//
void M_HammerSwing(Mob* const actor)
{
	M_FacePlayr(actor);
	if (M_SeePlayr(actor) && disBetweenOBJ(actor->mpos, game->playr->pos) < 4)
		M_DoPlayrDamage(actor->c_mob.melee_dmg);
	
	// ground-slam
	else if (M_HearPlayr(actor) && disBetweenOBJ(actor->mpos, game->playr->pos) < 6) {
		area_t knockback = {
			{actor->mpos.y - 5, actor->mpos.x - 5},
			{actor->mpos.y - 5, actor->mpos.x + 5},
			{actor->mpos.y + 5, actor->mpos.x - 5},
			{actor->mpos.y + 5, actor->mpos.x + 5}};
		if (inArea(knockback, game->playr->pos)) {
			M_DoPlayrDamage(actor->c_mob.melee_dmg);
			M_PlayrKnockBack((P_Random() & 5)+1, actor->mdir);
		}
	}
}

//
// M_RavMissiles
//
void M_RavMissiles(Mob* const actor)
{
	M_FacePlayr(actor);
	coord_t pos = M_GetDir(actor->mdir);
	nomadbyte_t num_missiles = 4;
	while (--num_missiles)
		G_SpawnProjectile(proj_t{ (void *)actor, ET_MOB, actor->mpos, PROJ_ROCKET, 3 });
}

//
// M_HammerDash
//
void M_HammerDash(Mob* const actor)
{
	M_FacePlayr(actor);
	coord_t pos = M_GetDir(actor->mdir);
	nomadbool_t done = false;
	nomaduint_t charge_bonus = 0;
	if (!(actor->mflags & MF_ATTACKING) && !actor->stepcounter) {
		actor->stepcounter = ticrate_mil*3;
		G_SpawnProjectile(proj_t{ (void *)actor, ET_MOB, actor->mpos, PROJ_CUSTOM, 1,
			PF_KILL_TICKER | PF_WITH_TICKER | PF_POS_OWNED, ticrate_mil*4 });
		actor->mflags |= MF_ATTACKING;
	}
}

//
// M_HulkSlap
//
void M_HulkSlap(Mob* const actor)
{
	area_t slap = {
		{actor->mpos.y - 6, actor->mpos.x - 2},
		{actor->mpos.y - 6, actor->mpos.x + 2},
		{actor->mpos.y, actor->mpos.x - 2},
		{actor->mpos.y, actor->mpos.x + 2}};
	
	if (inArea(slap, game->playr->pos)) {
		M_DoPlayrDamage(actor->c_mob.melee_dmg);
	}
}

//
// M_HulkCharge
//
void M_HulkCharge(Mob* const actor)
{
	if (!actor->stepcounter && !(actor->mflags & MF_ATTACKING)) {
		G_SpawnProjectile(proj_t{ (void *)actor, ET_MOB, actor->mpos, PROJ_CUSTOM, 2,
			PF_WITH_TICKER | PF_KILL_TICKER | PF_POS_OWNED, ticrate_mil, 15 });
		actor->mflags |= MF_ATTACKING;
	}
}

//
// M_HulkSniff
//
void M_HulkSniff(Mob* const actor)
{
	area_t a = {
		{actor->mpos.y - HULK_SNIFF_AREA, actor->mpos.x - HULK_SNIFF_AREA},
		{actor->mpos.y - HULK_SNIFF_AREA, actor->mpos.x + HULK_SNIFF_AREA},
		{actor->mpos.y + HULK_SNIFF_AREA, actor->mpos.x - HULK_SNIFF_AREA},
		{actor->mpos.y + HULK_SNIFF_AREA, actor->mpos.x + HULK_SNIFF_AREA}};
	
	if (inArea(a, game->playr->pos)) // never rest
		M_ChangeState(actor, S_MOB_CHASE);
	else
		M_ChangeState(actor, S_MOB_WANDER);
}

//
// M_RavBoost
//
void M_RavBoost(Mob* const actor)
{
	area_t boost_perim = {
		{actor->mpos.y - 8, actor->mpos.x - 8},
		{actor->mpos.y - 8, actor->mpos.x + 8},
		{actor->mpos.y + 8, actor->mpos.x - 8},
		{actor->mpos.y + 8, actor->mpos.x + 8}};
	for (linked_list<Mob*>::iterator it = game->m_Active.begin();
	it != game->m_Active.end(); it = it->next) {
		if (inArea(boost_perim, it->val->mpos)) {
			it->val->health = it->val->c_mob.health; // restore to full health
		}
	}
}


inline void M_PunchPlayr(Mob* const actor)
{
//	P_PlaySFX(scf::sounds::sfx_mpistol_punch);
	M_FacePlayr(actor);
	if ((P_Random() & 25) > 20)
		M_DoPlayrDamage(actor->c_mob.melee_dmg);
}

inline void M_FacePlayr(Mob* const actor)
{
	if (game->playr->pos.y > actor->mpos.y)
		actor->mdir = D_SOUTH;
	if (game->playr->pos.x > actor->mpos.x)
		actor->mdir = D_EAST;
	if (game->playr->pos.y < actor->mpos.y)
		actor->mdir = D_NORTH;
	if (game->playr->pos.x < actor->mpos.x)
		actor->mdir = D_WEST;
}

inline void M_DoorInteract(Mob* const actor, const coord_t& pos)
{
	// filter out invalid calls
	if (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x] != '>')
		return;
	
	filestream fp("Files/gamedata/RUNTIME/mapfile.txt", "w+");
	char c = fp.getc();
	nomadshort_t y = 0, x = 0;
	// drags down performance a little bit
	// do a search for the coords, and once they're
	// found, the map itself will be changed
	// TODO: add this shit in (but only when its actually needed)
	game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x] = '<';
	actor->mpos += pos;
}

inline coord_t M_GetDir(nomadenum_t dir)
{
    switch (dir) {
    case D_NORTH:
        return {-1, 0};
        break;
    case D_WEST:
        return {0, -1};
        break;
    case D_SOUTH:
        return {1, 0};
        break;
    case D_EAST:
        return {0, 1};
        break;
    default:
        LOG_WARN("Unknown/invalid mob entity direction %hu, assigning default value of D_NORTH (not the ref)", dir);
        return {-1, 0};
        break;
    };
    N_Error("M_GetDir: reached outside of switch statement");
    return {0, 0};
}
inline void M_ChangeState(Mob* const actor, state_t newstate)
{
	switch (newstate) {
	case S_MOB_WANDER:
	case S_MOB_FIGHT:
	case S_MOB_CHASE:
		actor->stepcounter = M_GiveStepcounter(actor);
		break;
	};
    actor->mstate = stateinfo[newstate];
    actor->mticker = actor->mstate.numticks;
}
inline void M_ChangeState(Mob* actor)
{
	switch (actor->mstate.next) {
	case S_MOB_WANDER:
	case S_MOB_FIGHT:
	case S_MOB_CHASE:
		actor->stepcounter = M_GiveStepcounter(actor);
		break;
	};
	actor->mstate = stateinfo[actor->mstate.next];
	actor->mticker = actor->mstate.numticks;
}

inline void M_DoPlayrDamage(nomaduint_t damage)
{
	switch (game->difficulty) {
	case DIF_NOOB: // the gamemode for pussies
		damage >>= 1;
		break;
	case DIF_RECRUIT: break;
	case DIF_MERC:
	case DIF_NOMAD: // for the brave
		damage += 15;
		break;
	case DIF_BLACKDEATH: // for those who wish to attain mastery
		damage *= 2;
		break;
	case DIF_HARDEST: // for the mentally deranged and people who for some reason wish to suffer
		damage <<= 1;
		break;
	};
//	if (damage < 20)
		P_PlaySFX(scf::sounds::sfx_playr_hurt_0);
	
	game->playr->health -= damage;
}

inline nomadbool_t M_SeePlayr(const Mob* actor)
{
	if (scf::launch::blindmobs && game->difficulty != DIF_HARDEST)
		return false;
	
    area_t see_perim;
    switch (actor->mdir) {
    case D_NORTH:
    	see_perim.tl = {actor->mpos.y - actor->c_mob.sight_range, actor->mpos.x - MOB_SIGHT_WIDTH};
    	see_perim.tr = {actor->mpos.y - actor->c_mob.sight_range, actor->mpos.x + MOB_SIGHT_WIDTH};
    	see_perim.bl = {actor->mpos.y, actor->mpos.x - MOB_SIGHT_WIDTH};
    	see_perim.br = {actor->mpos.y, actor->mpos.x + MOB_SIGHT_WIDTH};
    	break;
    case D_WEST:
    	see_perim.tl = {actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x - actor->c_mob.sight_range};
    	see_perim.tr = {actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x};
    	see_perim.bl = {actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x - actor->c_mob.sight_range};
    	see_perim.br = {actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x};
    	break;
    case D_SOUTH:
    	see_perim.tl = {actor->mpos.y, actor->mpos.x - MOB_SIGHT_WIDTH};
    	see_perim.tr = {actor->mpos.y, actor->mpos.x + MOB_SIGHT_WIDTH};
    	see_perim.bl = {actor->mpos.y + actor->c_mob.sight_range, actor->mpos.x - MOB_SIGHT_WIDTH};
    	see_perim.br = {actor->mpos.y + actor->c_mob.sight_range, actor->mpos.x + MOB_SIGHT_WIDTH};
    	break;
    case D_EAST:
    	see_perim.tl = {actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x};
    	see_perim.tr = {actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x + actor->c_mob.sight_range};
    	see_perim.bl = {actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x};
    	see_perim.br = {actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x + actor->c_mob.sight_range};
    	break;
    };
    if (inArea(see_perim, game->playr->pos)) {
    	switch (actor->c_mob.mtype) {
		case MT_SHOTTY:
//			P_PlaySFX(scf::sounds::sfx_mshotty_wakeup);
			break;
		case MT_PISTOL:
//			P_PlaySFX(scf::sounds::sfx_mpistol_wakeup);
			break;
		};
    	return true;
    }
    return false;
}
inline nomadbool_t M_CanMove(const Mob* actor)
{
    coord_t pos = M_GetDir(actor->mdir);
    switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
    case ' ':
	case '.':
	case '<':
        return true;
        break;
//    case '>':
//    	M_DoorInteract(actor, pos);
//    	break;
    case '>':
    case '@':
    case '~':
    case '#':
    case '_':
    	return false;
    	break;
    default:
    	return false;
    	break;
    };
    N_Error("M_CanMove: invalid sprite at %i:%i, %c", actor->mpos.y+pos.y,
    	actor->mpos.x+pos.x, game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]);
    return false;
}
inline nomadbool_t M_NewMoveDir(Mob* const actor)
{
    nomadenum_t numtries = (P_Random() & 8)+1;
    coord_t pos;
	for (nomadenum_t i = 0; i < numtries; ++i) {
        actor->mdir = P_Random() & 3;
        pos = M_GetDir(actor->mdir);
        switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
		case ' ':
		case '.':
		case '<':
            return true;
            break;
//       	case '>':
//     		M_DoorInteract(actor, pos);
//     		break;
        case '@':
        case '~':
        case '>':
        case '#':
        case '_':
        	return false;
        	break;
        default:
        	return false;
        	break;
        };
    }
    N_Error("M_NewMoveDir: invalid sprite at %i:%i, %c", actor->mpos.y+pos.y,
    	actor->mpos.x+pos.x, game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]);
    return false;
}

inline nomadbool_t M_HearPlayr(const Mob* actor)
{
	if (scf::launch::deafmobs && game->difficulty != DIF_HARDEST)
		return false;
	
	area_t perim;
	perim.tl = {actor->mpos.y - MOB_HEAR_PERIM, actor->mpos.x - MOB_HEAR_PERIM};
	perim.tr = {actor->mpos.y - MOB_HEAR_PERIM, actor->mpos.x + MOB_HEAR_PERIM};
	perim.bl = {actor->mpos.y + MOB_HEAR_PERIM, actor->mpos.x - MOB_HEAR_PERIM};
	perim.br = {actor->mpos.y + MOB_HEAR_PERIM, actor->mpos.x + MOB_HEAR_PERIM};
	if (inArea(perim, game->playr->pos)) {
		switch (actor->c_mob.mtype) {
		case MT_SHOTTY:
//			P_PlaySFX(scf::sounds::sfx_mshotty_wakeup);
			break;
		case MT_PISTOL:
//			P_PlaySFX(scf::sounds::sfx_mpistol_wakeup);
			break;
		};
		return true;
	}
	return false;
}

inline nomadbool_t M_CheckHitscanRange(const Mob* actor)
{
	if (!actor->c_mob.hashitscan)
		return false;
	
	if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.hitscan_range) {
		return true;
	}
	return false;
}
inline nomadbool_t M_CheckMeleeRange(const Mob* actor)
{
	if (!actor->c_mob.hasmelee)
		return false;
	
	if (disBetweenOBJ(actor->mpos, game->playr->pos) <= 10 && actor->c_mob.mtype == MT_HULK) {
		return true;
	}
	else if (disBetweenOBJ(actor->mpos, game->playr->pos) < 2 && actor->c_mob.mtype != MT_HULK) {
		return true;
	}
	return false;
}
inline nomadbool_t M_CheckProjectileRange(const Mob* actor)
{
	if (!actor->c_mob.hasprojectile)
		return false;
	
	if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.projectile_range) {
		return true;
	}
	return false;
}

inline void M_DoMove(Mob* actor)
{
	if (!M_CanMove(actor) && !M_NewMoveDir(actor))
		return;
	else {
		coord_t pos = game->E_GetDir(actor->mdir);
		actor->mpos.y += pos.y;
		actor->mpos.x += pos.x;
	}
}

inline void M_FollowPlayr(Mob* actor)
{
	if (!M_CanMove(actor))
		return;

	if (game->playr->pos.y > actor->mpos.y)
		actor->mpos.y += MOB_SPEED_STD;
	else if (game->playr->pos.y < actor->mpos.y)
		actor->mpos.y -= MOB_SPEED_STD;
	else if (game->playr->pos.x > actor->mpos.x)
		actor->mpos.x += MOB_SPEED_STD;
	else if (game->playr->pos.x < actor->mpos.x)
		actor->mpos.x -= MOB_SPEED_STD;
	
	actor->stepcounter = M_GiveStepcounter(actor);
}

inline void M_PlayrKnockBack(nomadshort_t amount, nomadenum_t mdir)
{
	nomadbool_t done = false;
	coord_t pos = game->E_GetDir(mdir);
	nomaduint_t damage = 0;
	for (nomadshort_t i = 0; i < amount && !done;
	++i, game->playr->pos += pos, damage += 3) {
		switch (game->c_map[game->playr->pos.y+pos.y][game->playr->pos.x]) {
		case sprites[SPR_FLOOR_INSIDE]:
		case sprites[SPR_FLOOR_OUTSIDE]:
			break;
		default:
			done = true;
			break;
		};
	}
	M_DoPlayrDamage(damage);
}

static boost::mutex mob_mutex;

void M_RunThinker(linked_list<Mob*>::iterator it)
{
//	boost::unique_lock<boost::mutex> lock{mob_mutex};
	Mob* const actor = it->val;
    --actor->mticker;
	if (actor->health < 1) {
		M_KillMob(it);
	}
    for (const auto& i : thinkers) {
        if (actor->mstate.id == i.statenum) {
            i.funcptr(actor);
		}
	}
}

void M_NullThink(Mob* actor)
{
	M_ChangeState(actor, S_MOB_IDLE);
//    if (actor->mticker <= -1)
//        M_ChangeState(actor, S_MOB_SPAWN);
//    else
//        return;
}

void M_SpawnThink(Mob* actor)
{
    if (actor->mticker > -1)
        return;
    else
        M_ChangeState(actor, S_MOB_IDLE);
}

void M_WanderThink(Mob* actor)
{
	if (actor->mticker <= -1) {
		if (actor->c_mob.mtype != MT_HULK && (P_Random() & 15) < 5) {
			if (game->difficulty < DIF_NOMAD) {
				M_ChangeState(actor, S_MOB_IDLE);
			}
			else if (game->difficulty > DIF_MERC) {
				actor->mticker = actor->mstate.numticks;
				actor->stepcounter = M_GiveStepcounter(actor);
			}
		}
		else {
			actor->stepcounter = M_GiveStepcounter(actor);
			actor->mticker = actor->mstate.numticks;
		}
		switch (actor->c_mob.mtype) {
		case MT_SHOTTY:
///			P_PlaySFX(scf::sounds::sfx_mshotty_wander);
			break;
		case MT_PISTOL:
	//		P_PlaySFX(scf::sounds::sfx_mpistol_wander);
			break;
		};
	}
	else {
		--actor->stepcounter;
		if (actor->stepcounter <= -1) {
			M_DoMove(actor);
			actor->stepcounter = M_GiveStepcounter(actor);
		}
		if (M_SeePlayr(actor)) {
			if (actor->c_mob.mtype == MT_HULK)
				M_ChangeState(actor, S_MOB_FIGHT);
			
			M_ChangeState(actor, S_MOB_CHASE);
		}
		else if (M_HearPlayr(actor) && !M_SeePlayr(actor)) {
			if (actor->c_mob.mtype == MT_HULK)
				M_ChangeState(actor, S_MOB_CHASE);
			
			actor->mticker = actor->mstate.numticks;
		}
	}
}

void M_IdleThink(Mob* actor)
{
	if (actor->mticker <= -1) {
		if (actor->c_mob.mtype == MT_HULK) {
			M_ChangeState(actor, S_MOB_WANDER);
			return;
		}
	}
	
	// always make these idle checks
	if (M_SeePlayr(actor)) {
		if (M_CheckMeleeRange(actor)) {
			M_ChangeState(actor, S_MOB_FIGHT);
		}
		else if (M_CheckHitscanRange(actor) || M_CheckProjectileRange(actor)
		&& game->difficulty != DIF_HARDEST) {
			M_ChangeState(actor, S_MOB_CHASE);
		}
		else {
			M_ChangeState(actor, S_MOB_FIGHT);
		}
	}
	else if (M_HearPlayr(actor)) {
		if (game->difficulty > DIF_MERC) {
			M_ChangeState(actor, S_MOB_CHASE);
		}
		else {
			M_ChangeState(actor, S_MOB_WANDER);
		}
	}
}

void M_ChasePlayr(Mob* actor)
{
	if (actor->mticker <= -1) {
		M_FacePlayr(actor);
		if (M_CheckMeleeRange(actor)
		|| M_CheckHitscanRange(actor)
		|| M_CheckProjectileRange(actor)) {
			M_ChangeState(actor, S_MOB_FIGHT);
		}
		else if (!M_SeePlayr(actor) && !M_HearPlayr(actor)) {
			if (actor->c_mob.mtype == MT_HULK) {
				M_ChangeState(actor, S_MOB_WANDER);
			}
		}
		else {
			actor->mticker = actor->mstate.numticks;
		}
	}
	else {
		--actor->stepcounter;
		if (actor->stepcounter <= -1) {
			if (M_SeePlayr(actor)) {
				M_FollowPlayr(actor);
			}
			else if (M_HearPlayr(actor)) {
				actor->stepcounter = M_GiveStepcounter(actor);
				M_DoMove(actor);
			}
		}
	}
}

#define MOB_ATK_MELEE   0
#define MOB_ATK_HITSCAN 1
#define MOB_ATK_PROJ    2

void M_FightThink(Mob* actor)
{
	if (actor->mticker <= -1) {
		//if (actor->mflags & MF_JUST_ATTACKED) {
		//	if (game->difficulty == DIF_HARDEST) {
		//		
		//	}
		//	if (actor->c_mob.mtype == MT_HULK) {
		//		
		//	}
		//}
		nomadenum_t atk = 3; // invalid atk type
		if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.melee_range)
			atk = MOB_ATK_MELEE;
		else if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.hitscan_range)
			atk = MOB_ATK_HITSCAN;
		else if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.projectile_range)
				atk = MOB_ATK_PROJ;
		else {
			M_ChangeState(actor, S_MOB_CHASE);
			return; // out of range, cancel, and set back to chase
		}
		
			// error checking
		if (atk == 3) {
			LOG_WARN("variable atk in M_FightThink was 3, invalid atk integer, aborting function.");
			return;
		}
		
		// deal with special-case mobs
		if (actor->c_mob.mtype == MT_HULK && atk == MOB_ATK_PROJ)
			actor->stepcounter = 0;
		
		if (atk == MOB_ATK_MELEE) {
			switch (actor->c_mob.mtype) {
			case MT_SHOTTY:
			case MT_PISTOL:
			case MT_GUNNER:
				M_PunchPlayr(actor);
				break;
			case MT_GRUNT:
				M_HammerSwing(actor);
				break;
			default: break;
			};
		}
		else if (atk == MOB_ATK_HITSCAN) {
			switch (actor->c_mob.mtype) {
			case MT_SHOTTY:
				M_ShottyHitscan(actor);
				break;
			case MT_GRUNT:
				M_HammerDash(actor);
				break;
			case MT_PISTOL:
				M_PistolShot(actor);
				break;
			case MT_GUNNER:
				M_GunnerRapid(actor);
				break;
			default: break;
			};
		}
		else if (atk == MOB_ATK_PROJ) {
			switch (actor->c_mob.mtype) {
			case MT_GUNNER:
				M_GunnerBarrage(actor);
				break;
			default: break;
			};
		}
		M_ChangeState(actor, S_MOB_CHASE);
	}
}

void M_DeadThink(Mob* actor)
{
    M_KillMob(actor);
}

//
// M_Infight
//
void M_Infight(Mob* const reactor, Mob* const inflictor)
{
}

inline void M_PlaySFX(const Mob* actor, nomadenum_t sfx)
{
	for (const auto &i : sfxinfo) {
		if (i.sfx == sfx) {
			if (sfx != SFX_MOB_STATE && i.mobtype == actor->c_mob.mtype)
				P_PlaySFX(i.name);
			else if ((sfx == SFX_MOB_STATE) && i.state == actor->mstate.id
			&& i.mobtype == actor->c_mob.mtype)
				P_PlaySFX(i.name);
		}
	}
}

void MobtAssigner(Game* const gptr)
{ game = gptr; }