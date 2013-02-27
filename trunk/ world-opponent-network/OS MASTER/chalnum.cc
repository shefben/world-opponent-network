#include "chalnum.h"

/******** Constructor/Destructor ********/

ChallengeList::ChallengeList() {
   PurgeAllChallenges();
}


/* DESCRIPTION: PurgeAllChallenges
//
// Zeros out the array.  Called by the constructor.  It doesn't appear to
// be necessary, as it's zeroed in the debugger regardless; could be compiler
// specific though, so I'll leave it.
//
*/
void ChallengeList::PurgeAllChallenges() {

   memset(ChallengeNumbers, 0xFFFFFFFF, sizeof(ChallengeNumbers));
   memset(ChallengeAddresses, 0, sizeof(ChallengeAddresses));
   memset(ArrayPositions, 0, sizeof(ArrayPositions));

   srand(time(NULL));
   HashBase = ((rand() ^ (rand()<<17)) ^ (rand()<<9)); //RAND_MAX is 7FFF for my libs.

}


/* DESCRIPTION: ComputeHash
//
// Computes a hash.  A hash of a 32 bit number, that is.  There are three
// requirements: even distribution, values will not necessarially return the
// same hash when the program is stopped and restarted, and values that all
// share the same hash in one program instance should be split up in another.
//
// Even distribution is met.  So are unique individual hashes.  After trying
// many things, I was able to reduce clumping, but there are definitely
// patterns in the distribution.  I tried; I'm not a hashing expert, but it
// does do all three, and it should be at a level where predicting and DoSing
// someone's challenge requests should be next to impossible without luck or a
// full blown denial of service attack.
//
*/
unsigned int ChallengeList::ComputeHash(unsigned long int senderIP) {

   //BINARY_SHIFT is a define, replacing that ugly mod (%).
   return(((senderIP + HashBase) ^ ((senderIP + HashBase)<<16) | ((senderIP+ HashBase)>>16))BINARY_SHIFT);
}


/* DESCRIPTION: AssignChallengeaddr
//
// Merely calls the corresponding IP function.
//
*/
signed long int ChallengeList::AssignChallengeaddr(struct sockaddr_in sender) {

   return(AssignChallengeIP(sender.sin_addr.s_addr));
}

/* DESCRIPTION: VerifyChallengeaddr
//
// Just calls the IP function as it is needed.
//
*/
char ChallengeList::VerifyChallengeaddr(struct sockaddr_in sender, signed long int PassedChallenge) {

   return(VerifyChallengeIP(sender.sin_addr.s_addr, PassedChallenge));
}


/* DESCRIPTION: AssignChallengeIP
//
// Takes an IP, computes a random 'challenge value' (which is returned), and
// then adds that number AND the address to our big table O challenges.
// The bulk of the code is actually for the situation where we still HAVE an
// an active challenge for the sender; in that case we do some reorganizing.
//
*/
signed long int ChallengeList::AssignChallengeIP(unsigned long int senderIP) {

   signed int challenge = 0;
   unsigned int ServerHash = ComputeHash(senderIP);

   //We add our challenge number to our table.
   //Sort've.  First we have to check if they have one already.

   for(int i = 0; i < ELEMENTS_PER_HASH; i++) {
      if(ChallengeAddresses[ServerHash][i] == senderIP) {
         //Hey, they still have one.  Return it to them, and reorder the list.


         challenge = ChallengeNumbers[ServerHash][i];

         while(i != ArrayPositions[ServerHash]) {
            if(i+1 < ELEMENTS_PER_HASH) {
               ChallengeAddresses[ServerHash][i] = ChallengeAddresses[ServerHash][i+1];
               ChallengeNumbers[ServerHash][i] = ChallengeNumbers[ServerHash][i+1];
               i++;
            }
            else {
               ChallengeAddresses[ServerHash][i] = ChallengeAddresses[ServerHash][0];
               ChallengeNumbers[ServerHash][i] = ChallengeNumbers[ServerHash][0];
               i = 0;
            }
         }
         ChallengeAddresses[ServerHash][i] = senderIP;
         ChallengeNumbers[ServerHash][i] = challenge;


         ArrayPositions[ServerHash] = (i++) ELEMENTS_PER_HASH_MOD;
         return(challenge);
      }
   }

   //We get to here, then they have no challenge number and need one.

      challenge = ((rand() | (rand()<<17) | (rand()<<9)) & 0x7FFFFFFF);

   /* Explanation time: For some reason HL never sends a challenge with
   // the first bit as a 1.  Is this intentional, or an issue with their rand?
   // Doesn't matter, but just in case I do the same thing.  Another issue is
   // we need to have some sort of 'never assigned' value to default to when
   // allocating the memory so people can't guess that value and have it work.
   // Since we never have the last bit set, I chose 0xFFFFFFFF as that value
   // (check the constructor).  It's also checked for and rejected in the
   // VerifyChallengeIP function--so we should be safe from the defaults.
   */

   //Now just replace the oldest element and be done.

   ChallengeAddresses[ServerHash][ArrayPositions[ServerHash]] = senderIP;
   ChallengeNumbers[ServerHash][ArrayPositions[ServerHash]] = challenge;

   //Whew!  That was hard.  Let's go grab some beers.

   return(challenge);
}

/* DESCRIPTION: VerifyChallengeIP
//
// A very quick search and return.  1 means we found it, 0 means you're SOL.
//
*/
char ChallengeList::VerifyChallengeIP(unsigned long int senderIP, signed long int PassedChallenge) {

   if(PassedChallenge & 0x80000000) {
      return(0);
   } //good guesser guard


   unsigned int ServerHash = ComputeHash(senderIP);

   for(int i = 0; i < ELEMENTS_PER_HASH; i++) {
      if(ChallengeAddresses[ServerHash][i] == senderIP) {
         if(ChallengeNumbers[ServerHash][i] == PassedChallenge) {
            return(1);
         }
         else{
            return(0);
         }
      }
   }
   return(0);
}
