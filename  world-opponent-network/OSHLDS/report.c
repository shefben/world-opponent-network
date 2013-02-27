
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cvar.h"
#include "report.h"

void Con_Init() {

   //Normally adds a command that we don't need.
}
void _Con_Printf(int level, const char *format, va_list argptr) {

   //level can be many things, but usually 0 == message, 1 == debug message, 2 == really wordy specific debug message.
   if(level > cvar_developer.value) { return; }

   vprintf(format, argptr);
}

/* DESCRIPTION: Sys_Error/Con_DPrintf
// LOCATION: Version can be found in both QW and the anon_source
// PATH: all over
//
// Variable argument functions.  There's a lot, and the differences
// minimal.  We'll rework it in time.
*/
void Sys_Error(const char *format, ...) {

   va_list argptr;

   va_start(argptr, format);
   _Con_Printf(-1, format, argptr);
   va_end(argptr);

   //cheap, but effective.  I personally believe that the mere act of
   //trying to salvage the server, or free resources, is bound to cause
   //more harm than good.
   exit(1000);
}
HLDS_DLLEXPORT void Con_Printf(const char *format, ...) {

   va_list argptr;

   va_start(argptr, format);
   _Con_Printf(0, format, argptr);
   va_end(argptr);
}
HLDS_DLLEXPORT void Con_DPrintf(const char *format, ...) {

   va_list argptr;

   va_start(argptr, format);
   _Con_Printf(1, format, argptr);
   va_end(argptr);
}
void Sys_Printf(const char *format, ...) {

   va_list argptr;

   va_start(argptr, format);
   _Con_Printf(0, format, argptr);
   va_end(argptr);
}
void Sys_Warning(const char *format, ...) {

   va_list argptr;

   va_start(argptr, format);
   _Con_Printf(1, format, argptr);
   va_end(argptr);
}
HLDS_DLLEXPORT void AlertMessage(ALERT_TYPE atype, char *szFmt, ...) {

   va_list argptr;

   switch(atype) {

   case at_notice:
   case at_console:
      if(cvar_developer.value < 0) { return; }
      break;

   case at_warning:
      if(cvar_developer.value < 1) { return; }
      break;

   case at_aiconsole:
      if(cvar_developer.value < 2) { return; }
      break;

   case at_logged:
      return;

   default:
      break;
   }

   va_start(argptr, szFmt);
   vprintf(szFmt, argptr);
   va_end(argptr);

   if(atype == at_error) { //we print to a log file, which we don't do
      exit(1001);
   }
}
