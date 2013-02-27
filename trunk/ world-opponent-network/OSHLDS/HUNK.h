/* Rather than malloc and free every which way, the HUNK carves out
// a huge chunk of RAM from the get go and doles it out as needed.
// This is nice in that it prevents malloc failures later on and
// eased the burden on those more limited systems.  This is BAD because
// their memory functions aren't bug free (QW had a few changes), and
// because while everyone knows how malloc works, the same can't be said
// for hunks.
*/

#ifndef OSHLDS_ENGINE_HEADER
#define OSHLDS_ENGINE_HEADER

#include "endianin.h"
#include "Q_memdef.h"
#include "ModCalls.h"

/*** structures ***/

//doesn't belong here, but it works pretty well, and I don't have a better spot
typedef struct cache_user_s {
   byte *data;
} cache_user_t;


/*** PUBLIC FUNCTIONS ***/

//mem
void Memory_Init(void *, unsigned int);

uint32 Hunk_LowMark(void);
uint32 Hunk_HighMark(void);
void Hunk_FreeToLowMark(unsigned int mark);
void Hunk_FreeToHighMark(unsigned int mark);

void * Hunk_Alloc(unsigned int);
void * Hunk_AllocName(unsigned int, const char *);
void * Hunk_TempAlloc(unsigned int);

void * Cache_Alloc(cache_user_t *, unsigned int, const char *);
HLDS_DLLEXPORT void * Cache_Check(cache_user_t *);
void Cache_Free(cache_user_t *);

void * Z_Malloc(unsigned int);
void Z_Free(void *);

#endif
