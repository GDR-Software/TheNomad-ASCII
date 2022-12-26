#ifndef _Z_ZONE_
#define _Z_ZONE_

constexpr auto TAG_FREE       = 0;
constexpr auto TAG_STATIC     = 1; // stays allocated for the entire execution time

constexpr auto TAG_PURGELEVEL = 100;
constexpr auto TAG_SCOPE      = 101; // only meant to last a single scope

typedef unsigned char byte;

#ifdef _TESTING
void Z_Init(int size);
#else
void Z_Init(void);
#endif

void* Z_Malloc(int size, int tag, void* user);
void Z_Free(void *ptr);
void Z_ClearZone(void);
#ifndef _TESTING
constexpr unsigned long Z_ZoneSize(void);
#endif
void Z_ChangeUser(void *ptr, void *user);
void Z_FreeTags(int lowtag, int hightag);
void Z_CheckHeap(void);

void Z_DumpHeap(void);
void Z_FileDumpHeap(void);

#endif