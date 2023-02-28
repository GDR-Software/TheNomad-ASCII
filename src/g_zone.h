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

#pragma once


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

#define ZONE_ID_MAINZONE 0
#define ZONE_ID_RESERVED 1
#define ZONE_ID_CARDINAL 2

template<typename T, int size, int tag>
class ScopedBlock
{
private:
    int zone_id;
    T *__ptr;
    memzone_t* GetMemzone();
public:
    ScopedBlock(T *ptr, int zoneid)
        : zone_id(zoneid)
    {
        ptr = (T *)Zone_Malloc(size, tag, &ptr, GetMemzone());
        __ptr = ptr;
    }
    ScopedBlock()
    {
    }
    ~ScopedBlock()
    {
        Zone_Free(__ptr, GetMemzone());
    }
    inline T* operator->() { return __ptr; }
    inline T& operator*() { return *__ptr; }
    inline T& operator[](int i) { return __ptr[i]; }
};

extern memzone_t* mainzone; // the main zone that the game uses
extern memzone_t* reserved; // zone where all the unused but stuff found in the bff will be alloced (slf data, extra maps)
extern memzone_t* cardinal; // zone where the cardinal system lies

constexpr auto TAG_FREE       = 0;
constexpr auto TAG_STATIC     = 1; // stays allocated for the entire execution time
constexpr auto TAG_MISSION    = 2;
constexpr auto TAG_PURGELEVEL = 100;
constexpr auto TAG_SCOPE      = 101; // only meant to last a single scope

#ifndef TESTING
static constexpr int heapsize = 50*1024*1024; // allocating 50 mb (mainzone size)
static constexpr int reserved_size = 30*1024*1024; // 30 mb reserved memory
static constexpr int cardinal_size = 25*1024*1024; // 25 mb for cardinal system
#endif

#ifdef TESTING
__CFUNC__ void Z_Init(int size); // only inits mainzone
#else
__CFUNC__ void Z_Init(); // inits all zones
#endif

__CFUNC__ void* Zone_Malloc(int size, int tag, void* user, memzone_t* zone);
__CFUNC__ void* Zone_Realloc(void *user, int nsize, int tag, memzone_t* zone);
__CFUNC__ void* Zone_Calloc(void *user, int nelem, int elemsize, int tag, memzone_t* zone);
__CFUNC__ void Zone_Free(void *ptr, memzone_t* zone);
__CFUNC__ void Zone_ClearZone(memzone_t* zone);
__CFUNC__ void Z_CleanCache(void);
#ifndef TESTING
__CFUNC__ int Zone_ZoneSize(memzone_t *zone);
#endif
__CFUNC__ void Zone_ChangeUser(void *ptr, void *user, memzone_t *zone);
__CFUNC__ void Zone_FreeTags(int lowtag, int hightag, memzone_t *zone);
__CFUNC__ void Zone_CheckHeap(memzone_t *zone);
__CFUNC__ void Zone_ChangeTag2 (void *ptr, int tag, const char *file, int line, memzone_t *zone);
#ifdef TESTING
// only allowed for the mainzone
__CFUNC__ void Z_DumpHeap(void);
#endif
__CFUNC__ void Zone_FileDumpHeap(memzone_t* zone);

// mainzone allocations
#define Z_Malloc(size,tag,ptr) Zone_Malloc(size,tag,ptr,mainzone)
#define Z_Realloc(ptr,nsize,tag) Zone_Realloc(ptr,nsize,tag,mainzone)
#define Z_Calloc(ptr,nelem,elemsize,tag) Zone_Calloc(ptr,nelem,elemsize,tag,mainzone)
#define Z_Free(ptr) Zone_Free(ptr,mainzone)
#define Z_ClearZone() Zone_ClearZone(mainzone)
#ifndef TESTING
#define Z_ZoneSize() Zone_ZoneSize(mainzone)
#endif
#define Z_ChangeUser(ptr,user) Zone_ChangeUser(ptr,user,mainzone)
#define Z_FreeTags(lowtag,hightag) Zone_FreeTags(lowtag,hightag,mainzone)
#define Z_CheckHeap() Zone_CheckHeap(mainzone)
#define Z_ChangeTag2(ptr,tag,file,line) Zone_ChangeTag2(ptr,tag,file,line,mainzone)
#define Z_ChangeTag(ptr,tag) Zone_ChangeTag2(ptr,tag,__FILE__,__LINE__,mainzone)
#define Z_FileDumpHeap() Zone_FileDumpHeap(mainzone)

// reserved allocations
#define R_Malloc(size,tag,ptr) Zone_Malloc(size,tag,ptr,reserved)
#define R_Realloc(ptr,nsize,tag) Zone_Realloc(ptr,nsize,tag,reserved)
#define R_Calloc(ptr,nelem,elemsize) Zone_Calloc(ptr,nelem,elemsize,reserved)
#define R_Free(ptr) Zone_Free(ptr,reserved)
#define R_ClearZone() Zone_ClearZone(reserved)
#ifndef TESTING
#define R_ZoneSize() Zone_ZoneSize(reserved)
#endif
#define R_ChangeUser(ptr,user) Zone_ChangeUser(ptr,user,reserved)
#define R_FreeTags(lowtag,hightag) Zone_FreeTags(lowtag,hightag,reserved)
#define R_CheckHeap() Zone_CheckHeap(reserved)
#define R_ChangeTag2(ptr,tag,file,line) Zone_ChangeTag2(ptr,tag,file,line,reserved)
#define R_ChangeTag(ptr,tag) Zone_ChangeTag2(ptr,tag,__FILE__,__LINE__,reserved)
#define R_FileDumpHeap() Zone_FileDumpHeap(reserved)

// cardinal system allocations
#define C_Malloc(size,tag,ptr) Zone_Malloc(size,tag,ptr,cardinal)
#define C_Realloc(ptr,nsize,tag) Zone_Realloc(ptr,nsize,tag,cardinal)
#define C_Calloc(ptr,nelem,elemsize) Zone_Calloc(ptr,nelem,elemsize,cardinal)
#define C_Free(ptr) Zone_Free(ptr,cardinal)
#define C_ClearZone() Zone_ClearZone(cardinal)
#ifndef TESTING
#define C_ZoneSize() Zone_ZoneSize(cardinal)
#endif
#define C_ChangeUser(ptr,user) Zone_ChangeUser(ptr,user,cardinal)
#define C_FreeTags(lowtag,hightag) Zone_FreeTags(lowtag,hightag,cardinal)
#define C_CheckHeap() Zone_CheckHeap(cardinal)
#define C_ChangeTag2(ptr,tag,file,line) Zone_ChangeTag2(ptr,tag,file,line,cardinal)
#define C_ChangeTag(ptr,tag) Zone_ChangeTag2(ptr,tag,__FILE__,__LINE__,cardinal)
#define C_FileDumpHeap() Zone_FileDumpHeap(cardinal)

#endif
