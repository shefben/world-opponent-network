#include "ModList.h"

/* Class notes:
//
// This class is more compact than others.  The basic idea is just to keep
// track of people's mods.
*/

/*** Constructor/Destructor and related ***/

/* DESCRIPTION: All of the class management stuff
//
// A little differen from how I normally describe things...
// The constructor nulls the lists, the destructor deallocates them.
// The three deallocation functions are all identical save for the name
// of the list they work off of.  Also can be used to force a refresh.
//
*/
ModList::ModList() {

   /* Thread locks */
   pthread_mutexattr_init(&ModList_linklist_lockAttribute);
   pthread_mutexattr_settype(&ModList_linklist_lockAttribute, PTHREAD_MUTEX_RECURSIVE);

   pthread_mutex_init(&ModList_linklist_lock, &ModList_linklist_lockAttribute);

   CurrentMods = NULL;
}
ModList::~ModList() {

   DeallocateMods();

   pthread_mutexattr_destroy(&ModList_linklist_lockAttribute);
   pthread_mutex_destroy(&ModList_linklist_lock);
}

//Standard freeing.  Remember--server entries have POINTERS to here.  Don't
//wipe one without wiping the other.
void ModList::DeallocateMods() {

   ActiveMods_t * ptr;

   pthread_mutex_lock(&ModList_linklist_lock);

   while(CurrentMods != NULL) {
      ptr = CurrentMods;
      CurrentMods = CurrentMods->next;
      free(ptr);
   }

   pthread_mutex_unlock(&ModList_linklist_lock);
}

/* DESCRIPTION: ChangeMod
//
// There are two arguments (and either one can be NULL).
// "NewModDirectory" indicates that a server is running the following mod.
//
// "CurrentModPointer" indicates that a server WAS running the mod it points
// to during the last update.
//
// Usually they both refer to the same mod--the server just wants to update
// how many people are playing it.
//
// If a server is being removed, NewModDirectory would be null.  If a server is
// being added for the first time, CurrentModPointer will be null.
//
// If a server is changing its mod (hard, but possible), they'll be different.
//
// Either way, we add and decrement as needed.  We MAY return NULL in some
// error cases.  The wisest move would probably be for any heartbeat
// that results in such a case to be terminated, but I'm reasonably certain
// a NULL pointer won't affect the stability of the program.
//
// note: NewModDir being "" would simplify strcmp checks.  Consider for future.
*/
ActiveMods_t * ModList::ChangeMod(const char *NewModDirectory, ActiveMods_t * CurrentModPointer, const int OldPlayerCount, const int NewPlayerCount) {

   ActiveMods_t * ptr, * ptr2;
   int i;

   pthread_mutex_lock(&ModList_linklist_lock);

   //We should probably start with removal.
   if(CurrentModPointer != NULL) {

      //Are we actually changing mods?
      if(NewModDirectory != NULL && strncasecmp(CurrentModPointer->ModName, NewModDirectory, 64) == 0) {

         //Just changing the number of players.
         CurrentModPointer->PlayerCount += (NewPlayerCount - OldPlayerCount);
         pthread_mutex_unlock(&ModList_linklist_lock);
         return(CurrentModPointer);
      }

      //Otherwise we're changing the mod.  Or (more likely) going offline.
      CurrentModPointer->PlayerCount -= OldPlayerCount;
      CurrentModPointer->ServerCount--;

      //And remove the old mod pointer if that's the last one.
      if(CurrentModPointer->ServerCount == 0) {

         UnlinkModNode(CurrentModPointer);
         free(CurrentModPointer);
      }
   }

   if(NewModDirectory != NULL) {

      if(strcmp(NewModDirectory, "start-of-list") == 0) { //nice try
         pthread_mutex_unlock(&ModList_linklist_lock);
         return(NULL);
      }

      //That was easy.  Now to see if any nodes already have this mod.
      ptr = CurrentMods;
      ptr2 = NULL;
      while(ptr != NULL) {

         i = strncasecmp(ptr->ModName, NewModDirectory, 64);

         if(i == 0) { //match

            ptr->ServerCount++;
            ptr->PlayerCount += NewPlayerCount;
            pthread_mutex_unlock(&ModList_linklist_lock);
            return(ptr);
         }
         if(i > 0) { break; }

         ptr2 = ptr;
         ptr = ptr->next;

      }
      //We didn't find a matching node.  We will have to make our own.
      //We're going to hang on to ptr2.  That var holds the node that
      //would precede our newly malloc'd node.

      ptr = (ActiveMods_t *)malloc(sizeof(ActiveMods_t));
      if(ptr == NULL) {
         pthread_mutex_unlock(&ModList_linklist_lock);
         return(NULL);
      }

      strncpy(ptr->ModName, NewModDirectory, 63);
      ptr->ModName[63] = '\0';

      ptr->ServerCount = 1;
      ptr->PlayerCount = NewPlayerCount;
      //And now to link.

      if(ptr2 == NULL) { //First node.

         ptr->previous = NULL;
         ptr->next = CurrentMods;
         if(CurrentMods != NULL) {
            CurrentMods->previous = ptr;
         }
         CurrentMods = ptr;
      }
      else {
         ptr->previous = ptr2;
         ptr->next = ptr2->next;
         if(ptr2->next != NULL) {
            ptr2->next->previous = ptr;
         }
         ptr2->next = ptr;
      }
      pthread_mutex_unlock(&ModList_linklist_lock);
      return(ptr);
   }

   pthread_mutex_unlock(&ModList_linklist_lock);
   return(NULL);
}


/* DESCRIPTION: FillPacketBufferWithMods
//
// It's assumed that the buffer is an outgoing packet.  Cycle through the mod
// list and add as needed.
//
// Notes of interest: "start-of-list" is a reserved value, and
//
*/
unsigned long int ModList::FillPacketBufferWithMods(const char * StartMod, char * buffer, const unsigned int SizeOfBuffer) {

   unsigned int packetlen = 0;
   ActiveMods_t * ptr;

   pthread_mutex_lock(&ModList_linklist_lock);

   ptr = CurrentMods;

   if(strcmp(StartMod, "start-of-list") != 0) {

      while(ptr != NULL && strncasecmp(ptr->ModName, StartMod, 63) != 0) {
         ptr = ptr->next;
      }
   }

   while(ptr != NULL) {

      if(packetlen >= SizeOfBuffer - (36 + strlen(ptr->ModName))) {
         //not enough room.  36 = sizeof("more-in-list") + 3 nulls + max length
         //of the two ints.  When porting to 64 bit, that won't fly.
         break;
      }
      packetlen += 1 + sprintf(buffer + packetlen, "%s", ptr->ModName);
      packetlen += 1 + sprintf(buffer + packetlen, "%u", ptr->ServerCount);
      packetlen += 1 + sprintf(buffer + packetlen, "%u", ptr->PlayerCount);
      ptr = ptr->next;
   }

   if(ptr == NULL) { //finished successfully
      memcpy(buffer + packetlen, "end-of-list", sizeof("end-of-list"));
      packetlen += sizeof("end-of-list");
   }
   else {
      memcpy(buffer + packetlen, "more-in-list", sizeof("more-in-list"));
      packetlen += sizeof("more-in-list");
   }

   pthread_mutex_unlock(&ModList_linklist_lock);

   return(packetlen);
}


//This function is actually more of an example.  Parts of it are used elsewhere.
void ModList::AddModNode(ActiveMods_t * modToBeAdded) {

   //list is in alphabetical order
   ActiveMods_t * ptr;

   //Condition 1: we have no nodes
   if(CurrentMods == NULL) {
      CurrentMods = modToBeAdded;
      CurrentMods->previous = NULL; //This node is now #1
      CurrentMods->next = NULL;
      return;
   }

   ptr = CurrentMods;

   //Condition 2: We need to change our first node
   if(strncasecmp(ptr->ModName, modToBeAdded->ModName, 64) > 0) {

      CurrentMods = modToBeAdded;
      CurrentMods->previous = NULL;
      CurrentMods->next = ptr;
      ptr->previous = CurrentMods;
   }
   else {
      while(ptr->next->ModName != NULL && strncasecmp(ptr->next->ModName, modToBeAdded->ModName, 64) < 0) {

         ptr = ptr->next;
      }

      //At this stage ptr is the node that should PRECEDE our passed node.
      modToBeAdded->previous = ptr;
      modToBeAdded->next = ptr->next;

      //Condition 3: This is going to be our last node
      if(ptr->next != NULL) {
         ptr->next->previous = modToBeAdded;
      }
      ptr->next = modToBeAdded;
   }
}
void ModList::UnlinkModNode(ActiveMods_t * modToBeDeleted) {

   if(modToBeDeleted->previous == NULL) { //This must be the first node
      CurrentMods = modToBeDeleted->next;
   }
   else {
      modToBeDeleted->previous->next = modToBeDeleted->next;
   }
   if(modToBeDeleted->next != NULL) {
      modToBeDeleted->next->previous = modToBeDeleted->previous; //There we go; unlinked.
   }
}


