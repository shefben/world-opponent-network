/* Relates to loading TEXTURES.  The code in this file is VERY unsafe,
// and requires a complete and total overhaul once I know how the damn files
// work...
*/
#ifndef OSHLDS_TEX_HEADER
#define OSHLDS_TEX_HEADER

#include "Mod.h"

#define MAX_MAP_TEXTURES 0x200

typedef struct wadinfo_s { //appears to be another way of looking at the dheader_t struct.

   char identification[4];      // should be WAD3
   int  numlumps;
   int  infotableofs;
} wadinfo_t;
typedef struct lumpinfo_s { //0x24

   int         filepos;
   int         disksize;
   int         size;               // uncompressed
   char      type;
   char      compression;
   char      pad1, pad2;
   char      name[16];            // must be null terminated

   int         iTexFile;   // index of the wad this texture is located in

} lumpinfo_t;
typedef struct lumpinfo2_s { //SDK claimed the miptex was a lumpinfo, but the struct is off. 0x40

   char name[32];
   char padding[32];

} lumpinfo2_t;

extern char texgammatable[0x100];

//functions
qboolean TEX_InitFromWad(char *);
void TEX_CleanupWadInfo();
int TEX_LoadLump(miptex_t *, byte *);
void TEX_AddAnimatingTextures();
#endif
