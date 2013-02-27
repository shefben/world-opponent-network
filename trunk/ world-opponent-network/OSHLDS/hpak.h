/*
*/
#ifndef HLDS_LINUX_HPAK_HIJACK
#define HLDS_LINUX_HPAK_HIJACK

#include "common.h"
#include "endianin.h"
#include "NET.h"


typedef struct hpak_s { //0x98 bytes

   char * name; //00
   resource_t HpakResource;
   int32 size; //8c
   void * data;
   struct hpak_s * next; //94
} hpak_t;

typedef struct hpak_header_s { //0x0C, convenience.

   char hpak[4]; //always equal to HPAK
   int32 version;
   int32 seek;
} hpak_header_t;

typedef struct hpak_dir_s { //0x90, convenience.  Most of these probably don't carry.

   resource_t DirectoryResource; //00, I doubt that 80 and 84 are still pointers though.
   int32 seek; //88 known
   int32 size; //8c known
} hpak_dir_t;
typedef struct hpak_dir_container_s {

   int count;
   hpak_dir_t * dirs; //variable sized.
} hpak_dir_container_t;

void HPAK_Init();
void HPAK_CheckIntegrity(char *);
int HPAK_GetDataPointer(const char *, resource_t *, byte **, int *);
int HPAK_ResourceForHash(char *, char *, resource_t *);
void HPAK_AddLump(int, char *, resource_t *, byte *, hl_file_t *);
void HPAK_FlushHostQueue();
void HPAK_CheckSize(char *);

#endif
