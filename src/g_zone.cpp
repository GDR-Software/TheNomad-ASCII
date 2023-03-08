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
#define GET_ZONE_ID(zone) \
({int id; \
	if (zone==mainzone) id = 0; \
	else if (zone==reserved) id = 1; \
	else if (zone==cardinal) id = 2; \
id;})

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
#define UNOWNED    ((void *)666)
#define ZONEID     0xa21d49

__CFUNC__ void Z_KillHeap();

memzone_t* mainzone;
memzone_t* reserved;
memzone_t* cardinal;

__CFUNC__ void Z_KillHeap(void)
{
	free(mainzone);
	free(reserved);
}

static void Z_ResizeZone(memzone_t* zone)
{
	int osize = zone->size;
	int nsize = zone->size * 2;
	byte* memory = (byte *)malloc(zone->size);
	memmove(memory, &zone, zone->size);
	free(zone);
	zone = (memzone_t *)((byte *)calloc(sizeof(byte) * nsize, sizeof(byte)));
	if (!zone)
		N_Error("Z_ResizeZone: calloc failed!");
	else {
		memblock_t *base;
		zone->size = nsize;
		memmove(&zone, memory, osize);
	}
}


static unsigned long log_size = 0; // once this reaches ~10 kb, it'll reset
static unsigned long free_size = 0; // same as above
static unsigned long numblocks = 0;

//
// Zone_Free
//
__CFUNC__ void Zone_Free(void *ptr, memzone_t* zone)
{
	NOMAD_ASSERT(ptr, "Zone_Free: ptr given was NULL!");
	memblock_t* block;
	memblock_t* other;
	
	block = (memblock_t *)((byte *)ptr - sizeof(memblock_t));
	
	if (block->id != ZONEID) {
		N_Error("Z_Free: trying to free a pointer without ZONEID!");
	}
	free_size += block->size;
	if (free_size > 10000) {
		LOG_HEAP();
		free_size = 0;
	}
	if (block->tag != TAG_FREE && block->user)
		block->user = (void *)NULL;
	if (block->user > (void *)0x100)
		block->user = (void *)NULL;

	// mark as free
	block->user = (void *)NULL;
	block->tag = TAG_FREE;
	block->id = 0;

	other = block->prev;
	--numblocks;

	if (other->tag == TAG_FREE) {
		// merge with previous free block
		other->size += block->size;
		other->next = block->next;
		other->next->prev = other;
		if (block == zone->rover) {
			zone->rover = other;
		}
		block = other;
	}

	other = block->next;
	if (other->tag == TAG_FREE) {
		// merge the next free block onto the end
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;
		
		if (other == zone->rover) {
			zone->rover = block;
		}
	}
}
#ifdef TESTING
__CFUNC__ void Z_DumpHeap(void)
{
	memblock_t* block;
	fprintf(stdout, "zone size:%i   location:%p\n", mainzone->size, mainzone);
	for (block = mainzone->blocklist.next;; block = block->next) {
		fprintf (stdout, "block:%p     size:%7i     user:%p      tag:%3i\n",
			block, block->size, block->user, block->tag);
		if (block->next == &mainzone->blocklist) {
			// all blocks have been hit
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			N_Error("Z_DumpHeap: block size doesn't touch next block!");
		}
		if (block->next->prev != block) {
			N_Error("Z_DumpHeap: next block doesn't have proper back linkage!");
		}
		if (!block->user && !block->next->user) {
			N_Error("Z_DumpHeap: two consecutive free blocks!");
		}
	}
}
#endif

__CFUNC__ void Zone_FileDumpHeap(memzone_t* zone)
{
	memblock_t* block;
	std::string filename = "Files/debug/heaplog_"+std::to_string(GET_ZONE_ID(zone))+".log";
	FILE* fp = fopen(filename.c_str(), "w");
	NOMAD_ASSERT(fp, "Zone_FileDumpHeap: failed to open file!");
	if (!fp)
		return;
	fprintf(fp, "zone size:%i   location:%p\n", zone->size, (void *)zone);
	for (block = zone->blocklist.next;; block = block->next) {
		fprintf (fp, "block:%p     size:%7i     user:%p      tag:%3i\n",
			block, block->size, block->user, block->tag);
		if (block->next == &zone->blocklist) {
			// all blocks have been hit
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			fprintf(fp, "ERROR: block size doesn't touch next block!\n");
		}
		if (block->next->prev != block) {
			fprintf(fp, "ERROR: next block doesn't have proper back linkage!\n");
		}
		if (!block->user && !block->next->user) {
			fprintf(fp, "ERROR: two consecutive free blocks!\n");
		}
	}
	fclose(fp);
}

template<typename T, int size, int tag>
memzone_t* ScopedBlock<T, size, tag>::GetMemzone() {
    switch (zone_id) {
    case ZONE_ID_MAINZONE: return mainzone; break;
    case ZONE_ID_RESERVED: return reserved; break;
    case ZONE_ID_CARDINAL: return cardinal; break;
    };
    LOG_WARN("zone_id for ScopedBlock is invalid enum %i! killing object", zone_id);
    this->~ScopedBlock();
	return (memzone_t*)NULL;
}

#ifdef TESTING
__CFUNC__ void Z_Init(int size)
{
	memblock_t* base;

	// account for header size
	size += sizeof(memzone_t);
	mainzone = (memzone_t *)((byte *)calloc(size, sizeof(byte)));
	if (!mainzone) {
		fprintf(stderr, "Z_Init: malloc failed!\n");
		exit(-1);
	}
	atexit(Z_KillHeap);
	mainzone->size = size;

	mainzone->blocklist.next = 
	mainzone->blocklist.prev = 
	base = (memblock_t *)((byte *)mainzone+sizeof(memzone_t));

	mainzone->blocklist.user = (void *)mainzone;
	mainzone->blocklist.tag = TAG_STATIC;
	mainzone->rover = base;

	base->prev = base->next = &mainzone->blocklist;
	base->user = (void *)NULL;
	base->size = mainzone->size - sizeof(memzone_t);
	printf("Allocated Zone From %p -> %p\n", (void *)mainzone, (void *)(mainzone+mainzone->size));
}
#else
__CFUNC__ void Z_Init()
{
	memblock_t* base;
	mainzone = (memzone_t *)((byte *)calloc(heapsize, sizeof(byte)));
	if (!mainzone)
		N_Error("Z_Init: calloc failed!");
	reserved = (memzone_t *)((byte *)calloc(reserved_size, sizeof(byte)));
	if (!reserved) {
		free(mainzone);
		N_Error("Z_Init: calloc failed!");		
	}
	
	PTR_CHECK(NULL_CHECK, mainzone);
	PTR_CHECK(NULL_CHECK, reserved);
	atexit(Z_KillHeap);
	mainzone->size = heapsize;
	reserved->size = reserved_size;
	
	mainzone->blocklist.next = 
	mainzone->blocklist.prev = 
	base = (memblock_t *)((byte *)mainzone+sizeof(memzone_t));

	mainzone->blocklist.user = (void *)mainzone;
	mainzone->blocklist.tag = TAG_STATIC;
	mainzone->rover = base;
	
	base->prev = base->next = &mainzone->blocklist;
	base->user = (void *)NULL;
	base->size = mainzone->size - sizeof(memzone_t);

	reserved->blocklist.next =
	reserved->blocklist.prev =
	base = (memblock_t *)((byte *)reserved+sizeof(memzone_t));
	reserved->blocklist.user = (void *)reserved;
	reserved->blocklist.tag = TAG_STATIC;
	reserved->rover = base;

	base->prev = base->next = &reserved->blocklist;
	base->user = (void *)NULL;
	base->size = reserved->size - sizeof(memzone_t);
}
#endif

__CFUNC__ void Zone_ClearZone(memzone_t* zone)
{
	LOG_INFO("clearing zone");
	memblock_t*		block;
	
	// set the entire zone to one free block
	zone->blocklist.next =
	zone->blocklist.prev =
	block = (memblock_t *)( (byte *)zone + sizeof(memzone_t) );
	
	zone->blocklist.user = (void *)zone;
	zone->blocklist.tag = TAG_STATIC;
	zone->rover = block;
	
	block->prev = block->next = &zone->blocklist;
	
	// a free block.
	block->tag = TAG_FREE;
	
	block->size = zone->size - sizeof(memzone_t);
}

// Z_Malloc: garbage collection and zone block allocater that returns a block of free memory
// from within the zone without calling malloc
__CFUNC__ void* Zone_Malloc(int size, int tag, void* user, memzone_t* zone)
{
	NOMAD_ASSERT(size > 0, "Zone_Malloc: size was 0");
	NOMAD_ASSERT(tag > -1, "Zone_Malloc: tag given was invalid: %i", tag);
	if (!user) {
		LOG_WARN("NULL user pointer given to Z_Malloc, returning NULL");
		return (void *)NULL;
	}
	memblock_t* rover;
	memblock_t* userblock;
	memblock_t* base;
	memblock_t* start;
	int space;

	size = (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);
	
	// accounting for header size
	size += sizeof(memblock_t);
	
	base = zone->rover;
	
	// checking behind the rover
	if (!base->prev->user)
		base = base->prev;
	
	rover = base;
	start = base->prev;
	
	do {
		if (rover == start) {
			if (zone == reserved) {
				Z_ResizeZone(zone);
				base = zone->rover;
				rover = base;
				start = base->prev;
			}
			else if (zone == mainzone) {
				N_Error("Z_Malloc: failed allocation of %i bytes because zone isn't big enough!", size);
			}
		}
		if (rover->user) {
			if (rover->tag < TAG_PURGELEVEL) {
				// hit a block that can't be purged, so move the base past it
				base = rover = rover->next;
			}
			else {
				// free the rover block (adding to the size of the base)
				// the rover can be the base block
				base = base->prev;
				Zone_Free((byte *)rover+sizeof(memblock_t), zone);
				base = base->next;
				rover = base->next;
			}
		}
		else {
			rover = rover->next;
		}
	} while (base->user || base->size < size);
	
	space = base->size - size;
	
	if (space > 64) {
		userblock = (memblock_t *)((byte *)base+size);
		NOMAD_ASSERT(userblock, "Zone_Malloc: userblock was NULL!");
		userblock->size = space;
		userblock->user = (void *)NULL;
		userblock->tag = TAG_FREE;
		userblock->prev = base;
		userblock->next = base->next;
		userblock->next->prev = userblock;
	
		base->next = userblock;
		base->size = size;
	}
	if (user) {
		// mark as an in use block
		base->user = user;
		user = (void *)((byte *)base+sizeof(memblock_t));
	}
	else {
		if (tag >= TAG_PURGELEVEL) {
			N_Error("Z_Malloc: an owner is required for purgable blocks!");
			return (void *)NULL;
		}
		// mark as in used, but unowned
		base->user = UNOWNED;
		LOG_WARN("block at %p is used, but currently unowned", base);
	}
	base->tag = tag;

	// next allocation will start looking here
	zone->rover = base->next;
	base->id = ZONEID;
	log_size += base->size;
	if (log_size > 10000) {
		LOG_HEAP();
		log_size = 0;
	}
	++numblocks;
	return (void *)((byte *)base+sizeof(memblock_t));
}

__CFUNC__ void* Zone_Realloc(void *user, int nsize, int tag, memzone_t* zone)
{
	PTR_CHECK(NULL_CHECK, user);
	void *ptr = Zone_Malloc(nsize, tag, ptr, zone);
	memblock_t* block = (memblock_t *)((byte *)user - sizeof(memblock_t));
	free_size += block->size;
	log_size += nsize;
	++numblocks;
	if (log_size > 10000) {
		LOG_HEAP();
		log_size = 0;
	}
	memcpy(ptr, user, nsize <= block->size ? nsize : block->size);
	Zone_Free(user, zone);
	return ptr;
}

__CFUNC__ void* Zone_Calloc(void *user, int nelem, int elemsize, int tag, memzone_t* zone)
{
	PTR_CHECK(NULL_CHECK, user);
	log_size += nelem * elemsize;
	++numblocks;
	if (log_size > 10000) {
		LOG_HEAP();
		log_size = 0;
	}
#if 0
	return (nelem*=elemsize) ? memset((Zone_Malloc(nelem, tag, user, zone)), 0, nelem) : (void *)NULL;
#elif 1
	return (nelem*=elemsize) ? memset((Zone_Malloc)(nelem, tag, user, zone), 0, nelem) : (void *)NULL;
#endif
}

__CFUNC__ void Zone_FreeTags(int lowtag, int hightag, memzone_t* zone)
{
	int numblocks = 0;
	int size = 0;
	memblock_t*	block;
    memblock_t*	next;
	
    for (block = zone->blocklist.next; 
		block != &zone->blocklist; block = next) {
		// get link before freeing
		next = block->next;
		
		// free block?
		if (block->tag == TAG_FREE) {
			continue;
		}
		if (block->tag >= lowtag && block->tag <= hightag) {
			++numblocks;
			size += block->size;
			Zone_Free ((byte *)block+sizeof(memblock_t), zone);
		}
	}
	LOG_FREETAGS(lowtag, hightag, numblocks, size);
}

// cleans all zone caches (only blocks from scope to free to unused)
__CFUNC__ void Z_CleanCache(void)
{
	memblock_t* block;
	for (block = mainzone->blocklist.next;; block = block->next) {
		if (block->next == &mainzone->blocklist) {
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			N_Error("Z_CleanCache(mainzone): block size doesn't touch next block!");
		}
		if (block->next->prev != block) {
			N_Error("Z_CleanCache(mainzone): next block doesn't have proper back linkage!");
		}
		if (!block->user && !block->next->user) {
			N_Error("Z_CleanCache(mainzone): two consecutive free blocks!");
		}
		switch (block->tag) {
		case TAG_PURGELEVEL:
		case TAG_SCOPE:
			Zone_Free((byte *)block+sizeof(memblock_t), mainzone);
			break;
		default:
			break;
		};
	}
	for (block = reserved->blocklist.next;; block = block->next) {
		if (block->next == &reserved->blocklist) {
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			N_Error("Z_CleanCache(reserved): block size doesn't touch next block!");
		}
		if (block->next->prev != block) {
			N_Error("Z_CleanCache(reserved): next block doesn't have proper back linkage!");
		}
		if (!block->user && !block->next->user) {
			N_Error("Z_CleanCache(reserved): two consecutive free blocks!");
		}
		switch (block->tag) {
		case TAG_PURGELEVEL:
		case TAG_SCOPE:
			Zone_Free((byte *)block+sizeof(memblock_t), reserved);
			break;
		default:
			break;
		};
	}
}

__CFUNC__ void Zone_CheckHeap(memzone_t* zone)
{
	memblock_t* block;

	for (block = zone->blocklist.next;; block = block->next) {
		if (block->next == &zone->blocklist) {
			// all blocks have been hit
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			N_Error("Z_CheckHeap: block size doesn't touch next block!");
		}
		if (block->next->prev != block) {
			N_Error("Z_CheckHeap: next block doesn't have proper back linkage!");
		}
		if (!block->user && !block->next->user) {
			N_Error("Z_CheckHeap: two consecutive free blocks!");
		}
	}
	LOG_INFO("heap check successful");
}
__CFUNC__ void Zone_ChangeTag2(void *ptr, int tag, const char *file, int line, memzone_t* zone)
{
	PTR_CHECK(NULL_CHECK, ptr);
	PTR_CHECK(NULL_CHECK, file);
    memblock_t*	block;
    block = (memblock_t *) ((byte *)ptr - sizeof(memblock_t));
    if (block->id != ZONEID)
        N_Error("%s:%i: Z_ChangeTag: block without a ZONEID!",
                file, line);

    if (tag >= TAG_PURGELEVEL && !block->user)
        N_Error("%s: %i: Z_ChangeTag: an owner is required "
                "for purgable blocks", file, line);

	LOG_INFO("changing tag of ptr %p to %i, old tag was %i", ptr, tag, block->tag);
    block->tag = tag;
}

__CFUNC__ void Zone_ChangeUser(void *ptr, void *user, memzone_t* zone)
{
	PTR_CHECK(NULL_CHECK, ptr);
	PTR_CHECK(NULL_CHECK, user);
	memblock_t*	block;
	
	block = (memblock_t *) ((byte *)ptr - sizeof(memblock_t));
	if (block->id != ZONEID) {
		N_Error("Z_ChangeUser: tried to change user for invalid block!");
		return;
	}
	LOG_INFO("changing user of ptr %p to %p, old user was %p", ptr, user, block->user);
	block->user = user;
	user = ptr;
}

#ifndef TESTING
__CFUNC__ int Zone_ZoneSize(memzone_t* zone)
{
	if (zone == mainzone) return heapsize;
	else if (zone == reserved) return reserved->size;
	
	// invalid zone
	LOG_WARN("invalid zone has been given!");
	return 0;
}
#endif
