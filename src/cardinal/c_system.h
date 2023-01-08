#ifndef _C_SYSTEM_
#define _C_SYSTEM_

#include "../n_shared.h"
#include "../g_zone.h"

#define INITIAL_MOBS_ACTIVE 500
#define INITIAL_NPC_ACTIVE 500

#ifndef _C_TYPES_
#define _C_TYPES_

typedef uint_fast8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef __uint128_t u128;
typedef int_fast8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef __int128_t i128;

#endif

constexpr u16 MAP_MAX_Y       = 360;
constexpr u16 MAP_MAX_X       = 360;
constexpr u8 SECTOR_MAX_X     = 120;
constexpr u8 SECTOR_MAX_Y     = 120;
constexpr u8 NUM_SECTORS      = 9;

namespace cardinal {

class C_NPC;
class C_Mob;
class C_Bot;

class Cardinal
{
private:
    std::vector<C_NPC*> b_Active;
    std::vector<C_Mob*> m_Active;

    pthread_mutex_t mobmutex;
    pthread_mutex_t npcmutex;

    pthread_t mobthread;
    pthread_t npcthread;
public:
    Cardinal() = default;
    ~Cardinal();

    void InitSystem();
    void C_GenerateBotGroups(); // really just generates the mobs
    void C_GenerateIndividuals(); // generates the individual bots, the ones who wander alone
};


};

#endif