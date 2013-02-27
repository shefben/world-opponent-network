#include "SrvrNtwk.h"

/*** Threads ***/


void ServerList::ServerTimeouts(void *a) {

   class ServerList* pthis = (class ServerList*)a;
   unsigned int UnixTime = (ptrIO->CheckCurrentTime()) - pthis->ServerTimeoutValueInSeconds;

   ptrIO->OutputFileData(4, "Timeout thread running");
   pthread_mutex_lock(&(pthis->Serverlist_lock));

   HLserver_t * ptr = pthis->BeginningOfServerlist;
   HLserver_t * ptr2;
   while(ptr != NULL) {
      ptr2 = ptr->left;

      if(ptr->last_heartbeat < UnixTime) {
         pthis->DeleteNode(ptr);
      }
      ptr = ptr2;
   }

   pthread_mutex_unlock(&(pthis->Serverlist_lock));
   pthis->AreServersTimingOut = 0;
}


/*** Constructor/Destructor and related ***/

/* DESCRIPTION: All of the class management stuff
//
// A little differen from how I normally describe things...
// The constructor nulls the lists, the destructor deallocates them.
// The three deallocation functions are all identical save for the name
// of the list they work off of.  Also can be used to force a refresh.
//
*/
ServerList::ServerList() {

   /* Thread locks */
   pthread_mutexattr_init(&Serverlist_lockAttribute);
   pthread_mutexattr_settype(&Serverlist_lockAttribute, PTHREAD_MUTEX_RECURSIVE);
   pthread_mutex_init(&Serverlist_lock, &Serverlist_lockAttribute);


   BeginningOfServerlist = NULL;
   FirstFreeNode = NULL;
   ReserveNodes = 0;
   ServerTimeoutValueInSeconds = 600;
   AreServersTimingOut = 0;
   HaveServersEverTimedOut = 0;
   orig_ThreadServerTimeouts = (lpfn_ServerTimeouts)ServerTimeouts;

}
ServerList::~ServerList() {

   if(HaveServersEverTimedOut) { pthread_join(ThreadServerTimeouts, NULL); }
   DeallocateServers();
   DeallocateReserveNodes();

   pthread_mutexattr_destroy(&Serverlist_lockAttribute);
   pthread_mutex_destroy(&Serverlist_lock);
}



/* C/D functions */

/* DESCRIPTION: DeallocateServers
//
// If we are wiping EVERY node, there's no point in pooling memory pages with
// FreeUnlinkedNode.  We just go and free() it all in one function.
// Intended as part of the program termination, but could be used in a reset.
//
*/
void ServerList::DeallocateServers() {

   HLserver_t * ptr;

   pthread_mutex_lock(&Serverlist_lock);
   while(BeginningOfServerlist != NULL) {
      ptr = BeginningOfServerlist;
      BeginningOfServerlist = BeginningOfServerlist->left;
      free(ptr);
   }
   pthread_mutex_unlock(&Serverlist_lock);
}
/* DESCRIPTION: DeallocateReserveNodes
//
// Eventually the program ends and all the memory it had needs to be returned.
// That time is now, so we go through out free structure pool and free() each
// one.  That's it.
//
*/
void ServerList::DeallocateReserveNodes() {

   HLserver_t* ptr;

   pthread_mutex_lock(&Serverlist_lock);
   while(FirstFreeNode != NULL) {
      ptr = FirstFreeNode;
      FirstFreeNode = FirstFreeNode->left;
      free(ptr);
      ReserveNodes--;
   }
   if(ReserveNodes != 0) { ptrIO->OutputWindowPrintf(2, "PSA: ReserveNodes doesn't equal zero and it should."); }
   pthread_mutex_unlock(&Serverlist_lock);
}

void ServerList::ChangeServerTimeoutValue(unsigned int x) {
   if(x > 60) {
      ServerTimeoutValueInSeconds = x; //OO makes EVERYTHING fun...
   }
}

/* Basic malloc and free */

/* DESCRIPTION: NewNode
//
// Easier than I thought it would be.  Since server allocation is
// constantly giving and receiving bytes, I figured it'd be easier
// for everyone involved to not have to malloc left and right.
// NewNode handles mallocs; when the free structure pool dips below
// 50, it mallocs 400 more, which are stored in a linked list pointed to
// by FirstFreeNode.  Other than that, it just returns a pointer to one
// of the free servers in the list, and moves FirstFreeNode to account
// for the change.
//
*/
HLserver_t * ServerList::NewNode() {

   HLserver_t * ptr;
   int i;

   pthread_mutex_lock(&Serverlist_lock);

   //If we have fewer than, say, fifty nodes we might as well allocate 400 more
   if(ReserveNodes < 50) {
      for(i=0; i<400; i++) { //400 mallocs; unrolling potential is there
         ptr = (HLserver_t *)malloc(sizeof(HLserver_t));
         if(ptr == NULL) { //Crap, we have no memory
            ptrIO->OutputWindowPrintf(1, "Memory allocation failure.  Buy more RAM, cheapskate.");
            break;
         }
         ptr->left = FirstFreeNode;
         FirstFreeNode = ptr;
         ReserveNodes++;
      }
   }

   //regardless of any mallocing, we assume ptr points nowhere we want
   ptr = FirstFreeNode;
   if(ptr == NULL) { //crap, we have no memory AND no remaining structures
      return(NULL);
   }
   FirstFreeNode = FirstFreeNode->left;
   ReserveNodes--;

   pthread_mutex_unlock(&Serverlist_lock);

   memset(ptr, 0, sizeof(HLserver_t));
   return(ptr);
}
/* DESCRIPTION: FreeUnlinkedNode
//
// Much like NewNode, it handles allocaton and deallocation of
// memory.  The difference is it adds the passed structure to
// the structure pool and, if ReserveNodes gets above, say 750,
// it goes through and liquidates 400.
// One note of warning: it is impossible to check to see if the
// address is valid, and infeasable to see if it has truly been
// removed from any trees it was in.
//
*/
void ServerList::FreeUnlinkedNode(HLserver_t * deadnode) {

   HLserver_t* ptr;
   int i;

   pthread_mutex_lock(&Serverlist_lock);
   //No evil input protection; this is especially problematic as any invalid
   //addresses won't be hit until there are 750 free nodes...

   deadnode->left = FirstFreeNode;
   FirstFreeNode = deadnode;
   ReserveNodes++;

   //If we have more than, say, 750 nodes we might as well be nice to the OS
      if(ReserveNodes > 750) {
      for(i=0; i<400; i++) {
         ptr = FirstFreeNode;
         FirstFreeNode = FirstFreeNode->left;
         free(ptr);
         ReserveNodes--;
      }
   }
   pthread_mutex_unlock(&Serverlist_lock);
}



/* Linked List Adds and Deletes */

/* DESCRIPTION: AddNode
//
// Takes an allocated server pointer and links it to our list.  Now, there is
// a sorting in our list based on the hash number (struct->hashvalue), and it
// is assumed that the programmer assigned that before calling this function.
// In future I can remove that requirement and have this calculate the hash
// manually, but I arbitrarially decided not to.
//
// Until I need to, it's a boring doubly linked list.  There is no effect on
// which order hash collisions are added.
*/
void ServerList::AddNode(HLserver_t * serverToBeAdded) {

   pthread_mutex_lock(&Serverlist_lock);
   HLserver_t * ptr = BeginningOfServerlist;


   //Condition 1: we have no nodes
   if(ptr == NULL) {
      BeginningOfServerlist = serverToBeAdded;
      BeginningOfServerlist->previous = NULL; //This node is now #1
      BeginningOfServerlist->left = NULL;
   }

   //Condition 2: We need to change our first node
   else if(ptr->hashvalue > serverToBeAdded->hashvalue) {
      BeginningOfServerlist = serverToBeAdded;
      BeginningOfServerlist->previous = NULL;
      BeginningOfServerlist->left = ptr;
      ptr->previous = BeginningOfServerlist;
   }
   else {
      while(ptr->left != NULL && ptr->left->hashvalue < serverToBeAdded->hashvalue) {

         ptr = ptr->left;
      }

      //At this stage ptr is the node that should PRECEDE our passed node.
      serverToBeAdded->previous = ptr;
      serverToBeAdded->left = ptr->left;

      //Condition 3: This is going to be our last node
      if(ptr->left != NULL) {
         ptr->left->previous = serverToBeAdded;
      }
      ptr->left = serverToBeAdded;
   }
   pthread_mutex_unlock(&Serverlist_lock);
}
/* DESCRIPTION: DeleteNode
//
// Unlinks one node and adds it to the freenode pool.  There's not much to
// it right now, but it's written to make converting it to a tree easier.
// For now you just pass it the node you want to delete.
//
*/
void ServerList::DeleteNode(HLserver_t * serverToBeDeleted) {

   pthread_mutex_lock(&Serverlist_lock);
   if(serverToBeDeleted != NULL) { //That's technically delaying the ineviteble if that mistake was made in coding...


      //Stats running.  We can do it here since we don't need to compare old and new values like when receiving packets.
      ptrIO->addServers(-1, (serverToBeDeleted->CheckboolIsServerLan()));
      ptrIO->addPlayers(-(serverToBeDeleted->players), (serverToBeDeleted->CheckboolIsServerLan()));
      ptrIO->addBots(-(serverToBeDeleted->fakeplayers), (serverToBeDeleted->CheckboolIsServerLan()));

      ptrMods->ChangeMod(NULL, serverToBeDeleted->GameDir, serverToBeDeleted->players, 0);

      if(serverToBeDeleted->previous == NULL) { //This must be the first node
         BeginningOfServerlist = serverToBeDeleted->left;
      }
      else {
         serverToBeDeleted->previous->left = serverToBeDeleted->left;
      }
      if(serverToBeDeleted->left != NULL) {
         serverToBeDeleted->left->previous = serverToBeDeleted->previous; //There we go; unlinked.
      }

      FreeUnlinkedNode(serverToBeDeleted);
   }
   else {
      ptrIO->OutputWindowPrintf(2, "NULL pointer passed to DeleteNode().  You might want to check that.");
   }
   pthread_mutex_unlock(&Serverlist_lock);
}


/* Functions related to the linked list */

/* DESCRIPTION: FindNodeBySockaddr
//
// Calls IP version of function.
//
*/
HLserver_t * ServerList::FindNodeBySockaddr(struct sockaddr_in server) {

   return(FindNodeByIP(server.sin_addr.s_addr, server.sin_port));
}
/* DESCRIPTION: FindNodeByIP
//
// Compares IPs and hashes until it finds a match.  We can use the fact that
// it's a sorted linked list to quickly get to the right block, and then
// compare until all is well.
//
*/
HLserver_t * ServerList::FindNodeByIP(unsigned long int IP, unsigned short int port) {

   unsigned int i = HashServerIP(IP, port);  //will be more useful once we have a binary tree
   HLserver_t * ptr = FindNodeByHash(i);

   //At this point we either have a null pointer or a pointer to the block
   //our desired server is in.

   while(ptr != NULL && ptr->hashvalue == i) {


      if(ptr->IntServerAddress == IP && ptr-> IntServerPort == port) {
         return(ptr);
      }
      ptr = ptr->left;
   }
   return(NULL);
}
/* DESCRIPTION: FindNodeByHash
//
// Finds the first node with a given hash.  Returns null if it isn't found.
//
*/
HLserver_t * ServerList::FindNodeByHash(unsigned long int hash) {


   HLserver_t * ptr = BeginningOfServerlist;
   while(ptr != NULL) {

      if(ptr->hashvalue >= hash) {
         break;
      }
      ptr = ptr->left;
   }
   if(ptr != NULL && ptr->hashvalue != hash) {
      ptr = NULL;
   }
   return(ptr);
}

/* DESCRIPTION: FindNodeofEqualOrLesserHash
//
// Intended for use when continuing a packet batch request.  Will return the
// first node that has a matching hash.  If no node has a matching hash, returns
// the closest match without going over.  If all nodes have a greater hash
// value (as in the common case of 0 passed), returns the beginning of the list.
// And lastly, returns NULL if there are no nodes.
//
*/
HLserver_t * ServerList::FindNodeofEqualorLesserHash(unsigned long int hash) {

   HLserver_t* ptr = BeginningOfServerlist;

   if(ptr != NULL) {

      while(ptr->hashvalue < hash && ptr->left != NULL) {
         ptr = ptr->left;
      }

   //At this stage: (cases 1, 2, 3, & 5 are set to return)
   // If we had no nodes, ptr = NULL and control doesn't come here.
   // If all nodes have hashes > 0, ptr = BeginningOfServerlist.
   // If the hash is in the middle and exists, ptr = the node.
   // If the hash is in the middle but DNE, ptr = the node after.
   // If the hash is too high, ptr = last node.

   if(ptr->hashvalue > hash) //cases 2 & 4
      if(ptr->previous != NULL) {
         ptr = ptr->previous;
      }
   }

   return(ptr);
}


/* DESCRIPTION: HashServer
//
// Calls IP version of function.
//
*/
unsigned int ServerList::HashServer(struct sockaddr_in server) {

   return(HashServerIP(server.sin_addr.s_addr, server.sin_port));
}
/* DESCRIPTION: HashServerIP
//
// In order to quickly pick up where we leave off in a server listing, we
// need to hash them.  We don't need to be quite as picky here though; we
// have a good 31 bits to store, and repeated hash collisions--MANY repeated
// hash collisions--won't do anything except possibly omit a server.  I
// would like to keep randomness so that people with a 'lucky' IP won't get
// an advantage in listing.
//
*/
unsigned int ServerList::HashServerIP(unsigned long int IP, unsigned short int port) {

   static int offset = ((rand() | (rand()<<17)) ^ (rand()<<9));

   return(((port ^ (IP<<16) | (IP>>16)) + offset) & 0x7FFFFFFF);

}


/* DESCRIPTION: FillPacketBufferWithIPs
//
// It's assumed that the buffer is an outgoing packet.  This cycles through the
// serverlist and adds the whole IP PORT combo until there's not enough room.
// Naturally this requires the caller to be careful and not pass a bad
// SizeOfBuffer.  StartingHash is used to determine where to start in our
// linked list, flags are as of yet unused, and this returns the number of
// bytes written.  StartingHash is also a return value, as it is modified to
// become the value of the next node's hash.
//
// The hash is 31 bits.  The 32nd bit is something that should be
// processed with another function entirely--do not call this version if the
// leftmost hash bit is set in the received packet.
//
*/
unsigned long int ServerList::FillPacketBufferWithIPs(unsigned long int* StartingHash, const char* CharBuffer, const unsigned int SizeOfBuffer, const int flags, const char* moddirectory) {

   pthread_mutex_lock(&Serverlist_lock);

   unsigned int packetlen = 0;
   HLserver_t * serverptr = FindNodeofEqualorLesserHash(*StartingHash);

   //To recap FNoEoLH, we have a null if there are no servers, or a server
   //pointer--probably the beginning of our list.

   while(serverptr != NULL) {

      if(flags) { //Hey, that's one less function call
         if(!ServerTypeCheck(serverptr, flags, moddirectory)) {
            serverptr = serverptr->left;
            continue;
         }
      }
      if(packetlen >= SizeOfBuffer-6) { break; }

      *(unsigned long int *)(CharBuffer+packetlen) = serverptr->IntServerAddress; //all little endian, not network order
      packetlen += 4;
      *(unsigned short int *)(CharBuffer+packetlen) = serverptr->IntServerPort;
      packetlen += 2;

      serverptr = serverptr->left;
   }

   if(serverptr == NULL) {
      *StartingHash = 0x00000000;
   }
   else {

      /* The following code that may add 1 to 'position' exists to catch an
      // infinite loop.  If there are 400 or so server hash collisions there's
      // a chance that we'll give the client the same data over and over again.
      // This would be hard to abuse (clients would suffer more than us),
      // and rare by chance (400, WON peaked at like 30000, and the range of
      // hashes is 2^31).  But it's there, and now it's protected.
      */

      if(*StartingHash == serverptr->hashvalue) {
         *StartingHash = (serverptr->hashvalue)+1;
      }
      else {
         *StartingHash = (serverptr->hashvalue);
      }
   }

   pthread_mutex_unlock(&Serverlist_lock);

   return(packetlen);

}


/* DESCRIPTION: ServerTypeCheck
//
// Clients can ask that we filter our responses by things like game types
// and whatnot.  Flags are defined; ModName may be empty or a null.
//
*/
int ServerList::ServerTypeCheck(const HLserver_t * ptr, int flags, const char* ModDir) {


    /* From left to right
      0-------
      8      1

        Secure 1
        Proxy 1
        Linux 1
        Empty 1
        Full 1
        Dedicated 1
        Mod name specified 1
    */

   if(flags & 0x01 == 1) { //mod name specified

      //Inefficient; if we restructured we could just compare pointers.
      if(strcmp(ModDir, ptr->GameDir->ModName) != 0) { return(0); }
   }
   flags = flags>>1;

   if(flags & 0x01 == 1) { //Dedicated 1
      if(!ptr->CheckboolIsServerDedicated()) { return(0); }
   }
   flags = flags>>1;

   if(flags & 0x01 == 1) { //not full
      if(ptr->players == ptr->maxplayers) { return(0); }
   }
   flags = flags>>1;

   if(flags & 0x01 == 1) { //not empty
      if(ptr->players == 0) { return(0); }
   }
   flags = flags>>1;

   if(flags & 0x01 == 1) { //linux
      if(!ptr->CheckboolIsServerLinux()) { return(0); }
   }
   flags = flags>>1;

   if(flags & 0x01 == 1) { //proxy.  Still don't know what that does to HL.
      if(!ptr->CheckboolIsServerProxy()) { return(0); }
   }
   flags = flags>>1;

   if(flags & 0x01 == 1) { //secure
      if(!ptr->CheckboolIsServerSecure()) { return(0); }
   }

   return(1);
}

/* DESCRIPTION: SetServerToTimeoutInOneMin
//
// Servers send special packets when they are going offline.  Unfortunately
// they are quite spoofable, so if we process them we are opening servers
// up for attack.  We could also ignore them, and dead servers will go away
// after 5 minutes, or we can do this--set the timeout to one minute and go
// from there.  Servers chime in every minute (in theory), and we can also
// ping them.  This will do the first half.  Returns 1 if there's cause to
// send a ping.
//
*/
int ServerList::SetServerToTimeoutInOneMin(struct sockaddr_in server) {

   int i = 0;

   pthread_mutex_lock(&Serverlist_lock);

   HLserver_t * ptr = FindNodeBySockaddr(server);
   unsigned int UnixTime = (ptrIO->CheckCurrentTime()) - ServerTimeoutValueInSeconds + 60;
   //UnixTime is set to 1 minute before timeout
   if(ptr != NULL) {
      if(ptr->last_heartbeat > UnixTime) { i = 1; }
      ptr->last_heartbeat = UnixTime;
   }

   pthread_mutex_unlock(&Serverlist_lock);

   return(i);
}

/* DESCRIPTION: ResetServerTimeout
//
// If we get an ack from a server, we know it's up.  So we update it's
// last response time.  If we don't have the server listed we ignore
// the ack, and return 1 (1 in the PKT_IN fields is a failure).
//
*/
int ServerList::ResetServerTimeout(struct sockaddr_in server) {

   pthread_mutex_lock(&Serverlist_lock);

   HLserver_t * ptr = FindNodeBySockaddr(server);

   if(ptr != NULL) {
      ptr->last_heartbeat = ptrIO->CheckCurrentTime();
      pthread_mutex_unlock(&Serverlist_lock);
      return(0);
   }

   pthread_mutex_unlock(&Serverlist_lock);
   return(1);
}


/* DESCRIPTION: SpawnServerTimeouts
//
// Spins a thread if one isn't already running (or if it's just about
// to end; actually locking the bool shouldn't have any effect, so
// that difference is probably splitting hairs).  Returns 1 normally
// or returns 0 if a thread is already running.  May return other
// values in future.
*/
int ServerList::SpawnServerTimeouts() {

   ptrIO->OutputWindowPrintf(2, "Spawning server timeout thread.");

   if(HaveServersEverTimedOut) {
      pthread_join(ThreadServerTimeouts, NULL);
   }

   if(!AreServersTimingOut) {
      AreServersTimingOut = 1;
      HaveServersEverTimedOut = 1;
      pthread_create(&ThreadServerTimeouts, NULL, orig_ThreadServerTimeouts, (void *)this);
      return(1);
   }
   ptrIO->OutputWindowPrintf(2, "ServerTimeouts() aborted.  Probably already running.");
   return(0);
}

void ServerList::FlushServers() {

   HLserver_t * ptr;

   pthread_mutex_lock(&Serverlist_lock);
   while(BeginningOfServerlist != NULL) {
      ptr = BeginningOfServerlist;
      BeginningOfServerlist = BeginningOfServerlist->left;
      DeleteNode(ptr);
   }
   pthread_mutex_unlock(&Serverlist_lock);
}
