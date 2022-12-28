#include "g_game.h"
#include "g_rng.h"

void M_FollowPlayr(Mob* const mob, Game* const game);

static nomadenum_t M_GetChaseDir(Mob* const mob, Game* const game);
void M_SearchForPlayr(Mob* const mob, Game* const game);

void M_GetLeaders(Game* const game);
static void M_FaceLeader(Mob* const mob, Game* const game);
void M_FollowLeader(Mob* const mob, Game* const game);

void M_KillMob(Mob* const mob);
void M_MakeTarget(Mob* const mob);
void M_AttackPlayr(Mob* const mob);
void M_AttackNPC(Mob* const mob);
void M_AttackMob(Mob* const revenge, Mob* const inflictor);

void M_FollowPlayr(Mob* const mob, Game* const game)
{
	coord_t pos = game->E_GetDir(mob->mdir);
	mob->mpos.y += pos.y;
	mob->mpos.x += pos.x;
}

static std::vector<Mob*> leaders;

void M_GetLeaders(Game* const game)
{
	leaders.clear();
	std::copy_if(game->m_Active.begin(), game->m_Active.end(), std::back_inserter(leaders),
               [](Mob* mob) { return ((mob->c_mob.mtype == MT_RAVAGER)
				|| (mob->c_mob.mtype == MT_NOMAD)
				|| (mob->c_mob.mtype == MT_MERC)
				|| (mob->c_mob.mtype == MT_SHOTTY)); });
}

static void M_FaceLeader(Mob* const mob, Game* const game)
{
	coord_t endc;
	coord_t startc;
	endc.y = mob->mpos.y + (mob->c_mob.sight_range >> 1);
	endc.x = mob->mpos.x + (mob->c_mob.sight_range >> 1);
	startc.y = mob->mpos.y - (mob->c_mob.sight_range >> 1);
	startc.x = mob->mpos.x - (mob->c_mob.sight_range >> 1);
	for (nomadshort_t y = startc.y; y < endc.y; ++y) {
		for (nomadshort_t x = startc.x; x < endc.x; ++x) {
			if (game->c_map[y][x] == mobinfo[MT_RAVAGER].sprite
			|| mobinfo[MT_NOMAD].sprite || mobinfo[MT_MERC].sprite
			|| mobinfo[MT_SHOTTY].sprite) {
				mob->mdir = game->E_GetFacing({y, x}, mob->mpos);
				break;
			}
		}
	}
}

void M_FollowLeader(Mob* const mob, Game* const game)
{
	// ignore this function if the mob type is a leader
	if ((mob->c_mob.mtype == MT_RAVAGER)
		|| (mob->c_mob.mtype == MT_NOMAD)
		|| (mob->c_mob.mtype == MT_MERC)
		|| (mob->c_mob.mtype == MT_SHOTTY)) {
		return;
	}
	// perform a search for the closest leader in the area
	std::vector<coord_t> coords;
	coords.reserve(leaders.size());
	for (const auto* coord : leaders)
		coords.emplace_back(coord->mpos);

	coord_t commander_pos = closestOBJ(coords, mob->mpos);
	if (disBetweenOBJ(mob->mpos, commander_pos) > 8) {
		M_FaceLeader(mob, game);
		coord_t pos = game->E_GetDir(mob->mdir);
		mob->mpos.y += pos.y;
		mob->mpos.x += pos.x;
	}
}

static nomadenum_t M_GetChaseDir(Mob* const mob, Game* const game)
{
	coord_t pos = game->E_GetDir(mob->mdir);

	// the mob sees the player, so move them directly towards the player
	if (mob->M_SeePlayr()) {
		coord_t& mpos = mob->mpos;
		char move = game->c_map[mpos.y+pos.y][mpos.x+pos.x];
		switch (move) {
		case '.':
		case ' ':
			return 5;
			break;
		default:
			return (P_Random() & 3);
			break;
		};
	}
	else {
		return (P_Random() & 3);
	}
}

void M_SearchForPlayr(Mob* const mob, Game* const game)
{
	nomadenum_t pursuit = idle;
	if (mob->M_SmellPlayr()) {
		pursuit++;
	}
	if (mob->M_HearPlayr()) {
		pursuit++;
	}
	if (mob->M_SeePlayr()) {
		pursuit++;
	}
	if (pursuit < 1) {
		return;
	}
	nomadenum_t chasedir = M_GetChaseDir(mob, game);
	nomadbool_t changedir = false;
	if (chasedir == 5) {
		// if the mob doesn't need to change direction, then best 
		// thing to do is to go in the direction of player's last pos
		M_FollowPlayr(mob, game);
	}
	else {
		changedir = true;
	}
	// changing direction if needed
	if (changedir) {
		mob->mdir = chasedir;
		// try again after changing
		chasedir = M_GetChaseDir(mob, game);
		if (chasedir == 5) {
			mob->mdir = chasedir;
		}
		// if it fails again, give up
		else {
			return;
		}
	}
}