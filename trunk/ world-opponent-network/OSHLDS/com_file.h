/* This file moves a few functions that deal with opening files to their
// own little section.
*/
#ifndef OSHLDS_COMMON_FILE_HEADER
#define OSHLDS_COMMON_FILE_HEADER

#include "HUNK.h"
#include "ModCalls.h"
#include "FS.hpp"

void COM_FileBase(char *, char *);
void COM_FileBaseMaxlen(char *, char *, int);
HLDS_DLLEXPORT int COM_GetGameDir(char *);
int  COM_GetGameDirSize(char *, int);
int  COM_FixSlashes(char *);

char *COM_FileExtension(char *);
void COM_FileExtensionThreadSafe(const char *, char *, unsigned int);
void COM_DefaultExtension(char *, char *);
void COM_StripExtension(char *, char *);

int COM_ListMaps(const char *);

HLDS_DLLEXPORT byte * COM_LoadFile(const char *, int, int *);
byte * COM_LoadHunkFile(const char *);
byte * COM_LoadTempFile(const char *, int *);
HLDS_DLLEXPORT void   COM_LoadCacheFile(const char *, cache_user_t *);
byte * COM_LoadStackFile(const char *, void *, int);
HLDS_DLLEXPORT byte * COM_LoadFileForMe(const char *, int *);

HLDS_DLLEXPORT void COM_FreeFile(byte *);

HLDS_DLLEXPORT int  COM_FileSize(char *);
HLDS_DLLEXPORT int  COM_CompareFileTime(char *, char *, int *);

void COM_CreatePath(char *);
void COM_CopyFileChunk(hl_file_t *, hl_file_t *, int);
void COM_WriteFile(char *, void *, int);


#endif
