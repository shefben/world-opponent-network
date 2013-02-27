/* This class deals with the management of servers on the network.  This
// definitely needs to be a tree or something if we grow too much, but for
// now a linked list is acceptable.
//
// This manages the creation and deletion of nodes, but it is the programmer's
// responsibility to ensure the node is properly handled until linked.
*/

#ifndef __MASTER__SERVER__HLSERVER_SERVER_NETWORK_H_
#define __MASTER__SERVER__HLSERVER_SERVER_NETWORK_H_


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"
#include "externs.h"
#include "input.h"
#include "ModList.h"

/*** The server ***/

typedef struct HLserver_s {

   struct HLserver_s *left, *right, *previous; //tree linkage; only left and previous are being used
   unsigned int depth; //not presently used

   char _booleanvalues;
   /* this may change; will have to analyze returned packets
       0x01 = old
       0x02 = lan
       0x04 = proxy
       0x08 = proxy pointer
       0x10 = dedicated
       0x20 = secure
       0x40 = password
       0x80 = linux
   */
   unsigned int IntServerAddress;
   unsigned int IntServerPort;
   unsigned int last_heartbeat; //that's for timeout
   unsigned int hashvalue;

   unsigned short int players;
   unsigned short int maxplayers;
   unsigned short int fakeplayers;

   struct ActiveMods_s * GameDir;
   char proxyserver[64]; //seems to hold an IP address in text form.
   char map[64]; //64 seems to be hardcoded in the responses

#define CheckboolIsServerOld()          _booleanvalues & 0x01
#define CheckboolIsServerLan()          _booleanvalues & 0x02
#define CheckboolIsServerProxy()        _booleanvalues & 0x04
#define CheckboolIsServerProxyPointer() _booleanvalues & 0x08
#define CheckboolIsServerDedicated()    _booleanvalues & 0x10
#define CheckboolIsServerSecure()       _booleanvalues & 0x20
#define CheckboolIsServerPassworded()   _booleanvalues & 0x40
#define CheckboolIsServerLinux()        _booleanvalues & 0x80

#define SetboolIsServerOld()          _booleanvalues |= 0x01
#define SetboolIsServerLan()          _booleanvalues |= 0x02
#define SetboolIsServerProxy()        _booleanvalues |= 0x04
#define SetboolIsServerProxyPointer() _booleanvalues |= 0x08
#define SetboolIsServerDedicated()    _booleanvalues |= 0x10
#define SetboolIsServerSecure()       _booleanvalues |= 0x20
#define SetboolIsServerPassworded()   _booleanvalues |= 0x40
#define SetboolIsServerLinux()        _booleanvalues |= 0x80

#define ClearboolIsServerOld()          _booleanvalues &= 0xFE
#define ClearboolIsServerLan()          _booleanvalues &= 0xFD
#define ClearboolIsServerProxy()        _booleanvalues &= 0xFB
#define ClearboolIsServerProxyPointer() _booleanvalues &= 0xF7
#define ClearboolIsServerDedicated()    _booleanvalues &= 0xEF
#define ClearboolIsServerSecure()       _booleanvalues &= 0xDF
#define ClearboolIsServerPassworded()   _booleanvalues &= 0xBF
#define ClearboolIsServerLinux()        _booleanvalues &= 0x7F

} HLserver_t;

/* The class */

typedef class ServerList {

private:

   /* lists */
   HLserver_t * BeginningOfServerlist;
   HLserver_t * FirstFreeNode;
   unsigned int ReserveNodes;
   unsigned int ServerTimeoutValueInSeconds;

   void DeallocateServers();
   void DeallocateReserveNodes();

   HLserver_t * FindNodeByHash(unsigned long int);
   HLserver_t * FindNodeofEqualorLesserHash(unsigned long int);

   int ServerTypeCheck(const HLserver_t *, int, const char*);
   bool AreServersTimingOut;
   bool HaveServersEverTimedOut;
   /* Thread related */
   typedef void* (*lpfn_ServerTimeouts)(void *);
   lpfn_ServerTimeouts orig_ThreadServerTimeouts;
   pthread_t ThreadServerTimeouts;
   static void ServerTimeouts(void *);
   pthread_mutex_t Serverlist_lock;
   pthread_mutexattr_t Serverlist_lockAttribute;


public:

   unsigned int HashServer(struct sockaddr_in);
   unsigned int HashServerIP(unsigned long int, unsigned short int);

   //Todo: move this from packetry to srvrntwk
   HLserver_t * FindNodeBySockaddr(struct sockaddr_in);
   HLserver_t * FindNodeByIP(char);

   HLserver_t * NewNode();
   void FreeUnlinkedNode(HLserver_t *);
   void AddNode(HLserver_t *);
   void DeleteNode(HLserver_t *);
   unsigned long int FillPacketBufferWithIPs(unsigned long int*, const char*, const unsigned int, const int, const char*);
   //int AssignHeartbeatValuesFromPacket(struct HLserver*, const char*);

   int SetServerToTimeoutInOneMin(struct sockaddr_in);
   int ResetServerTimeout(struct sockaddr_in);
   int SpawnServerTimeouts();
   void ChangeServerTimeoutValue(unsigned int);

   void FlushServers();
   /* constructor/destructor */
   ServerList();
   ~ServerList();
} ServerList_c;

#endif
