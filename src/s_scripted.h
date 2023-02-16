#ifndef _S_SCRIPTED_
#define _S_SCRIPTED_

#pragma once

void E_InitScripts(bff_file_t* file);

/* all currently allowed event bools */

#define EVENT_INLINE(type) inline type

// simple bools
EVENT_INLINE(nomadbool_t) PLAYR_AT_YX(nomadshort_t y,nomadshort_t x);
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_EQ(nomadint_t x);
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_GT(nomadint_t x);
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_LT(nomadint_t x);
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_GTEQ(nomadint_t x);
EVENT_INLINE(nomadbool_t) PLAYR_HEALTH_LTEQ(nomadint_t x);
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_EQ(nomadushort_t x);
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_GT(nomadushort_t x);
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_LT(nomadushort_t x);
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_GTEQ(nomadushort_t x);
EVENT_INLINE(nomadbool_t) PLAYR_ARMOR_LTEQ(nomadushort_t x);
EVENT_INLINE(nomadbool_t) MOB_AT_YX(nomadshort_t y,nomadshort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_HEALTH_EQ(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_HEALTH_GT(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_HEALTH_LT(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_HEALTH_GTEQ(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_HEALTH_LTEQ(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_ARMOR_EQ(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_ARMOR_GT(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_ARMOR_LT(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_ARMOR_GTEQ(nomadushort_t x,Mob* m);
EVENT_INLINE(nomadbool_t) MOB_ARMOR_LTEQ(nomadushort_t x,Mob* m);

// complex bools
#define PLAYR_HAS_WPN(x) \
({ \
    for (const auto& i : game->playr->P_wpns) \
        b = x == i; \
    b; \
})
#define PLAYR_HAS_ITEM(x) \
({ \
    for (const auto& i : game->playr->inv) \
        b = x == i; \
    b; \
})

#endif