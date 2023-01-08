#include "c_system.h"
#include "c_cnums.h"

namespace cardinal {

static pthread_t systhread;
static void* C_Loop(void *arg);
static void* M_Loop(void *arg);
static void* N_Loop(void *arg);

void C_Init()
{
    cardinal = (Cardinal *)Z_Malloc(sizeof(Cardinal), TAG_STATIC, &cardinal);
    cardinal->InitSystem();
}

Cardinal::~Cardinal()
{
    pthread_mutex_destroy(&mobmutex);
    pthread_mutex_destroy(&npcmutex);
    Z_Free(this);
}

void Cardinal::InitSystem()
{
    cnum_gentable();
    pthread_mutex_init(&mobmutex, NULL);
    pthread_mutex_init(&npcmutex, NULL);

    b_Active.reserve(INITIAL_MOBS_ACTIVE);
    m_Active.reserve(INITIAL_NPC_ACTIVE);
}

void Cardinal::C_GenerateBotGroups()
{
    cnum nummobs = get_cnum() & INITIAL_MOBS_ACTIVE;
    std::vector<area_t> points;

    // go through each sector and generate a random number of groups
    for (i8 i = 0; i < NUM_SECTORS; ++i) {
        cnum numgroups = get_cnum() & 4; // maximum of 4 groups per sector
    }
}

};