#include <stdlib.h>
#include <stdio.h>

#include "g_zone.h"

#define UNOWNED ((void*)666)
static constexpr auto ZONEID  = 0xa21d49;
static constexpr auto MEM_ALIGN = (sizeof(void *));

static void Z_KillHeap();

// size: 60 bytes
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

static constexpr int heapsize = 1126136 + sizeof(memzone_t);

memzone_t* mainzone;

static void Z_KillHeap(void)
{
	free(mainzone);
}

void Z_Free(void *ptr)
{
	memblock_t* block;
	memblock_t* other;
	
	block = (memblock_t *)((byte *)ptr - sizeof(memblock_t));
	
	if (block->id != ZONEID) {
		fprintf(stderr, "Z_Free: trying to free a pointer without ZONEID!\n");
		exit(-1);
	}
	if (block->user > (void **)0x100) {
		block->user = nullptr;
	}

	// mark as free
	block->user = nullptr;
	block->tag = TAG_FREE;
	block->id = 0;

	other = block->prev;

	if (!other->user) {
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
	if (!other->user) {
		// merge the next free block onto the end
		block->size += other->size;
		block->next = other->next;
		block->next->prev = block;
		
		if (other == mainzone->rover) {
			mainzone->rover = block;
		}
	}
}

void Z_DumpHeap(void)
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
			fprintf(stderr, "ERROR: block size doesn't touch next block!\n");
		}
		if (block->next->prev != block) {
			fprintf(stderr, "ERROR: next block doesn't have proper back linkage!\n");
		}
		if (!block->user && !block->next->user) {
			fprintf(stderr, "ERROR: two consecutive free blocks!\n");
		}
	}
}

void Z_FileDumpHeap(void)
{
	memblock_t* block;
	FILE* fp = fopen("heaplog.txt", "w");
	fprintf(fp, "zone size:%i   location:%p\n", mainzone->size, mainzone);
	for (block = mainzone->blocklist.next;; block = block->next) {
		fprintf (fp, "block:%p     size:%7i     user:%p      tag:%3i\n",
			block, block->size, block->user, block->tag);
		if (block->next == &mainzone->blocklist) {
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

#ifdef _TESTING
void Z_Init(int size)
{
	memblock_t* base;

	// account for header size
	size += sizeof(memzone_t);
	puts("Z_Init(): Allocating Zone Memory...");
	mainzone = (memzone_t *)((byte *)malloc(size));
	if (!mainzone) {
		fprintf(stderr, "Z_Init: malloc failed!\n");
		exit(-1);
	}
	mainzone->size = size;

	mainzone->blocklist.next = 
	mainzone->blocklist.prev = 
	base = (memblock_t *)((byte *)mainzone+sizeof(memzone_t));

	mainzone->blocklist.user = (void *)mainzone;
	mainzone->blocklist.tag = TAG_STATIC;
	mainzone->rover = base;

	base->prev = base->next = &mainzone->blocklist;
	base->user = NULL;
	base->size = mainzone->size - sizeof(memzone_t);
	atexit(Z_KillHeap);
	printf("Allocated Zone From %p -> %p\n", mainzone, (mainzone+mainzone->size));
}
#else
void Z_Init(void)
{
	memblock_t* base;
	puts("Z_Init(): Allocating Zone Memory...");
	
	mainzone = (memzone_t *)((byte *)malloc(heapsize));
	if (!mainzone) {
		fprintf(stderr, "Z_Init: malloc failed!\n");
		exit(-1);
	}
	mainzone->size = heapsize;
	
	mainzone->blocklist.next = 
	mainzone->blocklist.prev = 
	base = (memblock_t *)((byte *)mainzone+sizeof(memzone_t));

	mainzone->blocklist.user = (void *)mainzone;
	mainzone->blocklist.tag = TAG_STATIC;
	mainzone->rover = base;
	
	base->prev = base->next = &mainzone->blocklist;
	base->user = NULL;
	base->size = mainzone->size - sizeof(memzone_t);
	atexit(Z_KillHeap);
	printf("Allocated Zone From %p -> %p\n", mainzone, (mainzone+mainzone->size));
}
#endif

void Z_ClearZone(void)
{
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

void* Z_Malloc(int size, int tag, void* user)
{
	memblock_t* rover;
	memblock_t* userblock;
	memblock_t* base;
	memblock_t* start;
	int space;

	size = (size + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);
	
	// accounting for header size
	size += sizeof(memblock_t);
	
	base = mainzone->rover;
	
	// checking behind the rover
	if (!base->prev->user) {
		base = base->prev;
	}
	
	rover = base;
	start = base->prev;
	
	do {
		if (rover == start) {
			fprintf(stderr, "Z_Malloc: failed on allocation of %i bytes because zone isn't\n"
				"big enough! zone size: %i\n", size, mainzone->size);
			exit(-1);
			return NULL;
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
				Z_Free((byte *)rover+sizeof(memblock_t));
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
		userblock->size = space;
		userblock->user = nullptr;
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
			fprintf(stderr, "Z_Malloc: an owner is required for purgable blocks\n");
			exit(-1);
			return NULL;
		}
		// mark as in used, but unowned
		base->user = UNOWNED;
	}
	base->tag = tag;

	// next allocation will start looking here
	mainzone->rover = base->next;
	base->id = ZONEID;

	return (void *)((byte *)base+sizeof(memblock_t));
}

void Z_FreeTags(int lowtag, int hightag)
{
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
			Z_Free ((byte *)block+sizeof(memblock_t));
		}
	}
}

void Z_CheckHeap(void)
{
	memblock_t* block;

	for (block = mainzone->blocklist.next;; block = block->next) {
		if (block->next == &mainzone->blocklist) {
			// all blocks have been hit
			break;
		}
		if ((byte *)block+block->size != (byte *)block->next) {
			fprintf(stderr, "ERROR: block size doesn't touch next block!\n");
			exit(-1);
		}
		if (block->next->prev != block) {
			fprintf(stderr, "ERROR: next block doesn't have proper back linkage!\n");
			exit(-1);
		}
		if (!block->user && !block->next->user) {
			fprintf(stderr, "ERROR: two consecutive free blocks!\n");
			exit(-1);
		}
	}
}

void Z_ChangeUser(void *ptr, void *user)
{
	memblock_t*	block;
	
	block = (memblock_t *) ((byte *)ptr - sizeof(memblock_t));
	if (block->id != ZONEID) {
		fprintf(stderr, "Z_ChangeUser: tried to change user for invalid block!\n");
		exit(-1);
		return;
	}
	block->user = user;
	user = ptr;
}

constexpr unsigned long Z_ZoneSize(void)
{
	return heapsize;
}