#include "silencerversion.h"
#include "connectedclientclass.h"
#include "SilError.h"

#include "options/options.h"

/*** threading ***/

// This singular global boolean makes my life easier.  There, I said it.
static int boolIsTerminating = 0;


//These threads are many, and handle as many connections as physically possible.

typedef void* (*lpfn_SpawnedConnection15100)(void *);
void SpawnedConnection15100(void *_sd) {

   ConnectedClient OurBrandNewClient((unsigned int) _sd);
   OurBrandNewClient.ProcessClient15100();
   printf("%i disconnected (15100).\n", (unsigned int)_sd);
}
typedef void* (*lpfn_SpawnedConnection15101)(void *);
void SpawnedConnection15101(void *_sd) {

   ConnectedClient OurBrandNewClient((unsigned int) _sd);
   OurBrandNewClient.ProcessClient15101();
   printf("%i disconnected (15101).\n", (unsigned int)_sd);
}

typedef void* (*lpfn_ListeningWhileLoop15100)(void *);
void ListeningWhileLoop15100(void *_ld) {

   unsigned int NewConnection;
   pthread_attr_t detatchedAttr;
   pthread_t ThreadSpawnedConnection;
   sockaddr_in TheConnectingGuy;


   if(pthread_attr_init(&detatchedAttr) != 0 ||
      pthread_attr_setdetachstate(&detatchedAttr, PTHREAD_CREATE_DETACHED) != 0) {

      printf(SIL_THREADATTRERROR);
   }

   int SizeofSockaddr = sizeof(struct sockaddr_in);

   while(boolIsTerminating == 0) {
      NewConnection = accept(((unsigned int)_ld), (struct sockaddr*)&TheConnectingGuy, &SizeofSockaddr);
      if(NewConnection != INVALID_SOCKET) {

         printf("User %u.%u.%u.%u:%u connected to port 15100\n", TheConnectingGuy.sin_addr.S_un.S_un_b.s_b1, TheConnectingGuy.sin_addr.S_un.S_un_b.s_b2, TheConnectingGuy.sin_addr.S_un.S_un_b.s_b3, TheConnectingGuy.sin_addr.S_un.S_un_b.s_b4, TheConnectingGuy.sin_port);
         if(pthread_create(&ThreadSpawnedConnection, &detatchedAttr, ((lpfn_SpawnedConnection15100)SpawnedConnection15100), (void *)NewConnection) != 0) {
            printf(SIL_THREADSPAWNERROR2);
         }
      }
   }
   pthread_attr_destroy(&detatchedAttr);
}
typedef void* (*lpfn_ListeningWhileLoop15101)(void *);
void ListeningWhileLoop15101(void *_ld) {

   unsigned int NewConnection;
   pthread_attr_t detatchedAttr;
   pthread_t ThreadSpawnedConnection;
   sockaddr_in TheConnectingGuy;

   if(pthread_attr_init(&detatchedAttr) != 0 ||
      pthread_attr_setdetachstate(&detatchedAttr, PTHREAD_CREATE_DETACHED) != 0) {

      printf(SIL_THREADATTRERROR);
   }

   int SizeofSockaddr = sizeof(struct sockaddr_in);

   while(boolIsTerminating == 0) {

      /* If you don't get it, this whole function basically just listens on
      // a socket endlessly and spawns new threads for each incoming connection
      // Same for the one above it.
      */

      NewConnection = accept(((unsigned int)_ld), (struct sockaddr*)&TheConnectingGuy, &SizeofSockaddr);
      if(NewConnection != INVALID_SOCKET) {

         printf("User %u.%u.%u.%u:%u connected to port 15101\n", TheConnectingGuy.sin_addr.S_un.S_un_b.s_b1, TheConnectingGuy.sin_addr.S_un.S_un_b.s_b2, TheConnectingGuy.sin_addr.S_un.S_un_b.s_b3, TheConnectingGuy.sin_addr.S_un.S_un_b.s_b4, TheConnectingGuy.sin_port);
         if(pthread_create(&ThreadSpawnedConnection, &detatchedAttr, ((lpfn_SpawnedConnection15101)SpawnedConnection15101), (void *)NewConnection) != 0) {
            printf(SIL_THREADSPAWNERROR2);
         }
      }
   }
   pthread_attr_destroy(&detatchedAttr);
}


//Configuration

/* DESCRIPTION: ProcessCommandlineInput
//
// This function takes care of all the command line stuff.
//
// Returns the port on success
// Returns -1 if the program was not given a valid IP/port
// Returns 0 if we need to leave for some other reason
*/
int ProcessCommandlineInput(int argc, char **argv) {

//All the boring code that was here has been replaced with a small
//library called options that lovingly takes the tedium out of my hands.

   const char * const optv[] = {
      "p:port <number>",
      "P: ",
      "i:ip <number>",
      "I: ",
      "h|help",
      "H| ",
      "v|version",
      "V| ",
      "?| ",
      NULL
   };

   int IP = 0;
   int Port = 15100;

   char argumentswitch;
   const char *optionalargument = NULL;
   class Options opts(*argv, optv);
   class OptArgvIter iter(--argc, ++argv);

   opts.ctrls(Options::PLUS); //just an enumerated four

   //I can honestly say I don't know what this statement does.  I mean, I guess
   //I could look in a debugger and find out, but I don't think anyone out
   //there truly knows how class inheritance works.  Somehow it does though.
   for(argumentswitch = opts(iter, optionalargument); argumentswitch != 0; argumentswitch = opts(iter, optionalargument)) {

      switch(argumentswitch) {

      case 'h':
      case 'H':
      case '?':
         printf("Usage: *name* [-options]\n"
                "-i=<#>, --ip=<#>   | *Set IP clients should connect to (required)\n"
                "-p=<#>, --port=<#> | *Set port clients should use (default=15100)\n"
                "-h -H, --help      | *This crazy gibberish (exit afterwards)\n"
                "-v -V, --version   | *Display version info (exit afterwards)\n\n"
                "This program simulates two servers.  The first server is ALWAYS\n"
                "running on port 15101, and is hardcoded into the Silencer client.\n"
                "The second server's address and port is passed to the client by\n"
                "the first server.  That is what you must specify on the commandline.\n\n");
      case 'v':
      case 'V':
         printf("Silencer Routing Server %s--%s for %s.\nCurrent maintainer: rguest/aady www.steamlessproject.nl/silencer.arsia-mons.com\n", __DATE__, __TIME__, __OUR__OS__);
         return(0);
      case 'p':
      case 'P':
         if(optionalargument == NULL) { return(-1); }

         Port = atoi(optionalargument);
         if(Port < 0 || Port > 65536) {

            printf("Error: the passed value \"%u\" is not a valid port.\n", Port);
            return(-1);
         }
         break;
      case 'i':
      case 'I':
         if(optionalargument == NULL) { return(-1); }

         IP = inet_addr(optionalargument);
         break;
      }
   }

   if(IP == 0) { return(-1); }

   //Adjust our premade packets accordingly...
   ActiveGameServers.ChangeServerNetworkAddresses(IP, (UINT16)Port);
   return(Port);
}



/*** MAIN ***/

int main(int argc, char* argv[]) {

   pthread_t ListeningThread15100;
   pthread_t ListeningThread15101;

   int Port15100;

   unsigned int ListeningSocket15100;
   unsigned int ListeningSocket15101;
   struct sockaddr_in ServerAddress15100;
   struct sockaddr_in ServerAddress15101;

   //Handle commandline

   Port15100 = ProcessCommandlineInput(argc, argv);
   if(Port15100 == -1) {

      printf("You need to pass the IP that we'll be telling the clients to connect to.  I don't know of a reliable way to get that information locally.\n\n"
             "Example (say your computer's configured to be at address 10.0.1.10:15100)\n"
             " RServer.exe --ip 10.0.1.10 --port 15100");
      return(1);
   }
   else if(Port15100 == 0) {
      return(0);
   }

   // start winsock if on Wandos
   #ifdef __SMS_USING_WIN32
   WSADATA wsa;
   if ((WSAStartup(MAKEWORD(1, 1), &wsa)) != 0) {
      printf("Failed on WSAStartup.  Error code %i\n", WSAGetLastError());
      return(255); //that's bad.
   }
   #endif

   memset(&ServerAddress15100, 0, sizeof(ServerAddress15100));
   ServerAddress15100.sin_family = AF_INET;
   ServerAddress15100.sin_port = htons(Port15100);
   ServerAddress15100.sin_addr.s_addr=INADDR_ANY;
   memset(&ServerAddress15101, 0, sizeof(ServerAddress15101));
   ServerAddress15101.sin_family = AF_INET;
   ServerAddress15101.sin_port = htons(15101);
   ServerAddress15101.sin_addr.s_addr=INADDR_ANY;

   ListeningSocket15100 = socket(AF_INET, SOCK_STREAM, 0);
   ListeningSocket15101 = socket(AF_INET, SOCK_STREAM, 0);

   if(ListeningSocket15100 == INVALID_SOCKET || ListeningSocket15101 == INVALID_SOCKET ||
      bind(ListeningSocket15100, (struct sockaddr*)&ServerAddress15100, sizeof(sockaddr_in)) == SOCKET_ERROR ||
      bind(ListeningSocket15101, (struct sockaddr*)&ServerAddress15101, sizeof(sockaddr_in)) == SOCKET_ERROR ||
      listen(ListeningSocket15100, SOMAXCONN) == SOCKET_ERROR ||
      listen(ListeningSocket15101, SOMAXCONN) == SOCKET_ERROR) {

      printf(SIL_SOCKETERROR);
      WSACleanup();
      return(254);
   }


   printf("Waiting for connections.\n");

   if(pthread_create(&ListeningThread15100, NULL, ((lpfn_ListeningWhileLoop15100)ListeningWhileLoop15100), (void *)ListeningSocket15100) == 0 &&
      pthread_create(&ListeningThread15101, NULL, ((lpfn_ListeningWhileLoop15101)ListeningWhileLoop15101), (void *)ListeningSocket15101) == 0) {

      //Spawned and all that rot.
      char c;
      do { //we'll make this curses later.  Much later.
         c = getchar();

      } while(c != 'q');
   }
   else {
      printf(SIL_THREADSPAWNERROR);
   }

   boolIsTerminating = 1;

   /* Okay, so we're quitting.  How do we make all of our blocking threads
   // terminate?  We could just kill the program and let the OS or whatever
   // handle it, but that would be evil.
   // We can also close our interface to winsock, which will make all the
   // threads return an error, probably whatever WSA means 'winsock not
   // initialized'.  That too is evil, and only works on Windows.
   // Lastly, provided we keep track of each open socket someplace globally
   // accessible, we can close each of our sockets (haven't tried it though).
   // This too is probably evil, but it's the least evil of the three.
   //
   // I really don't like select().
   */

   WSACleanup(); //I choose the midlevel evilness for now.

   pthread_join(ListeningThread15100, NULL);
   pthread_join(ListeningThread15101, NULL);

   return(0);
}

