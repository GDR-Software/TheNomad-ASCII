#include "g_game.h"
#include "g_animation.h"

static Game* game;
static pthread_mutex_t pushmutex, loopmutex;
static std::vector<animator> animations;

static void A_Kill()
{
    pthread_mutex_destroy(&pushmutex);
    pthread_mutex_destroy(&loopmutex);
}

void A_Init(Game* const gptr)
{
    game = gptr;
    atexit(A_Kill);
    pthread_mutex_init(&pushmutex, NULL);
    pthread_mutex_init(&loopmutex, NULL);
}

inline void A_PushAnimation(nomadulong_t ticcount, animator anim)
{
    pthread_mutex_lock(&pushmutex);
    animations.push_back(anim);
    pthread_mutex_unlock(&pushmutex);
}

void* A_Loop(void *arg)
{
    pthread_mutex_lock(&loopmutex);
    for (const auto& i : animations)
        (*i)();
    pthread_mutex_unlock(&loopmutex);
    return NULL;
}