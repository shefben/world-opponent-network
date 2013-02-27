/* cvars are console variables.  They're basically encapsulated globals.
// When declaring a cvar, we only need to assign first three parameters:
//
// char *name;
// char *string;
// int flags;
//
// In mods, the other two are usually left empty (defaulting to zero).
// When the cvar is added to the cvar list, the string is copied to Z-memory
// (see hunk for what that is), and the float value is computed with a simple
// atof.  The cvar is then linked in.
//
// Mods zero out the last two values.  I usually actually assign the float
// the correct value.  It's all static afterall.
*/

#ifndef OSHLDS_CVAR_HEADER
#define OSHLDS_CVAR_HEADER

#include "endianin.h"
#include "ModCalls.h"


//structs
/* The cvar_s structure is used by mods.  The first thre fields CANNOT be changed,
// and the size of the struct cannot change either.
*/
typedef struct cvar_s {

   char *name;
   char *string;
   int flags;
   float value; //This value was all but unused in the engine.
   struct cvar_s *next;
} cvar_t;


//flags (from the SDK of course)
#define FCVAR_NONE          0x0000
#define FCVAR_ARCHIVE       0x0001 // set to cause it to be saved to vars.rc
#define FCVAR_USERINFO      0x0002 // changes the client's info string
#define FCVAR_SERVER        0x0004 // notifies players when changed
#define FCVAR_EXTDLL        0x0008 // defined by external DLL
#define FCVAR_CLIENTDLL     0x0010 // defined by the client dll
#define FCVAR_PROTECTED     0x0020 // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY        0x0040 // This cvar cannot be changed by clients connected to a multiplayer server.
#define FCVAR_PRINTABLEONLY 0x0080 // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED      0x0100 // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log

//functions
void Cvar_RegisterVariable(cvar_t *);
void Cvar_Set(const char *, const char *);
void Cvar_SetValue(const char *, float);
void Cvar_DirectSet(cvar_t *, const char *);
void Cvar_DirectSetValue(cvar_t *, float);
int Cvar_VariableInt(const char *);
float Cvar_VariableValue(const char *);
const char * Cvar_VariableString(const char *);
unsigned int Cvar_CountServerVariables();
int Cvar_SVCRuleInfoVarWrite(char *, unsigned int);
qboolean Cvar_Command();
void Cvar_UnlinkExternals();
void Cvar_Shutdown();

HLDS_DLLEXPORT float CVarGetFloat(const char *);
HLDS_DLLEXPORT cvar_t * CVarGetPointer(const char *);
HLDS_DLLEXPORT const char * CVarGetString(const char *);
HLDS_DLLEXPORT void CVarRegister(cvar_t *);
HLDS_DLLEXPORT void CVarSetFloat(const char *, float);
HLDS_DLLEXPORT void CVarSetString(const char *, const char *);


//global cvars

/* Cvars are suppossed to be encapsulated, however since a cvar lookup is one
// of those N level deals, that's just not practical.  We can directly READ
// cvars; writing to them is another story, but we have cvar functions that
// don't involve the N complexity search to handle that.
//
// Making it a binary tree of some sort is high on the to-do list, since
// additions only happen during the init stage and deletions only happen
// on termination.  However, for speed, the engine should never search
// for cvars.  It should access them directly.
//
// For most cvars, this can still be encapsulated by making the var static
// and referencing it only in one file.  For some though, this isn't really
// doable or practical.  They are still declared in whatever file they
// belong in, but the extern is declared here (that way they're all in the same
// place, and can be included with ease).
//
//
// Do not break these rules.  I just explained why they exist, so follow them.
*/
extern cvar_t cvar_mapcyclefile; //present until map cycling is worked out, may go into own file.
extern cvar_t cvar_developer; //many uses, will probably always be here.
extern cvar_t cvar_announce_bans; //controlled by the banlist 'class', referenced elsewhere.  In limbo for now.
extern cvar_t cvar_sv_lan; //used in a few packet places, can *probably* be worked around.
extern cvar_t cvar_hostname; //This is probably just better off as a global.
extern cvar_t cvar_deathmatch; //used in a lot of places, I dno't think it should be a cvar, but as it's a server cvar, I can't change that.
extern cvar_t cvar_coop; //same as above, but not as common.  Both logically belong in host, and are edited early on.
extern cvar_t cvar_sv_stepsize; //limbo
extern cvar_t cvar_clockwindow; //pending reordering of init functions.
//extern cvar_t cvar_sv_filterban; //another ban oriented cvar.  Limbo.
#endif
