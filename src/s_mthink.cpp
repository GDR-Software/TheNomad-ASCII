//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/s_mthink.cpp
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

struct mobthinker_t
{
	state_t statenum;
	actionf_t funcptr;
};

void M_HulkCharge(Mob* const actor);
void M_HulkSniff(Mob* const actor);
void M_HulkSlap(Mob* const actor);
void M_RavBoost(Mob* const actor);
void M_RavMissiles(Mob* const actor);
void M_ShottyHitscan(Mob* const actor);
void M_PunchPlayr(Mob* const actor);
void M_GunnerRapid(Mob* const actor);
void M_GunnerBarrage(Mob* const actor);
void M_HammerDash(Mob* const actor);
void M_HammerSwing(Mob* const actor);
void M_Infight(Mob* const reactor, Mob* const inflictor);

static mobthinker_t thinkers[] = {
	{S_MOB_NULL,   M_NullThink},
	{S_MOB_SPAWN,  M_SpawnThink},
	{S_MOB_IDLE,   M_IdleThink},
	{S_MOB_WANDER, M_WanderThink},
	{S_MOB_CHASE,  M_ChasePlayr},
	{S_MOB_FIGHT,  M_FightThink},
	{S_MOB_FLEE,   M_FleeThink},
	{S_MOB_DEAD,   M_DeadThink},
};

void M_FacePlayr(Mob* const actor)
{
	if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.sight_range) {
		if (is_oppositedir(actor->mdir, game->playr->pdir))
			return;
		else if (actor->mdir == game->playr->pdir) // already facing
			return;
		else
			actor->mdir = oppositedir(game->playr->pdir);
	}
    M_KillMob(actor);
}
coord_t M_GetDir(nomadenum_t dir)
{
    switch (dir) {
    case D_NORTH:
        return coord_t(-1, 0);
        break;
    case D_WEST:
        return coord_t(0, -1);
        break;
    case D_SOUTH:
        return coord_t(1, 0);
        break;
    case D_EAST:
        return coord_t(0, 1);
        break;
    default:
        LOG_WARN("Unknown/invalid mob entity direction %hu, assigning default value of D_NORTH (not the ref)", dir);
        return {-1, 0};
        break;
    };
}
void M_ChangeState(Mob* actor, state_t newstate)
{
    actor->mstate = stateinfo[newstate];
    actor->mticker = actor->mstate.numticks;    
}
void M_ChangeState(Mob* actor)
{
	actor->mstate = stateinfo[actor->mstate.next];
	actor->mticker = actor->mstate.numticks;
}

void M_DoPlayrDamage(nomaduint_t damage)
{
	switch (game->difficulty) {
	case DIF_NOOB:
		damage /= 3; // we know what people play this difficulty, pussies
		break;
	case DIF_RECRUIT:
		damage /= 2; // half it, somewhat of a step-up
		break;
	case DIF_MERC:
		damage = damage;
		break;
	case DIF_NOMAD:
		damage *= 2; // now we get to the actually hard stuff
		break;
	case DIF_BLACKDEATH:
		damage >>= 1;
		break;
	case DIF_MINORINCONVENIENCE:
		damage >>= 3; // why play this mode?
		break;
	};
	
	// account for armor
	if (game->playr->armor == ARMOR_STREET)
		damage *= QUARTER;
	else if (game->playr->armor == ARMOR_MILITARY)
		damage *= HALF;
	else if (game->playr->armor == ARMOR_MERC)
		damage *= THREE_QUARTERS;
	else {
		LOG_WARN("game->playr->armor was an invalid enum! assigned default val of ARMOR_STREET");
		damage *= QUARTER;
	}
	game->playr->health -= damage;
}

nomadbool_t M_CheckMoral(const Mob* actor)
{
	area_t ally_perim;
	ally_perim.tl = coord_t(actor->mpos.y - MOB_MORAL_PERIM, actor->mpos.x - MOB_MORAL_PERIM);
	ally_perim.tr = coord_t(actor->mpos.y - MOB_MORAL_PERIM, actor->mpos.x + MOB_MORAL_PERIM);
	ally_perim.bl = coord_t(actor->mpos.y + MOB_MORAL_PERIM, actor->mpos.x - MOB_MORAL_PERIM);
	ally_perim.br = coord_t(actor->mpos.y + MOB_MORAL_PERIM, actor->mpos.x + MOB_MORAL_PERIM);
	nomaduint_t total = 0;
	for (const auto& i : game->m_Active) {
		if (inArea(ally_perim, i->mpos)) {
			++total;
		}
	}
	nomadbool_t health_ok;
	if (game->difficulty < DIF_NOMAD)
		health_ok = actor->health < (actor->health * HALF);
	else
		health_ok = actor->health < (actor->health * QUARTER); // less to make mobs on harder difficulties more agressive
	
	if (game->difficulty < DIF_BLACKDEATH)
		return total > 4 && health_ok;
	else
		return total > 2 && health_ok;
}
nomadbool_t M_SeePlayr(const Mob* actor)
{
	if (scf::launch::blindmobs && game->difficulty != DIF_HARDEST)
		return false;
	
    area_t see_perim;
    switch (actor->mdir) {
    case D_NORTH:
    	see_perim.tl = coord_t(actor->mpos.y - actor->c_mob.sight_range, actor->mpos.x - MOB_SIGHT_WIDTH);
    	see_perim.tr = coord_t(actor->mpos.y - actor->c_mob.sight_range, actor->mpos.x + MOB_SIGHT_WIDTH);
    	see_perim.bl = coord_t(actor->mpos.y, actor->mpos.x - MOB_SIGHT_WIDTH);
    	see_perim.br = coord_t(actor->mpos.y, actor->mpos.x + MOB_SIGHT_WIDTH);
    	break;
    case D_WEST:
    	see_perim.tl = coord_t(actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x - actor->c_mob.sight_range);
    	see_perim.tr = coord_t(actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x);
    	see_perim.bl = coord_t(actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x - actor->c_mob.sight_range);
    	see_perim.br = coord_t(actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x);
    	break;
    case D_SOUTH:
    	see_perim.tl = coord_t(actor->mpos.y, actor->mpos.x - MOB_SIGHT_WIDTH);
    	see_perim.tr = coord_t(actor->mpos.y, actor->mpos.x + MOB_SIGHT_WIDTH);
    	see_perim.bl = coord_t(actor->mpos.y + actor->c_mob.sight_range, actor->mpos.x - MOB_SIGHT_WIDTH);
    	see_perim.br = coord_t(actor->mpos.y + actor->c_mob.sight_range, actor->mpos.x + MOB_SIGHT_WIDTH);
    	break;
    case D_EAST:
    	see_perim.tl = coord_t(actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x);
    	see_perim.tr = coord_t(actor->mpos.y - MOB_SIGHT_WIDTH, actor->mpos.x + actor->c_mob.sight_range);
    	see_perim.bl = coord_t(actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x);
    	see_perim.br = coord_t(actor->mpos.y + MOB_SIGHT_WIDTH, actor->mpos.x + actor->c_mob.sight_range);
    	break;
    };
    return inArea(see_perim, game->playr->pos);
}
nomadbool_t M_CanMove(const Mob* actor)
{
    coord_t pos = M_GetDir(actor->mdir);
    switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
    case sprites[SPR_FLOOR_OUTSIDE]:
    case sprites[SPR_FLOOR_INSIDE]:
    case sprites[SPR_DOOR_OPEN]:
        return true;
        break;
    default:
        return false;
        break;
    };
    LOG_WARN("reached end of switch statement for this function");
    return false;
}
nomadbool_t M_NewMoveDir(Mob* actor)
{
    nomadenum_t numtries = (P_Random() & 8)+1;
    for (nomadenum_t i = 0; i < numtries; ++i) {
        actor->mdir = P_Random() & 3;
        coord_t pos = M_GetDir(actor->mdir);
        switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
        case sprites[SPR_FLOOR_OUTSIDE]:
    	case sprites[SPR_FLOOR_INSIDE]:
    	case sprites[SPR_DOOR_OPEN]:
            return true;
            break;
        default: break;
        };
    }
    return false;
}

nomadbool_t M_HearPlayr(const Mob* actor)
{
	if (scf::launch::deafmobs && game->difficulty != DIF_HARDEST)
		return false;
	
	area_t perim;
	perim.tl = coord_t(actor->mpos.y - MOB_HEAR_PERIM, actor->mpos.x - MOB_HEAR_PERIM);
	perim.tr = coord_t(actor->mpos.y - MOB_HEAR_PERIM, actor->mpos.x + MOB_HEAR_PERIM);
	perim.bl = coord_t(actor->mpos.y + MOB_HEAR_PERIM, actor->mpos.x - MOB_HEAR_PERIM);
	perim.br = coord_t(actor->mpos.y + MOB_HEAR_PERIM, actor->mpos.x + MOB_HEAR_PERIM);
	return inArea(perim, game->playr->pos);
}

nomadbool_t M_CheckHitscanRange(const Mob* actor)
{
	if (!actor->c_mob.hashitscan)
		return false;
	
	return disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.hitscan_range;
};
nomadbool_t M_CheckMeleeRange(const Mob* actor)
{
	if (!actor->c_mob.hasmelee)
		return false;
	
	if (disBetweenOBJ(actor->mpos, game->playr->pos) <= 10 && actor->c_mob.mtype == MT_HULK)
		return true;
	else if (disBetweenOBJ(actor->mpos, game->playr->pos) < 2 && actor->c_mob.mtype != MT_HULK)
		return true;
	else
		return false;
}
nomadbool_t M_CheckProjectileRange(const Mob* actor)
{
	if (!actor->c_mob.hasprojectile)
		return false;
	
	return disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.projectile_range;
}



void M_RunThinker(Mob* actor)
{
    --actor->mticker;
    if (actor->mticker < 0)
    	M_ChangeState(actor);
    
    for (const auto& i : thinkers) {
        if (actor->mstate.id == i.statenum) {
            (*i.funcptr)(actor);
        }
    }
}

void MobtAssigner(std::shared_ptr<Game>& gptr)
{
    game = gptr;
}

void M_DoMove(Mob* actor, nomadulong_t amount)
{
	coord_t pos = game->E_GetDir(actor->mdir);
	while (--amount && M_CanMove(actor))
		actor->mpos += pos;
}

void M_DoMove(Mob* actor)
{
	coord_t pos = game->E_GetDir(actor->mdir);
	nomaduint_t amount = scf::mobspeed;
	while (--amount && M_CanMove(actor))
		actor->mpos += pos;
}

void M_FollowPlayr(Mob* actor)
{
	if (game->playr->pos.y > actor->mpos.y)
		actor->mdir = D_SOUTH;
	else if (game->playr->pos.x > actor->mpos.x)
		actor->mdir = D_EAST;
	else if (game->playr->pos.y < actor->mpos.y)
		actor->mdir = D_NORTH;
	else if (game->playr->pos.x < actor->mpos.x)
		actor->mdir = D_WEST;
	
	M_DoMove(actor);
}

void M_PlayrKnockBack(nomadshort_t amount, nomadenum_t mdir)
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

void M_NullThink(Mob* actor)
{
    if (actor->mticker <= -1)
        M_ChangeState(actor, S_MOB_SPAWN);
    else
        return;
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
	if (actor->mticker < 0 && actor->c_mob.mtype != MT_HULK && game->difficulty != DIF_HARDEST) {
		M_ChangeState(actor, S_MOB_IDLE);
	}
	// perform extra checks for hardest difficulty
	else if (actor->mticker < 0 && actor->c_mob.mtype != MT_HULK && game->difficulty == DIF_HARDEST) {
		if (!M_SeePlayr(actor) && !M_HearPlayr(actor)) {
			M_ChangeState(actor, S_MOB_IDLE);
		}
		else if (M_HearPlayr(actor)) {
			if (M_SeePlayr(actor))
				M_ChangeState(actor, S_MOB_CHASE);
			else
				actor->mticker = actor->mstate.numticks;
		}
	}
	else if (actor->mticker < 0 && actor->c_mob.mtype == MT_HULK && game->difficulty == DIF_HARDEST) {
		if (M_SeePlayr(actor) || M_HearPlayr(actor))
			M_ChangeState(actor, S_MOB_CHASE);
		else
			actor->mticker = actor->mstate.numticks;
	}
	else if (actor->mticker < 0 && actor->c_mob.mtype == MT_HULK && game->difficulty != DIF_HARDEST) {
		if ((P_Random() & 15) > 9) // small chance for sniffing action
			M_HulkSniff(actor);
	}
	if (actor->stepcounter > -1) {
		--actor->stepcounter;
		M_DoMove(actor);
	}
	else {
		if (!M_CanMove(actor)) {
			M_NewMoveDir(actor);
		}
		actor->stepcounter = (P_Random() & 15) + 3;
	}
	nomadbool_t see = M_SeePlayr(actor);
	nomadbool_t hear = M_HearPlayr(actor);
	nomadbool_t melee = M_CheckMeleeRange(actor);
	nomadbool_t hitscan = M_CheckHitscanRange(actor);
	nomadbool_t proj = M_CheckProjectileRange(actor);
	
	if ((see || hear) && (melee || hitscan || proj)) {
		M_ChangeState(actor, S_MOB_FIGHT);
	}
	if (disBetweenOBJ(actor->mpos, game->playr->pos) > 20 && actor->mstate.id != S_MOB_FIGHT) {
		M_ChangeState(actor, S_MOB_CHASE);
	}
}

void M_IdleThink(Mob* actor)
{
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
		if (game->difficulty > DIF_NOMAD) {
			M_ChangeState(actor, S_MOB_CHASE);
		}
	}
	else {
		actor->mticker = actor->mstate.numticks;
	}
}

void M_ChasePlayr(Mob* actor)
{
	// check for flee
	if (!M_CheckMoral(actor))
		M_ChangeState(actor, S_MOB_FLEE);
	
	nomadbool_t see = M_SeePlayr(actor);
	nomadbool_t hear = M_HearPlayr(actor);
	nomadbool_t sane = actor->c_mob.mtype != MT_HULK || MT_GRUNT;
	if (!see && !hear && sane) {
		M_ChangeState(actor, S_MOB_IDLE);
	}
	else if (!see && !hear && !sane) {
		M_ChangeState(actor, S_MOB_WANDER);
	}
	else if (see || hear) {
		if (disBetweenOBJ(actor->mpos, game->playr->pos) <= 20) {
			M_ChangeState(actor, S_MOB_FIGHT);
		}
		else if (disBetweenOBJ(actor->mpos, game->playr->pos) <= 40) {
			actor->mticker = actor->mstate.numticks;
		}
		M_FollowPlayr(actor);
	}
}

void M_FightThink(Mob* actor)
{
	if (!M_CheckMoral(actor) && actor->health < (actor->c_mob.health*QUARTER)) {
		M_ChangeState(actor, S_MOB_FLEE); // cowardice
	}
	if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.melee_range) {
		if (M_SeePlayr(actor)) {
			switch (actor->c_mob.mtype) {
			case MT_GUNNER:
				M_PunchPlayr(actor);
				break;
			case MT_GRUNT:
				M_HammerSwing(actor);
				break;
			case MT_HULK:
				M_HulkSlap(actor);
				break;
			case MT_PISTOL:
				M_PunchPlayr(actor);
				break;
			};
		}
	}
	else if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.hitscan_range) {
		if (M_SeePlayr(actor)) { // quickly check for line of sight before making an attack
			switch (actor->c_mob.mtype) {
			case MT_GUNNER:
				M_GunnerRapid(actor);
				break;
			case MT_PISTOL:
				M_DoPlayrDamage(actor->c_mob.hitscan_dmg);
				break;
			case MT_SHOTTY:
				M_ShottyHitscan(actor);
				break;
			case MT_GRUNT:
				M_HammerDash(actor);
				break;
			};
		}
	}
	else if (disBetweenOBJ(actor->mpos, game->playr->pos) <= actor->c_mob.projectile_range) {
		if (M_SeePlayr(actor)) {
			switch (actor->c_mob.mtype) {
			case MT_GUNNER:
				M_GunnerBarrage(actor);
				break;
			case MT_RAVAGER:
				M_RavMissiles(actor);
				break;
			case MT_HULK:
				M_HulkCharge(actor);
				break;
			};
		}
	}
}

void M_FleeThink(Mob* actor)
{
	if (actor->c_mob.mtype == MT_HULK) {
		M_ChangeState(actor, S_MOB_FIGHT);
		return;
	}
	
	nomadshort_t scaler = 0;
	if (actor->c_mob.mtype == MT_GRUNT || MT_PISTOL)
		--scaler;
	else if (actor->c_mob.mtype == MT_GUNNER)
		scaler += 3;
	else if (actor->c_mob.mtype == MT_RAVAGER || MT_SHOTTY)
		++scaler;
	
	if (actor->health < actor->c_mob.health * QUARTER)
		--scaler;
	else if (actor->health == 1)
		scaler -= 3;
	
	// PANIC!!!
	if (scaler < 2) {
		actor->mdir = game->playr->pdir;
		M_DoMove(actor, scf::mobspeed + 2); // add a little adrenaline boost in there
	}
}

void M_DeadThink(Mob* actor)
{
    M_KillMob(actor);
}

//
// M_Infight
//
void M_Infight(Mob* const actor, Mob* const actor)
{
}

//
// M_HulkCharge
//
void M_HulkCharge(Mob* const actor)
{
	if (!actor->stepcounter)
		actor->stepcounter = ticrate_base*5; // use stepcounter as artificial timer
	else
		--actor->stepcounter;
	
	coord_t pos = game->E_GetDir(actor->mdir);
	if (actor->stepcounter < ticrate_base)
		M_ChangeState(actor, S_MOB_WANDER);
	if ((actor->stepcounter % ticrate_base) == 0) {
		M_DoMove(actor, 3);
		switch (game->c_map[actor->mpos.y.load()+pos.y.load()][actor->mpos.x.load()+pos.x.load()]) {
		case sprites[SPR_WALL]:
		case sprites[SPR_DOOR_STATIC]:
		case sprites[SPR_DOOR_CLOSE]:
		case sprites[SPR_WINDOW]:
			M_ChangeState(actor, S_MOB_WANDER);
			break;
		case sprites[SPR_PLAYR]:
			M_DoPlayrDamage(actor->c_mob.melee_dmg);
			M_PlayrKnockBack(6, actor->mdir);
			M_ChangeState(actor, S_MOB_WANDER);
			break;
		default:
			break;
		};
	}
}

//
// M_HulkSniff
//
void M_HulkSniff(Mob* actor)
{
	area_t a;
	a.tl = coord_t(actor->mpos.y - HULK_SNIFF_AREA, actor->mpos.x - HULK_SNIFF_AREA);
	a.tr = coord_t(actor->mpos.y - HULK_SNIFF_AREA, actor->mpos.x + HULK_SNIFF_AREA);
	a.bl = coord_t(actor->mpos.y + HULK_SNIFF_AREA, actor->mpos.x - HULK_SNIFF_AREA);
	a.br = coord_t(actor->mpos.y + HULK_SNIFF_AREA, actor->mpos.x + HULK_SNIFF_AREA);
	
	if (inArea(a, game->playr->pos)) // never rest
		M_ChangeState(actor, S_MOB_CHASE);
	else
		M_ChangeState(actor, S_MOB_WANDER);
}

//
// M_RavBoost
//
void M_RavBoost(Mob* actor)
{
	area_t boost_perim = {
		coord_t(actor->mpos.y - 8, actor->mpos.x - 8),
		coord_t(actor->mpos.y - 8, actor->mpos.x + 8),
		coord_t(actor->mpos.y + 8, actor->mpos.x - 8),
		coord_t(actor->mpos.y + 8, actor->mpos.x + 8)};
	for (auto* i : game->m_Active) {
		
	}
}

//
// M_ShottyHitscan
//
void M_ShottyHitscan(Mob* const actor)
{
	if (!M_SeePlayr(actor))
		return;
	
	M_FacePlayr(actor);
	nomadint_t distance = disBetweenOBJ(actor->mpos, game->playr->pos);
	nomadint_t maxdis = actor->c_mob.hitscan_range;
	area_t shotty_area;
	G_GetShottyArea(&shotty_area, actor->mdir, actor->mpos, actor->c_mob.hitscan_range,
		SHOTTY_HITSCAN_WIDTH);
	if (inArea(shotty_area, game->playr->pos)) {
		if (distance <= (maxdis / 4)) // 1/4, max damage
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg);
		else if (distance <= (maxdis / 2)) // 1/2, some damage
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg >> 1);
		else if (distance == maxdis) // least damage, max range
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg * (1/4));
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
	a.tl = coord_t(actor->mpos.y.load() - 10, actor->mpos.x.load() - 10);
	a.tr = coord_t(actor->mpos.y.load() - 10, actor->mpos.x.load() + 10);
	a.bl = coord_t(actor->mpos.y.load() + 10, actor->mpos.x.load() - 10);
	a.br = coord_t(actor->mpos.y.load() + 10, actor->mpos.x.load() + 10);
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
	a.tl = coord_t(actor->mpos.y.load() - 15, actor->mpos.x.load() - 15);
	a.tr = coord_t(actor->mpos.y.load() - 15, actor->mpos.x.load() + 15);
	a.bl = coord_t(actor->mpos.y.load() + 15, actor->mpos.x.load() - 15);
	a.br = coord_t(actor->mpos.y.load() + 15, actor->mpos.x.load() + 15);
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
// M_PunchPlayr
//
void M_PunchPlayr(Mob* const actor)
{
	M_FacePlayr(actor);
	if ((P_Random() & 25) > 20)
		M_DoPlayrDamage(actor->c_mob.melee_dmg);
}

//
// M_HammerSwing
//
void M_HammerSwing(Mob* const actor)
{
	if ((!M_SeePlayr(actor) && !M_HearPlayr(actor)) || !M_CheckMeleeRange(actor))
		return;
	
	M_FacePlayr(actor);
	if (M_SeePlayr(actor) && disBetweenOBJ(actor->mpos, game->playr->pos) < 4) {
		M_DoPlayrDamage(actor->c_mob.melee_dmg);
	}
	// ground-slam
	else if (M_HearPlayr(actor) && disBetweenOBJ(actor->mpos, game->playr->pos) < 6) {
		area_t knockback(
			coord_t(actor->mpos.y - 5, actor->mpos.x - 5),
			coord_t(actor->mpos.y - 5, actor->mpos.x + 5),
			coord_t(actor->mpos.y + 5, actor->mpos.x - 5),
			coord_t(actor->mpos.y + 5, actor->mpos.x + 5)
		);
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
	if (!M_SeePlayr(actor) || !M_CheckProjectileRange(actor))
		return;
	
	M_FacePlayr(actor);
	coord_t pos = M_GetDir(actor->mdir);
	nomadbyte_t num_missiles = 4;
	while (--num_missiles) {
		G_SpawnProjectile({ (void *)actor, ET_MOB, PROJ_ROCKET, actor->mpos, actor->c_mob.projectile_range,
			actor->mdir, sprites[SPR_ROCKET] });
	}
}

//
// M_HammerDash
//
void M_HammerDash(Mob* const actor)
{
	if (!M_SeePlayr(actor) || !M_CheckHitscanRange(actor))
		return;
	
	M_FacePlayr(actor);
	coord_t pos = M_GetDir(actor->mdir);
	nomadbool_t done = false;
	nomaduint_t charge_bonus = 0;
	for (nomadshort_t i = 0; i < actor->c_mob.hitscan_range && !done; ++i, charge_bonus += 2) {
		switch (game->c_map[actor->mpos.y.load()][actor->mpos.x.load()]) {
		case sprites[SPR_WINDOW]:
		case sprites[SPR_DOOR_STATIC]:
		case sprites[SPR_DOOR_CLOSE]:
		case sprites[SPR_WALL]:
			done = true;
			break;
		case sprites[SPR_PLAYR]:
			M_DoPlayrDamage(actor->c_mob.hitscan_dmg * charge_bonus);
			done = true;
			break;
		case sprites[SPR_FLOOR_INSIDE]:
		case sprites[SPR_FLOOR_OUTSIDE]:
			break;
		};
	}
}