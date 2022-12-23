#ifndef _Z_ZONE_
#define _Z_ZONE_

#define TAG_FREE       0
#define TAG_STATIC     1

#define TAG_PURGELEVEL 100

typedef unsigned char byte;

#ifdef _TESTING
void Z_Init(int size);
#else
void Z_Init(void);
#endif

void* Z_Malloc(int size, int tag, void* user);
void Z_Free(void *ptr);
void Z_ClearZone(void);
constexpr unsigned long Z_ZoneSize(void);
void Z_ChangeUser(void *ptr, void *user);
void Z_FreeTags(int lowtag, int hightag);
void Z_CheckHeap(void);

void Z_DumpHeap(void);
void Z_FileDumpHeap(void);

#endif