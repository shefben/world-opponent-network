#include <ctype.h>

#include "cvar.h"
#include "cmd.h"
#include "common.h"
#include "NET.h"
#include "HUNK.h"
#include "SV_mcast.h"
#include "report.h"
#include "Q_memdef.h"

#include "libcprops-0.1.8/avl.h"

/*** GLOBALS ***/
static cp_avltree * cvar_avl_tree = NULL;


/*** AVL functions ***/



/*** FUNCTIONS ***/

//decs
void Cvar_CvarList_f();

void Cvar_CmdInit() {
   Cmd_AddCommand("cvarlist", Cvar_CvarList_f);
}

void Cvar_Init() { //Nothing needed at this time

   #if(HLDS_PARANOIA > 0)
      if(cvar_avl_tree != NULL) {
         Sys_Error("%s: Init called, but binary tree already exists.\n", __FUNCTION__);
      }
   #endif

   cvar_avl_tree = cp_avltree_create((cp_compare_fn)Q_strcmp_ptr); //Lesson learned on macros with arguments...
   if(cvar_avl_tree == NULL) {
      Sys_Error("%s: Memory failure allocating new binary tree.\n", __FUNCTION__);
   }

   Cvar_CmdInit();
}
void Cvar_Shutdown() {

   #if(HLDS_PARANOIA > 0)
      if(cvar_avl_tree == NULL) {
         Sys_Error("%s: Shutdown called, but binary tree is not allocated.\n", __FUNCTION__);
      }
   #endif
   cp_avltree_destroy(cvar_avl_tree);
   cvar_avl_tree = NULL;
}


/* DESCRIPTION: Cvar_FindVar
// LOCATION: cvar.c
// PATH: Mostly cvar functions
//
// Finds and returns a variable named *var_name*.  Or NULL of not found.
// Unfortunately,
*/
cvar_t * Cvar_FindVar(const char *var_name) {



   if(cvar_avl_tree == NULL) { return(NULL); } //We sometimes search for vars before initting cvar.
   return((cvar_t *)cp_avltree_get(cvar_avl_tree, var_name)); //safe, but I'd like to alter this anyway
}

#if 0
/* DESCRIPTION: Cvar_FindPrevVar
// LOCATION:
// PATH: CompleteVariable
//
// Appears to do the same thing FindVar does, except it returns the preceding
// cvar structure instead of the matching one.
*/
cvar_t * Cvar_FindPrevVar(const char *var_name) {

   cvar_t *var;

   if(cvar_vars == NULL) { return(NULL); }
   if(Q_strcasecmp(var_name, cvar_vars->name) == 0) {
      //First one.  We cannot return a 'previous' var...
      return(NULL);
   }

   for(var = cvar_vars; var->next != NULL; var = var->next) {

      if(Q_strcasecmp(var_name, var->next->name) == 0) {
         return(var);
      }
   }
   return(NULL);
}
#endif
/* DESCRIPTION: Cvar_DirectSet
// LOCATION: cvar.c as cvar_core
// PATH: cvar_setvalue and more
//
// Assign a cvar a given value.  This is the bit that actually does the assign.
//
// A lot of odd things can happen depending on the flags the cvar struct has
// set, but they're all intertwined with the game logic--so I don't know what
// any of them do.
//
// Once the flag checks are done memory is zmallocd and zfreed to assign it.
*/
void Cvar_DirectSet(cvar_t *var, const char *value) {

   unsigned int i;
   int IsDiff;
   const char * ptr, * ptr2;

   char tempbuffer[1024];
   char * bufferptr;


   if(var == NULL || value == NULL) {
      Con_Printf("%s: passed a NULL argument (%X, %X)", __FUNCTION__, var, value);
      return;
   }

   ptr = value;

   if(var->flags & FCVAR_PRINTABLEONLY) {

      tempbuffer[0] = '\0';
      bufferptr = tempbuffer;
      i = 0;

      for(ptr2 = ptr; *ptr2 != '\0'; ptr++) {

         if(isprint(*ptr2)) {

            if(i >= sizeof(tempbuffer)-1) { break; } //overflow protection.
            i++;

            *bufferptr = *ptr2;
            bufferptr++;
         }
      }

      //we have now copied over our string.  Let's cap it.
      *bufferptr = '\0';
      if(tempbuffer[0] == '\0') {

         //if an empty string, fill it with the following:
         Q_strcpy(tempbuffer, "empty");
      }
      ptr = tempbuffer;
   }

   // var_8 either holds 'value' or var_408, which had 'value'
   // minus unprintables copied over.  Or "empty".
   IsDiff = Q_strcmp(var->string, ptr);

   if(var->flags & FCVAR_USERINFO) {

      if(global_cls.state == ca_dedicated) { //we are, aren't we?

         Info_SetValueForKey(Info_Serverinfo(), var->name, ptr, 0x200);
         SV_BroadcastCommand("fullserverinfo \"%s\"\n", Info_Serverinfo());
      }

      if(global_cls.state != ca_dedicated) {

         Info_SetValueForKey(global_cls.userinfo, var->name, ptr, 0x100);

         if(IsDiff != 0 && global_cls.state > ca_connecting) {

            //wholly innapropriate and guessage as to what the heck this is.
            MSG_WriteByte_C(&global_cls.netchan.netchan_message, 0x03);
            SZ_Print(&global_cls.netchan.netchan_message, va("setinfo \"%s\" \"%s\"\n", var->name, ptr));
         }
      }
   }
   //whatever that is is sort've done...

   if(var->flags & FCVAR_SERVER) {
      if(IsDiff != 0 && ((var->flags & FCVAR_UNLOGGED) == 0)) {
         if(var->flags & FCVAR_PROTECTED) {
            SV_BroadcastPrintf("%s changed to %s", var->name, "***PROTECTED***");
         }
         else {
            SV_BroadcastPrintf("%s changed to %s", var->name, ptr);
         }
      }
   }

   Z_Free(var->string);
   var->string = (char *)Z_Malloc(Q_strlen(ptr)+1); //var4 should still hold this.  Eh.
   Q_strcpy(var->string, ptr);
   var->value = atof(ptr);
}
void Cvar_DirectSetValue(cvar_t *var, float value) {

   unsigned int i;
   char * value_string;


   if(var == NULL) {
      Con_Printf("%s: passed a NULL argument (%X)", __FUNCTION__, var);
      return;
   }

   //Get the string equivalent of our float.
   //This is perhaps a little cheap, but I don't really know
   //the float min/maxs, and I can't think of an easy way to manually

   //find the length.
   i = 1 + Q_snprintf(NULL, 0, "%g", value);

   //Allocate room for the string.
   value_string = (char *)Z_Malloc(i);

   //Quick error checking.  If we were given a NULL, don't change things.
   if(value_string == NULL) {
      Con_Printf("%s: Could not change cvar \"%s\".  Z_Malloc failure.\n", __FUNCTION__, var->name);
      return;
   }

   //We have a buffer.  Make the string.
   Q_sprintf(value_string, "%g", value);


   //If the cvar is related to us and our serverinfo thing, update that.
   if(var->flags & FCVAR_USERINFO) {

      if(global_cls.state == ca_dedicated) { //we are

         Info_SetValueForKey(Info_Serverinfo(), var->name, value_string, 0x200);
         SV_BroadcastCommand("fullserverinfo \"%s\"\n", Info_Serverinfo());
      }
      else {

         Info_SetValueForKey(global_cls.userinfo, var->name, value_string, 0x100);

         if(value != var->value && global_cls.state > ca_connecting) {

            MSG_WriteByte_C(&global_cls.netchan.netchan_message, 0x03);
            SZ_Print(&global_cls.netchan.netchan_message, va("setinfo \"%s\" \"%s\"\n", var->name, value_string));
         }
      }
   }

   //Server vars should be broadcast to people.  If changed.
   if(var->flags & FCVAR_SERVER) {
      if(value != var->value && ((var->flags & FCVAR_UNLOGGED) == 0)) {
         if(var->flags & FCVAR_PROTECTED) {
            SV_BroadcastPrintf("%s changed to %s", var->name, "***PROTECTED***");
         }
         else {
            SV_BroadcastPrintf("%s changed to %s", var->name, value_string);
         }
      }
   }

   //Now assign the cvar its new values.
   Z_Free(var->string);
   var->string = value_string;
   var->value = value;
}

/* DESCRIPTION: Cvar_Set
// LOCATION: cvar.c
// PATH: cvar_setvalue and more
//
// Assign a cvar a given value.  Really just a wrapper.
*/
void Cvar_Set(const char *var, const char *value) {

   cvar_t * set;

   set = Cvar_FindVar(var);

   if(set == NULL) {
      Con_Printf("%s: Couldn't change var %s because it doesn't exist.\n", __FUNCTION__, var);
   }
   else {
      Cvar_DirectSet(set, value);
   }
}

/* DESCRIPTION: Cvar_SetValue
// LOCATION: cvar.c
// PATH: CompleteVariable
//
// Assign a cvar a given value.  cvars store values as strings, this function
// takes numbers.  Therefore we do some converting.
*/
void Cvar_SetValue(const char *var, float value) {

   cvar_t * set;

   set = Cvar_FindVar(var);

   if(set == NULL) {
      Con_Printf("%s: Couldn't change var %s because it doesn't exist.\n", __FUNCTION__, var);
   }
   else {
      Cvar_DirectSetValue(set, value);
   }
}

/* DESCRIPTION: Cvar_VariableX
// LOCATION: cvar.c
// PATH: A few places, the int version seems unused
//
// Finds a cvar of 'name' and retrieves its value.
// For some reason the 'value' argument that 'Cvar_DirectSet' assigns
// is unused, and instead the string-to-whatever functions are used
// in the cases where an actual integer/float is requested.
*/
int Cvar_VariableInt(const char *var_name) {

   cvar_t *var;

   var = Cvar_FindVar(var_name);
   if(var == NULL) {
      Con_DPrintf("%s: Cvar %s not found.\n", __FUNCTION__, var_name);
      return(0);
   }

   return(Q_atoi(var->string));
}
float Cvar_VariableValue(const char *var_name) {

   cvar_t *var;

   var = Cvar_FindVar(var_name);
   if(var == NULL) {
      Con_DPrintf("%s: Cvar %s not found.\n", __FUNCTION__, var_name);
      return(0);
   }

   return(Q_atof(var->string));
}
const char * Cvar_VariableString(const char *var_name) {

   cvar_t *var;

   var = Cvar_FindVar(var_name);
   if(var == NULL) {
      Con_DPrintf("%s: Cvar %s not found.\n", __FUNCTION__, var_name);
      return("");
   }

   return(var->string);
}



/* DESCRIPTION: Cvar_RemoveHudCvars
// LOCATION:
// PATH: none
//
// does a little variable freeing depending on the type.
*/
int  Cvar_RemoveHudCvarsCallBackFunction(cp_avlnode * node, void * none) {

   cvar_t * ptr;

   // This function has to conform to the callback's requests.  We return !0 if
   // we want to terminate early.

   //Get the cvar.
   ptr = (cvar_t *)node->value;

   //Do we need to terminate the cvar?

   if(ptr->flags & FCVAR_CLIENTDLL) {

      cp_avltree_destroy_node(cvar_avl_tree, node);
      Z_Free(ptr->string); //You must wait until after freeing the node to free the string, since it is used as the key as well
      Z_Free(ptr);
   }

   return(0);
}
void Cvar_RemoveHudCvars() {
   cp_avltree_callback(cvar_avl_tree, (cp_callback_fn)Cvar_RemoveHudCvarsCallBackFunction, NULL);
}

/* DESCRIPTION: Cvar_UnlinkExternals
// LOCATION:
// PATH: startup
//
// The below is an (almost) exact match with gcc with i386 as the instruction set.
// I can see what it's trying to do just fine; it's doing the cvar equivalent of
// CmdRemoveMallocdCommands().  But it's messed up.  Is this a bug?  A glitch
// in the 386 binary?  A misunderstanding of the cvar struct?
//
// Well it only runs on shutdown and doesn't seem to be breaking things, but
// just the same from here on I am changing it.
*/
int  Cvar_UnlinkExternalsCallBackFunction(cp_avlnode * node, void * none) {

   cvar_t * ptr;

   // This function has to conform to the callback's requests.  We return !0 if
   // we want to terminate early.

   //Get the cvar.
   ptr = (cvar_t *)node->value;

   //Do we need to terminate the cvar?

   if(ptr->flags & FCVAR_EXTDLL) {

      //We don't free these.  I figure we SHOULD free the string,
      //but it's not in the ASM.  Still worth checking...
      cp_avltree_destroy_node(cvar_avl_tree, node);
   }

   return(0);
}
void Cvar_UnlinkExternals() {
   cp_avltree_callback(cvar_avl_tree, (cp_callback_fn)Cvar_RemoveHudCvarsCallBackFunction, NULL);
}

/* DESCRIPTION: Cvar_WriteVariables
// LOCATION: cvar.c
// PATH: none known
//
// Seems to log the values of variables to a log file.
// Probably commented out normally.
*/
int  Cvar_WriteVariablesCallBackFunction(cp_avlnode * node, hl_file_t * file) {

   cvar_t * ptr;

   //Get the cvar.
   ptr = (cvar_t *)node->value;
   FS_FPrintf(file, "%s \"%s\"\n", ptr->name, ptr->string);

   return(0);
}
void Cvar_WriteVariables(hl_file_t *file) {
   cp_avltree_callback(cvar_avl_tree, (cp_callback_fn)Cvar_WriteVariablesCallBackFunction, file);
}

/* DESCRIPTION: Cvar_RegisterVariable
// LOCATION: cvar.c
// PATH: Lots and lots
//
// Adds a cvar to our cvar list.  I'm assuming at least that:
//  Flags are taken care of elsewhere (we don't touch them)
//  Nothing bad will happen to us by ignoring flags (by not using directset)
//  Putting them in alphabetical order serves some purpose
//
// We take the variable, see if the name is taken, then re-assign the 'string'
// value to z_malloc'd memory (I assume because we can't trust the programmer to
// give us writable memory).  Then we throw it in our cvar_vars list.  Sorted.
*/
void Cvar_RegisterVariable(cvar_t *variable) {

   char * temp;

   #if(HLDS_PARANOIA > 0)
      if(cvar_avl_tree == NULL) {
         Sys_Error("%s: Attempted to add variable before calling Cvar_Init().\n", __FUNCTION__);
      }
   #endif

   //If it already exists, abort.
   if(Cvar_FindVar(variable->name) != NULL) {
      Con_Printf("%s: Can't register the cvar \"%s\".  It already exists.\n", __FUNCTION__, variable->name);
      return;
   }

   //Same goes for like named commands.
   if(Cmd_Exists(variable->name) != 0) {
      Con_Printf("%s: \"%s\" is a command; it can't be a cvar too.  Registration failed.\n", __FUNCTION__, variable->name);
      return;
   }

   //The cvar we have now is incomplete.  The cvar->string value MUST BE
   //in malloced memory, and the number fields are all null.
   //We can take care of that by calling directset.  The problem with that is
   //it will try to free the previous string.  Until I've sorted
   //it all out, we just copy paste.
   // Cvar_DirectSet(variable, variable->string);
   temp = variable->string;
   variable->string = (char *)Z_Malloc(Q_strlen(temp)+1); //var4 should still hold this.  Eh.
   Q_strcpy(variable->string, temp);
   variable->value = atof(temp);

   //Now we're ready.  Insert the cvar into our avl tree.

   if(cp_avltree_insert(cvar_avl_tree, variable->name, variable) == NULL)  {
      Con_Printf("%s: Inserting \"%s\" into list failed, probably memory related.  This will likely fubar the mod.\n", __FUNCTION__, variable->name);
      Z_Free(variable->string);
      return;
   }
}

/* DESCRIPTION: Cvar_CompleteVariable
// LOCATION: cvar.c
// PATH: none known
//
// Takes a string and tries to find a variable that matches it.
// exact matches take priority.  Odd static variable use.
//
// The code below appears to replicate this function, however due to the NATURE
// of said code I'm pretty certain this function is incomplete.  I can sort've
// see what it's doing though.  At the moment it aborts so that I can test my
// 'this never runs' theory.
*/
void Cvar_CompleteVariable() {
   EXIT();
}
//Not called, at least not regulary.  Below relies on older LL structure.
#if 0
char * Cvar_CompleteVariable(char *partial, int con) {

   char * var10c;
   unsigned int var108;
   char var100[0x100];
   cvar_t * var104;

   static char lastpartial[0x100] = "";


   printf("Cvar_CompleteVariable called");
   exit(99);

   Q_strncpy(var100, partial, 0xFF);
   var100[0x100] = '\0';

   var108 = strlen(var100);
   if(var108 == 0) { return(NULL); }

   //remove excess whitespace
   while(var100[var108-1] == ' ') {
      var108--;
      var100[var108] = '\0';
   }

   if(Q_strcasecmp(lastpartial, var100) == 0) {
      //match
      var104 = Cvar_FindVar(var100);
      if(var104 != NULL) {
         if(con == 1) {
            var104 = var104->next;
         }
         else {
            var104 = Cvar_FindPrevVar(var100);
         }
         if(var104 != NULL) {

            Q_strncpy(lastpartial, var104->name, 0xFF);
            lastpartial[100] = '\0';
            return(var104->name);
         }
      }
   }

   var104 = cvar_vars;
   while(var104 != NULL) {

      if(strncmp(var100, var104->name, var108) == 0) {
         if(strlen(var104->name) == var108) {
            //faster way to determine if regular strcmp == 0
            Q_strncpy(lastpartial, var104->name, 0xFF);
            lastpartial[100] = '\0';
            return(var104->name);
         }
         else {
            var10c = var104->name;
            break;
         }
      }
      var104 = var104->next;
   }
   if(var104 != NULL) {
      Q_strncpy(lastpartial, var104->name, 0xFF);
      lastpartial[100] = '\0';
      return(var10c);
   }
*/
}
#endif

/* DESCRIPTION: Cvar_IsMultipleTokens
// LOCATION: cvar.c
// PATH: cvar_command
//
// You'd think it'd return TRUE if there were multiple tokens.  You'd be wrong.
//
// It's an odd one, but I *think* it returns the first token in the static var
// 'firstToken' unless there is only one token.  Then it returns null.
*/
char * Cvar_IsMultipleTokens(char * arg0) {

   static char firstToken[0x204];
   int var8 = 0;
   const char * varc = arg0;

   firstToken[0] = '\0';

   while(1) {
      varc = COM_Parse(varc);

      if(Q_strlen(global_com_token) > 0) {
         if(var8 == 0) {
            Q_strncpy(firstToken, global_com_token, 0x203);
            firstToken[0x204] = '\0';
         }
         var8++;
      }
      else {
         //for some reason there's an int->char conversion.  ignored.
         if(var8 != 1) { return(firstToken); }
         else { return(NULL); }
      }
   }
}

/* DESCRIPTION: Cvar_Command
// LOCATION: cvar.c
// PATH: The console (through executeString)
//
// Sets or prints a variable named by the console.
*/
qboolean Cvar_Command() {

   cvar_t *v;


   v = Cvar_FindVar(Cmd_Argv(0));
   if(v == NULL) { return(0); }

   if(Cmd_Argc() == 1) {
      Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
      return(1);
   }

   if(v->flags & FCVAR_SPONLY && global_cls.state != ca_dedicated && global_cls.state != ca_disconnected) { //possibly CVAR_NOSET in code or CVAR_USERCREATED in .h
      //another switch was if byte_13AA20[1A34E0] > 1
      Con_Printf("Cvar %s can't be set in multiplayer\n", v->name);
      return(1);
   }

   Cvar_DirectSet(v, Cmd_Argv(1));
   return(1);
}

/* DESCRIPTION: Cvar_CountServerVariables
// LOCATION:
// PATH: SVC_RuleInfo->Cvar_CountServerVariables
//
// This is very simple; it just totals the number of variables flagged.
*/
int  Cvar_CountServerVariablesCallBackFunction(cp_avlnode * node, unsigned int * count) {

   cvar_t * ptr;

   //Get the cvar.
   ptr = (cvar_t *)node->value;
   if(ptr->flags & FCVAR_SERVER) { (*count)++; }

   return(0);
}
unsigned int Cvar_CountServerVariables() {

   unsigned int count = 0;

   cp_avltree_callback(cvar_avl_tree, (cp_callback_fn)Cvar_CountServerVariablesCallBackFunction, &count);
   return(count);
}


//other
/* DESCRIPTION: Cmd_CvarList_f
// LOCATION: cmd.c
// PATH: When "cvarlist" is entered into the console
//
// Lists all the registered cvars.  They should be alphabetized.  Like the cmd
// equivalent, filtering, file IO, and other nonsense will have to wait until
// I care.
*/
int  Cvar_CvarListCallBackFunction(cp_avlnode * node, unsigned int * matches) {

   cvar_t * ptr;

   //Get the cvar.
   ptr = (cvar_t *)node->value;

   Con_Printf("%s = \"%s\".  Flags are 0x%X\n", ptr->name, ptr->string, ptr->flags);
   (*matches)++;

   return(0);
}
void Cvar_CvarList_f() {

   unsigned int matches;


   if(Cmd_Argc() > 1) {

      //This can be as simple as spitting everything out
      //or as hard as opening and writing to a file.
      if(Q_strcmp("?", Cmd_Argv(1)) == 0) {

         Con_Printf("Cvarlist           : List all commands\nCvarlist [Partial] : List commands starting with 'Partial'\nCvarlist log [Partial] : Logs commands to file \"cmdlist.txt\" in the gamedir.\n");
         return;
      }
      else if(Q_strcasecmp("log", Cmd_Argv(1)) == 0) {

         Con_Printf("File redirection will be added in later.  Like after the engine is finished.\n");
         return;
      }
      else {
         Con_Printf("Actually adding in the [partial] stuff is currently a low priority.  Google it, lazy.\n");
      }
   }

   matches = 0;
   Con_Printf("\"Cvars:\"\n");
   cp_avltree_callback(cvar_avl_tree, (cp_callback_fn)Cvar_CvarListCallBackFunction, &matches);
   Con_Printf("\n--%u matching vars found.\n", matches);
}

void Log_PrintServerVars() {
   Con_Printf("No logging of server vars.  Too embedded, absolutely no effect on the engine...\n");
}

//self made in order to keep SV and CVAR globals separate.
//Returns total number of bytes written.
struct temp_rule_struct_s {

   char * buffer;
   unsigned int cursize;
   unsigned int maxsize;
   unsigned short int varcount;
};
int  Cvar_SVCRuleInfoVarWriteCallBackFunction(cp_avlnode * node, struct temp_rule_struct_s * packet_details) {

   cvar_t * ptr;
   int sizeofname;
   int sizeofvalue;


   //Get the cvar.
   ptr = (cvar_t *)node->value;

   if(ptr->flags & FCVAR_SERVER) {

      packet_details->varcount++;

      sizeofname = strlen(ptr->name) +1;
      if(ptr->flags & FCVAR_PROTECTED) { sizeofvalue = 2; }
      else { sizeofvalue = strlen(ptr->string) +1; }

      //Chicka Chicka Boom Boom, Will there be enough room?
      if(packet_details->cursize + sizeofname + sizeofvalue > packet_details->maxsize) { return(1); }

      Q_memcpy(packet_details->buffer + packet_details->cursize, ptr->name, sizeofname);
      packet_details->cursize += sizeofname;

      if(ptr->flags & FCVAR_PROTECTED) { //restructure program or repeat comparison...
         if(ptr->string[0] == '\0' || Q_strcasecmp(ptr->string, "none") == 0) {
            Q_memcpy(packet_details->buffer + packet_details->cursize, "0", 2);
         }
         else {
            Q_memcpy(packet_details->buffer + packet_details->cursize, "1", 2);
         }
         packet_details->cursize += 2;
      }
      else {
         Q_memcpy(packet_details->buffer + packet_details->cursize, ptr->string, sizeofvalue);
         packet_details->cursize += sizeofvalue;
      }
   }

   return(0);
}
int Cvar_SVCRuleInfoVarWrite(char * buffer, unsigned int size) {

   struct temp_rule_struct_s packet_details;


   if(size < 7) { return(0); } //the bare minimum of bare minimums.

   *(uint32*)(buffer) = 0xFFFFFFFF; //[0-3]
   buffer[4] = 'E';
   //We'll have to write the short int 'servervarcount' when we finish.

   packet_details.buffer = buffer;
   packet_details.cursize = 7;
   packet_details.maxsize = size;
   packet_details.varcount = 0;

   cp_avltree_callback(cvar_avl_tree, (cp_callback_fn)Cvar_SVCRuleInfoVarWriteCallBackFunction, &packet_details);

   *(uint16*)(buffer+5) = hosttole16(packet_details.varcount);

   return(0);
}


//CVar wrappers
HLDS_DLLEXPORT float CVarGetFloat(const char *var_name) { return(Cvar_VariableValue(var_name)); }
HLDS_DLLEXPORT cvar_t * CVarGetPointer(const char *var_name) { return(Cvar_FindVar(var_name)); }
HLDS_DLLEXPORT const char * CVarGetString(const char *var_name) { return(Cvar_VariableString(var_name)); }
HLDS_DLLEXPORT void CVarRegister(cvar_t *var) {
   if(var != NULL) {
      var->flags |= FCVAR_EXTDLL;
      Cvar_RegisterVariable(var);
   }
}
HLDS_DLLEXPORT void CVarSetFloat(const char *var, float value) { Cvar_SetValue(var, value); }
HLDS_DLLEXPORT void CVarSetString(const char *var, const char *value) { Cvar_Set(var, value); }

