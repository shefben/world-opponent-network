/* This class deals with the management of other masters on the network.
// It's also being designed with both auth and titan servers in mind.  Many
// of the actions have three variants dealing with the three unique servers,
// and one base function that actually does the modifications.
*/

#ifndef __MASTER__SERVER__WON2_SERVER_NETWORK_H_
#define __MASTER__SERVER__WON2_SERVER_NETWORK_H_

#include <pthread.h>
#include "port.h"
#include "externs.h"
#include "packetry.h"
#include <string>

#ifndef MAX_PACKET_LENGTH
#define MAX_PACKET_LENGTH 1400
#endif

/*** Structs ***/
typedef struct WON2modlist_s {
   struct WON2modlist_s *next, *previous; //tree linkage
   std::string *Name; //Transistor pointed out that most of our servers are on dynamic IPs
    int *uniqueid;
    char *game;
	char *gdir;
	char *url_dl;
    char *url_info;
    double *version;
    int *size
    char *type;

} WON2Modlist_t;

/*** The class ***/

typedef class WON2ServerNetwork {

private:

   /* lists */
   WON2Modlist_t *Mods;

   unsigned int NumMods;

   void DeallocateMods();

   char AddMod(const char *, WON2Modlist_t **);

   void Remake_w_replypacket();
   char OUT_w_replyingpacket[MAX_PACKET_LENGTH];
   unsigned int OUT_w_replyingpacketsize;

   /* Thread related */
   pthread_mutexattr_t WONnetwk_linklist_lockAttribute;
   pthread_mutexattr_t WONnetwk_packet_lockAttribute;

public:

   /* Thread locks */
   pthread_mutex_t WONnetwk_linklist_lock;
   pthread_mutex_t WONnetwk_packet_lock;

   //A little overloading...
   char AddMod(char *);

   void send_IN_w_ReplyPacket(struct sockaddr_in);

   /* constructor/destructor */
   WON2ServerNetwork();
   ~WON2ServerNetwork();
} WON2ServerNetwork_c;

#endif
