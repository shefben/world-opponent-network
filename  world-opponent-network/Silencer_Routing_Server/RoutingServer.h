#include "silencerversion.h"

#ifndef __WE_HAVE_ALREADY_INCLUDED_SERVER1
#define __WE_HAVE_ALREADY_INCLUDED_SERVER1

//Stores the list of active conflicts.

typedef struct SilencerConflict_s {

	struct SilencerConflict_s *next; //tree linkage; only left and previous are being used
   char conflict[136]; //I don't know what all of it does and don't care.

} SilencerConflict_t;

class RoutingServer {

private:

   /* lists */
   SilencerConflict_t * BeginningOfConflictlist;
   unsigned int ConflictTimeoutValueInSeconds;

   pthread_mutex_t Conflict_lock;

   SilencerConflict_t * FindNode(unsigned long int);
   SilencerConflict_t * FindNodeBySockaddr(struct sockaddr_in);
   SilencerConflict_t * FindNodeByIP(unsigned long int, unsigned short int);

   SilencerConflict_t * NewNode();
   void DeallocateConflicts();
   void AddConflict(SilencerConflict_t *);
   void DeleteConflict(SilencerConflict_t *);
   void FreeUnlinkedConflict(SilencerConflict_t *);
   void FlushConflict();

public:

   int ProcessNewClient(const unsigned char *, unsigned int);
   void ProcessNewConflict(const unsigned char *, unsigned int);
   void RemoveConflict(const unsigned char *, unsigned int);
   void FillPacketBufferWithConflicts(unsigned char *, unsigned int);
   int GetConflictPacket(unsigned char **);
   RoutingServer();
   ~RoutingServer();
};


#endif

