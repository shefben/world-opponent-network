/*
//
// This class deals with challenge numbers.  In Half-Life challenge numbers
// exist mainly to prevent IP spoofing, and to prevent having servers be
// unwitting participants in DDoS attacks.  The HL servers don't pull it off
// so well, but there's no reason why we shouldn't.  Especially since it's part
// of the engine and we have to anyway :).
//
// Basically we get a request for a challenge and we pass it to this class.
// The class will store it, and the IP of the requester, in some sort of table.
// The table has a finite size and as elements are added old ones are removed.
// To prevent a serious DoS attack, some sort of hashing must be used so that
// if someone issues out many challenges they will only succeed in filling up
// their block, denying only themselves and a few other IPs unfortunate to
// share the hash of the master server.  Later, when a challenge number is
// received, this class will verify its authenticity.
//
// Oh yeah.  Don't call this baby statically.  You'll kill the stack.  Always
// declare this class with malloc or new.
//
*/
#include "port.h"  //port includes networking stuff
#include <string.h>
#include <stdlib.h>   //need rand();
#include <time.h>

#ifndef __MASTER__SERVER__CHALNUM_H_
#define __MASTER__SERVER__CHALNUM_H_


#define MAX_HASH_VALUE 4096
#define ELEMENTS_PER_HASH 16
#define BINARY_SHIFT >>20  //If you change MAX_HASH_VALUE do it here too
#define ELEMENTS_PER_HASH_MOD & 0x0000000F//equiv to a "% ELEMENTS_PER_HASH
//that's about 8 megs of memory for what we want by my count

/*** The class ***/


// Only one class exists, but it's self-contained.  You could add more.
typedef class ChallengeList {

private:
   signed long int ChallengeNumbers[MAX_HASH_VALUE][ELEMENTS_PER_HASH];
   unsigned long int ChallengeAddresses[MAX_HASH_VALUE][ELEMENTS_PER_HASH];
   unsigned char ArrayPositions[MAX_HASH_VALUE];
   int HashBase;

public:
   ChallengeList(); //For zeroing it out.

   void PurgeAllChallenges(); //Eh; we might need it.

   char VerifyChallengeaddr(struct sockaddr_in, signed long int);
   char VerifyChallengeIP(unsigned long int, signed long int);

   signed long int AssignChallengeaddr(struct sockaddr_in);
   signed long int AssignChallengeIP(unsigned long int);
   unsigned int ComputeHash(unsigned long int);

} ChallengeList_c;


#endif
