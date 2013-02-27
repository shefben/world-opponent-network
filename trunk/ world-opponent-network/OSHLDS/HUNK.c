#include "HUNK.h"

#include "report.h"
#include "common.h"
#include "cmd.h"

/*** DEFINES ***/

#define HUNK_ARRAYNAMELENGTH 64
#define HUNK_SENTINAL 0x1df001ed
#define ZONEID 0x1d4a11
#define ZONESENTINAL 0xdeadbeaf
#define MINFRAGMENT 64


/*** SRUCTS ***/

typedef struct hunk_s {
   uint32 sentinal;
   unsigned int size;
   char name[HUNK_ARRAYNAMELENGTH]; //it's definitely not 8
} hunk_t;
typedef struct cache_system_s {
   unsigned int size;      //4
   cache_user_t *user; //8
   char name[64]; //72-8=64
   struct cache_system_s *prev, *next, *lru_prev, *lru_next;
   int locked;
   //The actual allocated data follows this struct.
} cache_system_t;
typedef struct memblock_s {
   unsigned int size;         // including the header and possibly tiny fragments
   int tag;         // a tag of 0 is a free block
   uint32 id;            // should be ZONEID
   struct memblock_s      *next, *prev;
   int pad;         // pad to 64 bit boundary
} memblock_t;
typedef struct memzone_s {
   unsigned int size;      // total bytes malloced, including header
   memblock_t blocklist;      // start / end cap for linked list
   memblock_t *rover;
} memzone_t;


/*** GLOBALS ***/

static byte * sg_hunk_base = NULL;
static uint32 sg_hunk_size = 0;
static uint32 sg_hunk_low_used = 0;
static uint32 sg_hunk_high_used = 0;
static BOOL sg_hunk_tempactive = 0;
static int sg_hunk_tempmark = 0;

static memzone_t * sg_mainzone = NULL;
static cache_system_t sg_cache_head = { 0, NULL, "", NULL, NULL, NULL, NULL, 0 };


/*** FUNCTIONS ***/

//hunk
void *Hunk_Alloc(unsigned int);

void *Hunk_HighAllocName(unsigned int, char*);
void Hunk_Check(void);
void Hunk_Print(/*qboolean*/);


//cache
void Cache_Init();
cache_system_t * Cache_TryAlloc(unsigned int, qboolean);
void Cache_MakeLRU(cache_system_t *);
void Cache_UnlinkLRU(cache_system_t *);

void Cache_Move(cache_system_t *);
void Cache_Flush();

void Cache_FreeLow(unsigned int);
void Cache_FreeHigh(unsigned int);


//Z
void Z_ClearZone(memzone_t *, unsigned int);
void *Z_TagMalloc(unsigned int, int);
void Z_Print(/*memzone_t **/);
void Z_CheckHeap(void);



/* DESCRIPTION: Memory_Init
// LOCATION: zone.c
// PATH: Load__7CEnginebPcT2->Sys_InitGame__FPcT0Pvi->Host_Init->Memory_Init
//
// Inits the big chunk of memory used for now and ever.  Appears to follow
// QW's example, with NOZONE undefined
*/
void Memory_Init(void *buf, unsigned int bufsize) {


   unsigned int zonesize = 0x200000; //this appears to be a default cache size
   const char * retp;

   sg_hunk_base = (byte *)buf;
   sg_hunk_size = bufsize;
   sg_hunk_low_used = 0;
   sg_hunk_high_used = 0;

   Cache_Init();

   //COM_CheckParm is a function that returns the position (as in argv[])
   //that a given parameter is at (or 0 if it doesn't exist).

   //We can then check the global argument list at that location and see what's
   //inside.

   retp = COM_GetParmAfterParm("-zone");
   if(retp[0] != '\0') {
      zonesize = (Q_atoi(retp) << 10); // This flags up a 'const discard' warning on my Fedora install, but not on Windows.
   }
   sg_mainzone = Hunk_AllocName(zonesize, "zone");

   if (sg_mainzone == NULL) {
      Sys_Error("Memory_Init: Zone allocation failed.  Check -zone and/or -heapsize switches\n");
   }

   Z_ClearZone(sg_mainzone, zonesize);
}


/*** HUNK ***/


/* DESCRIPTION: Hunk_Alloc
// LOCATION: zone.c
// PATH: All over
//
// Calls hunk_allocName with a generic name
*/
void *Hunk_Alloc(unsigned int size) {

   return(Hunk_AllocName(size, "noname"));
}

/* DESCRIPTION: Hunk_AllocName
// LOCATION: zone.c
// PATH: All over
//
// Carves a chunk of the hunk
*/
void *Hunk_AllocName(unsigned int size, const char *name) {

   hunk_t *h;
   unsigned int freespace = sg_hunk_size - sg_hunk_low_used - sg_hunk_high_used;
   unsigned int totalsize = sizeof(hunk_t) + ((size+15)&~15);

   if(totalsize < size || totalsize > freespace) { //overflow catching
      Sys_Error ("Hunk_AllocName: Requested size %u, padded to size %u, total free hunk is %u", size, totalsize, freespace);
   }

   h = (hunk_t *)(sg_hunk_base + sg_hunk_low_used);

   sg_hunk_low_used += totalsize;
   Cache_FreeLow(sg_hunk_low_used);

   memset(h, 0, totalsize);


   h->size = totalsize;
   h->sentinal = HUNK_SENTINAL;

   /* Orginally this in the QW source was Q_strncpyz with a call to COM_SkipPath in there.
   // Skippath() basically ignored anything before a forward slash.
   // The strcpy rewrite looked to just be strncpy with a precautionary null tacked on.
   // well I didn't do all that.  We zero'd out the structure; if we copy 63 bytes
   // we should be guaranteed a properly terminated string.
   //
   // The paths could cause trouble.  What if it's ungodly long and all unique
   // names are truncated?  Well HL didn't worry about it, neither will I.
   // Besides I'm not a modder.  That's their problem.
   */

   Q_strncpy(h->name, name, HUNK_ARRAYNAMELENGTH-1);

   return(void *)(h+1);
}

/* DESCRIPTION: Hunk_TempAlloc
// LOCATION: zone.c
// PATH: Mods I think
//
// Allocates with the name 'temp'.  A small block of code seems to exist to
// purge thigns allocated as 'temp' from time to time.  It looks like only
// one temp is allowed at a given time.
*/
void *Hunk_TempAlloc(unsigned int size) {

   void *buf;

// size = (size+15)&~15; //unnecessary; hunkhighallocname does this already

   if(sg_hunk_tempactive) {
      sg_hunk_tempactive = 0;
      Hunk_FreeToHighMark(sg_hunk_tempmark);
   }

   sg_hunk_tempmark = Hunk_HighMark();

   buf = Hunk_HighAllocName(size, "temp");

   sg_hunk_tempactive = 1;

   return(buf);
}

/* DESCRIPTION: Hunk_HighAllocName
// LOCATION: zone.c
// PATH:
//
// Carves an upper chunk of the hunk; can fail.
*/
void *Hunk_HighAllocName(unsigned int size, char *name) {

   hunk_t   *h;
   unsigned int freespace;
   unsigned int totalsize = sizeof(hunk_t) + ((size+15)&~15);

   if(sg_hunk_tempactive) {

      Hunk_FreeToHighMark(sg_hunk_tempmark);
      sg_hunk_tempactive = 0;
   }

   freespace = sg_hunk_size - sg_hunk_low_used - sg_hunk_high_used;

   if(totalsize < size || totalsize > freespace) { //overflow catching
      Con_Printf("Hunk_HighAllocName: Requested size %u, padded to size %u, total free hunk is %u", size, totalsize, freespace);
      return(NULL);
   }

   sg_hunk_high_used += size;
   Cache_FreeHigh(sg_hunk_high_used);

   h = (hunk_t *)(sg_hunk_base + sg_hunk_size - sg_hunk_high_used);

   memset(h, 0, size);
   h->size = size;
   h->sentinal = HUNK_SENTINAL;
   Q_strncpy(h->name, name, HUNK_ARRAYNAMELENGTH-1);

   return(void *)(h+1);
}

/* DESCRIPTION: Hunk_Low/HighMark
// LOCATION: zone.c
// PATH:
//
// Return some global vars.
*/
uint32 Hunk_LowMark(void) {

   return(sg_hunk_low_used);
}
uint32 Hunk_HighMark(void) {

   if(sg_hunk_tempactive) {

      sg_hunk_tempactive = 0;
      Hunk_FreeToHighMark(sg_hunk_tempmark);
   }
   return(sg_hunk_high_used);
}

/* DESCRIPTION: Hunk_FreeToLowMark
// LOCATION: zone.c
// PATH: Some host and mod thing
//
// reassigns some variables.  We zero the memory out; HL doesn't seem to care.
*/
void Hunk_FreeToLowMark(unsigned int mark) {

   if (mark > sg_hunk_low_used) {
      Sys_Error ("Hunk_FreeToLowMark: bad mark %i", mark);
   }
   memset(sg_hunk_base + mark, 0, sg_hunk_low_used - mark);
   sg_hunk_low_used = mark;
}

/* DESCRIPTION: Hunk_FreeToHighMark
// LOCATION: zone.c
// PATH: Other hunk functions
//
// This seems to mostly be used for clearing up the temp allocation.  Anything
// between the high stack's current position and the desired mark--an area not
// used for much--is wiped clean.
*/
void Hunk_FreeToHighMark(unsigned int mark) {

   if (sg_hunk_tempactive) {
      sg_hunk_tempactive = 0;
      Hunk_FreeToHighMark (sg_hunk_tempmark);
   }
   if (mark > sg_hunk_high_used) {
      Sys_Error("Hunk_FreeToHighMark: bad mark %i", mark);
   }
   memset(sg_hunk_base + sg_hunk_size - sg_hunk_high_used, 0, sg_hunk_high_used - mark);
   sg_hunk_high_used = mark;
}


/* DESCRIPTION: Hunk_Check
// LOCATION: zone.c
// PATH: nowhere yet
//
// Makes sure the sentinel is intact
*/
void Hunk_Check(void) {

   hunk_t *h;

   for (h = (hunk_t *)sg_hunk_base; (byte *)h < sg_hunk_base + sg_hunk_low_used; h = (hunk_t *)((byte *)h+h->size)) {
      if (h->sentinal != HUNK_SENTINAL) {
         Sys_Error("Hunk_Check: trashed sentinal at address %p. If the name isn't trashed too, it's %s.", h, h->name);
      }
      if ((h->size && 15) != 0 || h->size + (unsigned)(byte *)h - (unsigned)sg_hunk_base > sg_hunk_size) {
         Sys_Error("Hunk_Check: bad size");
      }
   }
}

/* DESCRIPTION: Hunk_Print
// LOCATION: zone.c
// PATH: nowhere
//
// nothing
*/
void Hunk_Print(/*qboolean ignored*/) {

   Con_Printf("Hunk_Print called, then ignored.\n");
}


/*** CACHE ***/

/* DESCRIPTION: Cache_Init
// LOCATION: zone.c
// PATH: This File: Memory_Init->Cache_Init
//
// It looks like it simply adds console commands and zeros out some global structure
*/
void Cache_Init() {

   sg_cache_head.next = sg_cache_head.prev = sg_cache_head.lru_next = sg_cache_head.lru_prev = &sg_cache_head;
   Cmd_AddCommand("flush", Cache_Flush);
}

/* DESCRIPTION: Cache_Alloc
// LOCATION: zone.c
// PATH: Mod and other places not in this file
//
// Gets a bit of the hunk.  Most of the work is done in tryalloc.
*/
void *Cache_Alloc(cache_user_t *c, unsigned int size, const char *name) {

   cache_system_t *cs;


   if(c->data) {
      Sys_Error("Cache_Alloc: already allocated");
   }
   if(size <= 0) {
      Sys_Error("Cache_Alloc: size is either SO LARGE IT WRAPPED AROUND or zero.", size);
   }
   size = (size + sizeof(cache_system_t) + 15) & ~15;

   //Took me a bit; the whole 'add 15 not 15' thing is to ensure this lines up on a 16 byte boundary

   // find memory for it
   while(1) {
      cs = Cache_TryAlloc(size, 0);

      if(cs) { //hooray, we found room
         Q_strncpy(cs->name, name, sizeof(cs->name)-1);
         c->data = (void *)(cs+1);
         cs->user = c;
         break;
      }

      // We didn't find room, free something and try again
      if(sg_cache_head.lru_prev == &sg_cache_head) { //nothing left to free
         Sys_Error("Cache_Alloc: out of memory");
      }
      Cache_Free(sg_cache_head.lru_prev->user);
   }

   return(Cache_Check(c));
}

/* DESCRIPTION: Cache_TryAlloc
// LOCATION: zone.c
// PATH: Cache_Move || Cache_Alloc ->Cache_TryAlloc
//
// From zone.c: Looks for a free block of memory between the high and low
// hunk marks.  Size should already include the header and padding
*/
cache_system_t *Cache_TryAlloc (unsigned int size, qboolean nobottom) {
   cache_system_t   *cs, *newc;

   // unique case 1: Cache is completely unused
   if (nobottom == 0 && sg_cache_head.prev == &sg_cache_head)   {

      //if they want more room than we have period then they need to fix their code
      if (sg_hunk_size - sg_hunk_high_used - sg_hunk_low_used < size) {
         Sys_Error("Cache_TryAlloc: %i is greater then the possibly empty hunk", size);
      } else {
         newc = (cache_system_t *) (sg_hunk_base + sg_hunk_low_used);
         memset (newc, 0, sizeof(*newc));
         newc->size = size;

         sg_cache_head.prev = sg_cache_head.next = newc;
         newc->prev = newc->next = &sg_cache_head;

         Cache_MakeLRU (newc);
         return(newc);
      }
   }

// search from the bottom up for space

   newc = (cache_system_t *) (sg_hunk_base + sg_hunk_low_used);
   cs = sg_cache_head.next;

   do   {
      if (nobottom == 0 || cs != sg_cache_head.next)   {
         if ( (unsigned)(byte *)cs - (unsigned)(byte *)newc >= size) {

            // found space
            memset (newc, 0, sizeof(*newc));
            newc->size = size;

            newc->next = cs;
            newc->prev = cs->prev;
            cs->prev->next = newc;
            cs->prev = newc;

            Cache_MakeLRU (newc);

            return(newc);
         }
      }

      // otherwise continue looking
      newc = (cache_system_t *)((byte *)cs + cs->size);
      cs = cs->next;

   } while (cs != &sg_cache_head);

// We either failed or ignored all the area at the beginning.  We must now try
// to allocate room right at the end of thi patchwork memory model.

   if ((unsigned)sg_hunk_base + sg_hunk_size - sg_hunk_high_used - (unsigned)(byte *)newc >= size)   {

      memset (newc, 0, sizeof(*newc));
      newc->size = size;

      newc->next = &sg_cache_head;
      newc->prev = sg_cache_head.prev;
      sg_cache_head.prev->next = newc;
      sg_cache_head.prev = newc;

      Cache_MakeLRU (newc);

      return(newc);
   }

   return(NULL);      // couldn't allocate
}

/* DESCRIPTION: Cache_MakeLRU
// LOCATION: zone.c
// PATH: Cache_TryAlloc (or check)->Cache_MakeLRU
//
// Adds this thing to a linked list, and aborts if it's already added
// Don't really know what makes the LRU list so special.
*/
void Cache_MakeLRU (cache_system_t *cs) {

   if ((cs->lru_next != NULL) || (cs->lru_prev != NULL)) {
      Sys_Error("Cache_MakeLRU: already linked, aborting");
   }
   sg_cache_head.lru_next->lru_prev = cs;
   cs->lru_next = sg_cache_head.lru_next;
   cs->lru_prev = &sg_cache_head;
   sg_cache_head.lru_next = cs;
}

/* DESCRIPTION: Cache_UnlinkLRU
// LOCATION: zone.c
// PATH: Cache_functions->Cache_UnlinkLRU
//
// The exact opposite of the above.
*/
void Cache_UnlinkLRU (cache_system_t *cs) {

   if (!cs->lru_next || !cs->lru_prev) {
      Sys_Error("Cache_UnlinkLRU: Not linked, aborting");
   }
   cs->lru_next->lru_prev = cs->lru_prev;
   cs->lru_prev->lru_next = cs->lru_next;

   cs->lru_prev = cs->lru_next = NULL;
}


/* DESCRIPTION: Cache_Check
// LOCATION: zone.c
// PATH: All over
//
// Just verifies that the data is there I guess...  Moves it too.
*/
HLDS_DLLEXPORT void * Cache_Check(cache_user_t *c) {
   cache_system_t * cs;

   if (!c->data)
      return NULL;

   cs = ((cache_system_t *)c->data) - 1;
   //cs = (cache_system_t *)(((byte *)c->data) - ((int)&(((cache_system_t *)0)->user)));

// move to head of LRU
   Cache_UnlinkLRU(cs);
   Cache_MakeLRU(cs);

   return(c->data);
}

/* DESCRIPTION: Cache_Move
// LOCATION: zone.c
// PATH: Cache_FreeLow || Cache_FreeHigh
//
// Well the name describes what it does better than I can.  I don't quite
// see right now why 'moving' takes this much effort though.
*/
void Cache_Move(cache_system_t *c) {
   cache_system_t      *newc;

// we are clearing up space at the bottom, so only allocate it late
   newc = Cache_TryAlloc(c->size, 1);

   if(newc) {

      Q_memcpy ( newc+1, c+1, c->size - sizeof(cache_system_t) );
      newc->user = c->user;
      Q_memcpy (newc->name, c->name, sizeof(newc->name));
      Cache_Free (c->user);
      newc->user->data = (void *)(newc+1);
   } else {

      Cache_Free (c->user);      // tough luck
   }
}


/* DESCRIPTION: Cache_Flush
// LOCATION: zone.c
// PATH: Called dynamically when "flush" console command is used
//
// In asm, this is long and boring and calls cache_force_flush.  In QW, this
// has been replaced with a nicer set of functions that I think can be used w/o worry.
*/
void Cache_Flush() {

   while(&sg_cache_head != sg_cache_head.next) {
      Cache_Free(sg_cache_head.next->user);
   }
}

/* DESCRIPTION: Cache_Free
// LOCATION: zone.c
// PATH: Many places besides this file
//
// Let My Memory Go (and remove it from the active list)
*/
void Cache_Free(cache_user_t *c) {

   cache_system_t   *cs;

   if (!c->data) {
      Sys_Error("Cache_Free: not allocated");
   }
   cs = ((cache_system_t *)c->data) - 1; //+FFFFFFA8?

   cs->prev->next = cs->next;
   cs->next->prev = cs->prev;
   cs->next = cs->prev = NULL;

   c->data = NULL;

   Cache_UnlinkLRU(cs);
}

/* DESCRIPTION: Cache_FreeLow/High
// LOCATION: zone.c
// PATH: Hunk_(High)AllocName->Cache_FreeLow/High
//
// Frees up memory so we can squeeze something new in
*/
void Cache_FreeLow(unsigned int new_low_hunk) {
   cache_system_t *c;

   while(1) {

      c = sg_cache_head.next;
      if(c == &sg_cache_head) {
         return;
      }
      if((byte *)c >= sg_hunk_base + new_low_hunk) {
         return;
      }
      Cache_Move(c); // reclaim the space
   }
}
void Cache_FreeHigh(unsigned int new_high_hunk) {

   cache_system_t *c, *prev;

   prev = NULL;
   while (1) {

      c = sg_cache_head.prev;
      if (c == &sg_cache_head) { //nothing cached
         return;
      }
      if ((byte *)c + c->size <= sg_hunk_base + sg_hunk_size - new_high_hunk) {
         return;
      }
      if (c == prev) {
         Cache_Free(c->user);   // didn't move out of the way
      } else {
         Cache_Move(c);   // try to move it
         prev = c;
      }
   }
}

/* UNIMPLEMENTED: CacheSystemCompare
// LOCATION:
// PATH: Passed as an argument to qsort, local to HUNK status functions
//
// Using strcmp, compares--get this--the names of two cache_system_t
// objects.  I'm pretty certain it's a straight Q_strcasecmp with no added
// trickery and definitely no modification of variables.
//
// But it's not necessary, and therefore won't be used in the finished product.
*/

//reporting is for chumps.  I won't be sorting or doing fancy crap cause it's not
//vital to a running server.
void Cache_TotalUsed() {
   Con_Printf("CacheTotal: Go away.\n");
}
void Cache_Report() {
   Con_Printf("%4.1f megabyte data cache\n", (sg_hunk_size - sg_hunk_high_used - sg_hunk_low_used) / (float)(1024*1024) );
}
void Cache_Print () {

   Con_Printf("Cache_Print: I thought the linux build didn't include these debug things.  Stop calling them.\n");
}
void Cache_Print_Models_And_Totals() {
   Con_Printf("Cache_Print_models: No, you can't haev a stats update.  Not yours.\n");
}
void Cache_Print_Sounds_And_Totals() {
   Con_Printf("Cache_Print_Sounds: Debug statements annoy me.\n");

}


/*** Z ***/

/* DESCRIPTION: Z_ClearZone
// LOCATION: zone.c
// PATH: Memory_Init->Z_ClearZone
//
// An init function, basically.
*/
void Z_ClearZone(memzone_t *zone, unsigned int size) {
   memblock_t *block;

// set the entire zone to one free block

   zone->blocklist.next = zone->blocklist.prev = block = (memblock_t *)( (byte *)zone + sizeof(memzone_t) );
   zone->blocklist.tag = 1;   // in use block
   zone->blocklist.id = 0;
   zone->blocklist.size = 0;
   zone->rover = block;

   block->prev = block->next = &zone->blocklist;
   block->tag = 0;         // free block
   block->id = ZONEID;
   block->size = size - sizeof(memzone_t);
}

/* DESCRIPTION: Z_Free
// LOCATION: zone.c
// PATH: all over
//
// Frees a Z block, marking it as such
*/
void Z_Free(void *ptr) {

   memblock_t *block, *other;

   if (!ptr) {
      Sys_Error("Z_Free: NULL pointer");
   }

   block = (memblock_t *) ( (byte *)ptr - sizeof(memblock_t));
   if (block->id != ZONEID) {
      Sys_Error ("Z_Free: freed a pointer without ZONEID");
   }
   if (block->tag == 0) {
      Sys_Error ("Z_Free: freed a freed pointer");
   }
   block->tag = 0;      // mark as free

   other = block->prev;
   if (!other->tag) {   // merge with previous free block
      other->size += block->size;
      other->next = block->next;
      other->next->prev = other;
      if (block == sg_mainzone->rover) {
         sg_mainzone->rover = other;
      }
      block = other;
   }

   other = block->next;
   if (!other->tag) {   // merge the next free block onto the end
      block->size += other->size;
      block->next = other->next;
      block->next->prev = block;
      if (other == sg_mainzone->rover) {
         sg_mainzone->rover = block;
      }
   }
}

/* DESCRIPTION: Z_Malloc
// LOCATION: zone.c
// PATH: all over
//
// Assigns a Z block
*/
void *Z_Malloc(unsigned int size) {

   void   *buf;

   Z_CheckHeap();
   buf = Z_TagMalloc(size, 1);
   if (!buf) {
      Sys_Error("Z_Malloc: failed on allocation of %i bytes",size);
   }
   Q_memset(buf, 0, size);

   return buf;
}

/* DESCRIPTION: Z_TagMalloc
// LOCATION: zone.c
// PATH: all over
//
// Does the actual assigning.  To be honest, this Z stuff seems weak.  Do not want.
*/
void *Z_TagMalloc(unsigned int size, int tag) {

   int extra;
   memblock_t *start, *rover, *newz, *base;

   if (tag == 0) {
      Sys_Error ("Z_TagMalloc: tried to use a 0 tag");
   }

//
// scan through the block list looking for the first free block
// of sufficient size
//
   size += sizeof(memblock_t);   // account for size of block header
   size += 4;               // space for memory trash tester
   size = (size + 7) & ~7;      // align to 8-qbyte boundary

   base = rover = sg_mainzone->rover;
   start = base->prev;

   do   {
      if (rover == start) { // scanned all the way around the list
         return(NULL);
      }
      if (rover->tag) {
         base = rover = rover->next;
      } else {
         rover = rover->next;
      }
   } while (base->tag || base->size < size);

//
// found a block big enough
//
   extra = base->size - size;
   if (extra >  MINFRAGMENT) {   // there will be a free fragment after the allocated block
      newz = (memblock_t *) ((byte *)base + size );
      newz->size = extra;
      newz->tag = 0;         // free block
      newz->prev = base;
      newz->id = ZONEID;
      newz->next = base->next;
      newz->next->prev = newz;
      base->next = newz;
      base->size = size;
   }

   base->tag = tag;            // no longer a free block

   sg_mainzone->rover = base->next;   // next allocation will start looking here

   base->id = ZONEID;

// marker for memory trash testing
   *(int *)((byte *)base + base->size - 4) = ZONEID;

   return (void *) ((byte *)base + sizeof(memblock_t));
}

/* DESCRIPTION: Z_Print
// LOCATION: zone.c
// PATH: nowhere
//
// Prints the lovely stuff.  Or as I do, prints nothing and calls it a day.
*/
void Z_Print(/*memzone_t *zone*/) {

   Con_Printf ("Z_Print called, then ignored.\n");
}

/* DESCRIPTION: Z_CheckHeap
// LOCATION: zone.c
// PATH: nowhere
//
// For some reason this statement that would probably be def'd out is in the executable.
// Just makes sure it's all up to snuff.
*/
void Z_CheckHeap(void) {

   memblock_t *block;

   for (block = sg_mainzone->blocklist.next; ; block = block->next) {
      if (block->next == &sg_mainzone->blocklist) {
         break;         // all blocks have been hit
      }
      if ( (byte *)block + block->size != (byte *)block->next) {
         Sys_Error("Z_CheckHeap: block size does not touch the next block\n");
      }
      if ( block->next->prev != block) {
         Sys_Error("Z_CheckHeap: next block doesn't have proper back link\n");
      }
      if (!block->tag && !block->next->tag) {
         Sys_Error("Z_CheckHeap: two consecutive free blocks\n");
      }
   }
}

