/* This is where I'm throwing things related to sv_runcmd().  That function
// belongs here too, but is actually in sv_phys
*/

#ifndef OSHLDS_SV_USER_HEADER
#define OSHLDS_SV_USER_HEADER

#include "endianin.h"
#include "NET.h"
#include "SV_hlds.h"


void SV_GetTrueOrigin(int, vec3_t);
void SV_GetTrueMinMax(int, float **, float **);
void SV_CopyEdictToPhysent(physent_t *, int, edict_t *);
void SetMinMaxSize(edict_t *, const vec3_t, const vec3_t, int);

void SV_ExtractFromUserinfo(client_t *);

int SV_CalcPing(client_t *);
int SV_ShouldUpdatePing(client_t *);
void SV_GetNetInfo(client_t *, int *, int *);
void SV_ComputeLatency(client_t *);

void SV_PlayerRunPreThink(edict_t *, float);
int SV_PlayerRunThink(edict_t *, float, double);

void SV_SetupMove(client_t *);
void SV_RestoreMove(client_t *);


void SV_User_Init();


extern unsigned int SV_UPDATE_MASK;
extern unsigned int SV_UPDATE_BACKUP;

#endif
