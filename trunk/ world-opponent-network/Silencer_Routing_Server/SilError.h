#ifndef __WE_HAVE_ALREADY_INCLUDED_SILERROR_H
#define __WE_HAVE_ALREADY_INCLUDED_SILERROR_H

//All this does is make it so that error messages can be easily adjusted.

#define SIL_THREADATTRERROR   "Failed to init thread attributes.  Thread not spawned.\n"
#define SIL_SOCKETERROR       "Failed when creating sockets.  Error code %i\n", WSAGetLastError()
#define SIL_THREADSPAWNERROR  "Failed to spawn a thread, for some reason...\n"
#define SIL_THREADSPAWNERROR2 "Failed to spawn a thread for connecting client.  Server might be past capacity.\n"
#define SIL_NOMEMERROR        "Memory allocation failure.  Buy more RAM, cheapskate.\n"
#define SIL_PASSEDNULL        "ERROR in %s.  Passed NULL address.\n", __FUNCTION__
#define SIL_NOTSILENCER       "Heartbeat rejected for improper formatting.\n"
#endif
