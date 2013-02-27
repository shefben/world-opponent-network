/* "common" stuff goes in here.  It was previously a hodge podge of random
// fragments; it's since been divided into several files, all of which are
// included if you specify this header.
*/
#ifndef OSHLDS_COMMON_HEADER
#define OSHLDS_COMMON_HEADER

//This is suppossed to include common things.  Include the base types that
//can't possibly conflict with anything.

#include "links.h"
#include "com_file.h"
#include "info.h"
#include "sizebuf.h"
#include "messages.h"

//typedefs
typedef enum {
   ca_dedicated = 0,
   ca_disconnected,
   ca_connecting,
   ca_connected,
   ca_uninitialized, //Not sure this is correct...
   ca_active
} cactive_t;

//structs
typedef struct quakeparms_s {
   char * padding; //Dunno why it's off...
   char *basedir;      // Base / Mod game directory
   int argc;
   char **argv;
   void *membase;
   unsigned int memsize;
} quakeparms_t;


//globals
extern int  global_com_argc;
extern char ** global_com_argv;
extern char global_com_token[0x400];
extern int  global_com_ignorecolons; //unused.  Thanks to Boost not caring.
extern quakeparms_t host_parms;


//functions

//init
void COM_Init();
void COM_InitArgv(char *);
int COM_EntsForPlayerSlots(int);

//parm
int COM_CheckNextParm(char *, int);
int COM_CheckParm(char *);
int COM_CheckNextParmCase(char *, int);
int COM_CheckParmCase(char *);
const char *COM_GetParm(int);
const char *COM_GetParmAfterParm(char *);
const char *COM_GetParmAfterParmCase(char *);
const char * COM_Parse(const char *);
void COM_UngetToken();

//other
void COM_Munge1(char *, int, int);
void COM_UnMunge1(char *, int, int);
void COM_Munge2(char *, int, int);
void COM_UnMunge2(char *, int, int);
void COM_Munge3(char *, int, int);
void COM_UnMunge3(char *, int, int);

int PR_IsEmptyString(const char *);
int build_number();

char * va(char *, ...);
#endif

