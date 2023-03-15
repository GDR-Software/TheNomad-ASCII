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
// DESCRIPTION: src/g_zone.h
//  header for the zone allocation daemon
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


extern memzone_t* mainzone; // the main zone that the game uses
extern memzone_t* reserved; // zone where all the unused but stuff found in the bff will be alloced (slf data, extra maps)
extern memzone_t* cardinal; // zone where the cardinal system lies

constexpr auto TAG_FREE       = 0;
constexpr auto TAG_STATIC     = 1; // stays allocated for the entire execution time
constexpr auto TAG_MISSION    = 2;
constexpr auto TAG_PURGELEVEL = 100;
constexpr auto TAG_SCOPE      = 101; // only meant to last a single scope

#ifndef TESTING
static constexpr int heapsize = 62*1024*1024; // allocating 62 mb (mainzone size)
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

#define GET_ZONE_ID(zone) \
({int id; \
	if (zone==mainzone) id = 0; \
	else if (zone==reserved) id = 1; \
	else if (zone==cardinal) id = 2; \
id;})

// mainzone allocations
#if 0
#define LOG_BLOCK(ptr,zone)                               \
{                                                         \
	fprintf(LOGGER_OUTFILE,                               \
	"[Zone Daemon Log]\n"                                 \
	"\tzone id               => %i\n"                     \
	"\tlog type              => BLOCK_LOG\n"              \
	"\tblock name            => %s\n"                     \
	"\tblock bytes alloc'd   => %i\n"                     \
	"\tblock tag             => %i\n",                    \
	GET_ZONE_ID(zone),#ptr,                               \
	((memblock_t*)((byte*)ptr-sizeof(memblock_t)))->size, \
	((memblock_t*)((byte*)ptr-sizeof(memblock_t)))->tag); \
}
#else
#define LOG_BLOCK(ptr,zone)
#endif

#define Z_FileDumpHeap() Zone_FileDumpHeap(mainzone)
#define Z_ClearZone() Zone_ClearZone(mainzone)
#ifndef TESTING
#define Z_ZoneSize() Zone_ZoneSize(mainzone)
#endif
#define Z_ChangeTag2(ptr,tag,file,line) Zone_ChangeTag2(ptr,tag,file,line,mainzone)
#ifndef _NOMAD_DEBUG
#define Z_Malloc(size,tag,ptr) Zone_Malloc(size,tag,ptr,mainzone)
#define Z_Realloc(ptr,nsize,tag) Zone_Realloc(ptr,nsize,tag,mainzone)
#define Z_Calloc(ptr,nelem,elemsize,tag) Zone_Calloc(ptr,nelem,elemsize,tag,mainzone)
#define Z_Free(ptr) Zone_Free(ptr,mainzone)
#define Z_ChangeUser(ptr,user) Zone_ChangeUser(ptr,user,mainzone)
#define Z_FreeTags(lowtag,hightag) Zone_FreeTags(lowtag,hightag,mainzone)
#define Z_CheckHeap() Zone_CheckHeap(mainzone)
#define Z_ChangeTag(ptr,tag) Zone_ChangeTag2(ptr,tag,__FILE__,__LINE__,mainzone)
#else

#define Z_Malloc(size,tag,ptr) \
	Zone_Malloc(size,tag,ptr,mainzone); LOG_DEBUG("Z_Malloc called from %s:%s:%u, name: %s",__FILE__,__func__,__LINE__,#ptr)
#define Z_Free(ptr) \
	Zone_Free(ptr,mainzone); LOG_DEBUG("Z_Free called from %s:%s:%u, name: %s",__FILE__,__func__,__LINE__,#ptr)
#define Z_ChangeTag(ptr,tag) \
	Zone_ChangeTag2(ptr,tag,__FILE__,__LINE__,mainzone); LOG_DEBUG("Z_ChangeTag called from %s:%s:%u, name: %s, newtag: %s",__FILE__,__func__,__LINE__,#ptr,#tag)
#define Z_ChangeUser(ptr,user) \
	Zone_ChangeUser(ptr,user); LOG_DEBUG("Z_ChangeUser called from %s:%s:%u, name %s, new user: %s",__FILE__,__func__,__LINE__,#ptr,#user)
#define Z_FreeTags(lowtag,hightag) \
	Zone_FreeTags(lowtag,hightag); LOG_DEBUG("Z_FreeTags called from %s:%s:%u, lowtag: %s, hightag %s",__FILE__,__func__,__LINUX__,#lowtag,#hightag)
#endif

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

// the usual operator allocaters, linker was giving me hell, so I just grabbed the stuff
// from the source code, modified it a bit, the slapped it in here

#if 0
inline void *operator new(unsigned long size)
{
    if (size == 0)
        size = 1;

    void *ptr;
    while ((ptr = malloc(size)) == NULL) {
        std::new_handler nh = std::get_new_handler();
        if (nh)
            nh();
        else
            N_Error("memory allocation failed");
    }
    return ptr;
}
inline void operator delete(void *ptr)
{
    if (!ptr) {
        LOG_WARN("gave NULL pointer to operator ::delete, aborting");
        return;
    }
    free(ptr);
}
inline void *operator new[](unsigned long size)
{
    return ::operator new(size);
}
inline void operator delete[](void *ptr)
{
    ::operator delete(ptr);
}
inline void operator delete[](void *ptr, unsigned long)
{
    ::operator delete[](ptr);
}
#endif

template<typename T>
struct linked_list
{
public:
	struct linked_list_element
	{
		void *element;
		linked_list_element* next;
		linked_list_element* prev;
	};
	typedef linked_list_element* iterator;
private:
	linked_list_element *ptr_list;
	linked_list<T>::iterator end;
	linked_list<T>::iterator begin;
public:
	linked_list() {
		ptr_list = (linked_list_element*)Z_Malloc(sizeof(linked_list_element), TAG_STATIC, &ptr_list);
		end = (linked_list_element*)Z_Malloc(sizeof(linked_list_element), TAG_STATIC, &end);
		ptr_list->next = end;
		end->next = NULL;
		end->next = NULL;
		ptr_list->prev = NULL;
	}
	~linked_list() {
		for (linked_list_element* it = ptr_list;; it = it->next) {
			if (it == end) {
				break;
			}
			Z_Free(it);
		}
		Z_Free(end);
	}
	inline nomadsize_t size() const {
		nomadsize_t len = 0;
		for (linked_list_element *it = ptr_list; it->next != end; it = it->next) {
			++len;
		}
		return len;
	}
	inline void push_back(T& i) {
		linked_list_element* it;
		for (it = ptr_list;; it = it->next) {
			if (it->next == end) {
				break;
			}
		}
		it->next = (linked_list_element*)Z_Malloc(sizeof(linked_list_element), TAG_STATIC, &it->next);
		it->next->element = (void *)&i;
		it->next->next = end;
		it->next->prev = it;
	}
	inline T& operator[](nomadsize_t i) {
#ifdef _NOMAD_DEBUG
		NOMAD_ASSERT(i > size(), "attempting to access element greater than the size of linked_list");
#endif
		linked_list_element *it;
		nomadsize_t index = 0;
		for (it = ptr_list; index < i; it = it->next, ++index) {
			if (it == NULL) {
				break;
			}
		}
		return *static_cast<T*>(it->element);
	}
	inline void pop_back() {
		linked_list_element *back;
		for (back = ptr_list; back->next != end; back = back->next);
		back->prev->next = end;
		Z_Free(back);
	}
	inline void pop_front() {
		linked_list_element *front = ptr_list->next;
		Z_Free(ptr_list);
		ptr_list = front;
	}
//	inline void erase(nomadsize_t index) {
//		
//	}
	inline void erase(linked_list_element* ptr) {
		if (ptr->next == end) {
			pop_back();
		}
		else {
			ptr->prev->next = ptr->next;
			ptr->next->prev = ptr->prev;
			Z_Free(ptr);
		}
	}
	inline void clear() {
		for (linked_list_element *it = ptr_list; it != end; it = it->next) {
			Z_Free(it);
		}
	}
	inline linked_list<T>::iterator front() {
		return ptr_list;
	}
};

#endif
