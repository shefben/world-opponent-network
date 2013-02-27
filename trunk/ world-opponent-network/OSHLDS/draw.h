/* A server doesn't draw, so one would expect these functions to be largely
// empty.  And they do seem to be that way.  These are sometimes present in
// the wad.c file, under different names.
*/

#ifndef OSHLDS_DRAW_HEADER
#define OSHLDS_DRAW_HEADER

#include "HUNK.h"
#include "Mod.h"

/*** structures ***/

//The main structs are in common.c pending reordering.

typedef struct tempdata_s { //0x44

   char name[0x40];
   cache_user_t cache; //texture_t *s I believe

} tempdata_t;

typedef struct wadlumpinfo_s { //0x20

   int filepos; //00
   int disksize; //04
   int size;               // uncompressed
   char type; //0c
   char compression; //0d
   char pad1, pad2; //0e
   char name[16];   //10         // must be null terminated
} wadlumpinfo_t;
typedef struct tempwad_s { //0x2C

   char * filename;

   tempdata_t * padding04;
   int num_items; //A count variable, tallying up the number of above items.
   int max_items; //Appears to be a MAX field.

   wadlumpinfo_t * padding10;
   int num_items2; // = wadinfo_t.numlumps;
   int max_items2; //I do not believe this is correct.  It appears to be an identifier.

   void (*padding1C)(struct tempwad_s *, byte *);
   int padding20;
   char ** padding24; //indexable
   int * padding28; //indexable
} tempwad_t;

typedef struct decalstruct_s { //0x0C

   wadlumpinfo_t * padding00;
   int32 padding04;
   struct decalstruct_s * next;
} decalstruct_t;

typedef struct miptex2_s { //0x40

   char      name[16]; //00
   unsigned   width, height; //10
   unsigned padding18; //18
   unsigned padding1C;
   unsigned padding20;
   unsigned padding24;
   unsigned padding28; //28
   unsigned   offsets[MIPLEVELS]; //2C
   unsigned padding3c;
} miptex2_t;

//vars
extern int global_sv_decalnamecount;
extern char global_sv_decalnames[0x200][0x11];


//functions
void Draw_CacheWadInit(char *, int, tempwad_t *);
int Draw_CacheIndex(tempwad_t *, char *);
texture_t * Draw_CacheGet(tempwad_t *, int);
int Draw_DecalSize(int);
void Draw_DecalShutdown();

void Decal_Init();
int CustomDecal_Init(tempwad_t *, byte *, int, int);
void * CustomDecal_Validate(byte *, int);

void W_LoadWadFile(char *);
void W_Shutdown();
#endif
