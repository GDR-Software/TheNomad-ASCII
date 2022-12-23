#include "g_mob.h"
#include "g_game.h"
#include "scf.h"
#include "g_rng.h"
#include "g_obj.h"

void Game::M_GenMobs(void)
{
	m_Active.reserve(MAX_MOBS_ACTIVE);
	for (nomadushort_t y = 0; y < MAP_MAX_Y; y++) {
		for (nomadushort_t x = 0; x < MAP_MAX_X; x++) {
			if (m_Active.size() == MAX_MOBS_ACTIVE) {
				break;
			}
			nomaduint_t i = rand() % 255;
			if (i > 50) {
				nomaduint_t randmob = 0;
				mobj_t newmob;
				for (nomaduint_t f = 0; f < NUMMOBS; f++) {
					if (f == randmob) {
						newmob = mobinfo[f];
					}
				}
				coord_t mpos;
				mpos.y = (P_Random() & 400)+90;
				mpos.x = (P_Random() & 400)+90;
				m_Active.emplace_back();
				m_Active.back() = (Mob*)Z_Malloc(sizeof(Mob), TAG_STATIC, &m_Active.back());
				Mob* mob = m_Active.back();
				mob->mpos = mpos;
				mob->c_mob = newmob;
				mob->is_boss = false;
				mob->mstate = stateinfo[S_MOB_NULL+mob->c_mob.mtype];
				mob->mticker = mob->mstate.numticks;
			}
		}
	}
}

Mob::~Mob()
{
	Z_Free(this);
}
/*
nomadbool_t Mob::M_HearImmediate(const Game* map)
{
	if (map->mapbuffer[mpos.y - 1][mpos.x].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y][mpos.x - 1].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y + 1][mpos.x].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y][mpos.x + 1].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y - 1][mpos.y - 1].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y - 1][mpos.y + 1].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y + 1][mpos.y - 1].snd_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y + 1][mpos.y + 1].snd_lvl >= SND_SOME) {
		return true;
	}
	else {
		return false;
	}
}

nomadbool_t Mob::M_HearPlayr(const std::unique_ptr<Game>& map)
{
	if (scf::launch::deafmobs) {
		return false;
	}
	else {
		if (!M_HearImmediate(map)) {
			bool found = false;
			for (nomadushort_t y = mpos.y;
			y <= (mpos.y - (c_mob.snd_area >> 1)); y--) {
				for (nomadushort_t x = mpos.x;
				x <= (mpos.x + (c_mob.snd_area >> 1)); x--) {
					if (map->mapbuffer[y][x].snd_lvl >= SND_LOW) {
						found = true;
					}
				}
			}
			return found;
		}
		else {
			return false;
		}
	}
}

nomadbool_t Mob::M_SeePlayr(const std::unique_ptr<Game>& map)
{
	if (scf::launch::blindmobs) {
		return false;
	}
	else {
		switch (mdir) {
		case D_NORTH: {
			for (nomadushort_t i = mpos.y; 
			i <= (mpos.y + c_mob.sight_range); i -= 1) {
			if (map->c_map[i][mpos.x] == '@') {
					return true;
				}
			}
			break; }
		case D_WEST: {
			for (nomadushort_t i = mpos.x;
			i <= (mpos.x + c_mob.sight_range); i -= 1) {
				if (map->c_map[mpos.y][i] == '@') {
					return true;
				}
			}
			break; }
		case D_SOUTH: {
			for (nomadushort_t i = mpos.y;
			i <= (mpos.y + c_mob.sight_range); i += 1) {
				if (map->c_map[i][mpos.x] == '@') {
					return true;
				}
			}
			break; }
		case D_EAST: {
			for (nomadushort_t i = mpos.x;
			i <= (mpos.x + c_mob.sight_range); i += 1) {
				if (map->c_map[mpos.y][i] == '@') {
					return true;
				}
			}
			break; }
		default:
			N_Error("Unknown/Invalid Direction For Mob: %s", c_mob.name);
			break;
		};
		return false;
	}
}

nomadbool_t Mob::M_SmellImmediate(const std::unique_ptr<Game>& map)
{
	if (map->mapbuffer[mpos.y - 1][mpos.x].smel_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y][mpos.x - 1].smell_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y + 1][mpos.x].smell_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y][mpos.x + 1].smell_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y - 1][mpos.y - 1].smell_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y - 1][mpos.y + 1].smell_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y + 1][mpos.y - 1].smell_lvl >= SND_SOME
	|| map->mapbuffer[mpos.y + 1][mpos.y + 1].smell_lvl >= SND_SOME) {
		return true;
	}
	else {
		return false;
	}
}

nomadbool_t Mob::M_SmellPlayr(const std::unique_ptr<Game>& map)
{
	if (scf::launch::nosmell) {
		return false;
	}
	else {
		if (!M_SmellImmediate(map)) {
			bool found = false;
			for (nomadushort_t y = mpos.y;
			y <= (mpos.y - (c_mob.snd_area >> 1)); y--) {
				for (nomadushort_t x = mpos.x;
				x <= (mpos.x + (c_mob.snd_area >> 1)); x--) {
					if (map->mapbuffer[y][x].smell_lvl >= SND_LOW) {
						found = true;
					}
				}
			}
			return found;
		}
		else {
			return false;
		}
	}
}

void Mob::M_SpawnThink(std::unique_ptr<Game>& game)
{
	if (mob->mticker) {
		return;
	}
	else {
		mob->mstate = stateinfo[S_MOB_IDLE+mob->c_mob.mtype];
		mob->mticker = mstate.numticks;
	}
}
void Mob::M_IdleThink(std::unique_ptr<Game>& game)
{
	if (M_SmellPlayr(game)) {
		if (M_HearPlayr(game)) {
			game->M_FollowPlayr(this, true, true, (M_SeePlayr(game));
		}
	}
	else if (M_SeePlayr(game)) {
		game
	}
}
void Mob::M_SmellThink(std::unique_ptr<Game>& game)
{
	if (mticker) {
		game->M_FollowPlayr(this, (M_SmellPlayr(game)), (M_HearPlayr(game)), (M_SeePlayr(game)));
	}
	else {
		if (M_SmellPlayr(mob)) {
			(void)game->E_Move(&mpos, &mdir);
		}
		else {
			mstate = stateinfo[S_MOB_IDLE+c_mob.mtype];
			mticker = mstate.numticks;
		}
	}
}
void Mob::M_HearThink(std::unique_ptr<Game>& game)
{
	if 
}
void Mob::M_ChasePlayr(std::unique_ptr<Game>& game)
{
	
}
void Mob::M_FightThink(std::unique_ptr<Game>& game)
{
	
}
void Mob::M_FleeThink(std::unique_ptr<Game>& game)
{
	
}
void Mob::M_WanderThink(std::unique_ptr<Game>& game)
{
	if (mob->mticker) {
		if (!game->E_Move(&mob->mpos, &mob->mdir)) {
			return;
		}
		else {
			if (game->M_HearPlayr(mob) || game->M_SmellPlayr(mob)) {
				game->M_FollowPlayr(mob);
				mob->mticker = mob->mstate.numticks;
			}
			if (game->M_SeePlayr(mob)) {
				game->M_FollowPlayr(mob);
				mob->mstate = stateinfo[S_MOB_CHASEPLAYR+mob->c_mob.mtype];
				mob->mticker = mob->mstate.numticks;
			}
		}
	}
	else {
		if (!game->M_HearPlayr(mob) && !game->M_SmellPlayr(mob)) {
			mob->mstate = stateinfo[S_MOB_IDLE];
		}
		else {
			mob->mticker = mob->mstate.numticks;
		}
	}
}
void Mob::M_DeadThink(std::unique_ptr<Game>& game)
{
	
}

void Game::M_FollowPlayr(Mob* mob, nomadbool_t smell, nomadbool_t hear,
	nomadbool_t see)
{
	
}

void M_AngryPunch(Mob* mob, std::unique_ptr<Game>& game)
{
	
}

*/