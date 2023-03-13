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

static Game* game;

static void A_Kill()
{
}

void A_Init(Game* const gptr)
{
    game = gptr;
//    atexit(A_Kill);
}

typedef struct animation_s
{
    nomadlong_t numtics;
    animator_t anim;
    animatorp_t panim;
    animatorm_t manim;
    animatorb_t banim;
    Mob* mob = NULL;
    Playr* playr = NULL;
    NPC* npc = NULL;
    animation_s(nomadlong_t ticker, animator_t func)
        : numtics(ticker), anim(func)
    {
    }
    animation_s(nomadlong_t ticker, animatorp_t func, Playr* ptr)
        : numtics(ticker), panim(func), playr(ptr)
    {
    }
    animation_s(nomadlong_t ticker, animatorm_t func, Mob* ptr)
        : numtics(ticker), manim(func), mob(ptr)
    {
    }
    animation_s(nomadlong_t ticker, animatorb_t func, NPC* ptr)
        : numtics(ticker), banim(func), npc(ptr)
    {
    }
} animation_t;

static std::vector<animation_t> animators;
static std::vector<animation_t> panimators;
static std::vector<animation_t> manimators;
static std::vector<animation_t> banimators;

void A_PushAnimation(nomadlong_t numtics, animator_t anim)
{
    animators.push_back({numtics, anim});
}
void A_PushAnimation(nomadlong_t numtics, animatorm_t anim, Mob* mob)
{
    animators.push_back({numtics, anim, mob});
}
void A_PushAnimation(nomadlong_t numtics, animatorp_t anim, Playr* playr)
{
    animators.push_back({numtics, anim, playr});
}
void A_PushAnimation(nomadlong_t numtics, animatorb_t anim, NPC* npc)
{
    animators.push_back({numtics, anim, npc});
}

void A_Loop()
{
//    pthread_mutex_lock(&loopmutex);
    std::vector<animation_t>::iterator it;
    for (it = animators.begin(); it != animators.end(); ++it) {
        --it->numtics;
        if (it->numtics <= -1) animators.erase(it);
        else (*it->anim)();
    }
    for (it = panimators.begin(); it != panimators.end(); ++it) {
        --it->numtics;
        if (it->numtics <= -1) panimators.erase(it);
        else (*it->panim)(it->playr);
    }
    for (it = manimators.begin(); it != manimators.end(); ++it) {
        --it->numtics;
        if (it->numtics <= -1) manimators.erase(it);
        else (*it->manim)(it->mob);
    }
    for (it = banimators.begin(); it != banimators.end(); ++it) {
        --it->numtics;
        if (it->numtics <= -1) banimators.erase(it);
        else (*it->banim)(it->npc);
    }
//    pthread_mutex_unlock(&loopmutex);
//    return NULL;
}