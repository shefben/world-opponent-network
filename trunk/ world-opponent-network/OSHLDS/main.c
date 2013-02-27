#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <ctype.h>

#ifdef _MSC_VER
 #include <winsock2.h>
 #include <windows.h>
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#endif

#include "cmd.h"
#include "host.h"
#include "sys.h"

//global
int WeAreQuitting = 0;
pthread_mutex_t IsUsingEngine;

typedef void * (*lpfn_InputWhileLoop)(void *);
void InputWhileLoop(void) {

   unsigned int i;
   int x;
   char string[256];


   //Do I reallllly want curses?  Really really?
   i = 0;
   while(1) {

      x = getchar();
      if(x < 0) {
         continue; //I'm rather curious as to how we can fail on a blocking read to stdio
      }

      x = (char)x;
      if(isprint(x)) {
         string[i] = x;
         if(i < sizeof(string)-1) { i++; } //This line prevents overflows.
      }
      else { //returns, nulls, and weird stuff that doesn't belong in the buffer triggers this.
         string[i] = '\0';
         i = 0;

         //Before we add it, we MUST check to see if it's a quit message.
         //Since we block, it's... inconvenient to wait on Host_Quit_f.
         if(Q_strcasecmp("quit", string) == 0 || Q_strcasecmp("exit", string) == 0) {

            WeAreQuitting = 1;
            break;
         }

         //We cannot execute this command while the other thread is running.
         pthread_mutex_lock(&IsUsingEngine);
         Cmd_ExecuteString(string, 1);
         pthread_mutex_unlock(&IsUsingEngine);
      }
   }
}

typedef void * (*lpfn_ProgramWhileLoop)(void *);
void ProgramWhileLoop(void) {

   double time = 0;
   double oldtime = 0;
   int ex;


   while(WeAreQuitting == 0) {
      time = Sys_FloatTime();
      if(time != oldtime) {
         pthread_mutex_lock(&IsUsingEngine);
         Host_Frame(time - oldtime, 1, &ex);
         pthread_mutex_unlock(&IsUsingEngine);
         oldtime = time;
      }
      else {
         Sys_uSleep(0); //this releases our timeslice.
      }
   }

   Host_ShutdownServer(0);
   Sys_Quit();
}

int main(int argc, char ** argv) {

   int i;
   unsigned int total = 0;
   char args[0x100] = ""; //this needs to be upped a bit both here and in cmd_initargv
   char * ptr;

   pthread_t ThreadInputWhileLoop;
   pthread_t ThreadProgramWhileLoop;

   //We do this because the later functions expect it that way.  I know, I know.
   ptr = args;
   for(i = 1; i < argc; i++) {

      total += Q_snprintf(ptr, (sizeof(args) - 1) - total, "%s ", argv[i]);
      if(total >= sizeof(args) - 1) {

         printf("Main: Too many args.\n"); //reporting is not set up.
         break;
      }
      ptr = args + total;
   }

   //Replace "." with arg[0] later.
   if(Init(".", args) == 0) { return(1); }

   pthread_mutex_init(&IsUsingEngine, NULL);

   pthread_create(&ThreadInputWhileLoop, NULL, (lpfn_InputWhileLoop)InputWhileLoop, NULL);
   pthread_create(&ThreadProgramWhileLoop, NULL, (lpfn_ProgramWhileLoop)ProgramWhileLoop, NULL);

   /* Wait for our threads to quit...*/
   pthread_join(ThreadInputWhileLoop, NULL);
   pthread_join(ThreadProgramWhileLoop, NULL);

   pthread_mutex_destroy(&IsUsingEngine);

   //shutting down needs a little work.
   Shutdown();

   return(0);
}
