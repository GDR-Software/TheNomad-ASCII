#ifndef _Z_ZONE_
#define _Z_ZONE_

#include "n_shared.h"

constexpr auto TAG_FREE       = 0;
constexpr auto TAG_STATIC     = 1; // stays allocated for the entire execution time

constexpr auto TAG_PURGELEVEL = 100;
constexpr auto TAG_SCOPE      = 101; // only meant to last a single scope

typedef unsigned char byte;

#ifdef _TESTING
__CFUNC__ void Z_Init(int size);
#else
__CFUNC__ void Z_Init(void);
#endif

__CFUNC__ void* Z_Malloc(int size, int tag, void* user);
__CFUNC__ void Z_Free(void *ptr);
__CFUNC__ void Z_ClearZone(void);
#ifndef _TESTING
__CFUNC__ constexpr unsigned long Z_ZoneSize(void);
#endif
__CFUNC__ void Z_ChangeUser(void *ptr, void *user);
__CFUNC__ void Z_FreeTags(int lowtag, int hightag);
__CFUNC__ void Z_CheckHeap(void);

__CFUNC__ void Z_DumpHeap(void);
__CFUNC__ void Z_FileDumpHeap(void);

#endif