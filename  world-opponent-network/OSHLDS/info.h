/* The info fucntions.  More of a holdover from Quake than something I
// like to use.  Responsible for several DoS attacks on servers.
//
// The good thing about 'em is that they can be pulled from the GPLd Quake.
*/

#ifndef OSHLDS_INFO_HEADER
#define OSHLDS_INFO_HEADER

#include "endianin.h"
#include "ModCalls.h"

#define MAX_INFO_KEY 64

// functions
qboolean Info_IsKeyImportant(const char *);
char *Info_FindLargestKey(char *);
void Info_Print(char *);
void Info_RemoveKey(char *, const char *);
void Info_RemovePrefixedKeys(char *, char);
char * Info_Serverinfo();
HLDS_DLLEXPORT const char * Info_ValueForKey(const char *, const char *); //Used by mods
void Info_SetValueForStarKey(char *, const char *, const char *, int);
void Info_SetValueForKey(char *, const char *, const char *, int);
#endif
