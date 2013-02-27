#include <ctype.h>

#include "NET.h"
#include "banlist.h"
#include "cmd.h"
#include "cvar.h"
#include "Modding.h"
#include "Q_memdef.h"
#include "report.h"
#include "SV_mcast.h"
#include "sys.h"

/*** GLOBALS ***/

client_static_t global_cls;
server_static_t global_svs;
server_t global_sv;

client_t * global_host_client = NULL;

netadr_t global_net_from;
sizebuf_t global_net_message;
netadr_t global_net_local_adr;
int global_net_drop;
qboolean global_noip;


//cvars
   static cvar_t cvar_net_address    = {    "net_address",          "",    FCVAR_NONE,       0, NULL };
   static cvar_t cvar_ipname         = {             "ip", "localhost",    FCVAR_NONE,       0, NULL };
   static cvar_t cvar_iphostport     = {    "ip_hostport",         "0",    FCVAR_NONE,     0.0, NULL };
   static cvar_t cvar_hostport       = {       "hostport",         "0",    FCVAR_NONE,     0.0, NULL };
   static cvar_t cvar_defport        = {           "port",     "27015",    FCVAR_NONE, 27015.0, NULL };
   static cvar_t cvar_ip_clientport  = {  "ip_clientport",         "0",    FCVAR_NONE,     0.0, NULL };
   static cvar_t cvar_clientport     = {     "clientport",     "27005",    FCVAR_NONE, 27005.0, NULL };

   static cvar_t cvar_multicastport  = {  "multicastport",     "27025",    FCVAR_NONE, 27025.0, NULL };
   static cvar_t cvar_fakelag        = {        "fakelag",         "0",    FCVAR_NONE,     0.0, NULL };
   static cvar_t cvar_fakeloss       = {       "fakeloss",         "0",    FCVAR_NONE,     0.0, NULL };
   static cvar_t cvar_net_graph      = {      "net_graph",         "0", FCVAR_ARCHIVE,     0.0, NULL };
   static cvar_t cvar_net_graphwidth = { "net_graphwidth",       "150",    FCVAR_NONE,   150.0, NULL };
   static cvar_t cvar_net_scale      = {      "net_scale",         "5", FCVAR_ARCHIVE,     5.0, NULL };
   static cvar_t cvar_net_graphpos   = {   "net_graphpos",         "1", FCVAR_ARCHIVE,     1.0, NULL };

   cvar_t cvar_sv_lan      = {      "sv_lan",     "1", FCVAR_NONE,     1.0, NULL };
   cvar_t cvar_sv_lan_rate = { "sv_lan_rate", "20000", FCVAR_NONE, 20000.0, NULL };

/*** NET ***/



   static qboolean net_sleepforever = 1; //net only
//   static qboolean global_noip = 0; //global, stupidly
   static qboolean net_configured = 0;

//   static sizebuf_t global_net_message; //global
   static BYTE global_net_message_buffer[0x10000]; //global but assigned in net
//   static sizebuf_t in_message; //here only
//   static BYTE in_message_buffer[0x10000];

//   static netadr_t global_net_from = { NA_UNUSED, { 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 }; //basically all zeros
//   static netadr_t in_from  = { NA_UNUSED, { 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 };
//   static netadr_t global_net_local_adr = { NA_UNUSED, { 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0 }; //global

//   static global_net_messages_t * normalqueue = NULL; //net only
   static SOCKET ip_sockets[3] = { INVALID_SOCKET, INVALID_SOCKET, INVALID_SOCKET }; //client, server, and unknown.  NET only.
   static loopback_t loopbacks[2]; //net only

void NET_Config(qboolean);

void NET_TransferRawData(sizebuf_t *, unsigned char *, unsigned int);
void NET_FreeMsg(global_net_messages_t *);

qboolean NET_GetPacket(netsrc_t);
qboolean NET_GetLoopPacket(netsrc_t, netadr_t *, sizebuf_t *);
qboolean NET_QueuePacket(netsrc_t);
qboolean NET_GetLong();

void NET_SendPacket(netsrc_t, int, byte *, netadr_t);
void NET_SendLoopPacket(netsrc_t, unsigned int, void *);
int  NET_SendLong(SOCKET, void *, int, int, struct sockaddr *, int);


//*** Actual functions ***//

//Dummy SDK insurance:
void NET_AddToLagged() { printf("%s called", __FUNCTION__); exit(99); }
void NET_AdjustLag() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_AdrToString() { printf("%s called", __FUNCTION__); exit(99); }
void NET_AllocMsg() { printf("%s called", __FUNCTION__); exit(99); }
void NET_AllocateQueues() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_BaseAdrToString() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_ClearLagData() { printf("%s called", __FUNCTION__); exit(99); }
void NET_ClearLaggedList() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_CompareAdr() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_CompareBaseAdr() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_CompareClassBAdr() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_Config() { printf("%s called", __FUNCTION__); exit(99); }
void NET_CountLaggedList() { printf("%s called", __FUNCTION__); exit(99); }
void NET_ErrorString() { printf("%s called", __FUNCTION__); exit(99); }
void NET_FlushQueues() { printf("%s called", __FUNCTION__); exit(99); }
void NET_FlushSocket() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_FreeMsg() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_GetLocalAddress() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_GetLong() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_GetLoopPacket() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_GetPacket() { printf("%s called", __FUNCTION__); exit(99); }
void NET_HostToNetShort() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_IPSocket() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_Init() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_IsConfigured() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_IsLocalAddress() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_IsReservedAdr() { printf("%s called", __FUNCTION__); exit(99); }
void NET_JoinGroup() { printf("%s called", __FUNCTION__); exit(99); }
void NET_LagPacket() { printf("%s called", __FUNCTION__); exit(99); }
void NET_LeaveGroup() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_OpenIP() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_QueuePacket() { printf("%s called", __FUNCTION__); exit(99); }
void NET_RemoveFromPacketList() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_SendLong() { printf("%s called", __FUNCTION__); exit(99); }
////void NET_SendLoopPacket() { printf("%s called", __FUNCTION__); exit(99); }
////void NET_SendPacket() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_Shutdown() { printf("%s called", __FUNCTION__); exit(99); }
void NET_Sleep() { printf("%s called", __FUNCTION__); exit(99); }
void NET_Sleep_Timeout() { printf("%s called", __FUNCTION__); exit(99); }
void NET_StartThread() { printf("%s called", __FUNCTION__); exit(99); }
void NET_StopThread() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_StringToAdr() { printf("%s called", __FUNCTION__); exit(99); }
//void NET_StringToSockaddr() { printf("%s called", __FUNCTION__); exit(99); }
void NET_ThreadLock() { printf("%s called", __FUNCTION__); exit(99); }
void NET_ThreadUnlock() { printf("%s called", __FUNCTION__); exit(99); }
////void NET_TransferRawData() { printf("%s called", __FUNCTION__); exit(99); }



/* DESCRIPTION: NetAdr/Sockaddr
// LOCATION: net_wins.c
// PATH: Net_SendPacket
//
// Converting between the netadr struct and a regular sockaddr isn't as easy
// as re-casting a pointer this time around.  Different vars are re-assigned,
// somewhat clumsily.  But it ain't broke, so...
*/
void NetadrToSockadr(netadr_t *a, struct sockaddr *s) {

   switch(a->type) {

   case NA_BROADCAST:
      Q_memset(s, 0, sizeof(struct sockaddr_in));
      ((struct sockaddr_in*)s)->sin_family = AF_INET;

      *(int *)&((struct sockaddr_in*)s)->sin_addr = INADDR_BROADCAST;
      ((struct sockaddr_in*)s)->sin_port = a->port;
      break;

   case NA_IP:
      memset(s, 0, sizeof(struct sockaddr_in));
      ((struct sockaddr_in*)s)->sin_family = AF_INET;

      *(int *)&((struct sockaddr_in*)s)->sin_addr = *(int *)&a->ip;
      ((struct sockaddr_in*)s)->sin_port = a->port;
      break;

   default:
      Sys_Error("NetadrToSockadr: Bad type - needs fixin'");
   }
}
void SockadrToNetadr(struct sockaddr *s, netadr_t *a) {

   switch(((struct sockaddr*)s)->sa_family)   {

   case AF_INET:
      a->type = NA_IP;
      *(int *)&a->ip = ((struct sockaddr_in *)s)->sin_addr.s_addr;
      a->port = ((struct sockaddr_in *)s)->sin_port;
      break;

   case AF_UNSPEC:
      Q_memset(a, 0, sizeof(*a));
      a->type = NA_UNUSED;
      break;

   default:
      Sys_Error("SockadrToNetadr: bad socket family");
   }
}

/* DESCRIPTION: NetAdr/StringTo
// LOCATION: net_wins.c
// PATH: enough
//
// String to numbers, or the other way around.  Taken from QW.
// The complete netadr struct is pushed onto the stack.
*/
char *NET_AdrToString(netadr_t a) {

   static char s[64]; //to do: un-staticky this

   switch(a.type)   {

   case NA_BROADCAST:
   case NA_IP:
      sprintf(s, "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
      break;
   case NA_LOOPBACK:
      sprintf(s, "loopback");
      break;
   default:
      sprintf(s, "invalid netadr_t type");
   }

   return(s);
}
char *NET_BaseAdrToString(netadr_t a) {
   static char s[64];

   switch(a.type) {

   case NA_BROADCAST:
   case NA_IP:
      sprintf(s, "%i.%i.%i.%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
      break;
   case NA_LOOPBACK:
      sprintf(s, "loopback");
      break;
   default:
      Sys_Error("NET_BaseAdrToString: Bad netadr_t type");
   }

   return s;
}
qboolean NET_StringToSockaddr(const char *s, struct sockaddr *sadr) {

   //This one is more complicated, but I can only tinker with IP4 right now;
   //I don't know much about IPX or IP6

   struct hostent *h;
   char *colon;
   char copy[128];
   SOCKET i;

   if (!(*s)) { return(0); } //empty string

   Q_memset(sadr, 0, sizeof(struct sockaddr));

   if(strlen(s) >= sizeof(copy)-1) { return(0); } //general size protection.

   ((struct sockaddr_in *)sadr)->sin_family = AF_INET;
   ((struct sockaddr_in *)sadr)->sin_port = 0;

   strcpy(copy, s);

   // strip off a trailing :port if present
   colon = strrchr(copy, ':');
   if(colon != NULL) {
      *colon = '\0';
      ((struct sockaddr_in *)sadr)->sin_port = htons((short)atoi(colon+1));
   }

   //Now we need to test and see if it's an IP or a string.
   i = inet_addr(copy);

   if(i != INADDR_NONE) {
      //Hey, it WAS an IP.  inet_addr returns -1 on failure, which is both INADDR_NONE and INADDR_BROADCAST in winsock and I assume others.
      *(int *)&((struct sockaddr_in *)sadr)->sin_addr = i;
      return(1);
   }

   //Let's try the PITA gethost functions then...
   h = gethostbyname(copy);
   if(h == NULL || h->h_addrtype != AF_INET) { return(0); }

      memcpy(&(((struct sockaddr_in *)sadr)->sin_addr), h->h_addr, h->h_length); //Personal choice
      //*(int *)&((struct sockaddr_in *)sadr)->sin_addr = *(int *)h->h_addr_list[0];
   return(1);
}
qboolean NET_StringToAdr(const char *s, netadr_t *a) {

   struct sockaddr sadr;

   if(Q_strcmp(s, "internalserver") == 0 || Q_strcmp(s, "localhost") == 0 || Q_strcmp(s, "loopback") == 0) { //one has to be correct...

      Q_memset(a, 0, sizeof(netadr_t));
      a->type = NA_LOOPBACK;
      return(1);
   }

   if(NET_StringToSockaddr(s, &sadr)) {
      SockadrToNetadr(&sadr, a);
      return(1);
   }
   return(0);
}

/* My version takes an array passed to it, and returns the total number of
// bytes written.  The return value therefore can't exceed bufsize.
// Return value does NOT follow standard nprintf rules.
*/
int NET_AdrToStringThread(netadr_t a, char *buf, int bufsize) {

   int i;

   if(bufsize < 1 || buf == NULL) { Sys_Error("%s: passed invalid arguments.  Fix it.", __FUNCTION__); }

   switch(a.type) {

   case NA_BROADCAST:
   case NA_IP:
      i = Q_snprintf(buf, bufsize, "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], ntohs(a.port));
      break;
   case NA_LOOPBACK:
      i = Q_snprintf(buf, bufsize, "loopback");
      break;
   default:
      i = Q_snprintf(buf, bufsize, "invalid netadr_t type");
   }

   //annoying traits of snprintf: Won't put terminating null if string is too big,
   //also doesn't return the number of bytes WRITTEN but the number that WOULD'VE
   //been written had the buffer been large enough.
   if(i >= bufsize) { i = bufsize; buf[bufsize-1] = '\0'; }

   return(i);
}


void NET_ClearLagData() { } //called by host, we wish it gone.

void MaxPlayers_f() {

   int i;

   if(Cmd_Argc() != 2) { //+8
      Con_Printf("maxplayers is %u\n", global_svs.allocated_client_slots);
      return;
   }
   if(global_sv.active) {
      Con_Printf("You can't change 'maxplayers' while the server is running.\n");
      return;
   }

   i = Q_atoi(Cmd_Argv(1));

   if(i < 1 || (unsigned int)i > global_svs.total_client_slots) { //HL normally just sets i to the limit; I prefer explicitness
      Con_Printf("Can't set 'maxplayers' to a number less than 1 or greater than %u\n", global_svs.total_client_slots);
      return;
   }

   global_svs.allocated_client_slots = i;
   //Poss hack:  There's an ordering issue.  We assign the mod's copy
   //of maxplayers before we lock out changes to it.  Is re-assgning
   //that global var enough, or should we ensure that the commandline
   //"+maxplayers #" is invalid? (normally it's "-maxplayers #")
   gGlobalVariables.maxClients = global_svs.allocated_client_slots;
   Con_Printf("'maxplayers' has been changed to %u\n", i);

   if(i == 1) { //I think this is related to Singleplayer/multiplayer
      Cvar_DirectSetValue(&cvar_deathmatch, 0);
   }
   else {
      Cvar_DirectSetValue(&cvar_deathmatch, 1);
   }
}


void NET_Init() {

   const char * temp;
   #ifdef _WIN32
    WSADATA wsa;
   #endif
   Cmd_AddCommand("maxplayers", MaxPlayers_f);

   Cvar_RegisterVariable(&cvar_net_address);
   Cvar_RegisterVariable(&cvar_ipname);
   Cvar_RegisterVariable(&cvar_iphostport);
   Cvar_RegisterVariable(&cvar_hostport);
   Cvar_RegisterVariable(&cvar_defport);
   Cvar_RegisterVariable(&cvar_ip_clientport);
   Cvar_RegisterVariable(&cvar_clientport);
   Cvar_RegisterVariable(&cvar_multicastport);
   Cvar_RegisterVariable(&cvar_fakelag);
   Cvar_RegisterVariable(&cvar_fakeloss);
   Cvar_RegisterVariable(&cvar_net_graph);
   Cvar_RegisterVariable(&cvar_net_graphwidth);
   Cvar_RegisterVariable(&cvar_net_scale);
   Cvar_RegisterVariable(&cvar_net_graphpos);

   Cvar_RegisterVariable(&cvar_sv_lan);
   Cvar_RegisterVariable(&cvar_sv_lan_rate);


   if(COM_CheckParmCase("-netsleep")) { net_sleepforever = 0; }
   if(COM_CheckParmCase("-global_noip")) { global_noip = 1; }

   temp = COM_GetParmAfterParmCase("-port");
   if(temp[0] != '\0') {
      Cvar_DirectSet(&cvar_hostport, temp);
   }
   temp = COM_GetParmAfterParmCase("-clockwindow");
   if(temp[0] != '\0') {
      Cvar_DirectSet(&cvar_clockwindow, temp);
   }

   //These two may not be sizebufs since the first parameter is NOT a boolean,
   //but a string with their name.  Seem to match in other regards.
   global_net_message.cursize = 0;
   global_net_message.maxsize = sizeof(global_net_message_buffer); //0x10000
   global_net_message.data = global_net_message_buffer;
   global_net_message.debugname = NULL;
   global_net_message.overflow = 0;

//   in_message.cursize = 0;
//   in_message.maxsize = sizeof(in_message_buffer); //0x10000
//   in_message.data = in_message_buffer;
//   in_message.allowoverflow = 0;
//   in_message.overflowed = 0;

   //something related to g_pLagData is ignored here because fakelag sucks.

   //NET_AllocateQueues();

#ifdef _WIN32
   if((WSAStartup(MAKEWORD(1, 1), &wsa)) != 0) {
      Sys_Error("%s: Call to WSAStartup failed.  %s\n", __FUNCTION__, WSAGetLastError());
   }
#endif
}

void NET_Shutdown() {

#ifdef _WIN32
   WSACleanup();
#endif
}

/* DESCRIPTION: NET_GetLocalAddress
// LOCATION: net_wins.c
// PATH:
//
// Configures global_net_local_adr with various net functions.
*/
void NET_GetLocalAddress() {

   char buf[256];
   struct sockaddr_in address;
   int len;
   static qboolean called = 0;

   if(called) { return; }

   called++;

   //Our static var is set this by default, but just in case...
   Q_memset(&global_net_local_adr, 0, sizeof(netadr_t));

   if(global_noip) { Con_Printf("TCP/IP Disabled.\n"); return; }
   else {

      //localhost is a default.  It can be changed on the commandline; if
      //it isn't, we ask the OS what name to use (which will probably be localhost)
      if(Q_strcmp(cvar_ipname.string, "localhost") == 0) {

         gethostname(buf, 255); //gethostname is actually limited to 255 bytes.
      }
      else {
         Q_strncpy(buf, cvar_ipname.string, 255);
      }
      buf[255] = '\0'; //It is undefined whether gethostname adds a null if it is forcedto truncate.


      NET_StringToAdr(buf, &global_net_local_adr);

      len = sizeof(address);
      if(getsockname(ip_sockets[NS_SERVER], (struct sockaddr *)&address, (socklen_t *)&len) != 0) {
         global_noip = 1;
         Con_Printf("NET_GetLocalAddress: Failed miserably.  Reason: %s\n", strerror(errno));
      }
      else {
         global_net_local_adr.port = address.sin_port;
         Con_Printf("Server IP address is %s\n", NET_AdrToString(global_net_local_adr) );

         Cvar_DirectSet(&cvar_net_address, NET_AdrToString(global_net_local_adr));
      }
   }
}
qboolean NET_IsLocalAddress(netadr_t address) {

   if(address.type == NA_LOOPBACK) { return(1); }
   return(0);
}


/* DESCRIPTION: NET_IPSocket
// LOCATION: net_wins.c (as TCP, UDP, other names basically)
// PATH: NET_OpenIP
//
// Actually binds our sockets.  ONLY seems to handle IPv4.
*/
int NET_IPSocket(const char * ip, int port) { //I ignore the third boolean arg.

   SOCKET newsocket;
   struct sockaddr_in address;
   unsigned long _true; //Nice name

   newsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

   if(newsocket != INVALID_SOCKET) {
      //So far, so good.  HL likes its sockets to be non-blocking; we oblige for now.

#ifdef _WIN32
      _true = 1;
      if(ioctlsocket(newsocket, FIONBIO, &_true) != SOCKET_ERROR)
#else
      if(fcntl(newsocket, F_SETFL, O_NONBLOCK) != -1)
#endif
      {
         //moving right along, now we--and I don't know why--set the socket to broadcast...
         _true = 1;
         if(setsockopt(newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&_true, sizeof(_true)) != SOCKET_ERROR) {
            //Now we MIGHT end up setsocking again here.
            _true = 1;
            if(COM_CheckParmCase("-reuse") == 0 || setsockopt(newsocket, SOL_SOCKET, SO_REUSEADDR, (char *)&_true, sizeof(_true)) != SOCKET_ERROR) {
               _true = 1;
               if(COM_CheckParmCase("-tos") == 0 || setsockopt(newsocket, IPPROTO_IP, IP_TOS, (char *)&_true, sizeof(_true)) != SOCKET_ERROR) {
                  //Now we're nearing our end.
                  address.sin_addr.s_addr = INADDR_ANY;
                  if(ip == NULL || ip[0] == '\0' || Q_strcasecmp(ip, "localhost") == 0 ||
                     NET_StringToSockaddr(ip, (struct sockaddr *)&address) != 0) {
                     //Sonny!  Groovy!  Wake up.  Mystery time.

                     if(port == -1) { address.sin_port = 0; }
                     else { address.sin_port = htons((short)(port)); }
                     address.sin_family = AF_INET;

                     if(bind(newsocket, (struct sockaddr *)&address, sizeof(address)) != SOCKET_ERROR) {
                        //We're in the clear now.
                        if(COM_CheckParmCase("-loopback")) { _true = 1; }
                        else { _true = 0; }

                        if(setsockopt(newsocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&_true, sizeof(_true)) == SOCKET_ERROR) {
                           Con_Printf("NET_IPSocket's last call to setsockopt (on loopbacks) failed, but we're bound so we're going for it anyway.\n");
                        }
                        return(newsocket);
//The error printf branches
                     }
                     else {
                        Con_Printf("NET_IPSocket failed trying to bind %s:%u.\n", ip, port);
                     }
                  }
                  else {
                     Con_Printf("NET_IPSocket failed trying to convert %s into a usable address.\n", ip);
                  }
               }
               else {
                  Con_Printf("NET_IPSocket failed when trying to set IP_TOS with setsockopt().\n");
               }
            }
            else {
                Con_Printf("NET_IPSocket failed when trying to set the reuseaddr flag with setsockopt().\n");
            }
         }
         else {
            Con_Printf("NET_IPSocket failed when trying to set the broadcast flag with setsockopt().\n");
         }
      }
      else {
         Con_Printf("NET_IPSocket failed on a call to ioctlsocket().\n");
      }
      //This branch will be taken by all failures once the socket is present.
      SOCKETCLOSE(newsocket);
   }
   else {
      Con_Printf("NET_IPSocket failed on a call to socket().\n");
   }

   //General error block
   Con_Printf("Failure reason: %s\n", strerror(errno));
   return(INVALID_SOCKET);
}


/* DESCRIPTION: NET_OpenIP
// LOCATION:
// PATH: NET_Config
//
// Opens up our socket, sort've, by calling the meaty IPSocket function.
*/
void NET_OpenIP() {

   int port;

   //First set up the SERVER port.
   if(ip_sockets[NS_SERVER] == INVALID_SOCKET) {

      //Our three port cvars follow some precedence order.  The default is of course 27015.

      port = cvar_iphostport.value;
      if(port == 0) {
         port = cvar_hostport.value;
         if(port == 0) {
            port = cvar_defport.value;
         }
      }

      ip_sockets[NS_SERVER] = NET_IPSocket(cvar_ipname.string, port);

      if(ip_sockets[NS_SERVER] == INVALID_SOCKET && global_cls.state == ca_dedicated) {
         Sys_Error ("NET_OpenIP: Call to NET_IPSocket failed on this, a dedicated server.\n");
      }
   }

   //Next we set up or CLIENT port.  But wait!  We're linux.  We never have to
   //deal with that, so screw it.  Let it lie for now.
}

void NET_Config(qboolean config) { //0 = shutdown, 1 = startup.  Or so.

   int i;
   static int old_config_150 = -1;

   if(old_config_150 == config) { return; }

   old_config_150 = config;


   if(config == 0) {

      for(i = 0; i < 3; i++) {
         if(ip_sockets[i] != INVALID_SOCKET) {

            SOCKETCLOSE(ip_sockets[i]); //#defiend as close or closesocket as needed.
            ip_sockets[i] = INVALID_SOCKET;
         }
      }
   }
   else {

      if(global_noip == 0) {
         NET_OpenIP();
      }

      NET_GetLocalAddress(); //This is only called ONCE EVER.
   }

   net_configured = config;
}

/* DESCRIPTION: NET_IsConfigured
// LOCATION:
// PATH: Master_Shutdown
//
// Kindly returns a 'global' variable.  This and NET_Config are the only two
// functions to touch this boolean.
*/
qboolean NET_IsConfigured() { return(net_configured); }




/* DESCRIPTION: NET_CompareAdr
// LOCATION: net_wins.c
// PATH: here and there
//
// Compares addresses and ports to see if they're the same.
// Usually that means comparing two IPs, bt not always.
*/
qboolean NET_CompareAdr(netadr_t a, netadr_t b) {

   if(a.type != b.type) { return(0); }

   else if(a.type == NA_LOOPBACK) { return(1); }

   else if(a.type == NA_IP || NA_BROADCAST) {
      if((memcmp(a.ip, b.ip, sizeof(a.ip)) == 0) && (a.port == b.port)) { return(1); }
      else { return(0); }
   }

   else if(a.type == NA_IPX || NA_IPX_BROADCAST) {
      if((memcmp(a.ipx, b.ipx, sizeof(a.ipx)) == 0) && (a.port == b.port)) { return(1); }
      else { return(0); }
   }

   else {
      return(0);
   }
}
/* DESCRIPTION: NET_CompareBaseAdr
// LOCATION: net_wins.c
// PATH: here and there
//
// Compares addresses to see if they're the same.
// Usually that means comparing two IPs, bt not always.
*/
qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b) {

   if(a.type != b.type) { return(0); }

   else if(a.type == NA_LOOPBACK) { return(1); }

   else if(a.type == NA_IP || NA_BROADCAST) {
      if(memcmp(a.ip, b.ip, sizeof(a.ip)) == 0) { return(1); }
      else { return(0); }
   }

   else if(a.type == NA_IPX || NA_IPX_BROADCAST) {
      if(memcmp(a.ipx, b.ipx, sizeof(a.ipx)) == 0) { return(1); }
      else { return(0); }
   }

   else {
      return(0);
   }
   //Eh, explicitly stating the 'else' seems to make a difference in optis.
}
/* DESCRIPTION: NET_CompareClassBAdr
// LOCATION:
// PATH: (class)->_Host_Frame->SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket->SV_ConnectClient->SV_CheckIPRestrictions->NET_CompareClassBAdr
//
// Compares IPs to see if they're part of the same class.  Assumes class B scope, or it did at first.
*/
qboolean NET_CompareClassBAdr(netadr_t adra, netadr_t adrb) {

   if (adra.type == adrb.type && (adra.type == NA_LOOPBACK ||
      (adra.ip[0] == adrb.ip[0] && (adra.ip[0] < 128 || //class a
      (adra.ip[1] == adrb.ip[1] && (adra.ip[0] < 192 || //class b
      (adra.ip[2] == adrb.ip[2]))))))) {
         return(1);
      }
   return(0);

}

/* DESCRIPTION: NET_IsReservedAdr
// LOCATION:
// PATH: (class)->_Host_Frame->SV_Frame->SV_ReadPackets->SV_ConnectionlessPacket->SV_ConnectClient->SV_CheckIPRestrictions->NET_IsReservedAdr
//
// Takes a sockaddr as an argument, which technically adds up to several args (well, their stupid net_adr struct arguments), returns a boolean.  Returns 0 if the address is routable.
*/
qboolean NET_IsReservedAdr(netadr_t address) {

#ifdef ENGINE_ALLOW_SV_LAN_HACK
   return(1);
#endif
#ifndef ENGINE_ALLOW_SV_LAN_HACK

   if (address.type == NA_LOOPBACK) { return(1); }
   if (address.type == NA_IP &&
      ((address.ip[0] == 10) ||
      (address.ip[0] == 127) ||
      ((address.ip[0] == 172) && (address.ip[1] > 15) && (address.ip[1] < 32)) ||
      ((address.ip[0] == 192) && (address.ip[1] == 168)))) { return(1); }
   return(0);

#endif
}


/* DESCRIPTION: NET_TransferRawData
// LOCATION:
// PATH: Packet functions->NET_TransferRawData
//
// Wrapper for memcpy that also happens to assign one argument to another.
*/
void NET_TransferRawData(sizebuf_t *message, unsigned char *destination, unsigned int size) {

   if(message->maxsize < size) {
      Sys_Error("NET_TransferRawData: Tried to copy more data than would fit in sizebuf");
   }
   Q_memcpy(message->data, destination, size);
   message->cursize = size;
}
//These are all greatly modified to ignore fakelag and demo playback.
//This makes them a lot simpler, but basically the goal of the GET functions is
//to copy a working packet into global_net_message if possible.

/* DESCRIPTION: NET_GetPacket
// LOCATION:
// PATH: Three packet things->NET_GetPacket
//
// Receives a packet.  May also apply fakelag or other crazy pointless
// things that no legit server ever need do.  Since we're running on
// Linux we know there's no threading and we're ignoring fakelag.
//
// This moves the packet over to a global set of variables, global_net_message and
// global_net_from.  It returns 1 if it did so and 0 if not.
*/
qboolean NET_GetPacket(netsrc_t netsrc) {

   //We check LoopPacket, then QueuePacket, then a messages array.  The messages array
   //is both mysterious and only referred to in the UNIX binary in this function and one
   //shutdown function.  I think it's safe to ignore them.

   if(NET_GetLoopPacket(netsrc, &global_net_from, &global_net_message)) {
      //loop packets are dumped right into global_net_from this way; we're good to go.
      return(1);
   }
   if(NET_QueuePacket(netsrc)) {
      //same thing applies
      return(1);
   }

   return(0);
}
/* DESCRIPTION: NET_GetLoopPacket
// LOCATION: net_wins.c
// PATH: Net_GetPacket->NET_GetLoopPacket
//
// For a linux server this does nothing.  It exists to let client and server
// communicate without dealing with those pesky networks.
*/
qboolean NET_GetLoopPacket(netsrc_t sock, netadr_t *from, sizebuf_t *message) {

   int i;
   loopback_t *loop;

   loop = &loopbacks[sock];

   if(loop->send - loop->get > MAX_LOOPBACK) {
      loop->get = loop->send - MAX_LOOPBACK;
   }
   if(loop->get >= loop->send) {
      return(0); //empty buffer.
   }

   i = loop->get & (MAX_LOOPBACK-1);
   loop->get++;

   if(message->maxsize < loop->msgs[i].datalen) {
      Sys_Error("NET_SendLoopPacket: Loopback buffer was too big");
   }

//   NET_TransferRawData was here.  It's been replaced directly with the memcpy
// and size allocation below.

   Q_memcpy(message->data, loop->msgs[i].data, loop->msgs[i].datalen);
   message->cursize = loop->msgs[i].datalen;

   memset(from, 0, sizeof(*from));
   from->type = NA_LOOPBACK;

//forgot; HL doesn't have these yet
//   message->packing = SZ_RAWBYTES;
//   message->currentbit = 0;

   return(1);
}
/* DESCRIPTION: NET_QueuePacket
// LOCATION:
// PATH: NET_GetPacket->NET_QueuePacket
//
// Our actual recv is done here.  If successful, any packet is placed in
// in_message and in_from, with the ultimate goal that the packet is placed in
// global_net_message and global_net_from by NET_GetPacket (so that we can do our fakelag
// and whatnot).  For now we skip the middleman.
*/
qboolean NET_QueuePacket(netsrc_t netsrc) {

   int retval;
   struct sockaddr from;
   static int sizeof_from = sizeof(struct sockaddr);
   SOCKET socket;

   socket = ip_sockets[netsrc];
   if(socket == INVALID_SOCKET) { return(0); }
   global_net_message.cursize = 0;

   do {
      retval = recvfrom(socket, (char *)(global_net_message.data), MAX_UDP_PACKET_DATA_SIZE, 0, &from, &sizeof_from);

      if(retval > 0) {
         if(retval > MAX_UDP_ROUTABLE_SIZE) { //Shirley you can't be serious?
            Con_Printf("Packet received that's larger than the maximum size UDP reliably allows (%i).  Rejected.\n", retval);
            continue;
         }
         else {

            //You know, until we have a lot of types, I'm only going to check
            //IPv4 for bans, and I'm going to do it BEFORE the netadr conversion.
            //So it's sloppy.  It'll need an overhaul later anyway.


            if(IsSockaddrBanned((struct sockaddr_in *)&from) != 0) {
               if(cvar_announce_bans.value != 0) {
                  sendto(socket, "\xff\xff\xff\xff" "lYou are banned from this server.\n",
                          sizeof("\xff\xff\xff\xff" "lYou are banned from this server.\n"), 0, &from, sizeof_from);
               }
               continue;
            }

            SockadrToNetadr(&from, &global_net_from);
            global_net_message.cursize = retval;

            //Split packets beginning with 0xFFFFFFFE (well, in little endian)
            if( *(unsigned int *)global_net_message.data == hosttole32(0xFFFFFFFE) ) {
               if(NET_GetLong() == 0) {
                  continue;
               }
            }
            return(1);
         }
      }
      else {

         #ifdef _WIN32
          retval = WSAGetLastError();
         #else
          retval = errno;
         #endif

         if(retval == ENETRESET) { continue; }
         if(retval != EWOULDBLOCK) { //Some errors are to be ignored
            Con_Printf("%s: %s\n", __FUNCTION__, strerror(retval));
         }
         return(0);
      }
   } while(1);  //Did you know that while(0) doesn't respect continue?  Too bad.
}
/* DESCRIPTION: NET_GetLong
// LOCATION:
// PATH: NET_GetPacket->NET_QueuePacket
//
// If the received packet identifies itself as a split message, this function
// takes and assembles it.
// Horribly broken on the HLDS, I basically rewrote this based on what I
// thought it should do instead of what it did do.
//
// Plans are to further develop this and integrate the object with the client
// structure, but first things first, this baby.  Returns 1 if a split
// packet was fully assembled, 0 otherwise.
//
// Split packets are handled strictly.  There are no offsets given to us beyond
// the ordering of the packets.  For that reason if we get out of order packets,
// we won't know where to put them unless we enforce strict size rules or
// refuse to handle packets out of order.  I think that all split packet
// segments (apart from the last obviously) must be the same length, and that
// length is 1400-overhead=1391.  But I do not enforce a strict value; I assume
// 1391 by default and if the non-last packet is a different size, I go with
// that.  If two packets have a different length and neither is the last,
// the assembled packet is aborted.
//
// I realize this makes no sense.  The code makes no sense to me either
// (and yes, I was completely sober at the time).  And it's not really
// easy to test or anything... Bottom line, it's at least four revisions from
// being workable.
*/
qboolean NET_GetLong() {

   //temporary until we can control the client
   static splitpacket_t SplitPacket[16] = {
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" },
   { { NA_UNUSED, "", "", 0 }, 0, 0, 0, 0, 0, "" }
   };
   static int OldestSplitPacket = 0;

   unsigned int PacketNumber, PacketTotal, SequenceNumber;
   int temp;
   qboolean LastPacket = 0;
   splitpacket_t * CurrentSplitPacket = NULL;


   Con_Printf("Split packet received\n");

   if(global_net_message.cursize < SIZEOF_splitpacket_header_t) {
      return(0);
      Con_Printf("Split packet rejected: less than 9 bytes.\n");
   } //bye-bye hl-boom



   SequenceNumber = letohost32( (*(splitpacket_header_t *)(global_net_message.data)).SequenceNumber);
   PacketNumber = ((*(splitpacket_header_t *)(global_net_message.data))._4_packetnum4_packettotal >>4 );
   PacketTotal = ((*(splitpacket_header_t *)(global_net_message.data))._4_packetnum4_packettotal & 0xF0 );

   Con_Printf("Split packet size is %i, Seq number %i, part %i or %i.\n", global_net_message.cursize, SequenceNumber, PacketNumber+1, PacketTotal);

   //some sanity checking, all input is evil afterall
   //Remember, PacketNumber starts with '0', so we use a >= instead of just >.
   if(PacketNumber >= PacketTotal || PacketTotal > MAX_NUMBER_OF_SPLIT_PACKETS) {

      Con_Printf("Split packet rejected: bad sequence order.\n");
      return(0);
   }

   //It's handy to know if this is the last packet since the last packet probably
   //won't be as large as the others.

   if(PacketNumber+1 == PacketTotal) { LastPacket = 1; }


   //Now we need to find the split packet structure this belonged to, or use an old
   //one if this is new.

   for(temp = 0; temp < 16; temp++) {
      if((NET_CompareAdr(SplitPacket[temp].SendingHost, global_net_from)) &&
         (SplitPacket[temp].SequenceNumber == SequenceNumber)) {
         //Perfect match.
         CurrentSplitPacket = &(SplitPacket[temp]);
      }
   }

   if(CurrentSplitPacket == NULL) { //no matches.  Assign a new packet.

      CurrentSplitPacket = &SplitPacket[OldestSplitPacket];
      OldestSplitPacket++;
      OldestSplitPacket &= 16;

      CurrentSplitPacket->SendingHost = global_net_from;
      CurrentSplitPacket->SequenceNumber = SequenceNumber;
      CurrentSplitPacket->TotalChunks = PacketTotal;
      CurrentSplitPacket->ReceivedChunks = 0; //We'll inc it later
      CurrentSplitPacket->CurrentSize = 0;

      if(!LastPacket) {
         CurrentSplitPacket->offset = global_net_message.cursize - SIZEOF_splitpacket_header_t;
      }
      else {

         //In this highly unlikely to be legit scenario, we received a split
         //packet's end before any other part.  See above function for logic.

         CurrentSplitPacket->offset = SPLIT_UDP_PACKET_SIZE;
      }

   }
   else {

      //A few more sanity checks.  If any fail, we NUKE this packet.

      if((CurrentSplitPacket->TotalChunks != PacketTotal) ||
         (PacketNumber >= CurrentSplitPacket->TotalChunks) ||
         ((!LastPacket) &&
          (CurrentSplitPacket->offset != global_net_message.cursize - SIZEOF_splitpacket_header_t))) {

         memset(CurrentSplitPacket, 0, sizeof(splitpacket_t));
         Con_Printf("Split packet rejected: Found record of this packet, data didn't match up.\n"
                    "The sequence on record shows a total of %i packets with data lengths of %i.\n",
                    CurrentSplitPacket->TotalChunks, CurrentSplitPacket->offset);
      }
   }

   //So far, so good.  Copy it in, set the appropriate bools, and if it's done,
   //return it.

   //First check to see if it's a duplicate packet, for whatever reason.
   //No point in wasting the cycles.

   temp = 1 << PacketNumber;
   if(((CurrentSplitPacket->ReceivedChunks) & temp) == 1) {

      Con_Printf("Split packet rejected: duplicate (it was index %i, our received mask is %X\n", PacketNumber, CurrentSplitPacket->ReceivedChunks);
      return(0);
   }

   //And one last sanity check

   if(LastPacket && (CurrentSplitPacket->CurrentSize + global_net_message.cursize-SIZEOF_splitpacket_header_t > global_net_message.maxsize)) {

      // There are a couple of ways to test for this.  This may look weird,
      // but it should work and doesn't include multiplication.
      Con_Printf("Split packet rejected: Apparently it was too big.");

      memset(CurrentSplitPacket, 0, sizeof(splitpacket_t));
      return(0);
   }


   //FINALLY we're good to go.

   CurrentSplitPacket->ReceivedChunks |= temp;

   temp = PacketNumber*CurrentSplitPacket->offset;
   Q_memcpy((CurrentSplitPacket->data)+temp, global_net_message.data + SIZEOF_splitpacket_header_t, global_net_message.cursize - SIZEOF_splitpacket_header_t);
   CurrentSplitPacket->CurrentSize += global_net_message.cursize;

   //Now if we've finished and built a complete packet we should copy it all back.
   if((CurrentSplitPacket->ReceivedChunks)+1 == (1 << ((CurrentSplitPacket->TotalChunks)+1))) {

      //If all bits are set, adding one will set the next highest bit.
      //Say there are seven. 00111111 -> 01000000.  1<<7 = 01000000.
      Q_memcpy(global_net_message.data, CurrentSplitPacket->data, CurrentSplitPacket->CurrentSize);
      global_net_message.cursize = CurrentSplitPacket->CurrentSize;

      //No need to zero out; further communication will eventually return 0.
      return(1);
   }
   return(0);
}


/* DESCRIPTION: NET_SendPacket
// LOCATION: net_ws.c
// PATH: all over
//
// Sends a packet.  In the case of loopback, there's a special function for it,
// and room to add in IPv6 support (see QW).  One COULD consider this a wrapper
// for sendto(), with localhost and split packet functionality built in.
*/
void NET_SendPacket(netsrc_t netsrc, int length, byte *data, netadr_t to) {

   int ret;
   struct sockaddr addr;
   SOCKET socket;
   int size;

   if(to.type == NA_LOOPBACK) {
      NET_SendLoopPacket(netsrc, length, data);
      return;
   }
   if(to.type == NA_BROADCAST || to.type == NA_IP) { //this should assemble into an identical code block actually

      socket = ip_sockets[netsrc];
      size = sizeof(struct sockaddr_in); //not here yet, but probably will be
   }
   else {

      Sys_Error("NET_SendPacket: Bad address type.");
   }

   if(socket == INVALID_SOCKET) {
      // HL checks for '0' when looking for bad sockets.  With Winsock, 0 can
      // be valid, and only INVALID_SOCKET (-1) is wrong.  Perhaps that define
      // equals zero on unix...
      Con_Printf("NET_SendPacket: Socket[%i] is not valid.");
      return;
   }
   //Now to do the actual sending.
   NetadrToSockadr(&to, &addr);

   ret = NET_SendLong(socket, data, length, 0, &addr, sizeof(addr));

   //And now the error checking.
   //Normally there is error string handling here.  I can look them up
   //jsut fine though, and so as error paths often are, this is light.
   if(ret == SOCKET_ERROR) {

      #ifdef _WIN32
       ret = WSAGetLastError();
      #else
       ret = errno;
      #endif

      if(ret == EWOULDBLOCK || ret == ECONNREFUSED || ret == ECONNRESET) {
         return;
      }
      else if(ret == EADDRNOTAVAIL) {
         if(to.type == NA_BROADCAST) { return; }
         else { Con_Printf("NET_SendPacket Warning: errno = EADDRNOTAVAIL\n"); }
      }
      else if(global_cls.state == ca_dedicated) { //if it equals 0, which it should
         Con_Printf("NET_SendPacket Warning: errno = %i\n", ret);
      }
      else {
         Sys_Error("NET_SendPacket Warning: errno = %i\n", ret);
      }
   }
}
/* DESCRIPTION: NET_SendLoopPacket
// LOCATION: net_wins.c
// PATH: Net_SendPacket->NET_SendLoopPacket
//
// For a linux server this does nothing.  It exists to let client and server
// communicate without dealing with those pesky networks.
*/
void NET_SendLoopPacket(netsrc_t sock, unsigned int length, void *data) { // a parameret 'to' was unused.  Since we hijack SendPacket, we're good.

   int i;
   loopback_t *loop;

//lock if needed

   loop = &loopbacks[sock^1];

   i = loop->send & (MAX_LOOPBACK-1);
   loop->send++;

   if(length > sizeof(loop->msgs[i].data)) {
      Sys_Error("NET_SendLoopPacket: Loopback buffer is too small");
   }

   Q_memcpy(loop->msgs[i].data, data, length);
   loop->msgs[i].datalen = length;

//unlock if needed
}
/* DESCRIPTION: NET_SendLong
// LOCATION:
// PATH: NET_SendPacket->NET_SendLong
//
// Sends a packet.  In the case of loopback, there's a special function for it,
// and room to add in IPv6 support (see QW).
*/
int NET_SendLong(SOCKET socket, void * data, int length, int flags, struct sockaddr * destination, int destinationlength) {

   static int32 SequenceNumber = 1; //seems like they don't let it above 2Bil, signed, probably limit exists in HL too

   char packet[MAX_UDP_ROUTABLE_SIZE];
   splitpacket_header_t *splitpacketunion;

   int CurrentNumBytesSent;
   int memcpySize;
   int PacketCount;
   int PacketNumber;

   int ret;

   if(length > MAX_UDP_PACKET_DATA_SIZE) { return(-2); } //silently fail until we have a better idea.

   if(length > MAX_UDP_ROUTABLE_SIZE) { //Packets larger than 1400 are a blight upon HL servers everywhere

      PacketNumber = CurrentNumBytesSent = 0;

      SequenceNumber++;
      if (SequenceNumber < 0 ) { SequenceNumber = 1; }

      splitpacketunion = (splitpacket_header_t *)packet; //Beats *(int*) assignments I guess
      splitpacketunion->feffffff = hosttole32(0xFFFFFFFE); //so it'll actually be feffffff
      splitpacketunion->SequenceNumber = hosttole32(SequenceNumber);


      //9 bytes overhead per packet, the total number of packets we split it into
      //should be len/1391.  I'll leave the shift opts for later.

      //We need to divide.  And we need to round up when doing it.
      PacketCount = length-1 / SPLIT_UDP_PACKET_SIZE;
      PacketCount++;

      while(length > 0) {

         if(length > SPLIT_UDP_PACKET_SIZE) { memcpySize = SPLIT_UDP_PACKET_SIZE; }
         else { memcpySize = length; }

         splitpacketunion->_4_packetnum4_packettotal = (byte)(PacketNumber<<4) + PacketCount;

         Q_memcpy(packet + SIZEOF_splitpacket_header_t, (byte *)data + CurrentNumBytesSent, memcpySize);

         ret = sendto(socket, packet, memcpySize + SIZEOF_splitpacket_header_t, flags, destination, destinationlength);
         if (ret != memcpySize + SIZEOF_splitpacket_header_t) { // I can't think of any reason why it should be between -1 and what we said to send, but if it is that's probably not good
            return(ret);
         }

         CurrentNumBytesSent += memcpySize;
         length -= memcpySize;
         PacketNumber++;

         // void Sys_uSleep(1); //Who needs sleep when I have COFfEE

         // something about net_showpackets that ain't happening.

      }
      return(CurrentNumBytesSent);
   }
   else {
      return(sendto(socket, data, length, flags, destination, destinationlength));
   }
}



/*** Net Chan ***/

//void Netchan_AddBufferToList() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_AddFragbufToTail() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_AllocFragbuf() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CanPacket() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CheckForCompletion() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_Clear() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_ClearFragbufs() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_ClearFragments() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CopyFileFragments() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CopyNormalFragments() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CreateFileFragments() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CreateFileFragmentsFromBuffer() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CreateFragments() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_CreateFragments_() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_FindBufferById() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_FlushIncoming() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_FragSend() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_IncomingReady() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_Init() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_IsReceiving() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_IsSending() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_OutOfBand() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_OutOfBandPrint() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_Process() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_Setup() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_Transmit() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_UnlinkFragment() { printf("%s called", __FUNCTION__); exit(99); }
//void Netchan_UpdateFlow() { printf("%s called", __FUNCTION__); exit(99); }
void Netchan_UpdateProgress() { printf("%s called", __FUNCTION__); exit(99); } //unused?
//void Netchan_Validate() { printf("%s called", __FUNCTION__); exit(99); }



   static cvar_t cvar_net_chokeloopback = {   "net_chokeloop", "0", FCVAR_NONE, 0.0, NULL };
   static cvar_t cvar_net_showpackets   = { "net_showpackets", "0", FCVAR_NONE, 0.0, NULL };
   static cvar_t cvar_net_showdrop      = {    "net_showdrop", "0", FCVAR_NONE, 0.0, NULL };
   static cvar_t cvar_net_log           = {         "net_log", "0", FCVAR_NONE, 0.0, NULL };
   static cvar_t cvar_net_drawslider    = {  "net_drawslider", "0", FCVAR_NONE, 0.0, NULL };


void Netchan_Init() {

   //testing is more important now with such a huge struct
//   if(sizeof(netchan_t) != 0x2414) { printf("Alignment issues: sizeof netchan struct is %X, not %X", sizeof(netchan_t), 0x2414); }

   Cvar_RegisterVariable(&cvar_net_chokeloopback);
   Cvar_RegisterVariable(&cvar_net_showpackets);
   Cvar_RegisterVariable(&cvar_net_showdrop);
   Cvar_RegisterVariable(&cvar_net_log);
   Cvar_RegisterVariable(&cvar_net_drawslider);
}

/* DESCRIPTION: Netchan_Setup
// LOCATION: net_wins.c
// PATH: Host_ClearClients & SV_ConnectClient
//
// Takes a netchan structure and preps it for use.  Lots of arguments, one of
// which is a struct.  The last argument appears to be a function pointer;
// The second-to-last arg appears to be +0 of a client_t struct, which is thought
// to be a boolean or an enumerated variable that refers to the client's state.
// It could also actually be the HOST_CLIENT variable and not an offset.
// The one right before that is a total mess.  I think it's a client_t structure
// based solely on that it's somehow derived from one.
*/
void Netchan_ClearFragbufs(fragbuf_t **AddressOfStructurePointer) {

   fragbuf_t *ptr, *ptr2;

   if(AddressOfStructurePointer == NULL) {
      Con_Printf("Netchan_ClearFragbufs: Passed NULL argument");
      return;
   }

   // Throw away any that are sitting around
   for(ptr = *AddressOfStructurePointer; ptr != NULL; ptr = ptr2) {
      ptr2 = ptr->next;
      Q_Free(ptr);
   }
   *AddressOfStructurePointer = NULL; //This is why there's the double deref.
}
void Netchan_FlushIncoming(netchan_t *channel, int arrayindex) {

   fragbuf_t *ptr, *ptr2;

   SZ_Clear(&global_net_message);
   global_msg_readcount = 0;

   for(ptr = channel->padding1FD0[arrayindex]; ptr != NULL; ptr = ptr2) {
      ptr2 = ptr->next;
      Q_Free(ptr);
   }
   channel->padding1FD0[arrayindex] = NULL;
   channel->padding1FD8[arrayindex] = 0;
}
void Netchan_ClearFragments(netchan_t *channel) {

   fragbufholder_t *ptr, *ptr2;
   int i;

   for(i = 0; i < 2; i++) {
      for(ptr = channel->padding1FA0[i]; ptr != NULL; ptr = ptr2) {

         ptr2 = ptr->next;
         Netchan_ClearFragbufs(&(ptr->fragbuf));
         Q_Free(ptr);
      }
      channel->padding1FA0[i] = NULL;

      Netchan_ClearFragbufs(&(channel->padding1FB8[i]));
      Netchan_FlushIncoming(channel, i);
   }
}
void Netchan_Clear(netchan_t *channel) {

   int i;

   Netchan_ClearFragments(channel);

   //this is called by more than just netchan_setup; a few vars need to be zerod.
   channel->cleartime = 0;
   channel->reliable_length = 0;

   for(i = 0; i < 2; i++) { //nothing really matches this in QW... I assume thse are related to packet handling since we're zderoing them though.
      channel->padding1FB0[i] = 0;
      channel->padding1FA8[i] = 0;
      channel->NumberOfFragbufsIn1FC0[i] = 0;
      channel->padding1FC8[i] = 0;
      channel->padding1FCC[i] = 0;
      channel->padding1FD8[i] = 0;
   }
   if(channel->TemporaryBuffer != NULL) {
      Q_Free(channel->TemporaryBuffer);
      channel->TemporaryBuffer = NULL;
   }
   channel->TemporaryBufferSize = 0;
}
void Netchan_Setup(netsrc_t netsrc, netchan_t *channel, netadr_t address, int clientindex, client_t * cl, void * function) {

   Netchan_Clear(channel); //This frees memory.

   Q_memset(channel, 0, sizeof(netchan_t));

   channel->socketToUse = netsrc;
   channel->remote_address = address;
   channel->client_index = clientindex;

   channel->last_received = global_realtime;
   channel->first_received = global_realtime; //connected?
   channel->rate = 0; //rate and cleartime are two doubles close in QW...

   channel->outgoing_sequence = 1;

   channel->owning_client = cl;
   channel->fn_GetFragmentSize = function;
   channel->netchan_message.debugname = "netchan->message";
   channel->netchan_message.overflow = 1;
   channel->netchan_message.data = channel->netchan_message_buf;
   channel->netchan_message.maxsize = 0x0f96;
}


/* DESCRIPTION: CanPacket
// LOCATION: net_wins.c
// PATH: SV_Frame->SV_SendClientMessages
//
// "Returns true if the bandwidth choke isn't active".  Guess that means it
// guesstimates a client's packet readiness.
*/
qboolean Netchan_CanPacket(netchan_t *channel) {

   if(cvar_net_chokeloopback.value == 0 && channel->remote_address.type == NA_LOOPBACK) {
      channel->cleartime = global_realtime;
      return(1);
   }
   if(channel->cleartime < global_realtime) {
      return(1);
   }
   return(0);
}


/* DESCRIPTION: Netchan_FragSend
// LOCATION:
// PATH:
//
// Copies a fragbuf_t structure from one part of the whole netchan struct
// to another.  Whoo.
*/
void Netchan_FragSend(netchan_t *channel) {

   fragbufholder_t *list;
   int i;

   if(channel == NULL) {
      Con_Printf("Netchan_FragSend: Passed NULL argument.\n");
      return;
   }

   for(i = 0; i < 2; i++) {

      if(channel->padding1FB8[i] != NULL || channel->padding1FA0[i] == NULL) {
         continue;
      }

      list = channel->padding1FA0[i];
      channel->padding1FA0[i] = channel->padding1FA0[i]->next;
      list->next = NULL;

      channel->padding1FB8[i] = list->fragbuf;
      channel->NumberOfFragbufsIn1FC0[i] = list->count;

      Q_Free(list);
   }
}

/* DESCRIPTION: Netchan_IncomingReady
// LOCATION:
// PATH: SV_ReadPackets
//
// Returns true if one of two values in the netchan struct are true, providing
// further insight into what the two values do.  Ditto for the other two.
*/
qboolean Netchan_IncomingReady(netchan_t *channel) {

   int i;

   if(channel == NULL) { Sys_Error("Netchan_IncomingReady: Passed NULL argument.\n"); }

   for(i = 0; i < 2; i++) {
      if(channel->padding1FD8[i] != 0) { return(1); }
   }
   return(0);
}
qboolean Netchan_IsSending(netchan_t *channel) {

   int i;

   if(channel == NULL) { Sys_Error("Netchan_IsSending called with NULL arg\n"); }

   for(i = 0; i < 2; i++) {
      if(channel->padding1FB8[i] != 0) { return(1); }
   }
   return(0);
}
qboolean Netchan_IsReceiving(netchan_t *channel) {

   int i;

   if(channel == NULL) { Sys_Error("Netchan_IsReceiving called with NULL arg\n"); }

   for(i = 0; i < 2; i++) {
      if(channel->padding1FD0[i] != 0) { return(1); }
   }
   return(0);
}

/* DESCRIPTION: Netchan_AddBufferToList
// LOCATION:
// PATH:  SV_ReadPackets->Netchan_Process->Netchan_FindBufferById
//
// Links one structure to another.  These structures are ordered... somehow...
*/
void Netchan_AddBufferToList(fragbuf_t **fragbuf_list, fragbuf_t *fragbuf) {

   fragbuf_t *ptr;

   if(fragbuf_list == NULL || fragbuf == NULL) {
      Con_Printf("Netchan_AddBufferToList was passed null arguments.\n");
      return;
   }
   //I suppose this is the special case 'goes at beginning of list'
   if(*fragbuf_list == NULL) {
      fragbuf->next = NULL;
      *fragbuf_list = fragbuf;
      return;
   }

   for(ptr = *fragbuf_list; ptr->next != NULL; ptr = ptr->next) {

      //Weird, isn't it.  It's getting the high 16 bits basically.
      if(((ptr->next->ShortID >> 16) & 0xFFFF) >
         ((fragbuf->ShortID >> 16)   & 0xFFFF)) {

         //looks like an insertion
         fragbuf->next = ptr->next->next;
         ptr->next = fragbuf;
         return;
      }
   }
   //Special case: Last in the list
   ptr->next = fragbuf;
}

/* DESCRIPTION: Netchan_AllocFragbuf
// LOCATION:
// PATH: Netchan exclusive
//
// mallocs and zeros this structure.
*/
fragbuf_t * Netchan_AllocFragbuf() {

   fragbuf_t *buf;

   buf = (fragbuf_t *)Q_Malloc(sizeof(fragbuf_t));
   if(buf == NULL) { //We should probably throw an error
      Sys_Error("Malloc failure");
   }

   memset(buf, 0, sizeof(fragbuf_t));

   //move zero to some variables, stupid since we zerod out the struct
   buf->buffer.maxsize = 0x578;
   buf->buffer.data = buf->packetsizedbuffer;

   return(buf);
}

/* DESCRIPTION: Netchan_CopyNormalFragments
// LOCATION:
// PATH: SV_ReadPackets
//
// Reads a chain of filebufs and writes the data to the netchan buffer.  Guess
// it's a higher level 'split packet' feature.
*/
int Netchan_CopyNormalFragments(netchan_t *channel) {

   fragbuf_t *ptr, *ptr2;

   if(channel == NULL) { Sys_Error("Netchan_CopyNormalFragments called with NULL arg"); }

   if(channel->padding1FD8[0] == 0) { return(0); }

   if(channel->padding1FD0[0] == NULL)   {
      Con_Printf( "Netchan_CopyNormalFragments: Called with no fragments readied or something\n" );
      channel->padding1FD8[0] = 0;
      return(0);
   }


   ptr = channel->padding1FD0[0];

   SZ_Clear(&global_net_message);
   MSG_BeginReading();

   do {

      SZ_Write(&global_net_message, ptr->buffer.data, ptr->buffer.cursize);

      ptr2 = ptr->next;
      Q_Free(ptr);
      ptr = ptr2;

   } while(ptr != NULL);

   channel->padding1FD8[0] = 0;
   channel->padding1FD0[0] = NULL;
   return(1);
}

/* DESCRIPTION: Netchan_CopyFileFragments
// LOCATION:
// PATH: SV_ReadPackets
//
// Reads a chain of filebufs and writes the data to a file.  File must fit in
// memory (twice, since the fragments go in memory too).
*/
int Netchan_CopyFileFragments(netchan_t *channel) {

   int filesize;
   char * filebuffer;
   fragbuf_t * ptr, * ptr2;
   char filename[0x104];

   if(channel == NULL) { Sys_Error("Netchan_CopyFileFragments called with NULL arg"); }

   if(channel->padding1FD8[1] == 0) { return(0); }

   if(channel->padding1FD0[1] == NULL)   {
      Con_Printf( "Netchan_CopyFileFragments: Called with no fragments readied or something\n" );
      channel->padding1FD8[1] = 0;
      return(0);
   }

   ptr = channel->padding1FD0[1];

   SZ_Clear(&global_net_message);
   MSG_BeginReading();
   SZ_Write(&global_net_message, ptr->buffer.data, ptr->buffer.cursize);

   Q_strncpy(filename, MSG_ReadString(), sizeof(filename)-1);
   filename[sizeof(filename)-1] = '\0';

   if(filename[0] == '\0') {

      Con_Printf("Netchan_CopyFileFragments: Bad file fragment received.  Purging.\n" );
      Netchan_FlushIncoming(channel, 1);
      return(0);
   }
   else if(strstr(filename, "..") != NULL || strchr(filename, ':') != NULL) { //Might want to look for absolute paths too

      Con_Printf("Netchan_CopyFileFragments: File fragment received with disallowed path.  Ignoring.\n");
      Netchan_FlushIncoming(channel, 1);
      return(0);
   }

   //Filename is the same size as this buffer; it's safe to copy.
   Q_strcpy(channel->filename, filename);

   if(filename[0] != '!') {
      if(FS_FileExists(filename)) {
         Con_Printf("Can't download %s.  That file already exists.\n", filename);
         Netchan_FlushIncoming(channel, 1);
         return(1);
      }
      COM_CreatePath(filename);
   }

   filesize = 0;
   do {

      filesize += ptr->buffer.cursize;
      if(channel->padding1FD0[1] == ptr) { filesize -= global_msg_readcount; }
      ptr = ptr->next;

   } while(ptr != NULL);

   filebuffer = Q_Malloc(filesize);
   if(filebuffer == NULL) {
      Con_Printf("Netchan_CopyFileFragments: Malloc failure (%u bytes).  Not getting file %s.\n", filesize, filename);
      Netchan_FlushIncoming(channel, 1);
      return(0);
   }

   ptr = channel->padding1FD0[1];

   do {

      if(ptr == channel->padding1FD0[1]) { //first iteration, could maybe opti.
         Q_memcpy(filebuffer, ptr->buffer.data+global_msg_readcount, ptr->buffer.cursize - global_msg_readcount);
         filebuffer += ptr->buffer.cursize - global_msg_readcount;
      }
      else {
         Q_memcpy(filebuffer, ptr->buffer.data, ptr->buffer.cursize);
         filebuffer += ptr->buffer.cursize;
      }
      ptr2 = ptr->next;
      Q_Free(ptr);
      ptr = ptr2;

   } while(ptr != NULL);

   if(filename[0] == '!') {

      if(channel->TemporaryBuffer != NULL) {
         Con_Printf("Netchan_CopyFileFragments: Freeing some temp buffer that someone thought was worth mentioning\n");
         Q_Free(channel->TemporaryBuffer);
      }
      channel->TemporaryBuffer = filebuffer;
      channel->TemporaryBufferSize = filesize;
   }
   else {
      COM_WriteFile(filename, filebuffer, filesize);
      Q_Free(filebuffer);
   }

   SZ_Clear(&global_net_message);
   global_msg_readcount = 0;
   channel->padding1FD8[1] = 0;
   channel->padding1FD0[1] = 0;

   return(1);
}

/* DESCRIPTION: Netchan_FindBufferById
// LOCATION:
// PATH: Netchan_Process
//
// Returns a pointer to the fragbuf object that matches the passed ID
// (There's no weird shifting or anything here BTW)
// If it's not found and arg8 is set, we make a brand new fragbuf and give it
// that ID.
*/
fragbuf_t * Netchan_FindBufferById(fragbuf_t **fragbuf_list, int ID, int arg8) {

   fragbuf_t *ptr;

   if(fragbuf_list == NULL) { Sys_Error("Netchan_FindBufferById: Passed NULL pointer"); }

   for(ptr = *fragbuf_list; ptr != NULL; ptr = ptr->next) {
      if(ptr->ShortID == ID) { return(ptr); }
   }

   if(arg8 == 0) { return(NULL); }

   ptr = Netchan_AllocFragbuf(); //Was malloc + manual var adjusting, but that seems a little silly.
   ptr->ShortID = ID;
   Netchan_AddBufferToList(fragbuf_list, ptr);

   return(ptr);
}


void Netchan_AddFragbufToTail(fragbufholder_t *list, fragbuf_t *fragbuf) {

   fragbuf_t *ptr;

   fragbuf->next = NULL;
   list->count++;

   //special case 1 Number one
   if(list->fragbuf == NULL) {
      list->fragbuf = fragbuf;
   }
   else {

      ptr = list->fragbuf;
      while(ptr->next != NULL) { ptr = ptr->next; }
      ptr->next = fragbuf;
   }
}
void Netchan_UnlinkFragment(fragbuf_t *fragbuf, fragbuf_t **fragbuf_list) {

   fragbuf_t *ptr;

   if(fragbuf_list == NULL || fragbuf == NULL) { Sys_Error("Netchan_UnlinkFragment: Called with NULL argument.\n"); }

   //Special case 1: Beginning of list
   if(fragbuf == *fragbuf_list) {

      *fragbuf_list = fragbuf->next;
      Q_Free(fragbuf);
      return;
   }
   for(ptr = *fragbuf_list; ptr->next != NULL; ptr = ptr->next) {

      if(ptr->next == fragbuf) {
         ptr->next = ptr->next->next;
         Q_Free(fragbuf);
         return;
      }
   }

   Sys_Error("Netchan_UnlinkFragment: Couldn't find fragment in passed list.  Maybe the pointers are blitzed?\n" );
}
/* DESCRIPTION: Netchan_UpdateFlow
// LOCATION:
// PATH: Netchan_Transmit/Process
//
// Whenever we send or receive a packet, we call this to help keep track of
// how many bits we've sent how often and how well the client can handle the
// given rate.  It's a mess, but I think I have it down now.
*/
void Netchan_UpdateFlow(netchan_t *channel) {

   int arrayindex;
   flowstruct1_t * ptr1;
   flowstruct2_t * ptr2, * ptr3;
   unsigned int var_8;
   unsigned int var_C;
   unsigned int var_10;
   double var_14;

   if(channel == NULL) { return; }

   var_14 = 0;
   var_10 = 0;

   for(arrayindex = 0; arrayindex < 2; arrayindex++) {

      ptr1 = &(channel->flow[arrayindex]);

      if(global_realtime - ptr1->padding184 < 0.1) { continue; }

      ptr1->padding184 = global_realtime + 0.1;
      var_8 = ptr1->last_received - 1;
      ptr2 = &(ptr1->array[var_8 & 0x1F]); //cheap way of doing an if(var8 > 32) var8 = 0;

      for(var_C = 1; var_C < 0x0F; var_C++) {

         ptr3 = &ptr1->array[((var_8 - var_C) & 0x1F)]; //ugh, counting backwards and manipulating ANDs to drop it down between 0 and 31

         var_10 += ptr3->padding00;
         var_14 += ptr2->padding04 - ptr3->padding04;

         ptr2 = ptr3;
      }

      //This seems to be a speed saver to prevent an extra divide.
      if(var_14 == 0) { ptr1->KBflowrate = 0.0; }
      else {
         ptr1->KBflowrate = var_10 / var_14;
         ptr1->KBflowrate /= 1024.0;
      }
      ptr1->KBflowrateaverage = (ptr1->KBflowrateaverage * (2/3)) + (ptr1->KBflowrate * (1/3));
   }
}

void Netchan_CreateFragments_(qboolean AreWeServer, netchan_t *channel, sizebuf_t *msg) {

   fragbuf_t * var_4;
   int var_8;
   int var_c;
   int var_10;
   int var_14;
   int var_18 = 1;
   fragbufholder_t * var_1c;
   fragbufholder_t * var_20;

   if(msg->cursize == 0) { return; }

   var_8 = channel->fn_GetFragmentSize(channel->owning_client);
   var_1c = Q_Malloc(sizeof(fragbufholder_t));

   if(var_1c == NULL) { Sys_Error("Netchan_CreateFragments_ failed to MALLOC something."); }

   Q_memset(var_1c, 0, sizeof(fragbufholder_t));
   var_14 = 0;
   var_10 = msg->cursize;

   while(var_10 > 0) {

      if(var_8 <= var_10) { var_c = var_8; }
      else { var_c = var_10; }

      var_10 -= var_c; //This appears to me a block size, var_8 and var_c.
      var_4 = Netchan_AllocFragbuf();
      if(var_4 == NULL) { //Bad error type path, bad

         Con_Printf("Netchan_CreateFragments_ failed after Netchan_AllocFragbuf returned NULL.\n");
         Q_Free(var_1c);
         if(AreWeServer != 0) { //I just realized, this isn't a netsrc, this is OUR status.  Whatever, change the bool later--same result, if we're a server, do stuff.
            SV_DropClient(global_host_client, 0, "Malloc problemo.\n");
         }
         return;
      }

      var_4->ShortID = var_18;
      var_18++;
      SZ_Clear(&(var_4->buffer));
      SZ_Write(&(var_4->buffer), msg->data + var_14, var_c);
      var_14 += var_c;
      Netchan_AddFragbufToTail(var_1c, var_4);
   }

   //We've processed all the passed data, whatever it was...
   if(channel->padding1FA0[0] != NULL) {
      var_20 = channel->padding1FA0[0];
      while(var_20->next != NULL) {
         var_20 = var_20->next;
      }
      var_20->next = var_1c;
   }
   else {
      channel->padding1FA0[0] = var_1c;
   }
}
void Netchan_CreateFragments(qboolean AreWeServer, netchan_t *channel, sizebuf_t *msg) {

   if(channel->netchan_message.cursize > 0) {
      Netchan_CreateFragments_(AreWeServer, channel, &(channel->netchan_message));
      channel->netchan_message.cursize = 0;
   }
   Netchan_CreateFragments_(AreWeServer, channel, msg);
}

int Netchan_CreateFileFragments(qboolean AreWeServer, netchan_t *channel, const char * filename) {

   hl_file_t * var_1c_filep;

   fragbuf_t * var_4_NewFragBuffer;
   int var_8_MaximumFragbufSize;
   int var_c_CurrentDataChunkSize;
   int var_10_DataLeftToSend;
   int var_14_DataAlreadySent;
   int var_18_NumberOfFragbufsMade = 1;

   int var_20_SizeOfFile = 0;
   int var_24_IsFirstFragment = 1; //We have a count var, we could just use that,,,
   fragbufholder_t * var_28_ptrCurrentFragbuf;
   fragbufholder_t * ptrTempFragbuf;

   var_1c_filep = FS_Open(filename, "rb");

   if(var_1c_filep == NULL) {
      Con_Printf("Netchan_CreateFileFragments: File \"%s\" couldn't be opened for transfer", filename);
      return(0);
   }

   var_8_MaximumFragbufSize = channel->fn_GetFragmentSize(channel->owning_client);

   //Though this is what HL does, it is not safe owing to the non-atomic nature
   //of a file system.
   var_20_SizeOfFile = FS_Size(var_1c_filep);
   FS_Close(var_1c_filep);

   var_28_ptrCurrentFragbuf = Q_Malloc(sizeof(fragbufholder_t));
   if(var_28_ptrCurrentFragbuf == NULL) { Sys_Error("Netchan_CreateFileFragmentsFromBuffer failed to MALLOC something."); }
   Q_memset(var_28_ptrCurrentFragbuf, 0, sizeof(fragbufholder_t));


   var_14_DataAlreadySent = 0;
   var_10_DataLeftToSend = var_20_SizeOfFile;

   while(var_10_DataLeftToSend > 0) {

      if(var_8_MaximumFragbufSize <= var_10_DataLeftToSend) { var_c_CurrentDataChunkSize = var_8_MaximumFragbufSize; }
      else { var_c_CurrentDataChunkSize = var_10_DataLeftToSend; }

      var_4_NewFragBuffer = Netchan_AllocFragbuf();
      if(var_4_NewFragBuffer == NULL) {

         Con_Printf("Netchan_CreateFileFragments failed after Netchan_AllocFragbuf returned NULL.\n");
         Q_Free(var_28_ptrCurrentFragbuf);
         if(AreWeServer != 0) { //Should always be true
            SV_DropClient(global_host_client, 0, "Malloc problemo.\n");
         }
         return(0);
      }

      var_4_NewFragBuffer->ShortID = var_18_NumberOfFragbufsMade;
      var_18_NumberOfFragbufsMade++;
      SZ_Clear(&(var_4_NewFragBuffer->buffer));

      if(var_24_IsFirstFragment) {
         var_24_IsFirstFragment = 0;
         MSG_WriteString(&(var_4_NewFragBuffer->buffer), filename);
         var_c_CurrentDataChunkSize -= var_4_NewFragBuffer->buffer.cursize;
      }

      var_4_NewFragBuffer->boolean1 = 1;

      var_4_NewFragBuffer->fragsize = var_c_CurrentDataChunkSize;
      var_4_NewFragBuffer->fragoffset = var_14_DataAlreadySent;

      strncpy(var_4_NewFragBuffer->padding, filename, 259);
      var_4_NewFragBuffer->padding[259] = '\0';

      var_14_DataAlreadySent += var_c_CurrentDataChunkSize;
      var_10_DataLeftToSend -= var_c_CurrentDataChunkSize;

      Netchan_AddFragbufToTail(var_28_ptrCurrentFragbuf, var_4_NewFragBuffer);
   }

   if(channel->padding1FA0[1] != NULL) {
      ptrTempFragbuf = channel->padding1FA0[1];
      while(ptrTempFragbuf->next != NULL) {
         ptrTempFragbuf = ptrTempFragbuf->next;
      }
      ptrTempFragbuf->next = var_28_ptrCurrentFragbuf;
   }
   else {
      channel->padding1FA0[1] = var_28_ptrCurrentFragbuf;
   }
   return(1);
}

void Netchan_CreateFileFragmentsFromBuffer(qboolean AreWeServer, netchan_t *channel, const char * filename, const char *filebuffer, int filesize) {

   fragbuf_t * var_4_NewFragBuffer;
   int var_8_MaximumFragbufSize;
   int var_c_CurrentDataChunkSize;
   int var_10_DataLeftToSend;
   int var_14_DataAlreadySent;
   int var_18_NumberOfFragbufsMade = 1;

   int var_20_IsFirstFragment = 1; //We have a count var, we could just use that,,,
   fragbufholder_t * var_24_ptrCurrentFragbuf;
   fragbufholder_t * ptrTempFragbuf;

   if(filesize < 1) { return; }

   var_8_MaximumFragbufSize = channel->fn_GetFragmentSize(channel->owning_client);

   var_24_ptrCurrentFragbuf = Q_Malloc(sizeof(fragbufholder_t));
   if(var_24_ptrCurrentFragbuf == NULL) { Sys_Error("Netchan_CreateFileFragmentsFromBuffer failed to MALLOC something."); }
   Q_memset(var_24_ptrCurrentFragbuf, 0, sizeof(fragbufholder_t));


   var_14_DataAlreadySent = 0;
   var_10_DataLeftToSend = filesize;

   while(var_10_DataLeftToSend > 0) {

      if(var_8_MaximumFragbufSize <= var_10_DataLeftToSend) { var_c_CurrentDataChunkSize = var_8_MaximumFragbufSize; }
      else { var_c_CurrentDataChunkSize = var_10_DataLeftToSend; }

      var_10_DataLeftToSend -= var_c_CurrentDataChunkSize; //Copied from the function netchan_createfragments
      var_4_NewFragBuffer = Netchan_AllocFragbuf();
      if(var_4_NewFragBuffer == NULL) {

         Con_Printf("Netchan_CreateFileFragmentsFromBuffer failed after Netchan_AllocFragbuf returned NULL.\n");
         Q_Free(var_24_ptrCurrentFragbuf);
         if(AreWeServer != 0) { //Should always be true
            SV_DropClient(global_host_client, 0, "Malloc problemo.\n");
         }
         return;
      }

      var_4_NewFragBuffer->ShortID = var_18_NumberOfFragbufsMade;
      var_18_NumberOfFragbufsMade++;
      SZ_Clear(&(var_4_NewFragBuffer->buffer));

      if(var_20_IsFirstFragment) {
         var_20_IsFirstFragment = 0;
         MSG_WriteString(&(var_4_NewFragBuffer->buffer), filename);
         var_c_CurrentDataChunkSize -= var_4_NewFragBuffer->buffer.cursize;
      }

      var_4_NewFragBuffer->boolean1 = 1;
      var_4_NewFragBuffer->boolean2 = 1;
      var_4_NewFragBuffer->fragsize = var_c_CurrentDataChunkSize;
      var_4_NewFragBuffer->fragoffset = var_14_DataAlreadySent;

      MSG_WriteBuf(&(var_4_NewFragBuffer->buffer), var_c_CurrentDataChunkSize, filebuffer + var_14_DataAlreadySent);

      var_14_DataAlreadySent += var_c_CurrentDataChunkSize;
      var_10_DataLeftToSend -= var_c_CurrentDataChunkSize;
      Netchan_AddFragbufToTail(var_24_ptrCurrentFragbuf, var_4_NewFragBuffer);
   }

   if(channel->padding1FA0[1] != NULL) {
      ptrTempFragbuf = channel->padding1FA0[1];
      while(ptrTempFragbuf->next != NULL) {
         ptrTempFragbuf = ptrTempFragbuf->next;
      }
      ptrTempFragbuf->next = var_24_ptrCurrentFragbuf;
   }
   else {
      channel->padding1FA0[1] = var_24_ptrCurrentFragbuf;
   }
}

void Netchan_Transmit(netchan_t *channel, int arg_length, char * arg_data) {

   char TempBuffer[4040]; //basically a max packet size.  8400 is what I have listed elsewhere.  I forget why.
   sizebuf_t packetthingy;

   qboolean transmit_IsReliable;
   unsigned int var_fe4;
   unsigned int var_fe8;
   int var_fec;
   unsigned int var_ff0;
   unsigned int var_ff4;
   unsigned int i;
   unsigned int j;

   float var_1000;
   unsigned int var_1004;
   fragbuf_t * var_1008;
   unsigned int var_1014;
   unsigned int var_1010[2];
   unsigned int var_1018; //Might get compiler warning, is safe.
   hl_file_t * var_101c;

   if(channel->netchan_message.overflow & 0x02) {
      Con_Printf("%s had an outgoing message overflow.  Sucks to be him.\n", NET_AdrToString(channel->remote_address));
      return;
   }

   packetthingy.overflow = 0;
   packetthingy.data = TempBuffer;
   packetthingy.maxsize = 4037;
   packetthingy.cursize = 0;

   transmit_IsReliable = 0;
   var_fe8 = 0;

   if((channel->incoming_acknowledged > channel->outgoing_last_reliable_sequence) && (channel->incoming_reliable_acknowledged != channel->outgoing_reliable_sequence)) {

      transmit_IsReliable = 1;
      var_fe8 = 1;
   }

   if(channel->reliable_length == 0) {

      var_1004 = 0;
      var_1010[0] = 0;
      var_1010[1] = 0;
      var_1014 = 0;

      Netchan_FragSend(channel);
      if(channel->netchan_message.cursize != 0) { var_1014 = 1; }

      for(i = 0; i < 2; i++) {
         if(channel->padding1FB8[i] != NULL) {
            var_1010[i] = 1;
         }
      }

      if(var_1010[0] != 0 && var_1014 != 0) {

         var_1014 = 0;
         if(channel->netchan_message.cursize > 1200) {

            //This is messed up...
            Netchan_CreateFragments((channel == &(global_cls.netchan)? 0 : 1), channel, &(channel->netchan_message));
         }
      }

      for(i = 0; i < 2; i++) {

         channel->padding1FB0[i] = 0;
         channel->padding1FA8[i] = 0;
         channel->padding1FC8[i] = 0;
         channel->padding1FCC[i] = 0;

         if(var_1010[i] != 0) {
            var_1004 = 1;
         }
      }

      if(var_1014 != 0 || var_1004 != 0) {

         //This looks like the reliable bit signalling...
         channel->outgoing_reliable_sequence ^= 1;
         transmit_IsReliable = 1;
      }

      if(var_1014 != 0) {

         Q_memcpy(channel->reliable_buf, channel->netchan_message_buf, channel->netchan_message.cursize);
         channel->reliable_length = channel->netchan_message.cursize;
         channel->netchan_message.cursize = 0;

         for(i = 0; i < 2; i++) {

            channel->padding1FC8[i] = channel->reliable_length;
         }
      }

      for(i = 0; i < 2; i++) {

         var_1008 = channel->padding1FB8[i];

         if(var_1008 != NULL) {
            var_1018 = var_1008->buffer.cursize;
            if((var_1008->boolean1 != 0) && (var_1008->boolean2 == 0)) {
               var_1018 = var_1008->fragsize;
            }
         }

         if(var_1010[i] != 0 && var_1008 != NULL &&
            channel->reliable_length + var_1018 < 1200) {

            channel->padding1FB0[i] = ((var_1008->ShortID & 0xFFFF) << 0x10) | (channel->NumberOfFragbufsIn1FC0[i] & 0xFFFF);
            if((var_1008->boolean1 != 0) && (var_1008->boolean2 == 0)) {

               /* I'm not 100% clear on what's going on, but I think
               // what happened was we allocated room for this file's sending
               // breforehand.
               // We didn't actually load that file IN though.
               // So what follows is, due to the volatile nature of the
               // filesystem, a big ol' crash just waiting to happen.
               //
               // Whoever wrote this needs to be shot.
               */
               printf("Scary file stuff entered\n");

               var_101c = FS_Open(var_1008->padding, "rb");
               if(var_101c == NULL) { Sys_Error("Netchan_Transmit: That POS file code?  Yeah, it crashed.\n"); }

               FS_Seek(var_101c, var_1008->fragoffset, 0);
               FS_Read(var_1008->buffer.data + var_1008->buffer.cursize, var_1008->fragsize, 1, var_101c);
               var_1008->buffer.cursize += var_1008->fragsize;
               FS_Close(var_101c);
            }

            Q_memcpy(channel->reliable_buf + channel->reliable_length, var_1008->buffer.data, var_1008->buffer.cursize);
            channel->reliable_length += var_1008->buffer.cursize;
            channel->padding1FCC[i] = var_1008->buffer.cursize;
            Netchan_UnlinkFragment(var_1008, &(channel->padding1FB8[i]));

            channel->padding1FA8[i] = 1;

            for(j = i +1; i < 2; i++) { //Pointless I know, but how it is coded.
               channel->padding1FC8[j] += channel->padding1FCC[j];
            }
         }
      }
   }

   var_ff0 = channel->outgoing_sequence | (transmit_IsReliable << 0x1f);
   var_ff4 = channel->incoming_sequence | (channel->incoming_reliable_sequence << 0x1f);
   var_fe4 = 0;

   for(i = 0; i < 2; i++) {
      if(channel->padding1FA8[i] != 0) {
         var_fe4 = 1;
         break;
      }
   }

   if(transmit_IsReliable != 0 && var_fe4 != 0) {
      var_ff0 |= (1 << 0x1e); //weird optimizing here :) but I know how it sends...
   }

   channel->outgoing_sequence++;
   MSG_WriteLong(&packetthingy, var_ff0);
   MSG_WriteLong(&packetthingy, var_ff4);

   if(transmit_IsReliable != 0 && var_fe4 != 0) {


      for(i = 0; i < 2; i++) {
         if(channel->padding1FA8[i] != 0) {
            MSG_WriteByte(&packetthingy, 1);
            MSG_WriteLong(&packetthingy, channel->padding1FB0[i]);
            MSG_WriteShort(&packetthingy, channel->padding1FC8[i]);
            MSG_WriteShort(&packetthingy, channel->padding1FCC[i]);
         }
         else {
            MSG_WriteByte(&packetthingy, 0);
         }
      }
   }

   if(transmit_IsReliable != 0) {
      SZ_Write(&packetthingy, &channel->reliable_buf, channel->reliable_length);
      channel->outgoing_last_reliable_sequence = channel->outgoing_sequence - 1;
   }

   if(var_fe8 == 0) { var_fec = packetthingy.maxsize; }
   else { var_fec = 0x578; }

   if(var_fec - packetthingy.cursize < arg_length) {
      //debug printf
      Con_Printf("Netchan_Transmit: Message would overflow, ignoring");
   }
   else {
      SZ_Write(&packetthingy, arg_data, arg_length);
   }

   //This looks to be padding, so I simplified it a little.
   while(packetthingy.cursize < 16) {
      MSG_WriteByte(&packetthingy, 1);
   }

   channel->flow[0].array[channel->flow[0].last_received & 31].padding00 = packetthingy.cursize + 28;
   channel->flow[0].array[channel->flow[0].last_received & 31].padding04 = global_realtime;
   channel->flow[0].last_received++;
   Netchan_UpdateFlow(channel);

   if(global_cls.demoplayback == DPB_0) {
#if 0
      if(packetthingy.cursize > 20) {

         int i;
         printf("**");
         for(i = 8; i < packetthingy.cursize; i++) {

            printf("0x%X", packetthingy.data[i]);
         }
         printf("\n");
      }
#endif
      COM_Munge2(packetthingy.data+8, packetthingy.cursize-8, ((channel->outgoing_sequence - 1) & 0xff));
      NET_SendPacket(channel->socketToUse, packetthingy.cursize, packetthingy.data, channel->remote_address);
   }

   if(global_sv.active && cvar_sv_lan.value && cvar_sv_lan_rate.value < 1000.0) {
      var_1000 = 1.0 / cvar_sv_lan_rate.value;
   }
   else {
      var_1000 = 1.0 / channel->rate;
   }

   if(channel->cleartime < global_realtime) {
      channel->cleartime = global_realtime;
   }
   channel->cleartime += (packetthingy.cursize + 28) * var_1000; //What the hell!?

   if(cvar_net_showpackets.value == 1) { //The check is actually != 0 && != 2.  *shrug*

      char temp;
      if(&(global_cls.netchan) == channel) { temp = 'c'; }
      else { temp = 's'; }

      //I'm omitting time because I hate deciphering floating point stuff
      Con_Printf("%c --> sz=%i seq=%i ack=%i rel=%i\n", temp, packetthingy.cursize, channel->outgoing_sequence - 1, channel->incoming_sequence, transmit_IsReliable);
   }
}

/* DESCRIPTION: Netchan_CheckForCompletion
// LOCATION:
// PATH: SV_ReadPackets->Netchan_Process
//
//
*/
void Netchan_CheckForCompletion(netchan_t *channel, int arrayindex, int arg_8) {

   int size, count;
   char temp[32];
   fragbuf_t *ptr;

   ptr = channel->padding1FD0[arrayindex];
   if(ptr == NULL) { return; }

   count = 0;
   size = 0;

   do {

      size += ptr->buffer.cursize;
      count++;

      if(((ptr->ShortID >> 16) & 0xFFFF) != count &&
         (&(global_cls.netchan) == channel)) {

         if(channel->socketToUse == NS_Unknown2) { //I didn't think there was a '2'...

            strcpy(temp, "listen ");
            NET_AdrToStringThread(channel->remote_address, temp+7, sizeof(temp)-7);
            Cbuf_AddText(temp);
            return;
         }
         else {
            Con_Printf( "Netchan_CheckForCompletion:  Lost/dropped fragment would cause stall, retrying connection\n" );
            Cbuf_AddText( "retry\n" );
         }
      }
      ptr = ptr->next;
   } while(ptr != NULL);

   if(count == arg_8) { channel->padding1FD8[arrayindex] = 1; }
}


/* DESCRIPTION: Netchan_OutOfBand
// LOCATION: net_wins.c
// PATH: SV_ConnectClient, logging
//
// Makes this OutOfBand packet thing.  Since it's only called by the function
// below thisaone, unless there's smoe SDK tomfoolery, I have to wonder why
// it's not all one nice, efficient function.
//
*/
void Netchan_OutOfBand(netsrc_t sock, netadr_t address, unsigned int length, char *data) {

   char send_buf[0x0FC5];

   //Write that -1 at the beginning...
   *(unsigned long int*)(send_buf) = 0xFFFFFFFF; //the same as memcpy /xff/xff/xff/xff

   //memcpy data.  Make sure it fits though.
   if(length > sizeof(send_buf) - 5) { //4 for the long, one for the terminating null
      length = sizeof(send_buf) - 5;
   }

   Q_memcpy(send_buf+4, data, length);
   send_buf[length+4] = '\0';

   //check for global_cls.demoplayback ignored; present in HL, but defined out for server only.
   NET_SendPacket(sock, length+5, send_buf, address);
}
/* DESCRIPTION: Netchan_OutOfBandPrint
// LOCATION: net_wins.c
// PATH: SV_ConnectClient, logging
//
// Prints a variable argument string to a temporary buffer and forwards it
// along to Netchan_OutOfBand.
//
*/
void Netchan_OutOfBandPrint(netsrc_t sock, netadr_t address, char *format, ...) {

   va_list argptr;
   char string[4096]; //This was static.  Now it isn't.  Shouldn't break anything..
   unsigned int length;

   va_start(argptr, format);
   length = Q_vsnprintf(string, sizeof(string), format, argptr);
   va_end(argptr);

   //printf functions return the number of characters written.
   //BUT if there isn't enough ROOM to store everything the function returns
   //the amount of characters that WOULD'VE been written.  We'll need to check
   //for and adjust for this possibility.  Faster than a strlen...

   if(length >= sizeof(string)) {
      length = sizeof(string);
   } //a terminating null shouldn't be necessary; OutOfBand as  wrote it doesn't require one (and will end up truncating anyway)

   Netchan_OutOfBand(sock, address, length, string);
}

//Does some bounds checking; needs work.
int Netchan_Validate(int * arg4, int * arg8, int * argc, int * arg10) {

   int i, var_8, var_c;

   for(i = 0; i < 2; i++) {

      if(arg4[i] == 0) { continue; }
      var_8 = arg8[i] >> 16;
      var_c = arg8[i] & 0xFFFF;
      if(var_8 < 0 || var_8 > 0x61A8) { break; }
      if(var_c < 0 || var_c > 0x61A8) { break; }
      if(arg10[i] < 0 || arg10[i] > 0x800) { break; }
      if(argc[i] < 0 || argc[i] > 0x4000) { break; }
   }
   if(i > 1) { return(1); }
   return(0);
}

/*
//
// Header details: 1 bit reliable, 1 bit validation, 30 bits sequence
//                 1 bit last reliable offbit, 1 bit unknown, 30 bits ack
*/
qboolean Netchan_Process(netchan_t *channel) {

   int i;
   unsigned int sequence;
   unsigned int acknowledgement;
   unsigned int ReliableConfirmation;
   qboolean receiving_IsReliable;
   qboolean RunValidation;
   qboolean RequireSecurity;

   int var_24[2];
   int var_1C[2];
   int var_2C[2];
   int var_34[2];

   int var_50;
   char * var_54;
   char * var_58;
   int var_5c;
   fragbuf_t * var_60;
   int var_64;
   int var_68;

   if(channel == NULL) { Sys_Error("%s: Passed NULL netchan.\n", __FUNCTION__); }

   if(global_cls.demoplayback == DPB_0 && global_cls.padding438C == 0 && NET_CompareAdr(channel->remote_address, global_net_from) == 0) {
      printf("%s: Ignoring packet due to some CLS checks", __FUNCTION__);
      return(0);
   }

   MSG_BeginReading();

   channel->last_received = global_realtime;
   //Get the sequence and acknowledgement values.
   sequence = MSG_ReadLong();
   acknowledgement = MSG_ReadLong();

   //Derive the four booleans we'll need.

   receiving_IsReliable = (sequence & 0x80000000) != 0;
   RunValidation = (sequence & 0x40000000) != 0;
   ReliableConfirmation = (acknowledgement & 0x80000000) != 0;
   RequireSecurity = (acknowledgement & 0x40000000) != 0;

   //And remove them from our seq and ack.
   sequence &= 0x3FFFFFFF;
   acknowledgement &= 0x3FFFFFFF;


   if(RequireSecurity) {
      printf("%s: Packet requested security processing, which we can't do.\n", __FUNCTION__);
      return(0);
   }

   //decode it
   COM_UnMunge2(global_net_message.data+8, global_net_message.cursize-8, (sequence & 0xFF));


   if(RunValidation != 0) {

      printf("%s: Doing validation thing\n", __FUNCTION__);
      for(i = 0; i < 2; i++) {
         if(MSG_ReadByte() != 0) {
            var_24[i] = 1;
            var_1C[i] = MSG_ReadLong();
            var_2C[i] = MSG_ReadShort();
            var_34[i] = MSG_ReadShort();
         }
         else {

            var_24[i] = 0;
            var_1C[i] = 0;
            var_2C[i] = 0;
            var_34[i] = 0;
         }
      }

      if(Netchan_Validate(var_24, var_1C, var_2C, var_34) == 0) {
         printf("%s: Failed validation\n", __FUNCTION__);
         return(0);
      }
   }


   if(cvar_net_showpackets.value != 0) {
      Con_Printf("%c <-- sz=%i seq=%i ack=%i rel=%i tm=%f\n",
                 (global_cls.netchan.socketToUse == channel->socketToUse) ? 'c' : 's',
                 global_net_message.cursize,
                 sequence,
                 acknowledgement,
                 receiving_IsReliable,
                 global_sv.time0c);
   }

   if(channel->incoming_sequence >= sequence) { //Drop this packet--it's in the wrong order.
      if(cvar_net_showdrop.value != 0) {
         if(sequence == channel->incoming_sequence) {
            Con_Printf("%s:duplicate packet %i at %i\n", NET_AdrToString(channel->remote_address), sequence, channel->incoming_sequence);
         }
         else {
            Con_Printf("%s:out of order packet %i at %i\n", NET_AdrToString(channel->remote_address), sequence, channel->incoming_sequence);
         }
      }
      return(0);
   }

   //Now, if the sequence number is HIGHER, we'll end up skipping packets.  Better lost than late.
   global_net_drop = (sequence - channel->incoming_sequence) - 1;
   if(global_net_drop > 0) {

      if(cvar_net_showdrop.value != 0) {
         Con_Printf("%s:Will drop %i packets at %i\n", NET_AdrToString(channel->remote_address), global_net_drop, sequence);
      }
   }

   if(ReliableConfirmation == channel->outgoing_reliable_sequence) {
      if(channel->incoming_acknowledged + 1 >= channel->outgoing_last_reliable_sequence) { //an old message isn't quite as believable
         channel->reliable_length = 0;
      }
   }

   channel->incoming_sequence = sequence;
   channel->incoming_acknowledged = acknowledgement;
   channel->incoming_reliable_acknowledged = ReliableConfirmation;

   if(receiving_IsReliable != 0) {
      channel->incoming_reliable_sequence ^= 1;
   }

   channel->flow[1].array[(channel->flow[1].last_received & 0x1F)].padding00 = (global_net_message.cursize)+0x1C;
   channel->flow[1].array[(channel->flow[1].last_received & 0x1F)].padding04 = global_realtime;
   channel->flow[1].last_received++;

   Netchan_UpdateFlow(channel);

   if(RunValidation == 0) { return(1); }

   for(i = 0; i < 2; i++) {
      if(var_24[i] == 0) { continue; }

      if(var_1C[i] != 0) {
         var_64 = (var_1C[i] >> 16) & 0xFFFF;
         var_68 = (var_1C[i]) & 0xFFFF;
         var_60 = Netchan_FindBufferById(&(channel->padding1FD0[i]), var_1C[i], 1);
         if(var_60 == NULL) {
            Con_Printf("%s: Couldn't allocate or find buffer %i\n", __FUNCTION__, var_64);
         }
         else {
            //Me no trust these numbers.  Note to self--check here for mean and nasty attacks.
            SZ_Clear(&(var_60->buffer));
            SZ_Write(&(var_60->buffer), global_net_message.data + global_msg_readcount + var_2C[i], var_34[i]);
            if(var_60->buffer.overflow & 2) {
               Con_Printf("%s: Overflow working on fragbuf %i\n", __FUNCTION__, var_64);
               if(channel != &(global_cls.netchan)) {
                  channel->netchan_message.overflow |= 2;
               }
               return(0);
            }
         }
         Netchan_CheckForCompletion(channel, i, var_68);
      }

      var_54 = global_net_message.data + global_msg_readcount + var_2C[i] + var_34[i];
      var_58 = global_net_message.data + global_msg_readcount + var_2C[i];
      var_5c = ((global_net_message.cursize - var_34[i]) - var_2C[i]) - global_msg_readcount;
      var_50 = 0;

      while(var_50 < var_5c) {

         *var_58 = *var_54;
         var_50++;
         var_54++;
         var_58++;
      }

      global_net_message.cursize = global_net_message.cursize - var_34[i];

      //Doesn't look like much of a loop to me
      var_50 = i + 1;
      while(var_50 < 2) {

         var_2C[var_50] -= var_34[i];
         var_50++;
      }
   }
   if(global_net_message.cursize > 0x10) { return(1); }

   return(0);
}

