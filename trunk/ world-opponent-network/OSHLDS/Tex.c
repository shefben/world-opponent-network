#include "Tex.h"
#include "com_file.h"
#include "FS.hpp"
#include "report.h"
#include "Q_memdef.h"

#include <ctype.h>

///global global
char texgammatable[0x100];


/*** GLOBALS ***/
static int nummiptex;
static lumpinfo2_t miptex[MAX_MAP_TEXTURES];
static int nTexLumps = 0;
static lumpinfo_t *lumpinfo = NULL;
static int nTexFiles = 0;
static hl_file_t *texfiles[128];


/*** FUNCTIONS ***/

//This mostly poor code is like a poor man's version of the 'common'
//functions.  Most of it's simple enough I won't go over it.
int lump_sorter(const void *lump1, const void *lump2) {
   lumpinfo_t *plump1 = (lumpinfo_t *)lump1;
   lumpinfo_t *plump2 = (lumpinfo_t *)lump2;
   return Q_strcmp(plump1->name, plump2->name);
}
void ForwardSlashes(char * string) {

   for(; *string != '\0'; string++) {
      if(*string == '\\') { *string = '/'; }
   }
}
void SafeRead(hl_file_t *f, void *buffer, int count) {

   if(FS_Read(buffer, count, 1, f) != count) {
      Sys_Error("%s: File read failure.", __FUNCTION__);
   }
}
void CleanupName(char *in, char *out) { //This seems a pretty useless function...

   int i;

   for (i=0 ; i< 16 ; i++ ) {
      if(!in[i]) { break; }

      out[i] = toupper(in[i]);
   }

   for(; i< 16; i++) {
      out[i] = 0;
   }
}
int FindMiptex(char *name) {

   int i;

   for(i = 0; i < nummiptex; i++) {

      if(Q_strcasecmp(name, miptex[i].name) == 0) {

         return(i);
      }
   }
   if(nummiptex >= MAX_MAP_TEXTURES) {
      Sys_Error("Exceeded MAX_MAP_TEXTURES");
   }
   Q_strncpy(miptex[i].name, name, sizeof(miptex[i].name)-1);
   miptex[i].name[sizeof(miptex[i].name)-1] = '\0';
   nummiptex++;

   return(i);
}

/* DESCRIPTION: TEX_InitFromWad
// LOCATION: SDK
// PATH: Mod_LoadTextures
//
// I am crap.  Rewrite me.
*/
int TEX_InitFromWad(char *path) {

   int i;
   wadinfo_t wadinfo;
   char szTmpPath[1024];
   char szFileBase[1024];
   char szFileBase2[1028];
   char *pszWadFile;
   hl_file_t * texfile;

   Q_strncpy(szTmpPath, path, sizeof(szTmpPath)-1);
   szTmpPath[sizeof(szTmpPath)-1] = '\0';

   // temporary kludge so we don't have to deal with no occurances of a semicolon
   // in the path name ..
   // It's sloppy too.
   if(Q_strchr(szTmpPath, ';') == NULL) {
      Q_strcat(szTmpPath, ";");
   }

   pszWadFile = Q_strtok(szTmpPath, ";"); //strtok is not thread safe, consider rewriting.

   while(pszWadFile != NULL) {

      ForwardSlashes(pszWadFile);
      COM_FileBase(pszWadFile, szFileBase);
      strcpy(szFileBase2, szFileBase);
      COM_DefaultExtension(szFileBase2, ".wad");

      if(Q_strstr(szFileBase, "pldecal") == NULL) {

         //just continue and get the hell out of this sloppy function.
         pszWadFile = Q_strtok(NULL, ";");
      }

      texfiles[nTexFiles] = FS_Open(szFileBase2, "rb");

      if(texfiles[nTexFiles] == NULL) {
         Sys_Error("WARNING: couldn't open %s\n", szFileBase2);
      }

      nTexFiles++;

      // temp assignment to make things cleaner:
      texfile = texfiles[nTexFiles-1];
      Con_Printf("Using WAD File: %s\n", szFileBase2);

      SafeRead(texfile, &wadinfo, sizeof(wadinfo));
      if(Q_strncmp(wadinfo.identification, "WAD2", 4) != 0 && strncmp(wadinfo.identification, "WAD3", 4) != 0) {

         Sys_Error("TEX_InitFromWad: %s isn't a wadfile", pszWadFile);
      }

      wadinfo.numlumps = letohost32(wadinfo.numlumps);
      wadinfo.infotableofs = letohost32(wadinfo.infotableofs);

      FS_Seek(texfile, wadinfo.infotableofs, SEEK_SET);
      lumpinfo = Q_Realloc(lumpinfo, (nTexLumps + wadinfo.numlumps) * sizeof(lumpinfo_t));

      for(i = 0; i < wadinfo.numlumps; i++) {

         SafeRead(texfile, &lumpinfo[nTexLumps], sizeof(lumpinfo_t) - sizeof(int));
         CleanupName(lumpinfo[nTexLumps].name, lumpinfo[nTexLumps].name);
         lumpinfo[nTexLumps].filepos = letohost32(lumpinfo[nTexLumps].filepos);
         lumpinfo[nTexLumps].disksize = letohost32(lumpinfo[nTexLumps].disksize);
         lumpinfo[nTexLumps].iTexFile = nTexFiles-1;

         nTexLumps++;
      }

      pszWadFile = strtok(NULL, ";");
   }

   qsort((void *)lumpinfo, (size_t)nTexLumps, sizeof(lumpinfo[0]), lump_sorter);

   return(1);
}
void TEX_CleanupWadInfo() {

   int i;

   if(lumpinfo != NULL) {
      Q_Free(lumpinfo);
      lumpinfo = NULL;
   }

   for(i = 0; i < nTexFiles; i++) {

      FS_Close(texfiles[i]);
      texfiles[i] = NULL;
   }

   nTexLumps = 0;
   nTexFiles = 0;
}
int TEX_LoadLump(miptex_t *source, byte *dest) {

   lumpinfo_t key;
   lumpinfo_t *found;

   CleanupName(source->name, key.name);
   found = bsearch(&key, (void *)lumpinfo, (size_t)nTexLumps, sizeof(lumpinfo[0]), lump_sorter);

   if(found == NULL) {
      Con_Printf("%s: Couldn't find texture %s.\n", __FUNCTION__, source->name);
      return(0);
   }

   FS_Seek(texfiles[found->iTexFile], found->filepos, SEEK_SET);
   SafeRead(texfiles[found->iTexFile], dest, found->disksize); //'dest' will map to a miptex_t structure.
   return(found->disksize);
}
void TEX_AddAnimatingTextures() {

   int base;
   int i, j, k;
   char name[32];

   base = nummiptex;

   for(i = 0; i < base; i++) {

      if(miptex[i].name[0] != '+' && miptex[i].name[0] != '-') {
         continue;
      }
      Q_strncpy(name, miptex[i].name, sizeof(name)-1);
      name[sizeof(name)-1] = '\0';

      for (j=0 ; j<20 ; j++) {

         if (j < 10) { name[1] = '0'+j; }
         else { name[1] = 'A'+j-10; } // alternate animation

         // see if this name exists in the wadfile
         for(k = 0; k < nTexLumps; k++) {
            if(Q_strcmp(name, lumpinfo[k].name) == 0) {
               FindMiptex(name); // add to the miptex list
               break;
            }
         }
      }
   }

   if(nummiptex != base) {
      Con_Printf("%s: added %i additional animating textures.\n", __FUNCTION__, nummiptex - base);
   }
}
