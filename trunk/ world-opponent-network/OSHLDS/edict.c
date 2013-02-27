#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "SV_hlds.h"
#include "common.h"
#include "sys.h"
#include "cvar.h"
#include "Modding.h"
#include "NET.h"

#include "FS.hpp"

#include "report.h"
#include "Q_memdef.h"
#include "pf.h"

edict_t * EDICT_NUM(const unsigned int num) {

   if(num >= global_sv.max_edicts) { //Could just use an unsigned comparison.
      Sys_Error("%s: %i is a bad edict number.", __FUNCTION__, num);
   }

   return(&(global_sv.edicts[num]));
}
int NUM_FOR_EDICT(const edict_t *e) {

   int position;

   position = e - global_sv.edicts;

   if(position < 0 || (unsigned)position >= global_sv.num_edicts) {
      Sys_Error("%s: Bad pointer. Bailing.", __FUNCTION__);
   }
   return(position);
}


void * GetEntityInit(const char * name) {
   return(GetDispatch(name));
}

void ReleaseEntityDLLFields(edict_t * e) {

   FreeEntPrivateData(e);
}
void InitEntityDLLFields(edict_t * e) {

   e->v.pContainingEntity = e;
}

/* DESCRIPTION: EntityInit
// LOCATION:
// PATH: LoadGameState, CreateEntityList
//
// Seems like an initting function.
*/
void EntityInit(edict_t * e, int string) {

   void (*function)(entvars_t *);


   if(string == 0) { Sys_Error("%s: Bad class.", __FUNCTION__); }

   ReleaseEntityDLLFields(e);
   InitEntityDLLFields(e);

   e->v.classname = string;
   function = (void (*)(entvars_t *))GetEntityInit(global_pr_strings + string);

   if(function != NULL) {

      function(&(e->v));
   }
}

/* DESCRIPTION: ED_ClearEdict
// LOCATION: pr_edict.c
// PATH: ED_Alloc
//
// When in doubt, zero it out.
*/
void ED_ClearEdict(edict_t *e) {

   Q_memset(&(e->v), 0, sizeof(entvars_t));
   e->free = 0;
   ReleaseEntityDLLFields(e);
   InitEntityDLLFields(e);
}

/* DESCRIPTION: ED_Alloc
// LOCATION: pr_edict.c
// PATH: lots
//
// Allocates a new edict.  And by 'allocates' I mean it picks one of
// the already allocated edicts.  QW's picking was much more involved,
// which makes sense as there are plenty of ways to screw with a mod
// here due to the memory reuse.
*/
edict_t * ED_Alloc() {

   unsigned int i;
   edict_t * e;

   for(i = global_svs.allocated_client_slots + 1; i < global_sv.num_edicts; i++) {

      e = &(global_sv.edicts[i]);

      if(e->free && (e->freetime < 2 || global_sv.time0c - e->freetime > 0.5 )) { //0.5 was originally zero, I trust QW to be smarter.

         ED_ClearEdict(e);
         return(e);
      }
   }

   if(i < global_sv.max_edicts) {

      global_sv.num_edicts++;
      e = &(global_sv.edicts[i]);
      ED_ClearEdict(e);
      return(e);
   }

   Sys_Error("%s: No edicts available.", __FUNCTION__);
}

//Unused, but hey, it was there.
void ED_Count() {

   unsigned int i;
   edict_t *ent;
   int active, models, solid, step;

   active = models = solid = step = 0;
   for(i = 0; i < global_sv.num_edicts; i++) {

      ent = &(global_sv.edicts[i]);
      if(ent->free) { continue; }
      active++;
      if(ent->v.solid != SOLID_NOT) { solid++; }
      if(ent->v.model) { models++; }
      if(ent->v.movetype == MOVETYPE_STEP) { step++; }
   }

   Con_Printf("num_edicts:%3i\n", global_sv.num_edicts);
   Con_Printf("active    :%3i\n", active);
   Con_Printf("view      :%3i\n", models);
   Con_Printf("touch     :%3i\n", solid);
   Con_Printf("step      :%3i\n", step);
}

/* DESCRIPTION: SuckOutClassname
// LOCATION:
// PATH: ED_ParseEdict
//
// leafs through the data lookign for a specific key/pair, and sends it off to
// that gEntityInterface thing.
*/
void SuckOutClassname(const char * data, edict_t * ent) {

   KeyValueData_t var_110_KeyValue;
   char buffer[256];

   while(1) {

      data = COM_Parse(data);
      if(global_com_token[0] == '}') { return; }

      Q_strncpy(buffer, global_com_token, sizeof(buffer)-1);
      buffer[sizeof(buffer)-1] = '\0';

      data = COM_Parse(data);

      if(Q_strcmp(buffer, "classname") == 0) {

         var_110_KeyValue.szClassName = NULL;
         var_110_KeyValue.szKeyName = buffer;
         var_110_KeyValue.szValue = global_com_token;
         var_110_KeyValue.fHandled = 0;

         gEntityInterface.pfnKeyValue(ent, &var_110_KeyValue);
         if(var_110_KeyValue.fHandled == 0) {

            Sys_Error("%s: Parse error.", __FUNCTION__);
         }
         return;
      }
   }
}

/* DESCRIPTION: ED_ParseEdict
// LOCATION: pr_edict.c
// PATH: ED_LoadFromFile
//
// To quote the text block that came with it:
// Parses an edict out of the given string, returning the new position
// ed should be a properly initialized empty edict.
// Used for initial level load and for savegames.
*/
const char * ED_ParseEdict(const char *data, edict_t *ent) {


   KeyValueData_t var_120_KeyValue;

   int init;
   char keyname[256];
   unsigned int var_108_keylen;
   double var_32c_angle;
   char * var_110_string;

   void (*function)(entvars_t *);

   // clear it
   if(ent != global_sv.edicts) { //listed as a hack, dunno why.
      memset(&(ent->v), 0, sizeof(entvars_t));
   }

   init = 0;
   InitEntityDLLFields(ent);
   SuckOutClassname(data, ent);
   var_110_string = global_pr_strings + ent->v.classname;
   function = (void (*)(entvars_t *))GetEntityInit(var_110_string);

   if(function == NULL) {

      var_110_string = "custom";
      function = (void (*)(entvars_t *))GetEntityInit(var_110_string);

      if(function == NULL) {

         Con_Printf("%s: Couldn't find an init function for some edict.\n", __FUNCTION__);
      }
      else {

         function(&(ent->v));
         var_120_KeyValue.szClassName = var_110_string;
         var_120_KeyValue.szKeyName = "customclass";
         var_120_KeyValue.szValue = global_pr_strings + ent->v.classname;
         var_120_KeyValue.fHandled = 0;

         gEntityInterface.pfnKeyValue(ent, &var_120_KeyValue);
         init = 1;
      }
   }
   else {

      function(&(ent->v));
      init = 1;
   }


   // go through all the dictionary pairs
   while(1) {

      // parse key
      data = COM_Parse(data);

      if(global_com_token[0] == '}') { break; }

      if(data == NULL) {
         Sys_Error("%s: EOF without closing brace.", __FUNCTION__);
      }

      //Q_strncpy(keyname, global_com_token, sizeof(keyname)-1);
      //keyname[sizeof(keyname)-1] = '\0';
      //var_108_keylen = strlen(keyname);
      var_108_keylen = Q_snprintf(keyname, sizeof(keyname), "%s", global_com_token);
      if(var_108_keylen >= sizeof(keyname)) {

         keyname[sizeof(keyname)-1] = '\0';
         var_108_keylen = sizeof(keyname)-1;
         Con_Printf("%s: truncating oversized keyname.", __FUNCTION__);
      }

      while(var_108_keylen > 0 && keyname[var_108_keylen-1] == ' ') {
         keyname[var_108_keylen-1] = '\0';
         var_108_keylen--;
      }

      // parse value
      data = COM_Parse(data);
      if(data == NULL) {
         Sys_Error("%s: EOF without closing brace.", __FUNCTION__);
      }
      if(global_com_token[0] == '}') {
         Sys_Error("%s: closing brace without data.", __FUNCTION__);
      }

      //Ignoring some Steam stuff that was here...

      if((global_pr_strings + ent->v.classname != NULL) &&
         (Q_strcmp(global_pr_strings + ent->v.classname, global_com_token) == 0)) { continue; }

      if(Q_strcmp(keyname, "angle") == 0) {

         var_32c_angle = atof(global_com_token);
         if(var_32c_angle >= 0) {

            Q_snprintf(global_com_token, 0x400, "%f %f %f", ent->v.angles[0], var_32c_angle, ent->v.angles[2]);
         }
         else if(var_32c_angle == -1) {
            Q_sprintf(global_com_token, "-90 0 0");
         }
         else {
            Q_sprintf(global_com_token, "90 0 0");
         }
         Q_strcpy(keyname, "angles");
      }

      var_120_KeyValue.szClassName = global_pr_strings + ent->v.classname;
      var_120_KeyValue.szKeyName = keyname;
      var_120_KeyValue.szValue = global_com_token;
      var_120_KeyValue.fHandled = 0;

      gEntityInterface.pfnKeyValue(ent, &var_120_KeyValue);

      //There are additional checks here, but none lead anywhere.
      //No doubt defined out in the dedicated server.
   }

   if(init == 0) {

      ent->free = 1;
      ent->serialnumber++;
   }

   return(data);
}

/* DESCRIPTION: ED_LoadFromFile
// LOCATION: pr_edict.c, but it's conspicuously absent in FTEQW.
// PATH: SV_LoadEntities
//
// To quote the text block that came with it:
// The entities are directly placed in the array, rather than allocated with
// ED_Alloc, because otherwise an error loading the map would have entity
// number references out of order.
//
// Creates a server's entity / program execution context by
// parsing textual entity definitions out of an ent file.
//
// Used for both fresh maps and savegame loads.  A fresh map would also need
// to call ED_CallSpawnFunctions () to let the objects initialize themselves.
*/
void ED_LoadFromFile(const char *data) {

   edict_t *ent;
   int inhibit;

   ent = NULL;
   inhibit = 0;
   gGlobalVariables.time = global_sv.time0c;

   //parse ents
   while(1) {

      // parse the opening brace
      data = COM_Parse(data);

      if(data == NULL) { break; }
      if(global_com_token[0] != '{') {
         Sys_Error ("%s: found %s when expecting {", __FUNCTION__, global_com_token);
      }

      if(ent == NULL) {
         ent = &(global_sv.edicts[0]);
         ReleaseEntityDLLFields(ent);
         InitEntityDLLFields(ent);
      }
      else { ent = ED_Alloc(); }

      data = ED_ParseEdict(data, ent);

      // remove things from different skill levels or deathmatch
      if(cvar_deathmatch.value != 0) {

         if((ent->v.spawnflags & 0x800) != 0) { //SPAWNFLAG_NOT_DEATHMATCH

            ED_Free(ent);
            inhibit++;
            continue;
         }
      }
      // immediately call spawn function
      if(ent->v.classname == 0) {

         Con_Printf("%s: Ent with no classname.\n", __FUNCTION__);
         ED_Free(ent);
         continue;
      }

      if(gEntityInterface.pfnSpawn(ent) < 0 || ent->v.flags & FL_KILLME) {

         ED_Free(ent);
      }
   }

   Con_Printf("%i entities inhibited\n", inhibit);
}

/* DESCRIPTION: ED_NewString
// LOCATION: pr_edict.c
// PATH: AllocEngineString
//
// To quote the text block that came with it:
// The entities are directly placed in the array, rather than allocated with
// ED_Alloc, because otherwise an error loading the map would have entity
// number references out of order.
//
// Creates a server's entity / program execution context by
// parsing textual entity definitions out of an ent file.
//
// Used for both fresh maps and savegame loads.  A fresh map would also need
// to call ED_CallSpawnFunctions () to let the objects initialize themselves.
*/
char * ED_NewString(const char * arg0_string) {

   char * newString1, * newString2;
   int i;
   int length;

   length = strlen(arg0_string) + 1;

   newString1 = newString2 = (char *)Hunk_Alloc(length);

   for(i = 0; i < length; i++) {

      if(arg0_string[i] == '\\' && i < length-1 && arg0_string[i+1] != '\0') {

         i++;
         if(arg0_string[i] == 'n') { *newString2 = '\n'; newString2++; }
         else { *newString2 = '\\'; newString2++; }
      }
      else {
         *newString2 = arg0_string[i];
         newString2++;
      }
   }

   return(newString1);
}

/* DESCRIPTION: ED_Free
// LOCATION: pr_edict.c
// PATH: lots
//
// Zeros out the edict.  I assume--but haven't yet found the init function--
// that edicts are on the hunk, and are reused.  QW indicates the possible need
// to go through and remove all possible traces to the now gone edict,
// which would probably help out EntMod, but that's not happening just yet.
*/
void ED_Free(edict_t *ed) {

   if(ed == NULL) {

      Con_Printf("%s: Passed NULL edict.\n", __FUNCTION__);
      return;
   }
   if(ed->free == 1) {

      Con_Printf("%s: Passed entity already 'free'.  Thought you should know.\n", __FUNCTION__);
   }

   SV_UnlinkEdict(ed);
   FreeEntPrivateData(ed);

   ed->free = 1;
   ed->serialnumber++;
   ed->v.flags = 0;

   ed->v.model = 0;
   ed->v.takedamage = 0;
   ed->v.modelindex = 0;

   ed->v.colormap = 0;
   ed->v.skin = 0;
   ed->v.frame = 0;
   ed->v.scale = 0;
   ed->v.gravity = 0;

   ed->v.origin[0] = 0;
   ed->v.origin[1] = 0;
   ed->v.origin[2] = 0;
   ed->v.angles[0] = 0;
   ed->v.angles[1] = 0;
   ed->v.angles[2] = 0;

   ed->v.nextthink = -1;
   ed->v.solid = SOLID_NOT;

   ed->freetime = global_sv.time0c;
}

int EntOffsetOfPEntity(const edict_t *pEdict) {

   return((byte *)pEdict - (byte *)global_sv.edicts);
}


HLDS_DLLEXPORT edict_t* PEntityOfEntOffset(int iEntOffset) {
   return(((edict_t *)(((byte *)global_sv.edicts) + iEntOffset)));
}
HLDS_DLLEXPORT const char * SzFromIndex(int iString) {
   return((const char *)(global_pr_strings + iString));
}
HLDS_DLLEXPORT struct entvars_s * GetVarsOfEnt(edict_t *pEdict) {
   return(&(pEdict->v));
}
