#include <ctype.h>

#include "SV_mcast.h"
#include "common.h"
#include "cmd.h"
#include "Modding.h"
#include "report.h"
#include "SV_clres.h"
#include "sys.h"
#include "world.h"

//SV_ValidClientMulticast: Bad argument 132

/* DESCRIPTION: SV_PointLeafnum
// LOCATION: by name, gl_q2bsp.c, but the 'point' functions are spread out.
// PATH: SV_Multicast, SV_ValidClientMulticast, EV_Playback
//
// Finds the index of a leaf, relative to the worldmodel leaves. Or, in other
// words, we're solving for i: our leaf == sv.worldmodel->leafs[i]
*/
int SV_PointLeafnum(vec3_t vector) {

   mleaf_t * leaf;


   leaf = Mod_PointInLeaf(vector, global_sv.worldmodel);
   if(leaf == NULL) { return(0); }

   return(leaf - global_sv.worldmodel->leafs);
}
/* DESCRIPTION: SV_ValidClientMulticast
// LOCATION:
// PATH: SV_Multicast, EV_Playback
//
// We send and send and send and send and send send send send send send send
*/
int SV_ValidClientMulticast(client_t * cl, int leafnum, int arg_8) {

   byte * row;
   int index;
   //We aren't a singleplayer game, so I can't just return 1 and be done...

   if(cl->padding243C_proxy != 0) { return(1); }


   arg_8 &= 0x7f;
   switch(arg_8) {

   case 1:
      return(1);

   case 2:
      row = CM_LeafPVS(leafnum);
      break;

   case 4:
      row = CM_LeafPAS(leafnum);
      break;

   default:

      Con_Printf("%s: Bad argument %i", __FUNCTION__, arg_8);
      return(0);
   }

   if(row == NULL) { return(1); }

   index = ((SV_PointLeafnum(cl->padding4A84->v.origin) - 1));
   //We are checking the var_Cth bit.  I know it's messy, but that's all we're doing.
   if((((row[index / 8]) >> (index & 7)) & 1) != 0) { return(1); }

   return(0);
}
/* DESCRIPTION: SV_Multicast
// LOCATION:
// PATH: SV_StartSound, PF_MessageEnd_I
//
// Just does some bit checking.  How odd.
*/
void SV_Multicast(edict_t * ent, vec3_t origin, int arg_8_flags, int arg_c_bool) {

   int leafnum;
   unsigned int i;
   client_t * oldclient, * client;


   oldclient = global_host_client;
   leafnum = SV_PointLeafnum(origin);

   if(ent != NULL && (global_host_client == NULL || global_host_client->padding4A84 != ent)) {
      for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

         if(client->padding4A84 == ent) {
            global_host_client = client;
            break;
         }
      }
   }

   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0 || (client == global_host_client && (arg_8_flags & 0x80) != 0)) { continue; }
      if(ent != NULL && ent->v.groupinfo != 0 && client->padding4A84->v.groupinfo != 0) {
         if((global_g_groupop == 0 && (client->padding4A84->v.groupinfo & ent->v.groupinfo) == 0) ||
            (global_g_groupop == 1 && (client->padding4A84->v.groupinfo & ent->v.groupinfo) != 0)) {
            continue;
         }
      }
      if(SV_ValidClientMulticast(client, leafnum, arg_8_flags) == 0) { continue; }


      if(arg_c_bool == 0) { //reliable I think

         if(client->sizebuf24AC.maxsize - client->sizebuf24AC.cursize > global_sv.sizebuf3dbd0_multicast.cursize) {
            SZ_Write(&(client->sizebuf24AC), global_sv.sizebuf3dbd0_multicast.data, global_sv.sizebuf3dbd0_multicast.cursize);
         }
      }
      else {

         if(client->netchan1C.netchan_message.maxsize - client->netchan1C.netchan_message.cursize > global_sv.sizebuf3dbd0_multicast.cursize) {
            SZ_Write(&(client->netchan1C.netchan_message), global_sv.sizebuf3dbd0_multicast.data, global_sv.sizebuf3dbd0_multicast.cursize);
         }
      }
   }

   SZ_Clear(&(global_sv.sizebuf3dbd0_multicast));
   global_host_client = oldclient;
}


/* DESCRIPTION: SV_HashString
// LOCATION: The SDK
// PATH: Wherever a hash table is used
//
// Hashes a string.  Nothing more.  Very oddly.
*/
unsigned int SV_HashString(const char * String, unsigned int max) {

   unsigned int hash = 0;
   unsigned char temp = tolower(*String);

   while(temp != '\0') {

      hash += temp | (temp << 8) | (temp << 16) | (temp << 24); //deep, complicated hashing algorithm here.
      String++;
      temp = tolower(*String);
   }
   return(hash % max);
}
void SV_AddSampleToHashedLookupTable(const char * sample, int num) {

   unsigned int i, var_8_hash;


   i = var_8_hash = SV_HashString(sample, 0x3FF);

   while(global_sv.padding32B48[i] != 0) { //hash collision

      i++;
      if(i > 0x3fe) { i = 0; }
      if(i == var_8_hash) {
         Sys_Error("%s: No free slots in lookup table.\n", __FUNCTION__);
      }
   }

   global_sv.padding32B48[i] = num;
}
void SV_BuildHashedSoundLookupTable() {

   int i;


   Q_memset(global_sv.padding32B48, 0, 0x7fe);
   for(i = 0; i < 0x200; i++) {

      if(global_sv.sound_precache[i] == NULL) { break; }
      SV_AddSampleToHashedLookupTable(global_sv.sound_precache[i], i);
   }


   global_sv.padding33348 = 1;
}
int SV_LookupSoundIndex(const char * sample) {

   unsigned int i, var_8_hash;


   var_8_hash = SV_HashString(sample, 0x3FF);

   if(global_sv.padding33348 == 0) {
      if(global_sv.padding3bc64 == 1) {

         for(i = 1; i < 0x200; i++) {

            if(global_sv.sound_precache[i] == NULL) { break; }
            if(Q_strcasecmp(sample, global_sv.sound_precache[i]) == 0) { return(i); }
         }
         return(0);
      }

      SV_BuildHashedSoundLookupTable();
   }

   i = var_8_hash;
   while(global_sv.padding32B48[i] != 0) {

      if(Q_strcasecmp(sample, global_sv.sound_precache[global_sv.padding32B48[i]]) == 0) {
         return(global_sv.padding32B48[i]);
      }

      i++;
      if(i > 0x3fe) { i = 0; }
      if(i == var_8_hash) { break; }
   }

   return(0);
}

int SV_BuildSoundMsg(edict_t * ent, int channel, const char * sample, int volume, float attenuation, int flags, int pitch, const vec3_t position, sizebuf_t * sz) {

   int var_4_SoundIndex;
   int var_C_edictnum;
   int var_8;


   if(volume < 0 || volume > 255) {
      Sys_Error("%s: volume = %i", __FUNCTION__, volume);
   }
   if(attenuation < 0 || attenuation > 4) {
      Sys_Error("%s: attenuation = %d", __FUNCTION__, attenuation);
   }
   if(channel < 0 || channel > 7) {
      Sys_Error("%s: channel = %i", __FUNCTION__, channel);
   }
   if(pitch < 0 || pitch > 255) {
      Sys_Error("%s: pitch = %i", __FUNCTION__, pitch);
   }
   if(sample == NULL) { //I find it doubtful taht this can be legit.
      Sys_Error("%s: NULL sample", __FUNCTION__);
   }

   if(sample[0] == '!') {

      flags |= 0x10;
      var_4_SoundIndex = Q_atoi(sample+1);
      if(var_4_SoundIndex >= 0x600) {

         Con_Printf("%s: Invalid sentence number %s.\n", __FUNCTION__, sample);
         return(0);
      }
   }
   else if(sample[0] == '#') {

      flags |= 0x10;
      var_4_SoundIndex = Q_atoi(sample+1) + 0x600;
   }
   else {

      var_4_SoundIndex = SV_LookupSoundIndex(sample);
      if(var_4_SoundIndex == 0 || global_sv.sound_precache[var_4_SoundIndex] == NULL) {

         Con_Printf("%s: Sound %s not precached.\n", __FUNCTION__, sample);
         return(0);
      }
   }


   var_C_edictnum = NUM_FOR_EDICT(ent);
   var_8 = flags;
   if(volume != 255) { var_8 |= 1; }
   if(attenuation != 1) { var_8 |= 2; }
   if(pitch != 100) { var_8 |= 8; }
   if(var_4_SoundIndex > 255) { var_8 |= 4; }

   MSG_WriteByte(sz, 0x06);

   MSG_StartBitWriting(sz);
   MSG_WriteBits(var_8, 9);
   if(var_8 & 1) { MSG_WriteBits(volume, 8); }
   if(var_8 & 2) { MSG_WriteBits((int)(attenuation * 64), 8); }
   MSG_WriteBits(channel, 3);
   MSG_WriteBits(var_C_edictnum, 0x0B);

   if(var_4_SoundIndex > 255) {
      MSG_WriteBits(var_4_SoundIndex, 16);
   }
   else {
      MSG_WriteBits(var_4_SoundIndex, 8);
   }

   MSG_WriteBitVec3Coord(position);
   if(var_8 & 8) { MSG_WriteBits(pitch, 8); }

   MSG_EndBitWriting(sz);
   return(1);
}
void SV_StartSound(int arg_0, edict_t * ent, int channel, const char * sample, int volume, float attenuation, int flags, int pitch) {

   int i;
   int var_24, var_8;
   vec3_t position;


   for(i = 0; i < 3; i++) {
      position[i] = ent->v.origin[i] + ((ent->v.mins[i] + ent->v.maxs[i]) / 2);
   }

   if(SV_BuildSoundMsg(ent, channel, sample, volume, attenuation, flags, pitch, position, &global_sv.sizebuf3dbd0_multicast) == 0) {
      return;
   }


   if(channel == 6 || (flags & 0x20) != 0) { var_8 = 0; }
   else { var_8 = 1; }

   if(arg_0 == 1) { var_24 = 0x80; } //Basicanlly a Don't Send to Originating Player flag
   else { var_24 = 0; }

   if(var_8 != 0) {
      SV_Multicast(ent, position, var_24 | 4, 0);
   }
   else {
      SV_Multicast(ent, position, var_24 | 1, 1);
   }
}
HLDS_DLLEXPORT void PM_SV_PlaySound(int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch) {

   SV_StartSound(1, global_host_client->padding4A84, channel, sample, volume * 255, attenuation, fFlags, pitch);
}

void EV_PlayReliableEvent(client_t * cl, int edictindex, int eventindex, float delay, event_args_t * event) {

   sizebuf_t buffer;
   event_args_t nullevent, tempevent2;
   char databuf[0x400];


   if(cl->padding2438_IsFakeClient != 0) { return; }

   buffer.debugname = "Reliable Event";
   buffer.overflow = 0;
   buffer.data = databuf;
   buffer.maxsize = 0x400;
   buffer.cursize = 0;

   Q_memset(&nullevent, 0, sizeof(event_args_t));
   tempevent2 = *event;
   tempevent2.entindex = edictindex;


   MSG_WriteByte(&buffer, 0x15);
   MSG_StartBitWriting(&buffer);
   MSG_WriteBits(eventindex, 0x0A);
   DELTA_WriteDelta((byte *)(&nullevent), (byte *)(&tempevent2), 1, SV_LookupDelta("event_t"), 0);

   if(delay != 0) {
      MSG_WriteBits(1, 1);
      MSG_WriteBits(delay * 100, 0x10);
   }
   else {
      MSG_WriteBits(0, 1);
   }

   MSG_EndBitWriting(&buffer);
   if(cl->netchan1C.netchan_message.cursize + buffer.cursize > cl->netchan1C.netchan_message.maxsize) {
      Netchan_CreateFragments(1, &(cl->netchan1C), &buffer);
   }
   else {
      SZ_Write(&(cl->netchan1C.netchan_message), buffer.data, buffer.cursize);
   }
}

HLDS_DLLEXPORT void EV_Playback(int flags, edict_t * passedClient, unsigned short eventindex, float delay, vec3_t origin, vec3_t angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {

   unsigned int i, j;
   int var_1c, var_20_leafnum, var_2c_index, var_7C_edictnum;
   event_info_t * var_24_event_info, * var_28_event_info_p;
   event_args_t var_74_event;
   vec3_t var_18_vecpoint;
   client_t * var_8_client;


   if((flags & 0x40) != 0) { return; }

   Q_memset(&var_74_event, 0, sizeof(event_args_t));
   if(origin[0] != 0 || origin[1] != 0 || origin[2] != 0) {

      var_74_event.origin[0] = origin[0];
      var_74_event.origin[1] = origin[1];
      var_74_event.origin[2] = origin[2];
      var_74_event.flags |= 1;
   }
   if(angles[0] != 0 || angles[1] != 0 || angles[2] != 0) {

      var_74_event.angles[0] = angles[0];
      var_74_event.angles[1] = angles[1];
      var_74_event.angles[2] = angles[2];
      var_74_event.flags |= 2;
   }

   var_74_event.fparam1 = fparam1;
   var_74_event.fparam2 = fparam2;
   var_74_event.iparam1 = iparam1;
   var_74_event.iparam2 = iparam2;
   var_74_event.bparam1 = bparam1;
   var_74_event.bparam2 = bparam2;

   //That's that done.
   if(eventindex == 0 || eventindex > 255) {
      Con_Printf("%s: index %i out of range.\n", __FUNCTION__, eventindex);
      return;
   }
   if(global_sv.padding31348[eventindex].mem == NULL) {
      Con_Printf("%s: index %i does not point to an event.\n", __FUNCTION__, eventindex);
      return;
   }

   if(passedClient == NULL) {

      var_18_vecpoint[0] = var_74_event.origin[0];
      var_18_vecpoint[1] = var_74_event.origin[1];
      var_18_vecpoint[2] = var_74_event.origin[2];
      var_7C_edictnum = -1;
   }
   else {

      var_18_vecpoint[0] = passedClient->v.origin[0];
      var_18_vecpoint[1] = passedClient->v.origin[1];
      var_18_vecpoint[2] = passedClient->v.origin[2];
      var_7C_edictnum = NUM_FOR_EDICT(passedClient);

      if(var_7C_edictnum > 0 && (unsigned)var_7C_edictnum <= global_svs.allocated_client_slots && (passedClient->v.flags & FL_DUCKING)) {
         var_74_event.ducking = 1;
      }
      if((var_74_event.flags & 1) == 0) {
         var_74_event.origin[0] = passedClient->v.origin[0];
         var_74_event.origin[1] = passedClient->v.origin[1];
         var_74_event.origin[2] = passedClient->v.origin[2];
      }
      if((var_74_event.flags & 2) == 0) {
         var_74_event.angles[0] = passedClient->v.angles[0];
         var_74_event.angles[1] = passedClient->v.angles[1];
         var_74_event.angles[2] = passedClient->v.angles[2];
      }
   }

   var_1c = 4;
   var_20_leafnum = SV_PointLeafnum(var_18_vecpoint); //This is in the QW.

   for(i = 0; i < global_svs.allocated_client_slots; i++) {

      var_8_client = &(global_svs.clients[i]);
      if(var_8_client->padding00_used == 0 || var_8_client->padding04 == 0 ||
         var_8_client->padding08 == 0 || var_8_client->padding0C_connected == 0 ||
         var_8_client->padding2438_IsFakeClient != 0) {

         continue;
      }

      if(passedClient != NULL && passedClient->v.groupinfo != 0 &&
         var_8_client->padding4A84->v.groupinfo != 0) {

         if((global_g_groupop == 0 && (var_8_client->padding4A84->v.groupinfo & passedClient->v.groupinfo) == 0) ||
            (global_g_groupop == 1 && (var_8_client->padding4A84->v.groupinfo & passedClient->v.groupinfo) != 0)) {

            continue;
         }
      }

      if((flags & 4) == 0 && passedClient != NULL &&
         SV_ValidClientMulticast(var_8_client, var_20_leafnum, var_1c) == 0) {
         continue;
      }

      if(global_host_client == var_8_client && (flags & 1) != 0 && var_8_client->cl_lw != 0) {

         continue;
      }
      if((flags & 0x10) != 0 && var_8_client->padding4A84 != passedClient) {

         continue;
      }
      if((flags & 2) != 0) {

         if(passedClient == NULL) {
            EV_PlayReliableEvent(var_8_client, 0, eventindex, delay, &var_74_event);
         }
         else {
            EV_PlayReliableEvent(var_8_client, NUM_FOR_EDICT(passedClient), eventindex, delay, &var_74_event);
         }
         continue;
      }

      var_24_event_info = var_8_client->events;
      var_2c_index = -1;

      if((flags & 8) != 0) {
         for(j = 0; j < 0x40; j++) {

            var_28_event_info_p = &(var_24_event_info[j]);
            if(var_28_event_info_p->index == eventindex && var_7C_edictnum != -1 && var_7C_edictnum == var_28_event_info_p->entity_index) {

               var_2c_index = j;
               break;
            }
         }
      }

      if(var_2c_index == -1) {
         for(j = 0; j < 0x40; j++) {

            var_28_event_info_p = &(var_24_event_info[j]);
            if(var_28_event_info_p->index == 0) {

               var_2c_index = j;
               break;
            }
         }

         if(var_2c_index != -1 && j > 0x3F) {

            var_28_event_info_p = &(var_24_event_info[var_2c_index]);
         }
      }

      if(var_2c_index == -1) { continue; }

      var_28_event_info_p->index = eventindex;
      var_28_event_info_p->packet_index = -1;

      if(passedClient == NULL) {
         var_28_event_info_p->entity_index = -1;
      }
      else {
         var_28_event_info_p->entity_index = var_7C_edictnum;
      }

      var_28_event_info_p->args = var_74_event;
      var_28_event_info_p->fire_time = delay;
   }
}
void EV_SV_Playback(int flags, int clientindex, unsigned short eventindex, float delay, vec3_t origin, vec3_t angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {

   if((flags & 0x40) != 0) { return; }
   if(clientindex < 0 || (unsigned)clientindex >= global_svs.allocated_client_slots) {

      Sys_Error("%s:  Client index %i out of range.\n", __FUNCTION__, clientindex);
      return;
   }

   EV_Playback(flags, global_svs.clients[clientindex].padding4A84, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}
HLDS_DLLEXPORT void PM_SV_PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, vec3_t origin, vec3_t angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {
   EV_SV_Playback(flags | 1, clientindex, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}


/* DESCRIPTION: SV_FullClientUpdate
// LOCATION:
// PATH: a few
//
// Sends a packet.  I don't really know what's so special about it.
*/
void SV_FullClientUpdate(client_t * client, sizebuf_t *buf) {

   int i;
   char info[0x100];
   MD5Context_t checksum;
   char hash[16];


   i = client - global_svs.clients;

   Q_strncpy(info, client->InfoKeyBuffer, sizeof(info)-1);
   info[sizeof(info)-1] = '\0';

   Info_RemovePrefixedKeys(info, '_');

   MSG_WriteByte(buf, 0x0D);
   MSG_WriteByte(buf, i);
   MSG_WriteLong(buf, client->padding4A8C_PlayerUID);
   MSG_WriteString(buf, info);

   MD5Init(&checksum);
   MD5Update(&checksum, client->authentication_CD_key, sizeof(client->authentication_CD_key));
   MD5Final(hash, &checksum);

   MSG_WriteBuf(buf, sizeof(hash), hash);
}
/* DESCRIPTION: SV_ForceFullClientsUpdate
// LOCATION:
// PATH: SV_RunCmd, SV_FullUpdate_f
//
// Sends some data, bla bla bla.
*/
void SV_ForceFullClientsUpdate() {

   unsigned int i;
   char buf[0x2400];
   client_t * cl;
   sizebuf_t sz;

   sz.debugname = "Force Update";
   sz.overflow = 0;
   sz.data = buf;
   sz.maxsize = sizeof(buf);
   sz.cursize = 0;


   cl = &global_svs.clients[0];
   for(i = 0; i < global_svs.allocated_client_slots; i++, cl++) {

      if(global_host_client != cl && cl->padding00_used == 0 &&
         cl->padding0C_connected == 0 && cl->padding04 == 0) { continue; }

      SV_FullClientUpdate(cl, &sz);
   }

   Con_Printf("%s: Client %s started recording. Send full update.", __FUNCTION__, cl->PlayerName);

   Netchan_CreateFragments(1, &(global_host_client->netchan1C), &sz);
   Netchan_FragSend(&(global_host_client->netchan1C));
}


/* DESCRIPTION: SV_BroadcastCommand
// LOCATION: SV_Send.c
// PATH: Cvar_DirectSet and SV_Serverinfo_f
//
// This sends a message to all connected clients (according to QW).
*/
void SV_BroadcastCommand(const char * format, ...) {

   char array[1024]; //128 or 1024?  Doesn't seem to matter.
   int length;
   unsigned int i;

   client_t * clients;
   va_list argptr;

   if(global_sv.active == 0) { return; }

   array[0] = 9;

   va_start(argptr, format);
   length = Q_vsnprintf(array+1, 1023, format, argptr) + 1;
   va_end(argptr);

   //Overflow testing.  vsnprintf (and all nprintfs) return the number of
   //chars that would have been written if there was enough room.
   //If we overflowed, this number will be greater than 1023 (1023 - 1 for
   //the null and +1 for the nine).  In that case we have a non-null
   //terminated array and a sys error to throw.
   if(length > 1023) {
      Sys_Error("SV_BroadcastCommand: Overflowed when writing message.\n");
   }

   for(i = 0, clients = global_svs.clients; i < global_svs.allocated_client_slots; i++, clients++) {

      if((clients->padding00_used == 0 && clients->padding0C_connected == 0 && clients->padding04 == 0) ||
         (clients->padding2438_IsFakeClient != 0)) { continue; }
      SZ_Write(&clients->netchan1C.netchan_message, array, length);
   }
}
void SV_BroadcastPrintf(const char * format, ...) {

   char array[1024]; //128 or 1024?  Doesn't seem to matter.
   int length;
   unsigned int i;

   client_t * clients;
   va_list argptr;

   if(global_sv.active == 0) { return; }

   array[0] = 8;

   va_start(argptr, format);
   length = Q_vsnprintf(array+1, 1023, format, argptr) + 1;
   va_end(argptr);

   //Overflow testing.  vsnprintf (and all nprintfs) return the number of
   //chars that would have been written if there was enough room.
   //If we overflowed, this number will be greater than 1023 (1023 - 1 for
   //the null and +1 for the nine).  In that case we have a non-null
   //terminated array and a sys error to throw.
   if(length > 1023) {
      Sys_Error("SV_BroadcastPrintf: Overflowed when writing message.\n");
   }

   for(i = 0, clients = global_svs.clients; i < global_svs.allocated_client_slots; i++, clients++) {

      if((clients->padding00_used == 0 && clients->padding04 == 0) ||
         (clients->padding2438_IsFakeClient != 0)) { continue; }
      SZ_Write(&clients->netchan1C.netchan_message, array, length);
   }
}
/* DESCRIPTION: SV_ClientPrintf
// LOCATION:
// PATH: Lots of kick and ban functions.
//
// Prints a client message.  Not an 'l' message--some new one.
// I've simplified it by writing the data directly to the stream.
*/
void SV_ClientPrintf(const char *format, ...) {

   va_list argptr;
   int sizeleft, written;


   if(global_host_client->padding2438_IsFakeClient) { return; }

   sizeleft = global_host_client->netchan1C.netchan_message.maxsize - global_host_client->netchan1C.netchan_message.cursize;
   if(sizeleft < 2) { return; } //one for the identifier, one for the null

   global_host_client->netchan1C.netchan_message.data[global_host_client->netchan1C.netchan_message.cursize] = 0x08;

   va_start(argptr, format);
   written = Q_vsnprintf(global_host_client->netchan1C.netchan_message.data + global_host_client->netchan1C.netchan_message.cursize + 1, sizeleft - 1, format, argptr) + 1;
   va_end(argptr);

   if(written > sizeleft) { //Message was too big.

      global_host_client->netchan1C.netchan_message.data[global_host_client->netchan1C.netchan_message.maxsize-1] = '\0';
      written = sizeleft;
   }

   global_host_client->netchan1C.netchan_message.cursize += written + 1;
}
/* DESCRIPTION: SV_DropClient
// LOCATION: sv_main.c
// PATH: many
//
// Sends the packet notifying the poor client of his failure
// to live up to our expectations.  Also sends the update
// to other connected clients.  This is fr maintaining
// a clean client list, so it's not used if the server itself
// is terminating.
*/
void SV_DropClient(client_t * client, int ClientHasCrashed, char * message, ...) {

   char array2[1024];
   int i;
   float tempfloat;
   va_list argptr;


   va_start(argptr, message);
   i = Q_vsnprintf(array2, sizeof(array2)-1, message, argptr) + 1;
   va_end(argptr);

   if(i > (signed)sizeof(array2)) {
      Con_Printf("SV_DropClientn: A client drop message had to be truncated.\n");
      array2[sizeof(array2) - 1] = '\0';
      i = sizeof(array2);
   }


   if(ClientHasCrashed == 0) {

      Con_Printf("Dropped %s, reason: %s\n", client->PlayerName, array2);

      if(client->padding2438_IsFakeClient == 0) {

         MSG_WriteByte(&client->netchan1C.netchan_message, 2);
         MSG_WriteBuf(&client->netchan1C.netchan_message, i, array2);

         //The broadcast message will be shorter if needed.
         if(i > 63) {
            i = 64;
            array2[63] = '\0';
         }
      }

      if(client->padding4A84 != NULL && client->padding04 != 0) {
         gEntityInterface.pfnClientDisconnect(client->padding4A84);
      }

      if(client->padding2438_IsFakeClient == 0) {
         Netchan_Transmit(&client->netchan1C, i, array2);
      }
   }

   tempfloat = global_realtime - client->netchan1C.first_received;
   if(tempfloat >= 0) {

      global_svs.padding6C++;
      global_svs.padding70 += tempfloat;
   }

   Netchan_Clear(&client->netchan1C);

   //I am ignoring a steam call.  Boo to steam.

   client->padding00_used = 0;
   client->padding04 = 0;
   client->padding08 = 0;
   client->padding0C_connected = 0;
   client->padding14 = 0;
   client->padding2438_IsFakeClient = 0;
   client->padding243C_proxy = 0;
   client->PlayerName[0] = '\0';
   client->padding3460_connection_started = global_realtime;

   COM_ClearCustomizationList(&client->padding4D30);

   client->padding4A84 = NULL;

   memset(client->InfoKeyBuffer, 0, 0x100);
   memset(client->UnknownStruct4DE0, 0, 0x100);

   SV_FullClientUpdate(client, &global_sv.sizebuf3CC1C);
   //NotifyDedicatedServerUI, that's surely just printing stuff, ignored.
}

/* DESCRIPTION: SV_RejectConnection
// LOCATION:
// PATH: All over
//
// So Long, Farewell, Auf Wiedersehen, Goodbye.
*/
void SV_RejectConnection(netadr_t * client, char * message, ...) {

   unsigned int i;
   char TerminationMessage[1024];
   va_list argptr;


   va_start(argptr, message);
   i = Q_vsnprintf(TerminationMessage, sizeof(TerminationMessage) - 1, message, argptr);
   va_end(argptr);

   // remember, 'i' holds the number of bytes that WOULD have been written, had
   // the buffer been infinite.

   if(i > sizeof(TerminationMessage) - 1) {
      Con_Printf("SV_RejectConnection: A client rejection message had to be truncated.  What, one kilo isn't ENOUGH?\n");
      TerminationMessage[sizeof(TerminationMessage) - 1] = '\0';
      i = sizeof(TerminationMessage) - 1;
   }

   SZ_Clear(&global_net_message);

   MSG_WriteLong(&global_net_message, 0xFFFFFFFF);
   MSG_WriteByte(&global_net_message, 57);
   MSG_WriteBuf(&global_net_message, i, TerminationMessage);
   NET_SendPacket(1, global_net_message.cursize, global_net_message.data, *client);

   //this also calls _WON_RemoveUserbyAddress.
}
void SV_RejectConnectionForPassword(netadr_t * client) {

   SZ_Clear(&global_net_message);

   MSG_WriteLong(&global_net_message, 0xFFFFFFFF);
   MSG_WriteByte(&global_net_message, 56);
   MSG_WriteString(&global_net_message, "BADPASSWORD");
   NET_SendPacket(1, global_net_message.cursize, global_net_message.data, *client);
}

