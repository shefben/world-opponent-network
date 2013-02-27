/* HOST functions are pretty much the highest level class of functions in the
// engine.  Only a few wrappers and init functions are above 'em.
*/

#ifndef OSHLDS_HOST_HEADER
#define OSHLDS_HOST_HEADER

#include "endianin.h"
#include "Q_memdef.h"
#include "common.h"

//vars
extern double host_frametime;
extern int host_hunklevel;

//functions
int  Host_Init(quakeparms_t *);
void Host_InitializeGameDLL();
int  Host_Frame(float, int, int *);
void Host_ClearMemory();
void Host_EndSection(const char *);
void Host_ShutdownServer(int);
void Host_Shutdown();
#endif
