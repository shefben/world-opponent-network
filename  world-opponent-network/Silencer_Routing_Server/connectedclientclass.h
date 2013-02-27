#include "silencerversion.h"

#ifndef __WE_HAVE_ALREADY_INCLUDED_CONNECTEDCLIENTCLASS
#define __WE_HAVE_ALREADY_INCLUDED_CONNECTEDCLIENTCLASS

class ConnectedClient {

private:

   unsigned int TCP_Socket;

   int IgnoreRemainingData();
   void KillThisConnection();

public:

   ConnectedClient();
   ConnectedClient(unsigned int OpenSocket) { TCP_Socket = OpenSocket; }
   ~ConnectedClient();

   void ProcessClient15101();
   void ProcessClient15100();
};

//15101 packet handling:

int IdentifyPacket15101(const unsigned char *, unsigned int);
int BuildGameServerPacket(unsigned char *);

#endif

