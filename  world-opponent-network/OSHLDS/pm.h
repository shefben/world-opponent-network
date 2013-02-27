/* This is messy.  I don't think much of it needs to be implemented, but if
// it does, it is both in the SDK as actual MOD functions, and in world.c,
// as slightly different versions.
*/
#ifndef OSHLDS_PM_HEADER
#define OSHLDS_PM_HEADER

#include "edict.h"
#include "SV_move.h"


typedef struct pmtrace_s { //44 another trace struct that's a little bit different from others. SDK.
   qboolean allsolid;         // 00 if true, plane is not valid
   qboolean startsolid;       // 04 if true, the initial point was in a solid area
   qboolean inopen, inwater;  // End point is in empty space or in water
   float    fraction;         // /10 time completed, 1.0 = didn't hit anything
   vec3_t   endpos;           // 14 final position
   plane_t  plane;            // 20 surface normal at impact
   int      ent;              // 30 entity at impact
   vec3_t   deltavelocity;    // 34 Change in player's velocity caused by impact.
                              // Only run on server.
   int      hitgroup;         // 40
} pmtrace_t;

void our_PM_Init();
void SV_ConvertPMTrace(trace_t *, pmtrace_t *, edict_t *);
#endif
