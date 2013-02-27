/* SV_Packet stuff, sitting above the NET stuff.  This isn't nearly as bad
// as sv_hlds was, but it's corrently the most unorganized file around.
*/

#ifndef OSHLDS_SV_PACKET_HEADER
#define OSHLDS_SV_PACKET_HEADER

#include "common.h"
#include "edict.h"
#include "ModCalls.h"
#include "NET.h"
#include "report.h"

void SV_ReadPackets();

//outgoing
void SV_SendBan();
void SV_BuildReconnect(sizebuf_t *);

HLDS_DLLEXPORT void ClientPrintf(edict_t *, PRINT_TYPE, const char *);
void SV_SendClientMessages();
void SV_StartParticle(const vec3_t, const vec3_t, int, int);
void SV_CreateBaseline();
void SV_SendUserReg(sizebuf_t *);
void SV_Packet_Init();
void SV_WriteSpawn(sizebuf_t *);
void SV_SendServerinfo(sizebuf_t *, client_t *);

void SV_Packet_Init();
#endif
