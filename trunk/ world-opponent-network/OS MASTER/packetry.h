/*
//
// This 'class' is the meat of the code.  There should only ever be one
// instance of this class ever, and only one thing for main() to call.
//
// This class acts as the parent of both the input and output classes.
// All that is here belongs to them too.
//
*/

#ifndef __MASTER__SERVER__PACKETRY_H_
#define __MASTER__SERVER__PACKETRY_H_

#include <pthread.h>
#include <time.h>
#include "port.h"
#include <stdio.h>
#include <ctype.h>
#include "externs.h"
#include "input.h"
#include "WONnetwk.h"
#include "ModList.h"

#ifndef MAX_PACKET_LENGTH
#define MAX_PACKET_LENGTH 1400
#endif

//#define DEBUG_NOFILTER
/*** The class ***/

// There can only be one.

typedef class PacketManagement {

private:

   signed int socket_GLOBAL;

   /* packet related vars */

   signed int CurrentNumberOfBytesReceived;
   struct sockaddr_in CurrentClientSockAddress;
   socklen_t sizeofClientAddress;
   char packetbuffer[MAX_PACKET_LENGTH+1]; //One char of wiggle room, hehe

   /*** Functions ***/

   /*  Packet handling related */

   void ProcessPacket();
   void StripOutUnwantedNullsInPacket(char*);
   char* GetKeyPairValue(const char*, const char*);
   unsigned int GetNextToken(const char*, char*, const unsigned int, const unsigned int, const char);
   unsigned int GetNextToken(const char*, char*, const unsigned int, const unsigned int);

// When sending or receiving data, '\0', '\n', and '\r' are removed.  Half-Life
// does it, HL expects us to do it when it sends us something, we do it.

   int   IN_x_ProcessModRequest();
   void OUT_y_SendModRequest(struct sockaddr_in, const char *);


public:

   PacketManagement();
   ~PacketManagement();

   void OUT_SendSpecificReply(char*, unsigned int, struct sockaddr_in);
   void ProcessInput();

   int StartTheNetwork();
   void DisableNetwork();

} PacketManagement_c;


#endif
