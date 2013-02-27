#include "master.h"
#include "cmd.h"
#include "cvar.h"
#include "NET.h"
#include "SV_hlds.h"
#include "SV_rcon.h"
#include "sys.h"
#include "report.h"
#include "Q_memdef.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Master_Init();

typedef struct master_server_s {

   struct master_server_s * next;
   netadr_t address;
   int HasRequested;
   int ChallengeRequest;
   float last_heartbeat;
   float TimeOfRequest;

} master_server_t;

static master_server_t * masterlist = NULL;
static double MasterHeartbeatTimeout = 15;
static int gfNoMasterServer = 0; //I'd rather make this a cvar.

//aah, this is stuff I know.

void Master_AddServer(netadr_t *adr) {

   master_server_t *ptr;


   Master_Init();
   ptr = masterlist;

   //Is it already listed?
   while(ptr != NULL) {
      if(NET_CompareAdr(ptr->address, *adr) != 0) {
         return;
      }
      ptr = ptr->next;
   }

   //Not found, make a new entry.
   ptr = (master_server_t *)Q_Malloc(sizeof(master_server_t));
   if(ptr == NULL) { //malloc failure.  Just back away and let some other function handle it...
      return;
   }

   ptr->address = *adr;
   ptr->last_heartbeat = -15;
   ptr->next = masterlist;
   masterlist = ptr;
}
void Master_UseDefault() {

 //hlmaster.valvesoftware.com:27010
 //half-life.east.won.net:27010
   netadr_t a;
   if(NET_StringToAdr("master3.won2.steamlessproject.nl:27010", &a)) {
      Master_AddServer(&a);
   }
}

//simplified.  What need, authentication...
int Master_IsLanGame() {

   return(cvar_sv_lan.value ? 1 : 0);
}

//Aah, bets repeated info calls.
void Master_RequestHeartbeat(master_server_t * master) {

   unsigned int PlayerCount;
   char buffer[1400];
   unsigned int len;

   if(master == NULL) { return; }
   if(global_realtime - master->TimeOfRequest > MasterHeartbeatTimeout) { return; }

   SV_CountPlayers(&PlayerCount);

   //Let's write our packet.  If at any time we overflow, bail.  That's impossible for the current setup though.
   len = snprintf(buffer, sizeof(buffer),
                  "0\n\\"
                  "protocol\\%i\\"
                  "challenge\\%i\\"
                  "players\\%u\\"
                  "max\\%u\\"
                  "gamedir\\"
                  , 46, master->ChallengeRequest, PlayerCount, global_svs.allocated_client_slots);

   if(len > sizeof(buffer)) { return; }
   //For encapsulation reasons, there's no way to get a straight pointer to the
   //mod directory.  There's a function that'll write it into the packet though.
   len += COM_GetGameDirSize(buffer+len, sizeof(buffer)-len);


   len = snprintf(buffer+len, sizeof(buffer)-len,
                         "\\"
                         "map\\%s\\"
                         "password\\%i\\"
                         "os\\%c\\"
                         "secure\\%i\\"
                         "lan\\%i\\"
                         "dedicated\\%i\\"
                         "version\\%s\n"
                         , global_sv.name, ServerIsPassworded(), 'l'
                         , global_svs.paddingE8 != 0, Master_IsLanGame(), 'd', "7.1.1.2");

   NET_SendPacket(NS_SERVER, len, buffer, master->address);
}
void SVC_Heartbeat() {

   master_server_t * ptr;

   for(ptr = masterlist; ptr != NULL; ptr = ptr->next) {

      if(NET_CompareAdr(global_net_from, ptr->address) != 0) {

         ptr->ChallengeRequest = MSG_ReadLong();
         Master_RequestHeartbeat(ptr);
         return;
      }
   }
}
void Master_SetMaster_f() {

   int AreAdding;
   const char * arg_1, * arg_2;
   int port;
   netadr_t netaddress;
   master_server_t * ptr, * ptr2;

   if(Cmd_Argc() < 2 || Cmd_Argc() > 4) {

      Con_Printf("Setmaster <add | remove | enable | disable> <IP:port>\n");
      Con_Printf(" Current master list:\n");

      for(ptr = masterlist; ptr != NULL; ptr = ptr->next) {

         Con_Printf("  %s\n", NET_AdrToString(ptr->address));
      }

      Con_Printf(" End if list\n");
      return;
   }

   arg_1 = Cmd_Argv(1);
   if(arg_1[0] == '\0') { return; }

   if(Q_strcasecmp(arg_1, "disable") == 0) { gfNoMasterServer = 1; return; }
   if(Q_strcasecmp(arg_1, "enable") == 0) { gfNoMasterServer = 0; return; }

   if(Q_strcasecmp(arg_1, "add") == 0) { AreAdding = 1; }
   else if(Q_strcasecmp(arg_1, "remove") == 0) { AreAdding = 0; }
   else {

      Con_Printf("Setmaster:  Unknown command \"%s\".\n", arg_1);
      return;
   }

   arg_2 = Cmd_Argv(2);

   if(Cmd_Argc() == 4) {
      port = Q_atoi(Cmd_Argv(3));
      if(port < 1 || port > 65535) {
         Con_Printf("Setmaster: Invalid port.  Ports can't be larger than 65535.\n");
         return;
      }
   }
   else {
      port = 27010;
   }

   if(NET_StringToAdr(arg_2, &netaddress) == 0) {
      Con_Printf("Setmaster: Passed address could not be processed by StringToAdr.\n");
      return;
   }
   netaddress.port = port;

   if(AreAdding) {

      Master_Init();
      Master_AddServer(&netaddress);
      gfNoMasterServer = 0;
      Con_Printf("Master server %s:%u added.\n", arg_2, port);
   }
   else {

      //case 1: first node.
      if(NET_CompareAdr(masterlist->address, netaddress) != 0) {
         ptr = masterlist;
         masterlist = masterlist->next;
         Q_Free(ptr);
         return;
      }

      //case 2: some node afterwards
      for(ptr = masterlist; ptr->next != NULL; ptr = ptr->next) {
         if(NET_CompareAdr(ptr->next->address, netaddress) != 0) {

            ptr2 = ptr->next;
            ptr->next = ptr->next->next;
            Q_Free(ptr2);
            return;
         }
      }
      //case 3: not here
      Con_Printf("Master %s:%u couldn't be removed.  Couldn't find it.\n", arg_2, port);
   }
}
void Master_Heartbeat_f() {

   master_server_t * ptr;


   for(ptr = masterlist; ptr != NULL; ptr = ptr->next) {

      ptr->last_heartbeat = 0;
   }
}
void Master_Heartbeat() {

   master_server_t * ptr;


   if(gfNoMasterServer != 0 || global_sv.active == 0 || global_svs.allocated_client_slots <= 1) { return; }

   Master_Init();
   for(ptr = masterlist; ptr != NULL; ptr = ptr->next) {

      if(global_realtime - ptr->last_heartbeat < 0) { continue; } //Not enough time has passed since the last heartbeat
      if(ptr->HasRequested && global_realtime - ptr->TimeOfRequest < MasterHeartbeatTimeout) { continue; } //not enough time has passed to consider the last request lost.

      ptr->HasRequested = 1;
      ptr->last_heartbeat = ptr->TimeOfRequest = global_realtime;
      NET_SendPacket(NS_SERVER, sizeof("q\n"), "q\n", ptr->address);
   }
}
void Master_Shutdown(int arg_0) {

   master_server_t * ptr, * ptr2;


   if(gfNoMasterServer != 0 || masterlist == NULL || global_svs.pad00 == 0 || NET_IsConfigured() == 0) { return; }

   Master_Init();
   for(ptr = masterlist; ptr != NULL; ptr = ptr->next) {
      NET_SendPacket(NS_SERVER, sizeof("b\n"), "b\n", ptr->address);
   }

   //Well, there go our termination packets (as noted in master3, there's
   //a spoofing attackt hat can be committed against a server here, and we can
   //upgrade our solution hack once this has proliferated)

   if(arg_0 == 0) { return; }

   for(ptr = masterlist; ptr != NULL; ptr = ptr2) {
      ptr2 = ptr->next;
      Q_Free(ptr);
   }
   masterlist = NULL;
}
void Host_GetBatchModList_f() { //Odd...

   master_server_t * ptr;


   NET_Config(1);
   if(gfNoMasterServer != 0 || masterlist == NULL) { return; }

   for(ptr = masterlist; ptr != NULL; ptr = ptr->next) {

      Con_Printf("Requesting batch mod status from %s", NET_AdrToString(ptr->address));
      NET_SendPacket(NS_CLIENT, sizeof("x\r\nstart-of-list\r\n"), "x\r\nstart-of-list\r\n", ptr->address);
   }
}
void Master_Init() {

   int i, SkipSection, tempport;
   static int HaveConfigured = 0;
   char * filep;
   const char * curfilep;
   netadr_t tempadr;


   if(HaveConfigured != 0 || gfNoMasterServer != 0) { return; }

   HaveConfigured = 1;

   if(COM_CheckParmCase("-nomaster") != 0 || global_svs.allocated_client_slots <= 1) {

      Con_Printf("%s: Master server comm disabled.\n", __FUNCTION__);
      gfNoMasterServer = 0;
      return; //and don't check again, ever.
   }

   MasterHeartbeatTimeout = 15;

   i = COM_CheckParmCase("-comm");
   if(i != 0 && i+1 < global_com_argc) {

      filep = COM_LoadFileForMe(global_com_argv[i+1], NULL);
   }
   else {

      filep = COM_LoadFileForMe("valvecomm.lst", NULL);
      if(filep == NULL) {
         filep = COM_LoadFileForMe("woncomm.lst", NULL);
      }
   }

   if(filep == NULL) {
      Con_Printf("%s: Couldn't load comm file.  Disabling masters--fix it.\n", __FUNCTION__);
      return;
   }

   curfilep = filep;
   i = 0;
   while(1) {

      curfilep = COM_Parse(curfilep);
      if(global_com_token[0] == '\0') { break; }

      SkipSection = 1;
      if(Q_strcasecmp("master", global_com_token) == 0) {
         SkipSection = 0;
      }

      curfilep = COM_Parse(curfilep);
      if(Q_strcmp("{", global_com_token) != 0) { break; }

      while(1) {

         curfilep = COM_Parse(curfilep);
         if(global_com_token[0] == '\0' || Q_strcmp("}", global_com_token) == 0) { break; }
         if(SkipSection == 0 && NET_StringToAdr(global_com_token, &tempadr) == 0) { break; }

         //We just (maybe) parsed the server name, if you missed it.  Now for the port.
         curfilep = COM_Parse(curfilep);
         if(Q_strcmp(":", global_com_token) != 0) { break; }

         curfilep = COM_Parse(curfilep);
         if(global_com_token[0] == '\0') { break; }

         if(SkipSection == 0) {

            tempport = Q_atoi(global_com_token);
            if(tempport < 1 || tempport > 65535) { tempport = 27010; }
            tempadr.port = tempport;

            Con_Printf("%s: Adding master server %s.\n", __FUNCTION__, NET_AdrToString(tempadr)); //all that work to avoid needless string copying, all gone :)
            Master_AddServer(&tempadr);
            i++;
         }
      }
   }

   COM_FreeFile(filep);

   if(i == 0) {
      Con_Printf("%s: Didn't parse any masters.  Disabling master comm.\n", __FUNCTION__);
      gfNoMasterServer = 0;
   }
}
