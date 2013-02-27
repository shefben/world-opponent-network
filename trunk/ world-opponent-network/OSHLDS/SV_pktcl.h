/* Connectionless packets go here.
*/

#ifndef OSHLDS_SV_PACKET_CONNECTIONLESS_HEADER
#define OSHLDS_SV_PACKET_CONNECTIONLESS_HEADER

#include "NET.h"

int SV_GetFragmentSize(client_t *);
char * SV_GetClientIDString(client_t *);
void SV_ConnectionlessPacket();

void SV_ResetModInfo();
void SV_SetUsingMod();
void SV_LoadModDataFromFile(const char *, const char *);

void SV_Packet_Connectionless_Init();
void SV_ConnectionlessPacket_Init();
#endif
