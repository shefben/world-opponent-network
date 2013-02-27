#include "SV_user.h"

#include <ctype.h>
#include "Modding.h"
#include "sys.h"
#include "VecMath.h"


/*** STRUCTS ***/
typedef struct temp_move_s { //0x54, has now been reordered since it seems to be unique.

   int IsActive; //00, this entity is being processed.
   int IsMoving; //04 destination and origin are not the same.
   int FirstIteration; //54
   int DoNotInterpolate; //44

   vec3_t Mins; //2C
   vec3_t Maxs; //38
   vec3_t CurrentPosition; //20
   vec3_t OldLocation; //14
   vec3_t NewLocation; //08
   vec3_t FinalVectors; //48
} temp_move_t;


/*** GLOBALS ***/

unsigned int SV_UPDATE_MASK = 7;
unsigned int SV_UPDATE_BACKUP = 8;

static int nofind = 0;
static temp_move_t truepositions[32]; //That'll need to be dynamic.  Can't have 600 players elsewise.

static cvar_t cvar_sv_maxrate       = {       "sv_maxrate",    "0", FCVAR_SERVER,  0.0, NULL };
static cvar_t cvar_sv_minrate       = {       "sv_minrate",    "0", FCVAR_SERVER,  0.0, NULL };
static cvar_t cvar_sv_maxupdaterate = { "sv_maxupdaterate", "30.0", FCVAR_NONE,   30.0, NULL };
static cvar_t cvar_sv_minupdaterate = { "sv_minupdaterate", "10.0", FCVAR_NONE,   10.0, NULL };
static cvar_t cvar_sv_unlag         = {         "sv_unlag",    "1", FCVAR_NONE,    1.0, NULL };
static cvar_t cvar_sv_maxunlag      = {      "sv_maxunlag",  "0.5", FCVAR_NONE,    0.5, NULL };
static cvar_t cvar_sv_unlagpush     = {     "sv_unlagpush",  "0.0", FCVAR_NONE,    0.0, NULL };
static cvar_t cvar_sv_unlagsamples  = {  "sv_unlagsamples",    "1", FCVAR_NONE,    1.0, NULL };

/*** FUNCTIONS ***/


//general purpose

/* DESCRIPTION: SV_FindEntInPack
// LOCATION:
// PATH: SV_RunCmd->SV_SetupMove
//
// This wee little function simply loops around to find an entity in--
// you guessed it--a pack.  It's just a 'general' function that only gets
// used in this file.
*/
entity_state_t * SV_FindEntInPack(int index, packet_entities_t * PacksToSearch) {

   unsigned int i;


   for(i = 0; i < PacksToSearch->max_entities; i++) {

      if(PacksToSearch->entities[i].number == index) {
         return(&(PacksToSearch->entities[i]));
      }
   }

   return(NULL);
}
/* DESCRIPTION: SV_GetTrueOrigin
// LOCATION:
// PATH: SV_AddLinksToPM_->SV_CopyEdictToPhysent
//
// This calls upon those trueposition statics in order to properly report
// a player's position mid-move or something.  There are an awful lot of
// abort cases, such as no lag compensation.
*/
void SV_GetTrueOrigin(int edictnum, vec3_t out_origin) {

   if(global_host_client->cl_lw == 0 || global_host_client->cl_lc == 0 ||
      (int)cvar_sv_unlag.value == 0 ||
      global_svs.allocated_client_slots <= 1 ||
      global_host_client->padding00_used == 0 ||
      edictnum < 0 || (unsigned)edictnum >= global_svs.allocated_client_slots ||
      truepositions[edictnum].IsActive == 0 || truepositions[edictnum].IsMoving == 0) {

      return;
   }

   out_origin[0] = truepositions[edictnum].NewLocation[0];
   out_origin[1] = truepositions[edictnum].NewLocation[1];
   out_origin[2] = truepositions[edictnum].NewLocation[2];
}
/* DESCRIPTION: SV_GetTrueMinMax
// LOCATION:
// PATH: SV_AddLinksToPM_
//
// Once more, digs data out of the ststic vars.
*/
void SV_GetTrueMinMax(int edictnum, float ** out_vecmins, float ** out_vecmaxs) {

   if(global_host_client->cl_lw == 0 || global_host_client->cl_lc == 0 ||
      (int)cvar_sv_unlag.value == 0 ||
      global_svs.allocated_client_slots <= 1 ||
      global_host_client->padding00_used == 0 ||
      edictnum < 0 || (unsigned)edictnum >= global_svs.allocated_client_slots ||
      truepositions[edictnum].IsActive == 0 || truepositions[edictnum].IsMoving == 0) {

      return;
   }

   *out_vecmins = truepositions[edictnum].Mins;
   *out_vecmaxs = truepositions[edictnum].Maxs;

}
/* DESCRIPTION: SV_CopyEdictToPhysent
// LOCATION:
// PATH: SV_AddLinksToPM_
//
// This appears to be a struct converter.  Probably doesn't belong here, but
// things it calls are here, and it has to go somewhere.
*/
void SV_CopyEdictToPhysent(physent_t * out_physent, int edictnum, edict_t * in_ent) {

   model_t * ptr;


   out_physent->info = edictnum;
   out_physent->origin[0] = in_ent->v.origin[0];
   out_physent->origin[1] = in_ent->v.origin[1];
   out_physent->origin[2] = in_ent->v.origin[2];

   if(edictnum <= 0 || (unsigned)edictnum > global_svs.allocated_client_slots) {

      out_physent->player = 0;
   }
   else {

      SV_GetTrueOrigin(edictnum-1, out_physent->origin);
      out_physent->player = edictnum;
   }

   out_physent->angles[0] = in_ent->v.angles[0];
   out_physent->angles[1] = in_ent->v.angles[1];
   out_physent->angles[2] = in_ent->v.angles[2];
   out_physent->studiomodel = NULL;
   out_physent->rendermode = in_ent->v.rendermode;

   switch(in_ent->v.solid) {

   case SOLID_NOT:

      if(in_ent->v.modelindex == 0) {
         out_physent->model = NULL;
      }
      else {
         out_physent->model = global_sv.models[in_ent->v.modelindex];
         Q_strncpy(out_physent->name, out_physent->model->name, 0x1F);
         out_physent->name[0x1F] = '\0';
      }
      break;

   case SOLID_BBOX:

      out_physent->model = NULL;
      if(in_ent->v.modelindex == 0) {
         ptr = NULL;
      }
      else {
         ptr = global_sv.models[in_ent->v.modelindex];
      }
      if(ptr != NULL) { //This block simplified.
         if((ptr->flags & 0x200) != 0) {

            out_physent->studiomodel = ptr;
         }

         Q_strncpy(out_physent->name, ptr->name, 0x1F);
         out_physent->name[0x1F] = '\0';
      }

      out_physent->mins[0] = in_ent->v.mins[0];
      out_physent->mins[1] = in_ent->v.mins[1];
      out_physent->mins[2] = in_ent->v.mins[2];
      out_physent->maxs[0] = in_ent->v.maxs[0];
      out_physent->maxs[1] = in_ent->v.maxs[1];
      out_physent->maxs[2] = in_ent->v.maxs[2];
      break;

   case SOLID_BSP:

      out_physent->model = global_sv.models[in_ent->v.modelindex];
      Q_strncpy(out_physent->name, out_physent->model->name, 0x1F);
      out_physent->name[0x1F] = '\0';
      break;

   default:

      out_physent->model = NULL;

      out_physent->mins[0] = in_ent->v.mins[0];
      out_physent->mins[1] = in_ent->v.mins[1];
      out_physent->mins[2] = in_ent->v.mins[2];
      out_physent->maxs[0] = in_ent->v.maxs[0];
      out_physent->maxs[1] = in_ent->v.maxs[1];
      out_physent->maxs[2] = in_ent->v.maxs[2];

      if(in_ent->v.classname == 0) {
         Q_strcpy(out_physent->name, "?");
      }
      else {
         Q_strncpy(out_physent->name, global_pr_strings + in_ent->v.classname, 0x1F);
         out_physent->name[0x1F] = '\0';
      }
   }

   //No more branches from this point.
   out_physent->solid = in_ent->v.solid;
   out_physent->skin = in_ent->v.skin;
   out_physent->frame = in_ent->v.frame;
   out_physent->sequence = in_ent->v.sequence;

   Q_memcpy(out_physent->controller, in_ent->v.controller, 4);
   Q_memcpy(out_physent->blending, in_ent->v.blending, 2);

   out_physent->movetype = in_ent->v.movetype;
   out_physent->takedamage = 0;
   out_physent->blooddecal = 0;

   out_physent->iuser1 = in_ent->v.iuser1;
   out_physent->iuser2 = in_ent->v.iuser2;
   out_physent->iuser3 = in_ent->v.iuser3;
   out_physent->iuser4 = in_ent->v.iuser4;
   out_physent->fuser1 = in_ent->v.fuser1;
   out_physent->fuser2 = in_ent->v.fuser2;
   out_physent->fuser3 = in_ent->v.fuser3;
   out_physent->fuser4 = in_ent->v.fuser4;
   out_physent->vuser1[0] = in_ent->v.vuser1[0];
   out_physent->vuser1[1] = in_ent->v.vuser1[1];
   out_physent->vuser1[2] = in_ent->v.vuser1[2];
   out_physent->vuser2[0] = in_ent->v.vuser2[0];
   out_physent->vuser2[1] = in_ent->v.vuser2[1];
   out_physent->vuser2[2] = in_ent->v.vuser2[2];
   out_physent->vuser3[0] = in_ent->v.vuser3[0];
   out_physent->vuser3[1] = in_ent->v.vuser3[1];
   out_physent->vuser3[2] = in_ent->v.vuser3[2];
   out_physent->vuser4[0] = in_ent->v.vuser4[0];
   out_physent->vuser4[1] = in_ent->v.vuser4[1];
   out_physent->vuser4[2] = in_ent->v.vuser4[2];
}
/* DESCRIPTION: SetMinMaxSize
// LOCATION:
// PATH: lots
//
// Sets the min, max, and size of an edict.  The min and max is given;
// the size isn't too hard to work out.
// The fourth argument triggers an extra long branch in the function.
// For some reason, it's assigned a value right before it's used in a
// comparison, guaranteeing the code won't ever be reached.
*/
void SetMinMaxSize(edict_t * ed, const vec3_t mins, const vec3_t maxs, int ChangedByMod) {

   int i;


   for(i = 0; i < 3; i++) {
      if(mins[i] > maxs[i]) {
         Sys_Error("%s: mismatched mins/maxs.", __FUNCTION__);
      }
   }

   //There's a very large unreachable block of code here
   ed->v.mins[0] = mins[0];
   ed->v.mins[1] = mins[1];
   ed->v.mins[2] = mins[2];
   ed->v.maxs[0] = maxs[0];
   ed->v.maxs[1] = maxs[1];
   ed->v.maxs[2] = maxs[2];
   ed->v.size[0] = maxs[0] - mins[0];
   ed->v.size[1] = maxs[1] - mins[1];
   ed->v.size[2] = maxs[2] - mins[2];

   SV_LinkEdict(ed, 0);
}


//userinfo based

/* DESCRIPTION: SV_CheckRate/SV_CheckUpdateRate
// PATH: SV_ExtractFromUserinfo
//
// Simply make sure the value given is within acceptable limits.
*/
void SV_CheckRate(client_t *cl) {

   float temp;


   temp = cvar_sv_maxrate.value;
   if(temp > 0 && cl->netchan1C.rate > temp) {
      cl->netchan1C.rate = temp;
   }

   temp = cvar_sv_minrate.value;
   if(temp > 0 && cl->netchan1C.rate < temp) {
      cl->netchan1C.rate = temp;
   }
}
void SV_CheckUpdateRate(double * rate) {

   float temp;


   if((*rate) == 0) {
      *rate = (1.0 / 20.0);
   }
   else {

      temp = cvar_sv_maxupdaterate.value;
      if(temp != 0) {
         if(temp <= 0.001) {
            Cvar_DirectSetValue(&cvar_sv_maxupdaterate, 30.0);
            temp = 30;
         }
         if(*rate < 1.0 / temp) {
            *rate = 1.0 / temp;
         }
      }

      temp = cvar_sv_minupdaterate.value;
      if(temp != 0) {
         if(temp <= 0.001) {
            Cvar_DirectSetValue(&cvar_sv_minupdaterate, 10.0);
            temp = 10;
         }
         if(*rate > 1.0 / temp) {
            *rate = 1.0 / temp;
         }
      }
   }
}
/* DESCRIPTION: TrimSpace
// PATH: SV_ExtractFromUserinfo
//
// Yet another boring text cleaner.  Probably not unbeatable,
// certainly not special.
*/
void TrimSpace(char * in, char * out) {

   char * maxlen;
   int i;


   //So what are the rules here?  Near as I can tell, the rules are 'no
   //whitespace before or after the name' and assumably 'all must be printable'
   while(isgraph(*in) == 0) { //skip whitespace

      if(*in == '\0') { //Uh oh, an invalid name.  I wonder if we could screw attackers here...
         *out = '\0';
         return;
      }

      in++;
   }

   //We know there's at least one printable character in this string now.
   maxlen = out + strlen(out) - 1;
   while(isgraph(*maxlen) == 0) {
      maxlen--;
   }
   i = maxlen - out; //number of chars

   //copy it.  This WAS a memcpy, but I want to make sure non-prints are hit, and I'm too lazy to see if com_arg does that.
   while(i >= 0) {
      if(isprint(*in)) { *out = *in; out++; }
      in++;
      i--;
   }

   *out = '\0';
}
/* DESCRIPTION: SV_ExtractFromUserinfo
// LOCATION: sv_main.c
// PATH: SV_UpdateToReliableMessages, PF_CreateFakeClient_I, SV_ConnectClient
//
// To quote QW: Pull specific info from a newly changed userinfo string
// into a more C freindly form.  I take it that means text processing.
//
// This function looks to be responsible for CS-DOS.  I blame bad
// design, so the problem's not that easily solved.
*/
void SV_ExtractFromUserinfo(client_t *cl) {

   const char * val;
   unsigned int i, j;
   client_t * client;
   int dupc;
   char var_34_charbuf1[0x20];
   char var_54_charbuf2[0x20];


   dupc = 1;
   val = Info_ValueForKey(cl->InfoKeyBuffer, "name");
   Q_strncpy(var_34_charbuf1, val, sizeof(var_34_charbuf1)-1);
   var_34_charbuf1[sizeof(var_34_charbuf1)-1] = '\0';
   Q_strcpy(var_54_charbuf2, var_34_charbuf1);
   TrimSpace(var_54_charbuf2, var_34_charbuf1);

   if(/*var_34_charbuf1[0] == '\0' ||*/ Q_strcasecmp(var_34_charbuf1, "console") == 0) {

      //IIRC, wasn't this responsible for an infinite loop if the user used the name ""?
      Info_SetValueForKey(cl->InfoKeyBuffer, "name", "unnamed", 0x100);
      val = Info_ValueForKey(cl->InfoKeyBuffer, "name");
   }
   else if(Q_strcmp(var_34_charbuf1, val) != 0) { //we had to truncate

      Info_SetValueForKey(cl->InfoKeyBuffer, "name", var_34_charbuf1, 0x100);
      val = Info_ValueForKey(cl->InfoKeyBuffer, "name");
   }

   // check to see if another user by the same name exists
   while(1) {
      for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

         if(client == cl || client->padding00_used == 0 || client->padding04 == 0) { continue; }
         if(Q_strcasecmp(client->PlayerName, val) == 0) { break; }
      }

      if(i != global_svs.allocated_client_slots) { // dup name

         //This is crap, but I'm tired.  I'll properly ban stick attackers later.
         /* Based on QW< it should be safe to just do the HL equiv of
               SV_BroadcastTPrintf (PRINT_HIGH, STL_CLIENTKICKEDNAMESPAM, cl->name);
               SV_ClientTPrintf (cl, PRINT_HIGH, STL_YOUWEREKICKEDNAMESPAM);
               SV_DropClient (cl);
               return;
         // on offendors.
         */
         j = strlen(var_34_charbuf1);
         if(j > sizeof(var_34_charbuf1)-4) { j = sizeof(var_34_charbuf1)-4; }
         var_34_charbuf1[j] = '(';
         j += sprintf(var_34_charbuf1+j, "%u", dupc++) + 1;
         var_34_charbuf1[j] = ')';
         var_34_charbuf1[j+1] = '\0';

         Info_SetValueForKey(cl->InfoKeyBuffer, "name", var_34_charbuf1, 0x100);
         val = Info_ValueForKey(cl->InfoKeyBuffer, "name");
      }
      else {
         break;
      }
   }

   gEntityInterface.pfnClientUserInfoChanged(cl->padding4A84, cl->InfoKeyBuffer); //50
   strcpy(cl->PlayerName, var_34_charbuf1);

   //whew
   val = Info_ValueForKey(cl->InfoKeyBuffer, "rate");
   if(val[0] != '\0') {

      i = Q_atoi(val);
      if(i < 999) { i = 999; }
      else if(i > 20000) { i = 20000; }

      cl->netchan1C.rate = i;
   }

   val = Info_ValueForKey(cl->InfoKeyBuffer, "topcolor");
   if(val[0] != '\0') {
      cl->topcolor = Q_atoi(val);
   }
   else {
      Con_Printf("%s: topcolor unchanged for %s.\n", __FUNCTION__, cl->PlayerName);
   }

   val = Info_ValueForKey(cl->InfoKeyBuffer, "bottomcolor");
   if(val[0] != '\0') {
      cl->bottomcolor = Q_atoi(val);
   }
   else {
      Con_Printf("%s: bottomcolor unchanged for %s.\n", __FUNCTION__, cl->PlayerName);
   }

   val = Info_ValueForKey(cl->InfoKeyBuffer, "cl_updaterate");
   if(val[0] != '\0') {
      i = Q_atoi(val);
      if(i < 10) { i = 10; }
      else if(i > 300) { i = 300; }

      cl->cl_updaterate = 1.0 / i;
   }

   val = Info_ValueForKey(cl->InfoKeyBuffer, "cl_lw");
   if(val[0] != '\0') {
      cl->cl_lw = (Q_atoi(val) != 0);
   }
   else {
      cl->cl_lw = 0;
   }

   val = Info_ValueForKey(cl->InfoKeyBuffer, "cl_lc");
   if(val[0] != '\0') {
      cl->cl_lc = (Q_atoi(val) != 0);
   }
   else {
      cl->cl_lc = 0;
   }

   val = Info_ValueForKey(cl->InfoKeyBuffer, "hltv");
   if(val[0] != '\0') {
      cl->padding243C_proxy = (Q_atoi(val) != 1);
   }
   else {
      cl->padding243C_proxy = 0;
   }

   SV_CheckUpdateRate(&(cl->cl_updaterate));
   SV_CheckRate(cl);
}


//time based

/* DESCRIPTION: SV_CalcPing
// LOCATION: sv_main.c
// PATH: A few status functions, called every two seconds.
//
// Ever wonder why bots have a ping of five?  Well read this, and be informed.
// Pings are caluclated by averaging out old frames.  No real surprises here.
*/
int SV_CalcPing(client_t *cl) {

   float ping;
   unsigned int i, count;

   client_frame_t * frame;

   if(cl->padding2438_IsFakeClient) { return(666); } //Bots are soulless headshotting machines

   ping = 0;
   count = 0;
   for(i = 0; i < SV_UPDATE_BACKUP; i++) {

      frame = &(cl->padding3480_frames[(cl->netchan1C.incoming_acknowledged - (i + 1)) & SV_UPDATE_MASK]);

      if(frame->ping_time > 0) {
         ping += frame->ping_time;
         count++;
      }
   }

   if(count == 0) { return(0); }

   return((ping / count) * 1000);
}
/* DESCRIPTION: SV_ShouldUpdatePing
// LOCATION:
// PATH: SV_WriteEntitiesToClient
//
// This calls SV_CalcPing, and returns true if this person needs ping data.
*/
int SV_ShouldUpdatePing(client_t *client) {

   if(client->padding243C_proxy == 0) {
      SV_CalcPing(client);
      return(client->padding2440.buttons & IN_SCORE); //They are viewing the scoreboard.  Send them pings.
   }

   if(global_realtime > client->padding249C_TimeOfNextPingCalc) {

      client->padding249C_TimeOfNextPingCalc += 2;
      return(1);
   }
   return(0);
}
/* DESCRIPTION: SV_GetNetInfo
// LOCATION:
// PATH: SV_EmitPings
//
// This function and its static vars track some of the networking
// conditions.  I haven't bothered to trace it beyond that, because
// this fucntion sucks pretty badly.
*/
void SV_GetNetInfo(client_t *client, int * arg_4, int * arg_8) {

   int i;
   static int lastping_300[32]; //These static vars will need to be altered to up the max player limit.
   static int lastloss_301[32];

   i = client - global_svs.clients; //index.
   if(client->padding249C_TimeOfNextPingCalc < global_realtime) {

      client->padding249C_TimeOfNextPingCalc = global_realtime + 2;
      lastping_300[i] = SV_CalcPing(client);
      lastloss_301[i] = client->packet_loss;
   }

   *arg_4 = lastping_300[i];
   *arg_8 = lastloss_301[i];
}
/* DESCRIPTION: SV_CalcClientTime
// PATH: SV_ExecuteClientMessage->SV_ComputeLatency
//
// Another, more critical method of calculating times.  This is
// directly related to lag compensation, unlike the averaged out ping.
*/
float SV_CalcClientTime(client_t * cl) {

   int i, temp_sv_unlagsamples, temp_max;

   client_frame_t * var_18_frame_p;
   int TotalReads = 0;
   float ReturnVal = 0;


   temp_sv_unlagsamples = (int)cvar_sv_unlagsamples.value;
   if(temp_sv_unlagsamples < 1) { temp_sv_unlagsamples = 1; }
   else {
      temp_max = (SV_UPDATE_BACKUP > 16) ? SV_UPDATE_BACKUP : 16; //max()
      if(temp_sv_unlagsamples > temp_max) { temp_sv_unlagsamples = temp_max; }
   }

   for(i = 0; i < temp_sv_unlagsamples; i++) {

      var_18_frame_p = &cl->padding3480_frames[(cl->netchan1C.incoming_acknowledged - i) & SV_UPDATE_MASK];
      if(var_18_frame_p->ping_time > 0) {
         ReturnVal += var_18_frame_p->ping_time;
         TotalReads++;
      }
   }

   if(TotalReads == 0) { return(0); }
   ReturnVal /= TotalReads;

   //There is more to this function in the assembly, but it's crazy code that
   //doesn't seem to serve any real purpose.

   return(ReturnVal);
}
/* DESCRIPTION: SV_ComputeLatency
// PATH: SV_ExecuteClientMessage
//
// Calls SV_CalcClientTime.
*/
void SV_ComputeLatency(client_t * cl) {

   cl->ping = SV_CalcClientTime(cl);
}

/* DESCRIPTION: SV_UnlagCheckTeleport
// PATH: SV_RunCmd->SV_SetupMove
//
// This 'checks' to see if the entity was teleported over the course of
// a frame or so.  It's clearly not intended to be perfectly accurate.
*/
int SV_UnlagCheckTeleport(vec3_t old_pos, vec3_t new_pos) {

   int i;


   for(i = 0; i < 3; i++) {
      if(fabs(old_pos[i] - new_pos[i]) > 50) { return(1); }
   }

   return(0);
}


//thinking

/* DESCRIPTION: SV_PlayerRunPreThink
// LOCATION:
// PATH: SV_RunCmd
//
// Calls the mod's prethink function.
*/
void SV_PlayerRunPreThink(edict_t * ed, float arg_4) {

   gGlobalVariables.time = arg_4;
   gEntityInterface.pfnPlayerPreThink(ed);
}
/* DESCRIPTION: SV_PlayerRunThink
// LOCATION:
// PATH: SV_RunCmd
//
// Might run thinking code if enough time has passed.  It seems that
// it will also NOT run thinking code if TOO much time has passed.
//
// Returns 0 if the object removed itself or one otherwise.
*/
int SV_PlayerRunThink(edict_t * ed, float arg_4, double arg_8) {

   float var_4;


   if((ed->v.flags & (FL_KILLME | FL_DORMANT)) == 0) {

      var_4 = ed->v.nextthink;
      if(var_4 <= 0 || var_4 > arg_4 + arg_8) { return(1); }

      if(var_4 > arg_8) {
         var_4 = arg_8;
      }

      ed->v.nextthink = 0;
      gGlobalVariables.time = var_4;
      gEntityInterface.pfnThink(ed);
   }

   if((ed->v.flags & FL_KILLME)) { //Kill them.
      ED_Free(ed);
   }

   return(ed->free == 0); //return 1 if it's zero.
}


//moving

void SV_SetupMove(client_t * arg_0_client) {

   unsigned int i, j, var_C_entindex;
   float var_14_finalpush, var_18_lerp_msec, var_1C_ping, temp, var_30;
   client_frame_t * var_20, * var_24;
   packet_entities_t * var_2C_packet_ents;
   entity_state_t   * var_34_CurrentEntity, * var_38_FoundEntity;
   client_t * var_10_client_p;
   temp_move_t * var_48_trueposition;
   vec3_t tempvec, tempvec2;


   Q_memset(truepositions, 0, sizeof(truepositions));
   nofind = 1;

   if(gEntityInterface.pfnAllowLagCompensation() == 0 ||
      (int)cvar_sv_unlag.value == 0 ||
      arg_0_client->cl_lw == 0 || arg_0_client->cl_lc == 0 ||
      global_svs.allocated_client_slots <= 1 ||
      arg_0_client->padding00_used == 0) {

      return;
   }

   nofind = 0;

   for(i = 0; i < global_svs.allocated_client_slots; i++) {

      var_10_client_p = &(global_svs.clients[i]);
      if(var_10_client_p->padding00_used == 0 || var_10_client_p == arg_0_client) { continue; }

      var_48_trueposition = &(truepositions[i]);

      var_48_trueposition->OldLocation[0] = var_10_client_p->padding4A84->v.origin[0];
      var_48_trueposition->OldLocation[1] = var_10_client_p->padding4A84->v.origin[1];
      var_48_trueposition->OldLocation[2] = var_10_client_p->padding4A84->v.origin[2];

      var_48_trueposition->Mins[0] = var_10_client_p->padding4A84->v.absmin[0];
      var_48_trueposition->Mins[1] = var_10_client_p->padding4A84->v.absmin[1];
      var_48_trueposition->Mins[2] = var_10_client_p->padding4A84->v.absmin[2];
      var_48_trueposition->Maxs[0] = var_10_client_p->padding4A84->v.absmax[0];
      var_48_trueposition->Maxs[1] = var_10_client_p->padding4A84->v.absmax[1];
      var_48_trueposition->Maxs[2] = var_10_client_p->padding4A84->v.absmax[2];

      var_48_trueposition->IsActive = 1;
   }

   if(arg_0_client->ping > 1.5) {
      var_1C_ping = 1.5;
   }
   else {
      var_1C_ping = arg_0_client->ping;
   }

   temp = cvar_sv_maxunlag.value;
   if(temp > 0 && var_1C_ping > temp) {
      var_1C_ping = temp;
   }

   var_18_lerp_msec = arg_0_client->padding2440.lerp_msec / 1000.0;
   if(var_18_lerp_msec > .1) {
      var_18_lerp_msec = .1;
   }

   if(var_18_lerp_msec < arg_0_client->cl_updaterate) {
      var_18_lerp_msec = arg_0_client->cl_updaterate;
   }

   var_14_finalpush = cvar_sv_unlagpush.value + ((global_realtime - var_1C_ping) - var_18_lerp_msec);

   if(var_14_finalpush > global_realtime) {
      var_14_finalpush = global_realtime;
   }

   var_20 = NULL;
   for(var_24 = NULL, i = 0; i < (unsigned)SV_UPDATE_BACKUP; i++, var_24 = var_20) {

      var_20 = &(arg_0_client->padding3480_frames[(arg_0_client->netchan1C.outgoing_sequence - 1) & SV_UPDATE_MASK]);
      var_2C_packet_ents = &(var_20->entities);

      for(j = 0; j < var_2C_packet_ents->max_entities; j++) {

         var_34_CurrentEntity = &(var_2C_packet_ents->entities[j]);
         if(var_34_CurrentEntity->number <= 0 || (unsigned)var_34_CurrentEntity->number >= global_svs.allocated_client_slots) { continue; }
         //You know, jge doesn't sound right given how the indexing normally works.

         var_48_trueposition = &(truepositions[var_34_CurrentEntity->number-1]);
         if(var_48_trueposition->DoNotInterpolate != 0) { continue; }

         if(var_34_CurrentEntity->health <= 0 || (var_34_CurrentEntity->effects & EF_NOINTERP) != 0) {
            var_48_trueposition->DoNotInterpolate = 1;
         }

         if(var_48_trueposition->FirstIteration == 0) {
            var_48_trueposition->FirstIteration = 1;
         }
         else if(SV_UnlagCheckTeleport(var_34_CurrentEntity->origin, var_48_trueposition->FinalVectors) != 0) {
            var_48_trueposition->DoNotInterpolate = 1;
         }


         var_48_trueposition->FinalVectors[0] = var_34_CurrentEntity->origin[0];
         var_48_trueposition->FinalVectors[1] = var_34_CurrentEntity->origin[1];
         var_48_trueposition->FinalVectors[2] = var_34_CurrentEntity->origin[2];
      }

      if(var_14_finalpush > var_20->sent_time) { break; }
   }


   if(i == (unsigned int)SV_UPDATE_BACKUP || var_14_finalpush - var_20->sent_time > 1.0) {

      Q_memset(truepositions, 0, sizeof(truepositions));
      nofind = 1;
      return;
   }

   var_2C_packet_ents = &(var_20->entities);
   if(var_24 == NULL) {

      var_24 = var_20;
      var_30 = 0;
   }
   else {

      if(var_24->sent_time - var_20->sent_time == 0) {
         var_30 = 0;
      }
      else {

         var_30 = (var_14_finalpush - var_20->sent_time) / (var_24->sent_time - var_20->sent_time);
         if(var_30 > 1) {
            var_30 = 1;
         }
         else if(var_30 < 0) {
            var_30 = 0;
         }
      }
   }


   for(i = 0; i < var_2C_packet_ents->max_entities; i++) {

      var_34_CurrentEntity = &(var_2C_packet_ents->entities[i]);
      if(var_34_CurrentEntity->number <= 0 || (unsigned)var_34_CurrentEntity->number >= global_svs.allocated_client_slots) { continue; }

      var_C_entindex = var_34_CurrentEntity->number - 1;
      var_10_client_p = &(global_svs.clients[var_C_entindex]);
      if(var_10_client_p->padding00_used == 0 || var_10_client_p == arg_0_client) { continue; }

      var_48_trueposition = &(truepositions[var_C_entindex]);
      if(var_48_trueposition->IsActive == 0 || var_48_trueposition->DoNotInterpolate != 0) { continue; }

      var_38_FoundEntity = SV_FindEntInPack(var_34_CurrentEntity->number, &(var_24->entities));
      if(var_38_FoundEntity == NULL) {

         tempvec[0] = var_34_CurrentEntity->origin[0];
         tempvec[1] = var_34_CurrentEntity->origin[1];
         tempvec[2] = var_34_CurrentEntity->origin[2];
      }
      else {

         tempvec2[0] = var_38_FoundEntity->origin[0] - var_34_CurrentEntity->origin[0];
         tempvec2[1] = var_38_FoundEntity->origin[1] - var_34_CurrentEntity->origin[1];
         tempvec2[2] = var_38_FoundEntity->origin[2] - var_34_CurrentEntity->origin[2];

         VectorMA(var_34_CurrentEntity->origin, var_30, tempvec2, tempvec);
      }

      var_48_trueposition->NewLocation[0] = tempvec[0];
      var_48_trueposition->NewLocation[1] = tempvec[1];
      var_48_trueposition->NewLocation[2] = tempvec[2];
      var_48_trueposition->CurrentPosition[0] = tempvec[0];
      var_48_trueposition->CurrentPosition[1] = tempvec[1];
      var_48_trueposition->CurrentPosition[2] = tempvec[2];

      if(VectorCompare(tempvec, var_10_client_p->padding4A84->v.origin) == 0) {

         var_10_client_p->padding4A84->v.origin[0] = tempvec[0];
         var_10_client_p->padding4A84->v.origin[1] = tempvec[1];
         var_10_client_p->padding4A84->v.origin[2] = tempvec[2];

         SV_LinkEdict(var_10_client_p->padding4A84, 0);
         var_48_trueposition->IsMoving = 1;
      }
   }
}
void SV_RestoreMove(client_t * cl) {

   unsigned int i;
   client_t * var_8_client;
   temp_move_t * var_C_oldevent;

   if(nofind != 0) {
      nofind = 0;
      return;
   }

   if(gEntityInterface.pfnAllowLagCompensation() == 0 ||
      global_svs.allocated_client_slots <= 1 ||
      (int)cvar_sv_unlag.value == 0 ||
      cl->cl_lw == 0 || cl->cl_lc == 0 ||
      cl->padding00_used == 0) {

      return;
   }

   for(i = 0; i < global_svs.allocated_client_slots; i++) {

      var_8_client = &(global_svs.clients[i]);
      if(var_8_client == cl || var_8_client->padding00_used == 0) { continue; }

      var_C_oldevent = &(truepositions[i]);

      if(VectorCompare(var_C_oldevent->OldLocation, var_C_oldevent->NewLocation) != 0) { continue; } //They didn't actually move.
      if(var_C_oldevent->IsMoving == 0) { return; }
      if(var_C_oldevent->IsActive == 0) {

         Con_Printf("%s:  Tried to restore an inactive player.\n", __FUNCTION__);
         return;
      }

      if(VectorCompare(var_C_oldevent->CurrentPosition, var_8_client->padding4A84->v.origin) != 0) {

         var_8_client->padding4A84->v.origin[0] = var_C_oldevent->OldLocation[0];
         var_8_client->padding4A84->v.origin[1] = var_C_oldevent->OldLocation[1];
         var_8_client->padding4A84->v.origin[2] = var_C_oldevent->OldLocation[2];
         SV_LinkEdict(var_8_client->padding4A84, 0);
      }
   }
}


//init

/* DESCRIPTION: SV_User_Init
// PATH: SV_Init
//
// A few local cvars are here.
*/
void SV_User_Init() {

   Cvar_RegisterVariable(&cvar_sv_maxrate);
   Cvar_RegisterVariable(&cvar_sv_minrate);
   Cvar_RegisterVariable(&cvar_sv_maxupdaterate);
   Cvar_RegisterVariable(&cvar_sv_minupdaterate);
   Cvar_RegisterVariable(&cvar_sv_unlag);
   Cvar_RegisterVariable(&cvar_sv_maxunlag);
   Cvar_RegisterVariable(&cvar_sv_unlagpush);
   Cvar_RegisterVariable(&cvar_sv_unlagsamples);
}

