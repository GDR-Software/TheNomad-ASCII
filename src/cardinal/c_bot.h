#ifndef _C_BOT_
#define _C_BOT_

#include "c_system.h"

namespace cardinal {

typedef enum : u8
{
    A_TN,
    A_LN,
    A_CN,
    A_LG,
    A_LE,
    A_CG,
    A_CE,
    
    NUMALIGNMENTS
} alignment_t;

enum : u8
{
    G_MALE,
    G_FEMALE,

    NUMGENDERS
};

// family enums, to help organize a bot's relationship with a relative
typedef enum : u8
{
    F_PARENT,
    F_SIBLING,
    F_COUSIN,
    F_AU // aunt or uncle
} fvar_t;

typedef enum : u8
{
    R_HATES, // absolutely despises said bot/player
    R_DISLIKES, // somewhat dislikes said bot/player
    R_MEH, // mutual relationship with said bot/player
    R_FRIENDLY, // friendly relationship with said bot/player
    R_BFF, // no not the file type, just best friends
    R_LOVE // is in love with said bot/player
} rvar_t;

enum : u8
{
    // the group factions, the bots who don't ally with a specific nation, but instead a group of people
    FACTION_ZURGUTS,
    FACTION_REDMOONCULT,
    FACTION_NOMADS,

    // mercenary-specific factions
    FACTION_SOS, // swords of sortior
    FACTION_GNG, // guns 'n' grenades
    FACTION_MS, // midnight stalkers

    // settlement factions, those who will ally with a certain settlement
    FACTION_GALAKAS,
    FACTION_SORTIOR,
    FACTION_LATAN,

    NUMFACTIONS
};

// occupation-specific bots (bots that are hard-coded into the game) don't use these enums
enum : u8
{
    O_MERCHANT,
    O_SOLDIER,
    O_MERCENARY,
    O_GUARD
};

// a bot's relationship with another bot or the player
struct botrelation
{
    bool isfamily;
    fvar_t f_var; // the type of family the bot is, see family enums
    C_Bot* botptr; // if the relationship is with a bot, then this points to the bot

    rvar_t r_var; // the overall relationship variable
};

typedef struct
{
    std::vector<C_Bot*> b_Active;
    C_Bot* leader;
    area_t area;
} botgroup_t;

class C_Bot
{
public: // relationship vars
    bool knowsplayer; // whether the bot is aware of the player's existence or not
    std::vector<botrelation> family; // the bot's family
    botrelation relationp; // the bot's relationship with the player, isn't applicable if knowsplayer = false
public: // general variables
    bool isnomad; // special stuff applies to the bot if it's a nomad
    u8 race; // the bot's race
    u8 gender; // the bot's gender
    u8 faction; // the faction that the bot is in
    u8 occupation;
    alignment_t alignment; // the bot's alignment
    coord_t bpos; // coordinates of the bot
public:
    C_Bot();
    ~C_Bot();
};

};

#endif