#include "draw.h"
#include "common.h"
#include "Tex.h"
#include "report.h"
#include "FS.hpp"

/*** STRUCTS ***/

typedef struct wadcontainer_s { //0x30

   int InUse;
   char name[0x20];
   unsigned int wad_numlumps;
   wadlumpinfo_t * wad_lumps;
   byte * wad_base; //can be many things

} wadcontainer_t;
typedef struct qpic_s {
   int         width, height;
   byte      data[4];         // variably sized
} qpic_t;


/*** GLOBAL GLOBALS ***/
static int gfCustomBuild;
static char szCustName[0x10];
static tempwad_t * decal_wad;
static wadcontainer_t wads[2];



/*** GLOBALS ***/
int  global_sv_decalnamecount = 0;
char global_sv_decalnames[0x200][0x11];


/*** W ***/

/*
//Draw_AllocateCacheSpace
//Draw_CacheByIndex
Draw_CacheFindIndex //unused
//Draw_CacheGet
//Draw_CacheIndex
//Draw_CacheLoadFromCustom
//Draw_CacheReload
//Draw_CacheWadHandler
//Draw_CacheWadInit
//Draw_CacheWadInitFromFile
//Draw_CustomCacheGet
//Draw_CustomCacheWadInit
//Draw_DecalCount
Draw_DecalIndex //unused
Draw_DecalIndexFromName //unused
//Draw_DecalName
Draw_DecalSetName //unused
//Draw_DecalShutdown
//Draw_DecalSize
Draw_DecalTexture//unused
//Draw_FreeWad
Draw_Init //empty
//Draw_MiptexTexture
Draw_Shutdown //empty--conditions that would run code are absent.
//Draw_ValidateCustomLogo

//Decal_CountLumps
//Decal_MergeInDecals
//Decal_ReplaceOrAppendLump
//Decal_SizeLumps
//Decal_Init
*/

/*
==================
W_CleanupName

Lowercases name and pads with spaces and a terminating 0 to the length of
lumpinfo_t->name.
Used so lumpname lookups can proceed rapidly by comparing 4 chars at a time
Space padding is so names can be printed nicely in tables.
Can safely be performed in place.
==================
*/
void W_CleanupName(char * in, char * out) {

   int i;
   int c;

   for(i = 0; i < 16; i++) {
      c = in[i];
      if (!c) { break; }

      if(c >= 'A' && c <= 'Z') {
         c += ('a' - 'A');
      }
      out[i] = c;
   }

   for(; i < 16; i++) {
      out[i] = '\0';
   }
}



void SwapPic(qpic_t *pic) {
   pic->width = letohost32(pic->width);
   pic->height = letohost32(pic->height);
}

void W_LoadWadFile(char * filename) {

   wadlumpinfo_t *lump_p;
   wadinfo_t *header;
   unsigned i;
   int infotableofs;
   wadcontainer_t * newwad;


   i = 0;
   while(1) {

      if(i > 1) {
         Con_Printf("%s: Both wad slots used up.\n", __FUNCTION__);
         return;
      }

      newwad = &wads[i];
      if(newwad->InUse == 0) { break; }
      i++;
   }

   newwad->wad_base = COM_LoadHunkFile(filename);
   if(newwad->wad_base == NULL) {

      Con_Printf("%s: Couldn't load wad %s.\n", __FUNCTION__, filename);
      return;
   }

   newwad->InUse = 1;

   Q_strncpy(newwad->name, filename, 0x1F);
   newwad->name[0x1F] = '\0';

   header = (wadinfo_t *)newwad->wad_base;
   if(header->identification[0] != 'W'
   || header->identification[1] != 'A'
   || header->identification[2] != 'D'
   || header->identification[3] != '3') {

      Sys_Error("%s: Wad file %s doesn't have WAD3 id.\n", __FUNCTION__, filename);
   }

   newwad->wad_numlumps = letohost32(header->numlumps);
   infotableofs = letohost32(header->infotableofs);
   newwad->wad_lumps = (wadlumpinfo_t *)(newwad->wad_base + infotableofs);

   for(i = 0, lump_p = newwad->wad_lumps ; i < newwad->wad_numlumps; i++,lump_p++) {

      lump_p->filepos = letohost32(lump_p->filepos);
      lump_p->size = letohost32(lump_p->size);
      W_CleanupName(lump_p->name, lump_p->name);
      if(lump_p->type == 'B') { //TYP_QPIC

         SwapPic((qpic_t *)(newwad->wad_base + lump_p->filepos));
      }
   }
}
wadlumpinfo_t * W_GetLumpinfo(int wadindex, char *name, int DisallowFail) {

   unsigned int i;
   wadlumpinfo_t * lump_p;
   char clean[16];


   W_CleanupName(name, clean);

   for(lump_p = wads[wadindex].wad_lumps, i = 0; i < wads[wadindex].wad_numlumps; i++,lump_p++) {

      if(Q_strcmp(clean, lump_p->name) == 0) {
         return(lump_p);
      }
   }

   if(DisallowFail != 0) {
      Sys_Error("%s: %s not found", __FUNCTION__, name);
   }

   return(NULL);
}
byte * W_GetLumpName(int wadindex, char *name) {

   wadlumpinfo_t *lump;


   lump = W_GetLumpinfo(wadindex, name, 1);
   return(((byte *)wads[wadindex].wad_base) + lump->filepos);
}
byte * W_GetLumpNum(int wadindex, unsigned int num) {

   wadlumpinfo_t *lump;


   if(num >= wads[wadindex].wad_numlumps) {
      Sys_Error("%s: bad number: %i.\n", __FUNCTION__, num);
   }

   lump = wads[wadindex].wad_lumps + num;
   return(((byte *)wads[wadindex].wad_base) + lump->filepos);
}

void W_Shutdown() {

   Q_memset(wads, 0, sizeof(wads));
}
/*** DRAW ***/

void Draw_AllocateCacheSpace(tempwad_t * wad, int num) { //arg_8 seems to always be zero.  Is unused.

   int totalsize;

   //todo: make not suck, actually CHECK return codes.  Should it allow for failure?
   totalsize = num * sizeof(tempdata_t);

   wad->padding04 = Q_Malloc(totalsize);
   Q_memset(wad->padding04, 0, totalsize);
}

//These two init funcitons seem to be parts of W_LoadWadFile
void Draw_CacheWadInitFromFile(hl_file_t * file, int size, char * filename, int num, tempwad_t * wad) {

   wadinfo_t header;
   wadlumpinfo_t * lumps;
   int var_14;

   //This doesnt HAVE to read the requested amount of data,
   //but rather than fix that 'can't possibly happen' bug, I'm
   //going to try finishing this as quickly as I can.
   FS_Read(&header, sizeof(wadinfo_t), 1, file);

   if(header.identification[0] != 'W' ||
      header.identification[1] != 'A' ||
      header.identification[2] != 'D' ||
      header.identification[3] != '3') {

      Sys_Error("%s: Wad file %s doesn't have WAD3 id\n", __FUNCTION__, filename);
   }

   wad->padding10 = Q_Malloc(size - header.infotableofs);

   FS_Seek(file, header.infotableofs, 0);
   FS_Read(wad->padding10, size - header.infotableofs, 1, file);

   for(var_14 = 0, lumps = wad->padding10; var_14 < header.numlumps; var_14++, lumps++) {

      W_CleanupName(lumps->name, lumps->name);
   }

   wad->num_items2 = header.numlumps;
   wad->num_items = 0;
   wad->max_items = num;
   wad->filename = strdup(filename);

   Draw_AllocateCacheSpace(wad, num);

   wad->max_items2 = 0;
   wad->padding1C = NULL;
}
void Draw_CacheWadInit(char * filename, int num, tempwad_t * wad) {

   int i;
   hl_file_t * fp;


   fp = FS_Open(filename, "rb");
   if(fp == NULL) {
      Sys_Error("%s: Couldn't open file %s.", __FUNCTION__, filename);
   }

   i = FS_Size(fp);
   Draw_CacheWadInitFromFile(fp, i, filename, num, wad);
   FS_Close(fp);
}


int Draw_CacheReload(tempwad_t * wad, int index, wadlumpinfo_t * lump, tempdata_t * datawenptycache, char * name, char * originalname) {

   hl_file_t * fp;
   int len;
   char * path;
   byte * var_8;

   if(wad->padding20 <= 0) {

      fp = FS_Open(wad->filename, "rb");
   }
   else {

      path = wad->padding24[wad->padding28[index]];
      fp = FS_OpenPathID(wad->filename, "rb", path);
   }

   if(fp == NULL) { return(0); }


   len = FS_Size(fp);
   var_8 = (byte *)Cache_Alloc(&datawenptycache->cache, lump->size + wad->max_items2 + 1, name);

   if(var_8 == NULL) {
      Sys_Error("%s: not enough space for %s in %s", __FUNCTION__, originalname, wad->filename);
   }
   var_8[lump->size + wad->max_items2] = '\0'; //Capping it with a null?

   FS_Seek(fp, lump->filepos, 0);
   FS_Read((var_8 + wad->max_items2), lump->size, 1, fp);
   FS_Close(fp);

   if(wad->padding1C != 0) {

      wad->padding1C(wad, var_8);
   }

   return(1);
}
int Draw_CacheIndex(tempwad_t * wad, char * str) {

   tempdata_t * var_4;
   int i;


   var_4 = wad->padding04;
   for(i = 0; i < wad->num_items; i++, var_4++) {

      if(Q_strcmp(var_4->name, str) == 0) { break; }
   }

   if(wad->num_items == i) {
      if(wad->num_items == wad->max_items) {

         Sys_Error("%s: Cache wad (%s) out of %d entries.", __FUNCTION__, wad->filename, wad->max_items);
      }
      else {

         wad->num_items++;
         Q_strncpy(var_4->name, str, 0x3F);
         var_4->name[0x3f] = '\0';
      }
   }

   return(i);
}
texture_t * Draw_CacheGet(tempwad_t * wad, int index) {

   tempdata_t * var_4;
   texture_t * var_c;
   char * var_10;
   char var_20[0x40];
   char var_30[0x40];
   wadlumpinfo_t * var_34;
   int i;


   if(wad->num_items >= index) {
      Sys_Error("%s: Requested item %d of wad %s when only %d elements are loaded.", __FUNCTION__, index, wad->filename, wad->num_items);
   }

   var_4 = &wad->padding04[index];
   var_c = (texture_t *)Cache_Check(&var_4->cache);

   if(var_c != NULL) { return(var_c); }

   var_10 = var_4->name;
   COM_FileBase(var_10, var_20);
   W_CleanupName(var_20, var_30);

   var_34 = wad->padding10;
   for(i = 0; i < wad->num_items2; i++, var_34++) {

      if(Q_strcmp(var_30, var_34->name) == 0) { break; }
   }

   if(i >= wad->num_items2) { return(NULL); }
   if(Draw_CacheReload(wad, i, var_34, var_4, var_30, var_10) == 0) { return(NULL); }

   var_c = (texture_t *)var_4->cache.data;
   if(var_c == NULL) {

      Sys_Error("%s: Failed to load %s.", __FUNCTION__, wad->filename);
   }

   return(var_c);
}

int Draw_CacheByIndex(tempwad_t * wad, int a, int b) {

   int i;
   char var_28[0x20];
   tempdata_t * var_4;

   Q_snprintf(var_28, sizeof(var_28)-1, "%03i%02i", b, a);
   var_28[sizeof(var_28)-1] = '\0';

   for(i = 0, var_4 = wad->padding04; i < wad->num_items; i++, var_4++) {
      if(Q_strcmp(var_4->name, var_28) == 0) { break; }
   }

   if(i == wad->num_items) {
      if(wad->num_items == wad->max_items) {
         Sys_Error("%s: Cache wad (%s) out of %d entries.", __FUNCTION__, wad->filename, wad->max_items);
      }
      else {
         wad->num_items++;
         strcpy(var_4->name, var_28); //var_28 is 0x20 bytes, var4->name 0x40.
      }
   }

   return(i);
}

/* DESCRIPTION: Draw_CacheWadHandler
// LOCATION:
// PATH: CustomDecal_Init
//
// Looks complicated, what with the function pointer, but it really just
// assigns a few members of a struct.
*/
void Draw_CacheWadHandler(tempwad_t * wad, void (*draw)(struct tempwad_s *, byte *), int num) {

   wad->max_items2 = num;
   wad->padding1C = draw;
}

void Draw_MiptexTexture(tempwad_t * wad, byte * data) {

   //This function is just screwy...
   unsigned int i, pixels, var_3c;
   miptex_t var_30;
   miptex_t * ptr;
   miptex2_t * ptr2;
   byte * ptr3;

   if(wad->max_items2 != 0x18) { Sys_Error("%s: Bad cached wad %s", __FUNCTION__, wad->filename); }

   ptr = (miptex_t *)(data + wad->max_items2);
   var_30 = *ptr;
   ptr2 = (miptex2_t *)data;

   Q_memcpy(ptr2->name, var_30.name, 0x10);

   ptr2->width = letohost32(var_30.width);
   ptr2->height = letohost32(var_30.height);
   ptr2->padding18 = 0;
   ptr2->padding1C = 0;
   ptr2->padding20 = 0;
   ptr2->padding24 = 0;
   ptr2->padding28 = 0;

   for(i = 0; i < MIPLEVELS; i++) {

      ptr2->offsets[i] = letohost32(var_30.offsets[i]) + wad->max_items2;
   }

   pixels = ptr2->width * ptr2->height;
   pixels += (pixels >> 2) + (pixels >> 4) + (pixels >> 6); //? overhead?

   ptr2->padding3c = pixels + ptr2->offsets[0] + 2;
   var_3c = *(short *)ptr + pixels + 0x28;

   //Okay, I'm sick of trying to ID file structs, so screw it.
   ptr3 = (byte *)ptr2 + ptr2->padding3c;

   if(gfCustomBuild != 0) {

      Q_strncpy(ptr2->name, szCustName, sizeof(szCustName)-1);
      ptr2->name[sizeof(szCustName)-1] = '\0';
   }

   if(ptr3[0x2FD] == 0 && ptr3[0x2FD] == 0 && ptr3[0x2FD] == 0xFF) {
      ptr2->name[0] = '{';
   }
   else {
      ptr2->name[0] = '}';
   }


   for(i = 0; i < var_3c; i++) {

      ptr3[3*i    ] = texgammatable[ptr3[3*i    ] & 0xFF];
      ptr3[3*i + 1] = texgammatable[ptr3[3*i + 1] & 0xFF];
      ptr3[3*i + 2] = texgammatable[ptr3[3*i + 2] & 0xFF];
   }
}

int Draw_CustomCacheWadInit(int arg_0, tempwad_t * wad, byte * data, int size) {

   wadinfo_t var_10;


   var_10 = *((wadinfo_t *)data); //copies 12 bytes total.

   if(Q_memcmp(var_10.identification, "WAD3", 4) != 0) {
      Con_Printf("%s: custom wad doesn't have a valid header.\n", __FUNCTION__);
      return(0);
   }
   if(var_10.numlumps != 1) {
      Con_Printf("%s: custom wad doesn't have a single lump.\n", __FUNCTION__);
      return(0);
   }
   if(var_10.infotableofs < 1 || (signed)sizeof(wadlumpinfo_t) + var_10.infotableofs != size) {
      Con_Printf("%s: custom wad has an abnormal offset.\n", __FUNCTION__);
      return(0);
   }

   //with only one lump, we can take a few shortcuts.  todo Use ptr, don't trust compiler to optimize repeated derefs
   wad->padding10 = Q_Malloc(sizeof(wadlumpinfo_t));
   if(wad->padding10 == NULL) {
      Con_Printf("%s: Memory error.  Did you know memory nowadays is little more than a case of beer?.\n", __FUNCTION__);
      return(0);
   }
   Q_memcpy(wad->padding10, data + var_10.infotableofs, sizeof(wadlumpinfo_t));

   W_CleanupName(wad->padding10->name, wad->padding10->name);

   if(wad->padding10->disksize != wad->padding10->size || wad->padding10->size < 0) {
      Con_Printf("%s: Bad lump sizes.\n", __FUNCTION__);
      Q_Free(wad->padding10);
      wad->padding10 = NULL;
      return(0);
   }

   if(wad->padding10->filepos < 12 || wad->padding10->filepos + wad->padding10->disksize > var_10.infotableofs) {
      Con_Printf("%s: Bad file offset.\n", __FUNCTION__);
      Q_Free(wad->padding10);
      wad->padding10 = NULL;
      return(0);
   }

   //So we've done sanity checks, now...

   wad->filename = strdup("pldecal.wad");
   if(wad->filename == NULL) {
      Con_Printf("%s: memory failure.\n", __FUNCTION__);
      Q_Free(wad->padding10);
      wad->padding10 = NULL;
      return(0);
   }

   wad->num_items2 = var_10.numlumps;
   wad->num_items = 0;
   wad->max_items = arg_0;
   wad->max_items2 = 0;
   wad->padding1C = NULL;
   Draw_AllocateCacheSpace(wad, arg_0);

   return(1);
}

int Draw_ValidateCustomLogo(tempwad_t * wad, byte * data, wadlumpinfo_t * wadlump) {

   unsigned int i, pixels, pixels2, var_78, var_7c;
   miptex_t var_6C;
   miptex2_t var_40;
   miptex_t * ptr;
   miptex2_t * ptr2;


   if(wad->max_items2 != 0x18) {
      Con_Printf("%s: Bad cached wad %s.\n", __FUNCTION__, wad->filename);
      return(0);
   }

   ptr = (miptex_t *)(data + wad->max_items2);
   var_6C = *ptr;

   ptr2 = (miptex2_t *)data;
   var_40 = *ptr2;

   Q_memcpy(var_40.name, var_6C.name, 0x10);

   var_40.width = letohost32(var_6C.width);
   var_40.height = letohost32(var_6C.height);
   var_40.padding18 = 0;
   var_40.padding1C = 0;
   var_40.padding20 = 0;
   var_40.padding24 = 0;
   var_40.padding28 = 0;

   for(i = 0; i < MIPLEVELS; i++) {

      var_40.offsets[i] = letohost32(var_6C.offsets[i]) + wad->max_items2;
   }

   pixels = var_40.width * var_40.height;
   pixels += (pixels >> 2) + (pixels >> 4) + (pixels >> 6); //? overhead?

   var_78 = pixels + ptr2->offsets[0] + 2;
   var_7c = *(short *)ptr + pixels + 0x28;

   if(var_40.width < 1 || var_40.width > 0x100 || var_40.height < 1 || var_40.height > 0x100) {

      Con_Printf("%s: Bad decal dimensions %s.\n", __FUNCTION__, wad->filename);
      return(0);
   }

   pixels2 = var_40.width * var_40.height;
   for(i = 0; i < MIPLEVELS-1; i++, pixels2 = pixels2 >> 2) {

      if(var_6C.offsets[i] + pixels2 != var_6C.offsets[i+1]) {

         Con_Printf("%s: Somethinfg was wrong with %s's graphic.\n", __FUNCTION__, wad->filename);
         return(0);
      }
   }

   if(var_7c > 0x100) {
      Con_Printf("%s: Bad decal palette (%s).\n", __FUNCTION__, wad->filename);
      return(0);
   }

   if(letohost32(var_6C.offsets[0]) + pixels + (3 * var_7c) + 2 > (unsigned)wadlump->disksize) {
      Con_Printf("%s: Actual palette size didn't match reported size (%s).\n", __FUNCTION__, wad->filename);
      return(0);
   }

   return(1);
}
int Draw_CacheLoadFromCustom(char * name, tempwad_t * wad, byte * data, int size, tempdata_t * cacheitem) { //size is unused.

   int var_4;
   byte * var_8;
   wadlumpinfo_t * ptr;

   //wha?
   if(Q_strlen(name) > 4) {

      var_4 = Q_atoi(name + 3);
      if(var_4 < 0 || var_4 > wad->num_items2) { return(0); }
   }
   else {
      var_4 = 0;
   }

   ptr = &(wad->padding10[var_4]);
   var_8 = Cache_Alloc(&(cacheitem->cache), wad->max_items2 + ptr->size + 1, name);

   if(var_8 == NULL) {
      Sys_Error("%s: %s broke the cache.\n", __FUNCTION__, name);
   }

   var_8[wad->max_items2 + ptr->size] = '\0';
   Q_memcpy(var_8 + wad->max_items2, data + ptr->filepos, ptr->size);

   if(Draw_ValidateCustomLogo(wad, var_8, ptr) == 0) { return(0); } //should we free?

   gfCustomBuild = 1;
   Q_strcpy(szCustName, "T");
   Q_memcpy(szCustName+1, name, 5);
   szCustName[5] = '\0';

   if(wad->padding1C != NULL) {
      wad->padding1C(wad, var_8);
   }

   gfCustomBuild = 0;
   return(1);
}

void * Draw_CustomCacheGet(tempwad_t * wad, byte * data, int size, int argc) {

   void * i;
   char buf[0x40];
   tempdata_t * ptr;


   if(argc >= wad->num_items) {
      Sys_Error("%s: Cache wad indexed before load.\n", __FUNCTION__);
   }

   ptr = &(wad->padding04[argc]);

   i = Cache_Check(&(ptr->cache));
   if(i != NULL) { return(i); }

   COM_FileBase(ptr->name, buf);
   W_CleanupName(buf, buf);

   if(Draw_CacheLoadFromCustom(buf, wad, data, size, ptr) == 0) { return(NULL); }

   if(ptr->cache.data == NULL) {
      Sys_Error("%s: Failed to load %s.\n", __FUNCTION__);
   }

   return(ptr->cache.data);
}
void Draw_FreeWad(tempwad_t * wad) {

   int i;
   tempdata_t * ptr;

   if(wad == NULL) { return; }

   if(wad->padding10 != NULL) { Q_Free(wad->padding10); wad->padding10 = NULL; }
   if(wad->filename != NULL) { Q_Free(wad->filename); wad->filename = NULL; }

   if(wad->padding24 != NULL) {
      for(i = 0; i < wad->padding20; i++) {
         Q_Free(wad->padding24[i]);
      }
      Q_Free(wad->padding24);
   }

   if(wad->padding28 != NULL) { Q_Free(wad->padding28); wad->padding28 = NULL; }

   if(wad->padding04 != NULL) {
      for(ptr = wad->padding04, i = 0; i < wad->num_items; i++, ptr++) {

         if(Cache_Check(&(ptr->cache))) {
            Cache_Free(&(ptr->cache));
         }
      }
      Q_Free(wad->padding04);
   }
}


/* DESCRIPTION: CustomDecal_Init
// LOCATION:
// PATH: COM_CreateCustomization, CustomDecal_Validate
//
//
*/
int CustomDecal_Init(tempwad_t * wad, byte * data, int size, int playernum) {

   int i, ret;


   ret = Draw_CustomCacheWadInit(0x10, wad, data, size);

   if(ret == 0) { return(0); }

   Draw_CacheWadHandler(wad, Draw_MiptexTexture, 0x18);
   for(i = 0; i < wad->num_items2; i++) {
      Draw_CacheByIndex(wad, i, playernum);
   }

   return(ret);
}

void * CustomDecal_Validate(byte * data, int size) {

   tempwad_t * wad;
   void * i;


   wad = Q_Malloc(sizeof(tempwad_t));
   if(wad == NULL) { return(NULL); } //Technically not due to validation, but...

   Q_memset(wad, 0, sizeof(*wad)); //is this necessary or not?

   if(CustomDecal_Init(wad, data, size, 0) != 0) {

      i = Draw_CustomCacheGet(wad, data, size, 0);
   }
   else {
      i = NULL;
   }

   Draw_FreeWad(wad);
   Q_Free(wad);

   return(i);
}



/*** Draw functions that use the wad static ***/

int Draw_DecalCount() {

   if(decal_wad == NULL) { return(0); }

   return(decal_wad->num_items2);
}
int Draw_DecalSize(int i) {

   if(decal_wad == NULL || decal_wad->num_items2 <= i) { return(0); }

   return(decal_wad->padding10[i].size);
}
char * Draw_DecalName(int i) {

   if(decal_wad == NULL || decal_wad->num_items2 <= i) { return(0); }

   return(decal_wad->padding10[i].name);
}
void Draw_DecalShutdown() {

   if(decal_wad != NULL) {
      Draw_FreeWad(decal_wad);
      Q_Free(decal_wad);
      decal_wad = NULL;
   }
}



unsigned int Decal_CountLumps(decalstruct_t * arg_0) {

   unsigned int i = 0;

   while(arg_0 != NULL) {
      arg_0 = arg_0->next;
      i++;
   }
   return(i);
}
unsigned int Decal_SizeLumps(decalstruct_t * arg_0) {

   return(Decal_CountLumps(arg_0) * sizeof(wadlumpinfo_t));
}

void Decal_ReplaceOrAppendLump(decalstruct_t ** OUT_lump, wadlumpinfo_t * IN_lump, int arg_8) {

   decalstruct_t * var_4;


   for(var_4 = *OUT_lump; var_4 != NULL; var_4 = var_4->next) {

      if(Q_strcasecmp(IN_lump->name, var_4->padding00->name) == 0) {

         Q_Free(var_4->padding00);

         var_4->padding00 = Q_Malloc(sizeof(wadlumpinfo_t));
         CHECK_MEMORY_MALLOC(var_4->padding00);

         Q_memcpy(var_4->padding00, IN_lump, sizeof(wadlumpinfo_t));
         var_4->padding04 = arg_8;
         return;
      }
   }

   var_4 = Q_Malloc(sizeof(decalstruct_t));
   CHECK_MEMORY_MALLOC(var_4);

   var_4->padding00 = Q_Malloc(sizeof(wadlumpinfo_t));
   CHECK_MEMORY_MALLOC(var_4->padding00);

   Q_memcpy(var_4->padding00, IN_lump, sizeof(wadlumpinfo_t));
   var_4->padding04 = arg_8;
   var_4->next = *OUT_lump;
   *OUT_lump = var_4;
}


void Decal_MergeInDecals(tempwad_t * wad, char * dir) {

   int i;
   decalstruct_t * var_10, * var_14, * var_18;
   tempwad_t * temp;
   wadlumpinfo_t * var_C;


   if(wad == NULL) { Sys_Error("%s: Passed NULL wad.\n", __FUNCTION__); }

   if(decal_wad == NULL) {

      decal_wad = wad;
      wad->padding20 = 1;

      wad->padding24 = Q_Malloc(sizeof(char *));
      CHECK_MEMORY_MALLOC(wad->padding24);
      wad->padding24[0] = strdup(dir);
      CHECK_MEMORY_MALLOC(wad->padding24[0]);
      wad->padding28 = Q_Malloc(wad->max_items * sizeof(int *));
      CHECK_MEMORY_MALLOC(wad->padding28);

      Q_memset(wad->padding28, 0, wad->max_items * sizeof(int *));
   }
   else {

      var_10 = NULL;
      temp = Q_Malloc(sizeof(tempwad_t));
      CHECK_MEMORY_MALLOC(temp);

      Q_memset(temp, 0, sizeof(tempwad_t));

      for(i = 0; i < decal_wad->num_items2; i++) {
         Decal_ReplaceOrAppendLump(&var_10, &(decal_wad->padding10[i]), 0);
      }

      for(i = 0; i < wad->num_items2; i++) {
         Decal_ReplaceOrAppendLump(&var_10, &(wad->padding10[i]), 1);
      }

      temp->num_items2 = Decal_CountLumps(var_10);
      temp->num_items = 0;
      temp->max_items = decal_wad->max_items;
      temp->filename = strdup(decal_wad->filename);
      CHECK_MEMORY_MALLOC(temp->filename);

      Draw_AllocateCacheSpace(temp, temp->max_items);

      temp->padding1C = decal_wad->padding1C;
      temp->max_items2 = decal_wad->max_items2;
      temp->padding28 = Q_Malloc(temp->max_items * sizeof(int *));
      CHECK_MEMORY_MALLOC(temp->padding28);

      Q_memset(temp->padding28, 0, temp->max_items * sizeof(int *));
      temp->padding20 = 2;
      temp->padding24 = Q_Malloc(sizeof(char *) * 2);
      CHECK_MEMORY_MALLOC(temp->padding24);

      temp->padding24[0] = strdup(decal_wad->padding24[0]);
      CHECK_MEMORY_MALLOC(temp->padding24[0]);
      temp->padding24[1] = strdup(dir);
      CHECK_MEMORY_MALLOC(temp->padding24[1]);

      temp->padding10 = Q_Malloc(Decal_SizeLumps(var_10));
      CHECK_MEMORY_MALLOC(temp->padding10);

      i = 0;
      var_14 = var_10;
      var_C = temp->padding10;
      while(var_14 != NULL) {

         var_18 = var_14->next;
         Q_memcpy(var_C, var_14->padding00, 0x20);
         Q_Free(var_14->padding00);

         temp->padding28[i] = (var_14->padding04 != 0);
         Q_Free(var_14);

         var_14 = var_18;
         i++;
         var_C++;
      }

      Draw_FreeWad(decal_wad);
      if(decal_wad != NULL) { Q_Free(decal_wad); }
      decal_wad = temp;

      Draw_FreeWad(wad);
      Q_Free(wad);
   }
}

//Don't ask me, I just work here
void Decal_Init() {

   int i, size;
   char gamedir[2][16] = { "DEFAULTGAME", "" };
   tempwad_t * wad;
   hl_file_t * fp;


   Draw_DecalShutdown();

   for(i = 0; i < 2; i++) {

      fp = FS_OpenPathID("decals.wad", "rb", gamedir[i]);
      if(fp == NULL) {

         if(i != 0) { continue; }
         Sys_Error("%s: Couldn't find decals.wad in default path.\n", __FUNCTION__);
      }

      size = FS_Size(fp);
      wad = Q_Malloc(sizeof(tempwad_t));
      CHECK_MEMORY_MALLOC(wad);
      Q_memset(wad, 0, sizeof(tempwad_t));

      Draw_CacheWadInitFromFile(fp, size, "decals.wad", 0x200, wad);
      Decal_MergeInDecals(wad, gamedir[i]);

      FS_Close(fp);
   }

   global_sv_decalnamecount = Draw_DecalCount();
   if(global_sv_decalnamecount > 0x200) {
      Sys_Error("%s: Too many decals.\n", __FUNCTION__);
   }

   for(i = 0; i < global_sv_decalnamecount; i++) {

      Q_memset(global_sv_decalnames[i], 0, 0x11);
      strncpy(global_sv_decalnames[i], Draw_DecalName(i), 0x0F);
   }
}





