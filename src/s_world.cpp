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
//  src/s_world.cpp
//----------------------------------------------------------
#include "g_game.h"
#include "s_world.h"

static pthread_mutex_t world_mutex;
static Game* game;
static World* world;
static std::atomic<nomadulong_t>* gametics;

enum : nomadenum_t
{
    MONTH_LONGSUMMER, // 123 days of the year; begins on the first day of the year
    MONTH_MIDAUTUMN, // 62 days of the year; after longsummer
    MONTH_LONGWINTER, // 81 days of the year; after midautumn, end of the month, beginning of the new year

    NUMMONTHS
};

static constexpr nomadulong_t start_year = 91025; // the first year whence the game takes place

World::~World()
{
    Z_Free(this);
    pthread_mutex_destroy(&world_mutex);
}

static inline void M_Init(void);

void W_Init(Game* const gptr)
{
#ifdef _NOMAD_DEBUG
    assert(gptr);
#endif
    puts("W_Init(): Initializing World Data...");
    game = gptr;
    gametics = &game->ticcount;
    world = (World *)Z_Malloc(sizeof(World), TAG_STATIC, &world); // this'll stay static until the player quits or changes the gamemode
    if (!world)
        N_Error("Failed To Successfully Allocate Memory To Game World!");

#ifdef _NOMAD_DEBUG
    assert(world); // to double-check
    LOG("Successfully Allocted Memory To Game World");
#endif
    game->world = world;
#ifdef _NOMAD_DEBUG
    assert(game->world);
    LOG("Successfully Assigned Game Class World* To World* Within This Function");
#endif
    world->time.ticcount = gametics;
    world->time.month = MONTH_LONGSUMMER; // start the player off at the beginning of longsummer
    world->time.year = start_year;
    world->time.day = 0;

    M_Init();

    pthread_mutex_init(&world_mutex, NULL);
}

static void W_MissionLoop(void);
static void W_RoamingLoop(void);

// 30-hour days in bellatum terrae, this is including both sunlight hours and nightime hours
#define ticrate_day                (ticrate_hour*30)
// 18 hours of daylight
#define ticrate_lighton            (ticrate_hour*18)
// 12 hourse of nighttime
#define ticrate_lightoff           (ticrate_hour*12)
// # of tics in longsummer
#define ticrate_month_1            (ticrate_day*123)
// # of tics in midautumn
#define ticrate_month_2            ((ticrate_day*62)+ticrate_month_1)
// # of tics in longwinter
#define ticrate_month_3            ((ticrate_day*81)+ticrate_month_2)
// # of tics in a year
#define ticrate_year               (ticrate_month_1+ticrate_month_2+ticrate_month_3)

void* W_Loop(void *arg)
{
    pthread_mutex_lock(&world_mutex);
    if ((*gametics % ticrate_lightoff) == 0) { // a single day has passed
        if (!world->day) {
            world->day = true;
            ++world->time.day;
        }
    }
    // check if the month has changed
    switch (*gametics) {
    case ticrate_month_1:
        world->time.month = MONTH_MIDAUTUMN;
        break;
    case ticrate_month_2:
        world->time.month = MONTH_LONGWINTER;
        break;
    case ticrate_month_3:
        world->time.month = MONTH_LONGSUMMER;
        break;
    default:
        break;
    };
    switch (*gametics) {
    case ticrate_year:
        ++world->time.year;
        *gametics = 0; // reset the ticcount/timer
        break;
    default:
        break;
    };

    switch (game->playr->pmode) {
    case P_MODE_MISSION:
        W_MissionLoop();
        break;
    case P_MODE_ROAMING:
        W_RoamingLoop();
        break;
    default:
        // if called, just ignore the call
#ifdef _NOMAD_DEBUG
        LOG("called this function without pmode being roaming or mission");
#endif
        break;
    };
    pthread_mutex_unlock(&world_mutex);
    return NULL;
}

static void W_RoamingLoop(void)
{
    
}

static void W_MissionLoop(void)
{

}




static inline void M_Init(void)
{
#ifdef _NOMAD_DEBUG
    assert(game);
    LOG("Initializing Map Data");
#endif
	char secbuffer[NUM_SECTORS][SECTOR_MAX_Y][SECTOR_MAX_X];
	nomaduint_t y, x;
	for (nomadenum_t i = 0; i < NUM_SECTORS; ++i) {
		char path[180];
		y = x = 0;
		snprintf(path, sizeof(path), "Files/gamedata/MAP/mapsector_%hu.txt", i);
		std::ifstream file(path, std::ios::in);
		if (file.fail())
			N_Error("M_Init: Could Not Open Mapsector File %hu!", i);
		
#ifdef _NOMAD_DEBUG
		assert(file.is_open());
		LOG("Successfully opened file map file");
#endif
		std::string line;
		std::vector<std::string> buffer;
		while (std::getline(file, line)) {
			buffer.push_back(line);
		};
		for (y = 0; y < SECTOR_MAX_Y; ++y) {
			for (x = 0; x < SECTOR_MAX_X; ++x) {
				secbuffer[i][y][x] = buffer[y][x];
			}
		}
		file.close();
	}
	FILE* fp = fopen("Files/gamedata/RUNTIME/mapfile.txt", "w");
	/*
	076
	185
	234
	*/
	if (!fp)
		N_Error("Could Not Create RUNTIME/mapfile.txt!");
	
#ifdef _NOMAD_DEBUG
	assert(fp);
	LOG("Successfully created RUNTIME/mapfile.txt");
#endif
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[0][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[7][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[6][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; x++) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[1][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[8][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[5][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < SECTOR_MAX_Y; ++y) {
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[2][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[3][y][x]);
		}
		for (x = 0; x < SECTOR_MAX_X; ++x) {
			fprintf(fp, "%c", secbuffer[4][y][x]);
		}
		for (x = 0; x < 80; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	for (y = 0; y < 80; ++y) {
		for (x = 0; x < MAP_MAX_X; ++x) {
			fprintf(fp, "#");
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
#ifdef _NOMAD_DEBUG
	LOG("Successfully Closed RUNTIME/mapfile.txt");
#endif
//	G_CopyBufferToMap();
//	I_InitBiomes();
	game->I_InitHUD();
}