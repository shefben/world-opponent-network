#include "SV_chals.h"
#include "cmd.h"
#include "random.h"
#include "report.h"
#include "SV_mcast.h"
#include "sys.h"

/*** STRUCTS ***/
typedef struct challenge_s {

   netadr_t address;
   int time;
   int challenge;
} challenge_t;


/*** GLOBALS ***/
static challenge_t g_rg_sv_challenges[1024];


/*** FUNCTIONS ***/

/* DESCRIPTION: SV_CheckChallenge
// LOCATION: Master
// PATH: ConnectClient and rcon_validate
//
// UDP is easily spoofed.  Challenge numbers make it a roughly
// 1 in 2 billion guessing attack, assuming our random selection is
// unpredictable.  I remember this from the master; it's pretty easy.
*/
int  SV_CheckChallenge(netadr_t * client, int challenge) {

   int i;


   if(client == NULL) { Sys_Error("SV_CheckChallenge: Null in arguments,\n"); return(0); }

   if(NET_IsLocalAddress(*client)) { return 1; }

   for(i = 0; i < 1023; i++) { //not how I prefer to do it actually.

      if(NET_CompareBaseAdr(global_net_from, g_rg_sv_challenges[i].address) != 0) {

         if(challenge == g_rg_sv_challenges[i].challenge) {
            return(1);
         }
         else {
           // SV_RejectConnection(client, "Denied--bad challenge.\n");
            return(0);
         }
      }
   }

   SV_RejectConnection(client, "Denied--no challenge for your address.\n");
   return(0);
}
void SVC_GetChallenge() {

   #define OUT_GETCHALLENGE "\xff\xff\xff\xff" "A00000000 %u 2\n"
   #define MAX_SIZEOF_OUTGC sizeof(OUT_GETCHALLENGE) + 9
   //We write one integer, which can (and probably will) take up ten bytes.
   //Ten - the two bytes taken up by %u == 8.
   int i;
   int oldest;
   int oldestTime;
   char packet[MAX_SIZEOF_OUTGC];


   oldest = 0;
   oldestTime = 0x7fffffff;

   for(i = 0; i < 1024; i++) { //todo: hash index like in master3
      if(NET_CompareBaseAdr(global_net_from, g_rg_sv_challenges[i].address) != 0) {
         break;
      }
      if(g_rg_sv_challenges[i].time < oldestTime) {
         oldestTime = g_rg_sv_challenges[i].time;
         oldest = i;
      }
   }

   if(i == 1024) { //no match, overwrite

      g_rg_sv_challenges[oldest].challenge = (RandomLong(0, 0x7FFFFFFF) << 1) ^ RandomLong(0, 0x7FFFFFFF);
      g_rg_sv_challenges[oldest].address = global_net_from;
      g_rg_sv_challenges[oldest].time = global_realtime;
      i = oldest;
   }
   else { //I'd like to create a new challenge number IF necessary.

      if(g_rg_sv_challenges[oldest].time + 8 < global_realtime) {

         g_rg_sv_challenges[oldest].challenge = (RandomLong(0, 0x7FFFFFFF) << 1) ^ RandomLong(0, 0x7FFFFFFF);
      }
   }

   //gfUseLANAuthentication = 1;

   //Here we tell the client what authentication protocol to use.
   //Well right now we only have one--hashed CD keys.

   //reuse i as the size.
   i = sprintf(packet, OUT_GETCHALLENGE, g_rg_sv_challenges[i].challenge) + 1;
   NET_SendPacket(NS_SERVER, i, packet, global_net_from);
}
void SVC_ServiceChallenge() {

   #define OUT_CHALLENGE "\xff\xff\xff\xff" "challenge rcon %u\n"
   #define MAX_SIZEOF_CH sizeof(OUT_CHALLENGE) + 8

   int i;
   int oldest;
   int oldestTime;
   char packet[MAX_SIZEOF_CH];


   if(Cmd_Argc() != 2 || Q_strcasecmp(Cmd_Argv(1), "rcon") != 0) { return; }

   oldest = 0;
   oldestTime = 0x7fffffff;
   for(i = 0; i < 1024; i++) { //todo: hash table like in master3
      if(NET_CompareBaseAdr(global_net_from, g_rg_sv_challenges[i].address) != 0) {
         break;
      }
      if(g_rg_sv_challenges[i].time < oldestTime) {
         oldestTime = g_rg_sv_challenges[i].time;
         oldest = i;
      }
   }

   if(i == 1024) {

      g_rg_sv_challenges[oldest].challenge = (RandomLong(0, 0x7FFFFFFF) << 1) ^ RandomLong(0, 0x7FFFFFFF);
      g_rg_sv_challenges[oldest].address = global_net_from;
      g_rg_sv_challenges[oldest].time = global_realtime;
      i = oldest;
   }
   else if(g_rg_sv_challenges[oldest].time + 8 < global_realtime) {

      g_rg_sv_challenges[oldest].challenge = (RandomLong(0, 0x7FFFFFFF) << 1) ^ RandomLong(0, 0x7FFFFFFF);
   }

   i = sprintf(packet, OUT_CHALLENGE, g_rg_sv_challenges[i].challenge) + 1;
   NET_SendPacket(NS_SERVER, i, packet, global_net_from);
}
