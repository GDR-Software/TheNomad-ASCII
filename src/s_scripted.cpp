#include "n_shared.h"
#include "g_game.h"
#include "s_scripted.h"

static void Run_S1C1(chapter_t& stage);
static void Run_S1C2(chapter_t& stage);
static void Run_S1C3(chapter_t& stage);
static void Run_S1C4(chapter_t& stage);
static void Run_EndOfGame(chapter_t& stage);

// global vars for the scripts file
pthread_mutex_t ev_mutex;
pthread_cond_t ev_bool;
nomadbool_t pmove_lock = false;
area_t lock;
std::vector<Mob*> spawned_mobs;
pthread_t c_thread;
campaign_prog_t player_progress = S1C1_FROM_EAGLES_PEAK;
int32_t stage = 0;
static Game* game;


void Event_Stage1(stage_t& stage);
void Event_Stage2(stage_t& stage);

static pthread_mutex_t waiter_mutex;
static stage_t* c_stage;

// this you MUST HAVE TO MAKE SCRIPTED ENCOUNTERS FOR THE GAME!!!!
stage_t stages[NUMSTAGES] = {
    {"Tales From A Lost Land", NULL, false,
        {
            {"From Eagle's Peak",      DIF_NOOB, S1C1_FROM_EAGLES_PEAK,    NULL, S1C2_TO_GUNS_N_GRENADES,  false, &stages[0]},
            {"To Guns \'n\' Grenades", DIF_NOOB, S1C2_TO_GUNS_N_GRENADES,  NULL, S1C3_INVASION_OF_GALAKAS, false, &stages[0]},
            {"Invasion of Galakas",    DIF_NOOB, S1C3_INVASION_OF_GALAKAS, NULL, S1C4_A_DUEL_WITH_DEATH,   false, &stages[0]},
            {"A Duel With Death",      DIF_NOOB, S1C4_A_DUEL_WITH_DEATH,   NULL, END_OF_CAMPAIGN,          false, &stages[0]},
        },
    },
};

//
// G_ConfirmChapter(): prompts the player before they embark on said chapter,
// just to make sure what they are doing is by choice
//
static nomadbool_t already_asked = false;
static nomadbool_t G_ConfirmChapter(chapter_t& chapter)
{
    already_asked = true;
    // ignore the trigger if the player isn't at this chapter's point yet
    if (chapter.prog_num > (game->playr->c_chapter->prog_num+1)) {
        game->playr->pmode = P_MODE_ROAMING;
        return false;
    }
    char c;
    Hud_Printf("System", "You are about to embark on a chapter, press y to continue");
    wrefresh(game->screen);
    c = getc(stdin);
    if (c == 'y') {
        Hud_Printf("System", "You have chose to continue, Chapter: %s Stage: %s",
            chapter.chaptername, chapter.stage->stagename);
        game->playr->c_chapter = &chapter;
        return true;
    } else if (c == 'n') {
        Hud_Printf("System", "Farewell then, Nomad");
        game->playr->pmode = P_MODE_ROAMING;
        pmove_lock = false;
        return false;
    }
    return false;
}

static void KillEvents()
{
#if 0
    pthread_mutex_destroy(&ev_mutex);
#endif
}

void G_InitEvents(Game* const gptr)
{
#if 0
    PTR_CHECK(NULL_CHECK, gptr);
    game = gptr;
    printf("G_InitEvents(): initializing scripted encounters subsystem daemon...\n");
    pthread_mutex_init(&ev_mutex, NULL);
    atexit(KillEvents);
    c_stage = &stages[0];
#endif
}

void* G_EventDaemon(void *arg)
{
//    pthread_mutex_lock(&ev_mutex);
//    if (game->playr->pmode != P_MODE_MISSION) {
//        nomadbool_t triggered = false;
//        switch (player_progress) {
//        case S1C1_FROM_EAGLES_PEAK: {
//            triggered = PLAYR_IN_AREA({{401, 117}, {401, 156}, {430, 117}, {430, 156}});
//            break; }
//        };
//        if (triggered) {
//            if (already_asked) {
//                already_asked = false;
//            }
//            else {
//                game->playr->pmode = P_MODE_MISSION;
//            }
//        }
//    } else {
//        (*game->playr->c_chapter->chapterfunc)(*game->playr->c_chapter);
//    }
//    pthread_mutex_unlock(&ev_mutex);
    return NULL;
}
/*

/
* [Run_S1C1]
* Name: From Eagle's Peak
* Stage: Tales From A Lost Land
/
static nomaduint_t progtrack = 0;
static void Run_S1C1(chapter_t& chapter)
{
    if (progtrack == 0) {
        if (!G_ConfirmChapter(chapter)) {
            game->playr->pmode = P_MODE_ROAMING;
            return;
        }
        pmove_lock = true;
        Hud_Printf("System", "You stand at the edge of Eagle's Peak, watching the world");
        P_Teleport(390, 120);
        LooperDelay(2);
        ++progtrack;
    }
    else if (progtrack == 1) {
        Hud_Printf("System", "You gaze out at the barren and rotting wastes");
        LooperDelay(2);
        ++progtrack;
    }
    else if (progtrack == 2) {
        Hud_Printf("System", "You are a mercenary, and right now, your objective is a bounty");
        LooperDelay(2);
        ++progtrack;
    }
    else if (progtrack == 3) {
        Hud_Printf("System", "Your bounty is known to all as Father Grunge, priced at 1000 gold");
        LooperDelay(2);
        ++progtrack;
    }
    else if (progtrack == 4) {
        Hud_Printf("System", "His compound is located near whence you stand");
        LooperDelay(2);
        ++progtrack;
    }
    else if (progtrack == 5) {
        Hud_Printf("System", "Kill him and the gold will be yours");
        LooperDelay(2);
        ++progtrack;
        pmove_lock = false;
    }
    // THE BEEFY BIT
    else if (progtrack == 6) {
        ++progtrack;
    }
    else {
        game->playr->pmode = P_MODE_ROAMING;
        progtrack = 0;
    }
}

static void Run_S1C2(chapter_t& chapter)
{

}

static void Run_S1C3(chapter_t& chapter)
{

}
static void Run_S1C4(chapter_t& chapter)
{

}

void Event_Stage1(stage_t& stage)
{
    switch (game->playr->c_chapter->prog_num) {
    case S1C1_FROM_EAGLES_PEAK:
        (*game->playr->c_chapter->chapterfunc)(*game->playr->c_chapter);
        break;
    };
}

inline bool PLAYR_IN_AREA(area_t a)
{
    if ((game->playr->pos.y >= a.tl.y && game->playr->pos.y <= a.br.y)
    &&  (game->playr->pos.x >= a.tl.x && game->playr->pos.x <= a.br.x)) {
        return true;
    } else {
        return false;
    }
}
inline bool PLAYR_NOT_YX(int16_t y,int16_t x){return game->playr->pos.y!=y&&game->playr->pos.x!=x;}
inline bool PLAYR_AT_YX(int16_t y,int16_t x){return game->playr->pos.y==x&&game->playr->pos.x==x;}
inline bool PLAYR_Y_GT(int16_t y){return game->playr->pos.y>y;}
inline bool PLAYR_X_GT(int16_t x){return game->playr->pos.x>x;}
inline bool PLAYR_Y_LT(int16_t y){return game->playr->pos.y<y;}
inline bool PLAYR_X_LT(int16_t x){return game->playr->pos.x<x;}
inline bool PLAYR_Y_GTEQ(int16_t y){return game->playr->pos.y>=y;}
inline bool PLAYR_X_GTEQ(int16_t x){return game->playr->pos.x>=x;}
inline bool PLAYR_Y_LTEQ(int16_t y){return game->playr->pos.y<=y;}
inline bool PLAYR_X_LTEQ(int16_t x){return game->playr->pos.x<=x;}
inline bool PLAYR_HEALTH_EQ(int32_t x){return game->playr->health.load()==x;}
inline bool PLAYR_HEALTH_GT(int32_t x){return game->playr->health.load()>x;}
inline bool PLAYR_HEALTH_GTEQ(int32_t x){return game->playr->health.load()>=x;}
inline bool PLAYR_HEALTH_LT(int32_t x){return game->playr->health.load()<x;}
inline bool PLAYR_HEALTH_LTEQ(int32_t x){return game->playr->health.load()<=x;}
inline bool PLAYR_ARMOR_EQ(uint16_t x){return game->playr->armor.load()==x;}
inline bool PLAYR_ARMOR_GT(uint16_t x){return game->playr->armor.load()>x;}
inline bool PLAYR_ARMOR_GTEQ(uint16_t x){return game->playr->armor.load()>=x;}
inline bool PLAYR_ARMOR_LT(uint16_t x){return game->playr->armor.load()<x;}
inline bool PLAYR_ARMOR_LTEQ(uint16_t x){return game->playr->armor.load()<=x;}
inline bool MOB_AT_YX(int16_t y,int16_t x,Mob* m){return m->mpos.y==x&m->mpos.x==x;}
inline bool MOB_Y_GT(int16_t y,Mob* m){return m->mpos.y>y;}
inline bool MOB_X_GT(int16_t x,Mob* m){return m->mpos.x>x;}
inline bool MOB_Y_LT(int16_t y,Mob* m){return m->mpos.y<y;}
inline bool MOB_X_LT(int16_t x,Mob* m){return m->mpos.x<x;}
inline bool MOB_Y_GTEQ(int16_t y,Mob* m){return m->mpos.y>=y;}
inline bool MOB_X_GTEQ(int16_t x,Mob* m){return m->mpos.x>=x;}
inline bool MOB_Y_LTEQ(int16_t y,Mob* m){return m->mpos.y<=y;}
inline bool MOB_X_LTEQ(int16_t x,Mob* m){return m->mpos.x<=x;}
inline bool MOB_HEALTH_EQ(int32_t x,Mob* m){return m->health.load()==x;}
inline bool MOB_HEALTH_GT(int32_t x,Mob* m){return m->health.load()>x;}
inline bool MOB_HEALTH_GTEQ(int32_t x,Mob* m){return m->health.load()>=x;}
inline bool MOB_HEALTH_LT(int32_t x,Mob* m){return m->health.load()<x;}
inline bool MOB_HEALTH_LTEQ(int32_t x,Mob* m){return m->health.load()<=x;}
inline bool MOB_ARMOR_EQ(uint16_t x,Mob* m){return m->armor.load()==x;}
inline bool MOB_ARMOR_GT(uint16_t x,Mob* m){return m->armor.load()>x;}
inline bool MOB_ARMOR_GTEQ(uint16_t x,Mob* m){return m->armor.load()>=x;}
inline bool MOB_ARMOR_LT(uint16_t x,Mob* m){return m->armor.load()<x;}
inline bool MOB_ARMOR_LTEQ(uint16_t x,Mob* m){return m->armor.load()<=x;}
inline bool BOT_AT_YX(int16_t y,int16_t x,NPC* b){return b->pos.y==x&b->pos.x==x;}
inline bool BOT_Y_GT(int16_t y,NPC* b){return b->pos.y>y;}
inline bool BOT_X_GT(int16_t x,NPC* b){return b->pos.x>x;}
inline bool BOT_Y_LT(int16_t y,NPC* b){return b->pos.y<y;}
inline bool BOT_X_LT(int16_t x,NPC* b){return b->pos.x<x;}
inline bool BOT_Y_GTEQ(int16_t y,NPC* b){return b->pos.y>=y;}
inline bool BOT_X_GTEQ(int16_t x,NPC* b){return b->pos.x>=x;}
inline bool BOT_Y_LTEQ(int16_t y,NPC* b){return b->pos.y<=y;}
inline bool BOT_X_LTEQ(int16_t x,NPC* b){return b->pos.x<=x;}
inline bool BOT_HEALTH_EQ(int32_t x,NPC* b){return b->health.load()==x;}
inline bool BOT_HEALTH_GT(int32_t x,NPC* b){return b->health.load()>x;}
inline bool BOT_HEALTH_GTEQ(int32_t x,NPC* b){return b->health.load()>=x;}
inline bool BOT_HEALTH_LT(int32_t x,NPC* b){return b->health.load()<x;}
inline bool BOT_HEALTH_LTEQ(int32_t x,NPC* b){return b->health.load()<=x;}
inline bool BOT_ARMOR_EQ(uint16_t x,NPC* b){return b->armor.load()==x;}
inline bool BOT_ARMOR_GT(uint16_t x,NPC* b){return b->armor.load()>x;}
inline bool BOT_ARMOR_GTEQ(uint16_t x,NPC* b){return b->armor.load()>=x;}
inline bool BOT_ARMOR_LT(uint16_t x,NPC* b){return b->armor.load()<x;}
inline bool BOT_ARMOR_LTEQ(uint16_t x,NPC* b){return b->armor.load()<=x;}

*/