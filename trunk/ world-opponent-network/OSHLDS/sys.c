#ifdef _WIN32
 #include <winsock2.h> //windows.h MAY already include regular winsock, fubaring us if so.
 #include <windows.h>
#endif

#include "sys.h"
#include "HUNK.h"
#include "banlist.h"
#include "common.h"
#include "cmd.h"
#include "cvar.h"
#include "host.h"
#include "Modding.h"
#include "NET.h"
#include "pf.h"
#include "random.h"
#include "report.h"
#include "Q_memdef.h"
#include "FS.hpp"



/*** GLOBALS ***/
double global_realtime;
int giActive;
char * global_pr_strings = NULL;


static cvar_t cvar_servercfgfile  = {  "servercfgfile",       "server.cfg", FCVAR_NONE, 0, NULL };
static cvar_t cvar_lservercfgfile = { "lservercfgfile", "listenserver.cfg", FCVAR_NONE, 0, NULL };
static cvar_t cvar_motdfile       = {       "motdfile",         "motd.txt", FCVAR_NONE, 0, NULL };
static cvar_t cvar_logsdir        = {        "logsdir",             "logs", FCVAR_NONE, 0, NULL };
       cvar_t cvar_mapcyclefile   = {   "mapcyclefile",     "mapcycle.txt", FCVAR_NONE, 0, NULL };

/*** FUNCTIONS ***/

//sys_init is unused, though called in Sys_InitGame (mangled)
void Sys_Init() {

   Cvar_RegisterVariable(&cvar_servercfgfile);
   Cvar_RegisterVariable(&cvar_lservercfgfile);
   Cvar_RegisterVariable(&cvar_motdfile);
   Cvar_RegisterVariable(&cvar_logsdir);
   Cvar_RegisterVariable(&cvar_mapcyclefile);
}

void Sys_Quit() {
   giActive = DLL_CLOSE;
}
//sys_makecodewriteable exists and is in the QW source, but in the binary does nothing and is not called
//sys_pagein is unreferenced and empty
//Sys_PopFPCW "" ""
//Sys_SetFPCW "" ""
//Sys_PushFPCW_SetHigh "" ""
//Sys_ShutdownLauncherInterface "" ""

//the plan is to make any sys_auth unnecessary


/* DESCRIPTION: Sys_InitArgv
// LOCATION:
// PATH: Init__19CDedicatedServerAPIPcT1PFPCcPi_P14IBaseInterfaceT3->Sys_InitArgv__FPc
//
// The passed string is the argument list, with spaces for nulls (then what the
// hell is COM_InitArgv for!?).  At this stage host_parms is completely empty...
// By the end of the game it contains our arguments just like the real argv would.
// Both global_com_argv and host_parms_argv hold the same addresses.
//
// Why exactly splitting this into a sys and a com function was deemed a good
// idea I'll leave for the philosophers.  I believe it should all be in com.
// And to that end this function becomes nothing more than a jump in the list.
*/
void Sys_InitArgv(char *ArgumentString) {

   COM_InitArgv(ArgumentString);
}

/* DESCRIPTION: Sys_ShutdownArgv
// LOCATION:
// PATH: Shutdown__19CDedicatedServerAPI3->Sys_InitArgv__Fv
//
//
*/
void Sys_ShutdownArgv() {

   //does nothing
}


/* DESCRIPTION: Sys_InitMemory
// LOCATION: global_sv_sys_win.c (not unique function, code block)
// PATH: Sys_InitGame->Sys_InitMemory__Fv
//
// I'll give you two guesses, but if your first isn't malloc I'll hurt you
*/
void Sys_InitMemory() {

   unsigned int MemoryA;
   const char * retp;

   //First we should see if the user explicitly told us how much mem to ask for.
   //32 is a good round number.  Let's start off with that.

   MemoryA = 0x02000000;

   retp = COM_GetParmAfterParmCase("-heapsize");
   if(retp[0] != '\0') {
      MemoryA = atoi(retp)<<10; //* 1024, this parm is in kB

   }
   if(COM_CheckParm("-minmemory")) {
      MemoryA = 0x00E00000; //15 megs, this switch does take priority
   }
   else if(MemoryA < 0x00E00000) {
      Con_Printf("I dunna think she can handle the memory stress, cap'n\n");
   }

   host_parms.membase = Q_Malloc(MemoryA);
   host_parms.memsize = MemoryA;

   if(host_parms.membase == NULL) {
      Sys_Error("Sys_InitMemory: Went in for %u bytes of memory--Denied\n", host_parms.memsize);
   }

}
/* DESCRIPTION: Sys_ShutdownMemory
// LOCATION: global_sv_sys_win.c (not unique function, code block)
// PATH:  Sys_ShutdownGame->Sys_ShutdownMemory_Fv
//
// Where there's a malloc...
*/
void Sys_ShutdownMemory() {

   Q_Free(host_parms.membase);
   host_parms.membase = NULL;
   host_parms.memsize = 0;

}

void Sys_ShutdownGame() {

   //Sys_ShutdownAuthentication();
   Host_Shutdown();
   NET_Config(0);
   //Sys_ShutdownLauncherInterface();
   Sys_ShutdownMemory();
   //Sys_Shutdown();
}


/* DESCRIPTION: Sys_LoadModule
// LOCATION: The SDK
// PATH:  Two places for loading libs
//
// *snicker* Bla bla bla dlsym doesn't look in the CWD, whoever originally wrote
// this somehow forgot about "./".
*//*
void * Sys_LoadModule(char *library) {

   char temp[70] = "./"; //I don't know what the max lib name may be but I'd guess under 70
   strncat(temp, library, 67);
   temp[70] = '\0';

   return(dlopen(temp, RTLD_NOW ));
}
Sys_UnloadModule(void *library) {

   if(library == NULL) {
      Con_Printf("Sys_UnloadModule warning: tried to unload unlinked library");
   }
   else {
      dlclose(library);
   }
}

*/

//Sys_SetupLegacyAPIs__Fv would normally add an extra layer of fun to our
//console reporting, however as I simplified Sys_Error(), it's not currently necessary.
//Normally called by Sys_InitLauncherInterface__Fv


/* DESCRIPTION: Sys_InitLauncherInterface
// LOCATION: The SDK
// PATH:  Sys_InitGame__FPcT0Pvi-> Sys_InitLauncherInterface
//
// The only thing this did is call _Sys_SetupLegacyAPIs__Fv.
// That is no longer necessary.
*/
void Sys_InitLauncherInterface() { }

#if 0
/* DESCRIPTION: Sys_ShowProgressTicks
// LOCATION:
// PATH:  dynamic?
//
// This does something... Something dependant on STEAM, which means it does
// nothing for me.
*/
void Sys_ShowProgressTicks(char* i) {

}

// called by one of the cache print functions
int Sys_SplitPath(char * a, char * b, char * c, char * d, char * e) {
   Sys_Error("SplitPath called; re-examine its usage\n");
}

#endif
/* DESCRIPTION: Sys_Sleep
// LOCATION: Pretty common technique
// PATH:  unclear
//
// Wrapper for the inconsistent windows and linux sleep functions.  To be honest
// sleeping is something that just doesn't need to be done that often.
*/
void Sys_Sleep(int msec) {

   #ifdef _WIN32
    Sleep(msec);
   #else
    usleep(msec*1000);
   #endif

}

void Sys_uSleep(int nsec) {

   #ifdef _WIN32
    Sleep(nsec/1000);
   #else
    usleep(nsec);
   #endif
}

/* DESCRIPTION: Sys_FloatTime
// LOCATION: sys_linux.c (as sys_doubletime)
// PATH:  unclear
//
// Gets the time.  To be honest, I don't know why it says it is a float.
// The return value is in seconds.  It's a double, so expect small numbers.
//
// A note on precision and accuracy:  Millisecond precision is what we're used
// to, and accuracy, though nice, isn't too important as long as we're not
// getting smaller numbers as we go through time.
//
// For Windows, we have several choices.  timeGetTime works, but the precision
// (on 2000 at least) sucks, and is about 15 ms.  We can change this though,
// and if we do, we average an almost steady 970 frames/s.
//
// GetTickCount is also an option.  Unlike timeGetTime, we don't need to set a
// precision (works fine for me on 2000), but the results vary more.
// We average of 965 frames/s, and it often varies up to +/- 5fps
//
// For Unix, we have gettimeofday  I assume it's suitiable, but haven't
// clocked it or analyzed its spread.
//
// Survey says we should.  Otherwise, we get 60,000 frames but only 64 unique
// times.
*/
HLDS_DLLEXPORT double Sys_FloatTime() {


  #if defined(USE_timeGetTime)
   static int starttime = 0;
   static qboolean first = 1;
   int now;

   now = timeGetTime();

   if(first) {
      first = 0;
      starttime = now;
      return(0.0);
   }

   return(((double)(now - starttime)) / 1000.0);

  #elif defined(USE_GetTickCount)
   static int starttime = 0;
   static qboolean first = 1;
   int now;

   now = GetTickCount();

   if(first) {
      first = 0;
      starttime = now;
      return(0.0);
   }

   return(((double)(now - starttime)) / 1000.0);

  #elif defined(USE_gettimeofday)

   static int secbase = 0;
   struct timeval tp;

   gettimeofday(&tp, NULL);

   if(secbase == 0)   {
      secbase = tp.tv_sec;
      return(tp.tv_usec/1000000.0);
   }

   return((tp.tv_sec - secbase) + tp.tv_usec/1000000.0);

  #else
   #error no timer specified
  #endif
}

int Sys_InitGame() {

   //Four args, three unused, and one I don't care to use.
   //char * gamedir[0x100];



   //gmodinfo might need zeroing out on server restarts and whatnot, but for the
   //first server, there ought to be no troubles.

   //Tracing sucks.
   SeedRandomNumberGenerator();

   Sys_InitMemory();

   if(Host_Init(&host_parms) == 0) { //failed.
      return(0);
   }

   Sys_Init(); //be after host_init, as cvars need memory.

   //COM_GetGameDirSize(gamedir, sizeof(gamedir));
   //Sys_InitAuthentication

   Host_InitializeGameDLL();
   Banlist_Init();
   NET_Config(1);

   return(1);
}



//cbase
//These last few functions are the highest level wrappers.
int Init(char * path, char * argv) {

   char temp[0x100];

   if(TestStructureAlignment() == 0) { return(0); }
   Sys_InitArgv(argv);

   if(FileSystem_Init(path) == 0) { return(0); }
   if(Sys_InitGame() == 0) { return(0); }

   Q_snprintf(temp, sizeof(temp)-1, "exec %s\n", cvar_servercfgfile.string);
   temp[sizeof(temp)-1] = '\0';
   Cbuf_InsertText(temp);

   #ifdef USE_timeGetTime
    timeBeginPeriod(1);
   #endif

   return(1);
}
int Shutdown() {

   Sys_ShutdownGame();
   FileSystem_Shutdown();
   Sys_ShutdownArgv();

   #ifdef USE_timeGetTime
    timeEndPeriod(1);
   #endif

   return(giActive);
}
