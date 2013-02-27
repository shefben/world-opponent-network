/* 'Port' is intended to handle EVERYTHING that is needed between
// windows and unix.  Includes, decs, defines, and some code
// will go here.  If things get too hairy, I'll go back to the
// traditional IF/ELSE blocks in the code, but right now this
// is pretty good.
*/


#ifndef __MASTER__SERVER__PORTABILITY_H_
#define __MASTER__SERVER__PORTABILITY_H_

   //No reliance on compiler.  Define the right one, don't be stupid and do
   //something like define them all.
   #define __MASTER_SERVER_USING_WIN32
   //#define __MASTER_SERVER_USING_UNIX


   #ifdef __MASTER_SERVER_USING_WIN32

      #include <winsock.h>
      #define LIBCONFIG "libconfig101windows\libconfig.h"
      #define __OS__ "Windows"
      #define close   closesocket
      #define usleep  sleep
      #define sleep(x) Sleep(x*1000)

      #define DoWindowsNetThing(); \
         WSADATA wsa; \
         if ((WSAStartup(MAKEWORD(1, 1), &wsa)) != 0) { return 255; }

      #define PRINTERROR Printf(1, "Socket Error %i line %u file %s\n", WSAGetLastError(), __LINE__, __FILE__)
      typedef int socklen_t;

   #endif


   #ifdef __MASTER_SERVER_USING_UNIX

      #include <unistd.h>
      #include <errno.h>
      #include <sys/socket.h>
      #include <sys/types.h>
      #include <arpa/inet.h>
      #include <netdb.h>

      #define LIBCONFIG "libconfig101unix/libconfig.h"
      #define LIBOPTIONS "options/options.h"
      #define __OS__ "Linux"

      #define WSAGetLastError() errno
      #define WSAENETRESET ENETRESET
      #define PRINTERROR ptrIO->OutputWindowPrintf(1, "Socket Error %i\n", errno)
      #define WSACleanup();
      #define DoWindowsNetThing();
      #define INVALID_SOCKET -1
      #define SOCKET_ERROR -1  //Defined to different signs... Damn Microsoft.

   #endif

#endif
