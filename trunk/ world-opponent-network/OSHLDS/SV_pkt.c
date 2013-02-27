#include "SV_pkt.h"

#include <ctype.h>
#include "cmd.h"
#include "cvar.h"
#include "delta.h"
#include "host.h"
#include "HPAK.h"
#include "Modding.h"
#include "pf.h"
#include "SV_hlds.h"
#include "SV_clres.h"
#include "SV_mcast.h"
#include "SV_phys.h"
#include "SV_pktcl.h"
#include "SV_user.h"
#include "sys.h"


/*** GLOBALS ***/
static int global_sv_playermodel;
static char com_clientfallback[0x104];
static netadr_t sUpdateIPAddress; //This one's usage is a bit of a mystery.
static int allow_cheats = 0;
static unsigned int sv_lastnum = 0;
static qboolean g_balreadymoved = 0;
static const char * clcommands[] = { "status", "god", "notarget", "fly", "name",
                                     "noclip", "kill", "pause", "spawn", "new",
                                     "sendres", "dropclient", "kick", "ping",
                                     "dlfile", "nextdl", "setinfo", "showinfo",
                                     "sendents", "fullupdate", "setpause",
                                     "unpause", NULL };


static cvar_t cvar_sv_voicecodec        = {        "sv_voicecodec", "voice_miles",  FCVAR_NONE,                      0, NULL };
static cvar_t cvar_sv_voiceenable       = {       "sv_voiceenable",           "1", (FCVAR_ARCHIVE | FCVAR_SERVER), 1.0, NULL };
static cvar_t cvar_sv_instancedbaseline = { "sv_instancedbaseline",           "1",  FCVAR_NONE,                    1.0, NULL };
static cvar_t cvar_host_limitlocal      = {      "host_limitlocal",           "0",  FCVAR_NONE,                    0.0, NULL };
static cvar_t cvar_sv_failuretime       = {       "sv_failuretime",         "0.5",  FCVAR_NONE,                    0.5, NULL };

/*** FUNCTIONS ***/

//general

/* DESCRIPTION: SV_IsPlayerIndex
// LOCATION:
// PATH: SV_FindBestBaseline, SV_CreatePacketEntities, SV_CreateBaseline
//
// Quick, easy.  Return 1 if the given index points to a client slot.
*/
qboolean SV_IsPlayerIndex(int entindex) {

   if(entindex > 0 && (unsigned)entindex <= global_svs.allocated_client_slots) { return(1); }
   else { return(0); }
}
/* DESCRIPTION: SV_CleanupEnts
// LOCATION: global_sv_ents.c
// PATH: SV_SendClientMessages
//
// Cleans up ents... Too simple to really mess up :)
*/
void SV_CleanupEnts() {

   unsigned int e;
   edict_t *ent;

   for(e = 1; e <= global_sv.num_edicts; e++) {

      ent = &global_sv.edicts[e];
      ent->v.effects &= ~(EF_MUZZLEFLASH | EF_NOINTERP);
   }
}
/* DESCRIPTION: SV_FindBestBaseline
// LOCATION: Baselines are mentioned in QW.
// PATH: SV_CreatePacketEntities
//
// I'm pretty sure what this does is compare entities (in delta form).
// The first entity is a 'baseline', or a default state with a lot of typical
// values, and the second is the entity we'd like to write.
//
// We compare all of these baselines to the entity we'd like to write
// because whichever baseline it most closely resembles is the one we-d like to
// use.  It basically means less data over the wire for us.
*/
int SV_FindBestBaseline(unsigned int arg_0, entity_state_t ** arg_4, entity_state_t * arg_8, unsigned int arg_c, unsigned int arg_10) {

   int var_4, var_8, var_c, var_10;
   //var_4 & 8 == the testdelta 'score' (lower == better),
   //var_c = index of best match so far, var_10 = index of current test candidte (and a general var at the end)

   var_c = arg_0;
   var_10 = arg_0-1;


   if(arg_10 == 0) {
      if(SV_IsPlayerIndex(arg_c) == 0) {

         var_4 = DELTA_TestDelta((void *)*arg_4, (void *)&(arg_8[arg_0]), SV_LookupDelta("entity_state_t"));
      }
      else {

         var_4 = DELTA_TestDelta((void *)*arg_4, (void *)&(arg_8[arg_0]), SV_LookupDelta("entity_state_player_t"));
      }
   }
   else {

      var_4 = DELTA_TestDelta((void *)*arg_4, (void *)&(arg_8[arg_0]), SV_LookupDelta("custom_entity_state_t"));
   }

   var_4 -= 6;
   while(var_4 > 0 && var_10 >= 0 && arg_0 - var_10 <= 0x3E) {

      if(arg_8[arg_0].entityType == arg_8[var_10].entityType) {

         if(arg_10 == 0) {
            if(SV_IsPlayerIndex(arg_c) == 0) {
               var_8 = DELTA_TestDelta((void *)&(arg_8[var_10]), (void *)&(arg_8[arg_0]), SV_LookupDelta("entity_state_t"));
            }
            else {
               var_8 = DELTA_TestDelta((void *)&(arg_8[var_10]), (void *)&(arg_8[arg_0]), SV_LookupDelta("entity_state_player_t"));
            }
         }
         else {
            var_8 = DELTA_TestDelta((void *)&(arg_8[var_10]), (void *)&(arg_8[arg_0]), SV_LookupDelta("custom_entity_state_t"));
         }

         if(var_8 < var_4) {

            var_4 = var_8;
            var_c = var_10;
         }
      }
      var_10--;
   }

   var_10 = arg_0 - var_c;
   if(var_10 != 0) {

      *arg_4 = &(arg_8[var_c]);
   }

   return(var_10);

}
/* DESCRIPTION: SV_EstablishTimeBase
// LOCATION:
// PATH: SV_ParseMove
//
// Finangles latency and the like.  Too many floats exist in the client struct.
*/
void SV_EstablishTimeBase(client_t * cl, usercmd_t *ucmd, int drops, int ClientCommands1, int ClientCommands2) {

   double var_18;
   double var_10;
   int i;

   var_10 = 0;
   var_18 = host_frametime + global_sv.time0c;

   if(drops < 24) {

      while(drops > ClientCommands1) {
         var_10 += cl->padding2440.msec / 1000.0;
         drops--;
      }
      while(drops > 0) {

         var_10 += ucmd[drops + ClientCommands2 - 1].msec / 1000.0;
         drops--;
      }
   }

   for(i = ClientCommands2-1; i >= 0; i--) {

      var_10 += ucmd[i].msec / 1000.0;
   }

   cl->padding24A4 = var_18 - var_10;
}
HLDS_DLLEXPORT const char * PM_SV_TraceTexture(int ground, vec3_t vstart, vec3_t vend) {

   physent_t * var_4_physent;
   edict_t * var_8_ent;

   if(ground < 0 || ground >= global_pmove->numphysent) { return(NULL); }

   var_4_physent = &global_pmove->physents[ground];
   if(var_4_physent->model == NULL) { return(NULL); }
   if(var_4_physent->info < 0 || (unsigned)var_4_physent->info >= global_sv.max_edicts) { return(NULL); }

   var_8_ent = &(global_sv.edicts[var_4_physent->info]);

   return(TraceTexture(var_8_ent, vstart, vend));
}
/* DESCRIPTION: SV_UpdateToReliableMessages
// LOCATION: global_sv_send.c
// PATH: SV_SendClientMessages
//
// Funny name, but all it does (I think) is add 'reliable' data to the packet streams.
*/
void SV_UpdateToReliableMessages() {

   unsigned int i;
   client_t * client;
   gpnewusermsg_struct_t * var_1c;

   // check for changes to be sent over the reliable streams to all clients
   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {


      if(global_host_client->padding4A84 == NULL) { continue; }

      if(global_host_client->padding4BA4 != 0 && global_host_client->padding4BA8 <= global_realtime) {

         global_host_client->padding4BA4 = 0;
         global_host_client->padding4BA8 = global_realtime + 1;
         SV_ExtractFromUserinfo(global_host_client);
         SV_FullClientUpdate(global_host_client, &(global_sv.sizebuf3CC1C));
      }

      if(global_host_client->padding2438_IsFakeClient != 0) { continue; }
      if(global_host_client->padding00_used == 0 && global_host_client->padding0C_connected == 0) { continue; }

      if(global_sv_gpNewUserMsg != NULL) {

         SV_SendUserReg(&(global_host_client->netchan1C.netchan_message));
      }
   }

   if(global_sv_gpNewUserMsg != NULL) {
      if(global_sv_gpUserMsg != NULL) {

         var_1c = global_sv_gpUserMsg;
         while(var_1c->next != NULL) {
            var_1c = var_1c->next;
         }

         var_1c->next = global_sv_gpNewUserMsg;
      }

      else {
         global_sv_gpUserMsg = global_sv_gpNewUserMsg;
      }
      global_sv_gpNewUserMsg = NULL;
   }

   if(global_sv.reliable_datagram.overflow & 2) {

      Con_Printf("%s: Reliable datagram overflowed\n", __FUNCTION__);
      SZ_Clear(&(global_sv.reliable_datagram));
   }

   if(global_sv.spectator_datagram.overflow & 2) {
      Con_Printf("%s: Spectator datagram overflowed\n", __FUNCTION__);
      SZ_Clear(&(global_sv.spectator_datagram));
   }

   // append the broadcast messages to each client messages
   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0 || client->padding2438_IsFakeClient != 0) { continue; }

      if(client->netchan1C.netchan_message.cursize + global_sv.sizebuf3CC1C.cursize < client->netchan1C.netchan_message.maxsize) {
         SZ_Write(&(client->netchan1C.netchan_message), global_sv.sizebuf3CC1C.data, global_sv.sizebuf3CC1C.cursize);
      }
      else {
         Netchan_CreateFragments(1, &(client->netchan1C), &(global_sv.sizebuf3CC1C));
      }

      if(client->sizebuf24AC.cursize + global_sv.reliable_datagram.cursize < client->sizebuf24AC.maxsize) {
         SZ_Write(&(client->sizebuf24AC), global_sv.reliable_datagram.data, global_sv.reliable_datagram.cursize);
      }
      else {
         Con_Printf("%s: Ignoring unreliable datagram for %s, would overflow.\n", __FUNCTION__, client->PlayerName);
      }


      if(client->padding243C_proxy != 0 && client->sizebuf24AC.cursize + global_sv.spectator_datagram.cursize < client->sizebuf24AC.maxsize) {

         SZ_Write(&(client->sizebuf24AC), global_sv.spectator_datagram.data, global_sv.spectator_datagram.cursize);
      }
   }

   SZ_Clear(&(global_sv.reliable_datagram));
   SZ_Clear(&(global_sv.spectator_datagram));
   SZ_Clear(&(global_sv.sizebuf3CC1C));
}
void SV_ProcessFile(client_t * cl, const char * filename) {

   int i;
   char var_1C_hash[16];
   const char * ptrname;
   char * ptrhash;
   resource_t * ptr;
   customization_t * cptr;

   if(filename[0] != '!') {

      Con_Printf("%s: Ignoring file upload of %s.\n", __FUNCTION__, filename);
      return;
   }

   //hex converting
   ptrname = filename+4;
   ptrhash = var_1C_hash;
   for(i = 0; i < 16; i++, ptrname++, ptrhash++) {

      if(isxdigit(*ptrname) == 0 || isxdigit(*(ptrname+1)) == 0) {

         Con_Printf("%s: non hex character in file hash %s.\n", __FUNCTION__, filename);
         return;
      }

      if(isdigit(*ptrname) != 0) { *ptrhash = (*ptrname - '0'); }
      else { *ptrhash = (toupper(*ptrname) - 'A'); }
      ptrname++;

      if(isdigit(*ptrname) != 0) { *ptrhash |= (*ptrname - '0') << 4; }
      else { *ptrhash = (toupper(*ptrname) - 'A') << 4; }
   }

   ptr = cl->resource4CA0.pNext;
   while(1) {

      if(ptr == &(cl->resource4CA0)) {
         Con_Printf("%s: unrequested file %s received.\n", __FUNCTION__, filename);
         return;
      }

      if(Q_memcmp(var_1C_hash, ptr->rgucMD5_hash, 16) == 0) { break; }
      ptr = ptr->pNext;
   }

   //ptr now points to the correct resource.
   if(ptr->nDownloadSize != cl->netchan1C.TemporaryBufferSize) {
      Con_Printf("%s: downloaded file %s size and purported file size do not match.\n", __FUNCTION__, filename);
      return;
   }

   HPAK_AddLump(1, "custom.hpk", ptr, cl->netchan1C.TemporaryBuffer, NULL);

   ptr->ucFlags &= 0xFD;
   SV_MoveToOnHandList(ptr);

   for(cptr = cl->padding4D30.pNext; cptr != NULL; cptr = cptr->pNext) {

      if(Q_memcmp(ptr->rgucMD5_hash, cptr->resource.rgucMD5_hash, 16) == 0) {
         Con_Printf("%s: duplicate resource %s downloaded.\n", __FUNCTION__, filename);
         return;
      }
   }

   if(COM_CreateCustomization(&(cl->padding4D30), ptr, -1, 7, NULL, NULL) == 0) {

      Con_Printf("%s: Error parsing resource %s from %s.\n", __FUNCTION__, filename, cl->PlayerName);
   }
}
HLDS_DLLEXPORT void ClientPrintf(edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg) {

   unsigned int ClientSlot;
   client_t * ptrClient;


   ClientSlot = NUM_FOR_EDICT(pEdict) - 1; //valid values: 0->allocated_slots-1
   if(ClientSlot >= global_svs.allocated_client_slots) {

      Con_Printf("%s: tried to print to a non-client.\n", __FUNCTION__);
      return;
   }

   ptrClient = &(global_svs.clients[ClientSlot]);
   if(ptrClient->padding2438_IsFakeClient != 0) { return; } //Bots probably won't care...

   switch(ptype) {

   case print_console:
   case print_chat:
      MSG_WriteByte(&(ptrClient->netchan1C.netchan_message), 0x08); //Must go through and decipher all these codes.
      MSG_WriteString(&(ptrClient->netchan1C.netchan_message), szMsg);
      break;

   case print_center:
      MSG_WriteChar(&(ptrClient->netchan1C.netchan_message), 0x1A); //Must go through and decipher all these codes.
      MSG_WriteString(&(ptrClient->netchan1C.netchan_message), szMsg);
      break;

   default:

      Con_Printf("%s: invalid PRINT_TYPE %i.\n", __FUNCTION__, ptype);
   }
}



//incoming

/* DESCRIPTION: SV_ParseDelta
// LOCATION: sv_user.c (as part of SV_ExecuteClientMessage)
// PATH: SV_Frame->SV_ReadPackets->SV_ExecuteClientMessage
//
//
*/
void SV_ParseDelta() { //I think there's an unused client_t arg

   global_host_client->padding2434_delta_sequence = MSG_ReadByte();
}
int SV_ValidateClientCommand(char * command) {

   int i;


   COM_Parse(command);

   for(i = 0; clcommands[i] != NULL; i++) {
      if(Q_strcasecmp(global_com_token, clcommands[i]) == 0) { return(1); }
   }

   return(0);
}
/* DESCRIPTION: SV_ParseStringCommand
// LOCATION: sv_user.c (as part of SV_ExecuteClientMessage)
// PATH: SV_Frame->SV_ReadPackets->SV_ExecuteClientMessage
//
//
*/
void SV_ParseStringCommand() {

   char * str;


   str = MSG_ReadString();

   if(SV_ValidateClientCommand(str) == 1) {

      Cmd_ExecuteString(str, 0);
      return;
   }

   str[0x7F] = '\0'; //This is bad practice, but technically safe since the static array ReadString returned is much bigger.
   Cmd_TokenizeString(str);

   gEntityInterface.pfnClientCommand(global_sv_player);
}
/* DESCRIPTION: SV_ParseVoiceData
// LOCATION: sv_user.c (as part of SV_ExecuteClientMessage)
// PATH: SV_Frame->SV_ReadPackets->SV_ExecuteClientMessage
//
//
*/
void SV_ParseVoiceData(client_t *cl) {

   unsigned int i;
   unsigned int ReadBufLength, WriteBufLength;
   int SendingClientIndex, IsLocal;

   client_t *ReceivingClient;
   char voicebuffer[0x1000];


   if(cvar_sv_voiceenable.value == 0) { return; }

   ReadBufLength = MSG_ReadShort();
   if(ReadBufLength > sizeof(voicebuffer)) {
      return;
   }

   SendingClientIndex = cl - global_svs.clients;
   MSG_ReadBuf(ReadBufLength, voicebuffer);

   for(i = 0; i < global_svs.allocated_client_slots; i++) {

      ReceivingClient = &global_svs.clients[i];
      IsLocal = (ReceivingClient == cl);

      //will need revision when 32 is increased.
      if((cl->padding4EE4 & (1 << i)) == 0 && IsLocal == 0) { continue; }
      if(ReceivingClient->padding00_used == 0 && ReceivingClient->padding0C_connected && IsLocal == 0) { continue; }

      if(IsLocal && ReceivingClient->padding4EE0 == 0) {
         WriteBufLength = 0;
      }
      else {
         WriteBufLength = ReadBufLength;
      }

      if(ReceivingClient->sizebuf24AC.maxsize - ReceivingClient->sizebuf24AC.cursize > WriteBufLength + 6) {

         MSG_WriteByte(&(ReceivingClient->sizebuf24AC), 0x35); //5
         MSG_WriteByte(&(ReceivingClient->sizebuf24AC), SendingClientIndex);
         MSG_WriteShort(&(ReceivingClient->sizebuf24AC), WriteBufLength);
         MSG_WriteBuf(&(ReceivingClient->sizebuf24AC), WriteBufLength, voicebuffer);
      }
   }
}
/* DESCRIPTION: SV_FilterPacket
// LOCATION: sv_main.c
// PATH: SV_Frame
//
// Note on filterban: If it's set to zero, the ban list becomes an allow list...
// How QW handles bans is pretty much exactly how I do, but for simplicity,
// I'm going to zero it for now.
*/
qboolean SV_FilterPacket() {

/*
   netadr_t *a;
   filteredip_t *banip;

   a = global_net_from.remote_address;

   for(banip = HIJACK_IPFILTERS; banip != NULL; banip=banip->next) {
      if(NET_CompareAdrMasked(*a, banip->adr, banip->adrmask)) {
         return(cvar_sv_filterban.value);
      }
   }
   return(!cvar_sv_filterban.value);
*/
   return(0);
}
/* DESCRIPTION: SV_ParseMove
// LOCATION: sv_user.c (as part of SV_ExecuteClientMessage)
// PATH: SV_Frame->SV_ReadPackets->SV_ExecuteClientMessage
//
// Heh, read it and figure it out yourselves.  Obviously it reads various
// moves, validates them, and passes them to RunCmd.
*/
void SV_ParseMove(client_t * cl) {

   int i;
   int var_624_cc1, var_628_cc2, var_62C_cctotal;
   int var_678_readcount, var_680_byte1, var_665_byte2;
   int var_684_byte3;
 // int var_670_buttons;
   float var_630_loss;
   client_frame_t *var_66C_frame;
   usercmd_t var_664_usercmd, * var_61C_usercmd_p;
   usercmd_t var_618_usercmds[0x20], * var_620_usercmds_p;

   static int ccc = 0;
   ccc++;
   if(ccc == 200) {
      ccc = 0;
   }


   var_624_cc1 = 2;

   if(g_balreadymoved != 0) {
      global_msg_badread = 1;
      return;
   }

   g_balreadymoved = 1;
   var_66C_frame = &global_host_client->padding3480_frames[global_host_client->netchan1C.incoming_acknowledged & SV_UPDATE_MASK];

   Q_memset(&var_664_usercmd, 0, sizeof(usercmd_t));

   var_678_readcount = global_msg_readcount + 1;
   var_680_byte1 = MSG_ReadByte();
   var_665_byte2 = MSG_ReadByte();

   COM_UnMunge1(global_net_message.data + var_678_readcount + 1, var_680_byte1, global_host_client->netchan1C.incoming_sequence);

   var_684_byte3 = MSG_ReadByte();

   cl->padding4EE0 = ((var_684_byte3 >> 7) & 1);
   var_684_byte3 &= 0xFFFFFF7F;
   var_630_loss = var_684_byte3;

   var_624_cc1 = MSG_ReadByte();
   var_628_cc2 = MSG_ReadByte();
   var_62C_cctotal = var_624_cc1 + var_628_cc2;
   global_net_drop = global_net_drop + 1 - var_628_cc2;

   if(var_62C_cctotal < 0 || var_62C_cctotal > 0x1c) {

      Con_Printf("%s: Dropping %s for sending too many commands (%i)\n", __FUNCTION__, global_host_client->PlayerName, var_62C_cctotal);
      SV_DropClient(global_host_client, 0, "%s: CMD_MAXBACKUP hit (which means you lagged out I think)\n");
      global_msg_badread = 1;
      return;
   }


   //Okay, go through all of the moves, past and present.
   var_61C_usercmd_p = &var_664_usercmd; //Pointer to our null usercmd.
   for(i = var_62C_cctotal-1; i >= 0; i--) {

      var_620_usercmds_p = &(var_618_usercmds[i]);
      MSG_ReadUsercmd(var_620_usercmds_p, var_61C_usercmd_p);
      var_61C_usercmd_p = var_620_usercmds_p; //new baseline
   }

   if(global_sv.active == 0 || (global_host_client->padding00_used == 0 &&  global_host_client->padding04 == 0)) { return; }

   if(global_msg_badread == 1) {
      Con_Printf("%s: %s sent a bad command.\n", __FUNCTION__, global_host_client->PlayerName);
      return;
   }
   if(COM_BlockSequenceCRCByte(global_net_message.data + var_678_readcount + 1, (global_msg_readcount - var_678_readcount) - 1, global_host_client->netchan1C.incoming_sequence) != var_665_byte2) {

      Con_Printf("%s: %s had a mangled transmission--CRC mismatch\n", __FUNCTION__, global_host_client->PlayerName);
      global_msg_badread = 1;
      return;
   }


   //That's that.
   global_host_client->packet_loss = var_630_loss;
   if((global_sv.padding04 != 0) ||
      (global_svs.allocated_client_slots < 2 && key_dest != 0) ||
      ((global_sv_player->v.flags) & FL_FROZEN)) {

      for(i = 0; i < var_628_cc2; i++) {

         var_618_usercmds[i].msec = 0;
         var_618_usercmds[i].forwardmove = 0;
         var_618_usercmds[i].sidemove = 0;
         var_618_usercmds[i].upmove = 0;
         var_618_usercmds[i].buttons = 0;

         if(global_sv_player->v.flags & FL_FROZEN) {
            var_618_usercmds[i].impulse = 0;
         }

         var_618_usercmds[i].viewangles[0] = global_sv_player->v.v_angle[0];
         var_618_usercmds[i].viewangles[1] = global_sv_player->v.v_angle[1];
         var_618_usercmds[i].viewangles[2] = global_sv_player->v.v_angle[2];
      }

      global_net_drop = 0;
   }
   else {

      global_sv_player->v.v_angle[0] = var_618_usercmds[0].viewangles[0];
      global_sv_player->v.v_angle[1] = var_618_usercmds[1].viewangles[1];
      global_sv_player->v.v_angle[2] = var_618_usercmds[2].viewangles[2];
   }

 // var_670_buttons = var_618_usercmds[0].buttons;
   global_sv_player->v.button = var_618_usercmds[0].buttons;
   global_sv_player->v.light_level = var_618_usercmds[0].lightlevel;
   SV_EstablishTimeBase(global_host_client, var_618_usercmds, global_net_drop, var_624_cc1, var_628_cc2); //amd inline
 //  SV_PreRunCmd();

   if(global_net_drop < 24) {

      while(global_net_drop > var_624_cc1) {

         SV_RunCmd(&(global_host_client->padding2440), 0);
         global_net_drop--;
      }
      while(global_net_drop > 0) {

         i = global_net_drop + var_628_cc2 - 1; //We're sort've going backwards.
         SV_RunCmd(&var_618_usercmds[i], global_host_client->netchan1C.incoming_sequence - i);
         global_net_drop--;
      }
   }

   for(i = var_628_cc2-1; i >= 0; i--) {

      SV_RunCmd(&var_618_usercmds[i], global_host_client->netchan1C.incoming_sequence - i);
   }

   global_host_client->padding2440 = var_618_usercmds[0]; //full struct copy

   var_66C_frame->ping_time -= global_host_client->padding2440.msec * 0.5 / 1000.0;
   if(var_66C_frame->ping_time < 0) {
      var_66C_frame->ping_time = 0;
   }

   if(global_sv_player->v.animtime > global_sv.time0c + host_frametime) {
      global_sv_player->v.animtime = global_sv.time0c + host_frametime;
   }
}
/* DESCRIPTION: SV_ExecuteClientMessage
// LOCATION: sv_user.c
// PATH: SV_Frame->SV_ReadPackets
//
// This handles some of the latency calcs and then passes the packet to an
// appropriate handler.  In QW the handlers are all inline.
*/
void SV_ExecuteClientMessage(client_t * cl) {

   client_frame_t *frame;
   int ByteToProcess;

   g_balreadymoved = 0;


   frame = &(cl->padding3480_frames[cl->netchan1C.incoming_acknowledged & SV_UPDATE_MASK]);

   frame->ping_time = (global_realtime - frame->sent_time) - cl->cl_updaterate;
   //What followed appeared to be beginning non-skewness that I ignored.

   SV_ComputeLatency(cl);
   global_host_client = cl;
   global_sv_player = cl->padding4A84;
   cl->padding2434_delta_sequence = -1;
   global_pmove = &global_svmove;

   while(1) {

      if(global_msg_badread != 0) {

         Con_Printf("%s: Bad read occured.\n", __FUNCTION__);
         SV_DropClient(cl, 0, "Bad send.\n");
         return;
      }

      ByteToProcess = MSG_ReadByte();

      if(ByteToProcess == -1) { return; } //signals error or end or something
      if(ByteToProcess < 0 || ByteToProcess > 9) {

         Con_Printf("%s: Encountered an out-of-range byte.  Dropping client.\n", __FUNCTION__);
         SV_DropClient(cl, 0, "Bad command character in packet.");
         return;
      }

      //The following was a very odd way to implement a jump table.  It is now
      //a switch.
      switch(ByteToProcess) {

      case 2: //clc_move
         SV_ParseMove(cl);
         break;
      case 3: //clc_stringcmd
         SV_ParseStringCommand();
         break;
      case 4: //clc_delta
         SV_ParseDelta();
         break;
      case 5: //clc_resourcelist
         SV_ParseResourceList();
         break;
      case 7: //clc_fileconsistency
         SV_ParseConsistencyResponse();
         break;
      case 8: //clc_voicedata
         SV_ParseVoiceData(cl);
         break;
      //These don't do anything.
      case 0: //Clc_bad
      case 1: //Clc_nop
      case 6: //Clc_tmove
      case 9: //endoflist--why is this an option?
      default:
         break;
      }
   }
}



//outgoing

void SV_SendBan() {

   #define SV_SENDBAN_MESSAGE "\xff\xff\xff\xff" "lYou are banned from this server.\n"

   NET_SendPacket(NS_SERVER, sizeof(SV_SENDBAN_MESSAGE), SV_SENDBAN_MESSAGE, global_net_from);
   SZ_Clear(&global_net_message);
}
/* DESCRIPTION: SV_FailDownload
// LOCATION: sv_main
// PATH: SV_BeginFileDownload
//
// Simply writes a packet to the reliable buffer telling the client they suck.
*/
void SV_FailDownload(char * arg0) {

   if(arg0 != NULL && arg0[0] != '\0') {

      MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), 0x31);
      MSG_WriteString(&(global_host_client->netchan1C.netchan_message), arg0);
   }
}
/* DESCRIPTION: SV_WriteVoiceCodec
// LOCATION:
// PATH: SV_CreateBaseline
//
// ooOOooh, voice codecs... Sounds complicated, right?  Well they probably are,
// but we luck out here--we jsut have to copy a string.
*/
void SV_WriteVoiceCodec(sizebuf_t *msg) {

   MSG_WriteByte(msg, 0x34);

   // Only send the voice codec in multiplayer. Otherwise, we don't want voice.
   if(global_svs.allocated_client_slots > 1) {

      MSG_WriteString(msg, cvar_sv_voicecodec.string);
   }
   else {
      MSG_WriteByte(msg, '\0'); //An empty string, effectively.
   }
}
/* DESCRIPTION: SV_EmitEvents
// LOCATION:
// PATH: SV_WriteEntitiesToClient
//
// Now that I know what the deltas do, this makes a lot more sense.
// In a nutshell, it writes events to the outgoing packet if they are
// events the client needs to know about.  There's some flag knowledge that's
// not quite understood, but since the structs are known, it's fairly
// understandable.
*/
void SV_EmitEvents(client_t *client, packet_entities_t * visibleEnts, sizebuf_t *msg) {

   unsigned int i, j, k, var_18_InUseCounter;
   int var_1C_index;
   event_args_t var_68_Event;
   event_info_t * var_C_eventptr1, * var_10_eventptr2;
   entity_state_t * var_14_entstate;


   var_18_InUseCounter = 0;
   var_C_eventptr1 = client->events;
   for(i = 0; i < 0x40; i++) { //Seems a bit inefficient, this extra loop...

      var_10_eventptr2 = &(var_C_eventptr1[i]);
      if(var_10_eventptr2->index != 0) { var_18_InUseCounter++; }
   }
   if(var_18_InUseCounter == 0) { return; } //nothing to send I guess.
   if(var_18_InUseCounter > 0x1F) { var_18_InUseCounter = 0x1F; } //Guess we never send more than 32...

   for(i = 0; i < 0x40; i++) {

      var_10_eventptr2 = &(var_C_eventptr1[i]);
      if(var_10_eventptr2->index == 0) { continue; }

      var_1C_index = var_10_eventptr2->entity_index;
      for(j = 0; j < visibleEnts->max_entities; j++) {

         var_14_entstate = &(visibleEnts->entities[j]);
         if(var_14_entstate->number == var_1C_index) { break; }
      }

      if(j >= 0x40) { //Didn't find it.  *shrug*

         var_10_eventptr2->packet_index = visibleEnts->max_entities;
         var_10_eventptr2->args.entindex = var_1C_index;
      }
      else {

         var_10_eventptr2->packet_index = j;
         var_10_eventptr2->args.ducking = 0;

         if((var_10_eventptr2->args.flags & 0x01) == 0) {

            var_10_eventptr2->args.origin[0] = 0;
            var_10_eventptr2->args.origin[1] = 0;
            var_10_eventptr2->args.origin[2] = 0;
         }
         if((var_10_eventptr2->args.flags & 0x02) == 0) {

            var_10_eventptr2->args.angles[0] = 0;
            var_10_eventptr2->args.angles[1] = 0;
            var_10_eventptr2->args.angles[2] = 0;
         }
      }
   }

   MSG_WriteByte(msg, 0x03);
   MSG_StartBitWriting(msg);
   MSG_WriteBits(var_18_InUseCounter, 5);

   k = 0;
   for(i = 0; i < 0x40; i++) {

      var_10_eventptr2 = &(var_C_eventptr1[i]);
      if(var_10_eventptr2->index == 0) {

         var_10_eventptr2->packet_index = -1;
         var_10_eventptr2->entity_index = -1;
         continue;
      }

      if(k < var_18_InUseCounter) { //prevents us from writing too many.

         MSG_WriteBits(var_10_eventptr2->index, 0x0A);
         if(var_10_eventptr2->packet_index == -1) {

            MSG_WriteBits(0, 1);
         }
         else {

            MSG_WriteBits(1, 1);
            MSG_WriteBits(var_10_eventptr2->packet_index, 0x0B);

            if(Q_memcmp(&var_68_Event, &(var_10_eventptr2->args), sizeof(event_args_t)) == 0) { //same.

               MSG_WriteBits(0, 1);
            }
            else {

               MSG_WriteBits(1, 1);
               DELTA_WriteDelta((void *)&var_68_Event, (void *)&(var_10_eventptr2->args), 1, SV_LookupDelta("event_t"), NULL);
            }
         }

         if(var_10_eventptr2->fire_time == 0) {

            MSG_WriteBits(0, 1);
         }
         else {

            MSG_WriteBits(1, 1);
            MSG_WriteBits(var_10_eventptr2->fire_time * 100, 0x10);
         }
      }

      var_10_eventptr2->index = 0;
      var_10_eventptr2->packet_index = -1;
      var_10_eventptr2->entity_index = -1;
      k++;
   }

   MSG_EndBitWriting(msg);
}
/* DESCRIPTION: SV_EmitPings
// LOCATION:
// PATH: SV_WriteEntitiesToClient
//
// This function writes pings and loss rates to a data stream.  It's
// a full update it seems--I didn't know you got info on loss though...
// Can I get a netgraph of other players then?
*/
void SV_EmitPings(client_t *client, sizebuf_t *msg) {

   unsigned int i, var_c, var_10;
   client_t * cl;


   MSG_WriteByte(msg, 0x11);
   MSG_StartBitWriting(msg);

   for(cl = global_svs.clients, i = 0; i < global_svs.allocated_client_slots; i++, cl++) {

     if(cl->padding00_used == 0) { continue; }

     SV_GetNetInfo(cl, &var_c, &var_10);
     MSG_WriteBits(1, 1);
     MSG_WriteBits(i, 0x05);
     MSG_WriteBits(var_c, 0x0C);
     MSG_WriteBits(var_10, 0x07);
   }

   MSG_WriteBits(0, 1);
   MSG_EndBitWriting(msg);
}
void SV_BuildReconnect(sizebuf_t * arg0) {

   MSG_WriteByte(arg0, 9);
   MSG_WriteString(arg0, "reconnect\n");
}
/* DESCRIPTION: SV_SendUserReg
// LOCATION:
// PATH: SV_New_f, SV_UpdateToReliableMessages, SV_ActivateServer
//
// Another unidentified struct.  What we're writing, I really can't say.
*/
void SV_SendUserReg(sizebuf_t *msg) {

   gpnewusermsg_struct_t * var_4;


   var_4 = global_sv_gpNewUserMsg;
   while(var_4 != NULL) {

      MSG_WriteByte(msg, 0x27);
      MSG_WriteByte(msg, var_4->padding00_id);
      MSG_WriteByte(msg, var_4->padding04_size);
      MSG_WriteLong(msg, var_4->padding08_name.asInts[0]); //This is a string.  I'm serious--it is writing the string as ints.
      MSG_WriteLong(msg, var_4->padding08_name.asInts[1]);
      MSG_WriteLong(msg, var_4->padding08_name.asInts[2]);
      MSG_WriteLong(msg, var_4->padding08_name.asInts[3]);
      var_4 = var_4->next;
   }
}
//sends things like gravity, part of this is IDable from SV_New
void SV_SendServerinfo(sizebuf_t * buffer, client_t * ptrClient) {

   char tempstr[0x80]; //Ought to be enough for now.
   unsigned int var808;
   CRC32_t var814;
   unsigned int var_filesize;
   BYTE * LoadedFile;

   int i;

   if(global_svs.allocated_client_slots > 1 || cvar_developer.value != 0) {

      MSG_WriteByte(buffer, 8);
      //This doesn't look like good, quality, correct functionality to me.
      /*
      Q_snprintf(array800, sizeof(array800)-1, "%c\nOSHLDS BUILD %d SERVER (%i CRC)\nServer # %i\n",
              2, build_number(), 0, global_svs.ServerCount);
      array800[sizeof(array800)-1] = '\0';
      MSG_WriteString(buffer, array800);
      */

      //Meanwhile, this is messy, but should be faster.
      //The -1 +1s are all in place to prevent us from writing a null until the very end.
      #define __SVSERVERWELCOME_1 "\x02" "\nOSHLDS VER "
      #define __SVSERVERWELCOME_2 " BUILT " __DATE__ "--" __TIME__ "\nServer # "
      MSG_WriteBuf(buffer, sizeof(__SVSERVERWELCOME_1)-1, __SVSERVERWELCOME_1);

      i = sprintf(tempstr, "%u", build_number());
      MSG_WriteBuf(buffer, i, tempstr);
      MSG_WriteBuf(buffer, sizeof(__SVSERVERWELCOME_2)-1, __SVSERVERWELCOME_2);

      i = sprintf(tempstr, "%u\n", global_svs.ServerCount);
      MSG_WriteBuf(buffer, i+1, tempstr);
   }

   MSG_WriteByte(buffer, 0x0B);
   MSG_WriteLong(buffer, 0x2E);
   MSG_WriteLong(buffer, global_svs.ServerCount);

   var808 = NUM_FOR_EDICT(ptrClient->padding4A84) - 1;
   var814 = global_sv.map_CRC;

   COM_Munge3((char *)(&var814), 4, (0xFF - var808) & 0xFF); //Yeah, looks like we're munging an int...

   MSG_WriteLong(buffer, var814);
   MSG_WriteBuf(buffer, 0x10, global_sv.usedpadding);
   //I'm guessing that means "player X of Y".
   MSG_WriteByte(buffer, global_svs.allocated_client_slots);
   MSG_WriteByte(buffer, var808);

   if(cvar_deathmatch.value == 0 || cvar_coop.value != 0) {
      MSG_WriteByte(buffer, 1);
   }
   else {
      MSG_WriteByte(buffer, 0);
   }

   COM_GetGameDirSize(tempstr, sizeof(tempstr));
   MSG_WriteString(buffer, tempstr);
   MSG_WriteString(buffer, cvar_hostname.string);
   MSG_WriteString(buffer, global_sv.name4);

   LoadedFile = COM_LoadFileForMe(cvar_mapcyclefile.string, &var_filesize);

   if(LoadedFile != NULL) {
      if(var_filesize != 0) {
         MSG_WriteString(buffer, LoadedFile);
      }
      else {
         MSG_WriteString(buffer, "mapcycle failure");
      }
      COM_FreeFile(LoadedFile);
   }
   else {
      MSG_WriteString(buffer, "mapcycle failure");
   }

   MSG_WriteByte(buffer, global_svs.paddingE8);
   if(global_svs.paddingE8 != 0) {

      MSG_WriteByte(buffer, 4);
      MSG_WriteBuf(buffer, 4, &(sUpdateIPAddress.ip)); //That's the ADDRESS, or a pointer to object.padding4
      MSG_WriteBuf(buffer, 0x10, &global_svs.module80[1].buf10);
   }
   MSG_WriteByte(buffer, 0x36);
   MSG_WriteString(buffer, com_clientfallback); //This was always an empty string for us.  What does it do otherwise?
   MSG_WriteByte(buffer, allow_cheats);

   SV_WriteDeltaDescriptionsToClient(buffer);
   SV_SetMoveVars();
   SV_WriteMovevarsToClient(buffer);

   MSG_WriteByte(buffer, 0x20);
   MSG_WriteByte(buffer, gGlobalVariables.cdAudioTrack);
   MSG_WriteByte(buffer, gGlobalVariables.cdAudioTrack);
   MSG_WriteByte(buffer, 0x05);

   MSG_WriteShort(buffer, NUM_FOR_EDICT(ptrClient->padding4A84));

   ptrClient->padding04 = 0;
   ptrClient->padding08 = 0;
   ptrClient->padding0C_connected = 1;
}
/* DESCRIPTION: SV_WriteClientdataToMessage
// LOCATION: It was somewhere, but the things were quite different
// PATH: SV_WriteSpawn, SV_SendClientDatagram
//
// This function very nicely pads out some structs, and also makes the
// whole delta mess finally make sense.  I'll have to go back and
// make those functions more readable... Anyway, the core of this
// involves checking variables, seeing if anything needs to be updated
// clientside, and then sending those updates.
*/
void SV_WriteClientdataToMessage(client_t *client, sizebuf_t *msg) {

   int i;
   clientdata_t var_1F4_nullcd;
   clientdata_t * clientdataptr, * clientdataptr2;
   weapon_data_t var_258_nullwep;
   weapon_data_t * weapondataptr, * weapondataptr2;
   client_frame_t * frame;
   edict_t *ent;


   Q_memset(&var_1F4_nullcd, 0, sizeof(var_1F4_nullcd));
   ent = client->padding4A84;
   frame = &(client->padding3480_frames[client->netchan1C.outgoing_sequence & SV_UPDATE_MASK]);

   frame->sent_time = global_realtime;
   frame->ping_time = -1;

   if(client->padding2430 != 0) { //chokecount

      MSG_WriteByte(msg, 0x2A); //global_svc_chokecount?
      client->padding2430 = 0;
   }

   if(ent->v.fixangle != 0) {
      if(ent->v.fixangle == 2) {

         MSG_WriteByte(msg, 0x26);
         MSG_WriteHiresAngle(msg, ent->v.avelocity[1]);
         ent->v.avelocity[1] = 0;
      }
      else {

         MSG_WriteByte(msg, 0x0A);
         for(i = 0; i < 3; i++) {

            MSG_WriteHiresAngle(msg, ent->v.angles[i]);
         }
      }
      ent->v.fixangle = 0;
   }

   Q_memset(&(frame->clientdata_0C), 0, sizeof(frame->clientdata_0C));
   //Second argument is being reduced to a boolean.  Third arg is unknown type.
   gEntityInterface.pfnUpdateClientData(ent, global_host_client->cl_lw != 0, &(frame->clientdata_0C));

   MSG_WriteByte(msg, 0x0F);
   if(client->padding243C_proxy != 0) { return; }

   MSG_StartBitWriting(msg);

   if(global_host_client->padding2434_delta_sequence == -1) {

      clientdataptr = &var_1F4_nullcd;
   }
   else {

      clientdataptr = &(global_host_client->padding3480_frames[global_host_client->padding2434_delta_sequence & SV_UPDATE_MASK].clientdata_0C);
   }
   clientdataptr2 = &(frame->clientdata_0C);


   if(global_host_client->padding2434_delta_sequence == -1) {

      MSG_WriteBits(0, 1);
   }
   else {
      MSG_WriteBits(1, 1);
      MSG_WriteBits(global_host_client->padding2434_delta_sequence, 8);
   }

   //That private var is a bit of a problem.  Resolved--maybe--for now.
  // DELTA_WriteDelta((void *)clientdataptr, (void *)clientdataptr2, 1, g_pclientdelta, NULL);
   DELTA_WriteDelta((void *)clientdataptr, (void *)clientdataptr2, 1, SV_LookupDelta("clientdata_t"), NULL);

   if(global_host_client->cl_lw != 0) {

      Q_memset(&var_258_nullwep, 0, sizeof(weapon_data_t));
      if(gEntityInterface.pfnGetWeaponData(global_host_client->padding4A84, frame->weapondata_1e8) != 0) {

         for(i = 0; i < 0x20; i++) {

            if(global_host_client->padding2434_delta_sequence == -1) {
               weapondataptr = &var_258_nullwep;
            }
            else {
               weapondataptr = &(global_host_client->padding3480_frames[global_host_client->padding2434_delta_sequence & SV_UPDATE_MASK].weapondata_1e8[i]);
            }
            weapondataptr2 = &(frame->weapondata_1e8[i]);

           // DELTA_CheckDelta()(void *)(weapondataptr), (void *)weapondataptr2, g_pweapondelta);
            if(DELTA_CheckDelta((void *)(weapondataptr), (void *)weapondataptr2, SV_LookupDelta("weapon_data_t")) != 0) {

               MSG_WriteBits(1, 1);
               MSG_WriteBits(i, 5);

              // DELTA_WriteDelta((void *)weapondataptr, (void *)weapondataptr2, 1, g_pweapondelta, NULL);
               DELTA_WriteDelta((void *)weapondataptr, (void *)weapondataptr2, 1, SV_LookupDelta("weapon_data_t"), NULL);
            }
         }
      }
   }

   MSG_WriteBits(0, 1);
   MSG_EndBitWriting(msg);
}
/* DESCRIPTION: SV_WriteSpawn
// LOCATION:
// PATH: SV_Spawn_f
//
// This appears to do more of the dirty work involved in spawning, like
// clearing out old edicts and creating new ones.
*/
//buffer seems to be: 0x20 bytes unknown, 4 byte num_strings (max 0x20), 0x28 byte strings
void SV_WriteSpawn(sizebuf_t *msg) {

   unsigned int i;
   client_t * ptr;

   char buffer[0x574]; //unlabeled struct.
   char tempstr[0x100]; //hacky, improve.


   if(global_sv.padding08 == 0) {


      global_sv.padding3bc64 = 1;

      ReleaseEntityDLLFields(global_sv_player);
      Q_memset(&(global_sv_player->v), 0, sizeof(entvars_t));
      InitEntityDLLFields(global_sv_player);

      global_sv_player->v.colormap = NUM_FOR_EDICT(global_sv_player); //wha?
      global_sv_player->v.netname = global_host_client->PlayerName - global_pr_strings;

      if(global_host_client->padding243C_proxy != 0) {
         global_sv_player->v.flags |= FL_PROXY;
      }

      gGlobalVariables.time = global_sv.time0c;
      gEntityInterface.pfnClientPutInServer(global_sv_player);
      global_sv.padding3bc64 = 2;
   }
   else {

      if(global_host_client->padding243C_proxy != 0) {
         Con_Printf("%s: ERROR! Spectator mode doesn't work with saved game.\n", __FUNCTION__);
         return;
      }
      global_sv.padding04 = 0;
   }

   SZ_Clear(&(global_host_client->netchan1C.netchan_message));
   SZ_Clear(&(global_host_client->sizebuf24AC));

   MSG_WriteByte(msg, 0x07);
   MSG_WriteFloat(msg, global_sv.time0c);

   global_host_client->padding4BA4 = 1;

   for(i = 0, ptr = global_svs.clients; i < global_svs.allocated_client_slots; i++, ptr++) {

      if(global_host_client == ptr || ptr->padding00_used != 0 ||
         ptr->padding04 != 0 || ptr->padding0C_connected != 0) {

         //Send to all active clients, as well as the current client no
         //matter what its state.
         SV_FullClientUpdate(ptr, msg);
      }
   }

   for(i = 0; i < 0x40; i++) {

      MSG_WriteByte(msg, 0x0C);
      MSG_WriteByte(msg, i);
      MSG_WriteString(msg, global_sv.padding3BB50[i]);
   }

   if(global_host_client->padding243C_proxy != 0) {


      MSG_WriteByte(msg, 0x0A);
      for(i = 0; i < 2; i++) {

         MSG_WriteHiresAngle(msg, global_sv_player->v.v_angle[i]);
      }
      MSG_WriteHiresAngle(msg, 0);

      SV_WriteClientdataToMessage(global_host_client, msg);
      if(global_sv.padding08 != 0) {

         Q_memset(buffer, 0, sizeof(buffer));
         gGlobalVariables.pSaveData = buffer;

         gEntityInterface.pfnParmsChangeLevel();
         MSG_WriteByte(msg, 0x21);

         sprintf(tempstr, "SAVE/%s.HL2", global_sv.name); //Ben thought this was in error.  It's not (though a quick skim indicated it might not be required)
         COM_FixSlashes(tempstr);
         MSG_WriteString(msg, tempstr);

         //Stupid buffer thing.
         MSG_WriteByte(msg, buffer[0x24]);

         for(i = 0; i < buffer[0x24]; i++) { //It's an int, technically.

            MSG_WriteString(msg, &(buffer[0x2c]) + (i*0x28));
         }

         global_host_client->padding243C_proxy = 0;
         gGlobalVariables.pSaveData = NULL;
      }
   }

   MSG_WriteByte(msg, 0x19);
   MSG_WriteByte(msg, 0x01);

   global_host_client->padding00_used = 1;
   global_host_client->padding2474_LastCommandTime = 0;
   global_host_client->padding247C_LastRunTime = 0;
   global_host_client->padding2484_NextRunTime = 0;

   global_host_client->padding04 = 1;
   global_host_client->padding08 = 0;
   global_host_client->padding0C_connected = 1;
}
void SV_StartParticle(const vec3_t org, const vec3_t dir, int color, int count) {

   int i;


   if(global_sv.reliable_datagram.cursize + 16 > global_sv.reliable_datagram.maxsize) { return; }


   MSG_WriteByte(&(global_sv.reliable_datagram), 0x12);
   MSG_WriteCoord(&(global_sv.reliable_datagram), org[0]);
   MSG_WriteCoord(&(global_sv.reliable_datagram), org[1]);
   MSG_WriteCoord(&(global_sv.reliable_datagram), org[2]);

   i = dir[0] * 16;
   if(i > 127) { i = 128; }
   else if(i < -127) { i = -128; }
   MSG_WriteChar(&(global_sv.reliable_datagram), i);

   i = dir[1] * 16;
   if(i > 127) { i = 128; }
   else if(i < -127) { i = -128; }
   MSG_WriteChar(&(global_sv.reliable_datagram), i);

   i = dir[2] * 16;
   if(i > 127) { i = 128; }
   else if(i < -127) { i = -128; }
   MSG_WriteChar(&(global_sv.reliable_datagram), i);


   MSG_WriteByte(&(global_sv.reliable_datagram), count);
   MSG_WriteByte(&(global_sv.reliable_datagram), color);
}
/* DESCRIPTION: SV_CreateBaseline
// LOCATION: //sv_init.c global_svq3_game.c
// PATH: SV_ActivateServer
//
// You'd think this made some baselines that the server would use to shorten
// packet data.  You might be right--there's a lot here--but what I see
// is the writing of these baselines so that the client can use them.
//
// As an aside, you can pretty well nix this function and the server will/
// still run (nixing was 'tested' by not properly using the private variable,
// which would remain at zero if not properly written to).  The result
// is *probably* an increase in bandwidth consumption, as it would mean
// delta compression wouldn't have any baselines, but I didn't notice any
// problems over a virtual LAN.
*/
void SV_CreateBaseline() {

   unsigned int i, var_c_player, var_10_bool;
   edict_t * svent;

   static OddEntityContainer_t gs_3638C;
   entity_state_t var_164_state;


   global_sv.padding3bc60_struct = &gs_3638C; //Give it a large chunk of data.
   Q_memset(&var_164_state, 0, sizeof(entity_state_t));

   for(i = 0; i < global_sv.num_edicts; i++) {

      svent = &(global_sv.edicts[i]);
      if(svent->free != 0) { continue; }
      if(i > global_svs.allocated_client_slots && svent->v.modelindex == 0) { //not player
         continue;
      }

      var_c_player = 0;
      if(i <= global_svs.allocated_client_slots && i > 0) { //There's a function for this.
         var_c_player = 1;
      }

      global_sv.edstates[i].number = i;
      global_sv.edstates[i].entityType = 1;
      if((svent->v.flags & FL_CUSTOMENTITY)) {

         global_sv.edstates[i].entityType = 2;
      }

      gEntityInterface.pfnCreateBaseline(var_c_player, i, &(global_sv.edstates[i]), svent, global_sv_playermodel, global_player_mins[0], global_player_maxs[0]);
      sv_lastnum = i;
   }

   gEntityInterface.pfnCreateInstancedBaselines();
   MSG_WriteByte(&(global_sv.signon_datagram), 0x16);  //global_svc_spawnbaseline
   MSG_StartBitWriting(&(global_sv.signon_datagram));

   for(i = 0; i < global_sv.num_edicts; i++) {

      svent = &(global_sv.edicts[i]);
      if(svent->free != 0) { continue; }
      if(i > global_svs.allocated_client_slots && svent->v.modelindex == 0) { //not player
         continue;
      }

      MSG_WriteBits(i, 0x0B);
      var_10_bool = (global_sv.edstates[i].entityType ^ 1) & 1; //if bit == 1, var == 0.
      MSG_WriteBits(global_sv.edstates[i].entityType, 2);

      if(var_10_bool == 0) {
         if(SV_IsPlayerIndex(i) == 0) {

            DELTA_WriteDelta((void *)&var_164_state, (void *)&(global_sv.edstates[i]),
                              1, SV_LookupDelta("entity_state_t"), NULL);
         }
         else {

            DELTA_WriteDelta((void *)&var_164_state, (void *)&(global_sv.edstates[i]),
                              1, SV_LookupDelta("entity_state_player_t"), NULL);
         }
      }
      else {

         DELTA_WriteDelta((void *)&var_164_state, (void *)&(global_sv.edstates[i]),
                           1, SV_LookupDelta("custom_entity_state_t"), NULL);
      }
   }

   MSG_WriteBits(0xFFFF, 0x10);
   MSG_WriteBits(global_sv.padding3bc60_struct->padding00, 6);
   for(i = 0; i < global_sv.padding3bc60_struct->padding00; i++) {
      DELTA_WriteDelta((void *)&var_164_state, (void *)&(global_sv.padding3bc60_struct->padding104[i]), 1, SV_LookupDelta("entity_state_t"), NULL);
   }

   MSG_EndBitWriting(&(global_sv.signon_datagram));
   SV_WriteVoiceCodec(&(global_sv.signon_datagram));
}

/* DESCRIPTION: SV_CreatePacketEntities/EmitPacketEntities
// LOCATION: global_sv_ents.c (as EmitPacketEntities)
// PATH: SV_WriteEntitiesToClient
//
// It writes entities.  I don't quite grasp what else it does.
// I also know from experience that for some reason it is called
// differently on the AMD engine, which eventualy crashed my client
// but not the server.  So don't comment out WriteEntitiesToClient.
*/
void SV_CreatePacketEntities(int SomeBoolean, client_t *client, packet_entities_t * visibleEnts, sizebuf_t *msg) {

   unsigned int i;
   unsigned int oldnum, newnum;
   unsigned int oldindex, newindex;
   unsigned int oldmax;
   uint32 var_24, var_28, var_2C;
   client_frame_t   * var_8_fromframe;
   packet_entities_t * var_c_from;
   edict_t * var_4_ent;
   entity_state_t * var_30_state;


   if(SomeBoolean == 1) {

      var_8_fromframe = &(client->padding3480_frames[client->padding2434_delta_sequence & SV_UPDATE_MASK]);
      var_c_from = &(var_8_fromframe->entities);
      oldmax = var_c_from->max_entities;

      MSG_WriteByte(msg, 0x29); //global_svc_deltapacketentities
      MSG_WriteShort(msg, visibleEnts->max_entities);
      MSG_WriteByte(msg, client->padding2434_delta_sequence);
   }
   else {

      var_c_from = NULL;
      oldmax = 0;

      MSG_WriteByte(msg, 0x28); //global_svc_packetentities
      MSG_WriteShort(msg, visibleEnts->max_entities);
   }

   var_24 = var_2C = 0;
   oldindex = newindex = 0;
   MSG_StartBitWriting(msg);

   while(newindex < visibleEnts->max_entities || oldindex < oldmax) {

      newnum = newindex >= visibleEnts->max_entities ? 9999 : visibleEnts->entities[newindex].number;
      oldnum = oldindex >= oldmax ? 9999 : var_c_from->entities[oldindex].number;

      if(newnum == oldnum) {

         var_24 = (visibleEnts->entities[newindex].entityType >> 1) & 1; //equals true if second bit is one?
         SV_SetCallback(newnum, 0, var_24, &var_2C, 0, 0);
         if(var_24 == 0) {

            if(SV_IsPlayerIndex(newnum) == 0) {

               DELTA_WriteDelta((void *)&(var_c_from->entities[oldindex]), (void *)&(visibleEnts->entities[newindex]),
                                 0, SV_LookupDelta("entity_state_t"), SV_InvokeCallback);
            }
            else {

               DELTA_WriteDelta((void *)&(var_c_from->entities[oldindex]), (void *)&(visibleEnts->entities[newindex]),
                                 0, SV_LookupDelta("entity_state_player_t"), SV_InvokeCallback);
            }

         }
         else {

            DELTA_WriteDelta((void *)&(var_c_from->entities[oldindex]), (void *)&(visibleEnts->entities[newindex]),
                              0, SV_LookupDelta("custom_entity_state_t"), SV_InvokeCallback);
         }

         oldindex++;
         newindex++;
         continue;
      }


      if(newnum < oldnum) { // this is a new entity, send it from the baseline <--QW

         var_4_ent = EDICT_NUM(newnum);

         var_24 = (visibleEnts->entities[newindex].entityType >> 1) & 1;
         SV_SetCallback(newnum, 0, var_24, &var_2C, var_c_from == NULL, 0);

         var_30_state = &(global_sv.edstates[newnum]);

         if(cvar_sv_instancedbaseline.value == 0 || global_sv.padding3bc60_struct->padding00 == 0 || sv_lastnum > newnum) {
            if(var_c_from == NULL) {

               var_28 = SV_FindBestBaseline(newindex, &var_30_state, visibleEnts->entities, newnum, var_24);
               if(var_28 != 0) {
                  SV_SetCallback(newnum, 0, var_24, &var_2C, 1, var_28);
               }
            }
         }
         else {

            for(i = 0; i < global_sv.padding3bc60_struct->padding00; i++) {

               if(var_4_ent->v.classname == global_sv.padding3bc60_struct->padding04[i]) {

                  SV_SetNewInfo(i);
                  var_30_state = &(global_sv.padding3bc60_struct->padding104[i]); //This breaks everything
                  break;
               }
            }
         }

         if(var_24 == 0) {

            if(SV_IsPlayerIndex(newnum) == 0) {

               DELTA_WriteDelta((void *)var_30_state, (void *)&(visibleEnts->entities[newindex]),
                                 1, SV_LookupDelta("entity_state_t"), SV_InvokeCallback);
            }
            else {

               DELTA_WriteDelta((void *)var_30_state, (void *)&(visibleEnts->entities[newindex]),
                                 1, SV_LookupDelta("entity_state_player_t"), SV_InvokeCallback);
            }

         }
         else {

            DELTA_WriteDelta((void *)var_30_state, (void *)&(visibleEnts->entities[newindex]),
                              1, SV_LookupDelta("custom_entity_state_t"), SV_InvokeCallback);
         }
         newindex++;
         continue;
      }

      //Effectively, newnum must be > than oldnum to get here.
      SV_WriteDeltaHeader(oldnum, 1, var_24, &var_2C, 0, 0, 0, 0);
      oldindex++;
   }

   MSG_WriteBits(0, 0x10);
   MSG_EndBitWriting(msg);
   //This has an unchecked and largely useless return.
}
void SV_EmitPacketEntities(client_t *client, packet_entities_t * visibleEnts, sizebuf_t *msg) {

   if(client->padding2434_delta_sequence == -1) {
      SV_CreatePacketEntities(0, client, visibleEnts, msg);
   }
   else {
      SV_CreatePacketEntities(1, client, visibleEnts, msg);
   }
}
/* DESCRIPTION: SV_WriteEntitiesToClient
// LOCATION:
// PATH: SV_SendClientDatagram
//
// Here it is, writing entities to the client.  Because they need to
// know these things.  First we check to see what the client can actually
// SEE (they don't need to know Joe Schmoe is on the other side of the map),
// then we send it to them using delta compression (which, if you recall, is
// a fancy way of taking a 'default' state and filling in the changed values).
*/
void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg) {

   unsigned int i, WritePing, CheckWeps, var_15404_visenttotal, var_15430_boolplayer;

   byte * var_1541C_pvs, * var_1542C_pvsptr;
   byte * var_15420_pas;
   packet_entities_t * var_15408_packent;
   client_t * var_15434_client;
   client_frame_t * var_15410_frame;
   edict_t * var_1540C_ent, * var_15438_ent2;

   entity_state_t * var_1543C_esptr, massivearray[0x100];

   var_1541C_pvs = NULL;
   var_15420_pas = NULL;
   var_15410_frame = &(client->padding3480_frames[client->netchan1C.outgoing_sequence& SV_UPDATE_MASK]);
   var_1540C_ent = client->padding4A84;

   gEntityInterface.pfnSetupVisibility(client->padding4A88, var_1540C_ent, &var_1541C_pvs, &var_15420_pas);
   SV_ClearPacketEntities(var_15410_frame);

   var_15408_packent = &var_15410_frame->entities;
   var_15404_visenttotal = 0;

   WritePing = SV_ShouldUpdatePing(client);
   CheckWeps = (client->cl_lw != 0);
   var_1542C_pvsptr = var_1541C_pvs;

   for(i = 1; i < global_sv.num_edicts; i++) {

      var_15438_ent2 = &global_sv.edicts[i];

      if(i > 0 && i < global_svs.allocated_client_slots) {
         var_15430_boolplayer = 1;
      }
      else {
         var_15430_boolplayer = 0;
      }

      if(var_15430_boolplayer != 0) {

         var_15434_client = &global_svs.clients[i-1];
         if((var_15434_client->padding00_used == 0 && var_15434_client->padding04 == 0) ||
            var_15434_client->padding243C_proxy != 0) {

            continue;
         }
      }

      //Now to process.
      if(var_15404_visenttotal > 255) {
         Con_Printf("%s: Too many entities in visible packet list.\n", __FUNCTION__);
         break;
      }

      var_1543C_esptr = &massivearray[var_15404_visenttotal];
      if(gEntityInterface.pfnAddToFullPack(var_1543C_esptr, i, var_15438_ent2, global_host_client->padding4A84, CheckWeps, var_15430_boolplayer, var_1542C_pvsptr)) {
         var_15404_visenttotal++;
      }
   }


   SV_AllocPacketEntities(var_15410_frame, var_15404_visenttotal);

   if(var_15408_packent->max_entities != 0) {
      Q_memcpy(var_15408_packent->entities, massivearray, sizeof(entity_state_t) * var_15408_packent->max_entities);
   }

   SV_EmitPacketEntities(client, var_15408_packent, msg);
   SV_EmitEvents(client, var_15408_packent, msg);
   if(WritePing != 0) {
      SV_EmitPings(client, msg);
   }
}
/* DESCRIPTION: SV_SendClientDatagram
// LOCATION: global_sv_send.c
// PATH: SV_SendClientMessages
//
// Creates a sizebuf, and begins the process of writing data to it that
// the client needs.  QW example is a nice match.
*/
int SV_SendClientDatagram(client_t *client) {

   byte buf[4000]; //MAX_DATAGRAM in QW, which should be about 1400...
   sizebuf_t msg;


   msg.debugname = "Client Datagram";
   msg.overflow = 1;
   msg.data = buf;
   msg.maxsize = sizeof(buf);
   msg.cursize = 0;

   MSG_WriteByte(&msg, 0x07);
   MSG_WriteFloat(&msg, global_sv.time0c);

   SV_WriteClientdataToMessage(client, &msg);
   SV_WriteEntitiesToClient(client, &msg);

   if(client->sizebuf24AC.overflow & 2) { //overflowed
      Con_Printf("%s: WARNING: datagram overflowed for %s.\n", __FUNCTION__, client->PlayerName);
   }
   else {
      SZ_Write(&msg, client->sizebuf24AC.data, client->sizebuf24AC.cursize);
   }
   SZ_Clear(&(client->sizebuf24AC));

   if(msg.overflow & 2) { //overflowed
      Con_Printf("%s: WARNING: msg overflowed for %s.\n", __FUNCTION__, client->PlayerName);
      SZ_Clear(&msg);
   }

   // send the datagram
   Netchan_Transmit(&client->netchan1C, msg.cursize, buf);

   return(1);
}
/* DESCRIPTION: SV_SendClientMessages
// LOCATION: global_sv_send.c
// PATH: SV_Frame
//
// Creates a sizebuf, and begins the process of writing data to it that
// the client needs.  QW example is a nice match.
*/
void SV_SendClientMessages() {

   unsigned int i;


   SV_UpdateToReliableMessages();

   // build individual updates
   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {

      if(global_host_client->padding00_used == 0 &&
         global_host_client->padding04 == 0 &&
         global_host_client->padding0C_connected == 0) {

         continue;
      }
      if(global_host_client->padding2438_IsFakeClient != 0) {
         continue;
      }

      if(global_host_client->padding347C_SkipNextTick != 0) {
         global_host_client->padding347C_SkipNextTick = 0;
         continue;
      }

      if(cvar_host_limitlocal.value == 0 && global_host_client->netchan1C.remote_address.type == NA_LOOPBACK) {
         global_host_client->padding3478_SendNextTick = 1;
      }

      if(global_host_client->padding00_used != 0 &&
         global_host_client->padding04 != 0 &&
         global_host_client->padding08 != 0 &&
         host_frametime + global_realtime > global_host_client->next_update_time) {

         global_host_client->padding3478_SendNextTick = 1;
      }

      if(global_host_client->netchan1C.netchan_message.overflow & 2) { //too much.

         SZ_Clear(&(global_host_client->netchan1C.netchan_message));
         SZ_Clear(&(global_host_client->sizebuf24AC));
         SV_BroadcastPrintf("%s overflowed\n", global_host_client->PlayerName);
         Con_Printf("%s: reliable overflow for %s\n", __FUNCTION__, global_host_client->PlayerName);
         SV_DropClient(global_host_client, 0, "Reliable channel overflowed (I feel your pain, modemer)");
         global_host_client->padding3478_SendNextTick = 1;
         global_host_client->netchan1C.cleartime = 0;   // don't choke this message
      }
      else if(global_host_client->padding3478_SendNextTick != 0 &&
         (global_realtime - global_host_client->netchan1C.last_received) > cvar_sv_failuretime.value) { //normal operation

         global_host_client->padding3478_SendNextTick = 0;
      }

      if(global_host_client->padding3478_SendNextTick == 0) {

         continue;
      }

      if(Netchan_CanPacket(&(global_host_client->netchan1C)) == 0) {

         global_host_client->padding2430++;
         continue;
      }

      global_host_client->padding3478_SendNextTick = 0;
      global_host_client->next_update_time = host_frametime + global_realtime + global_host_client->cl_updaterate;

      if(global_host_client->padding00_used != 0 &&
         global_host_client->padding04 != 0 &&
         global_host_client->padding08 != 0) {

         SV_SendClientDatagram(global_host_client);
      }
      else {
         Netchan_Transmit(&(global_host_client->netchan1C), 0, 0);
      }
   }

   SV_CleanupEnts();
}

/* DESCRIPTION: SV_ReadPackets
// LOCATION: sv_main.c
// PATH: SV_Frame
//
// Ah, the heart and soul of the server.
*/
void SV_ReadPackets() {

   unsigned int i;
   client_t *cl;

   while(NET_GetPacket(NS_SERVER) != 0) {

      if(SV_FilterPacket() == 1) { continue; }
      if(*(unsigned int *)global_net_message.data == 0xFFFFFFFF) {
         SV_ConnectionlessPacket();
         continue;
      }

      //looking for the sender.
      for(i = 0, cl = global_svs.clients; i < global_svs.allocated_client_slots; i++, cl++) {

         if((cl->padding00_used == 0 && cl->padding04 == 0 && cl->padding0C_connected == 0) ||
             NET_CompareAdr(global_net_from, cl->netchan1C.remote_address) == 0) { continue; }

         //If we are here, we have a match.
         if(Netchan_Process(&(cl->netchan1C)) != 0) {

            if(global_svs.allocated_client_slots == 1 ||
               cl->padding00_used == 0 || cl->padding04 == 0 || cl->padding08 == 0) {
               cl->padding3478_SendNextTick = 1;
            }

            SV_ExecuteClientMessage(cl);
            gGlobalVariables.frametime = host_frametime;
         }
         if(Netchan_IncomingReady(&(cl->netchan1C)) != 0) {

            if(Netchan_CopyNormalFragments(&(cl->netchan1C)) != 0) {
               MSG_BeginReading();
               SV_ExecuteClientMessage(cl);
            }
            if(Netchan_CopyFileFragments(&(cl->netchan1C)) != 0) {
               global_host_client = cl;
               SV_ProcessFile(cl, cl->netchan1C.filename);
            }
         }
         break;
      }
   }
}

void SV_Packet_Init() {

   Cvar_RegisterVariable(&cvar_sv_voicecodec);
   Cvar_RegisterVariable(&cvar_sv_voiceenable);
   Cvar_RegisterVariable(&cvar_sv_instancedbaseline);
   Cvar_RegisterVariable(&cvar_host_limitlocal);
   Cvar_RegisterVariable(&cvar_sv_failuretime);
}
