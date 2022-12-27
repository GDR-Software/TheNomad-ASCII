#include "g_game.h"

void M_FollowPlayr(Mob* const mob, Game* const game);

void M_SearchForPlayr(Mob* const mob, Game* const game);

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

void M_SearchForPlayr(Mob* const mob, Game* const game)
{
	nomadenum_t pursuit;
	if (mob->M_SmellPlayr()) {
		pursuit
	}
}