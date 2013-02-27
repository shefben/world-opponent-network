#include <limits.h>
#if(defined(_MSC_VER) || defined(__BORLANDC__)) //Might as well be if WINDOWS but not MINGW
 #include <windows.h> //for max_path
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#endif

#include <fcntl.h>
#include "com_file.h"
#include "report.h"
#include "Q_memdef.h"

/*** GLOBALS ***/
//used for types 3 and 4
static cache_user_t *loadcache; //3
static byte *loadbuf; //4
static int loadsize; //4


/*** FUNCTIONS ***/
//COM_LoadFileLimit is unknown and uncalled it seems
//COM_AddAppDirectory messes with the FS, but is uncalled in the DS
//COM_CopyFile doesn't do what you'd think and isn't called
//COM_ExpandFilename unused
//COM_(extended)ExplainDisconnection unused
//COM_SetupDirectories is omitted.

/* DESCRIPTION: COM_FileBase
// LOCATION: common.c
// PATH: lots
//
// Gets a file name sans dots and other nonsense
// Looks weird, but I trust the QW source is good.
*/
void COM_FileBase(char *in, char *out) {

   char *s, *s2;
   int length;

   s = in + strlen(in) - 1;

   while (s != in && *s != '.') { s--; }

   for (s2 = s ; *s2 && *s2 != '/' ; s2--) { }

   if (in > s2) {
      s2 = in;
   }

   length = s - s2;
   if (length < 2) {
      Q_strncpy(out,"?model?", strlen(in));
   }
   else {
      Q_memcpy(out, s2, length);
      out[length] = '\0';
   }
}
void COM_FileBaseMaxlen(char *in, char *out, int outlen) {

   //This addon shouldn't be necessary since we're at most copying an
   //idetically sized string into out.

   char *s, *s2;

   s = in + strlen(in) - 1;

   while (s != in && *s != '.') { s--; }

   for (s2 = s ; *s2 && *s2 != '/' ; s2--) { }

   if (in > s2) {
      s2 = in;
   }

   if (s-s2 < 2) {
      Q_strncpy (out,"?model?", outlen-1);
   }
   else {
      s--;
      outlen--;
      if (outlen > s-s2)
         outlen = s-s2;
      Q_strncpy (out,s2+1, outlen);
      out[outlen] = 0;
   }
}

/* DESCRIPTION: COM_GetGameDir
// LOCATION:
// PATH: Nowhere.  They just use the global.
//
// Copies com_gamedir into out.  The FS needs to handle this though...
// Returns strlen(out).
*/
int COM_GetGameDirSize(char *out, int outlen) {

   return(FS_GetGameDir(out, outlen));
}
HLDS_DLLEXPORT int COM_GetGameDir(char *out) {

   return(COM_GetGameDirSize(out, PATH_MAX));
}

/* DESCRIPTION: COM_FixSlashes
// LOCATION: slash conversion isn't that uncommon
// PATH: oo
//
// '\' becomes '/'.  In time (with the Boost filesystem taking care of this
// transparently for more than just slashes), I'd like to phase it out.
*/
int COM_FixSlashes(char * str) {

   char *temp;
   temp = strchr(str,'\\');

   if(temp == NULL) { return(0); }

   do {
      *temp = '/';
      temp++;
      temp = strchr(temp, '\\');
   } while(temp != NULL);

   return(1);
}
int COM_FixSlashesWithMem(char * str, unsigned int size) {

   char *temp;
   int cursize;
   temp = (char *)memchr((void *)str,'\\', size);

   if(temp == NULL) { return(0); }

   do {
      *temp = '/';
      temp++;
      cursize = size - (temp - str);
      temp = (char *)memchr((void *)temp, '\\', cursize);
   } while(temp != NULL);

   return(1);
}


/* DESCRIPTION: COM_FileExtension
// LOCATION: common.c
// PATH: Cmd_Exec_f->FileExtension (rather unnecessary)
//
// Copy/pasted from the QW code.  This returns the extension of a file in
// a static char array.  I don't like static arrays.  They can cause threading
// issues, so--knowing full well that extensions are usually no more than three
// characters, and also knowing that it's only called in one function and
// doesn't appear to have an SDK hook--I made a second one.
*/
char *COM_FileExtension(char *in) {

   static char exten[8];
   char * dot;
   unsigned int i;

   dot = strrchr(in, '.');
   if(dot == NULL) { return(""); }

   dot++;

   for(i = 0; (i < 7) && (*dot != '\0'); i++) { exten[i] = dot[i]; }

   exten[i] = '\0';
   return(exten);
}
void COM_FileExtensionThreadSafe(const char *in, char *exten, unsigned int SizeofExten) {

   char * dot;
   unsigned int i;

   dot = strrchr(in, '.');
   if(dot == NULL) { *exten = '\0'; return; }

   dot++;
   SizeofExten--; //Otherwise we get an off by one.  Passing a SizeOfExten of 0 is an error.

   for(i = 0; (i < SizeofExten) && (*dot != '\0'); i++) { exten[i] = dot[i]; }

   exten[i] = '\0';
}
/* DESCRIPTION: COM_DefaultExtension
// LOCATION: common.c
// PATH: too much
//
// This takes a filename and an extension as an argument and checks to see if
// the passed filename has an extension.  If not it cats on the default one
// (arg4).  There's no bounds checking in the binary; this is probably unsafe.
*/
void COM_DefaultExtension(char *path, char *extension) {

   char *src;
   src = path + strlen(path) - 1;

   while (*src != '/' && src != path) {
      if (*src == '.') { return; }
      src--;
   }

   Q_strcat(path, extension); //Danger!
}

/* DESCRIPTION: COM_SkipPath
// LOCATION: common.c
// PATH: UPDATE_GetFile (most functions do it locally)
//
// looks for a slash and then returns a pointer to it.  It's strrchr
// written all over again but less efficiently.
*/
char *COM_SkipPath (char *pathname) {

   char * chr;
   chr = strrchr(pathname, '/');
   if(chr == NULL) { chr = pathname; }
   return(chr);
}
/* DESCRIPTION: COM_StripExtension
// LOCATION: common.c
// PATH: HPAK stuff
//
// Replaces the last dot with a null and copies it into the buffer.  Apparently
// "out" and "in" can be the same buffer.
*/
void COM_StripExtension(char *in, char *out) {

   char *s;
   int i;

   if(out == in) {

      s = strrchr(in, '.');
      if(s != NULL) { *s = '\0'; }
   }
   else {

      s = strrchr(in, '.');
      i = s - in;
      Q_memcpy(out, in, i);
      out[i] = '\0';
   }
}
/* DESCRIPTION: COM_StripTrailingSlash
// LOCATION:
// PATH: exec buffer
//
// This one seems pretty self-explanatory, though it ought to be phased out.
*/
void COM_StripTrailingSlash(char *pathname) {

   int i = strlen(pathname);
   if(pathname[i-1] == '/') {
      pathname[i-1] = '\0';
   }
}

/* DESCRIPTION: COM_ListMaps
// LOCATION: As other names I think.  Too messy to bother though.
// PATH: console command 'maps'
//
// Prints out map names.  I don't know if the string has the /maps appended
// to it or not yet; we'll work that out later, as well as extensions, gamedir,
// etc.
*/
int COM_ListMaps(const char * wildcards) {

   int i = 0;
   const char * filename;

   Con_Printf("COM_ListMaps \"%s\":\n", wildcards);

   filename = Sys_FindFirst(wildcards, NULL);
   while(filename != NULL) {
      i++;
      Con_Printf(" %s\n", filename);
      filename = Sys_FindNext(NULL);
   }
   Sys_FindClose();

   Con_Printf("%u matches found.\n", wildcards);
   return(i);
}


/* DESCRIPTION: COM_LoadFile
// LOCATION: SDK
// PATH: Theoretically anywhere, since it's in the SDK.
//
// The args are known, the function cannot be altered.
// LoadFileForMe is a wrapper for COM_LoadFile, which is ALSO
// in the SDK.
//
// pLength can be null, for some reason (backwards compat?).
// What 'usehunk' does is tell the function HOW to load the file;
// I think the functions below better illustrate the types.
*/
HLDS_DLLEXPORT byte * COM_LoadFile(const char *path, int usehunk, int *pLength) {

   //pLength is a variable passed by reference we are suppossed to put
   //the size of our loaded file in.  It is optional though.

   hl_file_t *file;

   byte *FileData = NULL; //what we end up returning
   const char *FileName;
   int len;

   file = FS_Open(path, "rb");

   if(file == NULL) {
      //No file.  How sad.
      if(pLength != NULL) {
         *pLength = 0;
      }
      return(NULL);
   }

   len = FS_Size(file);

   switch(usehunk) {
   case 0:
      //0 must be small text files, but I'm not sure it's used
      FileData = (byte *)Z_Malloc(len+1);
      break;
   case 1:
      //1 must be regular hunk
      FileName = strrchr(path, '/');

      if(FileName == NULL) { FileName = path; }
      else { FileName++; } //worst case oddities, an empty string, shouldn't hurt.

      FileData = (byte *)Hunk_AllocName(len+1, FileName);
      break;
   case 2:
      //2 must be temporary file allocation
      FileData = (byte *)Hunk_TempAlloc(len+1);
      break;
   case 3:
      //3 must be caching.  I still don't quite understand how that works.
      FileName = strrchr(path, '/');

      if(FileName == NULL) { FileName = path; }
      else { FileName++; }

      FileData = (byte *)Cache_Alloc(loadcache, len+1, FileName); //loadcache = global
      break;
   case 4:
      //Similar to case 2.  Temp allocation, or allocation to a temporary file pointer.
      if (len+1 > loadsize) { FileData = (byte *)Hunk_TempAlloc(len+1); }
      else { FileData = loadbuf; }

      break;
   case 5:
      //And 5 is mallocing the memory.  But how is it freed?
      FileData = (byte *)Q_Malloc(len+1); //Freeing relies on the user.
      CHECK_MEMORY_MALLOC(FileData);

      //printf("%s: Allocating file %s (%u)\n", __FUNCTION__, path, len+1);

      break;
   //tempallocmore--case 6--doesn't exist in HL
   default:
      Sys_Error ("COM_LoadFile: \"usehunk\" is suppossed to be between 0 and 5.  Fix it, modder.\n");
   }

   if(FileData == NULL)   {
      Sys_Error("COM_LoadFile: Couldn't load \"%s\" for some memory related reason.\n", path);
      FS_Close(file); //since sys_error kills us, why close?
   }

   //whoawhoawhoa, tacking on a null? I spy a pretty nasty off by one error...
   //And now len has been +1'd, fixing it.

   FS_Read(FileData, len, 1, file);
   FileData[len] = '\0';

   FS_Close(file);

   if(pLength != NULL) { *pLength = len; }

   return(FileData);
}

byte *COM_LoadHunkFile(const char *path) { //1
   return(COM_LoadFile(path, 1, NULL));
}
byte *COM_LoadTempFile(const char *path, int *pLength) { //2
   return(COM_LoadFile(path, 2, pLength));
}
HLDS_DLLEXPORT void COM_LoadCacheFile(const char *path, cache_user_t *cu) { //3

   loadcache = cu;
   COM_LoadFile(path, 3, NULL);
}
byte *COM_LoadStackFile(const char *path, void *buffer, int bufsize) { //4

   //This appears to load a file into the passed void * buffer.  If that buffer
   //is too small, temp allocation is done instead.  Never called in i386 as
   //far as I can see.  Almost line for line what's in QW.
   byte *buf;

   loadbuf = (byte *)buffer;
   loadsize = bufsize;
   buf = COM_LoadFile(path, 4, NULL);

   return(buf);
}
HLDS_DLLEXPORT byte * COM_LoadFileForMe(const char *filename, int *pLength) { //5
   return(COM_LoadFile(filename, 5, pLength));
}

// Wrapper for free.  Altering would be a bad idea--it's assumed by us and mods.
HLDS_DLLEXPORT void COM_FreeFile(byte *buffer) {
   Q_Free(buffer);
}

/* DESCRIPTION: COM_FileSize
// LOCATION: who cares
// PATH: here and there
//
// Gets a file's size.  Hey!  There's an FS function that does that.
*/
HLDS_DLLEXPORT int COM_FileSize(char *filename) {

   return(FS_FileSize(filename));
}
HLDS_DLLEXPORT int COM_CompareFileTime(char *filename1, char *filename2, int *iCompare) {

   int i, j;

   i = FS_GetFileTime(filename1);
   j = FS_GetFileTime(filename2);
   if(i == 0 || j == 0) {
      *iCompare = 0;
      return(0);
   }

   if(i < j) {
      *iCompare = -1;
   }
   else if(i > j) {
      *iCompare = 1;
   }
   else {
      *iCompare = 0;
   }

   return(1);
}


/* DESCRIPTION: COM_CreatePath
// LOCATION: same as above
// PATH: opening, saves, writing, extracting, etc
//
// Remember the FS function that makes directories?  The one that automatically
// splits up multiple paths and whatnot?  Normally this one looks for the
// slashes and calls 'makedir' accordingly, but now all that is needed--thanks
// my brilliant planning ahead--is to call it once.
*/
void COM_CreatePath(char * path) {

   //Actually, minor concern--did I remember to NOT create the file name itself?
   //Or does boost take care of that?  Well, it'll be obvious one way or the
   //other.
   FS_CreateDirHierarchy(path, NULL);
}

/* DESCRIPTION: COM_CopyFileChunk
// LOCATION: Doesn't appear here, but isn't hard to implement
// PATH: Some HPAK add/remove functions
//
// Two files.  It's like strcpy except for some reason we're given the
// size.  I guess that means don't zero out any pointers.  Since there's no
// return value and the FS catches exceptions, checking for errors doesn't
// seem worthwhile.
*/
void COM_CopyFileChunk(hl_file_t * file1, hl_file_t * file2, int size) {

   char buffer[1024];

   while(size > 1024) {

      FS_Read(buffer, 1, 1024, file2);
      FS_Write(buffer, 1, 1024, file1);
      size -= 1024;
   }
   //little bit to go

   FS_Read(buffer, 1, size, file2);
   FS_Write(buffer, 1, size, file1);

   FS_Flush(file2);
   FS_Flush(file1);
}
/* DESCRIPTION: COM_WriteFile
// LOCATION: NOT the info_write thing that's in common.c
// PATH:
//
// We make a file and write to it.  The target is the game directory.
// Until I rework the various systems and better know how HL should
// signify which dir to write to, we use this cheap trick:
// The first dir on the list should be the game dir (unless another was aded).
// So just open the file regularly and it'll be in the right place.
*/
void COM_WriteFile(char *filename, void *filedata, int length) {

   hl_file_t *fp;
   char * c;
   c = strrchr(filename, '/');

   if(c != NULL) {

      *c = '\0';
      FS_CreateDirHierarchy(filename, NULL);
      *c = '/';
   }

   fp = FS_Open(filename, "wb");
   if (!fp) {
      Sys_Warning("COM_WriteFile: Couldn't open some file %s.\n", filename);
   }
   else {
      if(FS_Write(filedata, 1, length, fp) != length) {
         Sys_Warning("COM_WriteFile: Didn't manage to write all of file %s.\n", filename);
      }
      FS_Close(fp);
   }
}
