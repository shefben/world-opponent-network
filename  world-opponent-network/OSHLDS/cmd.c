#include <ctype.h>


#include "cmd.h"
#include "cvar.h"
#include "Q_memdef.h"
#include "report.h"
#include "common.h"
#include "HUNK.h"
#include "NET.h"


/*** GLOBAL GLOBAL ***/
cmd_source_t global_cmd_source;

/*** GLOBALS ***/
static sizebuf_t cmd_text;
static int cmd_wait = 0;


static cmd_function_t *cmd_functions = NULL;
static cmdalias_t * cmd_alias = NULL;


/*** FUNCTIONS ***/

/* DESCRIPTION: Cmd_AddCommand
// LOCATION: cmd.c
// PATH: AddCommand is in many places, AddMallocCommand is SDK
//
// Add commands to our linked list.
//
// They have odd behaviors in my humble opinion.  AddCommand places
// them alphabetically, and insists on using memory from the heap to store
// everything.  The SDK version(s) don't care about order, uses malloc'd memory,
// and may be relying on the mod writer to free it.  Methinks it would probably
// be worth merging the two so that they behave similarly, but that's touchup.
*/
void Cmd_AddCommand(const char * cmd_name, xcommand_t function) {

   cmd_function_t *cmd, *TempPtr1, *TempPtr2;

/* not worth hijacking, re-enable later
   if(host_initialized) { //because QW says so
      Sys_Error ("Cmd_AddCommand after host_initialized\n");
   }
*/

   // fail if the command is a variable name.  This works because the
   // cvar function returns an empty string (""), and array element 0
   // for an empty string is a null.  Todo: make a more appropriate 'exists' function.
   if(*(Cvar_VariableString(cmd_name)) != '\0')   {
      Con_Printf("Cmd_AddCommand: %s already defined as a var\n", cmd_name);
   }

   // fail if the command already exists.  A more appropriate 'exists'
   // function is present, but for now the error reporting is moer fun this way.
   for(cmd = cmd_functions; cmd != NULL; cmd = cmd->next) {
      if(Q_strcmp(cmd_name, cmd->name) == 0) { //matching console command

         if(cmd->function == function) { //for our error message, difference between trying to remake and trying to override.
            Con_Printf("Cmd_AddCommand: %s has already been made\n", cmd_name);
         }
         else {
            Con_Printf("Cmd_AddCommand: %s is mapped to something else already\n", cmd_name);
         }
         return;
      }
   }


   cmd = (cmd_function_t*)Hunk_AllocName(sizeof(cmd_function_t), cmd_name);
   cmd->name = cmd_name;
   cmd->function = function;
   cmd->restriction = 0;

   //Now we add our structure to our magical list.  Our magic alphabetical list.


   //We're ready.
   //Now what we want to do is place it in our linked list.
   //We insert it so that they're in alphabetical order.  This was messy before.

   //Special case 1:  Nothing in the list.
   if(cmd_functions == NULL) {

      cmd_functions = cmd;
      cmd_functions->next = NULL;
      return;
   }

   //Special Case 2:  We're Number One!
   if(Q_strcasecmp(cmd_name, cmd_functions->name) < 0) {

      TempPtr1 = cmd_functions;
      cmd_functions = cmd;
      cmd_functions->next = TempPtr1;
      return;
   }

   //Now time to search through our list.
   TempPtr1 = cmd_functions;
   TempPtr2 = cmd_functions->next;

   while(TempPtr2 != NULL) {

      if(Q_strcasecmp(cmd_name, TempPtr2->name) < 0) {

         //We now know that 'variable' needs to be linked ahead of 'tempptr2'
         cmd->next = TempPtr2;
         TempPtr1->next = cmd;
         return;
      }
      //or alternatively we need to link it in somewhere AFTER 'tempptr2'
      TempPtr1 = TempPtr2;
      TempPtr2 = TempPtr2->next;

   }
   //Special Case 3: End of the line.

   TempPtr1->next = cmd;
   cmd->next = NULL;
}
void Cmd_AddMallocCommand(const char * cmd_name, xcommand_t function, int type) {

   cmd_function_t *cmd;

   if(*(Cvar_VariableString(cmd_name)) != '\0')   {
      Con_Printf("Cmd_AddMallocCommand: %s already defined as a var\n", cmd_name);
   }

   for(cmd = cmd_functions; cmd != NULL; cmd = cmd->next) {
      if(Q_strcmp(cmd_name, cmd->name) == 0) { //matching console command

         if(cmd->function == function) { //for our error message, difference between trying to remake and trying to override.
            Con_Printf("Cmd_AddMallocCommand: %s has already been made\n", cmd_name);
         }
         else {
            Con_Printf("Cmd_AddMallocCommand: %s is mapped to something else already\n", cmd_name);
         }
         return;
      }
   }

   cmd = Q_Malloc(sizeof(cmd_function_t));
   if(cmd == NULL) { //I guess we should throw an error or something.
      Sys_Error("Sys_InitMemory: Went in for %u bytes of memory--Denied\n", host_parms.memsize);
   }

   //Q_memset(cmd, 0, sizeof(cmd_function_t)); no need; we change all four values.

   cmd->name = cmd_name;
   cmd->function = function;
   cmd->restriction = type; //Still don't really know what this does.  Might just be there to identify a variable as being a mod's var.

   //I notice we're not adding these alphabetically...
   cmd->next = cmd_functions;
   cmd_functions = cmd;
}
HLDS_DLLEXPORT void Cmd_AddGameCommand(char * cmd_name, xcommand_t function) { Cmd_AddMallocCommand(cmd_name, function, 2); }
void Cmd_AddHUDCommand(char * cmd_name, xcommand_t function) { Cmd_AddMallocCommand(cmd_name, function, 1); }
void Cmd_AddWrapperCommand(char * cmd_name, xcommand_t function) { Cmd_AddMallocCommand(cmd_name, function, 4); }

/* DESCRIPTION: RemoveMallocedCmds
// LOCATION: cmd.c
// PATH: SDK
//
// Remove (free) commands that are malloc'd by our mod.  They also mess up
// the order of our functions, but since it's on termination I guess it
// doesn't matter.
*/
void Cmd_RemoveMallocedCmds(int type) {

   cmd_function_t *cmd, *TempPtr1, *TempPtr2;

   cmd = NULL;
   TempPtr1 = cmd_functions;

   while(TempPtr1 != NULL) {

      TempPtr2 = TempPtr1->next;
      if(TempPtr1->restriction & type) {
         Q_Free(TempPtr1);
      }
      else {
         TempPtr1->next = cmd;
         cmd = TempPtr1;
      }
      TempPtr1 = TempPtr2;
   }

   cmd_functions = cmd;
}
void Cmd_RemoveGameCmds() { Cmd_RemoveMallocedCmds(2); }
void Cmd_RemoveHUDCmds() { Cmd_RemoveMallocedCmds(1); }
void Cmd_RemoveWrapperCmds() { Cmd_RemoveMallocedCmds(4); }


/* DESCRIPTION: Cmd_StuffCmds
// LOCATION: cmd.c
// PATH: apparently when 'stuffcmds' is entered, though its usage would indicate
// it is called when parsing the commandline.
//
// Remember how commandline arguments can be '-' or '+'?  Why the '+'?
// Arguments beginning with + are parsed by this command--they're handled like
// console commands.
//
// The code below works, though it's a bit washy and wasteful with regard to
// spaces (hey, I wrote it before I knew what it did and what it'd be parsing,
// so cut me some slack).  Since it works, SHIP IT!
*/
void Cmd_StuffCmds_f() {

   unsigned int argvLength;
   int i, j;

   char * ZMallocdString, * build;
   char c;

   //I already know from the COMMON functions that at some point in execution
   //the arguments exist all as one string.  But I haven't gone back far enough
   //to find out where or why.  Having the arguments present in both forms
   //sounds like something to do once I know how they all work.

   //For now we'll have to temporarially do that ourselves...

   argvLength = 0;

   for(i=1; i < global_com_argc; i++) {

      if(global_com_argv[i] == NULL) { continue; }
      argvLength += Q_strlen(global_com_argv[i]) + 3; //+1 makes sense; replace the null with a space.  Why +3?
   }

   if(argvLength == 0) { return; }

   //argvLength now stores the size all the arguments will take up...

   ZMallocdString = (char *)Z_Malloc(argvLength+1); //Why +1?
   ZMallocdString[0] = '\0';

   //rinse repeat, this time copying strings.  memcpy should be faster
   //but strcat for now gets the job done.

   for(i=1; i < global_com_argc; i++) {

      if(global_com_argv[i] == NULL) { continue; } //why would argv have a null string?

      Q_strcat(ZMallocdString, global_com_argv[i]);
      if(i != global_com_argc - 1) {
         Q_strcat(ZMallocdString, " ");
      }
   }

   // Now we convert our commandline into something we can pass off to Cbuf_Addtext

   build = (char *)Z_Malloc(argvLength+1);
   build[0] = '\0';

   for(i = 0; (unsigned)i < argvLength-1; i++) {

      if(ZMallocdString[i] == '+') { //copy this data over
         i++;

         //Should we keep quotes in mind?
         for (j=i ; (ZMallocdString[j] != '+') && (ZMallocdString[j] != '-') && (ZMallocdString[j] != '\0'); j++) { }

         //At this point i points to the beginning of our segment, j to the end.
         //QW still strcats, but it's no longer that easy.
         c = ZMallocdString[j];
         ZMallocdString[j] = '\0';

         if(build[0] != '\0') { Q_strcat(build, "\n"); } //putting returns ahead of all but the first parameter.
         Q_strcat (build, ZMallocdString+i);

         ZMallocdString[j] = c;
         i = j-1;
      }
   }

   if(build[0] != '\0') { Cbuf_AddText(build); }

   Z_Free(ZMallocdString);
   Z_Free(build);
}
/* DESCRIPTION: Cmd_Exec_f
// LOCATION: cmd.c
// PATH: When "exec" is entered into the console
//
// Loads a configuration file and adds all the commands within to the
// command buffer.  More important than it sounds, that.
*/
void Cmd_Exec_f() {

   const char *f;
   char * fp;
   char extension[10];

   if(Cmd_Argc() != 2) {
      Con_Printf("exec <filename> : execute a script file\"\n");
      return;
   }

   f = Cmd_Argv(1); //<filename>

   /*If our server op is a dumbass and running the server as root, this fuction
   //could be used to open and potentially reveal the contents of passwd and
   //similar functions.  Technically that's not *our* fault, but I suppose
   //we could do something to at least simulate a chroot jail and make it harder
   //for an attacker to use.  They are probably not perfect, but at the same
   //time they are NOT the actual problem.
   */

   if(strstr(f, "..") != NULL || //This should prevent going backwards
      strchr(f, ':') != NULL || //This should prevent c:
      f[0] == '\\' || f[0] == '/') { //This should prevent absolute paths

      Con_Printf("Cmd_Exec: Did not execute \"%s\"; path was disallowed.\n", f);
      return;
   }

   // HL also likes to make sure there's only one dot, but I don't see the
   // advantage of that.  HL only executes .rc and .cfg further messing with us.
   COM_FileExtensionThreadSafe(f, extension, sizeof(extension));

   if(Q_strcasecmp(extension, "rc") != 0 && Q_strcasecmp(extension, "cfg") != 0) {
      Con_Printf("Cmd_Exec: Did not execute \"%s\"; didn't have either a .rc or .cfg extension.\n", f);
      return;
   }

   fp = COM_LoadFileForMe(f, NULL);
   if(fp == NULL) { //fail
      Con_Printf("Cmd_Exec: Couldn't open %s.  Better luck next time.\n", f);
      return;
   }

   Con_Printf("Cmd_Exec: Executing %s...\n", f);

   //'fp' now contains our complete file, null terminated.  Add it.
   Cbuf_InsertText(fp); //this can fail if the log is big enough.  If so, tough!

   //and since we malloc'd in LoadFileForMe, we must now free.
   COM_FreeFile(fp);
}
/* DESCRIPTION: Cmd_Echo_f
// LOCATION: cmd.c
// PATH: When "echo" is entered into the console
//
// Just prints the rest of the line to the console.  Whoopee.
*/
void Cmd_Echo_f() {

   int i;

   for (i = 0; i < Cmd_Argc(); i++) {
      Con_Printf("%s ",Cmd_Argv(i));
   }
   Con_Printf("\n");
}
/* DESCRIPTION: Cmd_ForwardToServer_f
// LOCATION: cmd.c
// PATH: lots
//
// We are a dedicated server.  What are we gonna forward to, ourselves?
*/
void Cmd_ForwardToServer() {

   Con_Printf("%s called", __FUNCTION__);
}
/* DESCRIPTION: Cmd_AliasList_f
// LOCATION: cmd.c
// PATH: NEW: When aliaslist is typed OR when alias is typed alone.
//
// Alias does this; QW does this separately.  Sounded like a good idea at the time.
*/
void Cmd_AliasList_f() {

   cmdalias_t *a;
   unsigned int num = 0;


   Con_Printf("Current aliases:\n");
   for(a = cmd_alias; a != NULL; a = a->next) {
      Con_Printf("  %s : %s\n", a->name, a->value);
      num++;
   }
   if(num > 0) {
      Con_Printf("%i total aliases.\n", num);
   }
   else {
      Con_Printf("No aliases found.\n");
   }
}
/* DESCRIPTION: Cmd_Alias_f
// LOCATION: cmd.c
// PATH: When "alias" is entered into the console
//
// Makes an alias.  Does this surprise you?
*/
void Cmd_Alias_f() {

   cmdalias_t *a;
   unsigned int i, j, length;
   const char * cs;
   char *s;

   //If it's just "alias", we print all known aliases.
   if (Cmd_Argc() == 1)   {
      Cmd_AliasList_f();
      return;
   }

   cs = Cmd_Argv(1);
   if (Q_strlen(cs) >= MAX_ALIAS_NAME || !Q_strcmp(cs, "say")) { //reject aliasing the say command.  Don't actually know why, don't care.
      Con_Printf("Alias name %s is too long--rejected", cs);
      return;
   }

   //HL doesn't seem to be as picky about creating aliases that are the same as
   //regular commands; it just assumes commands take priority.
   //That might be worth changing...

   //See how big this string will be...
   j = Cmd_Argc();
   length = 0;
   for(i = 2; i < j; i++) {

      j += Q_strlen(Cmd_Argv(i)) + 1; //space and null
   }
   if(length == 0) { //someone wants to delete this alias.  Allow it.

      //todo: make deletey function and delete.
      return;
   }
   if(length > 1024) { //sort've the general limit...
      Con_Printf("Alias %s was more than 1024 characters--did not create/replace.", cs);
      return;
   }

   // See if the alias already exists.
   for(a = cmd_alias; ; a = a->next) {

      if(a == NULL) { //rather then testing a twice, we break here.
         a = (cmdalias_t*)Z_Malloc(sizeof(cmdalias_t));
         Q_strcpy(a->name, cs);
         a->next = cmd_alias;
         cmd_alias = a;
         break;
      }
      else if(Q_strcmp(cs, a->name) == 0) {
         //is it worth the effort to see if it's the same as our change?
         Z_Free(a->value);
         break;
      }
   }

   //I think we're past all the errors invalid input could provide.

   s = (char *)Z_Malloc(length);
   j = Cmd_Argc();
   for(i = 2; i < j; i++) {

      if(i != 2) { Q_strcat(s, "\n"); }
      Q_strcat(s, Cmd_Argv(i)); //space and null
   }
   a->value = s;
}
/* DESCRIPTION: Cmd_Wait_f
// LOCATION: cmd.c
// PATH: When "wait" is entered into the console
//
// Will cause the command buffer to not process the remaining commands until
// next frame.  Mostly used for annoying scripts.
//
// How is this accomplished?  By modifying a global var.  QW is far more
// elegant in its solution.
*/
void Cmd_Wait_f() {
   cmd_wait = 1;
}
/* DESCRIPTION: Cmd_Cmdlist_f
// LOCATION: cmd.c
// PATH: When "cmdlist" is entered into the console
//
// Lists all the registered commands.  SHOULD list them probably alphabetically,
// should allow fiCmd_Alias_fle redirection, and should allow for some form of filtering.
// should, doesn't.
*/
void Cmd_CmdList_f() {

   cmd_function_t * cmd;
   int matches;

   if(Cmd_Argc() > 1) {

      //This can be as simple as spitting everything out
      //or as hard as opening and writing to a file.
      if(Q_strcmp("?", Cmd_Argv(1)) == 0) {

         Con_Printf("CmdList           : List all commands\nCmdList [Partial] : List commands starting with 'Partial'\nCmdList log [Partial] : Logs commands to file \"cmdlist.txt\" in the gamedir.\n");
         return;
      }
      else if(Q_strcasecmp("log", Cmd_Argv(1)) == 0) {

         Con_Printf("File redirection will be added in later.  Like after the engine is finished.\n");
         return;
      }
      else {
         Con_Printf("Actually adding in the [partial] stuff is currently a low priority.  Google it.\n");
      }
   }

   matches = 0;

   Con_Printf("\"Command list:\"\n");

   for (cmd=cmd_functions; cmd != NULL; cmd=cmd->next) {

      //todo: make a filter continue option within, say, five years.
      Con_Printf("%s\n", cmd->name);
      matches++;
   }

   Con_Printf("\n--%u matching commands found.\n", matches);
}
/* DESCRIPTION: Cmd_ShowAlias_f
// LOCATION: cmd.c
// PATH: NEW: When showalias is typed.
//
// Looked cool, so I added it.  Demystifies an alias.  I don't remember being
// easily able to in HL...
*/
void Cmd_ShowAlias_f() {

   cmdalias_t   *a;
   const char *s;

   if(Cmd_Argc() != 2) {
      Con_Printf("showalias <alias> : Prints out what the alias is equivalent to.\n");
      return;
   }

   s = Cmd_Argv(1);

   //find it, print it
   for (a = cmd_alias; a != NULL; a = a->next) {
      if(Q_strcmp(s, a->name) == 0)   {
         Con_Printf("alias %s is equivalent to \" %s\"\n", a->name, a->value);
         return;
      }
   }

   //look for it with mismatched case
   for (a = cmd_alias; a != NULL; a = a->next) {
      if(Q_strcasecmp(s, a->name) == 0) {
         Con_Printf("alias found with mismatched case (%s is equivalent to \" %s\")\n", a->name, a->value);
         return;
      }
   }

   Con_Printf("Alias %s doesn't exist\n", s);
}

/* DESCRIPTION: Cmd_Init
// LOCATION: cmd.c
// PATH: Host_Init->Cmd_Init
//
// Calls addcommand() a lot, and that's it right now.
*/
void Cmd_Init() {

   Cmd_AddCommand("stuffcmds", Cmd_StuffCmds_f);
   Cmd_AddCommand("exec", Cmd_Exec_f);
   Cmd_AddCommand("echo", Cmd_Echo_f);
   Cmd_AddCommand("alias", Cmd_Alias_f);
   Cmd_AddCommand("cmd", Cmd_ForwardToServer);
   Cmd_AddCommand("wait", Cmd_Wait_f);
   Cmd_AddCommand("cmdlist", Cmd_CmdList_f);
   Cmd_AddCommand("showalias", Cmd_ShowAlias_f);
   Cmd_AddCommand("aliaslist", Cmd_AliasList_f);

}

static int cmd_argc;
static char *cmd_argv[MAX_ARGS];
static const char *cmd_args = NULL;

void Cmd_Shutdown() {

   //mostly just tidies up the cmd_argx data sets.
   //They never call z_free though... I guess that means during shutdown it's
   //all taken care of elsewhere.
   cmd_functions = NULL;
   cmd_argc = 0;
   memset(cmd_argv, 0, MAX_ARGS<<2); //that should wipe out all those pointers.
   cmd_args = NULL;
}

HLDS_DLLEXPORT int Cmd_Argc() {
   return cmd_argc;
}
HLDS_DLLEXPORT const char * Cmd_Argv(int arg) {
   if(arg >= cmd_argc) { return(""); }
   return(cmd_argv[arg]);
}
HLDS_DLLEXPORT const char * Cmd_Args() {
   if(!cmd_args) { return(""); }
   return(cmd_args);
}

/* DESCRIPTION: Cmd_TokenizeString
// LOCATION: cmd.c (as levels)
// PATH: cmd & global_sv
//
// Tokenizes a string with the three 'class' variables cmd_argc/s/v.
*/
void Cmd_TokenizeString(const char *text) {

   int i;

   //Clear out the old
   for(i = 0; i < cmd_argc; i++) {
      Z_Free(cmd_argv[i]);
   }

   cmd_argc = 0;
   cmd_args = NULL;

   while (1) {

      //Skip whitespace
      while((!isprint(text[0])) && //This hits ';' as well
             text[0] != '\0' &&
             text[0] != '\n') {
         text++;
      }

      if(text[0] == '\n') { return; }
      if(text[0] == '\0') { return; }

      if(cmd_argc == 1) { cmd_args = text; } //should we malloc?

      text = COM_Parse(text);
      if(text == NULL) { return; }

      i = Q_strlen(global_com_token);
      i++;

      if(i > 0x203) { return; }

      if(cmd_argc >= MAX_ARGS) { continue; }

      cmd_argv[cmd_argc] = (char *)Z_Malloc(i);
      Q_strcpy(cmd_argv[cmd_argc], global_com_token);
      cmd_argc++;
   }

}




/* DESCRIPTION: Cmd_CheckParm
// LOCATION: cmd.c
// PATH: none?
//
// much like the COM equivalents, except they're looking for a word in our
// parsed console command instead.
//
// Much simpler, no nextparn gobbledygook.
*/
int Cmd_CheckParm(const char *parm) {

   unsigned int i, j;

   if(parm == NULL) { Sys_Error ("Cmd_CheckParm: NULL"); }

   i = 1;
   j = Cmd_Argc();
   while(i < j) {
      if(Q_strcasecmp(parm, Cmd_Argv(i)) == 0) { return(i); }
      i++;
   }
   return(0);
}

void Cmd_CompleteCommand() {
   Sys_Error("%s called\n", __FUNCTION__);
}
//char *Cmd_CompleteCommand(char *partial, int matchnum);

/* DESCRIPTION: Cmd_ExecuteString
// LOCATION: cmd.c (as levels)
// PATH: Whenever someone needs to translate some text into a function.
//
// Goes through and tries to match up a command with aliases, functions, and
// variables.  It then calls/changes the corresponding item.
*/
void Cmd_ExecuteString(const char *text, int level) {

   cmd_function_t   *cmd;
   cmdalias_t *alias;

   //after this we don't deal with arg4.
   global_cmd_source = level;


   Cmd_TokenizeString(text);
   if(Cmd_Argc() == 0) { return; } //Must be an empty string.

   /* Now we cycle through a long list of aliases, commands, etc. */

   //Matching functions.

   //In QW there's some sort of 'if the cases don't match check aliases
   //first' nonsense.  I thought about doing the same, however I felt
   //that sort of ambiguity would cause too many problems.  Noted for
   //possible future tweakage.
   cmd = cmd_functions;

   while(cmd != NULL) {

      if(Q_strcasecmp(cmd_argv[0], cmd->name) == 0) {

         cmd->function();

         /* What followed were various checks to unknown parms in
         // the CLS struct, but nothing really came out of it.
         // It's assumed that privileged commands like cheats
         // are filtered within the called xcommand, unlike in the
         // QW code where they seem to be checked for from the get go.
         */
         return;
      }
      cmd = cmd->next;
   }

   //Matching aliases
   alias = cmd_alias;
   while(alias != NULL) {

      if(Q_strcasecmp(cmd_argv[0], alias->name) == 0) {

         Cbuf_InsertText(alias->value);
         return;
      }
      alias = alias->next;
   }

   // check cvars
   if(Cvar_Command() != 0) { return; } //cvar_command prints its value if found.

   //these typedefs are 3, 4, and 5 respectively.
   if(global_cls.state == ca_connected || global_cls.state == ca_uninitialized || global_cls.state ==  ca_active) {

      Cmd_ForwardToServer(); //We should never do this since we ARE the server.
   }
   else {
      Sys_Warning("Unknown variable or unsupported command: ");
      Cmd_Echo_f();
   }
}

/* DESCRIPTION: Cmd_Exists
// LOCATION: cmd.c
// PATH: cvar_registerVariable
//
// If the name doesn't fit, the function a'quits.
*/
qboolean Cmd_Exists(const char *cmd_name) {

   cmd_function_t   *cmd;

   for(cmd=cmd_functions ; cmd ; cmd=cmd->next) {
      if(!Q_strcmp (cmd_name,cmd->name)) { return(1); }
   }
   return(0);
}

/* DESCRIPTION: Cmd_FindCmd
// LOCATION: derived from cvar.c
// PATH: Cmd_CompleteCommand
//
// Finds and returns a console command named *var_name*.  Or NULL of not found.
*/
cmd_function_t * Cmd_FindCmd(const char *var_name) {

   cmd_function_t *var;

   for(var = cmd_functions; var != NULL; var = var->next) {

      if (Q_strcmp(var_name, var->name) == 0) {
         return(var);
      }
   }
   return(NULL);
}

/* DESCRIPTION: Cmd_FindCmdPrev
// LOCATION: derived from cvar.c
// PATH: Cmd_CompleteCommand
//
// Appears to do the same thing FindCmd does, except it returns the preceding
// cmd structure instead of the matching one.
*/
cmd_function_t * Cmd_FindCmdPrev(const char *var_name) {

   cmd_function_t *var;

   if(cmd_functions == NULL) { return(NULL); }

   //special case: First on list
   if(Q_strcmp(var_name, cmd_functions->name) == 0) {
      return(cmd_functions);
   }

   for(var = cmd_functions; var->next != NULL; var = var->next) {

      if(Q_strcmp(var_name, var->next->name) == 0) {
         return(var);
      }
   }
   return(NULL);
}

/* DESCRIPTION: Cmd_GetFirst
// LOCATION:
// PATH: none
//
// Just returns the beginning of the linked list.
*/
cmd_function_t * Cmd_GetFirstCmd() { return(cmd_functions); }



int Cmd_ForwardToServerUnreliable() {
   Con_Printf("%s called", __FUNCTION__);
   return(0);
}
int Cmd_ForwardToServerInternal(char * s) {
   Con_Printf("%s called w buffer %s", __FUNCTION__, s);
   return(0);
}


//cbuf.


/* DESCRIPTION: Cbuf_Init
// LOCATION: cmd.c
// PATH: Host_Init->Cbuf_Init
//
// Carves out some memory and assigns it to the global variable cmd_text
*/
void Cbuf_Init() {

   SZ_Alloc("cmd_text", &cmd_text, 0x4000); //16K
}
void Cbuf_Shutdown() {

}

/* DESCRIPTION: Cbuf_AddText
// LOCATION: cmd.c
// PATH: lots
//
// Tacks on some text to our cmd_text storage.
*/
void Cbuf_AddText(const char *text) {

   unsigned int length;


   length = Q_strlen(text);
   if(length == 0) { return; } //do nothing, don't proceed to further checks.


   if(cmd_text.cursize+length+1 < cmd_text.maxsize) {
      SZ_Write(&cmd_text, text, length);
      SZ_Write(&cmd_text, "\n", Q_strlen("\n"));
   }
   else {
      Con_Printf("%s: overflow\n", __FUNCTION__);
   }
}

/* DESCRIPTION: Cbuf_InsertText
// LOCATION: cmd.c
// PATH: A fair few
//
// Tacks on some text to our cmd_text storage.  The difference
// between it and addtext is that this function puts it at the beginning,
// needlessly shifting all the rest of the data.
*/
void Cbuf_InsertText(const char *text) {

   unsigned int length;


   length = Q_strlen(text);
   if(length == 0) {
      Con_Printf("%s: empty string\n", __FUNCTION__);
      return;
   }

   if(cmd_text.cursize+length+1 < cmd_text.maxsize) {
      if(cmd_text.cursize > 0) {

         //Our command buffer is NOT null terminated.  That's important.
         //It IS however \n terminated.

         Q_memmove(cmd_text.data+length+1, cmd_text.data, cmd_text.cursize);
         Q_memcpy(cmd_text.data, text, length);
         cmd_text.data[length] = '\n';
         cmd_text.cursize += length + 1;
      }
      else {
         //we know it fits, we checked that already.  Don't call AddText
         //and relock our lock, just copy the code segments.
         SZ_Write(&cmd_text, text, length);
         SZ_Write(&cmd_text, "\n", Q_strlen("\n"));
      }
   }
   else {
      Con_Printf("%s: overflow.\n", __FUNCTION__);
   }
}

/* DESCRIPTION: Cbuf_InsertTextLines
// LOCATION: cmd.c
// PATH: Cmd_Exec_f->Cbuf_InsertTextLines
//
// See the above function.  This looks identical save for some padding '\n's.
// I want to see what happens with a compromise, adding one \n to both.
// My knowledge of HL's parsing tells me this shouldn't be a huge issue.
*/
void Cbuf_InsertTextLines(const char *text) {

   Cbuf_InsertText(text);
}

/* DESCRIPTION: Cbuf_Execute
// LOCATION: cmd.c (as levels)
// PATH: Notably once per frame, a few others
//
// This goes through our buffer and digs out lines terminated by \ns or ;s.
// It strcmps them to our registered commands and calls functions.  Woo.
//
// I made the mistake earlier of moving my string pointer with the buffer.
// Unfortunately I forgot about Cbuf_InsertText(), which can nicely MOVE it all
// around, and which can be called by the functions.  Hate to say it, but
// until I figure out something better I'm going to memmove the data around
// after each successfully executed function.
*/
void Cbuf_Execute() {

   unsigned int length;
   qboolean quoted;
   char *start;
   char line[1024];


   if(cmd_text.cursize == 0) { return; }

   start = (char *)cmd_text.data;
   length = 0;
   quoted = 0;

   do {

      //We are interested in \n, ;, and ". Any of those three we break for.
      //Unfortunately since a terminating null isn't guaranteed, it's
      //probably not safe to try using strpbrk.  A terminating \n is guaranteed.
      //Quotes may be an issue; todo: look into them for possible exploit.
      while(length < cmd_text.cursize) {

         if(start[length] == '"') {
            quoted++;
            quoted &= 1;
         }
         else if((quoted == 0 && start[length] == ';') ||
                 (start[length] == '\n') || (start[length] == '\r')) {
            length++;
            break;
         }
         length++;
      }

      //hack: max size of 1024, look into later.
      //We should be able to safely wipe the \n or ;.

      if(length > 0) { //if it's exactly one, we still want to modify our counters.
         if(length == 1 || length > 1023) {

            //In these two cases, we actually want to move our pointer up instead of data down.
            start += length;
            cmd_text.cursize -= length;
            length = 0;
         }
         else {

            Q_memcpy(line, start, length-1);
            line[length-1] = '\0';
            Q_memmove(cmd_text.data, start+length, cmd_text.cursize-length);
            cmd_text.cursize -= length;

            //Whatever we execute might just replace our buffer you know.
            Cmd_ExecuteString(line, 1);

            start = (char *)cmd_text.data;
            length = 0;
         }
      }

      if(cmd_wait != 0) {
//         Q_memcpy(cmd_text.data, start, cmd_text.cursize);
         cmd_wait = 0;
         break;
      }

   } while(cmd_text.cursize > 0);
}
