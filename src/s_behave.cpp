#include "g_game.h"
#include "g_rng.h"
#include "p_npc.h"
#include "g_obj.h"

npc_t Game::N_MakeNPC(void)
{
	nomaduint_t randnpc = rand() % sizeof(npcinfo);
	npc_t npc;
	for (nomadulong_t i = 0; i < 9; i++) {
		if (randnpc == i) {
			npc =  npcinfo[i];
		}
	}
	return npc;
}

void Game::I_InitNPCs(void)
{
	b_Active.reserve(MAX_NPC_ACTIVE);
	for (nomaduint_t y = 0; y < MAP_MAX_Y; y++) {
		for (nomaduint_t x = 0; x < MAP_MAX_X; x++) {
			if (b_Active.size() == MAX_NPC_ACTIVE) {
				break;
			}
			nomaduint_t i = rand() & 255;
			if (i > 100) {
				b_Active.emplace_back();
				b_Active.back() = (NPC*)Z_Malloc(sizeof(NPC), TAG_STATIC, &b_Active.back());
				NPC* newnpc = b_Active.back();
				newnpc->c_npc = N_MakeNPC();
				newnpc->pos.y = (P_Random() & 400)+90;
				newnpc->pos.x = (P_Random() & 400)+90;
				newnpc->ndir = D_NORTH;
			}
		}
	}
}

NPC::~NPC()
{
	Z_Free(this);
}