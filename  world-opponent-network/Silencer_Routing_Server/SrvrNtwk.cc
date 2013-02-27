#include "SrvrNtwk.h"
#include "PManip.h"
#include "SilError.h"


/*** Globals ***/
//premade packets--personally I think the way 'static' is used in classes seriously fubars a lot of its potential use.
#define PAYLOAD2  "\x30\x00\x00\x00" "\x45" "\x52\x60" "\x73\x80" "\x90\xA0" "\xB0" "\xC1\xDA\xE0\xF0" "\x01\x00" "\x53" "\x0A\x00" "A u t h S e r v e r " "\x06" "\x00\x00" "\x00\x00\x00\x00"
#define PAYLOAD3  "\x6E\x00\x00\x00" "\x45" "\x52\x60" "\x73\x80" "\x90\xA0" "\xB0" "\xC1\xDA\xE0\xF0" "\x02\x00" "\x53" "\x12\x00" "T i t a n F a c t o r y S e r v e r " "\x06" "\x00\x00" "\x00\x00\x00\x00" \
                  "\x53" "\x12\x00" "T i t a n R o u t i n g S e r v e r " "\x06" "\x00\x00" "\x00\x00\x00\x00"

static unsigned char AuthServerPacket[52] = PAYLOAD2;
static unsigned char RoutingServerPacket[112] = PAYLOAD3;

/*** Constructor/Destructor and related ***/

/* DESCRIPTION: All of the class management stuff
//
// The constructor nulls the lists, the destructor deallocates them.
// The three deallocation functions are all identical save for the name
// of the list they work off of.  Also can be used to force a refresh.
//
*/
ServerList::ServerList() {

   /* Thread locks */
   pthread_mutex_init(&Serverlist_lock, NULL);

   BeginningOfServerlist = NULL;
   ServerTimeoutValueInSeconds = 600; //until further notice
}
ServerList::~ServerList() {

   DeallocateServers();

   pthread_mutex_destroy(&Serverlist_lock);
}



/* C/D functions */

/* DESCRIPTION: DeallocateServers
//
// Wipes every node.  Nothing fancy; intended to be used on termination, but
// could also server as a restart.
*/
void ServerList::DeallocateServers() {

   SilencerGameServer_t * ptr;

   pthread_mutex_lock(&Serverlist_lock);
   while(BeginningOfServerlist != NULL) {
      ptr = BeginningOfServerlist;
      BeginningOfServerlist = BeginningOfServerlist->next;
      free(ptr);
   }
   pthread_mutex_unlock(&Serverlist_lock);
}



/* Basic malloc and free */

/* DESCRIPTION: NewNode
//
// Mallocs a node.  Since there's no node pool stuff, it's just an inline
// wrapper for malloc.
*/
inline SilencerGameServer_t * ServerList::NewNode() {

   return((SilencerGameServer_s*)malloc(sizeof(struct SilencerGameServer_s)));
}
/* DESCRIPTION: FreeUnlinkedNode
//
// Much like NewNode, this becomes no more than an inline wrapper.
//
*/
void ServerList::FreeUnlinkedNode(SilencerGameServer_t * deadnode) {

   #if(PARANOID)

   if(deadnode == NULL) {
      printf(SIL_PASSEDNULL);
      return;
   }

   #endif


   free(deadnode);
}



/* Linked List Adds and Deletes */

/* DESCRIPTION: AddNode
//
// Takes an allocated server pointer and links it to our list.  Since it is
// unlikely that there will ever be more than a handful of servers, there is
// no reason to sort it in any way.  It is therefore a boring linked list.
//
// Only called in code segments already locked.
*/
void ServerList::AddNode(SilencerGameServer_t * serverToBeAdded) {


   #if(PARANOID)

   if(serverToBeAdded == NULL) {
      printf(SIL_PASSEDNULL);
      return;
   }

   #endif

   SilencerGameServer_t * ptr = BeginningOfServerlist;

   ptr = BeginningOfServerlist;
   BeginningOfServerlist = serverToBeAdded;
   BeginningOfServerlist->next = ptr;
}
/* DESCRIPTION: DeleteNode
//
// Unlinks one node and frees it.
// Only called in code segments already locked.
*/
void ServerList::DeleteNode(SilencerGameServer_t * serverToBeDeleted) {


   #if(PARANOID)

   if(serverToBeDeleted == NULL) {
      printf(SIL_PASSEDNULL);
      return;
   }

   #endif


   //Special case of deleting the beginning node:
   if(BeginningOfServerlist = serverToBeDeleted) {
      BeginningOfServerlist = serverToBeDeleted->next;
   }
   else {

      SilencerGameServer_t * ptr = BeginningOfServerlist;

      //Someone somewhere points to the server that is to be deleted.
      //We need to find out who so we can bridge the hole we're about to make.

      while(ptr->next != serverToBeDeleted) { //ptr should never equal null if we have a server to delete, doy
         ptr = ptr->next;
      }

      ptr->next = serverToBeDeleted->next;
   }
   FreeUnlinkedNode(serverToBeDeleted);
}



/* Functions related to the linked list */

/* DESCRIPTION: FindNodeBySockaddr
//
// Calls IP version of function.
//
*/
SilencerGameServer_t * ServerList::FindNodeBySockaddr(struct sockaddr_in server) {

   return(FindNodeByIP(server.sin_addr.s_addr, server.sin_port));
}
/* DESCRIPTION: FindNodeByIP
//
// Compares IPs until it finds a match.  It's unlikely it will have to look far.
// Returns NULL if not found.
//
*/
SilencerGameServer_t * ServerList::FindNodeByIP(unsigned long int IP, unsigned short int port) {

   SilencerGameServer_t * ptr = BeginningOfServerlist;

   while(BeginningOfServerlist != NULL) {

      if(ptr->IntServerAddress == IP && ptr->IntServerPort == port) { return(ptr); }
      ptr = ptr->next;
   }
   return(NULL);
}



/* Functions related to sending or processing packets */

void ServerList::ChangeServerNetworkAddresses(UINT32 IP, UINT16 Port) {

   //This won't be pretty since we premake our packets...

   //First let's get the proper endian.  I think it was big endian, for a change.
   //Since inet_addr returns a big endian var, that one's set.
   Port = hosttobe16(Port);

   //Now if you do the math, you'll know that the servers are listed at
   //AuthServerPacket+42, RoutingServerPacket+58, and RoutingServerPacket+105.

   *(unsigned short int *)(AuthServerPacket+42)     = Port;
   *(unsigned short int *)(RoutingServerPacket+58)  = Port;
   *(unsigned short int *)(RoutingServerPacket+104) = Port;

   *(unsigned long int *)(AuthServerPacket+44)     = IP;
   *(unsigned long int *)(RoutingServerPacket+60)  = IP;
   *(unsigned long int *)(RoutingServerPacket+106) = IP;
}

/* These two functions do little more than pass a premade packet
// back to the running threads (and its length).  This really
// is just a little bit of overhead on top of what I originally had--
// send(socket, PAYLOAD, sizeof)payload), 0).  But moving it here
// seems nicer, for some reason.
*/
int ServerList::GetAuthPacket(unsigned char ** packetptr) {
   *packetptr = AuthServerPacket;
   return(0x30);
}
int ServerList::GetRoutingPacket(unsigned char **packetptr) {
   *packetptr = RoutingServerPacket;
   return(0x6E);
}

#define HEARTBEAT_HEADER "\x05\x02\x00\xcb\x00\x08"
#define HEARTBEAT_FOOTER "\x2c\x01\x00\x00\x01\x00\x01\x00\x02\x00"

/* DESCRIPTION: ProcessHeartbeat
//
// Once the listening thread has determined it has a heartbeat (it's
// basically processed the header), it forwards the rest of the packet
// here for processing.  We extract the relevant info, make a server struct,
// and tack it on to the server linked list.  Unless it's a broken heartbeat.
// Then we just chuck it.
*/
void ServerList::ProcessHeartbeat(const unsigned char * packet, unsigned int len) {

   unsigned char strings[64];
   int LengthOfString;

   UINT32 IPaddress;
   UINT32 IPport;

   SilencerGameServer_t * ptr;

   //bypass the header.
   packet += (sizeof(HEARTBEAT_HEADER)-1);
   len -= (sizeof(HEARTBEAT_HEADER)-1);


   //After the header the thing should identify itself as a game server.

   if(ExtractUnicodeString(&packet, len, strings, sizeof(strings)) == sizeof("/Silencer/GameServers")-1 &&
      memcmp(strings, "/Silencer/GameServers", sizeof("/Silencer/GameServers")-1) == 0) {

      len -= (((sizeof("/Silencer/GameServers")-1) << 1) + 2);

      if(ExtractUnicodeString(&packet, len, strings, sizeof(strings)) == sizeof("GameServer")-1 &&
         memcmp(strings, "GameServer", sizeof("GameServer")-1) == 0) {

         len -= (((sizeof("GameServer")-1) << 1) + 2);

         //Time to extract the port and IP.
         if(len > 7 && *packet == 6) {
            packet++;
            ExtractWordntohs (&packet, &IPport);
            ExtractDWordntohs(&packet, &IPaddress);

            len -= 7;

            //Now we get into actual usable stuff, like the server name.
            LengthOfString = ExtractUnicodeString(&packet, len, strings, sizeof(strings)-2);

            if(LengthOfString > 0 && len >= (sizeof(HEARTBEAT_FOOTER)+1) &&
               memcmp(HEARTBEAT_FOOTER, packet, sizeof(HEARTBEAT_FOOTER)-1) == 0) {

               strings[LengthOfString] = '\0';
               len -= ((LengthOfString << 1) + (sizeof(HEARTBEAT_FOOTER)+1));
               packet += (sizeof(HEARTBEAT_FOOTER)-1);

               //The math is correct, trust it.
               printf("Heartbeat: Server %u.%u.%u.%u:%u named %s has %u of %u players.\n", (*(((char *)(&IPaddress))+3)), (*(((char *)(&IPaddress))+2)), (*(((char *)(&IPaddress))+1)), (*(((char *)(&IPaddress))+0)), IPport, strings, packet[0], packet[1]);

               pthread_mutex_lock(&Serverlist_lock);

               ptr = FindNodeByIP(IPaddress, IPport);
               if(ptr == NULL) { //not found, make our own

                  ptr = NewNode();
                  if(ptr == NULL) {
                     printf(SIL_NOMEMERROR);
                  }
                  else {
                     ptr->IntServerAddress = IPaddress;
                     ptr->IntServerPort = IPport;

                     ptr->Players = packet[0]; //The last two bytes are player and max, respectively
                     ptr->PlayerMax = packet[1];

                     ptr->LengthOfServerName = LengthOfString;
                     memcpy(ptr->ServerName, strings, LengthOfString);
                     ptr->last_heartbeat = (unsigned int)time(NULL);
                     AddNode(ptr);
                  }
               }
               else {
                  //I think this is the only bit that can change.
                  ptr->Players = packet[0];
                  ptr->last_heartbeat = (unsigned int)time(NULL);

                  // Just to mix things up (and keep things fresh if there are
                  // many servers), move it to the front of the linked list.
                  // unimplemented right now, possible tweak later.
               }
               pthread_mutex_unlock(&Serverlist_lock);
               return;
            }
         }
      }
   }

   printf(SIL_NOTSILENCER);
   return;
}

/* DESCRIPTION: FillPacketBufferWithIPs
//
// It's assumed that the buffer is an outgoing packet.  This cycles through the
// serverlist and adds the whole IP PORT combo.  But there's an interesting
// question left:  We're using TCP/IP.  There are no hard limits, only how
// much we're willing to send and how much Silencer is willing to take.
// Silencer however treats it like UDP, with a hard limit of 7D00h.  Well
// TCP can't be relied on to not split our answer into multiple parts, so I
// propose keeping it below 500 or so bytes  This is about 10 servers.
//
*/
#define GAMESERVER_PACKET_ENTITY1_HEADER "\x53\x0a\x00G a m e S e r v e r \x06"
#define GAMESERVER_PACKET_ENTITY2_HEADER "\x01\x00\x00\x00\x00\x00"
unsigned int ServerList::FillPacketBufferWithGameServers(unsigned char * packet, unsigned int SizeOfBuffer) {

   unsigned char * ptr;
   unsigned int packetlen;
   SilencerGameServer_t * serverptr;
   unsigned int servers;

   unsigned int currentTime;

   //We'll be messing with the serverlist continuously.  We might as well hold
   //the lock and keep it now and forevermore.

   pthread_mutex_lock(&Serverlist_lock);

   servers = 0;
   packetlen = 2;
   ptr = packet+2;
   serverptr = BeginningOfServerlist;
   currentTime = time(NULL);

   while(serverptr != NULL) {

      //Firrt things first, if a server's getting old, don't mess with it.
      if(serverptr->last_heartbeat + ServerTimeoutValueInSeconds < currentTime) {

         DeleteNode(serverptr);
         continue;
      }


      //The overhead for the server is 39 + 2*the length of its name
      if(packetlen + (39 + (serverptr->LengthOfServerName << 1)) >= SizeOfBuffer) { break; }

      //There's enough room if we're here, so start writing.
      WriteAndPassStringA(&ptr, GAMESERVER_PACKET_ENTITY1_HEADER, sizeof(GAMESERVER_PACKET_ENTITY1_HEADER)-1);

      //Port and IP
      WriteAndPassWordntohs(&ptr, &(serverptr->IntServerPort));
      WriteAndPassDWordntohs(&ptr, &(serverptr->IntServerAddress));

      //Write the server's name and size.
      WriteAndPassWord(&ptr, &(serverptr->LengthOfServerName));
      WriteAndPassStringW(&ptr, serverptr->ServerName, serverptr->LengthOfServerName);

      //There's some more weird junk here
      WriteAndPassStringA(&ptr, GAMESERVER_PACKET_ENTITY2_HEADER, sizeof(GAMESERVER_PACKET_ENTITY2_HEADER)-1);

      //Player totals
      WriteAndPassByte(&ptr, &(serverptr->PlayerMax));
      WriteAndPassByte(&ptr, &(serverptr->Players));

      servers++;
      packetlen += (39 + (serverptr->LengthOfServerName << 1));
      serverptr = serverptr->next;
   }

   // We can easily check and see if we're aborting early by checking serverptr
   // for a null, but there's not really anything we can do about it.
   pthread_mutex_unlock(&Serverlist_lock);

   //We skipped the first two bytes because we didn't know this yet.
   WriteWord(packet, servers);

   return(packetlen);
}

