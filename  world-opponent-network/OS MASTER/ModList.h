/* This class deals with the management of mods for the mod server.
*/

#ifndef __MASTER__SERVER__WON2_MOD_LIST__H_
#define __MASTER__SERVER__WON2_MOD_LIST__H_

#include <pthread.h>
#include "port.h"
#include "externs.h"
#include "packetry.h"
#include <string>



/*** Structs ***/
typedef struct ActiveMods_s {

   struct ActiveMods_s *next, *previous;
   char ModName[64];

} ActiveMods_t;


/*** The class ***/
typedef class ModList {

private:

   /* lists */
   ActiveMods_t * CurrentMods;

   void AddModNode(ActiveMods_t *);
   void UnlinkModNode(ActiveMods_t *);
   void DeallocateMods();

   /* Thread locks */
   pthread_mutex_t ModList_linklist_lock;
   pthread_mutexattr_t ModList_linklist_lockAttribute;

public:

   ActiveMods_t * ChangeMod(const char *, ActiveMods_t *, const int, const int);
   unsigned long int FillPacketBufferWithMods(const char *, char *, const unsigned int);
   ModList();
   ~ModList();

} ModList_c;

#endif
