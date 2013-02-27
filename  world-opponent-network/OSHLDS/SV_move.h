/* The function SV_Move, and many of the functions directly underneath it.
// Sort've sits between sv_phys and world, though sv_move is called in many
// places.
*/

#ifndef OSHLDS_SV_MOVE_HEADER
#define OSHLDS_SV_MOVE_HEADER

#include "edict.h"
#include "endianin.h"
#include "Mod.h"


//structs
typedef struct trace_s { //sizeof 0x38

   qboolean   allsolid;     //if true, plane is not valid
   qboolean   startsolid;   // if true, the initial point was in a solid area
   qboolean   inopen, inwater;
   float      fraction;     // time completed, 1.0 = didn't hit anything
   vec3_t     endpos;       // final position
   plane_t    plane;        // 20 surface normal at impact.  Not its own struct in HL.
   edict_t    *pHit;        // 30, entity the surface is on
   int        hitgroup;     // 0 == generic, non zero is specific body part
} trace_t;
typedef struct TraceResult_s {

   int     fAllSolid;
   int     fStartSolid;
   int     fInOpen;
   int     fInWater;
   float   flFraction;
   vec3_t  vecEndPos;
   float   flPlaneDist;
   vec3_t  vecPlaneNormal;
   edict_t *pHit;
   int     iHitgroup;
} TraceResult_t;
typedef struct moveclip_s { //sizeof(0x84)

   vec3_t boxmins; //enclose the test object along entire move
   vec3_t boxmaxs; //0c
   vec_t * mins; // size of the moving object
   vec_t * maxs; //1c
   vec3_t mins2; // size when clipping against mosnters
   vec3_t maxs2; //2c
   vec_t * start; //38
   vec_t * end;   //3c
   trace_t trace; //40 confirmed

   short int type1; //78
   short int type2; //7A
   edict_t *passedict; //7c
   int hullnum; //80
} moveclip_t;


trace_t SV_Move(vec3_t, vec3_t, vec3_t, vec3_t, int, edict_t *, int);
trace_t * SV_ClipMoveToEntity(trace_t *, edict_t *, vec3_t, vec3_t, vec3_t, vec3_t);

void PF_WaterMove(edict_t *);
qboolean SV_movestep(edict_t *, vec3_t, qboolean);
qboolean SV_movetest(edict_t *, vec3_t, qboolean);
void SV_SetGlobalTrace(trace_t *);

HLDS_DLLEXPORT void SV_MoveToOrigin_I(edict_t *, const vec3_t, float, int);
trace_t * SV_Trace_Toss(trace_t *, edict_t *, edict_t *);

#endif
