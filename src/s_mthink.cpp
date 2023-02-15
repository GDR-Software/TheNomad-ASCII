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
#include "g_rng.h"
#include "g_game.h"
#include "g_animation.h"
#include "g_mob.h"

static Game* game;

typedef void (*actionf_t)(Mob* const actor);

struct mob_thinker
{
    nomaduint_t mtype;
    actionf_t funcptr;
};

void M_GruntThink(Mob* const actor);
void M_GunnerThink(Mob* const actor);
void M_HulkThink(Mob* const actor);
void M_RavThink(Mob* const actor);
void M_MercThink(Mob* const actor);
void M_PistolThink(Mob* const actor);
void M_ShottyThink(Mob* const actor);

// quick little lambdas
auto M_GetDir(nomadenum_t dir) -> coord_t;
// override the next state
auto M_ChangeState(Mob* const actor, state_t newstate) -> void;
// change current mstate to next state within the stateinfo array
auto M_ChangeState(Mob* const actor) -> void;
auto M_SeePlayr(Mob* const actor) -> nomadbool_t;
auto M_CheckMissleRange(Mob* const actor) -> nomadbool_t;
auto M_CheckMeleeRange(Mob* const actor) -> nomadbool_t;
auto M_CanMove(Mob* const actor) -> nomadbool_t;
auto M_NewMoveDir(Mob* const actor) -> nomadbool_t;
auto M_HearPlayr(Mob* const actor) -> nomadbool_t;

static mob_thinker thinkers[] = {
    {MT_GRUNT,   M_GruntThink},
    {MT_GUNNER,  M_GunnerThink},
    {MT_HULK,    M_HulkThink},
    {MT_RAVAGER, M_RavThink},
    {MT_MERC,    M_MercThink},
    {MT_PISTOL,  M_PistolThink},
    {MT_SHOTTY,  M_ShottyThink},
};

// triggers if target is within mob's missile range && mob's state is set to S_MOB_FIGHT
// for mobs with more than one missile attack, rng is used to determine which attack is used
static mob_thinker missile_thinkers[] = {
};

// same as the missile_thinkers, but for melee-only attacks
static mob_thinker melee_thinkers[] = {
};

struct mob_thinker2
{
    state_t statenum;
    actionf_t funcptr;
};
static mob_thinker2 thinker_funcs[] = {
    {S_MOB_NULL,   M_NullThink},
    {S_MOB_SPAWN,  M_SpawnThink},
    {S_MOB_IDLE,   M_IdleThink},
    {S_MOB_WANDER, M_WanderThink},
    {S_MOB_CHASE,  M_ChasePlayr},
    {S_MOB_FIGHT,  M_FightThink},
    {S_MOB_FLEE,   M_FleeThink},
    {S_MOB_DEAD,   M_DeadThink},
};

void M_RunThinker(Mob* const actor)
{
    --actor->mticker;
//    if (actor->mticker <= -1) {
//        M_ChangeState(actor, actor->mstate.next);
//    }
    for (const auto& i : thinker_funcs) {
        if (actor->mstate.id == i.statenum) {
            (*i.funcptr)(actor);
        }
    }
}

void MobtAssigner(Game* const gptr)
{
    game = gptr;
}

void M_DoMove(Mob* const actor)
{
    if (!M_CanMove(actor)) {
        if (!M_NewMoveDir(actor)) {
            return;
        }
    }
    coord_t pos = M_GetDir(actor->mdir);
    actor->mpos.y += (pos.y * scf::mobspeed);
    actor->mpos.x += (pos.x * scf::mobspeed);
}


void M_HulkCharge(Mob* actor)
{
    coord_t pos = M_GetDir(actor->mdir);
    nomadbool_t hitwall = false;
    for (nomadenum_t i = 0; i < 4; ++i) {
        if (hitwall) break;
        switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
        case ' ':
        case '.':
            actor->mpos.y += (pos.y * MOB_SPEED_STD);
            actor->mpos.x += (pos.x * MOB_SPEED_STD);
            break;
        default:
            hitwall = true;
            break;
        };
    }
}
void M_HulkSlap(Mob* actor);
void M_HulkSniff(Mob* actor);
void M_RavBoost(Mob* actor);

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
    M_DoMove(actor);
}

void M_IdleThink(Mob* actor)
{
	if (actor->mobflags & EF_JUST_HIT) {
		M_ChangeState(actor, S_MOB_CHASE);
		return;
	}
	if (actor->mticker > -1)
		return;
	if (M_HearPlayr(actor)) {
		if (M_SeePlayr(actor)) {
			M_ChangeState(actor, S_MOB_CHASE);
		}
		else {
			M_ChangeState(actor, S_MOB_WANDER);
		}
	}
}

void M_FollowPlayr(Mob* actor)
{
	if (game->playr->pos.y > actor->mpos.y)
		actor->mpos.y += scf::mobspeed;
	else if (game->playr->pos.y < actor->mpos.y)
		actor->mpos.y -= scf::mobspeed;
	if (game->playr->pos.x > actor->mpos.x)
		actor->mpos.x += scf::mobspeed;
	else if (game->playr->pos.x < actor->mpos.x)
		actor->mpos.x -= scf::mobspeed;
}

void M_ChasePlayr(Mob* actor)
{
	if (actor->mticker <= -1) {
		if (!M_SeePlayr(actor) || !M_HearPlayr(actor)) {
			M_ChangeState(actor, S_MOB_IDLE);
			return;
		}
		if (M_SeePlayr(actor)) {
			actor->mticker = actor->mstate.numticks;
		}
		if (M_HearPlayr(actor)) {
			M_ChangeState(actor, S_MOB_WANDER);
		}
	}
	else {
		if (disBetweenOBJ(actor->mpos, game->playr->pos) > actor->c_mob.sight_range) {
			// attempt to close the gap
			M_FollowPlayr(actor);
		}
	}
}

void M_FightThink(Mob* actor)
{
}

void M_FleeThink(Mob* actor)
{
}

void M_DeadThink(Mob* actor)
{
    M_KillMob(actor);
}

void M_GruntThink(Mob* const actor)
{
    LOG_PROFILE();
    switch (actor->mstate.id) {
    case S_MOB_IDLE: {
        if (M_SeePlayr(actor)) {
            M_ChangeState(actor, S_MOB_CHASE);
        }
        break; }
    case S_MOB_CHASE: {
        if (!M_SeePlayr(actor)) {
            M_ChangeState(actor, S_MOB_WANDER);
        }
        else {
            M_FightThink(actor);
        }
        break; }
    };
}
void M_GunnerThink(Mob* const actor)
{
}
void M_HulkThink(Mob* const actor)
{
    LOG_PROFILE();
    if (M_SeePlayr(actor)) {
        M_ChangeState(actor, S_MOB_CHASE);
    }
    switch (actor->mstate.id) {
    case S_MOB_IDLE:
        // hulks never rest
        M_ChangeState(actor, S_MOB_WANDER);
        break;
    case S_MOB_CHASE:
        M_ChasePlayr(actor);
        break;
    case S_MOB_WANDER:
        M_WanderThink(actor);
        break;
    case S_MOB_FIGHT:
        M_FightThink(actor);
        break;
    };
}
void M_RavThink(Mob* const actor)
{
}
void M_MercThink(Mob* const actor)
{
}
void M_PistolThink(Mob* const actor)
{
}
void M_ShottyThink(Mob* const actor)
{
}

// quick little lambdas
auto M_GetDir(nomadenum_t dir) -> coord_t
{
    coord_t pos;
    switch (dir) {
    case D_NORTH:
        pos.y = -1;
        pos.x = 0;
        break;
    case D_WEST:
        pos.y = 0;
        pos.x = -1;
        break;
    case D_SOUTH:
        pos.y = 1;
        pos.x = 0;
        break;
    case D_EAST:
        pos.y = 0;
        pos.x = 1;
        break;
    default:
        LOG_WARN("Unknown/invalid mob entity direction %hu, assigning default value of D_NORTH", dir);
        pos.y = -1;
        pos.x = 0;
        break;
    };
    return pos;
};
auto M_ChangeState(Mob* const actor, state_t newstate) -> void
{
    actor->mstate = stateinfo[newstate];
    actor->mticker = actor->mstate.numticks;    
};
auto M_ChangeState(Mob* const actor) -> void
{
    actor->mstate = stateinfo[actor->mstate.next];
    actor->mticker = actor->mstate.numticks;
};
auto M_SeePlayr(Mob* const actor) -> nomadbool_t
{
    area_t see_perim;
    switch (actor->mdir) {
    case D_NORTH:
    	see_perim.tl.y = actor->mpos.y - actor->c_mob.sight_range;
    	see_perim.tl.x = actor->mpos.x - 1;
    	see_perim.tr.y = actor->mpos.y - actor->c_mob.sight_range;
    	see_perim.tr.x = actor->mpos.x + 1;
    	see_perim.bl.y = actor->mpos.y;
    	see_perim.bl.x = actor->mpos.x - 1;
    	see_perim.br.y = actor->mpos.y;
    	see_perim.br.x = actor->mpos.x + 1;
    	break;
    case D_WEST:
    	see_perim.tl.y = actor->mpos.y - 1;
    	see_perim.tl.x = actor->mpos.x - actor->c_mob.sight_range;
    	see_perim.tr.y = actor->mpos.y - 1;
    	see_perim.tr.x = actor->mpos.x;
    	see_perim.bl.y = actor->mpos.y + 1;
    	see_perim.bl.x = actor->mpos.x - actor->c_mob.sight_range;
    	see_perim.br.y = actor->mpos.y + 1;
    	see_perim.br.x = actor->mpos.x;
    	break;
    case D_SOUTH:
    	see_perim.tl.y = actor->mpos.y;
    	see_perim.tl.x = actor->mpos.x - 1;
    	see_perim.tr.y = actor->mpos.y;
    	see_perim.tr.x = actor->mpos.x + 1;
    	see_perim.bl.y = actor->mpos.y + actor->c_mob.sight_range;
    	see_perim.bl.x = actor->mpos.x - 1;
    	see_perim.br.y = actor->mpos.y + actor->c_mob.sight_range;
    	see_perim.br.x = actor->mpos.x + 1;
    	break;
    case D_EAST:
    	see_perim.tl.y = actor->mpos.y - 1;
    	see_perim.tl.x = actor->mpos.x;
    	see_perim.tr.y = actor->mpos.y - 1;
    	see_perim.tr.x = actor->mpos.x + actor->c_mob.sight_range;
    	see_perim.bl.y = actor->mpos.y + 1;
    	see_perim.bl.x = actor->mpos.x;
    	see_perim.br.y = actor->mpos.y + 1;
    	see_perim.br.x = actor->mpos.x + actor->c_mob.sight_range;
    	break;
    };
    if ((game->playr->pos.y >= see_perim.tl.y && game->playr->pos.y <= see_perim.br.y)
    && (game->playr->pos.x >= see_perim.tl.x && game->playr->pos.x <= see_perim.br.x)) {
    	return true;
    }
    return false;
};
auto M_CanMove(Mob* const actor) -> nomadbool_t
{
    coord_t pos = M_GetDir(actor->mdir);
    switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
    case ' ':
    case '.':
        return true;
        break;
    default:
        return false;
        break;
    };
    LOG_WARN("reached end of switch statement for this function");
    return false;
};
auto M_NewMoveDir(Mob* const actor) -> nomadbool_t
{
    nomadenum_t numtries = (P_Random() & 8)+1;
    for (nomadenum_t i = 0; i < numtries; ++i) {
        actor->mdir = P_Random() & 3;
        coord_t pos = M_GetDir(actor->mdir);
        switch (game->c_map[actor->mpos.y+pos.y][actor->mpos.x+pos.x]) {
        case ' ':
        case '.':
            return true;
            break;
        default: break;
        };
    }
    return false;
};
auto M_HearPlayr(Mob* const actor) -> nomadbool_t
{
	area_t perim;
	perim.tl.y = actor->mpos.y - 5;
	perim.tl.x = actor->mpos.x - 5;
	perim.tr.y = actor->mpos.y - 5;
	perim.tr.x = actor->mpos.x + 5;
	perim.bl.y = actor->mpos.y + 5;
	perim.bl.x = actor->mpos.x - 5;
	perim.br.y = actor->mpos.y + 5;
	perim.br.x = actor->mpos.x + 5;
	if ((game->playr->pos.y >= perim.tl.y && game->playr->pos.y <= perim.br.y)
	&& (game->playr->pos.x >= perim.tl.x && game->playr->pos.x <= perim.br.x)) {
		return true;
	}
	return false;
};