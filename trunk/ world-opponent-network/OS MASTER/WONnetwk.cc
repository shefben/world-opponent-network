#include "WONnetwk.h"

/*** Constructor/Destructor and related ***/

/* DESCRIPTION: All of the class management stuff
//
// A little differen from how I normally describe things...
// The constructor nulls the lists, the destructor deallocates them.
// The three deallocation functions are all identical save for the name
// of the list they work off of.  Also can be used to force a refresh.
//
*/
WON2ServerNetwork::WON2ServerNetwork() {

   /* Thread locks */
   pthread_mutexattr_init(&WONnetwk_linklist_lockAttribute);
   pthread_mutexattr_init(&WONnetwk_packet_lockAttribute);
   pthread_mutexattr_settype(&WONnetwk_linklist_lockAttribute, PTHREAD_MUTEX_RECURSIVE);
   pthread_mutexattr_settype(&WONnetwk_packet_lockAttribute, PTHREAD_MUTEX_RECURSIVE);
   pthread_mutex_init(&WONnetwk_linklist_lock, &WONnetwk_linklist_lockAttribute);
   pthread_mutex_init(&WONnetwk_packet_lock, &WONnetwk_packet_lockAttribute);

   Mods = NULL;
   NumMods = 0;

   Remake_w_replypacket();

}
WON2ServerNetwork::~WON2ServerNetwork() {

   DeallocateMods();

   pthread_mutexattr_destroy(&WONnetwk_linklist_lockAttribute);
   pthread_mutexattr_destroy(&WONnetwk_packet_lockAttribute);
   pthread_mutex_destroy(&WONnetwk_linklist_lock);
   pthread_mutex_destroy(&WONnetwk_packet_lock);
}

//These three are too small to bother combining into one.
void WON2ServerNetwork::DeallocateMods() {

   WON2Modlist_t* ptr;
	pthread_mutex_lock(&WONnetwk_linklist_lock);
   while(Mods != NULL) {
      ptr = Mods;
      Mods = Mods->next;
      free(ptr);
   }
   NumMods = 0;

pthread_mutex_unlock(&WONnetwk_linklist_lock);

}

void WON2ServerNetwork::RetrieveMods(void **name) {

   WON2Modlist_t *ptr = (WON2Modlist_t*)*name;


   if(ptr == NULL) {
      ptr = Mods;
   }
   else {
      ptr = ptr->next;
   }

   while(ptr != NULL && ptr->IsLoaded != 1) {
      ptr = ptr->next;
   }
      //At this stage we have a ptr variable which points to a valid master
      //entry, or to NULL, meaning we should just leave.

   if(ptr == NULL) {
      *name = NULL;
      return;
   }

   *name = (void *)ptr;
}


unsigned long int ModList::FillPacketBufferWithMods(const char * StartMod, char * buffer, const unsigned int SizeOfBuffer) {

   unsigned int packetlen = 0;
   WON2Modlist_t * ptr;

   pthread_mutex_lock(&ModList_linklist_lock);

   ptr = CurrentMods;

   if(strcmp(StartMod, "start-of-list") != 0) {

      while(ptr != NULL && strncasecmp(ptr->Name, StartMod, 63) != 0) {
         ptr = ptr->next;
      }
   }

   while(ptr != NULL) {

      if(packetlen >= SizeOfBuffer - (36 + strlen(ptr->Name))) {
         break;
      }
      packetlen += 1 + sprintf(buffer + packetlen, "%s", ptr->Name);
      while(ptr->tokens)
      packetlen += 1 + sprintf(buffer + packetlen, "%u", ptr->tokens);

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
/* DESCRIPTION: AddMod
*/
char WON2ServerNetwork::AddMod(const char *arg, WON2Modlist_t **modlist, WON2Modlist_t *toke ) {

   char temparray[10];
   char modvar;

   strncpy(temparray, arg, 9);

   //For now let's malloc the structure we'll definitely need.

   WON2Modlist_t * NewMod = (WON2Modlist_t *)malloc(sizeof(WON2Modlist_t));
   if(NewMod == NULL) { return(3); } //BAIL!  Bad things, man.

   NewMod->previous = NULL;
   NewMod->Name = NULL;

   //Copy the name and resolve it.
   NewMod->Name = new std::string(temparray);

   //At this point we have a valid node.  Add it onto the beginning of our list.

   NewMod->next = (*modlist);
   if((*modlist) != NULL) {
      (*modlist)->previous = NewMod;
   }
   (*modlist) = NewMod;

   RetrieveMods(temparray);

   Remake_w_replypacket();

   return(temparray);
}

char WON2ServerNetwork::AddMod(char *arg, WON2Modlist_t *tokens) {

   char returnValue;
   pthread_mutex_lock(&WONnetwk_linklist_lock);

   returnValue = AddMod(arg, &Mods, tokens);

   pthread_mutex_unlock(&WONnetwk_linklist_lock);

   if(returnValue == 0 || returnValue == 1) { NumMods++; }
   return(returnValue);
}


void WON2ServerNetwork::send_IN_w_ReplyPacket(struct sockaddr_in DestinationAddress) {

   pthread_mutex_lock(&WONnetwk_packet_lock);

   ptrSocketClass->OUT_SendSpecificReply(OUT_w_replyingpacket, OUT_w_replyingpacketsize, DestinationAddress);

   pthread_mutex_unlock(&WONnetwk_packet_lock);
}
