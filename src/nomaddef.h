//----------------------------------------------------------
//
// Copyright (C) SIGAAMDAD 2022-2023
//
// This source is available for distribution and/or modification
// only under the terms of the SACE Source Code License as
// published by SIGAAMDAD. All rights reserved
//
// The source is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of FITNESS FOR A PARTICLAR PURPOSE. See the SACE
// Source Code License for more details. If you, however do not
// want to use the SACE Source Code License, then you must use
// this source as if it were to be licensed under the GNU General
// Public License (GPL) version 2.0 or later as published by the
// Free Software Foundation.
//
// DESCRIPTION:
//  src/nomaddef.h
//  general macros and defines that aren't really specific to
//  this game. Big 'ol header
//----------------------------------------------------------
#ifndef __NOMADDEF__
#define __NOMADDEF__

#pragma once

#define CURRENCY_EMERALD       4
#define CURRENCY_GOLD          3
#define CURRENCY_SILVER        2
#define CURRENCY_COPPER        1
#define CURRENCY_ZINC          0

#define rightarm      playr->P_wpns[0]
#define leftarm       playr->P_wpns[1]
#define sidearm       playr->P_wpns[2]
#define heavyside     playr->P_wpns[3]
#define primary       playr->P_wpns[4]
#define heavyprimary  playr->P_wpns[5]
#define shotty        playr->P_wpns[6]
#define melee1        playr->P_wpns[7]
#define melee2        playr->P_wpns[8]
#define melee3        playr->P_wpns[9]

#define p_rightarm      P_wpns[0]
#define p_leftarm       P_wpns[1]
#define p_sidearm       P_wpns[2]
#define p_heavyside     P_wpns[3]
#define p_primary       P_wpns[4]
#define p_heavyprimary  P_wpns[5]
#define p_shotty        P_wpns[6]
#define p_melee1        P_wpns[7]
#define p_melee2        P_wpns[8]
#define p_melee3        P_wpns[9]

#define PLAYR_MAX_WEIGHT       1000
#define PLAYR_MAX_WPNS         10
#define PLAYR_NAME_MAX_LEN     100
#define PLAYR_MAX_LVL          100
#define PLAYR_MAX_HEALTH       100
#define PLAYR_MAX_ARMOR        48

#define COIN_EMERALD_STRING    "Coin (emerald)"
#define COIN_GOLD_STRING       "Coin (gold)"
#define COIN_SILVER_STRING     "Coin (silver)"
#define COIN_COPPER_STRING     "Coin (copper)"
#define COIN_ZINC_STRING       "Coin (zinc)"

#define SHELL_STRING           "shotgun shell"
#define SHELLBOX_STRING        "box of shotgun shells"
#define BULLET_STRING          "magazine of bullets"
#define BULLETBOX_STRING       "box of bullets"

#define GAIN_HEALTH(x)         "You gained "x" health"
#define GAIN_ARMOR(x)          "You gained "x" armor"
#define PICKUP_AMMO(x)         "You picked up "x"!"
#define PICKUP_WEAPON(x)       "You got the "x"!"

#define SHOTTY_ADB_STRING      "Asturion Double-Barrel"
#define SHOTTY_FAB_STRING      "Full-Auto Bitch"
#define SHOTTY_QS_STRING       "Quick-Shotter"
#define SIDE_AUP_STRING        "All-Use-Pistol"
#define SIDE_BOS_STRING        "Browning Old-School"
#define SIDE_FUSION_STRING     "Fusion Sidearm"
#define SIDE_ION_STRING        "Ion Pistol"
#define SIDE_PLASMA_STRING     "Plasma Pistol"
#define HSIDE_A8SHOT_STRING    "Asturion 8-Shot"
#define HSIDE_SADB_STRING      "Asturion Sawed-Off"
#define PRIM_M23C5_STRING      "M23C5 Assault-Rifle"
#define PRIM_AK77_STRING       "AK-77 Assault-Rifle"
#define PRIM_RAG15_STRING      "RAG model 15"
#define PRIM_RAG14_STRING      "RAG model 14"
#define PRIM_PLASMASMG_STRING  "Plasma SMG"
#define HPRIM_FUSION_STRING    "Fusion Cannon"
#define HPRIM_HOSIG_STRING     "HOS-IG Sniper Rifle"
#define HPRIM_DR_STRING        "Desert Rifle"
#define HPRIM_RAG13_STRING     "RAG model 13"
#define LAUNCHER_MGL_STRING    "MGL Grenade Launcher"
#define ARM_HB_STRING          "Hidden Blade"
#define ARM_GRAPPLE_STRING     "Grappling Hook"
#define ARM_HC_STRING          "Hand Cannon"
#define ARM_SB_STRING          "Sound Blaster"
#define ARM_FT_STRING          "Flamethrower"

#define SHOTTY_ADB_DAMAGE      45
#define SHOTTY_FAB_DAMAGE      21
#define SHOTTY_QS_DAMAGE       15
#define SIDE_AUP_DAMAGE        33
#define SIDE_BOS_DAMAGE        15
#define SIDE_FUSION_DAMAGE     59
#define SIDE_ION_DAMAGE        12
#define SIDE_PLASMA_DAMAGE     16
#define HSIDE_A8SHOT_DAMAGE    70
#define HSIDE_SADB_DAMAGE      56
#define PRIM_M23C5_DAMAGE      34
#define PRIM_AK77_DAMAGE       37
#define PRIM_RAG15_DAMAGE      11
#define PRIM_RAG14_DAMAGE      31
#define PRIM_PLASMASMG_DAMAGE  19
#define HPRIM_FUSION_DAMAGE    160
#define HPRIM_HOSIG_DAMAGE     91
#define HPRIM_DR_DAMAGE        99
#define HPRIM_RAG13_DAMAGE     75
#define LAUNCHER_MGL_DAMAGE    55
#define ARM_HB_DAMAGE          70
#define ARM_GRAPPLE_DAMAGE     34
#define ARM_HC_DAMAGE          54
#define ARM_SB_DAMAGE          60
#define ARM_FT_DAMAGE          29

#define SHOTTY_ADB_WEIGHT      40
#define SHOTTY_FAB_WEIGHT      56
#define SHOTTY_QS_WEIGHT       37
#define SIDE_AUP_WEIGHT        20
#define SIDE_BOS_WEIGHT        14
#define SIDE_FUSION_WEIGHT     44
#define SIDE_ION_WEIGHT        31
#define SIDE_PLASMA_WEIGHT     34
#define HSIDE_A8SHOT_WEIGHT    50
#define HSIDE_SADB_WEIGHT      49
#define PRIM_M23C5_WEIGHT      67
#define PRIM_AK77_WEIGHT       59
#define PRIM_RAG15_WEIGHT      78
#define PRIM_RAG14_WEIGHT      48
#define PRIM_PLASMASMG_WEIGHT  23
#define HPRIM_FUSION_WEIGHT    90
#define HPRIM_HOSIG_WEIGHT     87
#define HPRIM_DR_WEIGHT        66
#define HPRIM_RAG13_WEIGHT     52
#define LAUNCHER_MGL_WEIGHT    96
#define ARM_HB_WEIGHT          10
#define ARM_GRAPPLE_WEIGHT     13
#define ARM_HC_WEIGHT          18
#define ARM_SB_WEIGHT          13
#define ARM_FT_WEIGHT          18

#define SHOTTY_ADB_NUMPELLETS  10
#define SHOTTY_FAB_NUMPELLETS  8
#define SHOTTY_QS_NUMPELLETS   4

#define SHOTTY_ADB_SPREAD      9
#define SHOTTY_FAB_SPREAD      7
#define SHOTTY_QS_SPREAD       5
#define BURST2_SPREAD          2
#define BURST3_SPREAD          3
#define BURST4_SPREAD          5
#define SINGLE_SPREAD          1
#define AUTO_SPREAD            6

#define SHOTTY_RNG             34
#define SIDE_RNG               56
#define HSIDE_RNG              59
#define PRIM_RNG               60
#define HPRIM_RNG              79
#define ARM_RNG                70

#define SHOTTY_ADB_MAGSIZE     2
#define SHOTTY_FAB_MAGSIZE     24
#define SHOTTY_QS_MAGSIZE      14
#define SIDE_AUP_MAGSIZE       33
#define SIDE_BOS_MAGSIZE       9
#define SIDE_FUSION_MAGSIZE    4
#define SIDE_ION_MAGSIZE       20
#define SIDE_PLASMA_MAGSIZE    18
#define HSIDE_A8SHOT_MAGSIZE   8
#define HSIDE_SADB_MAGSIZE     2
#define PRIM_M23C5_MAGSIZE     45
#define PRIM_AK77_MAGSIZE      30
#define PRIM_RAG15_MAGSIZE     32
#define PRIM_RAG14_MAGSIZE     20
#define PRIM_PLASMASMG_MAGSIZE 44
#define HPRIM_FUSION_MAGSIZE   6
#define HPRIM_HOSIG_MAGSIZE    12
#define HPRIM_DR_MAGSIZE       4
#define HPRIM_RAG13_MAGSIZE    24

// always in pints
#define SHOTTY_ADB_RANGE       20
#define SHOTTY_FAB_RANGE       12
#define SHOTTY_QS_RANGE        8
#define SIDE_AUP_RANGE         23
#define SIDE_BOS_RANGE         39
#define SIDE_FUSION_RANGE      43
#define SIDE_ION_RANGE         14
#define SIDE_PLASMA_RANGE      12
#define HSIDE_A8SHOT_RANGE     24
#define HSIDE_SADB_RANGE       8
#define PRIM_M23C5_RANGE       56
#define PRIM_AK77_RANGE        44
#define PRIM_RAG15_RANGE       18
#define PRIM_RAG14_RANGE       54
#define PRIM_PLASMASMG_RANGE   23
#define HPRIM_FUSION_RANGE     150
#define HPRIM_HOSIG_RANGE      240 // can reach mobs nearly across the map
#define HPRIM_DR_RANGE         101
#define HPRIM_RAG13_RANGE      70
#define LAUNCHER_MGL_RANGE     15
#define ARM_HB_RANGE           2
#define ARM_GRAPPLE_RANGE      36
#define ARM_HC_RANGE           6
#define ARM_SB_RANGE           15
#define ARM_FT_RANGE           10

// always in gold
#define SHOTTY_ADB_COST        45
#define SHOTTY_FAB_COST        23
#define SHOTTY_QS_COST         12
#define SIDE_AUP_COST          89
#define SIDE_BOS_COST          12
#define SIDE_FUSION_COST       34
#define SIDE_ION_COST          10
#define SIDE_PLASMA_COST       9
#define HSIDE_A8SHOT_COST      99
#define HSIDE_SADB_COST        43
#define PRIM_M23C5_COST        43
#define PRIM_AK77_COST         39
#define PRIM_RAG15_COST        7
#define PRIM_RAG14_COST        21
#define PRIM_PLASMASMG_COST    13
#define HPRIM_FUSION_COST      138
#define HPRIM_HOSIG_COST       90
#define HPRIM_DR_COST          77
#define HPRIM_RAG13_COST       56
#define LAUNCHER_MGL_COST      43
#define ARM_HB_COST            15
#define ARM_GRAPPLE_COST       10
#define ARM_HC_COST            20
#define ARM_SB_COST            25
#define ARM_FT_COST            3

// timed in-game delays (tickers)
#define FULL_TIC               (ticrate_base)
#define HALF_TIC               (ticrate_base>>1)
#define QUARTER_TIC            (ticrate_base/4)
#define FRACTION_TICKER(frac)  (ticrate_base*(frac))
#define TICKER(numtics)        (ticrate_base*numtics)

#define PLAYR_SHOOT_TICKER     FRACTION_TICKER(3/5)

#endif
