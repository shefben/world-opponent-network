#include <ctype.h>
#include <limits.h>
#if(defined(_MSC_VER) || defined(__BORLANDC__))
 #include <winsock2.h>
 #include <windows.h>
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#endif

#include "SV_hlds.h"

#include "cmd.h"
#include "cvar.h"
#include "host.h"
#include "HPAK.h"
#include "Modding.h"
#include "pf.h"
#include "report.h"
#include "SV_clres.h"
#include "SV_frame.h"
#include "SV_mcast.h"
#include "SV_phys.h"
#include "SV_pkt.h"
#include "SV_pktcl.h"
#include "SV_rcon.h"
#include "SV_user.h"
#include "sys.h"
#include "VecMath.h"
#include "world.h"

/*** STRUCTS ***/
typedef struct svdeltacallback_s {

   uint32 * padding00;
   uint32 padding04;
   uint32 padding08;
   uint32 padding0C;
   uint32 padding10;
   uint32 padding14;
   uint32 padding18;
   uint32 padding1C;
} svdeltacallback_t;


/*** GLOBALS ***/
playermove_t * global_pmove = NULL;
playermove_t   global_svmove;
char global_localinfo[0x8001];
edict_t * global_sv_player = NULL;
gpnewusermsg_struct_t * global_sv_gpNewUserMsg;
gpnewusermsg_struct_t * global_sv_gpUserMsg;
int key_dest;


/*** STATIC GLOBALS ***/
static svdeltacallback_t g_global_svdeltacallback;
static char localmodels[512][5];


/*** CVARS ***/

static cvar_t sv_contact = { "sv_contact", "", 4, 0, 0 };
static cvar_t sv_logrelay = { "sv_logrelay", "", 0, 0, 0 };
static cvar_t sv_proxies = { "sv_proxies", "1", 4, 0, 0 };
static cvar_t sv_newunit = { "sv_newunit", "0", 0, 0, 0 };
static cvar_t sv_clipmode = { "sv_clipmode", "0", 4, 0, 0 };
static cvar_t sv_airmove = { "sv_airmove", "1", 4, 0, 0 };
static cvar_t sv_logbans = { "sv_logbans", "0", 0, 0, 0 };
static cvar_t sv_logblocks = { "sv_logblocks", "0", 4, 0, 0 };
static cvar_t max_queries_sec = { "max_queries_sec", ".5", 0x24, 0, 0 };
static cvar_t max_queries_sec_global = { "max_queries_sec_global", "10", 0x24, 0, 0 };
static cvar_t max_queries_window = { "max_queries_window", "30", 0x24, 0, 0 };
static cvar_t showtriggers = { "showtriggers", "0", 4, 0.0, 0 }; //added.  This seems to make HL crash in certain circumstances (the client), and did NOT do what I would've expected.  Perhaps a modder will know what it's for; I for one see nothing useful.

static cvar_t cvar_sv_cheats  = {  "sv_cheats", "0", FCVAR_SERVER, 0.0, NULL };

/*** FUNCTIONS ***/


//delta callback nonsense
/* There's nothing I can say about what these functions are
// for.  SV_InvokeCallback is called INDIRECTLY through WriteDelta.
// The 'data' being passed appears to all be integers (and one pointer),
// but what it represents is unknown...
*/
void SV_InvokeCallback() {

   SV_WriteDeltaHeader(g_global_svdeltacallback.padding04, g_global_svdeltacallback.padding08, g_global_svdeltacallback.padding0C, g_global_svdeltacallback.padding00,
                       g_global_svdeltacallback.padding10, g_global_svdeltacallback.padding14, g_global_svdeltacallback.padding18, g_global_svdeltacallback.padding1C);
}
void SV_SetCallback(uint32 arg_0, uint32 arg_4, uint32 arg_8, uint32 * arg_C, uint32 arg_10, uint32 arg_14) {

   g_global_svdeltacallback.padding04 = arg_0;
   g_global_svdeltacallback.padding08 = arg_4;
   g_global_svdeltacallback.padding0C = arg_8;
   g_global_svdeltacallback.padding00 = arg_C;
   g_global_svdeltacallback.padding10 = 0;
   g_global_svdeltacallback.padding14 = 0;
   g_global_svdeltacallback.padding18 = arg_10;
   g_global_svdeltacallback.padding1C = arg_14;
}
void SV_SetNewInfo(uint32 arg_0) {

   g_global_svdeltacallback.padding10 = 1;
   g_global_svdeltacallback.padding14 = arg_0;
}


//pas pvs
static int fatbytes;
static int fatpasbytes;
static unsigned char fatpvs[0x400];
static unsigned char fatpas[0x400];

void SV_AddToFatPVS(vec3_t org, mnode_t * rootnode) {

   int i;
   double var_10;
   byte * var_8;
   mplane_t   * plane;


   while(rootnode->contents >= 0) {

      plane = rootnode->plane;
      var_10 = (plane->normal[0] * org[0]) +
               (plane->normal[1] * org[1]) +
               (plane->normal[2] * org[2]) - plane->dist;

      if(var_10 > 8) {
         rootnode = rootnode->children[0];
      }
      else if(var_10 < -8) {
         rootnode = rootnode->children[1];
      }
      else {

         SV_AddToFatPVS(org, rootnode->children[0]);
         rootnode = rootnode->children[1];
      }
   }

   if(rootnode->contents == -2) { return; }

   var_8 = Mod_LeafPVS((mleaf_t *)rootnode, global_sv.worldmodel);
   for(i = 0; i < fatbytes; i++) {
      fatpvs[i] |= var_8[i];
   }
}
void SV_AddToFatPAS(vec3_t org, mnode_t * rootnode) {

   int i;
   double var_10;
   byte * var_8;
   mplane_t   * plane;


   while(rootnode->contents >= 0) {

      plane = rootnode->plane;
      var_10 = (plane->normal[0] * org[0]) +
               (plane->normal[1] * org[1]) +
               (plane->normal[2] * org[2]) - plane->dist;

      if(var_10 > 8) {
         rootnode = rootnode->children[0];
      }
      else if(var_10 < -8) {
         rootnode = rootnode->children[1];
      }
      else {

         SV_AddToFatPAS(org, rootnode->children[0]);
         rootnode = rootnode->children[1];
      }
   }

   if(rootnode->contents == -1) { return; }

   var_8 = CM_LeafPAS(((mleaf_t *)rootnode) - global_sv.worldmodel->leafs);
   for(i = 0; i < fatpasbytes; i++) {
      fatpas[i] |= var_8[i];
   }
}
HLDS_DLLEXPORT unsigned char * SV_FatPVS(vec3_t org) {

   //Count bytes.  Align to 32 bit boundary.
   fatbytes = (global_sv.worldmodel->numleafs + 31) / 8;
   Q_memset(fatpvs, 0, fatbytes);

   SV_AddToFatPVS(org, global_sv.worldmodel->nodes);

   return(fatpvs);
}
HLDS_DLLEXPORT unsigned char * SV_FatPAS(vec3_t org) {

   fatpasbytes = (global_sv.worldmodel->numleafs + 31) / 8;
   Q_memset(fatpas, 0, fatpasbytes);

   SV_AddToFatPAS(org, global_sv.worldmodel->nodes);

   return(fatpas);
}



//assorted

void SV_AllocClientFrames() {

   unsigned int i;
   client_t * cl;


   for(i = 0, cl = global_svs.clients; i < global_svs.total_client_slots; i++, cl++) {

      if(cl->padding3480_frames != NULL) {
         Con_Printf("%s: Client frames already allocted.", __FUNCTION__);
         SV_ClearFrames(&(cl->padding3480_frames));
      }

      cl->padding3480_frames = Q_Malloc(sizeof(client_frame_t) * SV_UPDATE_BACKUP);
      CHECK_MEMORY_MALLOC(cl->padding3480_frames);
      Q_memset(cl->padding3480_frames, 0, sizeof(client_frame_t) * SV_UPDATE_BACKUP);
   }
}
/* DESCRIPTION: SV_ClearFrames
// LOCATION:
// PATH: lots
//
// Frees memory.  Client frames are malloc'd in groups of eight, it seems.
*/
void SV_ClearFrames(client_frame_t ** arg_0) {

   unsigned int i;
   client_frame_t * ptr;


   if(*arg_0 == NULL) { return; }

   for(i = 0, ptr = *arg_0; i < SV_UPDATE_BACKUP; i++, ptr++) {

      SV_ClearPacketEntities(ptr);
      //ptr->sent_time = 0; //Why do this?  We are freeing the memory...
      //ptr->ping_time = -1;
   }

   Q_Free(*arg_0);
   *arg_0 = NULL;
}

/* DESCRIPTION: SV_AllocPacketEntities
// LOCATION:
// PATH: SV_WriteEntitiesToClient
//
// Mallocs data.  Whoopee.
*/
void SV_AllocPacketEntities(client_frame_t * arg0, int count) {

   packet_entities_t * ptr;


   if(arg0 == NULL) { Con_Printf("%s: Passed NUL pointer\n", __FUNCTION__); }
   else {

      ptr = &(arg0->entities); //inline..

      if(ptr->entities != NULL) {
         SV_ClearPacketEntities(arg0);
      }

      if(count < 1) { count = 1; }

      ptr->entities = Q_Malloc(sizeof(entity_state_t) * count);
      CHECK_MEMORY_MALLOC(ptr->entities);

      Q_memset(ptr->entities, 0, sizeof(entity_state_t) * count);
      ptr->max_entities = count; //It's possible to mis-assign this var as count BEFORE a negative count is bumped to one.  At least, I assume it's an error...
   }
}
/* DESCRIPTION: SV_ClearPacketEntities
// LOCATION:
// PATH: Three calls
//
// Frees some memory
*/
void SV_ClearPacketEntities(client_frame_t * arg0) {

   packet_entities_t * ptr;


   if(arg0 == NULL) { Con_Printf("SV_ClearPacketEntities: Passed NULL pointer\n"); }
   else {
      ptr = &(arg0->entities); //inline...
      if(ptr != NULL) {

         if(ptr->entities != NULL) {
            Q_Free(ptr->entities);
         }
         ptr->entities = NULL;
         ptr->max_entities = 0;
      }
   }
}

/* DESCRIPTION: SV_ModelIndex
// LOCATION: sv_init.c
// PATH: PF_modelindex and PF_makestatic_I
//
// Gets the index of a model.  Unlike the other 'index' functions, this
// one's functionality seems a bit less obvious, and a bit more broken.
// Still, it's simple enough.
*/
int SV_ModelIndex(char * name) {

   int i;

   if(name == NULL || name[0] == '\0') { return(0); }

   for(i = 0; i < MAX_MODELS && global_sv.model_precache[i] != NULL; i++) { //if there is a max model var in global_svs, use that.
      if(Q_strcasecmp(global_sv.model_precache[i], name) == 0) {
         return(i);
      }
   }

   //Didnae find it.
   if(i == MAX_MODELS || global_sv.model_precache[i] != NULL) {
      Sys_Error("%s: model %s not precached", __FUNCTION__, name);
   }

   //I don't see how we can possibly get to this segment.
   return(i);
}

void SV_DeallocateDynamicData() {

   if(g_moved_edict != NULL) {
      Q_Free(g_moved_edict);
   }
      if(g_moved_from != NULL) {
      Q_Free(g_moved_from);
   }

   g_moved_edict = NULL;
   g_moved_from = NULL;
}
void SV_ReallocateDynamicData() {

   unsigned int i;

   i = sizeof(void *) * global_sv.max_edicts; //Number of bytes that x pointers will take.

   if(i == 0) {

      Con_Printf("SV_ReallocateDynamicData with global_sv.max_edicts == 0.\n");
   }
   else {

      if(g_moved_edict != NULL) {
         Con_Printf("Reallocate on moved_edict.\n"); //Do we leak the memory, or free it, all but guaranteeing a crash?
      }
      if(g_moved_from != NULL) {
         Con_Printf("Reallocate on moved_from.\n");
      }

      g_moved_edict = Q_Malloc(i);
      Q_memset(g_moved_edict, 0, i);

      i *= 3; //Each pointer takes four bytes.  Each pointer needs a corresponding 12 byte vec3_t.

      g_moved_from = Q_Malloc(i);
      Q_memset(g_moved_from, 0, i);
   }
}

/* DESCRIPTION: SV_CountPlayers
// LOCATION: zqtp.c global_sv_mvd.c
// PATH: a few
//
// Totals the player count.
*/
void SV_CountPlayers(int * count) {

   unsigned int i;
   client_t * clients;

   clients = global_svs.clients;
   *count = 0;

   for(i = 0; i < global_svs.allocated_client_slots; i++, clients++) {
      if(clients->padding00_used != 0 || clients->padding04 != 0 || clients->padding0C_connected != 0) { (*count)++; }
   }
}
int SV_CountPlayersReturnInt() {

   unsigned int i;
   int count;
   client_t * clients;

   clients = global_svs.clients;
   count = 0;
   for(i = 0; i < global_svs.allocated_client_slots; i++, clients++) {
      if(clients->padding00_used != 0 || clients->padding04 != 0 || clients->padding0C_connected != 0) { (count)++; }
   }
   return(count);
}


//init related
void SV_SetMaxclients() {

   unsigned int i;
   client_t * client;


   for(i = 0, client = global_svs.clients; i < global_svs.total_client_slots; i++, client++) {
      SV_ClearFrames(&(client->padding3480_frames));
   }

   global_svs.allocated_client_slots = Q_atoi(COM_GetParmAfterParmCase("-maxplayers"));

   if(global_svs.allocated_client_slots < 1 || global_svs.allocated_client_slots > 32) {
      global_svs.allocated_client_slots = 16;
   }

   global_cls.state = ca_dedicated;
   global_svs.total_client_slots = 32;

   SV_UPDATE_MASK = 63;
   SV_UPDATE_BACKUP = 64;

   //Shouldn't we free to this point or something?
   global_svs.clients = Hunk_AllocName(sizeof(client_t) * global_svs.total_client_slots, "clients");

   for(i = 0, client = global_svs.clients; i < global_svs.total_client_slots; i++, client++) {
      Q_memset(client, 0, sizeof(client_t));
      client->resource4CA0.pNext = client->resource4CA0.pPrev = &client->resource4CA0;
      client->resource4C18.pNext = client->resource4C18.pPrev = &client->resource4C18;
   }

   Cvar_DirectSetValue(&cvar_deathmatch, 1);
   SV_AllocClientFrames();
}

void SV_GetPlayerHulls() {

   int i;

   for(i = 0; i < 4; i++) {
      if(gEntityInterface.pfnGetHullBounds(i, global_player_mins[i], global_player_maxs[i]) == 0) {
         return;
      }
   }
}
/* DESCRIPTION: SV_LoadEntities
// LOCATION:
// PATH: Various functions that deal with moading a new map.
//
// Seems to be a 'simple' ED_LoadFromFile wrapper.
*/
void SV_LoadEntities() {

   ED_LoadFromFile(global_sv.worldmodel->entities);
}
void SV_ActivateServer(int arg_0) {

   unsigned int i;
   char data[0x10000];
   sizebuf_t buffer;
   gpnewusermsg_struct_t * temp;

   buffer.debugname = "Activate";
   buffer.overflow = 0;
   buffer.data = data;
   buffer.cursize = 0;
   buffer.maxsize = sizeof(data);

   //g_bIsCStrike = 0;
   Cvar_Set("sv_newunit", "0");
   gEntityInterface.pfnServerActivate(global_sv.edicts, global_sv.num_edicts, global_svs.allocated_client_slots);
   SV_CreateGenericResources();

   global_sv.active = 1;
   global_sv.padding3bc64 = 2;

   if(arg_0 == 0) {

      host_frametime = 0.001;
      SV_Physics();
   }
   else {

      host_frametime = 0.8;
      for(i = 0; i < 16; i++) {
         SV_Physics();
      }

      /* if singleplayer do this instead
      host_frametime = 0.1;
      SV_Physics();
      SV_Physics();
      */
   }

   SV_CreateBaseline();
   SV_CreateResourceList();
   global_sv.padding30144_SomeTotalValue = SV_TransferConsistencyInfo();

   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {

      if((global_host_client->padding00_used == 0 && global_host_client->padding0C_connected == 0) || global_host_client->padding2438_IsFakeClient != 0) { continue; }

      Netchan_Clear(&(global_host_client->netchan1C));
      SV_BuildReconnect(&(global_host_client->netchan1C.netchan_message));
      Netchan_Transmit(&(global_host_client->netchan1C), 0, NULL);

      if(global_sv_gpUserMsg != NULL) {

         temp = global_sv_gpNewUserMsg;
         global_sv_gpNewUserMsg = global_sv_gpUserMsg;

         SV_SendUserReg(&buffer);

         global_sv_gpNewUserMsg = temp;
      }

      global_host_client->padding14 = 1;

      Netchan_CreateFragments(1, &(global_host_client->netchan1C), &buffer);
      Netchan_FragSend(&(global_host_client->netchan1C));
      SZ_Clear(&buffer);
   }

   HPAK_FlushHostQueue();

   //There's a gamedir check here that sees if we're running CS.
   //A bool is set if we are.  Distasteful, but such a hack might be useful
   //for many many poorly coded mods...

   Con_Printf("Server started.\n");
}
int SV_SpawnServer(int arg_0_unused, const char * mapname, const char * arg_8) {

   unsigned int i;
   char map[0x40];
   char dll[0x40];
   const char * hostname;
   client_t * clients;



   if(global_sv.active != 0) { //and we're multiplayer, which we are
      for(i = 0, clients = global_svs.clients; i < global_svs.allocated_client_slots; i++, clients++) {

         //Shoul we zero some of these out?
         if((clients->padding00_used == 0 && clients->padding0C_connected == 0 && clients->padding04 == 0) ||
            clients->padding4A84 == NULL || clients->padding4A84->free != 0) { continue; }

         if(clients->padding4A84->pvPrivateData != NULL) {

            gEntityInterface.pfnClientDisconnect(clients->padding4A84);
         }
      }
   }


   NET_Config(1); //interesting, the argument signals multiplayerstate, not shutdown/startup.

   hostname = cvar_hostname.string;
   if(hostname == NULL || hostname[0] == '\0') { //Canna have empty server names now.

      Cvar_DirectSet(&cvar_hostname, "OSHLDS");
   }

   global_svs.ServerCount++;
   if(cvar_coop.value != 0) {

      Cvar_DirectSetValue(&cvar_deathmatch, 0);
   }

   HPAK_CheckSize("custom");

   Q_snprintf(map, sizeof(map)-1, "%s", global_sv.name);
   map[sizeof(map)-1] = '\0';

   Host_ClearMemory();
   for(i = 0, clients = global_svs.clients; i < global_svs.total_client_slots; i++, clients++) {
      SV_ClearFrames(&(clients->padding3480_frames));
   }

   //If non dedicated, use more update frames.
   //Dammit, I thought those never changed.  I sure hope I didn't miss any...
   //Though it works with eight.  I could just leave it if this turns ugly.
   SV_UPDATE_MASK = 63;
   SV_UPDATE_BACKUP = 64;

   SV_AllocClientFrames();
   Q_memset(&global_sv, 0, sizeof(server_t)); //we already do this in host_clearmemory
   Q_strcpy(global_sv.name2, map);
   Q_strcpy(global_sv.name, map);

   if(arg_8 == NULL) {
      //was already zerod out with memset
   }
   else {
      Q_strncpy(global_sv.name3, arg_8, 0x3F);
      global_sv.name3[0x3F] = '\0';
   }

   //This always seemed arbitrary to me.  But whatever.
   //Bet I could set it to zero and not have to deal with this crap.
   global_pr_strings = "";
   gGlobalVariables.pStringBase = global_pr_strings;


   global_sv.max_edicts = COM_EntsForPlayerSlots(global_svs.allocated_client_slots);
   SV_DeallocateDynamicData();
   SV_ReallocateDynamicData();


   gGlobalVariables.maxEntities = global_sv.max_edicts;

   global_sv.edicts = Hunk_AllocName(global_sv.max_edicts * sizeof(edict_t), "edicts");
   global_sv.edstates = Hunk_AllocName(global_sv.max_edicts * sizeof(entity_state_t), "baselines");


   global_sv.reliable_datagram.debugname = "Server Datagram";
   global_sv.reliable_datagram.data = global_sv.reliablebuffer;
   global_sv.reliable_datagram.maxsize = sizeof(global_sv.reliablebuffer);

   global_sv.sizebuf3CC1C.debugname = "Server Reliable Datagram";
   global_sv.sizebuf3CC1C.data = global_sv.OtherReliableBuffer;
   global_sv.sizebuf3CC1C.maxsize = sizeof(global_sv.OtherReliableBuffer);

   global_sv.sizebuf3dbd0_multicast.debugname = "Server Multicast Buffer";
   global_sv.sizebuf3dbd0_multicast.data = global_sv.MulticastBuffer;
   global_sv.sizebuf3dbd0_multicast.maxsize = sizeof(global_sv.MulticastBuffer);

   global_sv.spectator_datagram.debugname = "Server Spectator Buffer";
   global_sv.spectator_datagram.data = global_sv.SpectatorBuffer;
   global_sv.spectator_datagram.maxsize = sizeof(global_sv.SpectatorBuffer);

   global_sv.signon_datagram.debugname = "Server Signon Buffer";
   global_sv.signon_datagram.data = global_sv.SignonBuffer;
   global_sv.signon_datagram.maxsize = sizeof(global_sv.SignonBuffer);

   global_sv.num_edicts = global_svs.allocated_client_slots + 1;

   for(i = 0; i < global_svs.allocated_client_slots; i++) {
      global_svs.clients[i].padding4A84 = &(global_sv.edicts[i+1]);
   }

   gGlobalVariables.maxClients = global_svs.allocated_client_slots;
   gGlobalVariables.time = global_sv.time0c = 1.0;
   global_sv.padding3bc64 = 1;
   global_sv.padding04 = 0;

   Q_snprintf(global_sv.name4, sizeof(global_sv.name4)-1, "maps/%s.bsp", mapname);

   global_sv.worldmodel = Mod_ForName(global_sv.name4, 0, 0);
   if(global_sv.worldmodel == NULL) {

      Con_Printf("%s: Couldn't spawn server with map %s.\n", __FUNCTION__, global_sv.name4);
      global_sv.active = 0;
      return(0);
   }

   Q_strcpy(dll, "cl_dlls/client.dll");

   if(MD5_Hash_File(global_sv.usedpadding, dll, 0, NULL) == 0) {

      Con_Printf("%s: Couldn't CRC game DLL.\n", __FUNCTION__);
      global_sv.active = 0;
      return(0);
   }

   //SV_LoadSecurityModules I think we can skip this.

   CRC32_Init(&(global_sv.map_CRC));
   if(CRC_MapFile(&(global_sv.map_CRC), global_sv.name4) == 0) {

      Con_Printf("%s: Couldn't CRC map %s.\n", __FUNCTION__, global_sv.name4);
      global_sv.active = 0;
      return(0);
   }

   CM_CalcPAS(global_sv.worldmodel);
   global_sv.models[1] = global_sv.worldmodel;

   SV_ClearWorld();

   global_sv.sound_precache[0] = global_pr_strings;
   global_sv.model_precache[0] = global_pr_strings;
   global_sv.model_precache[1] = global_sv.name4;
   global_sv.padding31148[1] |= 1;
   global_sv.generic_precache[0] = global_pr_strings;

   for(i = 1; i < global_sv.worldmodel->numsubmodels; i++) {

      global_sv.model_precache[i+1] = localmodels[i];
      global_sv.models[i+1] = Mod_ForName(localmodels[i], 0, 0);
      global_sv.padding31148[i+1] |= 1;
   }


   Q_memset(&(global_sv.edicts[0].v), 0, sizeof(entvars_t));
   global_sv.edicts[0].free = 0;
   global_sv.edicts[0].v.model = ((char *)global_sv.worldmodel) - global_pr_strings; //Ya got me.
   global_sv.edicts[0].v.modelindex = 1;
   global_sv.edicts[0].v.movetype = MOVETYPE_PUSH;
   global_sv.edicts[0].v.solid = SOLID_BSP;

   if(cvar_coop.value != 0) {
      gGlobalVariables.coop = cvar_coop.value;
   }
   else {
      gGlobalVariables.deathmatch = cvar_deathmatch.value;
   }

   gGlobalVariables.mapname = global_sv.name - global_pr_strings;
   gGlobalVariables.startspot = global_sv.name3 - global_pr_strings;
   gGlobalVariables.serverflags = global_svs.padding14;


   //allow_cheats is altered here.  Ignored for now.
   SV_SetMoveVars();

   return(1);
}



//shutdown related
void SV_ClearEntities() {

   unsigned int i;


   for(i = 0; i < global_sv.num_edicts; i++) {
      if(global_sv.edicts[i].free == 0) {

         ReleaseEntityDLLFields(&(global_sv.edicts[i]));
      }
   }
}
void SV_ClearCaches() {

   unsigned int i;


   for(i = 1; i < 0x100; i++) {

      if(global_sv.padding31348[i].str == NULL) { return; }

      global_sv.padding31348[i].str = NULL;
      if(global_sv.padding31348[i].mem != NULL) {

         Q_Free(global_sv.padding31348[i].mem);
         global_sv.padding31348[i].mem = NULL;
      }
   }
}
void SV_ClearClientStates() {

   unsigned int i;
   client_t * cl;


   for(i = 0, cl = global_svs.clients; i < global_svs.allocated_client_slots; i++, cl++) {

      COM_ClearCustomizationList(&(cl->padding4D30));
      SV_ClearResourceLists(cl);
   }
}
//obviously, this does not belong here.  Or does it, but with an SV tag?
void Host_ClearClients() {

   unsigned int i, j;
   netadr_t address;
   client_frame_t * var_C;


   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {

      if(global_host_client->padding3480_frames != NULL) {
         for(j = 0; j < SV_UPDATE_BACKUP; j++) {

            var_C = &(global_host_client->padding3480_frames[j]);
            SV_ClearPacketEntities(var_C);
            var_C->sent_time = 0;
            var_C->ping_time = -1;
         }
      }

      if(global_host_client->netchan1C.remote_address.type != NA_UNUSED) {

         address = global_host_client->netchan1C.remote_address;
         Q_memset(&(global_host_client->netchan1C), 0, sizeof(netchan_t));

         //I think i would work in place of the subtraction.
         Netchan_Setup(1, &(global_host_client->netchan1C), address, global_host_client - global_svs.clients, global_host_client, SV_GetFragmentSize);

         COM_ClearCustomizationList(&(global_host_client->padding4D30));
      }
   }

   for(i = 0, global_host_client = global_svs.clients; i < global_svs.total_client_slots; i++, global_host_client++) {

      SV_ClearFrames(&(global_host_client->padding3480_frames));
   }

   Q_memset(global_svs.clients, 0, sizeof(client_t) * global_svs.total_client_slots);
   SV_AllocClientFrames();
}
void SV_InactivateClients() {

   unsigned int i;
   client_t * ptrClients;

   i = 0;
   ptrClients = global_svs.clients;

   while(i < global_svs.allocated_client_slots) {

      if(ptrClients->padding00_used != 0 || ptrClients->padding0C_connected != 0 || ptrClients->padding04 != 0) {

         if(ptrClients->padding2438_IsFakeClient == 0) {

            ptrClients->padding00_used = 0;
            ptrClients->padding04 = 0;
            ptrClients->padding08 = 0;
            ptrClients->padding0C_connected = 1;

            SZ_Clear(&(ptrClients->netchan1C.netchan_message));
            SZ_Clear(&(ptrClients->sizebuf24AC));
            COM_ClearCustomizationList(&(ptrClients->padding4D30));
            Q_memset(&(ptrClients->UnknownStruct4DE0), 0, 0x100);
         }
         else {

            SV_DropClient(ptrClients, 0, "Dropping fake client on level change");
         }
      }

      i++;
      ptrClients++;
   }
}
void SV_ServerShutdown() {

   gGlobalVariables.time = global_sv.time0c;
   if(global_svs.pad00 != 0 && global_sv.active != 0) {
      gEntityInterface.pfnServerDeactivate();
   }
}


//commands

//Dumps info on players.  I assume users is 'user' for all users, but there's
//an added call in global_sv_users that I decided not to mess with.
void SV_User_f() {

   client_t *client;
   int userID;
   unsigned int i;

   if(global_sv.active == 0) {

      Con_Printf("You can't query a user when the server isn't running, numbnuts.\n" );
      return;
   }
   if(Cmd_Argc() != 2) {

      Con_Printf("Usage: user <username or userid>\n");
      return;
   }

   //Usernames can have numbers in them.  I do not know if they can be
   //entirely numbers however--if so, this presents us with an unresolvable
   //interpretation problem that 'll just have to deal with later...
   userID = atoi(Cmd_Argv(1));
   client = global_svs.clients;

   for(i = 0; i < global_svs.allocated_client_slots; i++, client++) {

      //ignore empty slots, and fake clients.
      if(client->PlayerName[0] == '\0' || client->padding2438_IsFakeClient != 0 ||
        (client->padding00_used == 0 && client->padding04 == 0 && client->padding0C_connected == 0)) { continue; }

      if(client->padding4A8C_PlayerUID == userID || Q_strcmp(client->PlayerName, Cmd_Argv(1)) == 0) {
         Info_Print(client->InfoKeyBuffer);
         return;
      }
   }

   Con_Printf("SV_User_f: Couldn't find the specified user.\n");
}
void SV_Users_f() {

   client_t *client;
   int userCount;
   unsigned int i;

   if(global_sv.active == 0) {

      Con_Printf("'Users' doesn't work well when there ARE none...\n" );
      return;
   }

   Con_Printf("userid : uniqueid : name\n");
   Con_Printf("------ : -------- : ----\n");

   userCount = 0;
   client = global_svs.clients;
   for(i = 0; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->PlayerName[0] == '\0' || client->padding2438_IsFakeClient != 0 ||
        (client->padding00_used == 0 && client->padding04 == 0 && client->padding0C_connected == 0)) { continue; }

      Con_Printf("%6i : %s : %s\n", client->padding4A8C_PlayerUID, SV_GetClientIDString(client), client->PlayerName);
      userCount++;
   }

   Con_Printf("%i users\n", userCount);
}

/* DESCRIPTION: SV_New_f
// LOCATION: sv_user.c
// PATH: Whenever a client connects
//
//
*/
void SV_New_f() {

   edict_t * edictPtr;
   gpnewusermsg_struct_t * ptrUserMsgs;

   client_t * ptrClient;
   unsigned int i;

   char BigArray[0x10000];
   sizebuf_t NewConnection;

   char Name[0x40];
   char Address[0x100];
   char RejectReason[0x80];

   if(global_cmd_source == cmd_SERVER ||
     (global_host_client->padding04 != 0 && global_host_client->padding00_used == 0)) {
      return;
   }

   NewConnection.debugname = "New Connection";
   NewConnection.overflow = 0;
   NewConnection.data = BigArray;
   NewConnection.maxsize = 0x10000;
   NewConnection.cursize = 0;

   edictPtr = global_host_client->padding4A84;
   global_host_client->padding0C_connected = 1; //aHA!  It's a bool for connected, most definitely.
   global_host_client->padding3460_connection_started = global_realtime;

   SZ_Clear(&(global_host_client->netchan1C.netchan_message));
   SZ_Clear(&(global_host_client->sizebuf24AC));
   Netchan_Clear(&(global_host_client->netchan1C));

   SV_SendServerinfo(&NewConnection, global_host_client);

   if(global_sv_gpUserMsg != NULL) {

      ptrUserMsgs = global_sv_gpNewUserMsg;
      global_sv_gpNewUserMsg = global_sv_gpUserMsg;

      SV_SendUserReg(&NewConnection); //inline in AMD

      global_sv_gpNewUserMsg = ptrUserMsgs;
   }

   global_host_client->padding14 = 1;
   if(global_host_client->padding4A84 != NULL &&
     (global_host_client->padding00_used != 0 || global_host_client->padding04 != 0)) {

      gEntityInterface.pfnClientDisconnect(global_host_client->padding4A84);
   }


   Q_snprintf(Name, sizeof(Name) - 1, "%s", global_host_client->PlayerName);
   Name[sizeof(Name) - 1] = '\0';
   Q_snprintf(Address, sizeof(Address) - 1, "%s", NET_AdrToString(global_host_client->netchan1C.remote_address));
   Address[sizeof(Address) - 1] = '\0';
   Q_snprintf(RejectReason, sizeof(RejectReason) - 1, "Connection rejected by game\n");
   RejectReason[sizeof(RejectReason) - 1] = '\0';

   if(gEntityInterface.pfnClientConnect(edictPtr, Name, Address, RejectReason) == 0) { //failed for some reason, or perhaps the mod didn't like that person

      MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), 0x09);
      //eh, beats the va()
      MSG_WriteString(&(global_host_client->netchan1C.netchan_message), "echo ");
      MSG_WriteString(&(global_host_client->netchan1C.netchan_message), RejectReason);
      MSG_WriteString(&(global_host_client->netchan1C.netchan_message), "\n");

      SV_DropClient(global_host_client, 0, "%s(%s) denied: %s", Name, Address, RejectReason);
   }
   else {


      #define __SVFULSERVERINFO_1 "fullserverinfo \""
      #define __SVFULSERVERINFO_2 "\"\n"

      MSG_WriteByte(&NewConnection, 0x09);
      MSG_WriteBuf(&NewConnection, sizeof(__SVFULSERVERINFO_1)-1, __SVFULSERVERINFO_1);//problem? fullserverinfo "%s"\n
      MSG_WriteString(&NewConnection, Info_Serverinfo());
      NewConnection.cursize--; //To eliminate the null.  This is probably bad practice though.
      MSG_WriteString(&NewConnection, __SVFULSERVERINFO_2);


      for(i = 0, ptrClient = global_svs.clients; i < global_svs.allocated_client_slots; i++, ptrClient++) {

         if(global_host_client == ptrClient || ptrClient->padding00_used != 0 ||
            ptrClient->padding04 != 0 || ptrClient->padding0C_connected != 0) {

            SV_FullClientUpdate(ptrClient, &NewConnection);
         }
      }
      Netchan_CreateFragments(1, &(global_host_client->netchan1C), &NewConnection);
      Netchan_FragSend(&(global_host_client->netchan1C));
   }
}
void SV_FullUpdate_f() {

   if(global_cmd_source == cmd_SERVER) {
      Cmd_ForwardToServer();
   }
   else {

      SV_ForceFullClientsUpdate();
      gEntityInterface.pfnClientCommand(global_sv_player);
   }
}

/* DESCRIPTION: SV_Spawn_f
// LOCATION:
// PATH: requested via console.
//
// This appears to init a new client if needed, or 'spawn' a older one.
// Whatever path it takes, the dirty work isn't here.
*/
void SV_Spawn_f() {

   char BigArray[0x10000];
   sizebuf_t Connection;


   if(Cmd_Argc() != 3) {
      Con_Printf("%s: spawn is not valid\n", __FUNCTION__);
      return;
   }


   Connection.debugname = "Spawning";
   Connection.overflow = 0;
   Connection.data = BigArray;
   Connection.maxsize = 0x10000;
   Connection.cursize = 0;

   global_host_client->local_map_CRC = Q_atoi(Cmd_Argv(2));
   COM_UnMunge2((char *)(&global_host_client->local_map_CRC), 4, 0xFF - global_svs.ServerCount);


   if(global_cmd_source != cmd_CLIENT) { //Is there a reason we don't run this check first thing?
      Con_Printf("%s: spawn is not valid from the console\n", __FUNCTION__);
      return;
   }


   if(global_cls.demoplayback == 0) {

      if((unsigned)Q_atoi(Cmd_Argv(1)) != global_svs.ServerCount) {
         SV_New_f();
         return;
      }
   }

   SZ_Write(&Connection, global_sv.signon_datagram.data, global_sv.signon_datagram.cursize);
   SV_WriteSpawn(&Connection);

   Netchan_CreateFragments(1, &(global_host_client->netchan1C), &Connection);
   Netchan_FragSend(&(global_host_client->netchan1C));
}

void SV_Drop_f() {

   if(global_cmd_source == cmd_SERVER) {
      return;
   }
   else {

      SV_BroadcastPrintf("%s has dropped\n", global_host_client->PlayerName);
      SV_DropClient(global_host_client, 0, "%s issued 'drop' command", global_host_client->PlayerName);
   }
}

void SV_SendEnts_f() {

   if(global_cmd_source == cmd_SERVER) { return; }
   if(global_host_client->padding00_used == 0 || global_host_client->padding04 == 0 || global_host_client->padding0C_connected == 0) { return; }

   global_host_client->padding08 = cmd_SERVER;
}

void SV_SendRes_f() {

   sizebuf_t sz;
   byte buffer[0x10000];


   if(global_cmd_source == 1) { return; }
   if(global_host_client->padding00_used == 0 && global_host_client->padding04 != 0) { return; }

   sz.debugname = "SendResources";
   sz.overflow = 0;
   sz.data = buffer;
   sz.maxsize = sizeof(buffer);
   sz.cursize = 0;

   SV_SendResources(&sz);
   Netchan_CreateFragments(1, &(global_host_client->netchan1C), &sz);
   Netchan_FragSend(&(global_host_client->netchan1C));
}



/*** SV ***/

void SV_BanId_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_RemoveId_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_ListId_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_WriteId_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_ResetRcon_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_SetLogAddress_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_AddLogAddress_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_DelLogAddress_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_ServerLog_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_Serverinfo_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_Localinfo_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_ShowServerinfo_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_BeginFileDownload_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_AddIP_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_ListIP_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_WriteIP_f() { printf("%s called", __FUNCTION__); exit(99); }
void SV_RemoveIP_f() { printf("%s called", __FUNCTION__); exit(99); }


void SV_Init() {

   unsigned int i;
   client_t * client;

   SV_Movevars_Init();
   SV_User_Init();
   SV_Frame_Init();
   SV_Packet_Init();
   SV_ConnectionlessPacket_Init();
   SV_CL_RES_Init();
   PF_Init();

   Cmd_AddCommand("banid", SV_BanId_f);
   Cmd_AddCommand("removeid", SV_RemoveId_f);
   Cmd_AddCommand("listid", SV_ListId_f);
   Cmd_AddCommand("writeid", SV_WriteId_f);
   Cmd_AddCommand("resetrcon", SV_ResetRcon_f);
   Cmd_AddCommand("logaddress", SV_SetLogAddress_f);
   Cmd_AddCommand("logaddress_add", SV_AddLogAddress_f);
   Cmd_AddCommand("logaddress_del", SV_DelLogAddress_f);
   Cmd_AddCommand("log", SV_ServerLog_f);
   Cmd_AddCommand("serverinfo", SV_Serverinfo_f);
   Cmd_AddCommand("localinfo", SV_Localinfo_f);
   Cmd_AddCommand("showinfo", SV_ShowServerinfo_f);
   Cmd_AddCommand("user", SV_User_f);
   Cmd_AddCommand("users", SV_Users_f);
   Cmd_AddCommand("dlfile", SV_BeginFileDownload_f);
   Cmd_AddCommand("addip", SV_AddIP_f);
   Cmd_AddCommand("removeip", SV_RemoveIP_f);
   Cmd_AddCommand("listip", SV_ListIP_f);
   Cmd_AddCommand("writeip", SV_WriteIP_f);
   Cmd_AddCommand("dropclient", SV_Drop_f);
   Cmd_AddCommand("spawn", SV_Spawn_f);
   Cmd_AddCommand("new", SV_New_f);
   Cmd_AddCommand("sendres", SV_SendRes_f);
   Cmd_AddCommand("sendents", SV_SendEnts_f);
   Cmd_AddCommand("fullupdate", SV_FullUpdate_f);

   Cvar_RegisterVariable(&sv_contact);
   Cvar_RegisterVariable(&sv_logrelay);
   Cvar_RegisterVariable(&sv_proxies);
   Cvar_RegisterVariable(&sv_newunit);
   Cvar_RegisterVariable(&sv_clipmode);
   Cvar_RegisterVariable(&sv_airmove);
   Cvar_RegisterVariable(&sv_logbans);
   Cvar_RegisterVariable(&sv_logblocks);
   Cvar_RegisterVariable(&max_queries_sec);
   Cvar_RegisterVariable(&max_queries_sec_global);
   Cvar_RegisterVariable(&max_queries_window);
   Cvar_RegisterVariable(&showtriggers);
   Cvar_RegisterVariable(&cvar_sv_cheats);

   if(COM_CheckParm("-dev") != 0) {
      Cvar_DirectSetValue(&cvar_sv_cheats, 1);
   }


   for(i = 0; i < 0x200; i++) {
      sprintf(localmodels[i], "*%i", i);
   }

   Q_memset(&(global_svs.module80[0]), 0, sizeof(security_module_t));
   Q_memset(&(global_svs.module80[1]), 0, sizeof(security_module_t));
   Q_memset(&(global_svs.module80[2]), 0, sizeof(security_module_t));
   Q_memset(&(global_svs.module80[3]), 0, sizeof(security_module_t));

   global_svs.paddingE0 = 0;
   global_svs.paddingE4 = 0;
   global_svs.paddingE8 = 0;

   for(i = 0, client = global_svs.clients; i < global_svs.total_client_slots; i++, client++) {

      SV_ClearFrames(&(client->padding3480_frames));
      Q_memset(client, 0, sizeof(client_t));
      client->resource4CA0.pNext = client->resource4CA0.pPrev = &client->resource4CA0;
      client->resource4C18.pNext = client->resource4C18.pPrev = &client->resource4C18;
   }

   SV_RconInit();
   our_PM_Init(&global_svmove);

   SV_AllocClientFrames();
   SV_InitDeltas();

   global_svs.paddingEC_inittime = global_realtime;

   //SteamInitializeUserIDTicketValidator
}
