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
//  src/g_zone.cpp
//  zone memory allocation, to prevent leaks from happening,
//  and to keep better track of allocations. It speeds stuff up
//  by allocating all the memory at the beginning, then returning
//  blocks of memory of desired size, no mallocs, callocs, or
//  reallocs (stdlib.h function calls that is) during the main
//  level loop (unless we're allocating with the reserved zone).
//----------------------------------------------------------

#include "n_shared.h"
#include "g_zone.h"

//
// If the program calls Z_Free on a block that doesn't have the ZONEID,
// meaning that it wasn't allocated via Z_Malloc, the allocater will
// throw an error
//

//
// When archiving and unarchiving save data, the allocater will scan the heap
// to check for heap corruption
//

//
// This allocater is a heavily modified version of z_zone.c and z_zone.h from
// varios DOOM source ports, credits to them and John Carmack/Romero for developing
// the system
//

#define MEM_ALIGN  (sizeof(void *))
#define UNOWNED    ((void **)666)
#define ZONEID     0xa21d49

typedef struct memblock_s
{
#ifdef _NOMAD_DEBUG
	nomadint_t id;
#endif
	nomadubyte_t tag;
	nomaduint_t size;
	void *user;
	
	struct memblock_s* next;
	struct memblock_s* prev;
} memblock_t;

typedef struct
{
	// size of the zone, including size of the header
	nomaduint_t size;
	
	// start/end cap for blocklist
	memblock_t blocklist;
	
	// rover block pointer
	memblock_t *rover;
} memzone_t;

memzone_t* mainzone;

extern "C" void Z_KillHeap(void)
{
	free(mainzone);
}

static const char* Z_TagToStr(nomadubyte_t tag)
{
	switch (tag) {
	case TAG_FREE: return "TAG_FREE";
	case TAG_STATIC: return "TAG_STATIC";
	case TAG_SCOPE: return "TAG_SCOPE";
	case TAG_PURGELEVEL: return "TAG_PURGELEVEL";
	case TAG_CACHE: return "TAG_CACHE";
	};
	return "Unknown tag";
}

//
// Z_Free
//
extern "C" void Z_Free(void *ptr)
{
#ifndef _NOMAD_DEBUG
	LOG_INFO("freeing up zone-allocated block memory");
#endif
	if (!ptr) {
		LOG_WARN("Z_Free pointer given is NULL, aborting.");
		return;
	}
	
	memblock_t* block;
	memblock_t* other;
	
	block = (memblock_t *)((byte *)ptr - sizeof(memblock_t));
	
#ifdef _NOMAD_DEBUG
	if (block->id != ZONEID) {
		LOG_WARN("Z_Free trying to free pointer without ZONEID, aborting.");
		return;
	}
#endif
	
	if (block->tag != TAG_FREE && block->user)
		block->user = (void *)NULL;
	if (block->user > (void *)0x100)
		block->user = (void *)NULL;

	// mark as free
	block->user = (void *)NULL;
	block->tag = TAG_FREE;
#ifdef _NOMAD_DEBUG
	block->id = 0;
#endif
	
	other = block->prev;
	
	if (other->tag == TAG_FREE) {
		// merge with previous free block
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;
		if (block == mainzone->rover) {
			mainzone->rover = other;
		}
		block = other;
	}

	other = block->next;
	if (other->tag == TAG_FREE) {
		// merge the next free block onto the end
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;
		
		if (other == mainzone->rover) {
			mainzone->rover = block;
		}
	}
}

extern "C" void Z_FileDumpHeap()
{
	memblock_t* block;
	const char* filename = "Files/debug/heaplog.log";
	filestream fp(filename, "w");
	fprintf(fp.fp, "zone size:%i   location:%p\n", mainzone->size, (void *)mainzone);
	for (block = mainzone->blocklist.next;; block = block->next) {
		fprintf (fp.fp, "block:%p     size:%7i     user:%p      tag:%3i\n",
			block, block->size, block->user, block->tag);
		if (block->next == &mainzone->blocklist) {
			// all blocks have been hit
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			fprintf(fp.fp, "ERROR: block size doesn't touch next block!\n");
		}
		if (block->next->prev != block) {
			fprintf(fp.fp, "ERROR: next block doesn't have proper back linkage!\n");
		}
		if (!block->user && !block->next->user) {
			fprintf(fp.fp, "ERROR: two consecutive free blocks!\n");
		}
	}
}

#define DEFAULT_SIZE (70*1024*1024) // 70 MiB
#define MIN_SIZE     (50*1024*1024) // 50 MiB

byte *I_ZoneMemory(nomadint_t *size)
{
	nomadint_t current_size = DEFAULT_SIZE;
	const nomadint_t min_size = MIN_SIZE;
	
	byte *ptr = (byte *)malloc(current_size);
	while (ptr == NULL) {
		if (current_size < min_size) {
			N_Error("I_ZoneMemory: failed allocation of zone memory of %i bytes", current_size);
		}
		
		ptr = (byte *)malloc(current_size);
		if (ptr == NULL) {
			current_size--;
		}
	}
	*size = current_size;
	return ptr;
}

static nomadbool_t initialized = false;

extern "C" void Z_Init()
{
	memblock_t* base;
	nomadint_t size;
	mainzone = (memzone_t*)((byte *)malloc(DEFAULT_SIZE));
	if (!mainzone)
		N_Error("Z_Init: memory allocation failed");
	
	if (!initialized)
		atexit(Z_KillHeap);
	
	mainzone->size = DEFAULT_SIZE;
	
	mainzone->blocklist.next = 
	mainzone->blocklist.prev = 
	base = (memblock_t *)((byte *)mainzone+sizeof(memzone_t));
	
	mainzone->blocklist.user = (void *)mainzone;
	mainzone->blocklist.tag = TAG_STATIC;
	mainzone->rover = base;
	
	base->prev = base->next = &mainzone->blocklist;
	base->user = (void *)NULL;
	base->size = mainzone->size - sizeof(memzone_t);
	if (!initialized) {
		printf("Allocated zone from %p -> %p\n", (void *)mainzone, (void *)(mainzone+mainzone->size));
		LOG_INFO("Initialzing Zone Allocation Daemon from addresses %p -> %p", (void *)mainzone, (void *)(mainzone+mainzone->size));
	}
	else
		LOG_INFO("Resizing zone from %p -> %p, new size %iu", (void *)mainzone, (void *)(mainzone+mainzone->size), mainzone->size);
	
	initialized = true;
}


extern "C" void Z_ScanForBlock(void *start, void *end)
{
	memblock_t *block;
	void **mem;
	nomadint_t i, len, tag;
	
	block = mainzone->blocklist.next;
	
	while (block->next != &mainzone->blocklist) {
		tag = block->tag;
		
		if (tag == TAG_STATIC) {
			// scan for pointers on the assumption the pointers are aligned
			// on word boundaries (word size depending on pointer size)
			mem = (void **)( (byte *)block + sizeof(memblock_t) );
			len = (block->size - sizeof(memblock_t)) / sizeof(void *);
			for (i = 0; i < len; ++i) {
				if (start <= mem[i] && mem[i] <= end) {
					LOG_WARN(
						"Z_ScanForBlock: "
						"%p has dangling pointer into freed block "
						"%p (%p -> %p)\n",
					mem, start, &mem[i], mem[i]);
				}
			}
		}
		block = block->next;
	}
}

extern "C" void Z_ChangeTag(void *ptr, nomadubyte_t tag)
{
	memblock_t* base = (memblock_t *)( (byte *)ptr - sizeof(memblock_t) );
#ifdef _NOMAD_DEBUG
	if (base->id != ZONEID)
		LOG_WARN("Z_ChangeTag: block %p has invalid zoneid", (void *)base);
#endif
	LOG_INFO("changing tag of block %p from %s to %s", (void *)base, Z_TagToStr(base->tag), Z_TagToStr(tag));
	base->tag = tag;
}

extern "C" void Z_ClearZone()
{
	LOG_INFO("clearing zone");
	memblock_t*		block;
	
	// set the entire zone to one free block
	mainzone->blocklist.next =
	mainzone->blocklist.prev =
	block = (memblock_t *)( (byte *)mainzone + sizeof(memzone_t) );
	
	mainzone->blocklist.user = (void *)mainzone;
	mainzone->blocklist.tag = TAG_STATIC;
	mainzone->rover = block;
	
	block->prev = block->next = &mainzone->blocklist;
	
	// a free block.
	block->tag = TAG_FREE;
	
	block->size = mainzone->size - sizeof(memzone_t);
}

#define MIN_FRAGMENT 64

// Z_Malloc: garbage collection and zone block allocater that returns a block of free memory
// from within the zone without calling malloc
extern "C" void* Z_Malloc(nomaduint_t size, nomadubyte_t tag, void* user)
{
	NOMAD_ASSERT(size > 0, "Z_Malloc: size was 0 or less");
	NOMAD_ASSERT(tag < NUMTAGS, "Z_Malloc: tag given was invalid: %hu", tag);
	
	memblock_t* rover;
	memblock_t* newblock;
	memblock_t* base;
	memblock_t* start;
	nomadint_t space;
	
	size = (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);
	
	// accounting for header size
	size += sizeof(memblock_t);
	
	base = mainzone->rover;
	
	// checking behind the rover
	if ((base->prev != NULL) && !base->prev->user)
		base = base->prev;

	rover = base;
	start = base->prev;
	
	do {
		if (rover == start) {
			LOG_WARN("zone size wasn't big enough for Z_Malloc size given, resizing zone");
			// allocate a new zone twice as big
			Z_Init();
			
			base = mainzone->rover;
			rover = base;
			start = base->prev;
		}
		// old: (rover->user)
		if (rover->tag != TAG_FREE) {
			if (rover->tag < TAG_PURGELEVEL) {
				// hit a block that can't be purged,
				// so move the base past it
				base = rover = rover->next;
			}
			else {
				// free the rover block (adding to the size of the base)
				// the rover can be the base block
				base = base->prev;
				Z_Free((byte *)rover+sizeof(memblock_t));
				base = base->next;
				rover = base->next;
			}
		}
		else {
			rover = rover->next;
		}
	} while (base->tag != TAG_FREE || base->size < size);
	// old: (base->user || base->size < size)
	
	space = base->size - size;
	
	if (space > MIN_FRAGMENT) {
		newblock = (memblock_t *)((byte *)base + size);
		newblock->size = space;
		newblock->user = NULL;
		newblock->prev = base;
		newblock->next = base->next;
		newblock->next->prev = newblock;
		
		base->next = newblock;
		base->size = size;
	}
	if (user == NULL && tag >= TAG_PURGELEVEL)
		N_Error("Z_Malloc: an owner is required for purgable blocks");
	
	base->user = user;
	base->tag = tag;
	
	void *retn = (void *)( (byte *)base + sizeof(memblock_t) );
	
	if (base->user)
		base->user = retn;

	// next allocation will start looking here
	mainzone->rover = base->next;
	
#ifdef _NOMAD_DEBUG
	base->id = ZONEID;
#endif
	
	return retn;
}

extern "C" void* Z_Realloc(void *user, nomaduint_t nsize)
{
	if (!user)
		N_Error("Z_Realloc: NULL user given");
	
	memblock_t* block = (memblock_t *)((byte *)user - sizeof(memblock_t));
	void *ptr = Z_Malloc(nsize, block->tag, ptr);
	memmove(ptr, user, nsize <= block->size ? nsize : block->size);
	Z_Free(user);
	return ptr;
}

extern "C" void* Z_Calloc(void *user, nomaduint_t nelem, nomaduint_t elemsize, nomadubyte_t tag)
{
	return memset((Z_Malloc)(nelem * elemsize, tag, user), 0, nelem * elemsize);
}

extern "C" void Z_FreeTags(nomadubyte_t lowtag, nomadubyte_t hightag)
{
	nomadint_t numblocks = 0;
	nomadint_t size = 0;
	memblock_t*	block;
    memblock_t*	next;
	
    for (block = mainzone->blocklist.next; 
		block != &mainzone->blocklist; block = next) {
		// get link before freeing
		next = block->next;
		
		// free block?
		if (block->tag == TAG_FREE) {
			continue;
		}
		if (block->tag >= lowtag && block->tag <= hightag) {
			++numblocks;
			size += block->size;
			Z_Free ((byte *)block+sizeof(memblock_t));
		}
	}
	LOG_FREETAGS(lowtag, hightag, numblocks, size);
}

// cleans all zone caches (only blocks from scope to free to unused)
extern "C" void Z_CleanCache(void)
{
	memblock_t* block;
	for (block = mainzone->blocklist.next;
	block != &mainzone->blocklist; block = block->next) {
		if ((byte *)block+block->size != (byte *)block->next) {
			N_Error("Z_CleanCache: block size doesn't touch next block!");
		}
		if (block->next->prev != block) {
			N_Error("Z_CleanCache: next block doesn't have proper back linkage!");
		}
		if (block->tag == TAG_FREE && block->next->tag == TAG_FREE) {
			N_Error("Z_CleanCache: two consecutive free blocks!");
		}
		if (block->tag < TAG_PURGELEVEL) {
			continue;
		}
		else {
			memblock_t* base = block;
			block = block->prev;
			Z_Free((byte*)block+sizeof(memblock_t));
			block = base->next;
		}
	}
}

extern "C" void Z_CheckHeap()
{
	memblock_t* block;

	for (block = mainzone->blocklist.next;; block = block->next) {
		if (block->next == &mainzone->blocklist) {
			// all blocks have been hit
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			N_Error("Z_CheckHeap: block size doesn't touch next block");
		}
		if (block->next->prev != block) {
			N_Error("Z_CheckHeap: next block doesn't have proper back linkage");
		}
		if (block->tag == TAG_FREE && block->next->tag == TAG_FREE) {
			N_Error("Z_CheckHeap: two consecutive free blocks");
		}
	}
	LOG_INFO("heap check successful");
}

extern "C" void Z_ChangeTag2(void *ptr, nomadubyte_t tag, const char *file, nomaduint_t line)
{
	if (ptr == NULL || file == NULL)
		N_Error("Z_ChangeTag2: nullptr given to ptr or file");
	
    memblock_t*	block;
    block = (memblock_t *) ((byte *)ptr - sizeof(memblock_t));
#ifdef _NOMAD_DEBUG
    if (block->id != ZONEID)
        LOG_WARN("%s:%i: Z_ChangeTag: block without a ZONEID!",
                file, line);
#endif
    if (tag >= TAG_PURGELEVEL && !block->user) {
        N_Error("%s: %i: Z_ChangeTag: an owner is required "
                "for purgable blocks", file, line);
    }
	LOG_INFO("changing tag of ptr %p to %s, old tag was %s", ptr, Z_TagToStr(tag), Z_TagToStr(block->tag));
    block->tag = tag;
}

extern "C" void Z_ChangeUser(void *ptr, void *user)
{
	if (ptr == NULL || user == NULL)
		N_Error("Z_ChangeUser: nullptr given to ptr or user");
	
	memblock_t*	block;
	
	block = (memblock_t *) ((byte *)ptr - sizeof(memblock_t));
#ifdef _NOMAD_DEBUG
	if (block->id != ZONEID)
		LOG_WARN("Z_ChangeUser: tried to change user for invalid block!");
#endif
	LOG_INFO("changing user of ptr %p to %p, old user was %p", ptr, user, block->user);
	block->user = user;
	user = ptr;
}

extern "C" nomaduint_t Z_ZoneSize() { return mainzone->size; }