#include <stdio.h>
#include <stdlib.h>

#include "hpak.h"
#include "cmd.h"
#include "cvar.h"
#include "report.h"
#include "Q_memdef.h"
#include "FS.hpp"
#include "crc.h"



hpak_t * gp_hpak_queue = NULL;
hpak_header_t hash_pack_header = { { 0, 0, 0, 0 }, 0, 0 };
hpak_dir_container_t hash_pack_dir = { 0, NULL };


static cvar_t cvar_hpk_maxsize = { "hpk_maxsize", "0", FCVAR_ARCHIVE, 0.0, NULL };

//unused it seems
void HPAK_GetItem() { printf("%s called", __FUNCTION__); exit(99); }

//You can do without these, methinks.
void HPAK_Extract_f() { printf("%s called", __FUNCTION__); exit(99); }
void HPAK_List_f() { printf("%s called", __FUNCTION__); exit(99); }
void HPAK_Remove_f() { printf("%s called", __FUNCTION__); exit(99); }
void HPAK_Validate_f() { printf("%s called", __FUNCTION__); exit(99); }

/* DESCRIPTION: HPAK_AddToQueue
// PATH: HPAK_AddLump
//
// Mallocs a new hpak_t structure, and copies the resources into it.  Then
// links it.  Resources can be pulled from a file or delivered in an array--
// if the file method is used, seek() must have already been called.
*/
void HPAK_AddToQueue(char * name, resource_t * DirEnt, byte * data, hl_file_t * filep) {

   hpak_t * ptr;

   ptr = Q_Malloc(sizeof(hpak_t));
   CHECK_MEMORY_MALLOC(ptr);

   ptr->name = strdup(name);
   CHECK_MEMORY_MALLOC(ptr->name);

   ptr->HpakResource = (*DirEnt);
   ptr->size = DirEnt->nDownloadSize;

   ptr->data = Q_Malloc(ptr->size);
   CHECK_MEMORY_MALLOC(ptr->data);

   if(data != NULL) {
      Q_memcpy(ptr->data, data, ptr->size);
   }
   else if(filep != NULL) {
      FS_Read(ptr->data, ptr->size, 1, filep);
   }
   else {
      Sys_Error("%s: Called with no data pointer and no file pointer.\n");
   }

   ptr->next = gp_hpak_queue;
   gp_hpak_queue = ptr;
}

/* DESCRIPTION: HPAK_CreatePak
// PATH: HPAK_AddLump
//
// Writes a file.  A file with the given HPAK structure.  Plenty can go wrong,
// but probably won't, and an odd note--the thing was half-written for writing
// variable count HPAK files.
*/
void HPAK_CreatePak(char * filename, resource_t * DirEnt, byte * data, hl_file_t * filep) {

   int filelocation;
   char string[0x100];
   char md5[16];
   char * temp;
   MD5Context_t MD5_Hash;
   hl_file_t * filewb;


   if((filep != NULL && data != NULL) || (filep == NULL && data == NULL)) {
      Con_Printf("%s: ONE, and only ONE source has to be non-null.\n", __FUNCTION__);
      return;
   }

   Q_strncpy(string, filename, sizeof(string)-4);
   string[sizeof(string)-4] = '\0';
   COM_DefaultExtension(string, ".hpk");

   Con_Printf("%s: creating HPAK %s.\n", __FUNCTION__, string);
   filewb = FS_Open(string, "wb");
   if(filewb == NULL) {
      Con_Printf("%s: Couldn't open %s for writing.\n", __FUNCTION__, string);
      return;
   }

   //Let's hash it.

   memset(&MD5_Hash, 0, sizeof(MD5Context_t));
   MD5Init(&MD5_Hash);
   if(data == NULL) {

      //there are better ways
      filelocation = FS_Tell(filep);
      temp = Q_Malloc(DirEnt->nDownloadSize);
      if(temp == NULL) { return; } //I'll tidy it later.
      FS_Read(temp, DirEnt->nDownloadSize, 1, filep);
      FS_Seek(filep, filelocation, SEEK_SET);

      MD5Update(&MD5_Hash, temp, DirEnt->nDownloadSize);
      Q_Free(temp);
   }
   else {

      MD5Update(&MD5_Hash, data, DirEnt->nDownloadSize);
   }

   MD5Final(md5, &MD5_Hash);
   if(Q_memcmp(md5, DirEnt->rgucMD5_hash, 0x10) != 0) {
      Con_Printf("%s: Encountered mismatched checksums when writing %s.  Ignoring.\n", __FUNCTION__, string);
      return;
   }

   Q_memcpy(&(hash_pack_header.hpak), "HPAK", 4);
   hash_pack_header.version = 1;
   hash_pack_header.seek = 0;

   FS_Write(&hash_pack_header, sizeof(hash_pack_header), 1, filewb);

   hash_pack_dir.count = 1;
   hash_pack_dir.dirs = Q_Malloc(sizeof(hpak_dir_t));
   CHECK_MEMORY_MALLOC(hash_pack_dir.dirs);

   hash_pack_dir.dirs[0].DirectoryResource = *DirEnt;
   hash_pack_dir.dirs[0].seek = FS_Tell(filewb);
   hash_pack_dir.dirs[0].size = DirEnt->nDownloadSize;

   if(data == NULL) {
      COM_CopyFileChunk(filewb, filep, hash_pack_dir.dirs[0].size);
   }
   else {
      FS_Write(data, hash_pack_dir.dirs[0].size, 1, filewb);
   }

   filelocation = FS_Tell(filewb);
   FS_Write(&(hash_pack_dir.count), 4, 1, filewb); //should be 1
   //omit for loop, executes one time
   FS_Write(&(hash_pack_dir.dirs[0]), sizeof(hpak_dir_t), 1, filewb);

   Q_Free(hash_pack_dir.dirs);
   Q_memset(&hash_pack_dir, 0, sizeof(hpak_dir_container_t));

   hash_pack_header.seek = filelocation;
   FS_Seek(filewb, 0, SEEK_SET);
   FS_Write(&(hash_pack_header), sizeof(hpak_header_t), 1, filewb);
   FS_Close(filewb);
}

/* DESCRIPTION: HPAK_FindResource
// PATH: HPAK functions
//
// Runs through all of the elements in an HPAK file looking for a matching hash.
*/
int HPAK_FindResource(hpak_dir_container_t * arg_0_container, char * arg_4_md5, resource_t * OUT_arg8_resource) {

   int i;


   for(i = 0; i < arg_0_container->count; i++) {

      if(Q_memcmp(arg_0_container->dirs[i].DirectoryResource.rgucMD5_hash, arg_4_md5, 0x10) == 0) {

         //matched hash.  I think we should check size as well, personally.
         if(OUT_arg8_resource != NULL) {

            *OUT_arg8_resource = arg_0_container->dirs[i].DirectoryResource; //full copy
         }
         return(1);
      }
   }
   return(0);
}

/* DESCRIPTION: HPAK_AddLump
// PATH: HPAK_AddLump, SV_ProcessFile
//
// Nercilessly tortures programmers with lots of structs on the stacks and
// lots and lots of data manipulation.  It shook me off about halfway through,
// so if you want to know what it does, you're asking the wrong guy.
*/
void HPAK_AddLump(int BoolQueue, char * name, resource_t * DirEnt, byte * data, hl_file_t * filep) {

   int position, length;
   byte * temp;
   char string1[0x100];
   char string2[0x100];
   char md5[16];
   MD5Context_t MD5_Hash;
   hl_file_t * filep1;
   hl_file_t * filep2;
   hpak_dir_container_t File1container;
   hpak_dir_container_t File2container;
   int var_4;
   hpak_dir_t * var_340;


   //some arg checks
   if(name == NULL || name[0] == '\0') {
      Con_Printf("%s: Bad arguments.  No name.\n", __FUNCTION__);
      return;
   }
   if(DirEnt == NULL) {
      Con_Printf("%s: Bad arguments.  No lump.\n", __FUNCTION__);
      return;
   }
   if(data == NULL && filep == NULL) {
      Con_Printf("%s: Bad arguments.  No data, no file.\n", __FUNCTION__);
      return;
   }
   if(DirEnt->nDownloadSize < 0x400 || DirEnt->nDownloadSize > 0x20000) {
      Con_Printf("%s: Passed lump size is out of our allowed range (%s %i).\n", __FUNCTION__, DirEnt->szFileName, DirEnt->nDownloadSize);
      return;
   }

   //hash it
   memset(&MD5_Hash, 0, sizeof(MD5Context_t));
   MD5Init(&MD5_Hash);
   if(data == NULL) {

      //there are better ways
      position = FS_Tell(filep);
      temp = Q_Malloc(DirEnt->nDownloadSize);
      if(temp == NULL) { return; } //I'll tidy it later.
      FS_Read(temp, DirEnt->nDownloadSize, 1, filep);
      FS_Seek(filep, position, SEEK_SET);

      MD5Update(&MD5_Hash, temp, DirEnt->nDownloadSize);
      Q_Free(temp);
   }
   else {

      MD5Update(&MD5_Hash, data, DirEnt->nDownloadSize);
   }

   MD5Final(md5, &MD5_Hash);
   if(Q_memcmp(md5, DirEnt->rgucMD5_hash, 0x10) != 0) {
      Con_Printf("%s: Encountered mismatched checksums when adding %s.\n", __FUNCTION__, DirEnt->szFileName);
      return;
   }

   if(BoolQueue) {

      HPAK_AddToQueue(name, DirEnt, data, filep);
      return;
   }

   //Open files
   Q_strncpy(string1, name, sizeof(string1)-4);
   string1[sizeof(string1)-4] = '\0';
   COM_DefaultExtension(string1, ".hpk");

   filep1 = FS_Open(string1, "rb");
   if(filep1 == NULL) { //This is okay

      HPAK_CreatePak(name, DirEnt, data, filep);
      return;
   }

   COM_StripExtension(string1, string2);
   COM_DefaultExtension(string2, ".hp2");
   filep2 = FS_Open(string2, "w+b");
   if(filep2 == NULL) { //This is not

      FS_Close(filep1);
      Con_Printf("%s: Couldn't open %s.\n", __FUNCTION__, string2);
      return;
   }

   //Load headers
   FS_Read(&hash_pack_header, sizeof(hpak_header_t), 1, filep1);
   if(hash_pack_header.version != 1) { //We don't check the HPAK bit for some reason.

      FS_Close(filep1);
      FS_Close(filep2);
      Con_Printf("%s: %s does not have a valid header.\n", __FUNCTION__, string2);
   }

   length = FS_Size(filep1);
   COM_CopyFileChunk(filep2, filep1, length);

   FS_Seek(filep1, hash_pack_header.seek, SEEK_SET);
   FS_Read(&(File1container.count), 4, 1, filep1);
   if(File1container.count < 1 || File1container.count > 0x8000) {

      FS_Close(filep1);
      FS_Close(filep2);
      Sys_Error("%s: %s has an invalid element count.\n", __FUNCTION__, string1);
      return;
   }

   //load data
   File1container.dirs = Q_Malloc(sizeof(hpak_dir_t) * File1container.count);
   if(File1container.dirs == NULL) {

      FS_Close(filep1);
      FS_Close(filep2);
      Con_Printf("%s: file %s used up the last of our memory.\n", __FUNCTION__, string1);
      return;
   }

   FS_Read(File1container.dirs, sizeof(hpak_dir_t) * File1container.count, 1, filep1);
   FS_Close(filep1);

   if(HPAK_FindResource(&File1container, DirEnt->rgucMD5_hash, NULL) != 0) {

      FS_Close(filep2);
      Q_Free(File1container.dirs);
   }

   //Make a new container
   File2container.count = File1container.count;
   File2container.dirs = Q_Malloc(sizeof(hpak_dir_t) * File2container.count);
   if(File2container.dirs == NULL) {

      FS_Close(filep2);
      Q_Free(File1container.dirs);
      Con_Printf("%s: file %s used up the last of our memory.\n", __FUNCTION__, string1);
      return;
   }

   Q_memcpy(File2container.dirs, File1container.dirs, File1container.count);

   //You lost me, chief.
   var_340 = NULL;
   for(var_4 = 0; var_4 < File1container.count; var_4++) {

      //A bug?  What good is this sort of comparison with hashes?
      if(Q_memcmp(File1container.dirs[var_4].DirectoryResource.rgucMD5_hash, DirEnt->rgucMD5_hash, 0x10) < 0) {

         var_340 = &(File1container.dirs[var_4]);
         while(var_4 < File1container.count) {

            File2container.dirs[var_4 + 1] = File1container.dirs[var_4];
            var_4++;
         }
         break;
      }
   }

   if(var_340 == NULL) {
      var_340 = &(File2container.dirs[File2container.count-1]);
   }

   Q_memset(var_340, 0, sizeof(hpak_dir_t));
   FS_Seek(filep2, hash_pack_header.seek, SEEK_SET);
   var_340->DirectoryResource = *DirEnt;
   var_340->seek = FS_Tell(filep2);
   var_340->size = DirEnt->nDownloadSize;

   if(data == NULL) {
      COM_CopyFileChunk(filep2, filep, var_340->size);
   }
   else {
      FS_Write(data, var_340->size, 1, filep2);
   }

   hash_pack_header.seek = FS_Tell(filep2);
   FS_Write(&(File2container.count), 4, 1, filep2);

   for(var_4 = 0; var_4 < File2container.count; var_4++) {
      FS_Write(&(File2container.dirs[var_4]), sizeof(hpak_dir_t), 1, filep2);
   }

   //finally
   Q_Free(File1container.dirs);
   Q_Free(File2container.dirs);

   FS_Seek(filep2, 0, SEEK_SET);
   FS_Write(&hash_pack_header, sizeof(hpak_header_t), 1, filep2);
   FS_Close(filep2);
   FS_Unlink(string1);
   FS_Rename(string2, string1);
}


/* DESCRIPTION: HPAK_FlushHostQueue
// PATH: lots of HPAK functions
//
// Cleans out the hpak queue with many calls to AddLump.
*/
void HPAK_FlushHostQueue() {

   hpak_t * ptr;

   for(ptr = gp_hpak_queue; ptr != NULL; ptr = gp_hpak_queue) {

      gp_hpak_queue = gp_hpak_queue->next; //it's here so we get that null check in first
      HPAK_AddLump(0, ptr->name, &(ptr->HpakResource), ptr->data, 0);
      Q_Free(ptr->name);
      Q_Free(ptr->data);
      Q_Free(ptr);
   }
}

/* DESCRIPTION: HPAK_ValidatePak
// PATH: Host_Init->HPAK_CheckIntegrity
//
// Checks to make sure the PAK fles aren't obviously broken.  It normally
// DELETES the damaged ones--I don't think so, I'm debugging.  I'll
// just exit instead.
*/
void HPAK_ValidatePak(char * filename) {

   hl_file_t * filep;
   hpak_dir_t * dataDir;
   hpak_header_t TempHeader;
   byte * dataPak;
   int var_18_count, i;
   MD5Context_t MD5_Hash;
   char md5[16];


   HPAK_FlushHostQueue();


   filep = FS_Open(filename, "rb");
   if(filep == NULL) { return; }

   FS_Read(&TempHeader, sizeof(TempHeader), 1, filep);
   if(TempHeader.version != 1 || memcmp(&TempHeader.hpak, "HPAK", 4) != 0) {

      FS_Close(filep);
      Sys_Error("%s: %s does not have a valid HPAK header.\n", __FUNCTION__, filename);
   }

   FS_Seek(filep, TempHeader.seek, SEEK_SET);
   FS_Read(&var_18_count, 4, 1, filep);

   if(var_18_count < 1 || var_18_count > 0x8000) {

      FS_Close(filep);
      Sys_Error("%s: %s has an invalid element count.\n", __FUNCTION__, filename);
   }

   //I'd rather loop through and read it in smaller chunks.
   //It might be faster to just TELL the compiler to use a register
   //instead of relying on it opimizing out the repeated dereferences.
   dataDir = Q_Malloc(sizeof(hpak_dir_t) * var_18_count);
   CHECK_MEMORY_MALLOC(dataDir);
   FS_Read(dataDir, sizeof(hpak_dir_t) * var_18_count, 1, filep);

   for(i = 0; i < var_18_count; i++) {

      if(dataDir[i].size < 1 || dataDir[i].size > 0x1FFFF) { //odd max size

         Q_Free(dataDir);
         FS_Close(filep);
         Sys_Error("%s: %s has an invalid element size.\n", __FUNCTION__, filename);
      }

      dataPak = Q_Malloc(dataDir[i].size);
      CHECK_MEMORY_MALLOC(dataPak);

      FS_Seek(filep, dataDir[i].seek, SEEK_SET);
      FS_Read(dataPak, dataDir[i].size, 1, filep);

      Q_memset(&MD5_Hash, 0, sizeof(MD5Context_t));
      MD5Init(&MD5_Hash);
      MD5Update(&MD5_Hash, dataPak, dataDir[i].size);
      MD5Final(md5, &MD5_Hash);

      if(Q_memcmp(md5, dataDir[i].DirectoryResource.rgucMD5_hash, 0x10) != 0) {

         Q_Free(dataPak);
         Q_Free(dataDir);
         FS_Close(filep);
         Sys_Error("%s: hasing data from %s produced a mismatched hash.\n", __FUNCTION__, filename);
      }

      //At this point, it's passed our checks.  Free it, loop back around.
      Q_Free(dataPak);
   }

   Q_Free(dataDir);
   FS_Close(filep);
}

/* DESCRIPTION: HPAK_CheckIntegrity
// PATH: Host_Init
//
// Acts as a filename correcting proxy for HPAK_ValidatePak
*/
void HPAK_CheckIntegrity(char * filename) {

   char string[0x100];

   Q_strncpy(string, filename, sizeof(string)-4);
   string[sizeof(string)-4] = '\0';
   COM_DefaultExtension(string, ".hpk");
   COM_FixSlashes(string);
   HPAK_ValidatePak(string);
}

/* DESCRIPTION: HPAK_CheckSize
// PATH: SV_SpawnServer
//
// Similar to CheckIntegrity, but not as in depth.
*/
void HPAK_CheckSize(char * filename) {

   float maxsize; //Instincts tell me this should be an int, but I can think of ways that would screw us up.
   int size;
   char string[0x100];


   maxsize = cvar_hpk_maxsize.value;
   if(maxsize == 0) { return; }
   if(maxsize < 0) {

      Con_Printf("%s: hpk_maxsize < 0, setting to 0.\n", __FUNCTION__);
      Cvar_DirectSetValue(&cvar_hpk_maxsize, 0);
      return;
   }

   Q_strncpy(string, filename, sizeof(string)-4);
   string[sizeof(string)-4] = '\0';
   COM_DefaultExtension(string, ".hpk");
   COM_FixSlashes(string);

   maxsize *= 1000000;

   size = FS_FileSize(string);
   /* if size == 0, the file wasn't found.  Unfortunately 0 is also a legitimate
   // return value--all values are--so it's also possible the file WAS found,
   // but that it's empty.  It doesn't matter for this check though--0 is
   // less than whatever maxsize could be.
   */

   if(size > maxsize) {

      Sys_Error("%s:  The server has a max file size limit of %fMB, violated by %s.\n", __FUNCTION__, maxsize, string);
   }
}

/* DESCRIPTION: HPAK_ResourceForHash
// PATH: SV_BeginFileDownload, SV_EstimateNeededResources
//
// Runs through all of the elements in our hpak queue looking for a match, and
// if it doesn't find one, then calls HPAK_FindResource to look through the
// files themselves.
*/
int HPAK_ResourceForHash(char * filename, char * arg_4_md5, resource_t * OUT_arg8_resource) {

   hl_file_t * filep;
   hpak_t * var_124_hpak;
   hpak_dir_container_t hpakcontainer;
   hpak_header_t hpakheader;
   char string[0x100];
   int ret;


   if(gp_hpak_queue != NULL) {
      for(var_124_hpak = gp_hpak_queue; var_124_hpak != NULL; var_124_hpak = var_124_hpak->next) {

         if(Q_strcasecmp(var_124_hpak->name, filename) == 0 && Q_memcmp(var_124_hpak->HpakResource.rgucMD5_hash, arg_4_md5, 0x10) == 0) {
            if(OUT_arg8_resource != NULL) {
               *OUT_arg8_resource = var_124_hpak->HpakResource;
            }
            return(1);
         }
      }
   }

   Q_strncpy(string, filename, sizeof(string)-4);
   string[sizeof(string)-4] = '\0';
   COM_DefaultExtension(string, ".hpk");
   COM_FixSlashes(string);

   filep = FS_Open(string, "rb");
   if(filep == NULL) { return(0); }

   FS_Read(&hpakheader, sizeof(hpakheader), 1, filep);
   if(memcmp("HPAK", hpakheader.hpak, sizeof(hpakheader.hpak)) != 0) {

      Con_Printf("%s: file %s doesn't look like an HPAK file.\n", __FUNCTION__, string);
      FS_Close(filep);
      return(0);
   }
   if(hpakheader.version != 1) {

      Con_Printf("%s: file %s claimed to be version %i instead of %i.\n", __FUNCTION__, string, hpakheader.version, 1);
      FS_Close(filep);
      return(0);
   }

   FS_Seek(filep, hpakheader.seek, SEEK_SET);
   FS_Read(&(hpakcontainer.count), sizeof(hpakcontainer.count), 1, filep);

   if(hpakcontainer.count < 1 || hpakcontainer.count > 0x8000) {

      Con_Printf("%s: file %s has %i directory entries, which is not allowed.\n", __FUNCTION__, string, hpakcontainer.count);
      FS_Close(filep);
      return(0);
   }

   hpakcontainer.dirs = Q_Malloc(sizeof(hpak_dir_t) * hpakcontainer.count);
   if(hpakcontainer.dirs == NULL) { //don't crash, just treat it like an error.
      Con_Printf("%s: file %s used up the last of our memory.\n", __FUNCTION__, string);
      FS_Close(filep);
      return(0);
   }

   FS_Read(hpakcontainer.dirs, sizeof(hpak_dir_t) * hpakcontainer.count, 1, filep);
   ret = HPAK_FindResource(&hpakcontainer, arg_4_md5, OUT_arg8_resource);

   Q_Free(hpakcontainer.dirs);
   FS_Close(filep);
   return(ret);
}
/* DESCRIPTION: HPAK_ResourceForIndex
// PATH: HPAK_Remove_f
//
// We once again copy/paste our fine code into another function, this time
// making slight changes in order to search by index.  Not much to it that
// isn't in the other half-dozen loading functions.
*/
int HPAK_ResourceForIndex(char * filename, int arg_4_index, resource_t * OUT_arg8_resource) {

   hl_file_t * filep;
   hpak_dir_container_t hpakcontainer;
   hpak_header_t hpakheader;
   char string[0x100];


   if(global_cmd_source != cmd_SERVER) { return(0); }

   Q_strncpy(string, filename, sizeof(string)-4);
   string[sizeof(string)-4] = '\0';
   COM_DefaultExtension(string, ".hpk");
   COM_FixSlashes(string);

   filep = FS_Open(string, "rb");
   FS_Read(&hpakheader, sizeof(hpakheader), 1, filep);
   if(memcmp("HPAK", hpakheader.hpak, sizeof(hpakheader.hpak)) != 0) {

      Con_Printf("%s: file %s doesn't look like an HPAK file.\n", __FUNCTION__, string);
      FS_Close(filep);
      return(0);
   }
   if(hpakheader.version != 1) {

      Con_Printf("%s: file %s claimed to be version %i instead of %i.\n", __FUNCTION__, string, hpakheader.version, 1);
      FS_Close(filep);
      return(0);
   }

   FS_Seek(filep, hpakheader.seek, SEEK_SET);
   FS_Read(&(hpakcontainer.count), sizeof(hpakcontainer.count), 1, filep);

   if(hpakcontainer.count < 1 || hpakcontainer.count > 0x8000) {

      Con_Printf("%s: file %s has %i directory entries, which is not allowed.\n", __FUNCTION__, string, hpakcontainer.count);
      FS_Close(filep);
      return(0);
   }

   if(arg_4_index < 1 || arg_4_index > hpakcontainer.count) {
      Con_Printf("%s: element index %i is out of bounds for file %s.\n", __FUNCTION__, arg_4_index, string);
      FS_Close(filep);
      return(0);
   }

   hpakcontainer.dirs = Q_Malloc(sizeof(hpak_dir_t) * hpakcontainer.count);
   if(hpakcontainer.dirs == NULL) { //don't crash, just treat it like an error.
      Con_Printf("%s: file %s used up the last of our memory.\n", __FUNCTION__, string);
      FS_Close(filep);
      return(0);
   }

   //we could just seek the right data...
   FS_Read(hpakcontainer.dirs, sizeof(hpak_dir_t) * hpakcontainer.count, 1, filep);
   *OUT_arg8_resource = hpakcontainer.dirs[arg_4_index-1].DirectoryResource;

   Q_Free(hpakcontainer.dirs);
   FS_Close(filep);
   return(1);
}


int HPAK_GetDataPointer(const char * filename, resource_t * pResource, byte ** buffer, int * size) {

   hl_file_t * filep;
   hpak_t * var_130;
   hpak_dir_t * direntries;
   byte * var_134;
   char string[0x100];
   hpak_header_t hpakheader;
   int var_1c;
   int i;


   if(buffer != NULL) {
      *buffer = NULL;
   }
   if(size != NULL) {
      *size = 0;
   }

   if(gp_hpak_queue != NULL) {
      for(var_130 = gp_hpak_queue; var_130 != NULL; var_130 = var_130->next) {

         if(Q_strcasecmp(var_130->name, filename) == 0 && Q_memcmp(var_130->HpakResource.rgucMD5_hash, pResource->rgucMD5_hash, 0x10) == 0) {

            if(buffer != NULL) {

               var_134 = Q_Malloc(var_130->size);
               CHECK_MEMORY_MALLOC(var_134);
               Q_memcpy(var_134, var_130->data, var_130->size);
            }
            if(size != NULL) {

               *size = var_130->size;
            }
            return(1);
         }
      }
   }

   //This is the 'didnae find it' path
   //bad code, redo.
   Q_strncpy(string, filename, sizeof(string)-4);
   string[sizeof(string)-4] = '\0';
   COM_DefaultExtension(string, ".hpk");
   filep = FS_Open(string, "rb");
   if(filep == NULL) { return(0); }


   FS_Read(&hpakheader, sizeof(hpakheader), 1, filep);
   if(memcmp("HPAK", hpakheader.hpak, sizeof(hpakheader.hpak)) != 0) {

      Con_Printf("%s: file %s doesn't look like an HPAK file.\n", __FUNCTION__, string);
      FS_Close(filep);
      return(0);
   }
   if(hpakheader.version != 1) {

      Con_Printf("%s: file %s claimed to be version %i instead of %i.\n", __FUNCTION__, string, hpakheader.version, 1);
      FS_Close(filep);
      return(0);
   }

   //We don't actually check the filesize to the seeking.  That might be exploitable.
   FS_Seek(filep, hpakheader.seek, SEEK_SET);
   FS_Read(&var_1c, sizeof(var_1c), 1, filep);

   if(var_1c < 1 || var_1c > 0x8000) {

      Con_Printf("%s: file %s has %i directory entries, which is not allowed.\n", __FUNCTION__, string, var_1c);
      FS_Close(filep);
      return(0);
   }

   direntries = Q_Malloc(sizeof(hpak_dir_t) * var_1c);
   if(direntries == NULL) { //don't crash, just treat it like an error.
      Con_Printf("%s: file %s used too much memory.  Go out and buy more.\n", __FUNCTION__, string);
      FS_Close(filep);
      return(0);
   }

   FS_Read(direntries, sizeof(hpak_dir_t) * var_1c, 1, filep);
   for(i = 0; i < var_1c; i++) {

      if(Q_memcmp(direntries[i].DirectoryResource.rgucMD5_hash, pResource->rgucMD5_hash, 0x10) == 0) {

         //Found it
         FS_Seek(filep, direntries[i].seek, SEEK_SET);

         if(buffer != NULL && direntries[i].size > 0) {

            var_134 = Q_Malloc(direntries[i].size);
            CHECK_MEMORY_MALLOC(var_134); //I don't see a point in recovering if this fails.  I don't think clients can submit hpaks...
            FS_Read(var_134, direntries[i].size, 1, filep);
            *buffer = var_134;
         }

         Q_Free(direntries);
         FS_Close(filep);
         return(1);
      }
   }

   Q_Free(direntries);
   FS_Close(filep);
   return(0);
}

/* DESCRIPTION: HPAK_RemoveLump
// PATH: HPAK_Remove_f
//
// Builds a new hpak file and then removes the old one.
*/
void HPAK_RemoveLump(char * name, resource_t * resource) {

   int i, j;
   char stringRead[0x100];
   char stringWrite[0x100];
   hl_file_t * fileRead;
   hl_file_t * fileWrite;
   hpak_dir_container_t FileReadcontainer;
   hpak_dir_container_t FileWritecontainer;


   if(name == NULL || name[0] == '\0' || resource == NULL) {
      Con_Printf("%s: Invalid arguments.\n", __FUNCTION__);
      return;
   }

   HPAK_FlushHostQueue();

   Q_strncpy(stringRead, name, sizeof(stringRead)-4);
   stringRead[sizeof(stringRead)-4] = '\0';
   COM_DefaultExtension(stringRead, ".hpk");

   fileRead = FS_Open(stringRead, "rb");
   if(fileRead == NULL) { //This is okay

      Con_Printf("%s: Couldn't open %s for removal.\n", __FUNCTION__, stringRead);
      return;
   }

   COM_StripExtension(stringRead, stringWrite);
   COM_DefaultExtension(stringWrite, ".hp2");
   fileWrite = FS_Open(stringWrite, "w+b");
   if(fileWrite == NULL) { //This is not

      FS_Close(fileRead);
      Con_Printf("%s: Couldn't open temp file %s.\n", __FUNCTION__, stringWrite);
      return;
   }

   FS_Seek(fileRead, 0, SEEK_SET);
   FS_Seek(fileWrite, 0, SEEK_SET);

   //header copy
   FS_Read(&hash_pack_header, sizeof(hpak_header_t), 1, fileRead);
   FS_Write(&hash_pack_header, sizeof(hpak_header_t), 1, fileWrite);

   if(Q_memcmp(hash_pack_header.hpak, "HPAK", 4) != 0 || hash_pack_header.version != 1) {

      FS_Close(fileRead);
      FS_Close(fileWrite);
      FS_Unlink(stringWrite); //delete temp file
      Con_Printf("%s: Couldn't finish, header was not correct.\n", __FUNCTION__, stringWrite);
      return;
   }

   FS_Seek(fileRead, hash_pack_header.seek, SEEK_SET);
   FS_Read(&(FileReadcontainer.count), 4, 1, fileRead);

   if(FileReadcontainer.count < 1 || FileReadcontainer.count > 0x8000) {

      FS_Close(fileRead);
      FS_Close(fileWrite);
      FS_Unlink(stringWrite);
      Con_Printf("%s: %s has an invalid count.\n", __FUNCTION__, stringRead);
      return;
   }
   if(FileReadcontainer.count == 1) { //We never check to see if they are the same lunp though.

      FS_Close(fileRead);
      FS_Close(fileWrite);
      FS_Unlink(stringRead);
      FS_Unlink(stringWrite);
      Con_Printf("%s: %s only has one element, so it's not deleted.\n", __FUNCTION__, stringRead);
      return;
   }

   FileWritecontainer.count = FileReadcontainer.count - 1;
   FileReadcontainer.dirs = Q_Malloc(sizeof(hpak_dir_t) * FileReadcontainer.count);
   if(FileReadcontainer.dirs == NULL) {

      FS_Close(fileRead);
      FS_Close(fileWrite);
      FS_Unlink(stringWrite);
      Con_Printf("%s: There was not enough memory to store hpak %s.\n", __FUNCTION__, stringRead);
      return;
   }
   FileWritecontainer.dirs = Q_Malloc(sizeof(hpak_dir_t) * FileWritecontainer.count);
   if(FileWritecontainer.dirs == NULL) {

      FS_Close(fileRead);
      FS_Close(fileWrite);
      Q_Free(FileReadcontainer.dirs);
      FS_Unlink(stringWrite);
      Con_Printf("%s: There was not enough memory to store hpak %s.\n", __FUNCTION__, stringRead);
      return;
   }

   FS_Read(FileReadcontainer.dirs, sizeof(hpak_dir_t) * FileReadcontainer.count, 1, fileRead);

   if(HPAK_FindResource(&FileReadcontainer, resource->rgucMD5_hash, NULL) == 0) {

      FS_Close(fileRead);
      FS_Close(fileWrite);
      Q_Free(FileReadcontainer.dirs);
      Q_Free(FileWritecontainer.dirs);
      FS_Unlink(stringWrite);
      Con_Printf("%s: Couldn't find the lump %s in hpak %s.\n", __FUNCTION__, resource->szFileName, stringRead);
      return;
   }

   Con_Printf("%s: Removing lump %s from %s.\n", __FUNCTION__, resource->szFileName, stringRead);

   //If there's a collision, we've just corrupted this hpak.
   for(i = 0, j = 0; i < FileReadcontainer.count; i++) {
      if(Q_memcmp(FileReadcontainer.dirs[i].DirectoryResource.rgucMD5_hash, resource->rgucMD5_hash, 0x10) == 0) {
         continue;
      }

      FileWritecontainer.dirs[j] = FileReadcontainer.dirs[i];
      FileWritecontainer.dirs[j].seek = FS_Tell(fileWrite);
      FS_Seek(fileRead, FileReadcontainer.dirs[j].seek, SEEK_SET);
      COM_CopyFileChunk(fileWrite, fileRead, FileWritecontainer.dirs[j].size);
      j++;
   }

   hash_pack_header.seek = FS_Tell(fileWrite);
   FS_Write(&(FileWritecontainer.count), 4, 1, fileWrite);

   for(i = 0, j = 0; i < FileWritecontainer.count; i++) {

      FS_Write(&FileWritecontainer.dirs[i], sizeof(hpak_dir_t), 1, fileWrite);
   }

   FS_Seek(fileWrite, 0, SEEK_SET);
   FS_Write(&hash_pack_header, sizeof(hpak_header_t), 1, fileWrite);

   FS_Close(fileRead);
   FS_Close(fileWrite);
   Q_Free(FileReadcontainer.dirs);
   Q_Free(FileWritecontainer.dirs);
   FS_Unlink(stringRead);
   FS_Rename(stringWrite, stringRead);
}



void HPAK_Init() {

   Cmd_AddCommand("hpklist", HPAK_List_f);
   Cmd_AddCommand("hpkremove", HPAK_Remove_f);
   Cmd_AddCommand("hpkval", HPAK_Validate_f);
   Cmd_AddCommand("hpkextract", HPAK_Extract_f);
   Cvar_RegisterVariable(&cvar_hpk_maxsize);

   gp_hpak_queue = NULL;
}


