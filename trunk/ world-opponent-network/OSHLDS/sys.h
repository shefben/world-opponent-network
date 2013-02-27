#ifndef OSHLDS_SYS_HEADER
#define OSHLDS_SYS_HEADER

#include "Q_memdef.h"
#include "endianin.h"
#include "ModCalls.h"

//They're around, not sure these are the right ones though for sys_quit
#define DLL_INACTIVE 0      // no dll
#define DLL_ACTIVE   1      // dll is running
#define DLL_PAUSED   2      // dll is paused
#define DLL_CLOSE    3      // closing down dll
#define DLL_TRANS    4      // Level Transition

#ifdef _WIN32
 #define USE_timeGetTime 1
 //#define USE_GetTickCount 1
#else
 #define USE_gettimeofday 1
#endif


//vars (this is the highest level short of main.  The most pervasive of vars go here.
extern double global_realtime;
extern int giActive;
extern char * global_pr_strings;

//funcs
void Sys_uSleep(int);
HLDS_DLLEXPORT double Sys_FloatTime();
int Init(char *, char *);
int Shutdown();
void Sys_Quit();
#endif
