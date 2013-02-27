#include "SV_frame.h"

#include "host.h"
#include "master.h"
#include "Modding.h"
#include "NET.h"
#include "SV_clres.h"
#include "SV_mcast.h"
#include "SV_pkt.h"
#include "SV_pktcl.h"
#include "SV_phys.h"
#include "sys.h"

/*** GLOBALS ***/

static cvar_t cvar_sv_timeout  = {  "sv_timeout",  "60", FCVAR_NONE, 60.0, NULL };
       cvar_t cvar_clockwindow = { "clockwindow", "0.5", FCVAR_NONE,  0.5, NULL }; //used in NET.c as well, but it's in the init so I'm okay with that.


/*** FUNCTIONS ***/



//checks and balances

/* DESCRIPTION: SV_CheckTimeouts
// PATH: SV_Frame
//
// If a client's been doing nothing for a prolonged period of time, we
// boot them to make room for someone a little more active.
*/
void SV_CheckTimeouts() {

   unsigned int i;
   double CutoffValue;
   client_t * ptrClient;


   CutoffValue = global_realtime - cvar_sv_timeout.value;
   for(ptrClient = global_svs.clients, i = 0; i < global_svs.allocated_client_slots; i++, ptrClient++) {

      if((ptrClient->padding00_used == 0 && ptrClient->padding0C_connected == 0 && ptrClient->padding04 == 0) ||
         (ptrClient->padding2438_IsFakeClient != 0)) { continue; }

      if(ptrClient->netchan1C.last_received < CutoffValue) {

         SV_BroadcastPrintf("%s timed out\n", ptrClient->PlayerName);
         SV_DropClient(ptrClient, 0, "Timed out.\n");
      }
   }
}
/* DESCRIPTION: SV_CheckMapDifferences
// PATH: SV_Frame
//
// Every five seconds, this loops through all of our clients and...
// well, I'm not actually sure.  Apparently it compares the CRCs of the maps.
// But why would they change in between connect and shutdown?
*/
void SV_CheckMapDifferences() {

   unsigned int i;
   client_t * ptrClient;
   static double lastcheck = 0;


   if(global_realtime - lastcheck < 5) { return; }
   lastcheck = global_realtime;


   for(ptrClient = global_svs.clients, i = 0; i < global_svs.allocated_client_slots; i++, ptrClient++) {

      if(ptrClient->padding00_used == 0 || ptrClient->local_map_CRC == 0 || ptrClient->netchan1C.remote_address.type == NA_LOOPBACK) { continue; }
      if(global_sv.map_CRC != ptrClient->local_map_CRC) {
         ptrClient->netchan1C.netchan_message.overflow |= 2; //I'm not sure why we do this.
      }
   }
}
/* DESCRIPTION: SV_CheckCmdTimes
// LOCATION:
// PATH: SV_Frame
//
// Manipulates a few variables in the client structures.  There are so many
// damnable counters, it's gonna take a miracle to sort them all out.
// Based on the function name, I'd say these ones have to do with commands...
*/
void SV_CheckCmdTimes() {

   unsigned int i;
   client_t * ptrClient;
   double varc;
   float ClockWindowVar;
   static double lastreset = 0;


   if(1.0 > global_realtime - lastreset) { return; }

   lastreset = global_realtime;
   for(i = 0; i < global_svs.allocated_client_slots; i++) { //It's more optimized in ASM to increment the pointer BTW

      ptrClient = &(global_svs.clients[i]);
      if(ptrClient->padding0C_connected == 0 || ptrClient->padding00_used == 0) { continue; }
      if(ptrClient->padding2474_LastCommandTime == 0) {
         ptrClient->padding2474_LastCommandTime = global_realtime;
      }

      varc = ptrClient->padding247C_LastRunTime + ptrClient->padding2474_LastCommandTime - global_realtime;
      ClockWindowVar = cvar_clockwindow.value;
      if(varc > ClockWindowVar) {

         ptrClient->padding2484_NextRunTime = ClockWindowVar + global_realtime;
         ptrClient->padding247C_LastRunTime = global_realtime - ptrClient->padding2474_LastCommandTime;
      }
      else if(varc < -ClockWindowVar) {

         ptrClient->padding247C_LastRunTime = global_realtime - ptrClient->padding2474_LastCommandTime;
      }
   }
}
/* DESCRIPTION: SV_IsSimulating
// LOCATION:
// PATH: SV_Frame
//
// Returns 1.  I'm guessing 'isSimulating' means 'isRunningAMultiplayerServer',
// or something to that effect, but I'm not really interested in mapping it all out.
*/
int SV_IsSimulating() {

   //The ordering is funky.  Cound be important though.

   /* This kind've indicates there's more to it, but for a dedicated server,
   // I think we're always 'simulating'.
   if(global_sv.padding04 != 0) { return(0); }
   if(global_svs.maxclients > 1) { return(1); }
   if(*key_dest != 0) { return(0); }
   if(global_cls.state == ca_dedicated || global_cls.state == ca_active) { return(1); }

   return(0);
   */
   if(global_sv.padding04 != 0) { return(0); }
   return(1);
}
/* DESCRIPTION: SV_GatherStatistics
// LOCATION:
// PATH: SV_Frame
//
// Adds subtracts and multiplies its way to victory.  So what if we don't
// actually know what it's tracking...
*/
void SV_GatherStatistics() {

   unsigned int i, var_10;
   float var_14;
   client_t * client;

   if(global_realtime < global_svs.padding38) {

      SV_CountPlayers(&i);
      global_svs.padding38 = 3600 + global_realtime;

      global_svs.padding48 = 0;
      global_svs.padding4C = 0;
      global_svs.padding50 = 0;
      global_svs.padding54 = 0;
      global_svs.padding58 = 0;
      global_svs.padding5C = i;
      global_svs.padding60 = i;
      global_svs.padding64 = 0;
      global_svs.padding68 = 0;
      global_svs.padding6C = 0;
      global_svs.padding70 = 0;
      global_svs.padding74 = 0;
      global_svs.padding78 = 0;
      global_svs.padding7C = 0;

      return;
   }

   if(global_realtime > global_svs.padding40) { return; }

   global_svs.padding48++;
   global_svs.padding40 = 60 + global_realtime;
   SV_CountPlayers(&i);

   global_svs.padding64 += (i * 100) / global_svs.allocated_client_slots;
   if(global_svs.padding48 > 0) { //doesn't look possible

      global_svs.padding68 = global_svs.padding64 / global_svs.padding48;
   }

   if(i < global_svs.padding5C) {
      global_svs.padding5C = i;
   }
   else if(i > global_svs.padding60) {
      global_svs.padding60 = i;
   }

   if(i >= global_svs.allocated_client_slots - 1) {
      global_svs.padding4C++;
   }
   if(i <= 1) {
      global_svs.padding50++;
   }

   if(global_svs.padding48 > 0) {

      global_svs.padding54 = (global_svs.padding4C * 100) / global_svs.padding48;
      global_svs.padding58 = (global_svs.padding50 * 100) / global_svs.padding48;
   }

   //i is now a free temp variable.
   var_14 = 0;
   var_10 = 0;
   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0 || client->padding2438_IsFakeClient != 0) { continue; }

      var_10++;
      var_14 += client->ping;
   }

   if(var_10 > 1) {
      var_14 = var_14 / var_10; //average ping;
   }

   global_svs.padding78 += var_14;
   if(global_svs.padding48 > 0) {

      global_svs.padding7C = global_svs.padding78 / global_svs.padding48;
   }

   global_svs.padding74 = global_svs.padding70 / global_svs.padding6C;
}



void SV_Frame() {

   if(global_sv.active == 0) { return; }

   gGlobalVariables.frametime = host_frametime;
   global_sv.time14 = global_sv.time0c;

   SV_CheckCmdTimes();
   SV_ReadPackets();
   if(SV_IsSimulating()) { SV_Physics(); }

   global_sv.time0c += host_frametime;

   SV_QueryMovevarsChanged();
   SV_RequestMissingResourcesFromClients();

   // if(Steam) don't do this.  hehe.
   //WON_HandleServerAuthMsgs();

  // ValveAuth_Init(); This function would be a lot more interestign if I still cared about the auth server.
   Master_Heartbeat();
   SV_CheckTimeouts();
   //SV_SecurityUpdate();
   SV_SendClientMessages();
   SV_CheckMapDifferences();
   SV_GatherStatistics();
   //SV_ProcessSteamClientConnections(); //I've a feeling we can skip this...
}


/* DESCRIPTION: SV_Frame_Init
// PATH: SV_Init
//
// Place those cvars where they need to go.
*/
void SV_Frame_Init() {

   Cvar_RegisterVariable(&cvar_clockwindow);
   Cvar_RegisterVariable(&cvar_sv_timeout);
}


