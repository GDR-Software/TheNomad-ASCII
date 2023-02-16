#include "n_shared.h"
#include "g_game.h"
#include "g_loadbff.h"
#include "s_scripted.h"

pthread_cond_t event;
pthread_mutex_t event_mutex;
static Game* game;

void E_InitScripts(bff_file_t* file)
{
    pthread_cond_init(&event, NULL);
    pthread_mutex_init(&event_mutex, NULL);
}

EVENT_INLINE(nomadbool_t) PLAYR_AT_YX(nomadshort_t y,nomadshort_t x)         { return (game->playr->pos.y==y&&game->playr->pos.x==x); }
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_EQ(nomadint_t x)                      { return (game->playr->health==x); }
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_GT(nomadint_t x)                      { return (game->playr->health>x); }
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_LT(nomadint_t x)                      { return (game->playr->health<x); }
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_GTEQ(nomadint_t x)                    { return (game->playr->health>=x); }
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_LTEQ(nomadint_t x)                    { return (game->playr->health<=x); }
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_EQ(nomadushort_t x)                    { return (game->playr->armor==x); }
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_GT(nomadushort_t x)                    { return (game->playr->armor>x); }
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_LT(nomadushort_t x)                    { return (game->playr->armor<x); }
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_GTEQ(nomadushort_t x)                  { return (game->playr->armor>=x); }
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_LTEQ(nomadushort_t x)                  { return (game->playr->armor<=x); }
EVENT_INLINE(nomadbool_t) MOB_AT_YX(nomadshort_t y,nomadshort_t x,Mob* m)    { return (m->mpos.y==y&&m->mpos.x==x); }
EVENT_INLINE(nomadbool_t) MOB_HEALTH_EQ(nomadushort_t x,Mob* m)              { return (m->health==x); }
EVENT_INLINE(nomadbool_t) MOB_HEALTH_GT(nomadushort_t x,Mob* m)              { return (m->health>x); }
EVENT_INLINE(nomadbool_t) MOB_HEALTH_LT(nomadushort_t x,Mob* m)              { return (m->health<x); }
EVENT_INLINE(nomadbool_t) MOB_HEALTH_GTEQ(nomadushort_t x,Mob* m)            { return (m->health>=x); }
EVENT_INLINE(nomadbool_t) MOB_HEALTH_LTEQ(nomadushort_t x,Mob* m)            { return (m->health<=x); }
EVENT_INLINE(nomadbool_t) MOB_ARMOR_EQ(nomadushort_t x,Mob* m)               { return (m->armor==x); }
EVENT_INLINE(nomadbool_t) MOB_ARMOR_GT(nomadushort_t x,Mob* m)               { return (m->armor>x); }
EVENT_INLINE(nomadbool_t) MOB_ARMOR_LT(nomadushort_t x,Mob* m)               { return (m->armor<x); }
EVENT_INLINE(nomadbool_t) MOB_ARMOR_GTEQ(nomadushort_t x,Mob* m)             { return (m->armor>=x); }
EVENT_INLINE(nomadbool_t) MOB_ARMOR_LTEQ(nomadushort_t x,Mob* m)             { return (m->armor<=x); }