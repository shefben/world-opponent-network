#include "SV_pktcl.h"

#include <ctype.h>
#include <limits.h>
#ifdef _MSC_VER
 #include <winsock2.h>
 #include <windows.h>
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#endif

#include "banlist.h"
#include "cmd.h"
#include "master.h"
#include "Modding.h"
#include "SV_chals.h"
#include "SV_clres.h"
#include "SV_mcast.h"
#include "SV_pkt.h"
#include "SV_rcon.h"
#include "SV_user.h"
#include "sys.h"

/*** Structs ***/
typedef struct modinfo_s {
   int UsingMod;
   char url_info[0x100];
   char url_dl[0x100];
   char hlversion[0x20];
   int version;
   int size;
   int global_svonly;
   int cldll;
} modinfo_t;

/*** GLOBALS ***/
static modinfo_t gmodinfo = { 0, "", "", "", 1, 0, 1, 0 } ;
static int g_userid = 1;


//cvars
static cvar_t cvar_sv_visiblemaxplayers = { "sv_visiblemaxplayers", "-1", FCVAR_NONE, -1.0, NULL };

/*** FUNCTIONS ***/

//local

/* DESCRIPTION: SV_FindEmptySlot
// LOCATION:
// PATH: SV_ConnectClient
//
// Goes through and looks for the first available client slot.  The actual
// function does a few other rather useless things too.
*/
int SV_FindEmptySlot(netadr_t * netadr, int * slot, client_t **cl) {

   unsigned int i;


   for(i = 0; i < global_svs.allocated_client_slots; i++) {

      //The compiler ought to do a good job of optimizing this.
      *cl = &(global_svs.clients[i]);
      if((*cl)->padding00_used == 0 &&
         (*cl)->padding04 == 0 &&
         (*cl)->padding0C_connected == 0) {

         break;
      }
   }

   if(i >= global_svs.allocated_client_slots) {
      return(0);
   }

   *slot = i;
   return(1);
}
//This looks interesting.
int SV_GetFragmentSize(client_t * cl) {

   const char * c;
   int i;

   if(cl->padding00_used == 0 || cl->padding04 == 0 ||
      cl->padding08 == 0 || cl->padding0C_connected == 0) {

      return(0x400);
   }

   c = Info_ValueForKey(cl->InfoKeyBuffer, "cl_dlmax");

   if(c[0] == '\0') {
      return(0x80);
   }

   i = Q_atoi(c);

   if(i < 0x40) {
      return(0x40);
   }

   if(i > 0x4000) { //400, 4000, what's the difference >:)
      return(0x400);
   }

   return(i);
}

//modinfo

/* DESCRIPTION: SV_GetModInfo
// LOCATION:
// PATH: SV_ConnectionlessPacket->SVC_Info
//
// Digs out mod info.  At the moment this is broken, because I don't want
// to hijack the vars and I don't have the function that assigns them values.
//
// This var originally strcpys.  It now passes pointers (since only one
// function calls it).
*/
int SV_GetModInfo(const char ** out_url_info, const char ** out_url_dl, int * out_version, int * out_size, int * out_global_svonly, int * out_cldll, const char ** out_hlversion) {

   if(gmodinfo.UsingMod != 0) {
      *out_url_info      = gmodinfo.url_info;
      *out_url_dl        = gmodinfo.url_dl;
      *out_hlversion     = gmodinfo.hlversion;
      *out_version       = gmodinfo.version;
      *out_size          = gmodinfo.size;
      *out_global_svonly = gmodinfo.global_svonly;
      *out_cldll         = gmodinfo.cldll;
      return(1);
   }
   else {
      *out_url_info      = "";
      *out_url_dl        = "";
      *out_hlversion     = "";
      *out_version       = 1;
      *out_size          = 0;
      *out_global_svonly = 1;
      *out_cldll         = 0;
      return(0);
   }
}
void SV_ResetModInfo() {

   gmodinfo.UsingMod = 0;
   gmodinfo.url_info[0] = '\0';
   gmodinfo.url_dl[0] = '\0';
   gmodinfo.hlversion[0] = '\0';
   gmodinfo.version = 1;
   gmodinfo.size = 0;
   gmodinfo.global_svonly = 1;
   gmodinfo.cldll = 0;
}
void SV_SetUsingMod() {
   gmodinfo.UsingMod = 1;
}
void SV_ResetUsingMod() {
   gmodinfo.UsingMod = 0;
}

void SV_LoadModDataFromFile(const char * liblist, const char * directory) {

   char DLLPath[PATH_MAX];

      while(1) {

         liblist = COM_Parse(liblist);
         if(global_com_token[0] == '\0') { break; }

         #ifdef _WIN32
         if(Q_strcasecmp("gamedll", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            Q_snprintf(DLLPath, sizeof(DLLPath)-1, "%s/%s", directory, global_com_token);
            DLLPath[sizeof(DLLPath)-1] = '\0';
            LoadThisDll(DLLPath);
         }
         #else
         if(Q_strcasecmp("gamedll_linux", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            Q_snprintf(DLLPath, sizeof(DLLPath)-1, "%s/%s", directory, global_com_token);
            DLLPath[sizeof(DLLPath)-1] = '\0';
            LoadThisDll(DLLPath);
         }
         #endif
         else if(Q_strcasecmp("url_dl", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            Q_strncpy(gmodinfo.url_dl, global_com_token, sizeof(gmodinfo.url_dl)-1);
            gmodinfo.url_dl[sizeof(gmodinfo.url_dl)-1] = '\0';
         }
         else if(Q_strcasecmp("url_info", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            Q_strncpy(gmodinfo.url_info, global_com_token, sizeof(gmodinfo.url_info)-1);
            gmodinfo.url_info[sizeof(gmodinfo.url_info)-1] = '\0';
         }
         else if(Q_strcasecmp("version", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            gmodinfo.version = Q_atoi(liblist);
         }
         else if(Q_strcasecmp("size", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            gmodinfo.size = Q_atoi(liblist);
         }
         else if(Q_strcasecmp("global_svonly", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            gmodinfo.global_svonly = Q_atoi(liblist);
         }
         else if(Q_strcasecmp("cldll", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            gmodinfo.cldll = Q_atoi(liblist);
         }
         /*
         else if(Q_strcasecmp("secure", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
         }
         */
         else if(Q_strcasecmp("hlversion", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            Q_strncpy(gmodinfo.hlversion, global_com_token, sizeof(gmodinfo.hlversion)-1);
            gmodinfo.hlversion[sizeof(gmodinfo.hlversion)-1] = '\0';
         }
         else if(Q_strcasecmp("fallback_dir", global_com_token) == 0) {
            liblist = COM_Parse(liblist);
            if(global_com_token[0] == '\0') { continue; }

            gmodinfo.UsingMod = 1;
            FS_AddSearchPath(liblist, "DEFAULTGAME", 1);
         }

      }


}


//SVC
int SVC_GameDllQuery(const char * message) {

   int ret;
   unsigned int response_size;
   char response_message[0x800];


   if(global_sv.active == 0) { return(0); }
   if(global_svs.allocated_client_slots < 2) { return(0); }


   response_message[0] = '\0';
   response_size = sizeof(response_message) - 4;

   ret = gEntityInterface.pfnConnectionlessPacket(&global_net_from, message, response_message+4, &response_size);

   if(response_size == 0) { return(ret); }

   if(response_size > sizeof(response_message) - 4) {

      Sys_Error("%s: MOD has specified a reply size larger than our buffer.\n"
                "Most likely MOD has BLITZED our STACK.  Bad MOD.  Bad.\n", __FUNCTION__);
   }

   *(int *)response_message = 0xFFFFFFFF;
   NET_SendPacket(NS_SERVER, response_size, response_message, global_net_from);
   return(ret);
}
/* DESCRIPTION: SVC_InfoString
// LOCATION:
// PATH: SV_ConnectionlessPacket
//
// Tells people about the server, but in a way that's different from SVC_Info.
// I wasn't as sloppy on this one.
*/
void SVC_InfoString() {

   unsigned int i;
   int active; // player count
   int spec;

   char packet[1400];
   unsigned int tempsize, packetsize;
   client_t * cl_p;


   if(global_noip != 0 || global_sv.active == 0 || global_svs.allocated_client_slots < 2) {
      return;
   }

   spec =  0;
   active = 0;
   for(i = 0, cl_p = global_svs.clients; i < global_svs.allocated_client_slots; i++, cl_p++) {

      if(cl_p->padding00_used != 0 || cl_p->padding04 != 0 || cl_p->padding0C_connected != 0) {

         if(cl_p->padding243C_proxy) { spec++; }
         active++;
      }
   }

   //Order does not matter.
   #define _InfoString_hdr "\xff\xff\xff\xffinfostringresponse\00"
   Q_memcpy(packet, _InfoString_hdr, sizeof(_InfoString_hdr));
   packetsize = sizeof(_InfoString_hdr);
   tempsize = Q_snprintf(packet + sizeof(_InfoString_hdr), sizeof(packet) - sizeof(_InfoString_hdr),
                       "\\protocol\\46\\players\\%i\\lan\\%i\\max\\%i"
                       "\\description\\%s\\hostname\\%s\\map\\%s"
                       "\\dedicated\\%i\\password\\%i\\proxytarget\\%i"
                       #ifdef _WIN32
                       "\\os\\w\\gamedir\\"
                       #else
                       "\\os\\l\\gamedir\\"
                       #endif
                       ,
                       active,
                       cvar_sv_lan.value ? 1 : 0,
                       global_svs.allocated_client_slots, //what is visiblemaxplayers?
                       gEntityInterface.pfnGetGameDescription(),
                       cvar_hostname.string,
                       global_sv.name,
                       global_cls.state == ca_dedicated ? 1 : 0,
                       ServerIsPassworded(),
                       spec);

   if(tempsize + sizeof(_InfoString_hdr) >= sizeof(packet)) {
      Con_Printf("%s: overflowed when writing (which probably means there's corrupted data)", __FUNCTION__);
      return;
   }
   packetsize += tempsize;

   //Now the tricky one
   tempsize = COM_GetGameDirSize(packet+packetsize, sizeof(packet) - packetsize);
   if(tempsize + packetsize >= sizeof(packet)) {
      Con_Printf("%s: overflowed when writing (which probably means there's corrupted data)", __FUNCTION__);
      return;
   }
   packetsize += tempsize;

   //Now the ones that are optional
   if(global_noip == 0)   {

      tempsize = Q_snprintf(packet+packetsize, sizeof(packet) - packetsize,
                       "\\address\\%s", NET_AdrToString(global_net_local_adr));
      if(tempsize + packetsize >= sizeof(packet)) {
         Con_Printf("%s: overflowed when writing (which probably means there's corrupted data)", __FUNCTION__);
         return;
      }
      packetsize += tempsize;
   }
   else {

      if(sizeof("\\address\\\x00") + packetsize >= sizeof(packet)) {
         Con_Printf("%s: overflowed when writing (which probably means there's corrupted data)", __FUNCTION__);
         return;
      }
      memcpy(packet+packetsize, "\\address\\\x00", sizeof("\\address\\\x00"));
      packetsize += sizeof("\\address\\\x00");
   }

   if(gmodinfo.UsingMod != 0) {

      tempsize = Q_snprintf(packet+packetsize, sizeof(packet) - packetsize,
                          "\\mod\\1\\modversion\\%i\\global_svonly\\%i\\cldll\\%i",
                          gmodinfo.version, gmodinfo.global_svonly, gmodinfo.cldll);
      if(tempsize + packetsize >= sizeof(packet)) {
         Con_Printf("%s: overflowed when writing (which probably means there's corrupted data)", __FUNCTION__);
         return;
      }
      packetsize += tempsize;
   }

   NET_SendPacket(NS_SERVER, packetsize, packet, global_net_from);
}
/* DESCRIPTION: SVC_Info
// LOCATION:
// PATH: (class)->_Host_Frame->SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket
//
// Tells people about the server.  I'll clean it up--you guessed it--later.
*/
void SVC_Info(qboolean details) {

   unsigned int i, j;
   char packet[1400];
   int packetsizeof;
   int lengths;
   const char * variables;

    const char * a, * b, * g;
    int c, d, e, f;
    int ret;


   if(global_sv.active == 0 || global_svs.allocated_client_slots < 2) { return; }


   *(unsigned long int*)(packet) = 0xFFFFFFFF; //the same as memcpy /xff/xff/xff/xff

   if(details) { packet[4] = 'm'; }
   else { packet[4] = 'C'; }

   if(global_noip != 0) {
      Q_memcpy(packet+5, "LOOPBACK", 8);
      packetsizeof = 13;
   }
   else {
      packetsizeof = 5;
      packetsizeof += NET_AdrToStringThread(global_net_local_adr, packet+5, 1395) + 1;
   }

   //Alas, from now on I can't hardcode the packet index...
   variables = cvar_hostname.string;
   lengths = strlen(variables) + 1;
   if(packetsizeof + lengths >= 1400) { Sys_Error("Paranoia failure: Somehow 1400 bytes for an info packet wasn't enough"); }
   memcpy(packet+packetsizeof, variables, lengths);
   packetsizeof += lengths;

   variables = global_sv.name;
   lengths = strlen(variables) + 1;
   if(packetsizeof + lengths >= 1400) { Sys_Error("Paranoia failure: Somehow 1400 bytes for an info packet wasn't enough"); }
   memcpy(packet+packetsizeof, variables, lengths);
   packetsizeof += lengths;

   //A reference to com_gamedir was made.  GetGameDir is preferred.
   lengths = COM_GetGameDirSize(packet+packetsizeof, 1399 - packetsizeof);
   if(lengths + packetsizeof >= 1400) { Sys_Error("Paranoia failure: Somehow 1400 bytes for an info packet wasn't enough"); }
   packetsizeof += lengths;

   variables = gEntityInterface.pfnGetGameDescription();
   lengths = strlen(variables) + 1;
   if(packetsizeof + lengths + 6 >= 1400) { Sys_Error("Paranoia failure: Somehow 1400 bytes for an info packet wasn't enough"); }
   memcpy(packet+packetsizeof, variables, lengths);
   packetsizeof += lengths;

   //We are counting players.
   i = 0;
   j = 0;
   while(i < global_svs.allocated_client_slots) {

      if(global_svs.clients[i].padding00_used != 0 ||
         global_svs.clients[i].padding04 != 0 ||
         global_svs.clients[i].padding0C_connected != 0) { j++; }
      i++;
   }

   packet[packetsizeof] = j;
   packetsizeof++;

   lengths = cvar_sv_visiblemaxplayers.value;
   if(lengths < 0) {
      packet[packetsizeof] = lengths;
   }
   else {
      packet[packetsizeof] = global_svs.allocated_client_slots;
   }
   packetsizeof++;

   packet[packetsizeof] = '.';
   packetsizeof++;

   if(details) {

      if(global_cls.state == ca_dedicated) { //is it a dedicated server?
         packet[packetsizeof] = 'd';
      }
      else {
         packet[packetsizeof] = 'l';
      }
      packetsizeof++;

      packet[packetsizeof] = 'l';
      packetsizeof++;

      if(ServerIsPassworded() == 0) {
         packet[packetsizeof] = 0;
      }
      else {
         packet[packetsizeof] = 1;
      }
      packetsizeof++;

      ret = SV_GetModInfo(&a, &b, &c, &d, &e, &f, &g);

      if(ret == 0) {
         packet[packetsizeof] = 0;
         packetsizeof++;
      }
      else {
         packet[packetsizeof] = 1;
         packetsizeof++;

         lengths = strlen(a) + 1;
         memcpy(packet+packetsizeof, a, lengths);
         packetsizeof += lengths;

         lengths = strlen(b) + 1;
         memcpy(packet+packetsizeof, b, lengths);
         packetsizeof += lengths;

         packet[packetsizeof] = '\0';
         packetsizeof++;

         *(unsigned long int*)(packet+packetsizeof) = c;
         packetsizeof += 4;
         *(unsigned long int*)(packet+packetsizeof) = d;
         packetsizeof += 4;

         packet[packetsizeof] = e;
         packetsizeof++;
         packet[packetsizeof] = f;
         packetsizeof++;
      }

      if(global_svs.paddingE8) {
         packet[packetsizeof] = 1;
      }
      else {
         packet[packetsizeof] = 0;
      }
      packetsizeof++;
   }

   NET_SendPacket(NS_SERVER, packetsizeof, packet, global_net_from);
}
/* DESCRIPTION: SVC_Ping
// LOCATION: The master3 :)
// PATH: (class)->_Host_Frame->SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket
//
// Replies to a ping (should be SVC_ACK then in my opinion).
*/
void SVC_Ping() {

   NET_SendPacket(NS_SERVER, sizeof("\xff\xff\xff\xff" "j"), "\xff\xff\xff\xff" "j", global_net_from);
}
/* DESCRIPTION: SVC_PlayerInfo
// PATH: SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket
//
*/
void SVC_PlayerInfo() {

   char packet[1400];
   unsigned int packetsize, tempsize;
   unsigned int count;
   unsigned int i;
   client_t * cl_p;

   if(global_sv.active == 0 || global_svs.allocated_client_slots < 2) { return; }

   //header
   *(int *)packet = 0xFFFFFFFF;
   packet[4] = 'D';
   //packet[5] is RESERVED.  We'll go back and write it later.
   packetsize = 6;

   count = 0;
   for(i = 0, cl_p = global_svs.clients; i < global_svs.allocated_client_slots; i++, cl_p++) {

      if(cl_p->padding00_used == 0) { continue; }
      count++;

      tempsize = Q_snprintf(packet+packetsize, sizeof(packet)-packetsize,
                          "%c%s", count, cl_p->PlayerName) + 1;
      if(tempsize + packetsize + 8 >= sizeof(packet)) {
         Con_Printf("%s: overflowed when writing (which probably means there's corrupted data)", __FUNCTION__);
         return;
      }

      *(int *)packet = (int)(cl_p->padding4A84->v.frags);
      *(float *)packet = global_realtime - cl_p->netchan1C.first_received;
   }

   packet[5] = count;
   NET_SendPacket(NS_SERVER, packetsize, packet, global_net_from);
}
/* DESCRIPTION: SVC_RuleInfo
// LOCATION:
// PATH: (class)->_Host_Frame->SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket
//
// Tells people about the some of the server variables.  This one has been split
// into two parts in order to properly separate the cvar globals and the
// the networking ones.
//
// A properly formatted packet goes 0xFFFFFFFF 'E' *short int varcount*
// and is followed by variable name and string, each one left null terminated.
*/
void SVC_RuleInfo() {

   char data[4096];
   unsigned int sizeofdata;

   if(global_sv.active == 0 || global_svs.allocated_client_slots < 2) { return; }

   sizeofdata = Cvar_SVCRuleInfoVarWrite(data, sizeof(data));
   if(sizeofdata == 0) { return; }

   NET_SendPacket(NS_SERVER, sizeofdata, data, global_net_from);
}


//IDs and protocol
// WON is protocol 46.  We can't accept anyone else--old WON uses
// different netcode, Steam uses a different munging
// algorithm (or table, I never really looked).  And probably
// incompaible netcode as well.  Either way, we filter them here.
int SV_CheckProtocol(netadr_t * client, int protocolvarsion) {

   if(protocolvarsion == 46) { return(1); }  //Hey, that was easy.

   return(0);
}
//Returns a pointer to a static char for most likely only printing.
char * SV_GetClientIDString(client_t * ptrClient) {

   static char ReturnString[0x40];

   ReturnString[0] = '\0';

   if(ptrClient == NULL) {
      Con_Printf("SV_GetClientIDString: Passed NULL argument\n");
      return(ReturnString);
   }

   if(ptrClient->authentication_method == 0) { //probably some old compatibility code.

      sprintf(ReturnString, "%010lu", ptrClient->authentication_WonID);
   }
   //1 == steam
   else if(ptrClient->authentication_method == 2) { //WON

      if(ptrClient->netchan1C.remote_address.type == NA_LOOPBACK) {

         Q_sprintf(ReturnString, "VALVE_ID_LOOPBACK");
      }
      else if(ptrClient->authentication_WonID == 0) {

         Q_sprintf(ReturnString, "VALVE_ID_PENDING");
      }
      else {

         //max of 16+1 letters.
         Q_sprintf(ReturnString, "VALVE_%010lu", ptrClient->authentication_WonID);
      }
   }
   else {

      Q_sprintf(ReturnString, "UNKNOWN");
   }

   return(ReturnString);
}
int SV_CheckKeyInfo(netadr_t * client, const char * data, int *port, int *authmethod, int *uniqueID, char *raw, char *cdkey) {

   int i, j, k;
   const char * hash;
   const char * ptr;

   //We only support hashed CD keys right now.  That's IDd by a 2.
   *authmethod = Q_atoi(Info_ValueForKey(data, "prot"));
   if(*authmethod != 2) {
      return(0);
   }

   //There's a 'unique' field check.  Well, unique is -1 for us...
   //If I'm right, THIS is where we can assign some 'fake' unique values
   //to make BB work again.

   hash = Info_ValueForKey(data, "raw");
   if(Q_strlen(hash) != 32) {
      return 0;
   }

   Q_memcpy(raw, hash, 33);
   Q_memcpy(cdkey, hash, 33);

   //These two terms are different when we're not using hashed keys.

   //Quick uniqueID generator, for BB
   ptr = hash;
   *uniqueID = 0;
   for(i = 0; i < 4; i++) {
      k = 0;
      for(j = 0; j < 8; j++) {

         if(isxdigit(*ptr) == 0) { return(0); } //hah, caught ya.
         if(isdigit(*ptr) != 0) { k |= (*ptr - '0') << j*4; }
         else { k |= (toupper(*ptr) - 'A') << j*4; }
         ptr++;
      }

      *uniqueID ^= k;
   }

   *port = 27005;
   return(1);
}


//highest

/* DESCRIPTION: SV_ConnectClient
// LOCATION: sv_main.c (by other names)
// PATH: (class)->_Host_Frame->SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket->SV_ConnectClient
//
// Connects the client, duh.
*/
void SV_ConnectClient() {

   unsigned int slot, IsReconnect;
   int port, UID;
   netadr_t address;
   int authmethod;
   char name[0x20]; //of questionable use.  To do: clean up, fix "" name bug, etc.
   char cert[33]; //normally 0x200, but as we don't have true certificates...
   char key[33]; //normally 0x40
   client_t * cl;


   address = global_net_from;
   port = 27005;

   if(Cmd_Argc() < 5) { SV_RejectConnection(&address, "Connection refused--incomplete"); return; }

   //Check the protocol.  I wonder how much it would take to do both 46 and 47...
   if(SV_CheckProtocol(&address, Q_atoi(Cmd_Argv(1))) == 0) {
      SV_RejectConnection(&address, "This is a WON/Won2 server, and you didn't indicate you had that client version.  Shoo.\n");
      return;
   }

   //challenge numbers.  Do NOT respond if these aren't matched.  Should be first in my opinion.
   if(SV_CheckChallenge(&address, Q_atoi(Cmd_Argv(2))) == 0) {
      SV_RejectConnection(&address, "Denied--bad challenge.\n");
      return;
   }

   //This function gets everything by address...
   if(SV_CheckKeyInfo(&address, Cmd_Argv(3), &port, &authmethod, &UID, cert, key) == 0) {
      SV_RejectConnection(&address, "Connect message failed auth check.  This server only supports raw keys.\n");
      return;
   }

   //This is new.  Check the CD key we just got against the ban list.
   if(IsCDKeyBanned(key) != 0) {

      //Should we tell them they are banned?

      if(cvar_announce_bans.value != 0) {
         SV_SendBan();
      }
      else {
         SZ_Clear(&global_net_message);
      }
      return;
   }

//testing, don't do this.
/*
   if(SV_CheckIPRestrictions(&address) == 0) {
*/
/*   if(cvar_sv_lan.value != 0 && (NET_CompareClassBAdr(address, HIJACK_NET_LOCAL_ADDRESS) == 0) {

      SV_RejectConnection(&address, "Connection refused--the dreaded class restriction");
      return;
   }
*/

   //Are we reconnecting?  And what slot did we use?
   IsReconnect = 0;
   for(cl = global_svs.clients, slot = 0; slot < global_svs.allocated_client_slots; slot++, cl++) {

      if(NET_CompareAdr(address, cl->netchan1C.remote_address) != 0) {
         IsReconnect = 1;
         break;
      }
   }

   //password check, moved out of another function.
   if(CheckPassword(Info_ValueForKey(Cmd_Argv(4), "password")) == 0) {
      SV_RejectConnection(&address, "Incorrect password.\n");
      return;
   }

   //We'd check for duplicate names here normally, but not right now.
   Q_strncpy(name, Info_ValueForKey(Cmd_Argv(4), "name"), sizeof(name)-1);
   name[sizeof(name)-1] = '\0';

   //SV_FinishCertificateCheck is where we ckeck to see if the key is already in use.

   //Now to find a slot for our client.
   if(IsReconnect) {

      if((cl->padding00_used != 0 || cl->padding04 != 0) && cl->padding4A84 != NULL) {
         gEntityInterface.pfnClientDisconnect(cl->padding4A84);
      }
   }
   else {
      if(SV_FindEmptySlot(&address, &slot, &cl) == 0) {
          SV_RejectConnection(&address, "Sorry, server is full.\n");
          return;
      }
   }

   //lots of steam crap is ignored here.  Boo, steam.  I like saying that.

   global_host_client = cl;
   cl->authentication_method = 0;
   cl->authentication_WonID = UID;

   SV_ClearResourceLists(cl);
   SV_ClearFrames(&(cl->padding3480_frames));

   cl->padding3480_frames = Q_Malloc(sizeof(client_frame_t) * SV_UPDATE_BACKUP);
   CHECK_MEMORY_MALLOC(cl->padding3480_frames);
   Q_memset(cl->padding3480_frames, 0, sizeof(client_frame_t) * SV_UPDATE_BACKUP);

   cl->resource4CA0.pNext = cl->resource4CA0.pNext = &cl->resource4CA0;
   cl->resource4C18.pNext = cl->resource4C18.pNext = &cl->resource4C18;

   cl->padding4A84 = EDICT_NUM(slot+1);

   if(g_modfuncs.Function18 != NULL) {
      g_modfuncs.Function18(slot);
   }

   Netchan_Setup(NS_SERVER, &(cl->netchan1C), address, cl - global_svs.clients, cl, SV_GetFragmentSize);

   cl->cl_updaterate = 1.0 / 20.0;
   cl->next_update_time = cl->cl_updaterate + global_realtime;
   cl->padding2434_delta_sequence = -1;

   Q_memset(&(cl->padding2440), 0, sizeof(usercmd_t));
   cl->padding249C_TimeOfNextPingCalc = -1;

   Con_Printf("%s has connected.\n", NET_AdrToString(address));

   Q_memcpy(cl->authentication_CD_key, key, sizeof(key));
   cl->padding00_used = 0;
   cl->padding04 = 0;
   cl->padding0C_connected = 1;
   cl->padding08 = 0;
   cl->padding10 = 0;

   cl->padding4A8C_PlayerUID = g_userid;
   g_userid++;

   Netchan_OutOfBandPrint(NS_SERVER, address, "B %s %i \"%s\"", SV_GetClientIDString(cl), cl->padding4A8C_PlayerUID, NET_AdrToString(address));
   Q_strncpy(cl->InfoKeyBuffer, Cmd_Argv(4), sizeof(cl->InfoKeyBuffer)-1);
   cl->InfoKeyBuffer[sizeof(cl->InfoKeyBuffer)-1] = '\0';

   SV_ExtractFromUserinfo(cl);

   cl->sizebuf24AC.overflow = 1;
   cl->sizebuf24AC.data = cl->datagram_buffer;
   cl->sizebuf24AC.maxsize = sizeof(cl->datagram_buffer);
   cl->sizebuf24AC.cursize = 0;
   cl->sizebuf24AC.debugname = cl->PlayerName;

   cl->padding4BA8 = 0;
}
HLDS_DLLEXPORT edict_t * PF_CreateFakeClient_I(const char *netname) {

   unsigned int i;
   client_t * cl;
   edict_t * ent;


   i = 0;
   cl = global_svs.clients;
   while(1) {

      if(i >= global_svs.allocated_client_slots) { return(NULL); }
      if(cl->padding00_used == 0 && cl->padding04 == 0 && cl->padding0C_connected == 0) { break; }

      i++;
      cl++;
   }

   //set it up.
   ent = EDICT_NUM(i+1);

   if(cl->padding3480_frames != NULL) {
      SV_ClearFrames(&(cl->padding3480_frames));
   }

   Q_memset(cl, 0, sizeof(client_t));
   cl->resource4CA0.pNext = cl->resource4CA0.pPrev = &cl->resource4CA0;
   cl->resource4C18.pNext = cl->resource4C18.pPrev = &cl->resource4C18;

   Q_strncpy(cl->PlayerName, netname, sizeof(cl->PlayerName)-1);
   cl->PlayerName[sizeof(cl->PlayerName)-1] = '\0';

   cl->padding00_used = 1;
   cl->padding04 = 1;
   cl->padding08 = 1;
   cl->padding0C_connected = 1;
   //cl->padding10 = 0; //Already done, memset.

   cl->padding4A84 = ent;
   cl->padding2438_IsFakeClient = 1;
   cl->padding4A8C_PlayerUID = g_userid;
   g_userid++;

   ent->v.netname = (int)(global_pr_strings + (int)cl->PlayerName);
   ent->v.pContainingEntity = ent;

   Info_SetValueForKey(cl->InfoKeyBuffer, "name", netname, 0x100);
   Info_SetValueForKey(cl->InfoKeyBuffer, "model", "gordon", 0x100);
   Info_SetValueForKey(cl->InfoKeyBuffer, "topcolor", "1", 0x100);
   Info_SetValueForKey(cl->InfoKeyBuffer, "bottomcolor", "1", 0x100);

   cl->padding4BA4 = 0;
   SV_ExtractFromUserinfo(cl);
   return(ent);
}

/* DESCRIPTION: SV_ConnectionlessPacket
// LOCATION: sv_main.c, general info is available in MANY places.
// PATH: SV_Frame->SV_ReadPackets
//
// This handles messages that don't require a reliable communication channel.
// Or in other words, they send us something, we send something back, and then
// we forget all about it.  Mods have a hook here that lets them define their
// own connectionless packets.
//
// Connectionless packets are identifiable because the first four bytes are
// 0xFFFFFFFF (instead of a sequence or something).
*/
void SV_ConnectionlessPacket() {

   char * s, * s2;
   const char * c;

   MSG_BeginReading();
   MSG_ReadLong(); //We already verified it was connectionless, so discard this.

   s = MSG_ReadStringLine();
   s2 = s;

   Cmd_TokenizeString(s);
   c = Cmd_Argv(0);

   //A call to CheckIP is made here.  CheckIP seems to exist to limit rates.
   //This is EXCEEDINGLY STUPID since UDP is TRIVIAL to spoof.  I won't bother.
   //And not to mention, if we're processing it, the packet got through...

   //We could probably speed this up with some compound case statements.
   if(Q_strcmp(c, "ping") == 0 ||
      (c[0] == 'i' && (c[1] == '\0' || c[1] == '\n'))) { //There are two pings.

      SVC_Ping();
      return;
   }
   else if(c[0] == 'l') { //A bad idea, but I'll 'fix' it later.

      MSG_BeginReading ();
      MSG_ReadLong();
      MSG_ReadByte();

      Con_Printf("%s said: %s", NET_AdrToString(global_net_from), MSG_ReadString());
      return;
   }
   else if(c[0] == 'O') { //This should run an 'isMaster' check.
      Con_Printf("Your server is 'out of date'... Dammit, why are you using ValvE's masters?\n");
      return;
   }
   else if(c[0] == 'j' && (c[1] == '\0' || c[1] == '\n')) {
      Con_Printf("You got ACKd.\n");
      return;
   }
   else if(c[0] == 's' && (c[1] == '\0' || c[1] == '\n')) { //heartbeat req
      SVC_Heartbeat();
      return;
   }
/*
   else if(c[0] == 'K' && (c[1] == '\0' || c[1] == '\n')) { //This is one I don't know anythign about.

      //Educated guess: relates to security modules.  Action: ignore all UPDATEs.
      UPDATE_ProcessMessage();
      return;
   }

   else if(Q_strcasecmp(c, "log") == 0) {
      return; //It looks like this just prints to console, provided some
      //console vars are set.  But I don't log yet, so whatever.
   }
*/
//Auth messages are next. Boring.  I'd rather make my own.
   else if(Q_strcmp(c,"getchallenge") == 0) { //That was easy.
      SVC_GetChallenge();
      return;
   }
   else if(Q_strcmp(c,"challenge") == 0) {
      SVC_ServiceChallenge();
      return;
   }
   else if(Q_strcasecmp(c, "info") == 0) {
      SVC_Info(0);
      return;
   }
   else if(Q_strcasecmp(c, "details") == 0) {
      SVC_Info(1);
      return;
   }
   else if(Q_strcasecmp(c, "infostring") == 0) {
      SVC_InfoString();
      return;
   }
   else if(Q_strcasecmp(c, "players") == 0) {
      SVC_PlayerInfo();
      return;
   }
   else if(Q_strcasecmp(c, "rules") == 0) {
      SVC_RuleInfo();
      return;
   }
   else if(Q_strcmp(c, "connect") == 0) {
      SV_ConnectClient();
      return;
   }
   else if(Q_strcmp(c, "pstat") == 0) {
      if(g_modfuncs.Function20 != NULL) {
         g_modfuncs.Function20(global_net_message.data, global_net_message.cursize);
      }
      return;
   }
   else if(Q_strcmp(c, "rcon") == 0) {
      SV_Rcon();
      return;
   }
   else {

      //Try the MOD.  This function returns 0 on failure, but I don't feel
      //like telling the server that someone sent a nonsense packet.
      if(SVC_GameDllQuery(s2) == 0) {
         Con_Printf("%s: Didnae find a match for connectionless %s.", __FUNCTION__, c);
      }
   }
}

void SV_ConnectionlessPacket_Init() {

   Cvar_RegisterVariable(&cvar_sv_visiblemaxplayers);
}
