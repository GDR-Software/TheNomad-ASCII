//----------------------------------------------------------
//
// Copyright (C) GDR Games 2022-2023
//
// This source code is available for distribution and/or
// modification under the terms of either the Apache License
// v2.0 as published by the Apache Software Foundation, or
// the GNU General Public License v2.0 as published by the
// Free Software Foundation.
//
// This source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY. If you are using this code for personal,
// non-commercial/monetary gain, you may use either of the
// licenses permitted, otherwise, you must use the GNU GPL v2.0.
//
// DESCRIPTION: src/g_rng.cpp
//  random-number generation
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