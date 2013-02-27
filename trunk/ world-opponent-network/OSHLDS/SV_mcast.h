/* So how does a frame go?  Usually somewhere along the lines of
// GetPacket->ProcessPacket->MoveEveryone->Broadcasthanges.  That fourth step
// is what these functions are for.
//
// An odd quirk is the large number of sould messages.  The reasoning is those
// are usually called in the "BroadcastThingies" stage, and don't fit well
// when placed in a "ProcessPacket" file.
*/

#ifndef OSHLDS_SV_MULTICAST_HEADER
#define OSHLDS_SV_MULTICAST_HEADER

#include "sizebuf.h"
#include "edict.h"
#include "endianin.h"
#include "ModCalls.h"
#include "NET.h"

void SV_Multicast(edict_t *, vec3_t, int, int);
int SV_BuildSoundMsg(edict_t *, int, const char *, int, float, int, int, const vec3_t, sizebuf_t *);
void SV_StartSound(int, edict_t *, int, const char *, int, float, int, int);

HLDS_DLLEXPORT void PM_SV_PlaySound(int, const char *, float, float, int, int);
HLDS_DLLEXPORT void PM_SV_PlaybackEventFull(int, int, unsigned short, float, vec3_t, vec3_t, float, float, int, int, int, int);
HLDS_DLLEXPORT void EV_Playback(int, edict_t *, unsigned short, float, vec3_t, vec3_t, float, float, int, int, int, int);

void SV_BroadcastCommand(const char * format, ...);
void SV_BroadcastPrintf(const char * format, ...);
void SV_ClientPrintf(const char * format, ...);
void SV_DropClient(client_t *, int, char *, ...);

void SV_FullClientUpdate(client_t *, sizebuf_t *);
void SV_ForceFullClientsUpdate();

void SV_RejectConnection(netadr_t *, char *, ...);
void SV_RejectConnectionForPassword(netadr_t *);

#endif
