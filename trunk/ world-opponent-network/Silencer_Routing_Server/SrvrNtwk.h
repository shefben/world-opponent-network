/*
//
// This class deals with the management of servers on the network.  It
// originally came from the Half-Life master, but can be adapted.  It differs
// in that while HL is expected to maintain several hundred (or more) servers
// that change often, Silencer's list changes only on occasion.
//
*/


#ifndef __WE_HAVE_ALREADY_INCLUDED_SERVER_NETWORK_H_
#define __WE_HAVE_ALREADY_INCLUDED_SERVER_NETWORK_H_

#include "silencerversion.h"
#include "endianin.h"
/*** The server ***/

typedef struct SilencerGameServer_s {

	struct SilencerGameServer_s *next; //tree linkage; only left and previous are being used

   unsigned int IntServerAddress; //IPv6 will require updating
   unsigned int IntServerPort;

   unsigned int last_heartbeat; //that's for timeout

   unsigned int Players;
   unsigned int PlayerMax;
   unsigned int LengthOfServerName;
   char ServerName[64];

} SilencerGameServer_t;

/* The class */

class ServerList {

private:

   /* lists */
   SilencerGameServer_t * BeginningOfServerlist;
   unsigned int ServerTimeoutValueInSeconds;

   pthread_mutex_t Serverlist_lock;

   SilencerGameServer_t * FindNode(unsigned long int);
   SilencerGameServer_t * FindNodeBySockaddr(struct sockaddr_in);
   SilencerGameServer_t * FindNodeByIP(unsigned long int, unsigned short int);

   SilencerGameServer_t * NewNode();
   void DeallocateServers();
   void AddNode(SilencerGameServer_t *);
   void DeleteNode(SilencerGameServer_t *);
   void FreeUnlinkedNode(SilencerGameServer_t *);

public:

   void ChangeServerNetworkAddresses(UINT32, UINT16);
   int GetAuthPacket(unsigned char **);
   int GetRoutingPacket(unsigned char **);
   void ProcessHeartbeat(const unsigned char *, unsigned int); //I would like to include the sockaddr for later.
   unsigned int FillPacketBufferWithGameServers(unsigned char *, unsigned int);


   /* constructor/destructor */
   ServerList();
   ~ServerList();
};

#endif
