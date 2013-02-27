/*
//
// A class for handling the relatively simple task of dropping packets
//
*/
#ifndef __MASTER__SERVER__BANLIST_H_
#define __MASTER__SERVER__BANLIST_H_
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "port.h"  //so it recognizes sockaddr_in

/*** The class ***/

//An IP linked list.  May eventually be tree.  Almost, but not quite, entirely
//unlike sin_addr.  But we need that pointer.

typedef struct IPBanListStructure_s {
   unsigned long int address;
   unsigned long int subnetmask;
   struct IPBanListStructure_s *next;
} IPBanListStructure_t;

typedef class IPBanList {

private:
   IPBanListStructure_t * BeginningOfBanlist;

public:
   IPBanList();
   ~IPBanList();

   // And the basic thread protection:
   pthread_mutex_t Banlist_undesirables_lock;

   int BanSockaddr(struct sockaddr_in);
   int UnbanSockaddr(struct sockaddr_in);
   int IsSockaddrBanned(struct sockaddr_in);

   int BanIP(unsigned long int, unsigned long int); //s_addr may be typecast to int
   int UnbanIP(unsigned long int, unsigned long int);
   int IsIPBanned(unsigned long int);

   void RetrieveBanlist(void **address, unsigned long int* IP, unsigned long int* CIDR);


   int ParseBanString(const char*, const char);
} IPBanList_c;


#endif
