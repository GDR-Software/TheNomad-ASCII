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
//  src/scf.cpp
//----------------------------------------------------------
#include "scf.h"
#include "g_game.h"

namespace scf {
	nomadbool_t music_on = false;
	nomadbool_t sfx_on = false;
	nomadushort_t music_vol = 0;
	nomadushort_t sfx_vol = 0;
	nomadushort_t mobspeed = MOB_SPEED_STD;
	nomadushort_t fov[2] = {MAX_VERT_FOV, MAX_HORZ_FOV};
	namespace launch {
		nomadbool_t fastmobs1 = false;
		nomadbool_t fastmobs2 = false;
		nomadbool_t fastmobs3 = false;
		nomadbool_t ext_bff = false;
		nomadbool_t deafmobs = false;
		nomadbool_t blindmobs = false;
		nomadbool_t nosmell = false;
		nomadbool_t nomobs = false;
		nomadbool_t godmode = false;
		nomadbool_t infinite_ammo = false;
		nomadbool_t bottomless_clip = false;
		nomadbool_t devmode = false;
	};
};
/* might use this... (in the future)
void strrch(char *__restrict str, char garbage)
{
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
*/