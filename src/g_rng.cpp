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
//  src/g_rng.cpp
//----------------------------------------------------------
#include "g_rng.h"
#include <stdlib.h>

int	rndindex = 0;

int G_GetRandom(int difficulty)
{
	int rnd = rand() % 256;
	int num = 0;
	switch (difficulty) {
	case 0: // DIF_NOOB
		num = rndtable[rnd & 50];
		break;
	case 1: // DIF_RECRUIT
		num = rndtable[rnd & 100];
		break;
	case 2: // DIF_MERCENART
		num = rndtable[rnd & 150];
		break;
	case 3: // DIF_NOMAD
		num = rndtable[rnd & 200];
		break;
	case 4: // DIF_BLACKDEATH
		num = rndtable[rnd & 250];
		break;
	case 5: // DIF_MINORINCONVENIENCE
		num = rndtable[rnd & 300];
		break;
	default: break;
	};
	return num;
}

int P_Random(void)
{
	if (rndindex > 255) {
		G_ClearRandom();
	}
    rndindex = (rndindex+1) & 0xff;
    return rndtable[rndindex];
}

void G_ClearRandom(void)
{
	rndindex = 0;
}