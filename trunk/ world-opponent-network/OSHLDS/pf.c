#include "pf.h"
#include "cmd.h"
#include "Modding.h"
#include "draw.h"
#include "host.h"
#include "SV_hlds.h"
#include "SV_mcast.h"
#include "SV_move.h"
#include "SV_phys.h"
#include "SV_pkt.h"
#include "SV_pktcl.h"
#include "SV_user.h"
#include "sys.h"
#include "VecMath.h"
#include "report.h"



/* This file handles all functions that are exported to mod DLLs.
// Most of them start with "PF_", hence the name.  But not all.
// Most of them are also used ONLY by mods, and are proxies.
// But not all.
*/

static vec3_t gHullMins[4] = { {   0,   0,   0 },
                               { -16, -16, -36 },
                               { -32, -32, -32 },
                               { -16, -16, -18 } };
static vec3_t gHullMaxs[4] = { {   0,   0,   0 },
                               {  16,  16,  36 },
                               {  32,  32,  32 },
                               {  16,  16,  18 } };

msurface_t * SurfaceAtPoint(model_t *, mnode_t *, vec3_t, vec3_t);

static cvar_t cvar_sv_aim = { "sv_aim", "0", (FCVAR_ARCHIVE | FCVAR_SERVER), 0.0, NULL };

HLDS_DLLEXPORT int  PF_CreateInstancedBaseline(int classname, struct entity_state_s *baseline) {

   int i;


   i = global_sv.padding3bc60_struct->padding00;

   if(i > 62) {
      return(0);
   }

   global_sv.padding3bc60_struct->padding04[i] = classname;
   global_sv.padding3bc60_struct->padding104[i] = *baseline;
   global_sv.padding3bc60_struct->padding00++;
   return(i+1);
}
HLDS_DLLEXPORT void PF_ForceUnmodified(FORCE_TYPE type, vec3_t mins, vec3_t maxs, const char *filename) {

   unsigned int i;
   ConsistencyData_t * ptr;


   //This function "Forces the client and server to be running with the same version of the specified file".
   if(filename == NULL || PR_IsEmptyString(filename)) {
      Sys_Error("%s: Bad filename string.\n", __FUNCTION__);
   }

   if(global_sv.padding3bc64 == 1) { //Are we allowed to add things, or only check them?

      for(i = 0, ptr = global_sv.padding2A944; i < 0x200; i++, ptr++) {

         if(ptr->name == NULL) { //End of list.  Add our data here.
            ptr->name = filename;
            ptr->padding10_force_state = type;
            if(mins != NULL) {
               ptr->mins[0] = mins[0];
               ptr->mins[1] = mins[1];
               ptr->mins[2] = mins[2];
            }
            if(maxs != NULL) {
               ptr->maxs[0] = maxs[0];
               ptr->maxs[1] = maxs[1];
               ptr->maxs[2] = maxs[2];
            }

            return;
         }
         else if(Q_strcmp(filename, ptr->name) == 0) { //already listed.
            return;
         }
      }

      Sys_Error("%s: Overflow adding force data.  512 max.\n", __FUNCTION__);
   }
   else {

      for(i = 0, ptr = global_sv.padding2A944; i < 0x200; i++, ptr++) {

         if(ptr->name == NULL || Q_strcmp(filename, ptr->name) != 0) { //Not a match.
            continue;
         }

         //If we are here' we found a match.
         return;
      }

      Sys_Error("%s: Overflow adding force data.  512 max.\n", __FUNCTION__);
   }
}
HLDS_DLLEXPORT void PF_SetGroupMask(int mask, int op) {

   global_g_groupop = op;
   global_g_groupmask = mask;
}
HLDS_DLLEXPORT void PF_RunPlayerMove_I(edict_t *fakeclient, const vec3_t viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec) {

   unsigned int clientIndex;
   usercmd_t cmd;

   edict_t * old_sv_player;
   client_t * old_host_client;


   clientIndex = NUM_FOR_EDICT(fakeclient) - 1;
   if(clientIndex >= global_svs.allocated_client_slots) {
      Con_Printf("%s: edict doesn't belong to client (index %i).\n", __FUNCTION__, clientIndex);
      return;
   }


   old_sv_player = global_sv_player;
   old_host_client = global_host_client;

   global_sv_player = fakeclient;
   global_host_client = &(global_svs.clients[clientIndex]);

   global_host_client->padding24A4 = (global_sv.time0c + host_frametime) - (msec / 1000.0);


   cmd.lerp_msec = 0;
   cmd.msec = msec;
   cmd.viewangles[0] = viewangles[0];
   cmd.viewangles[1] = viewangles[1];
   cmd.viewangles[2] = viewangles[2];
   cmd.forwardmove = forwardmove;
   cmd.sidemove = sidemove;
   cmd.upmove = upmove;
   cmd.lightlevel = 0;
   cmd.buttons = buttons;
   cmd.impulse = impulse;
   cmd.weaponselect = 0;
   cmd.impact_index = 0;
   cmd.impact_position[0] = 0;
   cmd.impact_position[1] = 0;
   cmd.impact_position[2] = 0;

   global_pmove = &global_svmove;
   //SV_PreRunCmd();
   SV_RunCmd(&cmd, 0);

   global_host_client->padding2440 = cmd;

   global_sv_player = old_sv_player;
   global_host_client = old_host_client;
}
HLDS_DLLEXPORT void PF_SetClientMaxspeed(edict_t *pEdict, float fNewMaxspeed) {

   unsigned int clientIndex;
   client_t * cl;


   clientIndex = NUM_FOR_EDICT(pEdict) - 1;
   if(clientIndex >= global_svs.allocated_client_slots) {
      Con_Printf("%s: edict doesn't belong to client (index %i).\n", __FUNCTION__, clientIndex);
      return;
   }

   cl = &(global_svs.clients[clientIndex]);
   if(cl->padding2438_IsFakeClient != 0) { return; }


   pEdict->v.maxspeed = fNewMaxspeed;
}
HLDS_DLLEXPORT void PF_FadeVolume(const edict_t * pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds) {

   unsigned int clientIndex;
   client_t * cl;


   clientIndex = NUM_FOR_EDICT(pEdict) - 1;
   if(clientIndex >= global_svs.allocated_client_slots) {
      Con_Printf("%s: edict doesn't belong to client (index %i).\n", __FUNCTION__, clientIndex);
      return;
   }

   cl = &(global_svs.clients[clientIndex]);
   if(cl->padding2438_IsFakeClient != 0) { return; }

   MSG_WriteChar(&(cl->netchan1C.netchan_message), 0x30);
   MSG_WriteByte(&(cl->netchan1C.netchan_message), fadePercent);
   MSG_WriteByte(&(cl->netchan1C.netchan_message), holdTime);
   MSG_WriteByte(&(cl->netchan1C.netchan_message), fadeOutSeconds);
   MSG_WriteByte(&(cl->netchan1C.netchan_message), fadeInSeconds);
}
HLDS_DLLEXPORT void PF_crosshairangle_I(const edict_t *pClient, float pitch, float yaw) {

   unsigned int clientIndex;
   client_t * cl;


   clientIndex = NUM_FOR_EDICT(pClient) - 1;
   if(clientIndex >= global_svs.allocated_client_slots) {
      Con_Printf("%s: edict doesn't belong to client (index %i).\n", __FUNCTION__, clientIndex);
      return;
   }

   cl = &(global_svs.clients[clientIndex]);
   if(cl->padding2438_IsFakeClient != 0) { return; }

   //-180 < var <= 180.  The 180 bounds themselves are probably both acceptable,
   //but since 180 and -180 refer to the same direction, positives are easier.
        if(pitch  >  180.0) { pitch -= 360.0; }
   else if(pitch <= -180.0) { pitch += 360.0; }
        if(yaw    >  180.0) { yaw   -= 360.0; }
   else if(yaw   <= -180.0) { yaw   += 360.0; }

   MSG_WriteByte(&(cl->netchan1C.netchan_message), 0x2F);
   MSG_WriteChar(&(cl->netchan1C.netchan_message), pitch * 5); //What, why?  We now truncate instead of rounding, and our max value is 51.  Why do this?
   MSG_WriteChar(&(cl->netchan1C.netchan_message), yaw * 5);
}
HLDS_DLLEXPORT void PF_setview_I(const edict_t * pClient, const edict_t * pViewent) {

   unsigned int clientIndex;
   client_t * cl;


   clientIndex = NUM_FOR_EDICT(pClient) - 1;


   if(clientIndex >= global_svs.allocated_client_slots) {
      Sys_Error("%s: edict doesn't belong to client (index %i).\n", __FUNCTION__, clientIndex);
   }

   cl = &(global_svs.clients[clientIndex]);
   if(cl->padding2438_IsFakeClient != 0) { return; }

   cl->padding4A88 = pViewent;

   MSG_WriteByte(&(cl->netchan1C.netchan_message), 0x05); //setview.  These HAVE to be listed somewhere; I don't want to sort out ALL of the possible identifiers.
   MSG_WriteShort(&(cl->netchan1C.netchan_message), NUM_FOR_EDICT(pViewent)); //Must go through and decipher all these codes.
}
HLDS_DLLEXPORT void PF_aim_I(edict_t* ent, float speed, vec3_t rgflReturn) {

   unsigned int i, j;
   float   dist, bestdist;
   vec3_t  start, dir, end, bestdir;
   trace_t tr;
   edict_t * check;
   //edict_t * bestent;

   static vec3_t origin = { 0, 0, 0 };


   if(ent == NULL || (ent->v.flags & FL_FAKECLIENT) != 0) {

      rgflReturn[0] = gGlobalVariables.v_forward[0];
      rgflReturn[1] = gGlobalVariables.v_forward[1];
      rgflReturn[2] = gGlobalVariables.v_forward[2];
      return;
   }

   start[0] = ent->v.origin[0] + ent->v.view_ofs[0];
   start[1] = ent->v.origin[1] + ent->v.view_ofs[1];
   start[2] = ent->v.origin[2] + ent->v.view_ofs[2];

   dir[0] = gGlobalVariables.v_forward[0];
   dir[1] = gGlobalVariables.v_forward[1];
   dir[2] = gGlobalVariables.v_forward[2];

   VectorMA(start, 2048, dir, end);
   tr = SV_Move(start, origin, origin, end, 0, ent, 0);

   if(tr.pHit != NULL && tr.pHit->v.takedamage == DAMAGE_AIM && (ent->v.team <= 0 || ent->v.team != tr.pHit->v.team)) {

      rgflReturn[0] = gGlobalVariables.v_forward[0];
      rgflReturn[1] = gGlobalVariables.v_forward[1];
      rgflReturn[2] = gGlobalVariables.v_forward[2];
      return;
   }

   // try all possible entities
   bestdir[0] = dir[0];
   bestdir[1] = dir[1];
   bestdir[2] = dir[2];
   bestdist = cvar_sv_aim.value;
  // bestent = NULL;

   for(i = 1; i < global_sv.num_edicts; i++) {

      check = &(global_sv.edicts[i]);

      if(check->v.takedamage != DAMAGE_AIM) { continue; }
      if(check->v.flags & FL_FAKECLIENT) { continue; } //What, fake clients are spare?
      if(ent->v.team > 0 && ent->v.team == check->v.team) { continue; } //don't aim at teammate
      if(check == ent) { continue; } //Don't aim at myself

      for(j = 0; j < 3; j++) {
         end[j] = check->v.origin[j] + ((check->v.mins[j] + check->v.maxs[j]) / 2);
      }

      dir[0] = end[0] - start[0];
      dir[1] = end[1] - start[1];
      dir[2] = end[2] - start[2];
      VectorNormalize(dir);

      dist = DotProduct(dir, gGlobalVariables.v_forward);
      if(dist < bestdist) { continue; } //either we have a better target, or there's nobody in range.

      tr = SV_Move(start, origin, origin, end, 0, ent, 0);
      if(tr.pHit == check) { //Find him.  Kill him.
         bestdist = dist;
         //bestent = check;
         bestdir[0] = dir[0];
         bestdir[1] = dir[1];
         bestdir[2] = dir[2];
      }
   }

   rgflReturn[0] = bestdir[0];
   rgflReturn[1] = bestdir[1];
   rgflReturn[2] = bestdir[2];
}

// walkmove modes (sdk)
#define WALKMOVE_NORMAL    0 // normal walkmove
#define WALKMOVE_WORLDONLY 1 // doesn't hit ANY entities, no matter what the solid type
#define WALKMOVE_CHECKONLY 2 // move, but don't touch triggers
HLDS_DLLEXPORT int PF_walkmove_I(edict_t *ent, float yaw, float dist, int iMode) {

   vec3_t move;


   if((ent->v.flags & (FL_ONGROUND | FL_FLY | FL_SWIM)) == 0) { return(0); }

   yaw = yaw * (M_PI * 2.0 / 360.0);

   move[0] = cos(yaw) * dist;
   move[1] = sin(yaw) * dist;
   move[2] = 0;

   switch(iMode) {

   case WALKMOVE_NORMAL:
      return(SV_movestep(ent, move, 1));

   case WALKMOVE_WORLDONLY:
      return(SV_movetest(ent, move, 1));

   case WALKMOVE_CHECKONLY:
      return(SV_movestep(ent, move, 0));

   default:
      Sys_Error("%s: Invalid walk mode (%i).\n", __FUNCTION__, iMode);
   }
}

static int r_visframecount = 0;
void PVSMark(model_t * world, byte * pvs) {

   int i;
   mleaf_t * ptr;


   r_visframecount++;

   for(i = 0; i < world->numleafs; i++) {

      if((((unsigned int)(pvs[i / 8])) >> (i & 7)) == 0) { continue; }

      ptr = &(world->leafs[i +1]);
      while(ptr->visframe == r_visframecount) {

         ptr->visframe = r_visframecount;
         ptr = (mleaf_t *)(ptr->parent); //Remember, interchangeable for the first few fields.

         if(ptr == NULL) { break; }
      }
   }
}
mnode_t * PVSNode(mnode_t * rootnode, vec3_t absmin, vec3_t absmax) {

   int sides;
   mplane_t * plane;
   mnode_t * retNode;


   if(rootnode->visframe != r_visframecount) { return(0); }
   if(rootnode->contents < 0) { //Is not a node, treat accordingly.
      if(rootnode->contents == -2) {
         return(NULL);
      }
      else {
         return(rootnode);
      }
   }


   plane = rootnode->plane;
   sides = BOX_ON_PLANE_SIDE(absmin, absmax, plane);

   if(sides & 1) {
      retNode = PVSNode(rootnode->children[0], absmin, absmax);
      if(retNode != NULL) { return(retNode); }
   }

   if(sides & 2) {
      retNode = PVSNode(rootnode->children[1], absmin, absmax);
      if(retNode != NULL) { return(retNode); }
   }

   return(NULL);
}
HLDS_DLLEXPORT edict_t * PVSFindEntities(edict_t *pplayer) {

   unsigned int i;
   vec3_t vec_pov;
   mleaf_t * leaf;
   edict_t * ptrEdict, * ptrEdict2, * LastEdict;
   byte * pvs;


   vec_pov[0] = pplayer->v.origin[0] + pplayer->v.view_ofs[0];
   vec_pov[1] = pplayer->v.origin[1] + pplayer->v.view_ofs[1];
   vec_pov[2] = pplayer->v.origin[2] + pplayer->v.view_ofs[2];

   leaf = Mod_PointInLeaf(vec_pov, global_sv.worldmodel);
   pvs  = Mod_LeafPVS(leaf, global_sv.worldmodel);

   PVSMark(global_sv.worldmodel, pvs);

   LastEdict = &(global_sv.edicts[0]);
   for(i = 1, ptrEdict = &(global_sv.edicts[1]); i < global_sv.num_edicts; i++, ptrEdict++) {

      if(ptrEdict->free != 0) { continue; }

      if(ptrEdict->v.movetype != MOVETYPE_FOLLOW || ptrEdict->v.aiment == NULL) {
         ptrEdict2 = ptrEdict;
      }
      else {
         ptrEdict2 = ptrEdict->v.aiment;
      }

      if(PVSNode(global_sv.worldmodel->nodes, ptrEdict2->v.absmin, ptrEdict2->v.absmax) != NULL) {
         ptrEdict->v.chain = LastEdict;
         LastEdict = ptrEdict;
      }
   }

   return(LastEdict);
}


HLDS_DLLEXPORT void SaveSpawnParms(edict_t * ent) {

   //This function gets the client pointer, but doesn't act on it.
   //Aside from an edict bounds checking, there's no effect whatsoever
   //on the dedicated server with this function.
}
HLDS_DLLEXPORT void PF_setspawnparms_I(edict_t * ent) { } //same as above.


// This function had a possible bug in the assembly.  Rather than check
// cl->IsFakeClient, the 386 asm had global_host_client->IsFakeClient.
// This makes no sense--the mod calls this on a map change, and global_client
// doesn't appear to be assigned beforehand.  So I changed it.
void SV_SkipUpdates() {

   unsigned int i;
   client_t * cl;


   for(i = 0, cl = global_svs.clients; i < global_svs.allocated_client_slots; i++, cl++) {

      if((cl->padding00_used == 0 && cl->padding0C_connected == 0 && cl->padding04 == 0) ||
         (cl->padding2438_IsFakeClient != 0)) { continue; }

      cl->padding347C_SkipNextTick = 1;
   }

}
HLDS_DLLEXPORT void PF_changelevel_I(char* s1, char* s2) {

   char temp[0x400];
   static unsigned int last_spawncount = 0;


   // make sure we don't issue two changelevels
   if(global_svs.ServerCount == last_spawncount) { return; }
   last_spawncount = global_svs.ServerCount;

   SV_SkipUpdates();
   if(s2 == NULL) {

      Q_snprintf(temp, sizeof(temp)-1, "changelevel %s\n", s1);
      temp[sizeof(temp)-1] = '\0';
   }
   else {

      Q_snprintf(temp, sizeof(temp)-1, "changelevel2 %s %s\n", s1, s2);
      temp[sizeof(temp)-1] = '\0';
   }

   Cbuf_AddText(temp);
}

HLDS_DLLEXPORT void PF_changepitch_I(edict_t * ent) {

   float   ideal, current, move, speed;


   current = anglemod(ent->v.angles[0]); //The QW uses angles[1].  Hmm.
   ideal = ent->v.idealpitch;
   speed = ent->v.pitch_speed;

   if(current == ideal) { return; }
   move =  - current;

   if(ideal > current) {
      if(move >= 180) { move = move - 360; }
   }
   else {
      if(move <= -180) { move = move + 360; }
   }

   if(move > 0) {
      if(move > speed) { move = speed; }
   }
   else {
      if(move < -speed) { move = -speed; }
   }

   ent->v.angles[0] = anglemod(current + move);
}
HLDS_DLLEXPORT void PF_changeyaw_I(edict_t * ent) {

   float   ideal, current, move, speed;


   current = anglemod(ent->v.angles[1]);
   ideal = ent->v.ideal_yaw;
   speed = ent->v.yaw_speed;
   if(current == ideal) { return; }

   move = ideal - current;
   if(ideal > current) {
      if(move >= 180) { move = move - 360; }
   }
   else {
      if(move <= -180) { move = move + 360; }
   }
   if(move > 0) {
      if(move > speed) { move = speed; }
   }
   else {
      if(move < -speed) { move = -speed; }
   }

   ent->v.angles[1] = anglemod(current + move);
}



HLDS_DLLEXPORT int PF_NumberOfEntities_I() {

   unsigned int i, total;


   total = 0;
   for(i = 0; i < global_sv.num_edicts; i++) {
      if(global_sv.edicts[i].free == 0) { total++; }
   }

   return(total);
}

HLDS_DLLEXPORT void PF_particle_I(const vec3_t org, const vec3_t dir, float color, float count) {
   SV_StartParticle(org, dir, color, count);
}
HLDS_DLLEXPORT int  PF_pointcontents_I(const vec3_t rgflVector) {
   return(SV_PointContents(rgflVector));
}

HLDS_DLLEXPORT qboolean Voice_GetClientListening(int iReceiver, int iSender) {

   if(iReceiver <= 0 || (unsigned)iReceiver > global_svs.allocated_client_slots ||
      iSender   <= 0 || (unsigned)iSender   > global_svs.allocated_client_slots) {

      Con_Printf("%s: Invalid client indexes (%i, %i).\n", __FUNCTION__, iReceiver, iSender);
      return(0);
   }

   return((global_svs.clients[iSender].padding4EE4 & (1 << iReceiver)) != 0);
}
HLDS_DLLEXPORT qboolean Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen) {

   if(iReceiver <= 0 || (unsigned)iReceiver > global_svs.allocated_client_slots ||
      iSender   <= 0 || (unsigned)iSender   > global_svs.allocated_client_slots) {

      Con_Printf("%s: Invalid client indexes (%i, %i).\n", __FUNCTION__, iReceiver, iSender);
      return(0);
   }

   if(bListen) {
      global_svs.clients[iSender].padding4EE4 |= (1 << iReceiver);
   }
   else {
      global_svs.clients[iSender].padding4EE4 &= ~(1 << iReceiver);
   }
   return(1);
}

/* DESCRIPTION: PF_GetPlayerWONId
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (pfnGetPlayerWONId)
//
// This returns IDs.  I don't knowQUITE what counts here, but I found a small
// description online.
//
// It's indicated this returns GetPlayerWonID for WON clients.  Since
// GetPlayerWonID returns an int, this must print that to a string.
//
// An example Steam ID is STEAM_0:94534
//
// Lastly, it was mentioned bits have a special ID of BOT.
*/
HLDS_DLLEXPORT const char * PF_GetPlayerAuthId(edict_t *e) {

   unsigned int i;
   client_t * client;
   static char returnbuffer[0x80];


   if(global_sv.active == 0 || e == NULL) {
      returnbuffer[0] = '\0';
      return(returnbuffer);
   }

   client = global_svs.clients;
   for(i = 0; i < global_svs.allocated_client_slots; i++, client++) { //why are we doing it this way instead of subtracting our way through?
      if(client->padding4A84 == e) {

         if(client->padding2438_IsFakeClient != 0) {
            Q_strcat(returnbuffer, "BOT");
         }
         else if(client->authentication_method == 0) {
            sprintf(returnbuffer, "%u", (unsigned int)client->authentication_WonID);
         }
         else {
            snprintf(returnbuffer, sizeof(returnbuffer)-1, "%s", SV_GetClientIDString(client));
            returnbuffer[sizeof(returnbuffer)-1] = '\0';
         }

         return(returnbuffer);
      }
   }

   returnbuffer[0] = '\0';
   return(returnbuffer);
}

HLDS_DLLEXPORT const char * PF_InfoKeyValue_I(char *infobuffer, char *key) {
   return(Info_ValueForKey(infobuffer, key));
}
HLDS_DLLEXPORT void PF_SetKeyValue_I(char *infobuffer, char *key, char *value) {

   if(infobuffer == global_localinfo) {

      Info_SetValueForKey(infobuffer, key, value, 0x8000);
      return;
   }
   if(infobuffer == Info_Serverinfo()) {

      Info_SetValueForKey(infobuffer, key, value, 0x200);
      return;
   }

   Sys_Error("%s: Not allowed to set client keys with this.\n", __FUNCTION__);
}
HLDS_DLLEXPORT void PF_SetClientKeyValue_I(int clientIndex, char *infobuffer, char *key, char *value) {

   client_t * ptr;


   if(infobuffer == global_localinfo || infobuffer == Info_Serverinfo()) {
      Con_Printf("%s: Not given a client infobuffer.\n");
      return;
   }
   if(clientIndex <= 0 || (unsigned)clientIndex > global_svs.allocated_client_slots) {
      Con_Printf("%s: Not given a valid client index.\n");
      return;
   }

   if(Q_strcmp(Info_ValueForKey(infobuffer, key), value) != 0) {

      Info_SetValueForKey(infobuffer, key, value, 0x100);
      ptr = &(global_svs.clients[clientIndex-1]);
      ptr->padding4BA4 = 1;
      ptr->padding4BA8 = 0;
   }
}

HLDS_DLLEXPORT int  PF_CanSkipPlayer(const edict_t *player) {

   unsigned int i;


   i = NUM_FOR_EDICT(player);
   if(i <= 0 || i > global_svs.allocated_client_slots) {
      Con_Printf("%s:  Passed non-player edict.\n", __FUNCTION__);
      return(0);
   }

   return(global_svs.clients[i-1].cl_lw != 0);
}

HLDS_DLLEXPORT edict_t * FindEntityByVars(struct entvars_s* pvars) {

   unsigned int i;
   edict_t * ptr;


   for(i = 0; i < global_sv.num_edicts; i++) {

      ptr = &(global_sv.edicts[i]);
      if(&(ptr->v) == pvars) { return(ptr); }
   }

   return(NULL);
}
HLDS_DLLEXPORT void * GetModelPtr(edict_t* pEdict) {

   if(pEdict == NULL) { return(NULL); }

   return(Mod_Extradata(global_sv.models[pEdict->v.modelindex]));
}

HLDS_DLLEXPORT int  GetEntityIllum(edict_t* pEnt) {

   unsigned int i;


   if(pEnt == NULL) { return(-1); }
   i = NUM_FOR_EDICT(pEnt);
   if(i > global_svs.allocated_client_slots) {
      return(0x80);
   }
   else {
      return(pEnt->v.light_level);
   }
}
HLDS_DLLEXPORT edict_t * FindEntityInSphere(edict_t *pEdictStartSearchAfter, const vec3_t org, float rad) {

   unsigned int var_c, var_10, var_14;
   edict_t * ent;
   float   var_8, var_18;


   if(pEdictStartSearchAfter == NULL) { var_c = 0; }
   else {
      var_c = NUM_FOR_EDICT(pEdictStartSearchAfter);
      var_10 = 1;
      while(var_c == 0) {
         var_c = NUM_FOR_EDICT(pEdictStartSearchAfter + var_10);
         var_10++;
      }
   }

   rad *= rad;

   for(var_10 = var_c + 1; var_10 < global_sv.num_edicts; var_10++) {

      ent = &(global_sv.edicts[var_10]);

      if(ent->free != 0 || ent->v.classname == 0 ||
        (var_10 <= global_svs.allocated_client_slots && global_svs.clients[var_10-1].padding00_used == 0)) {

         continue;
      }

      for(var_14 = 0, var_18 = 0; var_14 < 3 && var_18 <= rad; var_14++ ) {

         if(org[var_14] < ent->v.absmin[var_14] ) {
            var_8 = org[var_14] - ent->v.absmin[var_14];
         }
         else if(org[var_14] > ent->v.absmax[var_14]) {
            var_8 = org[var_14] - ent->v.absmax[var_14];
         }
         else {
            var_8 = 0;
         }

         var_18 += var_8 * var_8;
      }

      if(var_18 < rad) { return(ent); }
   }

   return(NULL);
}

HLDS_DLLEXPORT void PF_RemoveKey_I(char *s, const char *key) {
   Info_RemoveKey(s, key);
}
HLDS_DLLEXPORT const char * PF_GetPhysicsKeyValue(const edict_t *pClient, const char *key) {

   unsigned int i;


   i = NUM_FOR_EDICT(pClient);
   if(i <= 0 || i > global_svs.allocated_client_slots) {

      Con_Printf("%s: edict not listed as a client.\n", __FUNCTION__);
      return("");
   }

   return(Info_ValueForKey(global_svs.clients[i-1].UnknownStruct4DE0, key));
}
HLDS_DLLEXPORT void PF_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value) {

   unsigned int i;


   i = NUM_FOR_EDICT(pClient);
   if(i <= 0 || i > global_svs.allocated_client_slots) {

      Con_Printf("%s: edict not listed as a client.\n", __FUNCTION__);
      return;
   }

   Info_SetValueForKey(global_svs.clients[i-1].UnknownStruct4DE0, key, value, sizeof(global_svs.clients[i-1].UnknownStruct4DE0));
}
HLDS_DLLEXPORT const char * PF_GetPhysicsInfoString(const edict_t *pClient) {

   unsigned int i;


   i = NUM_FOR_EDICT(pClient);
   if(i <= 0 || i > global_svs.allocated_client_slots) {

      Con_Printf("%s: edict not listed as a client.\n", __FUNCTION__);
      return("");
   }

   return(global_svs.clients[i-1].UnknownStruct4DE0);
}

int IndexOfEdict(const edict_t *pEdict) {

   int i;


   if(pEdict == NULL) { return(0); }
   i = pEdict - global_sv.edicts;

   if(i < 0 || (unsigned)i > global_sv.max_edicts) {

      Sys_Error("%s: Index out of range.\n");
   }
   return(i);
}
edict_t* PEntityOfEntIndex(int iEntIndex) {

   edict_t * p;


   if(iEntIndex < 0 || (unsigned)iEntIndex > global_sv.max_edicts) {
      return(NULL);
   }

   p = EDICT_NUM(iEntIndex);
   if(p->free == 0 && p->pvPrivateData != NULL) { return(p); }
   if((unsigned)iEntIndex < global_svs.allocated_client_slots && p->free == 0) { return(p); }

   return(NULL);
}

HLDS_DLLEXPORT void PF_sound_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch) {

   if(volume < 0 || volume > 255) {
      Sys_Error("%s: sound out of range for sample %s (0..255)\n", __FUNCTION__, sample);
   }
   if(attenuation < 0 || attenuation > 4) {
      Sys_Error("%s: attenuation out of range for sample %s (0..4)\n", __FUNCTION__, sample);
   }
   if(channel < 0 || channel > 7) {
      Sys_Error("%s: invalid channel selected for sample %s (0..7)\n", __FUNCTION__, sample);
   }
   if(pitch < 0 || pitch > 255) {
      Sys_Error("%s: invalid pitch shift selected for sample %s (0..255)\n", __FUNCTION__, sample);
   }

   SV_StartSound(0, entity, channel, sample, volume * 255, attenuation, fFlags, pitch);
}


//This is exploitive, but should work perfectly with a good ol C compiler.
int iGetIndex(const char * field) {

/*
   entvars_t temp;


        if(Q_strcasecmp("classname", field) == 0) { return(temp.classname - temp.classname); }
   else if(Q_strcasecmp("model", field) == 0) { return(temp.model - temp.classname); }
   else if(Q_strcasecmp("viewmodel", field) == 0) { return(temp.viewmodel - temp.classname); }
   else if(Q_strcasecmp("weaponmodel", field) == 0) { return(temp.weaponmodel - temp.classname); }
   else if(Q_strcasecmp("netname", field) == 0) { return(temp.netname - temp.classname); }
   else if(Q_strcasecmp("target", field) == 0) { return(temp.target - temp.classname); }
   else if(Q_strcasecmp("targetname", field) == 0) { return(temp.targetname - temp.classname); }
   else if(Q_strcasecmp("message", field) == 0) { return(temp.message - temp.classname); }
   else if(Q_strcasecmp("noise", field) == 0) { return(temp.noise - temp.classname); }
   else if(Q_strcasecmp("noise1", field) == 0) { return(temp.noise1 - temp.classname); }
   else if(Q_strcasecmp("noise2", field) == 0) { return(temp.noise2 - temp.classname); }
   else if(Q_strcasecmp("noise3", field) == 0) { return(temp.noise3 - temp.classname); }
   else if(Q_strcasecmp("globalname", field) == 0) { return(temp.globalname - temp.classname); }
   else { return(-1); }
*/

        if(Q_strcasecmp("classname", field) == 0) { return((int)&(((entvars_t *)0)->classname)); }
   else if(Q_strcasecmp("model", field) == 0) { return((int)&(((entvars_t *)0)->model)); }
   else if(Q_strcasecmp("viewmodel", field) == 0) { return((int)&(((entvars_t *)0)->viewmodel)); }
   else if(Q_strcasecmp("weaponmodel", field) == 0) { return((int)&(((entvars_t *)0)->weaponmodel)); }
   else if(Q_strcasecmp("netname", field) == 0) { return((int)&(((entvars_t *)0)->netname)); }
   else if(Q_strcasecmp("target", field) == 0) { return((int)&(((entvars_t *)0)->target)); }
   else if(Q_strcasecmp("targetname", field) == 0) { return((int)&(((entvars_t *)0)->targetname)); }
   else if(Q_strcasecmp("message", field) == 0) { return((int)&(((entvars_t *)0)->message)); }
   else if(Q_strcasecmp("noise", field) == 0) { return((int)&(((entvars_t *)0)->noise)); }
   else if(Q_strcasecmp("noise1", field) == 0) { return((int)&(((entvars_t *)0)->noise1)); }
   else if(Q_strcasecmp("noise2", field) == 0) { return((int)&(((entvars_t *)0)->noise2)); }
   else if(Q_strcasecmp("noise3", field) == 0) { return((int)&(((entvars_t *)0)->noise3)); }
   else if(Q_strcasecmp("globalname", field) == 0) { return((int)&(((entvars_t *)0)->globalname)); }
   else { return(-1); }
}
edict_t * PF_find_Shared(int pEdictNum, int elementindex, const char *pszValue) {

   edict_t * ptr;
   char * str;


   pEdictNum++;
   while((unsigned)pEdictNum < global_sv.num_edicts) {

      ptr = &(global_sv.edicts[pEdictNum]);
      if(ptr->free == 0) {

         str = (char *)(*(int *)(((byte *)&ptr->v) + elementindex) + global_pr_strings); //weird
         if(str != NULL && str != global_pr_strings) {
            if(Q_strcmp(pszValue, str) == 0) {

               return(ptr);
            }
         }
      }

      pEdictNum++;
   }

   return(global_sv.edicts);
}
HLDS_DLLEXPORT edict_t * FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue) {

   int i;


   i = iGetIndex(pszField);
   if(i == -1 || pszValue == NULL) { return(NULL); }

   if(pEdictStartSearchAfter == NULL) {
      return(PF_find_Shared(0, i, pszValue));
   }
   else {
      return(PF_find_Shared(NUM_FOR_EDICT(pEdictStartSearchAfter), i, pszValue));
   }
}


HLDS_DLLEXPORT int PF_droptofloor_I(edict_t* e) {

   int var_48;
   vec3_t var_c;
   trace_t trace;


   if(e->v.flags & FL_MONSTERCLIP) {
      var_48 = 1;
   }
   else {
      var_48 = 0;
   }

   var_c[0] = e->v.origin[0];
   var_c[1] = e->v.origin[1];
   var_c[2] = e->v.origin[2] - 256;

   trace = SV_Move(e->v.origin, e->v.mins, e->v.maxs, var_c, 0, e, var_48);

   if(trace.allsolid != 0) { return(-1); }
   if(trace.fraction == 1) { return(0); }

   e->v.origin[0] = trace.endpos[0];
   e->v.origin[1] = trace.endpos[1];
   e->v.origin[2] = trace.endpos[2];

   SV_LinkEdict(e, 0);

   e->v.flags |= FL_ONGROUND;
   e->v.groundentity = trace.pHit;
   return(1);
}

HLDS_DLLEXPORT void PF_ambientsound_I(edict_t *entity, vec3_t pos, const char *samp, float vol, float attenuation, int fFlags, int pitch) {

   unsigned int i, var_c_num, var_10_ednum;
   char ** ptr;
   sizebuf_t * sz;

   if(samp[0] == '!') {

      fFlags |= 0x10;
      var_c_num = Q_atoi(samp+1);
      if(var_c_num > 0x5ff) {

         Con_Printf("%s: invalid sentence number: %s.\n", __FUNCTION__, samp);
         return;
      }
   }
   else {
      var_c_num = 0;
      ptr = global_sv.sound_precache;
      while(1) {

         if(ptr == NULL) {

            Con_Printf("%s: Coundn't find %s in precache.\n", __FUNCTION__, samp);
            return;
         }
         if(Q_strcasecmp(*ptr, samp) == 0) {
            break;
         }

         ptr++;
         var_c_num++;
      }
   }

   var_10_ednum = NUM_FOR_EDICT(entity);
   if(fFlags & 0x100) {
      sz = &(global_sv.signon_datagram);
   }
   else {
      sz = &(global_sv.reliable_datagram);
   }

   MSG_WriteByte(sz, 0x1D);
   for(i = 0; i < 3; i++) {

      MSG_WriteCoord(sz, pos[i]);
   }
   MSG_WriteShort(sz, var_c_num);
   MSG_WriteByte(sz, vol*255);
   MSG_WriteByte(sz, attenuation*64);
   MSG_WriteShort(sz, var_10_ednum);
   MSG_WriteByte(sz, pitch);
   MSG_WriteByte(sz, fFlags);
}

HLDS_DLLEXPORT void PF_makevectors_I(const vec3_t rgflVector) {

   AngleVectors(rgflVector, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
}
HLDS_DLLEXPORT void PF_setorigin_I(edict_t *e, const vec3_t rgflOrigin) {

   e->v.origin[0] = rgflOrigin[0];
   e->v.origin[1] = rgflOrigin[1];
   e->v.origin[2] = rgflOrigin[2];

   SV_LinkEdict(e, 0);
}
HLDS_DLLEXPORT void PF_setsize_I(edict_t *e, const vec3_t rgflMin, const vec3_t rgflMax) {

   SetMinMaxSize(e, rgflMin, rgflMax, 0);
}

HLDS_DLLEXPORT void PF_lightstyle_I(int style, char* val) {

   unsigned int i;
   client_t * cl;


   global_sv.padding3BB50[style] = val;
   if(global_sv.padding3bc64 != 2) { return; }

   for(i = 0, cl = global_svs.clients; i < global_svs.total_client_slots; i++, cl++) {

      if((cl->padding00_used == 0 && cl->padding04 == 0) || cl->padding2438_IsFakeClient != 0) { continue; }

      MSG_WriteChar(&(cl->netchan1C.netchan_message), 0x0c);
      MSG_WriteChar(&(cl->netchan1C.netchan_message), style);
      MSG_WriteString(&(cl->netchan1C.netchan_message), val);
   }
}
HLDS_DLLEXPORT int PF_DecalIndex(const char *name) {

   int i;


   for(i = 0; i < global_sv_decalnamecount; i++) {
      if(Q_strcasecmp(global_sv_decalnames[i], name) == 0) {
         return(i);
      }
   }

   return(-1);
}

HLDS_DLLEXPORT unsigned short EV_Precache(int type, const char * psz) {

   int i, size;
   byte * fp;


   if(psz == NULL) { Sys_Error("%s: NULL pointer.", __FUNCTION__); return(0); }
   if(PR_IsEmptyString(psz)) { Sys_Error("%s: Bad string.", __FUNCTION__); return(0); }

   if(global_sv.padding3bc64 == 1) {
      for(i = 1; i < 0x100; i++) {
         if(global_sv.padding31348[i].str == NULL) {

            if(type != 1) {
               Sys_Error("%s: only file type 1 is allowed.\n", __FUNCTION__);
            }

            fp = COM_LoadFileForMe(psz, &size);
            if(fp == NULL) {
               Sys_Error("%s: file %s couldn't be loaded.\n", __FUNCTION__, psz);
            }

            global_sv.padding31348[i].str = psz;
            global_sv.padding31348[i].size = size;
            global_sv.padding31348[i].mem = fp;
            global_sv.padding31348[i].index = i;

            return(i);
         }
         else if(Q_strcasecmp(global_sv.padding31348[i].str, psz) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: Overflow on %s.\n", __FUNCTION__, psz);
   }
   else {
      for(i = 1; i < 0x100; i++) {
         if(global_sv.padding31348[i].str != NULL && Q_strcasecmp(global_sv.padding31348[i].str, psz) == 0) {

            return(i);
         }
      }

      Sys_Error("%s: Precaching can only be done in spawn functions (%s).\n", __FUNCTION__, psz);
   }
}

HLDS_DLLEXPORT edict_t * PF_Spawn_I() {

   return(ED_Alloc());
}
HLDS_DLLEXPORT int  PF_IsDedicatedServer() { return(1); }
HLDS_DLLEXPORT void * PvAllocEntPrivateData(edict_t * pEdict, long cb) {

   FreeEntPrivateData(pEdict);
   if(cb > 0) {

      pEdict->pvPrivateData = Q_Malloc(cb);
      Q_memset(pEdict->pvPrivateData, 0, cb);
      return(pEdict->pvPrivateData);
   }

   return(NULL);
}
HLDS_DLLEXPORT void * PvEntPrivateData(edict_t *pEdict) {

   if(pEdict == NULL) {
      Con_Printf("%s: Passed NULL pointer.\n", __FUNCTION__);
      return(NULL);
   }

   return(pEdict->pvPrivateData);
}

//cheap hack
HLDS_DLLEXPORT int  PF_IsMapValid_I(const char * name) {

   char temp[0x100];

   sprintf(temp, "maps/%.32s.bsp", name);
   return(FS_FileExists(temp));
}

/* DESCRIPTION: SV_CheckVisibility
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnCheckVisibility)
//
//
*/
HLDS_DLLEXPORT int SV_CheckVisibility(edict_t * entity, unsigned char * pset) {

   int i;
   int var_10;


   if(pset == NULL) { return(1); }

   if(entity->headnode < 0) {
      for(i = 0; i < entity->num_leafs; i++) {

         if((pset[entity->leafnums[i] >> 3] & (1 << (entity->leafnums[i] & 7))) != 0) {
            return(1);
         }
      }
      return(0);
   }
   else {
      i = 0;
      do {

         var_10 = entity->leafnums[i];
         if(var_10 == -1) { break; }
         if((pset[var_10 >> 3] & (1 << (var_10 & 7))) != 0) { return(1); }
         i++;
      } while(i < 0x30);

      if(CM_HeadnodeVisible(&(global_sv.worldmodel->nodes[entity->headnode]), pset, &var_10) == 0) {
         return(0);
      }

      entity->leafnums[entity->num_leafs] = var_10;
      entity->num_leafs = (entity->num_leafs + 1) % MAX_ENT_LEAFS;
      return(2);
   }
}


/* DESCRIPTION: PF_checkclient_I
// LOCATION: pr_cmds.c
// PATH: g_engfuncsExportedToDlls (pfnFindClientInPVS)
//
// Seems to return an entity that would make a nice target for whoever
// pEdict is.  If it can't find one, it seems that the first
// entity is returned instead.
*/
//int c_invis, c_notvis; //Counters that exist in QW for debugging.
byte * checkpvs[0x400];
int PF_newcheckclient(unsigned int check) {

   unsigned int i;
   byte * var_8;
   vec3_t org;
   mleaf_t * leaf;
   edict_t * ent;


   if(check < 1) { check = 1; }
   else if(check > global_svs.allocated_client_slots) { check = global_svs.allocated_client_slots; }

   if(check == global_svs.allocated_client_slots) { i = 1; }
   else { i = check + 1; }

   for(; ; i++) {

      if(i == global_svs.allocated_client_slots+1) { i = 1; }

      ent = &(global_sv.edicts[i]);

      if(i == check) { break; }
      if(ent->free != 0) { continue; }
      if(ent->pvPrivateData == NULL) { continue; }
      if(ent->v.flags & FL_NOTARGET) { continue; }

      break;
   }

   org[0] = ent->v.origin[0] + ent->v.view_ofs[0];
   org[1] = ent->v.origin[1] + ent->v.view_ofs[1];
   org[2] = ent->v.origin[2] + ent->v.view_ofs[2];

   leaf = Mod_PointInLeaf(org, global_sv.worldmodel);
   var_8 = Mod_LeafPVS(leaf, global_sv.worldmodel);
   Q_memcpy(checkpvs, var_8, (global_sv.worldmodel->numleafs + 7) >> 3);

   return(i);
}
HLDS_DLLEXPORT edict_t * PF_checkclient_I(edict_t *pEdict) {

   vec3_t view;
   edict_t * ent;
   mleaf_t * leaf;
   int i;

   if(global_sv.time0c - global_sv.lastchecktime >= 0.1) {

      global_sv.lastcheck = PF_newcheckclient(global_sv.lastcheck);
      global_sv.lastchecktime = global_sv.time0c;
   }

   ent = &(global_sv.edicts[global_sv.lastcheck]);

   if(ent->free != 0 || ent->pvPrivateData == NULL) {
      return(&global_sv.edicts[0]);
   }

   view[0] = pEdict->v.origin[0] + pEdict->v.view_ofs[0];
   view[1] = pEdict->v.origin[1] + pEdict->v.view_ofs[1];
   view[2] = pEdict->v.origin[2] + pEdict->v.view_ofs[2];

   leaf = Mod_PointInLeaf(view, global_sv.worldmodel);
   i = (leaf - global_sv.worldmodel->leafs) - 1;

   if(i < 0 || ((int)(checkpvs[i / 8]) & (1 << (i & 7))) == 0) {
      //c_notvis++;
      return(&global_sv.edicts[0]);
   }

   //c_invis++;
   return(ent);
}

//Very long version exists in QW.
HLDS_DLLEXPORT void PF_setmodel_I(edict_t *e, const char *m) {

   int i;
   char ** ptr;
   static const vec3_t origin = { 0, 0, 0 };
   model_t * mod;

   i = 0;
   ptr = &(global_sv.model_precache[0]);
   while(1) {

      if(*ptr == NULL) {
         Sys_Error("%s: No precache for %s.\n", __FUNCTION__, m);
      }

      if(Q_strcasecmp(*ptr, m) == 0) { break; }

      i++;
      ptr++;
   }

   e->v.model = m - global_pr_strings;
   e->v.modelindex = i;

   mod = global_sv.models[i];
   if(mod == NULL) {
      SetMinMaxSize(e, origin, origin, 1);
   }
   else {
      SetMinMaxSize(e, mod->mins, mod->maxs, 1);
   }
}

//Since the server doesn't exactly play any sounds, I'm not surprised to see
//that this function doesn't appear to load any actual data anywhere.
HLDS_DLLEXPORT int  PF_precache_sound_I(char* s) {

   int i;

   if(s == NULL) { Sys_Error("%s: NULL pointer.", __FUNCTION__); return(0); }
   if(PR_IsEmptyString(s))   { Sys_Error("%s: Bad string.", __FUNCTION__); return(0); }
   if(*s == '!') { Sys_Error("%s: \"%s\" do not precache sentence names. (rguest note--someone want to tell me what this error means?)", __FUNCTION__, s); return(0); }


   if(global_sv.padding3bc64 == 1) {

      global_sv.padding33348 = 0;

      for(i = 0; i < MAX_SOUNDS; i++) {
         if(global_sv.sound_precache[i] == NULL) {

            global_sv.sound_precache[i] = s; //I don't trust mod writers to give us a static string... Look into this later when function is fully understood.
            return(i);
         }
         else if(Q_strcasecmp(global_sv.sound_precache[i], s) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: Sound \"%s\" couldn't be precached.  All %d precache slots were used.", __FUNCTION__, s, MAX_SOUNDS);
      return(0);
   }
   else {
      for(i = 0; i < MAX_SOUNDS; i++) {
         if(global_sv.sound_precache[i] != NULL && Q_strcasecmp(global_sv.sound_precache[i], s) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: '%s' Precache can only be done in spawn functions.", __FUNCTION__, s);
      return(0);
   }


}
HLDS_DLLEXPORT int  PF_precache_model_I(char* s) {

   int i;
   int ex;

   if(s == NULL) { Sys_Error("%s: NULL pointer.", __FUNCTION__); return(0); }
   if(PR_IsEmptyString(s))   { Sys_Error("%s: Bad string.", __FUNCTION__); return(0); }

   if(global_sv.padding3bc64 == 1) {

      if(*s == '!') { s++; ex = 1; } //Must be some special char.
      else { ex = 0; }

      for(i = 0; i < MAX_MODELS; i++) {
         if(global_sv.model_precache[i] == NULL) {

            global_sv.model_precache[i] = s; //I don't trust mod writers to give us a static string... Look into this later when function is fully understood.
            global_sv.models[i] = Mod_ForName(s, 1, 1);

            if(ex == 0) {

               global_sv.padding31148[i] |= 1;
            }

            return(i);
         }
         else if(Q_strcasecmp(global_sv.model_precache[i], s) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: Model \"%s\" couldn't be precached.  All %d precache slots were used.", __FUNCTION__, s, MAX_MODELS);
      return(0);
   }
   else {
      for(i = 0; i < MAX_MODELS; i++) {
         if(global_sv.model_precache[i] != NULL && Q_strcasecmp(global_sv.model_precache[i], s) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: '%s' Precache can only be done in spawn functions.", __FUNCTION__, s);
      return(0);
   }
}
//Noteworthy change--uses strcmp in one instance instead of Q_strcasecmp.  Probably an oversight by the maintainer.
HLDS_DLLEXPORT int  PF_precache_generic_I(char* s) {

   int i;


   if(s == NULL) { Sys_Error("%s: NULL pointer.", __FUNCTION__); return(0); }
   if(PR_IsEmptyString(s))   { Sys_Error("%s: Bad string.", __FUNCTION__); return(0); }

   if(global_sv.padding3bc64 == 1) {

      for(i = 0; i < MAX_GNERIC; i++) {
         if(global_sv.generic_precache[i] == NULL) {

            global_sv.generic_precache[i] = s; //I don't trust mod writers to give us a static string... Look into this later when function is fully understood.

            return(i);
         }
         else if(Q_strcasecmp(global_sv.generic_precache[i], s) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: Model \"%s\" couldn't be precached.  All %d precache slots were used.", __FUNCTION__, s, MAX_GNERIC);
      return(0);
   }
   else {
      for(i = 0; i < MAX_GNERIC; i++) {
         if(global_sv.generic_precache[i] != NULL && Q_strcmp(global_sv.generic_precache[i], s) == 0) {
            return(i);
         }
      }

      Sys_Error("%s: '%s' Precache can only be done in spawn functions.", __FUNCTION__, s);
      return(0);
   }
}


/* DESCRIPTION: AllocEngineString
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnAllocString)
//
// Wrapper for ED_NewString that returns the a pointer to the string.
// Wait, I lied.  It returns the pointer relative to global_pr_strings, which is
// a cheap way of blocking mod writers from messing with it.
*/
HLDS_DLLEXPORT int  AllocEngineString(const char *szValue) {

   return(ED_NewString(szValue) - global_pr_strings);
}

/* DESCRIPTION: PF_Remove_I
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnRemoveEntity)
//
// Edict freeing wrapper.
*/
HLDS_DLLEXPORT void PF_Remove_I(edict_t* e) {
   ED_Free(e);
}

/* DESCRIPTION: PF_makestatic
// LOCATION: pr_cmds.c
// PATH: g_engfuncsExportedToDlls (pfnMakeStatic)
//
// Copies various elements of the edict somewhere.
*/
HLDS_DLLEXPORT void PF_makestatic_I(edict_t *ent) {

   int mdlindex, i;

   mdlindex = SV_ModelIndex(global_pr_strings + ent->v.model);

   MSG_WriteByte(&(global_sv.signon_datagram), 0x14);
   MSG_WriteShort(&(global_sv.signon_datagram), mdlindex);
   MSG_WriteByte(&(global_sv.signon_datagram), ent->v.sequence);
   MSG_WriteByte(&(global_sv.signon_datagram), ent->v.frame);
   MSG_WriteWord(&(global_sv.signon_datagram), ent->v.colormap);
   MSG_WriteByte(&(global_sv.signon_datagram), ent->v.skin);

   for(i = 0; i < 3; i++) {
      MSG_WriteCoord(&(global_sv.signon_datagram), ent->v.origin[i]);
      MSG_WriteAngle(&(global_sv.signon_datagram), ent->v.angles[i]);
   }

   MSG_WriteByte(&(global_sv.signon_datagram), ent->v.rendermode);
   if(ent->v.rendermode != 0) {

      MSG_WriteByte(&(global_sv.signon_datagram), ent->v.renderamt);
      MSG_WriteByte(&(global_sv.signon_datagram), ent->v.rendercolor[0]);
      MSG_WriteByte(&(global_sv.signon_datagram), ent->v.rendercolor[1]);
      MSG_WriteByte(&(global_sv.signon_datagram), ent->v.rendercolor[2]);
      MSG_WriteByte(&(global_sv.signon_datagram), ent->v.renderfx);
   }

   ED_Free(ent);
}


/* DESCRIPTION: CreateNamedEntity
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (pfnCreateNamedEntity), some others
//
// broken GetEntityInit, dunno why--seems simple enough.
*/
HLDS_DLLEXPORT edict_t * CreateNamedEntity(int className) {

   edict_t * var_4;
   void (*var_8)(entvars_t *);

   if(className == 0) { Sys_Error("%s: NULL entity.", __FUNCTION__); }

   var_4 = ED_Alloc();

   var_4->v.classname = className;
   var_8 = (void (*)(entvars_t *))GetEntityInit(global_pr_strings + className);

   if(var_8 == NULL) {

      ED_Free(var_4);
      Con_Printf("%s: Couldn't create entity %s.", __FUNCTION__, global_pr_strings + className);
      return(NULL);
   }

   var_8(&(var_4->v));
   return(var_4);
}


/* DESCRIPTION: PF_checkbottom_I
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (as the less obvious pfnEntIsOnFloor)
//
// Boring old wrapper.
*/
HLDS_DLLEXPORT qboolean PF_checkbottom_I(edict_t *e) {

   return(SV_CheckBottom(e));
}

/* DESCRIPTION: PF_vectoangles_I
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (pfnVecToAngles)
//
// Wrapper.  Missed that math function the first time around.
*/
HLDS_DLLEXPORT void PF_vectoangles_I(const vec3_t rgflVectorIn, vec3_t rgflVectorOut) {

   VectorAngles(rgflVectorIn, rgflVectorOut);
}
/* DESCRIPTION: PF_vectoyaw_I
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (pfnVecToYaw)
//
// I don't actually know what yaw is, but I've gotten pretty good at
// manipulating these vectors.  This function is a bit different than other
// PF math functions--it's not a wrapper for something in the VecMath family
// (though maybe it should be).
*/
HLDS_DLLEXPORT float PF_vectoyaw_I(const vec3_t rgflVector) {

   double yaw;

   if(rgflVector[0] == 0 && rgflVector[1] == 0) { return(0); }

   yaw = atan2(rgflVector[1], rgflVector[0]) * 180.0 / M_PI;
   if(yaw < 0) { yaw += 360.0; }

   return((float)yaw);
}

/* DESCRIPTION: PF_GetPlayerWONId
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (pfnGetPlayerWONId)
//
// Goes through the list of clients looking for the given edict.
// Returns the WonID when found or -1 (well, an unsigned -1) if not.
*/
HLDS_DLLEXPORT unsigned int PF_GetPlayerWONId(edict_t *e) {

   unsigned int i;
   client_t * client;

   if(global_sv.active == 0 || e == NULL) { return(-1); }

   client = global_svs.clients;

   for(i = 0; i < global_svs.allocated_client_slots; i++, client++) {
      if(client->padding4A84 == e && client->authentication_method == 0) {
         return(client->authentication_WonID);
      }
   }

   return(-1);
}

/* DESCRIPTION: PF_GetPlayerUserId
// LOCATION: none
// PATH: g_engfuncsExportedToDlls (pfnGetPlayerUserId)
//
// Goes through the list of clients looking for the given edict.
// Doesn't return the index, instead returns a unique ID.
// Doesn't appear to be WonID--how UID isgenerated unknown.
*/
HLDS_DLLEXPORT int PF_GetPlayerUserId(edict_t *e) {

   unsigned int i;
   client_t * client;

   if(global_sv.active == 0 || e == NULL) { return(-1); }

   client = global_svs.clients;

   for(i = 0; i < global_svs.allocated_client_slots; i++, client++) {
      if(client->padding4A84 == e) {
         return(client->padding4A8C_PlayerUID);
      }
   }

   return(-1);
}

/* DESCRIPTION: PF_Time
// LOCATION: QW has time functions listed, but this is just a wrapper
// PATH: g_engfuncsExportedToDlls (pfnTime)
//
// A wrapper for sys_floattime.  Despite the name, sys_floattime returns
// a double, which this function casts down to a float.
*/
HLDS_DLLEXPORT float PF_Time() {
   return(Sys_FloatTime());
}

/* DESCRIPTION: PF_StaticDecal
// LOCATION: None
// PATH: g_engfuncsExportedToDlls (pfnStaticDecal)
//
// This function is actually a bit of a mystery.  I see no references to
// the sizebuf that it is referring to elsewhere...
*/
HLDS_DLLEXPORT void PF_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex) {

   MSG_WriteByte(&(global_sv.signon_datagram), 0x17);
   MSG_WriteByte(&(global_sv.signon_datagram), 0x0d);
   MSG_WriteCoord(&(global_sv.signon_datagram), origin[0]);
   MSG_WriteCoord(&(global_sv.signon_datagram), origin[1]);
   MSG_WriteCoord(&(global_sv.signon_datagram), origin[2]);
   MSG_WriteShort(&(global_sv.signon_datagram), decalIndex);
   MSG_WriteShort(&(global_sv.signon_datagram), entityIndex);

   if(entityIndex != 0) {
      MSG_WriteShort(&(global_sv.signon_datagram), modelIndex);
   }
}

/* DESCRIPTION: PF_GetCurrentPlayer
// LOCATION: None
// PATH: g_engfuncsExportedToDlls (pfnGetCurrentPlayer)
//
// We need to know the index of a client structure, and all we have
// is a pointer.  No prob, let's do some pointer arithmetic and figure it out.
*/
HLDS_DLLEXPORT int PF_GetCurrentPlayer() {

   int position;

   position = global_svs.clients - global_host_client;

   if(position < 0 || (unsigned int)position >= global_svs.allocated_client_slots) { return(-1); }
   return(position);
}

/* DESCRIPTION: PF_GetPlayerStats
// LOCATION: None
// PATH: g_engfuncsExportedToDlls (pfnGetPlayerStats)
//
// Sounds self-explanatory to me.  Things of note: clients[0-max]
// are linked to edicts[1-max+1]
*/
HLDS_DLLEXPORT void PF_GetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss) {

   int var_8_edictnum;
   client_t * var_4_client;

   *ping = 0;
   *packet_loss = 0;

   var_8_edictnum = NUM_FOR_EDICT(pClient);

   if(var_8_edictnum <= 0 || (unsigned int)var_8_edictnum > global_svs.allocated_client_slots) {
      Con_Printf("%s: Passed edict did not point to a known client.\n", __FUNCTION__);
      return;
   }
   var_4_client = &(global_svs.clients[var_8_edictnum-1]);
   *ping = var_4_client->ping * 1000;
   *packet_loss = var_4_client->packet_loss;
}

/* DESCRIPTION: PF_modelindex
// LOCATION: None
// PATH: g_engfuncsExportedToDlls (pfnModelIndex)
//
// Wrapper for SV_ModelIndex.  Nothing more.
*/
HLDS_DLLEXPORT int PF_modelindex(char * name) {
   return(SV_ModelIndex(name));
}

/* DESCRIPTION: PF_GetInfoKeyBuffer_I
// LOCATION: None
// PATH: g_engfuncsExportedToDlls (pfnGetInfoKeyBuffer)
//
// Returns a string of some sort.  That string may be empty, and exactly
// what it contains is a mystery I'm not worried about enough to check out.
*/
HLDS_DLLEXPORT char * PF_GetInfoKeyBuffer_I(edict_t *e) {

   int position;

   if(e == NULL) {
      return(global_localinfo);
   }

   position = NUM_FOR_EDICT(e);

   if(position == 0) {
      return(Info_Serverinfo());
   }
   if((unsigned int)position > global_svs.total_client_slots) {
      return("");
   }

   return(global_svs.clients[position-1].InfoKeyBuffer);
}

/* DESCRIPTION: PF_traceline_DLL (amd)
// LOCATION: None worth noting
// PATH: g_engfuncsExportedToDlls (pfnTraceLine)
//
// Notably different as it combines the above two functions.
*/
HLDS_DLLEXPORT void PF_traceline_DLL(vec3_t v1, vec3_t v2, int fNoMonsters, edict_t * pentToSkip, TraceResult_t * Outgoing_Trace) {

   static vec3_t origin = { 0, 0, 0 };
   trace_t TemporaryTrace;

   if(pentToSkip == NULL) {
      TemporaryTrace = SV_Move(v1, origin, origin, v2, fNoMonsters, global_sv.edicts, 0);
   }
   else {
      TemporaryTrace = SV_Move(v1, origin, origin, v2, fNoMonsters, pentToSkip, 0);
   }

   gGlobalVariables.trace_flags = 0;
   SV_SetGlobalTrace(&TemporaryTrace);

   Outgoing_Trace->fAllSolid = gGlobalVariables.trace_allsolid;
   Outgoing_Trace->fStartSolid = gGlobalVariables.trace_startsolid;
   Outgoing_Trace->fInOpen = gGlobalVariables.trace_inopen;
   Outgoing_Trace->fInWater = gGlobalVariables.trace_inwater;

   Outgoing_Trace->flFraction = gGlobalVariables.trace_fraction;
   Outgoing_Trace->flPlaneDist = gGlobalVariables.trace_plane_dist;

   Outgoing_Trace->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
   Outgoing_Trace->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
   Outgoing_Trace->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
   Outgoing_Trace->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
   Outgoing_Trace->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
   Outgoing_Trace->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];

   Outgoing_Trace->pHit = gGlobalVariables.trace_ent;
   Outgoing_Trace->iHitgroup = gGlobalVariables.trace_hitgroup;
}

/* DESCRIPTION: TraceHull
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnTraceHull)
//
// Usual old trace.  Why oh why do we have all these different conventions?
// One uses globals, another does not...
*/
HLDS_DLLEXPORT void TraceHull(vec3_t v1, vec3_t v2, int fNoMonsters, int hullNumber, edict_t * pentToSkip, TraceResult_t * Outgoing_Trace) {

   trace_t var_38_trace;

   //Some sort of default check
   if(hullNumber < 0 || hullNumber > 3) { hullNumber = 0; }

   var_38_trace = SV_Move(v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2, fNoMonsters, pentToSkip, 0);

   Outgoing_Trace->fAllSolid = var_38_trace.allsolid;
   Outgoing_Trace->fStartSolid = var_38_trace.startsolid;
   Outgoing_Trace->fInOpen = var_38_trace.inopen;
   Outgoing_Trace->fInWater = var_38_trace.inwater;

   Outgoing_Trace->flFraction = var_38_trace.fraction;
   Outgoing_Trace->flPlaneDist = var_38_trace.plane.dist;

   Outgoing_Trace->vecEndPos[0] = var_38_trace.endpos[0];
   Outgoing_Trace->vecEndPos[1] = var_38_trace.endpos[1];
   Outgoing_Trace->vecEndPos[2] = var_38_trace.endpos[2];
   Outgoing_Trace->vecPlaneNormal[0] = var_38_trace.plane.normal[0];
   Outgoing_Trace->vecPlaneNormal[1] = var_38_trace.plane.normal[1];
   Outgoing_Trace->vecPlaneNormal[2] = var_38_trace.plane.normal[2];

   Outgoing_Trace->pHit = var_38_trace.pHit;
   Outgoing_Trace->iHitgroup = var_38_trace.hitgroup;
}

/* DESCRIPTION: TraceSphere
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnTraceSphere)
//
// Never used.  Never implemented, for that matter.
*/
HLDS_DLLEXPORT void TraceSphere(/*vec3_t v1, vec3_t v2, int fNoMonsters, float radius, edict_t * pentToSkip, TraceResult_t * Outgoing_Trace*/) {

  // trace_t var_38_trace;

   //Heh heh
   Sys_Error("TraceSphere called.  HL1110 never implemented tracesphere, tough luck.");
}

/* DESCRIPTION: TraceModel
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnTraceModel)
//
// A slightly more involved trace.  Still need to discern those various constants.
*/
HLDS_DLLEXPORT void TraceModel(vec3_t v1, vec3_t v2, int hullNumber, edict_t *pent, TraceResult_t * Outgoing_Trace) {

   trace_t var_38_trace;
   model_t * var_44_model;
   int var_40_temptype;
   int var_3c_tempsolid;

   if(hullNumber < 0 || hullNumber > 3) { hullNumber = 0; }

   var_44_model = global_sv.models[pent->v.modelindex];

   if(var_44_model != NULL && var_44_model->modeltype == 0) { //structured this way to get rid of annoying warning

      var_40_temptype = pent->v.movetype;
      var_3c_tempsolid = pent->v.solid;
      pent->v.movetype = MOVETYPE_PUSH;
      pent->v.solid = SOLID_BSP;

      SV_ClipMoveToEntity(&var_38_trace, pent, v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2);

      pent->v.movetype = var_40_temptype;
      pent->v.solid = var_3c_tempsolid;
   }
   else {
      SV_ClipMoveToEntity(&var_38_trace, pent, v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2);
   }

   Outgoing_Trace->fAllSolid = var_38_trace.allsolid;
   Outgoing_Trace->fStartSolid = var_38_trace.startsolid;
   Outgoing_Trace->fInOpen = var_38_trace.inopen;
   Outgoing_Trace->fInWater = var_38_trace.inwater;

   Outgoing_Trace->flFraction = var_38_trace.fraction;
   Outgoing_Trace->flPlaneDist = var_38_trace.plane.dist;

   Outgoing_Trace->vecEndPos[0] = var_38_trace.endpos[0];
   Outgoing_Trace->vecEndPos[1] = var_38_trace.endpos[1];
   Outgoing_Trace->vecEndPos[2] = var_38_trace.endpos[2];
   Outgoing_Trace->vecPlaneNormal[0] = var_38_trace.plane.normal[0];
   Outgoing_Trace->vecPlaneNormal[1] = var_38_trace.plane.normal[1];
   Outgoing_Trace->vecPlaneNormal[2] = var_38_trace.plane.normal[2];

   Outgoing_Trace->pHit = var_38_trace.pHit;
   Outgoing_Trace->iHitgroup = var_38_trace.hitgroup;
}

/* DESCRIPTION: TraceTexture
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnTraceTexture)
//
//
*/
HLDS_DLLEXPORT const char * TraceTexture(edict_t *pTextureEntity, vec3_t v1, vec3_t v2) {

   static vec3_t origin = { 0, 0, 0 };

   model_t * var_4_model;
   msurface_t * var_8;
   int var_c = 0;
   vec3_t var_18, var_24, var_30;
   hull_t * var_34_hull;
   vec3_t var_40_forward, var_4C_right, var_58_up;
   vec3_t var_64;

   if(pTextureEntity == NULL) {

      var_4_model = global_sv.worldmodel;
      var_18[0] = v1[0];
      var_18[1] = v1[1];
      var_18[2] = v1[2];
      var_24[0] = v2[0];
      var_24[1] = v2[1];
      var_24[2] = v2[2];
   }
   else {

      var_4_model = global_sv.models[pTextureEntity->v.modelindex];

      if(var_4_model == NULL || var_4_model->modeltype != 0) { return(NULL); }

      var_34_hull = SV_HullForBsp(pTextureEntity, origin, origin, var_30);
      var_c = var_34_hull->firstclipnode;

      var_18[0] = v1[0] - var_30[0];
      var_18[1] = v1[1] - var_30[1];
      var_18[2] = v1[2] - var_30[2];
      var_24[0] = v2[0] - var_30[0];
      var_24[1] = v2[1] - var_30[1];
      var_24[2] = v2[2] - var_30[2];

      if(pTextureEntity->v.angles[0] != 0 || pTextureEntity->v.angles[1] != 0 || pTextureEntity->v.angles[2] != 0) {

         AngleVectors(pTextureEntity->v.angles, var_40_forward, var_4C_right, var_58_up);

         var_64[0] = var_18[0];
         var_64[1] = var_18[1];
         var_64[2] = var_18[2];
         //dot
         var_18[0] = (var_64[0] * var_40_forward[0]) + (var_64[1] * var_40_forward[1]) + (var_64[2] * var_40_forward[2]);
         var_18[1] = -((var_64[0] * var_4C_right[0]) + (var_64[1] * var_4C_right[1]) + (var_64[2] * var_4C_right[2]));
         var_18[2] = (var_64[0] * var_58_up[0]) + (var_64[1] * var_58_up[1]) + (var_64[2] * var_58_up[2]);

         var_64[0] = var_24[0];
         var_64[1] = var_24[1];
         var_64[2] = var_24[2];

         var_24[0] = (var_64[0] * var_40_forward[0]) + (var_64[1] * var_40_forward[1]) + (var_64[2] * var_40_forward[2]);
         var_24[1] = -((var_64[0] * var_4C_right[0]) + (var_64[1] * var_4C_right[1]) + (var_64[2] * var_4C_right[2]));
         var_24[2] = (var_64[0] * var_58_up[0]) + (var_64[1] * var_58_up[1]) + (var_64[2] * var_58_up[2]);
      }
   }

   if(var_4_model == NULL || var_4_model->modeltype != 0 || //I thought we did those two checks already...
      var_4_model->nodes == NULL) { return(NULL); }

   var_8 = SurfaceAtPoint(var_4_model, &(var_4_model->nodes[var_c]), var_18, var_24);
   if(var_8 == NULL) { return(NULL); }

   return(var_8->texinfo->texture->name);
}

/* DESCRIPTION: SurfaceAtPoint
// LOCATION:
// PATH: TraceTexture, recursive
//
// A weird one.  First, it seems to recursively call itself to dig deep into
// the node tree, treating its own failure as a sign that it's dug deep
// enough.  Then, it loops through some odd texture stuff, looking for
// a match.  A match of what?  Don't know yet.
*/
msurface_t * SurfaceAtPoint(model_t * model, mnode_t * node, vec3_t v1, vec3_t v2) {

   double var_4, var_8, var_c;
   int var_10, var_10_2;
   mplane_t   * var_14_plane;
   vec3_t var_20;
   msurface_t * var_24_surface;
   int var_28, var_2c;
   int var_30, var_34;
   int var_38;
   mtexinfo_t * var_3C_texinfo;


   if(node->contents < 0) { return(NULL); }

   var_14_plane = node->plane;

   var_4 = ((v1[0] * var_14_plane->normal[0]) + (v1[1] * var_14_plane->normal[1]) + (v1[2] * var_14_plane->normal[2])) - var_14_plane->dist;
   var_8 = ((v2[0] * var_14_plane->normal[0]) + (v2[1] * var_14_plane->normal[1]) + (v2[2] * var_14_plane->normal[2])) - var_14_plane->dist;

   if(var_4 < 0) { var_10 = 1; }
   else { var_10 = 0; }

   if(var_8 < 0) { var_10_2 = 1; }
   else { var_10_2 = 0; }

   if(var_10 == var_10_2) {

      return(SurfaceAtPoint(model, node->children[var_10], v1, v2));
   }

   var_c = var_4 / (var_4 - var_8);

   var_20[0] = ((v2[0] - v1[0]) * var_c) + v1[0];
   var_20[1] = ((v2[1] - v1[1]) * var_c) + v1[1];
   var_20[2] = ((v2[2] - v1[2]) * var_c) + v1[2];

   //Now THIS is weird.
   var_24_surface = SurfaceAtPoint(model, node->children[var_10], v1, var_20);
   if(var_24_surface != NULL || var_10 == var_10_2) { return(var_24_surface); } //Second check not possible as in asm... I think.


   var_24_surface = &(model->surfaces[node->firstsurface]);

   for(var_38 = 0; var_38 < node->numsurfaces; var_38++, var_24_surface++) {

      var_3C_texinfo = var_24_surface->texinfo;

      var_28 = (var_20[0] * var_3C_texinfo->vecs[0][0]) + (var_20[1] * var_3C_texinfo->vecs[0][1]) + (var_20[2] * var_3C_texinfo->vecs[0][2]) + var_3C_texinfo->vecs[0][3];
      var_2c = (var_20[0] * var_3C_texinfo->vecs[1][0]) + (var_20[1] * var_3C_texinfo->vecs[1][1]) + (var_20[2] * var_3C_texinfo->vecs[1][2]) + var_3C_texinfo->vecs[1][3];

      if(var_28 >= var_24_surface->texturemins[0] && var_2c >= var_24_surface->texturemins[1]) {

         var_30 = var_28 - var_24_surface->texturemins[0];
         var_34 = var_2c - var_24_surface->texturemins[1];

         if(var_30 <= var_24_surface->extents[0] && var_34 <= var_24_surface->extents[1]) {

            return(var_24_surface);
         }
      }
   }

   if(var_10 == 1) { var_10 = 0; }
   else { var_10 = 1; }
   return(SurfaceAtPoint(model, node->children[var_10], var_20, v2));
}

/* DESCRIPTION: TraceMonsterHull
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnTraceMonsterHull)
//
//
*/
HLDS_DLLEXPORT int TraceMonsterHull(edict_t * pEdict, vec3_t v1, vec3_t v2, int fNoMonsters, edict_t * pentToSkip, TraceResult_t * Outgoing_Trace) {

   int var_3c;
   trace_t var_38_trace;


   if((pEdict->v.flags & FL_MONSTERCLIP) == 0) { var_3c = 0; }
   else { var_3c = 1; }

   var_38_trace = SV_Move(v1, pEdict->v.mins, pEdict->v.maxs, v2, fNoMonsters, pentToSkip, var_3c);

   if(Outgoing_Trace != NULL) { //wonder why this one allows NULL traces...

      Outgoing_Trace->fAllSolid = var_38_trace.allsolid;
      Outgoing_Trace->fStartSolid = var_38_trace.startsolid;
      Outgoing_Trace->fInOpen = var_38_trace.inopen;
      Outgoing_Trace->fInWater = var_38_trace.inwater;

      Outgoing_Trace->flFraction = var_38_trace.fraction;
      Outgoing_Trace->flPlaneDist = var_38_trace.plane.dist;

      Outgoing_Trace->vecEndPos[0] = var_38_trace.endpos[0];
      Outgoing_Trace->vecEndPos[1] = var_38_trace.endpos[1];
      Outgoing_Trace->vecEndPos[2] = var_38_trace.endpos[2];
      Outgoing_Trace->vecPlaneNormal[0] = var_38_trace.plane.normal[0];
      Outgoing_Trace->vecPlaneNormal[1] = var_38_trace.plane.normal[1];
      Outgoing_Trace->vecPlaneNormal[2] = var_38_trace.plane.normal[2];

      Outgoing_Trace->pHit = var_38_trace.pHit;
      Outgoing_Trace->iHitgroup = var_38_trace.hitgroup;
   }

   if(var_38_trace.allsolid != 0 || var_38_trace.fraction != 1) { return(1); }

   return(0);
}



/* DESCRIPTION: PF_TraceToss_Shared
// LOCATION: None worth noting
// PATH: PF_TraceToss_DLL
//
// Serves as little more than a bridge between the mod and the engine.
*/
void PF_TraceToss_Shared(edict_t *tossent, edict_t *ignore) {

   trace_t TemporaryTrace;

   SV_Trace_Toss(&TemporaryTrace, tossent, ignore);
   SV_SetGlobalTrace(&TemporaryTrace);
}
/* DESCRIPTION: PF_TraceToss_DLL
// LOCATION: None worth noting
// PATH: g_engfuncsExportedToDlls (so a mod function)
//
// Once more, bridges the gap between SV_TraceToss and the mod.
// Is there a reason why we're going through the global vars to return this?
*/
HLDS_DLLEXPORT void PF_TraceToss_DLL(edict_t * pent, edict_t * pentToIgnore, TraceResult_t * Outgoing_Trace) {

   if(pentToIgnore == NULL) {
      PF_TraceToss_Shared(pent, global_sv.edicts);
   }
   else {
      PF_TraceToss_Shared(pent, pentToIgnore);
   }

   Outgoing_Trace->fAllSolid = gGlobalVariables.trace_allsolid;
   Outgoing_Trace->fStartSolid = gGlobalVariables.trace_startsolid;
   Outgoing_Trace->fInOpen = gGlobalVariables.trace_inopen;
   Outgoing_Trace->fInWater = gGlobalVariables.trace_inwater;

   Outgoing_Trace->flFraction = gGlobalVariables.trace_fraction;
   Outgoing_Trace->flPlaneDist = gGlobalVariables.trace_plane_dist;

   Outgoing_Trace->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
   Outgoing_Trace->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
   Outgoing_Trace->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
   Outgoing_Trace->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
   Outgoing_Trace->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
   Outgoing_Trace->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];

   Outgoing_Trace->pHit = gGlobalVariables.trace_ent;
   Outgoing_Trace->iHitgroup = gGlobalVariables.trace_hitgroup;
}

/* DESCRIPTION: FreeEntPrivateData
// LOCATION:
// PATH: g_engfuncsExportedToDlls, also internal freeing. (pfnFreeEntPrivateData)
//
// Frees memory.  Lets the MOD have a run at it too.
*/
HLDS_DLLEXPORT void FreeEntPrivateData(edict_t *ed) {

   if(ed->pvPrivateData == NULL) { return; }

   if(gNewDLLFunctions.pfnOnFreeEntPrivateData != NULL) {
      gNewDLLFunctions.pfnOnFreeEntPrivateData(ed);
   }

   Q_Free(ed->pvPrivateData);
   ed->pvPrivateData = NULL;
}

//Because.
void FreeAllEntPrivateData() {

   unsigned int i;

   for(i = 0; i < global_sv.num_edicts; i++) {

      FreeEntPrivateData(&(global_sv.edicts[i]));
   }
}




/* DESCRIPTION: PF_Cvar_DirectSet
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnCvar_DirectSet)
//
// wrapper
*/
HLDS_DLLEXPORT void PF_Cvar_DirectSet(cvar_t * var, char * value) {

   Cvar_DirectSet(var, value);
}



//This was pretty easy.
int ValidCmd(const char * command) {

   // I personally think that, instead of just strlen()ing, this'd be a great
   // place to check for little things like shellcode.

   int len = strlen(command);

   if(len != 0 && (command[len-1] == '\n' || command[len-1] == ';')) { return(1); }
   return(0);
}

void Host_ClientCommands(char * fmt, ...) {

   static char Buffer[0x400];
   va_list argptr;

   if(global_host_client == NULL) { return; }

   va_start(argptr, fmt);
   vsnprintf(Buffer, 0x3FF, fmt, argptr);
   va_end(argptr);
   Buffer[0x3FF] = '\0'; //maybe we should explicitly check for truncating...

   MSG_WriteByte(&(global_host_client->netchan1C.netchan_message), 9);
   MSG_WriteString(&(global_host_client->netchan1C.netchan_message), Buffer);
}

/* DESCRIPTION: PF_localcmd_I
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnServerCommand), WriteDest_Parm
//
// 'wrapper' for Cbuf_AddText
*/
HLDS_DLLEXPORT void PF_localcmd_I(char * str) {

   if(ValidCmd(str)) {
      Cbuf_AddText(str);
   }
   else {
      Con_Printf("%s: Error, bad server command %s", __FUNCTION__, str);
   }
}
/* DESCRIPTION: PF_localexec_I
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnServerExecute)
//
// wrapper for Cbuf_Execute
*/
HLDS_DLLEXPORT void PF_localexec_I() {

   Cbuf_Execute();
}
/* DESCRIPTION: PF_stuffcmd_I
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnClientCommand)
//
// variable string wrapper for Host_ClientCommands
*/
HLDS_DLLEXPORT void PF_stuffcmd_I(edict_t * pEdict, char * szFmt, ...) {

   static char szOut_105[0x400];

   int var_4_edictnum;
   client_t * var_8_temp_host_client;
   va_list argptr;

   var_4_edictnum = NUM_FOR_EDICT(pEdict);


   va_start(argptr, szFmt);
   vsnprintf(szOut_105, 0x3FF, szFmt, argptr);
   va_end(argptr);

   szOut_105[0x3FF] = '\0'; //maybe we should explicitly check for truncating...

   if(var_4_edictnum <= 0 || (unsigned)var_4_edictnum > global_svs.allocated_client_slots) {
      Con_Printf("%s: Some entity tried to stuff \"%s\" to console buffer of entity %i when allocated_client_slots was set to %i.", __FUNCTION__, szOut_105, var_4_edictnum, global_svs.allocated_client_slots);
   }
   else if(ValidCmd(szOut_105) == 0) {
      Con_Printf("%s: Tried to stuff bad command %s", __FUNCTION__, szOut_105);
   }
   else {
      var_8_temp_host_client = global_host_client;
      global_host_client = &(global_svs.clients[var_4_edictnum-1]);
      Host_ClientCommands("%s", szOut_105); //glaringly inefficient.
      global_host_client = var_8_temp_host_client;
   }
}

HLDS_DLLEXPORT void ServerPrint(const char *szMsg) {
   Con_Printf("%s", szMsg);
}


static int gMsgStarted = 0;
static int gMsgDest = 0;
static int gMsgType = 0;
static edict_t * gMsgEntity = NULL;

static vec3_t gMsgOrigin = { 0, 0, 0 };
static byte gMsgData[0x200];
static sizebuf_t gMsgBuffer = { "MessageBegin/End", 0, gMsgData, sizeof(gMsgData), 0 };

/* DESCRIPTION: RegUserMsg
// LOCATION:
// PATH: g_engfuncsExportedToDlls (pfnClientCommand), Host_Say_f
//
// You know the custom commands mods can add to the engine?  This keeps track
// of 'em.  Anyone wanting to add a custom command does so here, and is
// given the byte code to identify it.  That means it technically doubles
// as a search function as well.
*/
HLDS_DLLEXPORT int RegUserMsg(const char *pszName, int iSize) {

   static int giNextUserMsg = 0x40; //we have this many codes by default.
   gpnewusermsg_struct_t * Messages;

   if(giNextUserMsg > 0xFF) {
      Con_Printf("%s: Cannot add message type.  Maximum number allowed reached.\n", __FUNCTION__);
      return(0);
   }
   if(pszName == NULL) {
      Con_Printf("%s: NULL name passed.\n", __FUNCTION__);
      return(0);
   }
   if(iSize > 0xC0) {
      Con_Printf("%s: A hard size limit cannot be above 192.\n", __FUNCTION__);
      return(0);
   }
   if(pszName[0] == '\0' || Q_strlen(pszName) > 11) {
      Con_Printf("%s: Name must be between 1 and 11 letters long.\n", __FUNCTION__);
      return(0);
   }


   for(Messages = global_sv_gpUserMsg; Messages != NULL; Messages = Messages->next) {

      if(Q_strcmp(Messages->padding08_name.asString, pszName) == 0) {
         return(Messages->padding00_id);
      }
   }

   Messages = Q_Malloc(sizeof(gpnewusermsg_struct_t));
   CHECK_MEMORY_MALLOC(Messages);

   Messages->padding00_id = giNextUserMsg;
   giNextUserMsg++;

   Messages->padding04_size = iSize;

   //For a change, I want the behaviour this has of writing zeros to the rest of the buffer.
   Q_strncpy(Messages->padding08_name.asString, pszName, 0x10);
   Messages->next = global_sv_gpUserMsg;
   global_sv_gpUserMsg = Messages;
   //should I zero out those last four bytes?
   return(Messages->padding00_id);
}

//Only called on program termination.  Were it posible to change mods
//halfway through a server's execution, we'd call this among mother things.
void FreeUserMsgs() {

   gpnewusermsg_struct_t * ptr, * ptr2;


   for(ptr = global_sv_gpUserMsg; ptr != NULL; ptr = ptr2) {
      ptr2 = ptr->next;
      Q_Free(ptr);
   }
   for(ptr = global_sv_gpNewUserMsg; ptr != NULL; ptr = ptr2) {
      ptr2 = ptr->next;
      Q_Free(ptr);
   }
   global_sv_gpUserMsg = NULL;
   global_sv_gpNewUserMsg = NULL;
}

//QW name is QWWriteDest
sizebuf_t * WriteDest_Parm(int dest) {

   unsigned int entnum;


   switch(dest) {

   case MSG_BROADCAST:
      return(&(global_sv.reliable_datagram));

   case MSG_ONE:
      entnum = NUM_FOR_EDICT(gMsgEntity);
      if(entnum < 1 || entnum > global_svs.allocated_client_slots) {
         Sys_Error("%s: not a client.\n",__FUNCTION__);
      }
      return(&(global_svs.clients[entnum-1].netchan1C.netchan_message));

   case MSG_ONE_UNRELIABLE:
      entnum = NUM_FOR_EDICT(gMsgEntity);
      if(entnum < 1 || entnum > global_svs.allocated_client_slots) {
         Sys_Error("%s: not a client.\n",__FUNCTION__);
      }

      return(&(global_svs.clients[entnum-1].sizebuf24AC));

   case MSG_ALL:
      return(&(global_sv.sizebuf3CC1C));

   case MSG_INIT:
      return(&(global_sv.signon_datagram));

   case MSG_PVS:
   case MSG_PAS:
      return(&(global_sv.sizebuf3dbd0_multicast));

   case MSG_SPEC:
      return(&(global_sv.spectator_datagram));

   case MSG_PVS_R:
   case MSG_PAS_R:
   default:

      Sys_Error("%s: %i is not a valid destination.\n", __FUNCTION__, dest);
   }
}


HLDS_DLLEXPORT void PF_MessageBegin_I(int msg_dest, int msg_type, const vec3_t pOrigin, edict_t *ed) {

   if(gMsgStarted != 0) {
      Sys_Error("%s: Called with an active message.\n", __FUNCTION__);
   }

   if(msg_dest == MSG_ONE || msg_dest == MSG_ONE_UNRELIABLE) { //single user
      if(ed == NULL) {
         Sys_Error("%s: MSG_ONE and MSG_ONE_UNRELIABLE require a target entity.\n", __FUNCTION__);
      }
   }
   else { //broadcast
      if(ed != NULL) {
         Sys_Error("%s: Cannot include a target entity for broadcast messages.\n", __FUNCTION__);
      }
   }
   if(msg_type == 0) {
      Sys_Error("%s: \"msgtype\" cannot be set to 0.\n", __FUNCTION__);
   }

   //Failure cases are over.
   gMsgStarted = 1;
   gMsgDest = msg_dest;
   gMsgType = msg_type;
   gMsgEntity = ed;
   gMsgBuffer.overflow = 1; //allow
   gMsgBuffer.cursize = 0;


   if(msg_dest == MSG_PAS || msg_dest == MSG_PVS) {
      if(pOrigin != NULL) {

         gMsgOrigin[0] = pOrigin[0];
         gMsgOrigin[1] = pOrigin[1];
         gMsgOrigin[2] = pOrigin[2];
      }
   }
}
HLDS_DLLEXPORT void PF_MessageEnd_I() {

   unsigned int entnum, var_4 = 0;
   gpnewusermsg_struct_t * MessageTypes;
   client_t * cl;
   sizebuf_t * DestBuf;


   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }
   gMsgStarted = 0;

   if(gMsgEntity != NULL && (gMsgEntity->v.flags & FL_FAKECLIENT) != 0) {
      return;
   }
   if(gMsgBuffer.overflow & 2) {
      Sys_Error("%s: Message overflowed.\n", __FUNCTION__);
   }

   if(gMsgType > 0x37) { //I'm guessing the first 55 are reserved.

      MessageTypes = global_sv_gpUserMsg;
      while(1) {

         if(MessageTypes == NULL) {
            Con_Printf("%s: Unknown user msg %i.\n", __FUNCTION__, gMsgType);
            return;
         }
         if(MessageTypes->padding00_id == gMsgType) {
            break;
         }

         MessageTypes = MessageTypes->next;
      }

      //MessageTypes contains--surprise--the message type.

      if(MessageTypes->padding04_size == -1) {
         var_4 = 1;
         if(gMsgBuffer.cursize > 0xC0) {
            Sys_Error("%s: Message is too big.\n", __FUNCTION__);
         }
      }
      else {
         if(gMsgBuffer.cursize != (unsigned)MessageTypes->padding04_size) {
            Sys_Error("%s: Message was not expected size.\n", __FUNCTION__);
         }
      }
   }


   DestBuf = WriteDest_Parm(gMsgDest);
   if(gMsgDest == MSG_BROADCAST && DestBuf->cursize + gMsgBuffer.cursize > DestBuf->maxsize) {

      Con_Printf("%s: Broadcast message would have overflowed buffer.\n", __FUNCTION__);
      return;
   }

   //ignored check--why would a sizebuf's buffer not be allocated?

   if(gMsgType > 0x37 && (gMsgDest == MSG_ONE_UNRELIABLE || gMsgDest == MSG_ONE)) {

      entnum = NUM_FOR_EDICT(gMsgEntity);
      if(entnum < 1 || entnum > global_svs.allocated_client_slots) {
         Sys_Error("%s: not a client.\n",__FUNCTION__);
      }

      cl = &(global_svs.clients[entnum-1]);
      if(cl->padding14 == 0 || (cl->padding00_used == 0 && cl->padding04 == 0) || cl->padding2438_IsFakeClient != 0) {
         return;
      }


   }

   MSG_WriteByte(DestBuf, gMsgType);
   if(var_4 != 0) {
      MSG_WriteByte(DestBuf, gMsgBuffer.cursize);
   }

   MSG_WriteBuf(DestBuf, gMsgBuffer.cursize, gMsgBuffer.data);

   switch(gMsgDest) { //only the following qualify

   case MSG_PVS:
      SV_Multicast(gMsgEntity, gMsgOrigin, 2, 0);
      break;

   case MSG_PAS:
      SV_Multicast(gMsgEntity, gMsgOrigin, 4, 0);
      break;

   case MSG_PVS_R:
      SV_Multicast(gMsgEntity, gMsgOrigin, 4, 1);
      break;

   case MSG_PAS_R:
      SV_Multicast(gMsgEntity, gMsgOrigin, 4, 1);
      break;
   default:
      break;
   }
}

HLDS_DLLEXPORT void PF_WriteByte_I(int iValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteByte(&gMsgBuffer, iValue);
}
HLDS_DLLEXPORT void PF_WriteChar_I(int iValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteChar(&gMsgBuffer, iValue);
}
HLDS_DLLEXPORT void PF_WriteShort_I(int iValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteShort(&gMsgBuffer, iValue);
}
HLDS_DLLEXPORT void PF_WriteLong_I(int iValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteLong(&gMsgBuffer, iValue);
}
HLDS_DLLEXPORT void PF_WriteAngle_I(float flValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteAngle(&gMsgBuffer, flValue);
}
HLDS_DLLEXPORT void PF_WriteCoord_I(float flValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteShort(&gMsgBuffer, flValue * 8); //eh?
}
HLDS_DLLEXPORT void PF_WriteString_I(const char *sz) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteString(&gMsgBuffer, sz);
}
HLDS_DLLEXPORT void PF_WriteEntity_I(int iValue) {

   if(gMsgStarted == 0) {
      Sys_Error("%s: Called without an active message.\n", __FUNCTION__);
   }

   MSG_WriteShort(&gMsgBuffer, iValue);
}


//Cheaty function uses one of the above statics.
HLDS_DLLEXPORT void PF_BuildSoundMsg_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const vec3_t pOrigin, edict_t *ed) {

   PF_MessageBegin_I(msg_dest, msg_type, pOrigin, ed);
   SV_BuildSoundMsg(entity, channel, sample, volume, attenuation, fFlags, pitch, pOrigin, &gMsgBuffer);
   PF_MessageEnd_I();
}

void PF_Init() {

   Cvar_RegisterVariable(&cvar_sv_aim);
}
