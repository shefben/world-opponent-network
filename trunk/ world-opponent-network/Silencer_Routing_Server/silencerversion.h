#ifndef __WE_HAVE_ALREADY_INCLUDED_SILENCERVERSION
#define __WE_HAVE_ALREADY_INCLUDED_SILENCERVERSION

/*** General compiletime settings ***/

#define SMS_PARANOID 1


/***  OS identification ***/

#if defined(__windows__) || defined(__win32__) || defined(__WIN32__) || defined(__WINDOWS__)
 #define __SMS_USING_WIN32
 #define __OUR__OS__ "Windows"

#elif defined(__linux__) || defined(__unix__) || defined(__LINUX__) || defined(__UNIX__)
 #define __SMS_USING_LINUX
 #define __OUR__OS__ "Unix"

#else
 #Warning "Didn't find any known default OS defines.  You might need to manually specify it (and tell me what they are so I can add 'em)"

#endif

//OS define sanity checking
#if (!defined(__SMS_USING_WIN32) && !defined(__SMS_USING_LINUX))

   /* manual deciding */
   //#define __SMS_USING_WIN32
   //#define __SMS_USING_LINUX

  #if (!defined(__SMS_USING_WIN32) && !defined(__SMS_USING_LINUX)))
    #error "You need to define an OS around line 25 of this file."
  #endif
#endif

#if (defined(__SMS_USING_WIN32) && defined(__SMS_USING_LINUX))
  #error "BOTH windows and unix defined.  That's not right.  Fix it."
#endif


//And now the specific things:

#include <pthread.h>
#ifdef __SMS_USING_WIN32

 #include <winsock.h>
 typedef int socklen_t;
 #define usleep  sleep
 #define sleep(x) Sleep(x*1000)

#endif
#ifdef __SMS_USING_LINUX

 #include <unistd.h>
 #include <errno.h>
 #include <sys/socket.h>
 #include <sys/types.h>
 #include <arpa/inet.h>
 #include <netdb.h>

 #define WSACleanup();
 #define WSAGetLastError() errno

#endif

#include <iostream>
#include "SrvrNtwk.h"


#define FakeServers \
  "\x00\x00" \
  "\x00" \
  "\x00" \
  "\x1D" \
  "\x00\x00" \
  "\x01\x00" \
  "\x00\x00" \
  "\x00\x00" \
  "\x00" \
  "\x01" \
  "\x88\x00" \
  "\x6e\x0\x9\x0\x0\x8\x6\x0\x0\x1\x0\x0\x1e\x0\x0\x0\x19\x0\x0\x0\x17\x7b\x91\x7c\xce\xa9\xfd\x58\x34\x1e\x53\x54\x41\x52\x37\x32\x2e\x53\x49\x4c\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x43\x6f\x6e\x66\x6c\x69\x63\x74\x21\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x52\x69\x63\x6b\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
//This last line, the one beginning with 6e, is a game 'heartbeat' for all practical purposes.  They're only good for one game.  This one was extracted from a game Rick played on 09.16.2007 at 0430 hours.  It will stay here.



#define MAX_SEND_SIZE 512  //at present the limits are not yet known.
#define MAX_RECV_SIZE 8192 //Be conservative in what we send, bla bla bla

#if MAX_SEND_SIZE < 100
#error "MAX_SEND_SIZE is too small.  You WILL crash the program doing that."
#endif

/*** sockets ***/

extern class ServerList ActiveGameServers;
#endif

