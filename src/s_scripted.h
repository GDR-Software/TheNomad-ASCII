#ifndef _S_SCRIPTED_
#define _S_SCRIPTED_

#pragma once


#define MAX_CHAPTERS 100

// data types to be used by the scripted encounters daemon
typedef struct stage_s stage_t;

class Mob;
class NPC;

enum campaign_prog_t : uint64_t;

#define NO_AREA_LOCK (area_t){{0,0},{0,MAP_MAX_X+80},{MAP_MAX_Y+80,0},{MAP_MAX_Y+80,MAP_MAX_X+80}}

typedef struct chapter_s
{
    const char* chaptername;
    uint_fast8_t highest_completed = DIF_NOOB;
	const campaign_prog_t prog_num;
	void (*chapterfunc)(chapter_s&);
	const campaign_prog_t next_num;
	bool done;
	const stage_t* stage;
} chapter_t;
typedef struct stage_s
{
    const char* stagename;
	void (*stagefunc)(stage_s&);
	bool done;
    std::vector<chapter_t> chapters;
} stage_t;

enum campaign_prog_t : uint64_t
{
    S1C1_FROM_EAGLES_PEAK,
    S1C2_THE_ONLY_WAY_IN_IS_THROUGH,
    NUM_STAGE1_MISSIONS,

    END_OF_CAMPAIGN
};

enum stage_num_t : uint32_t
{
    TALES_FROM_A_LOST_LAND,
    GHOSTS_FROM_THE_PAST,

    NUMSTAGES
};

class Script
{
public:
    Script(stage_t* stage, chapter_t* chapter);
    ~Script();
};

extern stage_t stages[NUMSTAGES];

// global vars for the scripts file
extern pthread_mutex_t ev_mutex;
extern pthread_cond_t ev_bool;
extern area_t lock;
extern std::vector<Mob*> spawned_mobs;
extern pthread_t c_thread;
extern campaign_prog_t player_progress;
extern int32_t stage;
extern nomadbool_t pmove_lock;

void* G_EventDaemon(void *arg);
void G_InitEvents(Game* const gptr);

/* the currently allowed event triggers */

/**
* @fn inline bool PLAYR_IN_AREA(area_t a)
* @brief event trigger that occurs if a players enters the given square area
* @param a the area_t struct the player should be within
* @return true if the player is located somewhere within the given area, false if otherwise
*/
inline bool PLAYR_IN_AREA(area_t a);
/**
* @fn inline bool PLAYR_NOT_YX(int16_t y,int16_t x)
* @brief event trigger that occurs if the player leaves a specific coordinate
* @param y the y-coordinate the player should not be at
* @param x the x-coordinate the player should not be at
* @return true if player is not at the specified coordinate, false if otherwise
*/
inline bool PLAYR_NOT_YX(int16_t y,int16_t x);
/**
* @fn inline bool PLAYR_AT_YX(int16_t x,int16_t x)
* @brief event trigger that occurs if the player is at the given coordinates
* @param y the y-coordinate the player should be at
* @param x the x-coordinate the player should be at
* @return true if the player is at the specified coordinate, false if otherwise
*/
inline bool PLAYR_AT_YX(int16_t y,int16_t x);
/**
* @fn inline bool PLAYR_Y_GT(int16_t y)
* @brief event trigger that occurs if a player's y coordinate is greater than the given
* @param y the y-coordinate the player should greater than
* @return true if the player's y is greater than given coordinate, false if otherwise
*/
inline bool PLAYR_Y_GT(int16_t y);
/**
* @fn inline bool PLAYR_X_GT(int16_t x)
* @brief event trigger that occurs if a player's x coordinate is greater than the given
* @param x the x-coordinate the player should greater than
* @return true if the player's x is greater than given coordinate, false if otherwise
*/
inline bool PLAYR_X_GT(int16_t x);
/**
* @fn inline bool PLAYR_Y_LT(int16_t y)
* @brief event trigger that occurs if a player's y coordinate is less than the given
* @param y the y-coordinate the player should greater than
* @return true if the player's y is less than given coordinate, false if otherwise
*/
inline bool PLAYR_Y_LT(int16_t y);
/**
* @fn inline bool PLAYR_X_LT(int16_t x)
* @brief event trigger that occurs if a player's x coordinate is greater than the given
* @param x the x-coordinate the player should greater than
* @return true if the player's x is greater than given coordinate, false if otherwise
*/
inline bool PLAYR_X_LT(int16_t x);
/**
* @fn inline bool PLAYR_Y_GTEQ(int16_t y)
* @brief event trigger that occurs if a player's y coordinate is greater than or equal to the given
* @param y the y-coordinate the player should greater than or equal to
* @return true if the player's y is greater than or equal to given coordinate, false if otherwise
*/
inline bool PLAYR_Y_GTEQ(int16_t y);
/**
* @fn inline bool PLAYR_X_GTEQ(int16_t x)
* @brief event trigger that occurs if a player's x coordinate is greater than or equal to the given
* @param x the x-coordinate the player should greater than or equal to
* @return true if the player's x is greater than or equal to given coordinate, false if otherwise
*/
inline bool PLAYR_X_GTEQ(int16_t x);
/**
* @fn inline bool PLAYR_Y_LTEQ(int16_t y)
* @brief event trigger that occurs if a player's y coordinate is less than or equal to the given
* @param y the y-coordinate the player should less than or equal to
* @return true if the player's y is less than or equal to given coordinate, false if otherwise
*/
inline bool PLAYR_Y_LTEQ(int16_t y);
/**
* @fn inline bool PLAYR_X_LTEQ(int16_t x)
* @brief event trigger that occurs if a player's x coordinate is less than or equal to the given
* @param x the x-coordinate the player should less than or equal to
* @return true if the player's x is less than or equal to given coordinate, false if otherwise
*/
inline bool PLAYR_X_LTEQ(int16_t x);
/**
* @fn inline bool PLAYR_HEALTH_EQ(int32_t x)
* @brief event trigger that occurs if a player's health is equal to given value
* @param x the value which the player's health should be equal to
* @return true if the player's health is equal to x, false if otherwise
*/
inline bool PLAYR_HEALTH_EQ(int32_t x);
/**
* @fn inline bool PLAYR_HEALTH_GT(int32_t)
* @brief event trigger that occurs if a player's health is greater than x
* @param x the value which the player's health should be greater than
* @return true if the player's health is greater than x, false if otherwise
*/
inline bool PLAYR_HEALTH_GT(int32_t x);
inline bool PLAYR_HEALTH_GTEQ(int32_t x);
inline bool PLAYR_HEALTH_LT(int32_t x);
inline bool PLAYR_HEALTH_LTEQ(int32_t x);
inline bool PLAYR_ARMOR_EQ(uint16_t x);
inline bool PLAYR_ARMOR_GT(uint16_t x);
inline bool PLAYR_ARMOR_GTEQ(uint16_t x);
inline bool PLAYR_ARMOR_LT(uint16_t x);
inline bool PLAYR_ARMOR_LTEQ(uint16_t x);
inline bool MOB_AT_YX(int16_t y,int16_t x,Mob* m);
inline bool MOB_Y_GT(int16_t y,Mob* m);
inline bool MOB_X_GT(int16_t x,Mob* m);
inline bool MOB_Y_LT(int16_t y,Mob* m);
inline bool MOB_X_LT(int16_t x,Mob* m);
inline bool MOB_Y_GTEQ(int16_t y,Mob* m);
inline bool MOB_X_GTEQ(int16_t x,Mob* m);
inline bool MOB_Y_LTEQ(int16_t y,Mob* m);
inline bool MOB_X_LTEQ(int16_t x,Mob* m);
inline bool MOB_HEALTH_EQ(int32_t x,Mob* m);
inline bool MOB_HEALTH_GT(int32_t x,Mob* m);
inline bool MOB_HEALTH_GTEQ(int32_t x,Mob* m);
inline bool MOB_HEALTH_LT(int32_t x,Mob* m);
inline bool MOB_HEALTH_LTEQ(int32_t x,Mob* m);
inline bool MOB_ARMOR_EQ(uint16_t x,Mob* m);
inline bool MOB_ARMOR_GT(uint16_t x,Mob* m);
inline bool MOB_ARMOR_GTEQ(uint16_t x,Mob* m);
inline bool MOB_ARMOR_LT(uint16_t x,Mob* m);
inline bool MOB_ARMOR_LTEQ(uint16_t x,Mob* m);
inline bool BOT_AT_YX(int16_t y,int16_t x,NPC* b);
inline bool BOT_Y_GT(int16_t y,NPC* b);
inline bool BOT_X_GT(int16_t x,NPC* b);
inline bool BOT_Y_LT(int16_t y,NPC* b);
inline bool BOT_X_LT(int16_t x,NPC* b);
inline bool BOT_Y_GTEQ(int16_t y,NPC* b);
inline bool BOT_X_GTEQ(int16_t x,NPC* b);
inline bool BOT_Y_LTEQ(int16_t y,NPC* b);
inline bool BOT_X_LTEQ(int16_t x,NPC* b);
inline bool BOT_HEALTH_EQ(int32_t x,NPC* b);
inline bool BOT_HEALTH_GT(int32_t x,NPC* b);
inline bool BOT_HEALTH_GTEQ(int32_t x,NPC* b);
inline bool BOT_HEALTH_LT(int32_t x,NPC* b);
inline bool BOT_HEALTH_LTEQ(int32_t x,NPC* b);
inline bool BOT_ARMOR_EQ(uint16_t x,NPC* b);
inline bool BOT_ARMOR_GT(uint16_t x,NPC* b);
inline bool BOT_ARMOR_GTEQ(uint16_t x,NPC* b);
inline bool BOT_ARMOR_LT(uint16_t x,NPC* b);
inline bool BOT_ARMOR_LTEQ(uint16_t x,NPC* b);

#endif