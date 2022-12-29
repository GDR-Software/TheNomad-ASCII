#ifndef _P_NPC_
#define _P_NPC_

#include "n_shared.h"
#include "g_obj.h"

#ifdef MAX_NPC_ACTIVE
#undef MAX_NPC_ACTIVE
#endif

#define MAX_NPC_ACTIVE 200

typedef struct
{
	nomadushort_t personality;
	nomadushort_t bond;
	nomadushort_t flaw;
	nomadushort_t ideal;
	nomadushort_t goal;
} traits_t;

typedef struct
{
	std::string name;
	sprite_t sprite;
	nomadint_t health;
	nomadushort_t armor;
	traits_t personality;
} npc_t;

class NPC
{
public:
	npc_t c_npc;

	coord_t pos;
	nomadenum_t ndir;
	entitystate_t nstate;
	nomaduint_t nticker;
	nomadint_t health;
	nomadushort_t armor;
public:
	NPC() = default;
	~NPC();
};

#endif
