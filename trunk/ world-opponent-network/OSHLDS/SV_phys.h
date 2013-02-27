/* SV_Phys handles the specially named physics functions, and also handles
// movevars.  The reason for the merger is that the physics functions
// use a couple of the movevars.  This layer sits above sv_move().
*/

#ifndef OSHLDS_SV_PHYSICS_HEADER
#define OSHLDS_SV_PHYSICS_HEADER

#include "delta.h"
#include "edict.h"
#include "sizebuf.h"

void SV_Movevars_Init();

void SV_SetMoveVars();
void SV_WriteMovevarsToClient(sizebuf_t *);
void SV_QueryMovevarsChanged();

void SV_CheckVelocity(edict_t *);
void SV_AddGravity(edict_t *);
qboolean SV_CheckBottom(edict_t *);

void SV_Physics();
void SV_RunCmd(usercmd_t *, unsigned int);

typedef struct movevars_s { //SDK, HL has more of these than QW

   float gravity;           // Gravity for map
   float stopspeed;         // Deceleration when not moving
   float maxspeed;          // Max allowed speed
   float spectatormaxspeed;
   float accelerate;        // Acceleration factor
   float airaccelerate;     // Same for when in open air
   float wateraccelerate;   // Same for when in water
   float friction;
   float edgefriction;      // Extra friction near dropofs
   float waterfriction;     // Less in water
   float entgravity;        // 1.0
   float bounce;            // Wall bounce value. 1.0
   float stepsize;          // sv_stepsize;
   float maxvelocity;       // maximum server velocity.
   float zmax;              // Max z-buffer range (for GL)
   float waveHeight;        // Water wave height (for GL)
   qboolean footsteps;      //0x40 Play footstep sounds
   char  skyName[32];       // Name of the sky map
   float rollangle;         //0x64
   float rollspeed;
   float skycolor_r;        // Sky color
   float skycolor_g;        //
   float skycolor_b;        //
   float skyvec_x;          // Sky vector
   float skyvec_y;          //
   float skyvec_z;          //
} movevars_t;

extern movevars_t movevars;

#endif
