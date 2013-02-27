/*

Simply a place to dump sys_error and Printf until they need more room.
Also added my own #define.
*/

#ifndef HLDS_LINUX_REPORT_SO
#define HLDS_LINUX_REPORT_SO

#include "gccops.h"
#include "ModCalls.h"

#define EXIT() Sys_Error("Function %s exited, file %s line %u\n", __FUNCTION__, __FILE__, __LINE__)
#define CHECK_MEMORY_MALLOC(x) if(x == NULL) { Sys_Error("%s: malloc failure.", __FUNCTION__); }
#if(defined(_MSC_VER) || defined(__BORLANDC__)) //Neither Borland nor MSVC6 support this (#ifdef doesn't work on __FUNCTION__)
 #define __FUNCTION__ "__FUNCTION__ not defined"
#endif


typedef enum {
   print_console,
   print_center,
   print_chat,
} PRINT_TYPE;
typedef enum {
   at_notice,
   at_console,      // same as at_notice, but forces a ConPrintf, not a message box
   at_aiconsole,   // same as at_console, but only shown if developer level is 2!
   at_warning,
   at_error,
   at_logged      // Server print to console ( only in multiplayer games ).
} ALERT_TYPE;

void Con_Init();
void __noreturn Sys_Error(const char *format, ...);
void Sys_Warning(const char *format, ...);
HLDS_DLLEXPORT void Con_Printf(const char *format, ...);
HLDS_DLLEXPORT void Con_DPrintf(const char *format, ...);
void Function(int);


HLDS_DLLEXPORT void AlertMessage(ALERT_TYPE atype, char *szFmt, ...);

#endif
