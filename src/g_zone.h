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
//  src/g_zone.h
//----------------------------------------------------------
#ifndef _Z_ZONE_
#define _Z_ZONE_

#include "n_shared.h"

constexpr auto TAG_FREE       = 0;
constexpr auto TAG_STATIC     = 1; // stays allocated for the entire execution time
constexpr auto TAG_MISSION    = 2;

constexpr auto TAG_PURGELEVEL = 100;
constexpr auto TAG_SCOPE      = 101; // only meant to last a single scope

#ifndef _TESTING
static constexpr int heapsize = 10000000; // allocating 3 mb
#endif

#ifdef TESTING
__CFUNC__ void Z_Init(int size);
#else
__CFUNC__ void Z_Init(uint64_t& start, uint64_t& end);
#endif

// size: 40 bytes...?
typedef struct memblock_s
{
	int size;
	int tag;
	int id;
	void* user;
	
	struct memblock_s* next;
	struct memblock_s* prev;
} memblock_t;

typedef struct
{
	// total bytes allocated, including the sizeof memzone_t
	int size;

	// start/end cap for the linked list of blocks
	memblock_t blocklist;
	// the block pointer
	memblock_t* rover;
} memzone_t;

extern memzone_t* mainzone;

__CFUNC__ void* Z_Malloc(int size, int tag, void* user);
__CFUNC__ void Z_Free(void *ptr);
__CFUNC__ void Z_ClearZone(void);
#ifndef TESTING
__CFUNC__ constexpr unsigned long Z_ZoneSize(void);
#endif
__CFUNC__ void Z_ChangeUser(void *ptr, void *user);
__CFUNC__ void Z_FreeTags(int lowtag, int hightag);
__CFUNC__ void Z_CheckHeap(void);
__CFUNC__ void Z_ChangeTag2 (void *ptr, int tag, const char *file, int line);
#ifdef TESTING
__CFUNC__ void Z_DumpHeap(void);
#endif
__CFUNC__ void Z_FileDumpHeap(void);

#define Z_ChangeTag(p,t)                                       \
    Z_ChangeTag2((p), (t), __FILE__, __LINE__)

#endif