#ifndef OSHLDS_CMD_HEADER
#define OSHLDS_CMD_HEADER

#include "endianin.h"
#include "ModCalls.h"

//defines
#define MAX_ALIAS_NAME 32
#define MAX_ARGS 80
typedef void (*xcommand_t) ();
typedef enum {
   cmd_CLIENT = 0,
   cmd_SERVER
} cmd_source_t;

//structs
typedef struct cmdalias_s {
   struct cmdalias_s *next;
   char name[MAX_ALIAS_NAME];
   char *value;
//   qbyte execlevel;
//   qbyte restriction;
//   int flags;
} cmdalias_t;
typedef struct cmd_function_s {

   struct cmd_function_s *next;
   const char *name;
   xcommand_t function; //That's actually a void pointer to a function

   int restriction;   //restriction of admin level; could be zmalloced too...
//  byte zmalloced;
} cmd_function_t;


//globals
extern cmd_source_t global_cmd_source;


//functions
void Cbuf_Init();
void Cbuf_Shutdown();
void Cbuf_AddText(const char *text);
void Cbuf_InsertText(const char *text);
void Cbuf_Execute(void);
void Cmd_RemoveGameCmds();
void Cmd_Shutdown();

void Cmd_Init();
HLDS_DLLEXPORT void Cmd_AddGameCommand(char *, xcommand_t);

HLDS_DLLEXPORT int Cmd_Argc(void);
HLDS_DLLEXPORT const char * Cmd_Argv(int);
HLDS_DLLEXPORT const char * Cmd_Args(void);
void Cmd_ExecuteString(const char *, int);
void Cmd_TokenizeString(const char *);
qboolean Cmd_Exists(const char *);
void Cmd_AddCommand(const char *, xcommand_t);
void Cmd_ForwardToServer();



#endif
