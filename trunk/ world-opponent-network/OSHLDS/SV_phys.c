#include "SV_phys.h"

#include "messages.h"
#include "host.h"
#include "Modding.h"
#include "NET.h"
#include "Q_memdef.h"
#include "report.h"
#include "SV_mcast.h"
#include "SV_move.h"
#include "SV_user.h"
#include "sys.h"
#include "world.h"
#include "VecMath.h"

/*** GLOBALS ***/
movevars_t movevars;

static cvar_t cvar_sv_gravity           = {           "sv_gravity",    "800", FCVAR_SERVER,  800.0, NULL };
static cvar_t cvar_sv_stopspeed         = {         "sv_stopspeed",    "100", FCVAR_SERVER,  100.0, NULL };
static cvar_t cvar_sv_maxspeed          = {          "sv_maxspeed",    "320", FCVAR_SERVER,  320.0, NULL };
static cvar_t cvar_sv_spectatormaxspeed = { "sv_spectatormaxspeed",    "500", FCVAR_NONE,    500.0, NULL };
static cvar_t cvar_sv_accelerate        = {        "sv_accelerate",     "10", FCVAR_SERVER,   10.0, NULL };
static cvar_t cvar_sv_airaccelerate     = {     "sv_airaccelerate",     "10", FCVAR_SERVER,   10.0, NULL };
static cvar_t cvar_sv_wateraccelerate   = {   "sv_wateraccelerate",     "10", FCVAR_SERVER,   10.0, NULL };
static cvar_t cvar_sv_friction          = {          "sv_friction",      "4", FCVAR_SERVER,    4.0, NULL };
static cvar_t cvar_sv_edgefriction      = {         "edgefriction",      "2", FCVAR_SERVER,    2.0, NULL };
static cvar_t cvar_sv_waterfriction     = {     "sv_waterfriction",      "1", FCVAR_SERVER,    1.0, NULL };
static cvar_t cvar_sv_entgravity        = {        "sv_entgravity",      "1", FCVAR_NONE,      1.0, NULL };
static cvar_t cvar_sv_bounce            = {            "sv_bounce",      "1", FCVAR_SERVER,    1.0, NULL };
       cvar_t cvar_sv_stepsize          = {          "sv_stepsize",     "18", FCVAR_SERVER,   18.0, NULL };
static cvar_t cvar_sv_maxvelocity       = {       "sv_maxvelocity",   "2000", FCVAR_NONE,   2000.0, NULL };
static cvar_t cvar_sv_zmax              = {              "sv_zmax",   "4096", FCVAR_SPONLY, 4096.0, NULL };
static cvar_t cvar_sv_wateramp          = {          "sv_wateramp",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_footsteps         = {         "mp_footsteps",      "1", FCVAR_SERVER,    1.0, NULL };

static cvar_t cvar_sv_rollangle         = {         "sv_rollangle",      "2", FCVAR_NONE,      2.0, NULL };
static cvar_t cvar_sv_rollspeed         = {         "sv_rollspeed",    "200", FCVAR_NONE,    200.0, NULL };
static cvar_t cvar_sv_skycolor_r        = {        "sv_skycolor_r",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_skycolor_g        = {        "sv_skycolor_g",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_skycolor_b        = {        "sv_skycolor_b",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_skyvec_x          = {          "sv_skyvec_x",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_skyvec_y          = {          "sv_skyvec_y",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_skyvec_z          = {          "sv_skyvec_z",      "0", FCVAR_NONE,      0.0, NULL };
static cvar_t cvar_sv_skyname           = {           "sv_skyname", "desert", FCVAR_NONE,        0, NULL };

#ifndef isnan
 #define isnan(x) ((x) != (x))
 #if(!defined(_MSC_VER) && !defined(__BORLANDC__))
  #warning using manual "isnan" define.
 #endif
#endif



/*** Basic physics engine stuff ***/

/* DESCRIPTION: SV_CheckVelocity
// LOCATION: SDK and sv_phys.c
// PATH: Lots
//
// Does some sanity checks on ent velocities, including my favorite, the NaN
// floating point check.  QW prefers to check the bits to see if the float is
// a NaN.  I prefer using the standard library :) (unless it's not available)
//
// This function is one of the most often called functions around.
// It might benefit from unrolling that little FOR loop, though I'd
// expect the compiler can do that.  It is also the first function
// I removed calls to VariableValue to; it shouldn't have a runtime of 6 * N
*/
void SV_CheckVelocity(edict_t *ent) {

   int i;

   for(i = 0; i < 3; i++) {

      // See if it's bogus.
      if(isnan(ent->v.velocity[i])) {
         Con_Printf("Got a NaN velocity %i\n", i);
         ent->v.velocity[i] = 0;
      }
      if(isnan(ent->v.origin[i])) {
         Con_Printf("Got a NaN origin on %i\n", i);
         ent->v.origin[i] = 0;
      }

      // Bound it.
      if(ent->v.velocity[i] > cvar_sv_maxvelocity.value) {
         Con_Printf("Got a velocity too high on %i\n", i);
         ent->v.velocity[i] = cvar_sv_maxvelocity.value;
      }
      else if(ent->v.velocity[i] < -cvar_sv_maxvelocity.value) {
         Con_Printf("Got a velocity too low on %i\n", i);
         ent->v.velocity[i] = -cvar_sv_maxvelocity.value;
      }
   }
}
/* DESCRIPTION: ClipVelocity
// LOCATION: SDK and sv_phys.c--global_sv_phys contains different, more interesting version.
// PATH: SV movement, throwing
//
// Listed as responsible for calculating an object's sliding speed, or perhaps
// bouncing speed--the description wasn't very long.
*/
int ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce) {

   float backoff;
   float change;
   float angle;
   int i, blocked;

   angle = normal[2];
   blocked = 0;


   if(angle > 0) { blocked |= 0x01; }
   if(angle == 0) { blocked |= 0x02; }


   backoff = ((in[0] * normal[0]) +
              (in[1] * normal[1]) +
              (in[2] * normal[2]) * overbounce);

   for(i = 0; i < 3; i++) {

      change = normal[i] * backoff;
      out[i] = in[i] - change;

      if(out[i] > -1.0 && out[i] < 1.0) {
         out[i] = 0;
      }
   }

   return(blocked);
}
/* DESCRIPTION: SV_AddGravity
// LOCATION: SDK and sv_phys.c
// PATH: Four physics trace functions.
//
// Helps to ensure objects fall at a uniform 9.8 meters per second squared.
*/
void SV_AddGravity(edict_t *ent) {

   float scale;

   if(ent->v.gravity == 0) {
      scale = 1.0;
   }
   else {
      scale = ent->v.gravity;
   }
   ent->v.velocity[2] -= scale * cvar_sv_gravity.value * host_frametime;
   ent->v.velocity[2] += ent->v.basevelocity[2] * host_frametime;
   ent->v.basevelocity[2] = 0;

   SV_CheckVelocity(ent);
}
/* DESCRIPTION: SV_Impact
// LOCATION: Don't remember, was pretty inaccurate anyway.
// PATH: Physics, movement.
//
// No, it doesn't test for collisions.  What it DOES do is, once a collision
// has been detected, it calls upon the appropriate mod function to take care
// of things (and does some collision validation).
*/
void SV_Impact(edict_t *e1, edict_t *e2, trace_t * tr) {

   gGlobalVariables.time = global_sv.time0c;

   if(((e1->v.flags | e2->v.flags) & FL_SPECTATOR)) { return; }

   if(e1->v.groupinfo != 0 && e2->v.groupinfo) {
      if((global_g_groupop == 0 && (e1->v.groupinfo & e2->v.groupinfo) == 0) ||
         (global_g_groupop == 1 && (e1->v.groupinfo & e2->v.groupinfo) != 0)) {
         return;
      }
   }
   //That's the fourth time I've seen that code block.  What does groupinfo do?

   if(e1->v.solid != SOLID_NOT) {

      SV_SetGlobalTrace(tr);
      gEntityInterface.pfnTouch(e1, e2);
   }
   if(e2->v.solid != SOLID_NOT) {

      SV_SetGlobalTrace(tr);
      gEntityInterface.pfnTouch(e2, e1);
   }
}
/* DESCRIPTION: SV_CheckBottom
// LOCATION: global_sv_move.c
// PATH: Numerous
//
// Gravity is a harsh mistress, and this function analyzes our choice of
// footing.  The initial check is simple--if all four corners are on solid
// ground, so are we.  Beyond that we get into more compicated techniques
// to determine where we stand.
//
// QW version included Hull preservation.  Don't see any reason why we
// shouldn't.
//
// Oh, and there were also unreferenced variables c_yes and c_no.  Ignored 'em.
*/
qboolean SV_CheckBottom(edict_t *ent) {

   static vec3_t origin = { 0, 0, 0 };

   int var_7c;
   vec3_t mins, maxs;
   vec3_t start, stop;
   int x, y;
   float Stepsize;
   trace_t var_68_trace;
   float mid, bottom;

   if((ent->v.flags & FL_MONSTERCLIP) != 0) { var_7c = 1; }
   else { var_7c = 0; }

   VectorAdd(ent->v.origin, ent->v.mins, mins);
   VectorAdd(ent->v.origin, ent->v.maxs, maxs);

   start[2] = mins[2] - 1.0;

   for(x = 0;; x++) {

      if(x > 1) { return(1); }

      for(y = 0; y <= 1; y++) {

         if(x != 0) { start[0] = maxs[0]; }
         else { start[0] = mins[0]; }

         if(y != 0) { start[1] = maxs[1]; }
         else { start[1] = mins[1]; }

         global_g_groupmask = ent->v.groupinfo;

         if(SV_PointContents(start) != Q1CONTENTS_SOLID) {
            break;
         }
      }
   }

   Stepsize = cvar_sv_stepsize.value;

   //If we are here, we did NOT return successfully and have to do the hard checks.

   start[2]           =  mins[2] + Stepsize;
   start[0] = stop[0] = (mins[0] + maxs[0])*0.5;
   start[1] = stop[1] = (mins[1] + maxs[1])*0.5;
              stop[2] = start[2] - (2 * Stepsize);

   //savedhull = ent->v.hull;
   //ent->v.hull = 0;
   var_68_trace = SV_Move(start, origin, origin, stop, 1, ent, var_7c);
   //ent->v.hull = savedhull;
   if(var_68_trace.fraction == 1.0) { return(0); } //completed move.
   mid = bottom = var_68_trace.endpos[2];

   for(x = 0; x <= 1; x++) {
      for(y = 0; y <= 1; y++) {

         if(x != 0) { start[0] = stop[0] = maxs[0]; }
         else { start[0] = stop[0] = mins[0]; }

         if(y != 0) { start[1] = stop[1] = maxs[1]; }
         else { start[1] = stop[1] = mins[1]; }


         //savedhull = ent->v.hull;
         //ent->v.hull = 0;
         var_68_trace = SV_Move(start, origin, origin, stop, 1, ent, var_7c);
         //ent->v.hull = savedhull;

         if(var_68_trace.fraction != 1.0 && var_68_trace.endpos[2] > bottom) {
            bottom = var_68_trace.endpos[2];
         }
         if(var_68_trace.fraction == 1.0 || mid - var_68_trace.endpos[2] > Stepsize) {
            return(0);
         }
      }
   }

   return(1);
}
/* DESCRIPTION: SV_CheckMovingGround
// PATH: SV_RunCmd
//
// This checks to see if our ent is standing on anything that's moving.
// If it is, we have basevelocity problems to hammer out.
*/
void SV_CheckMovingGround(edict_t * cl, float frametime) {

   edict_t * var_4_ground;


   if((cl->v.flags & FL_ONGROUND)) { //If it's not on the ground, we obviously don't need to check it.

      var_4_ground = cl->v.groundentity;
      if(var_4_ground == NULL) { //That's bad.  We CAN ignore this, but it might bite us eventually.
         //Was error, existence 1.5 improperly sets this.
         Sys_Warning("%s: Entity \"%s\" claims to be on ground, has no ground ent.\n", __FUNCTION__, global_pr_strings + cl->v.classname);
         cl->v.flags &= ~FL_ONGROUND;
      }
      else if(var_4_ground->v.flags & FL_CONVEYOR) {
         if(cl->v.flags & FL_BASEVELOCITY) {
            VectorMA(cl->v.basevelocity, var_4_ground->v.speed, var_4_ground->v.movedir, cl->v.basevelocity);
         }
         else {
            VectorScale(var_4_ground->v.movedir, var_4_ground->v.speed, cl->v.basevelocity);
            cl->v.flags |= FL_BASEVELOCITY;
         }
      }
   }


   if(cl->v.flags & FL_BASEVELOCITY) { //I'm not *quite* sure why we're doing this.  I think this function simply doubles as the frame's basevelocity cleanup.
      cl->v.flags &= ~FL_BASEVELOCITY;
   }
   else {

      VectorMA(cl->v.velocity, 1 + (frametime * .5), cl->v.basevelocity, cl->v.velocity);
      cl->v.basevelocity[0] = 0;
      cl->v.basevelocity[1] = 0;
      cl->v.basevelocity[2] = 0;
   }
}

/* DESCRIPTION: SV_TestEntityPosition
// LOCATION: world.c
// PATH: Movement functions
//
// Described as a wrapper for SV_BoxInSolidEntity that never clips against
// the passed ent.  So I guess a 'move' that doesn't hit the given ent.
*/
edict_t * SV_TestEntityPosition(edict_t *ent) {

   trace_t trace;
   int var_3c;

   //we could optimize this away...
   if(ent->v.flags & FL_MONSTERCLIP) { var_3c = 1; }
   else { var_3c = 0; }

   trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, 0, ent, var_3c);

   if(trace.startsolid == 0) {
      return(NULL);
   }

   SV_SetGlobalTrace(&trace);
   return(trace.pHit);
}
/* DESCRIPTION: SV_RunThink
// LOCATION: sv_phys.c
// PATH: SV_Physics_(insertfunctionhere)
//
// Does thinking code, if there's enough time.  QW's version is more
// complicated, but the basic rules are probably the same--not for
// pushable objects, returns 0 if the entity was freed, etc.
*/
qboolean SV_RunThink(edict_t *ent) {

   double thinktime;

   if((ent->v.flags & FL_SPECTATOR) == 0) {

      thinktime = ent->v.nextthink;
      if(thinktime <= 0 || thinktime > global_sv.time0c + host_frametime) {
         return(1);
      }
      if(thinktime < global_sv.time0c) {
         thinktime = global_sv.time0c;
      }
      ent->v.nextthink = 0;

      gGlobalVariables.time = thinktime;
      gEntityInterface.pfnThink(ent);
   }
   if((ent->v.flags & FL_SPECTATOR) != 0) { //should be else?  Or would pfnThink change it?
      ED_Free(ent);
   }

   if(ent->free != 0) { return(0); }
   else { return(1); }
}



/*** Water ***/

/* DESCRIPTION: SV_CheckWaterTransition
// LOCATION: sv_phys.c
// PATH: SV movement, throwing
//
// Go into the water.  Live there.  Play bubble sound effects there.
// waterlevel 0 - not in water
// waterlevel 1 - feet in water
// waterlevel 2 - waist in water
// waterlevel 3 - head in water
*/
void SV_CheckWaterTransition(edict_t *ent) {

   int cont;
   vec3_t halfmax;

   halfmax[0] = (ent->v.absmax[0] + ent->v.absmin[0]) * .5;
   halfmax[1] = (ent->v.absmax[1] + ent->v.absmin[1]) * .5;
   halfmax[2] =  ent->v.absmin[2] + 1;

   global_g_groupmask = ent->v.groupinfo;
   cont = SV_PointContents(halfmax);

   if(ent->v.watertype == 0) {

      ent->v.watertype = cont;
      ent->v.waterlevel = 1;
      return;
   }

   if(cont > -3 || cont <= -15) {
      if (ent->v.watertype != -1) {
         SV_StartSound(0, ent, 0, "player/pl_wade2.wav", 255, 1.0, 0, 100);
      }
      ent->v.watertype = -1; //This seems like poor placement...
      ent->v.waterlevel = 0;
      return;
   }

   if (ent->v.watertype == -1) {

      SV_StartSound(0, ent, 0, "player/pl_wade1.wav", 255, 1.0, 0, 100);

      ent->v.velocity[2] *= .5;
   }

   ent->v.watertype = cont;
   ent->v.waterlevel = 1;

   if(ent->v.absmin[2] == ent->v.absmax[2]) {

      ent->v.waterlevel = 3;
   }

   halfmax[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * .5;

   global_g_groupmask = ent->v.groupinfo;
   cont = SV_PointContents(halfmax);

   if(cont > -3 || cont <= -15) { return; }

   ent->v.waterlevel = 2;
   halfmax[0] += ent->v.view_ofs[0];
   halfmax[1] += ent->v.view_ofs[1];
   halfmax[2] += ent->v.view_ofs[2];

   global_g_groupmask = ent->v.groupinfo;
   cont = SV_PointContents(halfmax);

   if(cont > -3 || cont <= -15) { return; }

   ent->v.waterlevel = 3;
}
/* DESCRIPTION: SV_RecursiveWaterLevel
// LOCATION:
// PATH: SV_Physics_Step->SV_Submerged (recursive, duh)
//
// Looks like it's trying to narrow *something* down by recursively
// recalculating the middle.  Sounds like a job for Calculus, but
// we really don't have time for that.
*/
double SV_RecursiveWaterLevel(vec3_t arg_0_vecs, float arg_4_fl1, float arg_8_fl2, int arg_c_reclevel) {

   vec3_t temp;
   double var_10;

   var_10 = ((arg_4_fl1 - arg_8_fl2) * 2.0) + arg_8_fl2;

   arg_c_reclevel++;
   if(arg_c_reclevel > 5) { return(var_10); }

   temp[0] = arg_0_vecs[0];
   temp[1] = arg_0_vecs[1];
   temp[2] = arg_0_vecs[2] + var_10;

   if(SV_PointContents(temp) == Q1CONTENTS_WATER) {

      return(SV_RecursiveWaterLevel(arg_0_vecs, arg_4_fl1, var_10, arg_c_reclevel));
   }
   else {

      return(SV_RecursiveWaterLevel(arg_0_vecs, var_10, arg_8_fl2, arg_c_reclevel));
   }
}
/* DESCRIPTION: SV_Submerged
// LOCATION:
// PATH: SV_Physics_Step
//
// Does different things depending on the waterlevel.  The exact purpose
// of this is unknown, but this particular function just seems to determine
// how deep the entity is, and whether to call another function.
//
// It returns a floating point type var.  Whether that's a float or a double
// doesn't seem to actually matter at the assembly level...
*/
double SV_Submerged(edict_t *ent) {

   vec3_t temp1;
   vec3_t temp2;
   double var_c;

   temp1[0] = (ent->v.absmin[0] + ent->v.absmax[0]) * 0.5;
   temp1[1] = (ent->v.absmin[1] + ent->v.absmax[1]) * 0.5;
   temp1[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5;

   var_c = ent->v.absmin[2] - temp1[2];

   switch(ent->v.waterlevel) {

   case 1:
      return(SV_RecursiveWaterLevel(temp1, 0.0, var_c, 0) - var_c);

   case 3:

      temp2[0] = temp1[0];
      temp2[1] = temp1[1];
      temp2[2] = ent->v.absmax[2];

      global_g_groupmask = ent->v.groupinfo;

      if(SV_PointContents(temp2) == Q1CONTENTS_WATER) {

         return(ent->v.maxs[2] - ent->v.mins[2]);
      }
      //else, drop down.

   case 2:
      return(SV_RecursiveWaterLevel(temp1, ent->v.absmax[2] - temp1[2], 0.0, 0) - var_c);

   default:
      return(0);
   }
}
/* DESCRIPTION: SV_CheckWater
// LOCATION: SDK (PM_CheckWater), and sv_phys.c
// PATH: SV_Physics_Step and SV_Physics_Toss
//
//
*/
qboolean SV_CheckWater(edict_t *ent) {

   static vec3_t current_table_42[] =
      { {1,  0, 0}, {0, 1, 0}, {-1, 0, 0 },
        {0, -1, 0}, {0, 0, 1}, { 0, 0, -1} };

   vec3_t point;
   int cont;
   int cont2;

   point[0] = (ent->v.absmin[0] + ent->v.absmax[0]) * .5;
   point[1] = (ent->v.absmin[1] + ent->v.absmax[1]) * .5;
   point[2] = ent->v.absmin[2] + 1;

   ent->v.waterlevel = 0;
   ent->v.watertype = Q1CONTENTS_EMPTY;

   global_g_groupmask = ent->v.groupinfo;
   cont = SV_PointContents(point);

   if(cont <= Q1CONTENTS_WATER && cont > -15) { //What -15 is I don't know; my defs only go to -6

      cont2 = cont;

      ent->v.watertype = cont;
      ent->v.waterlevel = 1;

      if(ent->v.absmin[2] == ent->v.absmax[2]) {

         ent->v.waterlevel = 3;
      }
      else {

         point[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5;

         global_g_groupmask = ent->v.groupinfo;
         cont = SV_PointContents(point);
         if(cont <= Q1CONTENTS_WATER && cont > -15) {

            ent->v.waterlevel = 2;

            point[0] += ent->v.view_ofs[0];
            point[1] += ent->v.view_ofs[1];
            point[2] += ent->v.view_ofs[2];

            global_g_groupmask = ent->v.groupinfo;
            cont = SV_PointContents(point);

            if(cont <= Q1CONTENTS_WATER && cont > -15) {

               ent->v.waterlevel = 3;
            }
         }
      }
      //whew
      if(cont2 <= -9 && cont2 >= -14) {

         VectorMA(ent->v.basevelocity, ent->v.waterlevel * 50, current_table_42[-9 - cont2], ent->v.basevelocity);
      }
   }

   return(ent->v.waterlevel > 1);
}



/*** Movevar querying ***/

/* DESCRIPTION: SV_SetMoveVars
// LOCATION: sv_phys.c
// PATH: SV_QueryMovevarsChanged, SV_SendServerInfo, SV_SpawnServer
//
// Sets data off of cvars.  None too complicated.
// This function was once responsible for ammo counters not updating.
// It can't be correctly completed right now, but this works.
*/
void SV_SetMoveVars() {

   movevars.gravity           = cvar_sv_gravity.value;
   movevars.stopspeed         = cvar_sv_stopspeed.value;
   movevars.maxspeed          = cvar_sv_maxspeed.value;
   movevars.spectatormaxspeed = cvar_sv_spectatormaxspeed.value;
   movevars.accelerate        = cvar_sv_accelerate.value;
   movevars.airaccelerate     = cvar_sv_airaccelerate.value;
   movevars.wateraccelerate   = cvar_sv_wateraccelerate.value;
   movevars.friction          = cvar_sv_friction.value;
   movevars.edgefriction      = cvar_sv_edgefriction.value;
   movevars.waterfriction     = cvar_sv_waterfriction.value;
   movevars.entgravity        = cvar_sv_entgravity.value;
   movevars.bounce            = cvar_sv_bounce.value;
   movevars.stepsize          = cvar_sv_stepsize.value;
   movevars.maxvelocity       = cvar_sv_maxvelocity.value;
   movevars.zmax              = cvar_sv_zmax.value;
   movevars.waveHeight        = cvar_sv_wateramp.value;
   movevars.footsteps         = cvar_sv_footsteps.value;

   movevars.rollangle  = cvar_sv_rollangle.value;
   movevars.rollspeed  = cvar_sv_rollspeed.value;
   movevars.skycolor_r = cvar_sv_skycolor_r.value;
   movevars.skycolor_g = cvar_sv_skycolor_g.value;
   movevars.skycolor_b = cvar_sv_skycolor_b.value;
   movevars.skyvec_x   = cvar_sv_skyvec_x.value;
   movevars.skyvec_y   = cvar_sv_skyvec_y.value;
   movevars.skyvec_z   = cvar_sv_skyvec_z.value;

   Q_strncpy(movevars.skyName, cvar_sv_skyname.string, sizeof(movevars.skyName)-1);      // Name of the sky map
   movevars.skyName[sizeof(movevars.skyName)-1] = '\0';
}
/* DESCRIPTION: SV_WriteMovevarsToClient
// LOCATION:
// PATH: SV_QueryMovevarsChanged, SV_SendServerInfo
//
// All those vars we set above?  Let's write 'em to a packet.
*/
void SV_WriteMovevarsToClient(sizebuf_t * buf) {

   MSG_WriteByte(buf, 0x2C);

   MSG_WriteFloat (buf, movevars.gravity);
   MSG_WriteFloat (buf, movevars.stopspeed);
   MSG_WriteFloat (buf, movevars.maxspeed);
   MSG_WriteFloat (buf, movevars.spectatormaxspeed);
   MSG_WriteFloat (buf, movevars.accelerate);
   MSG_WriteFloat (buf, movevars.airaccelerate);
   MSG_WriteFloat (buf, movevars.wateraccelerate);
   MSG_WriteFloat (buf, movevars.friction);
   MSG_WriteFloat (buf, movevars.edgefriction);
   MSG_WriteFloat (buf, movevars.waterfriction);
   MSG_WriteFloat (buf, movevars.entgravity);
   MSG_WriteFloat (buf, movevars.bounce);
   MSG_WriteFloat (buf, movevars.stepsize);
   MSG_WriteFloat (buf, movevars.maxvelocity);
   MSG_WriteFloat (buf, movevars.zmax);
   MSG_WriteFloat (buf, movevars.waveHeight);
   MSG_WriteByte  (buf, (movevars.footsteps != 0)); //Sets it to 1 if nonzero, just in case someone's abusing the whole 'bool' thing.
   MSG_WriteFloat (buf, movevars.rollangle);
   MSG_WriteFloat (buf, movevars.rollspeed);
   MSG_WriteFloat (buf, movevars.skycolor_r);
   MSG_WriteFloat (buf, movevars.skycolor_g);
   MSG_WriteFloat (buf, movevars.skycolor_b);
   MSG_WriteFloat (buf, movevars.skyvec_x);
   MSG_WriteFloat (buf, movevars.skyvec_y);
   MSG_WriteFloat (buf, movevars.skyvec_z);
   MSG_WriteString(buf, movevars.skyName);
}
/* DESCRIPTION: SV_QueryMovevarsChanged
// LOCATION:
// PATH: SV_Frame
//
// Every frame we check and make sure the movevars are what we
// expect them to be.  Personally, I think it'd be better to add
// a 'movevars' cvar flag, and only call this function if
// a flagged variable is altered.
*/
void SV_QueryMovevarsChanged() {

   unsigned int i;
   client_t * cl;

   //Do nothing if nothing has changed.
   if(movevars.gravity           == cvar_sv_gravity.value &&
      movevars.stopspeed         == cvar_sv_stopspeed.value &&
      movevars.maxspeed          == cvar_sv_maxspeed.value &&
      movevars.spectatormaxspeed == cvar_sv_spectatormaxspeed.value &&
      movevars.accelerate        == cvar_sv_accelerate.value &&
      movevars.airaccelerate     == cvar_sv_airaccelerate.value &&
      movevars.wateraccelerate   == cvar_sv_wateraccelerate.value &&
      movevars.friction          == cvar_sv_friction.value &&
      movevars.edgefriction      == cvar_sv_edgefriction.value &&
      movevars.waterfriction     == cvar_sv_waterfriction.value &&
      movevars.entgravity        == cvar_sv_entgravity.value &&
      movevars.bounce            == cvar_sv_bounce.value &&
      movevars.stepsize          == cvar_sv_stepsize.value &&
      movevars.maxvelocity       == cvar_sv_maxvelocity.value &&
      movevars.zmax              == cvar_sv_zmax.value &&
      movevars.waveHeight        == cvar_sv_wateramp.value &&
      movevars.footsteps         == cvar_sv_footsteps.value &&
      movevars.rollangle         == cvar_sv_rollangle.value &&
      movevars.rollspeed         == cvar_sv_rollspeed.value &&
      movevars.skycolor_r        == cvar_sv_skycolor_r.value &&
      movevars.skycolor_g        == cvar_sv_skycolor_g.value &&
      movevars.skycolor_b        == cvar_sv_skycolor_b.value &&
      movevars.skyvec_x          == cvar_sv_skyvec_x.value &&
      movevars.skyvec_y          == cvar_sv_skyvec_y.value &&
      movevars.skyvec_z          == cvar_sv_skyvec_z.value &&
      Q_strcmp(movevars.skyName, cvar_sv_skyname.string) == 0) {

      return; //And under your chairs, Nothing!
   }

   Con_Printf("%s: changed movevars...\n", __FUNCTION__);
   SV_SetMoveVars();

   for(i = 0, cl = global_svs.clients; i < global_svs.allocated_client_slots; i++, cl++) {

      if((cl->padding00_used == 0 && cl->padding0C_connected == 0 && cl->padding04 == 0) ||
         (cl->padding2438_IsFakeClient != 0)) { continue; }

      SV_WriteMovevarsToClient(&(cl->netchan1C.netchan_message));
   }
}



/*** Pushing ***/

/* DESCRIPTION: SV_PushEntity
// LOCATION: sv_phys.c
// PATH: Three physics functions
//
// Moves an entity WITHOUT altering its velocity.
*/
trace_t SV_PushEntity(edict_t *ent, vec3_t push) {

   trace_t trace;
   vec3_t end;
   int var_48;
   int var_4c;

   if((ent->v.flags & FL_MONSTERCLIP) == 0) { var_48 = 0; }
   else { var_48 = 1; }

   end[0] = push[0] + ent->v.origin[0];
   end[1] = push[1] + ent->v.origin[1];
   end[2] = push[2] + ent->v.origin[2];

   if(ent->v.movetype == MOVETYPE_FLYMISSILE) { //
      var_4c = 2; //MOVE_MISSILE
   }
   else if(ent->v.solid == SOLID_NOT || ent->v.solid == SOLID_TRIGGER) {
      var_4c = 1; //MOVE_NOMONSTERS
   }
   else {
      var_4c = 0; //MOVE_NORMAL
   }

   trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, var_4c, ent, var_48);

   if(trace.fraction != 0) {

      ent->v.origin[0] = trace.endpos[0];
      ent->v.origin[1] = trace.endpos[1];
      ent->v.origin[2] = trace.endpos[2];
   }

   SV_LinkEdict(ent, 1);

   if(trace.pHit != NULL) {
      SV_Impact(ent, trace.pHit, &trace);
   }

   return(trace);
}
/* DESCRIPTION: SV_PushRotate
// LOCATION:
// PATH: SV_Physics_pusher
//
// Similar to PushMove, but even more complicated and with even less relevance
// to QW.  Has a bug, in that a player on the object does not rotate.
*/
int SV_PushRotate(edict_t *pusher, float movetime) {

   unsigned int i, j;
   unsigned int var_40;
   vec3_t var_24_amove, var_3C_angles;
   vec3_t var_94_forward, var_a0_right, var_ac_up;
   vec3_t var_70_tforward, var_7c_tright, var_88_tup;
   vec3_t var_30_temporigin, var_58_OrigDiff, var_4C_absmid;
   vec3_t var_64_MoreAngles, var_18_FinalVector;
   trace_t var_F0_trace;

   edict_t * TempEnt;

   if(pusher->v.avelocity[0] == 0 && pusher->v.avelocity[1] == 0 && pusher->v.avelocity[2] == 0) {
      pusher->v.ltime += movetime;
      return(1);
   }

   for(i = 0; i < 3; i++) {

      var_24_amove[i] = pusher->v.avelocity[i] * movetime;
   }

   AngleVectors(pusher->v.angles, var_94_forward, var_a0_right, var_ac_up);
   var_3C_angles[0] = pusher->v.angles[0];
   var_3C_angles[1] = pusher->v.angles[1];
   var_3C_angles[2] = pusher->v.angles[2];
   pusher->v.angles[0] += var_24_amove[0];
   pusher->v.angles[1] += var_24_amove[1];
   pusher->v.angles[2] += var_24_amove[2];
   AngleVectorsTranspose(pusher->v.angles, var_70_tforward, var_7c_tright, var_88_tup);

   pusher->v.ltime += movetime;
   SV_LinkEdict(pusher, 0);

   if(pusher->v.solid == SOLID_NOT) { return(1); }

   var_24_amove[0] = 0;
   var_24_amove[2] = 0;

   var_40 = 0;

   for(i = 1; i < global_sv.num_edicts; i++) {

      TempEnt = &(global_sv.edicts[i]);

      if(TempEnt->free != 0 ||
         TempEnt->v.movetype == MOVETYPE_PUSH || TempEnt->v.movetype == MOVETYPE_NONE ||
         TempEnt->v.movetype == MOVETYPE_FOLLOW || TempEnt->v.movetype == MOVETYPE_NOCLIP ||
         (((TempEnt->v.flags & FL_ONGROUND) == 0 || TempEnt->v.groundentity != NULL) &&
         (TempEnt->v.absmin[0] > pusher->v.absmax[0] ||
          TempEnt->v.absmin[1] > pusher->v.absmax[1] ||
          TempEnt->v.absmin[2] > pusher->v.absmax[2] ||
          TempEnt->v.absmax[0] < pusher->v.absmin[0] ||
          TempEnt->v.absmax[1] < pusher->v.absmin[1] ||
          TempEnt->v.absmax[2] < pusher->v.absmin[2] ||
          SV_TestEntityPosition(TempEnt) == 0))) {

         continue;
      }

      if(TempEnt->v.movetype != MOVETYPE_WALK) {
         TempEnt->v.flags &= ~FL_ONGROUND;
      }

      var_30_temporigin[0] = TempEnt->v.origin[0];
      var_30_temporigin[1] = TempEnt->v.origin[1];
      var_30_temporigin[2] = TempEnt->v.origin[2];

      g_moved_from[var_40][0] = TempEnt->v.origin[0]; //Hope that's right...
      g_moved_from[var_40][1] = TempEnt->v.origin[1];
      g_moved_from[var_40][2] = TempEnt->v.origin[2];
      g_moved_edict[var_40] = TempEnt;

      var_40++;
      if(var_40 >= global_sv.max_edicts) { Sys_Error("%s: Out of edicts in simulator.", __FUNCTION__); }

      if(TempEnt->v.movetype == MOVETYPE_PUSHSTEP) {

         var_4C_absmid[0] = (TempEnt->v.absmin[0] + TempEnt->v.absmax[0]) / 2;
         var_4C_absmid[1] = (TempEnt->v.absmin[1] + TempEnt->v.absmax[1]) / 2;
         var_4C_absmid[2] = (TempEnt->v.absmin[2] + TempEnt->v.absmax[2]) / 2;

         var_58_OrigDiff[0] = var_4C_absmid[0] - pusher->v.origin[0];
         var_58_OrigDiff[1] = var_4C_absmid[1] - pusher->v.origin[1];
         var_58_OrigDiff[2] = var_4C_absmid[2] - pusher->v.origin[2];
      }
      else {

         var_58_OrigDiff[0] = TempEnt->v.origin[0] - pusher->v.origin[0];
         var_58_OrigDiff[1] = TempEnt->v.origin[1] - pusher->v.origin[1];
         var_58_OrigDiff[2] = TempEnt->v.origin[2] - pusher->v.origin[2];
      }

      //dot time.
      var_4C_absmid[0] =  (var_58_OrigDiff[0] * var_94_forward[0]) + (var_58_OrigDiff[1] * var_94_forward[1]) + (var_58_OrigDiff[2] * var_94_forward[2]);
      var_4C_absmid[1] = -(var_58_OrigDiff[0] * var_a0_right[0]) + (var_58_OrigDiff[1] * var_a0_right[1]) + (var_58_OrigDiff[2] * var_a0_right[2]);
      var_4C_absmid[2] =  (var_58_OrigDiff[0] * var_ac_up[0]) + (var_58_OrigDiff[1] * var_ac_up[1]) + (var_58_OrigDiff[2] * var_ac_up[2]);

      var_64_MoreAngles[0] = (var_4C_absmid[0] * var_70_tforward[0]) + (var_4C_absmid[1] * var_70_tforward[1]) + (var_4C_absmid[2] * var_70_tforward[2]);
      var_64_MoreAngles[1] = (var_4C_absmid[0] * var_7c_tright[0]) + (var_4C_absmid[1] * var_7c_tright[1]) + (var_4C_absmid[2] * var_7c_tright[2]);
      var_64_MoreAngles[2] = (var_4C_absmid[0] * var_88_tup[0]) + (var_4C_absmid[1] * var_88_tup[1]) + (var_4C_absmid[2] * var_88_tup[2]);

      var_18_FinalVector[0] = var_64_MoreAngles[0] - var_58_OrigDiff[0];
      var_18_FinalVector[1] = var_64_MoreAngles[1] - var_58_OrigDiff[1];
      var_18_FinalVector[2] = var_64_MoreAngles[2] - var_58_OrigDiff[2];

      pusher->v.solid = SOLID_NOT;
      var_F0_trace = SV_PushEntity(TempEnt, var_18_FinalVector);

      pusher->v.solid = SOLID_BSP;

      if(TempEnt->v.movetype != MOVETYPE_PUSHSTEP) {
         if((TempEnt->v.flags & FL_CLIENT) == 0) {

            TempEnt->v.angles[0] += var_24_amove[0];
            TempEnt->v.angles[1] += var_24_amove[1];
            TempEnt->v.angles[2] += var_24_amove[2];
         }
         else {

            TempEnt->v.avelocity[1] += var_24_amove[1];
            TempEnt->v.fixangle = 2;
         }
      }

      if(SV_TestEntityPosition(TempEnt) == 0) { continue; }
      if(TempEnt->v.mins[0] == TempEnt->v.maxs[0]) { continue; }

      if(TempEnt->v.solid == SOLID_NOT || TempEnt->v.solid == SOLID_TRIGGER) {

         TempEnt->v.mins[0] = 0;
         TempEnt->v.mins[1] = 0;
         TempEnt->v.maxs[0] = 0;
         TempEnt->v.maxs[1] = 0;
         TempEnt->v.maxs[2] = TempEnt->v.mins[2];
         continue;
      }


      TempEnt->v.origin[0] = var_30_temporigin[0];
      TempEnt->v.origin[1] = var_30_temporigin[1];
      TempEnt->v.origin[2] = var_30_temporigin[2];
      SV_LinkEdict(TempEnt, 1);

      pusher->v.origin[0] = var_3C_angles[0];
      pusher->v.origin[1] = var_3C_angles[1];
      pusher->v.origin[2] = var_3C_angles[2];
      SV_LinkEdict(pusher, 0);

      pusher->v.ltime -= movetime;

      gEntityInterface.pfnBlocked(pusher, TempEnt);

      for(j = 0; j < var_40; j++) {

         g_moved_edict[j]->v.origin[0] = g_moved_from[j][0];
         g_moved_edict[j]->v.origin[1] = g_moved_from[j][1];
         g_moved_edict[j]->v.origin[2] = g_moved_from[j][2];

         if((g_moved_edict[j]->v.flags & FL_CLIENT) == 0) {

            if(TempEnt->v.movetype != MOVETYPE_PUSHSTEP) {

               g_moved_edict[j]->v.angles[0] -= var_24_amove[0];
               g_moved_edict[j]->v.angles[1] -= var_24_amove[1];
               g_moved_edict[j]->v.angles[2] -= var_24_amove[2];
            }
         }
         else {

            g_moved_edict[j]->v.avelocity[1] = 0;
         }
         SV_LinkEdict(g_moved_edict[j], 0);
      }
      return(0);
   }
   return(1);
}
/* DESCRIPTION: SV_PushMove
// LOCATION:
// PATH: SV_Physics_pusher
//
// You can tell this wasn't part of QW due to it being so very very messy.
// Well, that's a lie.  It is part of QW, in the same way that a computer is
// part of of QW, but the similarities are few, and the structure is quite
// different.  This is probably NOT right, either.
*/
void SV_PushMove(edict_t *pusher, float movetime) {

   unsigned int i, j;
   unsigned int var_4c;
   vec3_t var_30_move;
   vec3_t var_18_tempmin, var_24_tempmax;
   vec3_t var_48_temporigin, var_3C_temporigin2;
   trace_t var_84_trace;
   edict_t * TempEnt;

   if(pusher->v.velocity[0] == 0 && pusher->v.velocity[1] == 0 && pusher->v.velocity[2] == 0) {
      pusher->v.ltime += movetime;
      return;
   }

   var_48_temporigin[0] = pusher->v.origin[0];
   var_48_temporigin[1] = pusher->v.origin[1];
   var_48_temporigin[2] = pusher->v.origin[2];

   for(i = 0; i < 3; i++) {

      var_30_move[i] = pusher->v.velocity[i] * movetime;
      var_18_tempmin[i] = pusher->v.absmin[i] + var_30_move[i];
      var_24_tempmax[i] = pusher->v.absmax[i] + var_30_move[i];

      pusher->v.origin[i] += var_30_move[i];
   }

   pusher->v.ltime += movetime;
   SV_LinkEdict(pusher, 0);

   if(pusher->v.solid == SOLID_NOT) { return; }

   var_4c = 0;
   for(i = 1; i < global_sv.num_edicts; i++) {

      TempEnt = &(global_sv.edicts[i]);

      if(TempEnt->free != 0 ||
         TempEnt->v.movetype == MOVETYPE_PUSH || TempEnt->v.movetype == MOVETYPE_NONE ||
         TempEnt->v.movetype == MOVETYPE_FOLLOW || TempEnt->v.movetype == MOVETYPE_NOCLIP ||
         (((TempEnt->v.flags & FL_ONGROUND) == 0 || TempEnt->v.groundentity != NULL) &&
         (TempEnt->v.absmin[0] > var_24_tempmax[0] ||
          TempEnt->v.absmin[1] > var_24_tempmax[1] ||
          TempEnt->v.absmin[2] > var_24_tempmax[2] ||
          TempEnt->v.absmax[0] < var_18_tempmin[0] ||
          TempEnt->v.absmax[1] < var_18_tempmin[1] ||
          TempEnt->v.absmax[2] < var_18_tempmin[2] ||
          SV_TestEntityPosition(TempEnt) == 0))) {

         continue; //Four distinct reasons to bail, one convenient if statement.
      }

      if(TempEnt->v.movetype != MOVETYPE_WALK) {
         TempEnt->v.flags &= ~FL_ONGROUND;
      }

      var_3C_temporigin2[0] = TempEnt->v.origin[0];
      var_3C_temporigin2[1] = TempEnt->v.origin[1];
      var_3C_temporigin2[2] = TempEnt->v.origin[2];

      g_moved_from[var_4c][0] = TempEnt->v.origin[0]; //Hope that's right...
      g_moved_from[var_4c][1] = TempEnt->v.origin[1];
      g_moved_from[var_4c][2] = TempEnt->v.origin[2];
      g_moved_edict[var_4c] = TempEnt;
      var_4c++;

      pusher->v.solid = SOLID_NOT;
      var_84_trace = SV_PushEntity(TempEnt, var_30_move);
      pusher->v.solid = SOLID_BSP;

      if(SV_TestEntityPosition(TempEnt) == 0) { continue; }
      if(TempEnt->v.mins[0] == TempEnt->v.maxs[0]) { continue; }

      if(TempEnt->v.solid == SOLID_NOT || TempEnt->v.solid == SOLID_TRIGGER) {

         TempEnt->v.mins[0] = 0;
         TempEnt->v.mins[1] = 0;
         TempEnt->v.maxs[0] = 0;
         TempEnt->v.maxs[1] = 0;
         TempEnt->v.maxs[2] = TempEnt->v.mins[2];
         continue;
      }

      TempEnt->v.origin[0] = var_3C_temporigin2[0];
      TempEnt->v.origin[1] = var_3C_temporigin2[1];
      TempEnt->v.origin[2] = var_3C_temporigin2[2];
      SV_LinkEdict(TempEnt, 1);

      pusher->v.origin[0] = var_48_temporigin[0];
      pusher->v.origin[1] = var_48_temporigin[1];
      pusher->v.origin[2] = var_48_temporigin[2];
      SV_LinkEdict(pusher, 0);

      pusher->v.ltime -= movetime;

      gEntityInterface.pfnBlocked(pusher, TempEnt);

      for(j = 0; j < var_4c; j++) {

         g_moved_edict[j]->v.origin[0] = g_moved_from[j][0];
         g_moved_edict[j]->v.origin[1] = g_moved_from[j][1];
         g_moved_edict[j]->v.origin[2] = g_moved_from[j][2];

         SV_LinkEdict(g_moved_edict[j], 0);
      }
      return;
   }
}



/*** Physics ***/

/* DESCRIPTION: SV_FlyMove
// PATH: SV_Physics_Step
//
// Well, I'll just paste the fucntion description here.
//
// The basic solid body movement clip that slides along multiple planes
// Returns the clipflags if the velocity was modified (hit something solid)
//
// 1 = floor
// 2 = wall / step
// 4 = dead stop
// If steptrace is not NULL, the trace of any vertical wall hit will be stored
*/
int SV_FlyMove(edict_t *ent, float time, trace_t * steptrace) {

   int     bumpcount, numbumps;
   vec3_t  dir;
   float   d;
   int     numplanes;
   vec3_t  planes[5];
   vec3_t  primal_velocity, original_velocity, new_velocity;
   int     i, j;
   trace_t trace;
   vec3_t  end;
   float   time_left;
   int     blocked;

   int var_d4;

   if((ent->v.flags & FL_MONSTERCLIP) == 0) {
      var_d4 = 0;
   }
   else {
      var_d4 = 1;
   }

   numbumps = 4;
   blocked = 0;

   VectorCopy (ent->v.velocity, original_velocity);
   VectorCopy (ent->v.velocity, primal_velocity);

   numplanes = 0;

   time_left = time;

   for(bumpcount = 0; bumpcount < numbumps; bumpcount++) {

      if(ent->v.velocity[0] == 0 && ent->v.velocity[1] == 0 && ent->v.velocity[2] == 0) {
         break;
      }

      for (i = 0; i < 3; i++) {
         end[i] = ent->v.origin[i] + time_left * ent->v.velocity[i];
      }

      trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, 0, ent, var_d4);

      if(trace.startsolid != 0) { // entity is trapped in another solid.  I wonder if we can help?

         ent->v.velocity[0] = 0;
         ent->v.velocity[1] = 0;
         ent->v.velocity[2] = 0;

         return(4);
      }

      if(trace.fraction > 0) { // we moved
         VectorCopy(trace.endpos, ent->v.origin);
         VectorCopy(ent->v.velocity, original_velocity);
         numplanes = 0;
      }

      if(trace.fraction == 1) { // we moved the entire distance
         break;
      }

      if(trace.pHit == NULL) {
         Sys_Error("%s: trace.fraction indicates collision, trace.ent is NULL", __FUNCTION__);
      }

      if(trace.plane.normal[2] > 0.7) {

         blocked |= 1; // floor
         if(trace.pHit->v.solid == SOLID_BSP || trace.pHit->v.solid == SOLID_SLIDEBOX ||
            trace.pHit->v.movetype == MOVETYPE_PUSHSTEP || (trace.pHit->v.flags & FL_CLIENT) != 0) {

            ent->v.flags |= FL_ONGROUND;
            ent->v.groundentity = trace.pHit;
         }
      }

      if(trace.plane.normal[2] == 0) {

         blocked |= 2; // step
         if(steptrace != NULL) {
            *steptrace = trace; // I hope they handle this safely...
         }
      }

      SV_Impact(ent, trace.pHit, &trace);

      if(ent->free) {
         break; // removed by the impact function
      }

      time_left -= time_left * trace.fraction;

      if(numplanes >= 5) { // this shouldn't really happen

         ent->v.velocity[0] = 0;
         ent->v.velocity[1] = 0;
         ent->v.velocity[2] = 0;

         return(blocked);
      }

      VectorCopy(trace.plane.normal, planes[numplanes]);
      numplanes++;

      if(ent->v.movetype == MOVETYPE_WALK && ((ent->v.flags & FL_ONGROUND) == 0 || ent->v.friction != 1)) {

         for(i = 0; i < numplanes; i++) {

            if(planes[i][2] > .7) {

               ClipVelocity(original_velocity, planes[i], new_velocity, 1.0);
               VectorCopy(new_velocity, original_velocity);
            }
            else {

               ClipVelocity(original_velocity, planes[i], new_velocity, (1 - ent->v.friction) * cvar_sv_bounce.value + 1);
            }
         }

         VectorCopy(new_velocity, ent->v.velocity);
         VectorCopy(new_velocity, original_velocity);
      }
      else {

         for(i = 0; i < numplanes; i++) {

            ClipVelocity(original_velocity, planes[i], new_velocity, 1);

            for(j = 0; j < numplanes; j++) {

               if(j != i) {

                  if(DotProduct(new_velocity, planes[j]) < 0) { break; }
               }
            }

            if(j == numplanes) {
               break;
            }
         }

         if(i != numplanes) { // go along this plane

            VectorCopy(new_velocity, ent->v.velocity);
         }
         else { // go along the crease

            if(numplanes != 2) {
               return(blocked);
            }

            CrossProduct(planes[0], planes[1], dir);
            //VectorNormalize(dir);   //fixes slow falling in corners
            d = DotProduct(dir, ent->v.velocity);
            VectorScale(dir, d, ent->v.velocity);
         }

         if(DotProduct(ent->v.velocity, primal_velocity) <= 0) {

            ent->v.velocity[0] = 0;
            ent->v.velocity[1] = 0;
            ent->v.velocity[2] = 0;
            return(blocked);
         }
      }
   }

   return(blocked);
}

/* DESCRIPTION: SV_Physics_Pusher
// LOCATION: sv_phys.c
// PATH: SV_Physics
//
// Moving those boxes around isn't fun.
*/
void SV_Physics_Pusher(edict_t * ent) {

   double thinktime;
   double oldltime;
   double movetime;
   double var_1c;

   oldltime = ent->v.ltime;
   thinktime = ent->v.nextthink;

   if(thinktime < oldltime + host_frametime) {
      movetime = thinktime - oldltime;
      if(movetime < 0) {
         movetime = 0;
      }
   }
   else {
      movetime = host_frametime;
   }

   if(movetime != 0) {
      if(ent->v.avelocity[0] == 0 && ent->v.avelocity[1] == 0 && ent->v.avelocity[2] == 0) {
         SV_PushMove(ent, movetime); // advances ent->v->ltime if not blocked
      }
      else if(ent->v.velocity[0] == 0 && ent->v.velocity[1] == 0 && ent->v.velocity[2] == 0) {
         SV_PushRotate(ent, movetime);
      }
      else if(SV_PushRotate(ent, movetime) != 0) {

         var_1c = ent->v.ltime;
         ent->v.ltime = oldltime;
         SV_PushMove(ent, movetime);

         if(var_1c < ent->v.ltime) {
            ent->v.ltime = var_1c;
         }
      }
   }

   if(thinktime > oldltime && ((ent->v.flags & FL_ALWAYSTHINK) || thinktime <= ent->v.ltime)) {

      ent->v.nextthink = 0;
      gGlobalVariables.time = global_sv.time0c;

      gEntityInterface.pfnThink(ent);

      //Extra check and jump no doubt defined out.
   }
}
/* DESCRIPTION: SV_Physics_Follow
// LOCATION: sv_phys.c
// PATH: SV_Physics
//
// I've not seen v_angle used before, and this is much different than QW's
// version.  Other than we're taking values from a lead object and applying
// them to the follower, I don't get it...
*/
void SV_Physics_Follow(edict_t *ent) {

   edict_t * pAiment;

   // regular thinking
   if(SV_RunThink(ent) == 0) { return; }

   pAiment = ent->v.aiment;
   if(pAiment == NULL) {

      Con_Printf("%s: Movetype FOLLOW with no corresponding ent", __FUNCTION__);
      ent->v.movetype = MOVETYPE_NONE;
      return;
   }

   ent->v.origin[0] = pAiment->v.origin[0] + ent->v.v_angle[0];
   ent->v.origin[1] = pAiment->v.origin[1] + ent->v.v_angle[1];
   ent->v.origin[2] = pAiment->v.origin[2] + ent->v.v_angle[2];

   ent->v.angles[0] = pAiment->v.angles[0];
   ent->v.angles[1] = pAiment->v.angles[1];
   ent->v.angles[2] = pAiment->v.angles[2];

   SV_LinkEdict(ent, 1);
}
/* DESCRIPTION: SV_Physics_Noclip
// LOCATION: sv_phys.c
// PATH: SV_Physics
//
// This throws my entire reality into question!  Oh, wait, it's noclip.
*/
void SV_Physics_Noclip(edict_t *ent) {

   // regular thinking
   if(SV_RunThink(ent) == 0) { return; }

   VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);
   VectorMA(ent->v.origin, host_frametime, ent->v.velocity, ent->v.origin);

   SV_LinkEdict(ent, 0);
}
/* DESCRIPTION: SV_Physics_Step
// LOCATION: sv_phys.c
// PATH: SV_Physics
//
// Moves things.  As listed in QW, this is used to handle edicets in freefall,
// and edicts moving in discrete steps.  It's also used for objects that have
// the floor pulled out from under them, but my inner HL player is pretty sure
// that it isn't foolproof there...
*/
void SV_Physics_Step(edict_t *ent) {

   int var_4_Flags_and_200;
   int var_8_checkwater;
   int var_c_hitsound;
   double var_14_distance;
   double var_18;
   double var_1c;
   double var_24;
   double var_20_friction;

   int x, y;

   vec3_t tempmins;
   vec3_t tempmaxs;
   vec3_t temp;

   trace_t var_9C_trace;


   PF_WaterMove(ent);
   SV_CheckVelocity(ent);

   var_4_Flags_and_200 = ent->v.flags & FL_ONGROUND;
   var_8_checkwater = SV_CheckWater(ent);

   if((ent->v.flags & FL_FLOAT) && ent->v.waterlevel > 0) {

      SV_Submerged(ent);

      var_24 = SV_Submerged(ent) * ent->v.skin * host_frametime;

      SV_AddGravity(ent);
      ent->v.velocity[2] += var_24;
   }

   if(var_4_Flags_and_200 == 0 && (ent->v.flags & FL_FLY) == 0 &&
      ((ent->v.flags & FL_SWIM) == 0 || ent->v.waterlevel <= 0)) {

      if(ent->v.velocity[2] < cvar_sv_gravity.value * -0.1) { var_c_hitsound = 1; }
      else { var_c_hitsound = 0; }

      if(var_8_checkwater == 0) {
         SV_AddGravity(ent);
      }
   }

   if(ent->v.velocity[0] == 0 && ent->v.velocity[1] == 0 && ent->v.velocity[2] == 0 &&
      ent->v.basevelocity[0] == 0 && ent->v.basevelocity[1] == 0 && ent->v.basevelocity[2] == 0) {

      if(gGlobalVariables.force_retouch != 0) {

         var_9C_trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, 0, ent, ((ent->v.flags & FL_MONSTERCLIP) != 0));

         if((var_9C_trace.fraction < 1 || var_9C_trace.startsolid != 0) && var_9C_trace.pHit != NULL) {

            SV_Impact(ent, var_9C_trace.pHit, &var_9C_trace);
         }
      }
   }
   else {

      ent->v.flags &= ~FL_ONGROUND;
      if(var_4_Flags_and_200 != 0 &&
         (ent->v.health > 0 || SV_CheckBottom(ent) != 0)) {

         var_14_distance = sqrt((ent->v.velocity[0] * ent->v.velocity[0]) +
                                (ent->v.velocity[1] * ent->v.velocity[1]));

         if(var_14_distance != 0) {

            var_20_friction = cvar_sv_friction.value * ent->v.friction;
            ent->v.friction = 1;

            if(var_14_distance > cvar_sv_stopspeed.value) {
               var_1c = cvar_sv_stopspeed.value;
            }
            else {
               var_1c = var_14_distance;
            }

            //Could be wrong here
            var_18 = var_14_distance - (var_1c * host_frametime * var_20_friction);
            if(var_18 < 0) { var_18 = 0; }

            var_18 /= var_14_distance;
            ent->v.velocity[0] *= var_18;
            ent->v.velocity[1] *= var_18;
         }
      }

      ent->v.velocity[0] += ent->v.basevelocity[0];
      ent->v.velocity[1] += ent->v.basevelocity[1];
      ent->v.velocity[2] += ent->v.basevelocity[2];

      SV_CheckVelocity(ent);
      SV_FlyMove(ent, host_frametime, 0);
      SV_CheckVelocity(ent);

      ent->v.velocity[0] -= ent->v.basevelocity[0];
      ent->v.velocity[1] -= ent->v.basevelocity[1];
      ent->v.velocity[2] -= ent->v.basevelocity[2];

      SV_CheckVelocity(ent);

      tempmins[0] = ent->v.origin[0] + ent->v.mins[0];
      tempmins[1] = ent->v.origin[1] + ent->v.mins[1];
      tempmins[2] = ent->v.origin[2] + ent->v.mins[2];
      tempmaxs[0] = ent->v.origin[0] + ent->v.maxs[0];
      tempmaxs[1] = ent->v.origin[1] + ent->v.maxs[1];
      tempmaxs[2] = ent->v.origin[2] + ent->v.maxs[2];

      temp[2] = tempmins[2] - 1.0;

      for(x = 0; x <= 1; x++) {
         for(y = 0; y <= 1; y++) {

            if(x != 0) { temp[0] = tempmaxs[0]; }
            else { temp[0] = tempmins[0]; }

            if(y != 0) { temp[1] = tempmaxs[1]; }
            else { temp[1] = tempmins[1]; }

            global_g_groupmask = ent->v.groupinfo;
            if(SV_PointContents(temp) == Q1CONTENTS_SOLID) {
               ent->v.flags |= FL_ONGROUND;
               break;
            }
         }
      }

      SV_LinkEdict(ent, 1);

      //do nothing, the no doubt sound related task isn't required.
      //if((ent->flags & 0x200) != 0 && var_4_Flags_and_200 == 0 && var_c_hitsound)
   }

   SV_RunThink(ent);
   SV_CheckWaterTransition(ent);
}
/* DESCRIPTION: SV_Physics_Toss
// LOCATION: sv_phys.c
// PATH: SV_Physics
//
// Gravity is a harsh mistress.  This function accounts for that little fact of
// life.  This plays with velocities and with a boolean I'm all but
// certain represents 'on_ground'.
*/
void SV_Physics_Toss(edict_t *ent) {

   trace_t trace;
   vec3_t var_44_move;
   double backoff;
   double var_5C_MoveSquare;

   SV_CheckWater(ent);
   if(SV_RunThink(ent) == 0) { return; }

   if(ent->v.velocity[2] > 0 ||
      ent->v.groundentity == NULL || (ent->v.groundentity->v.flags & (FL_MONSTER | FL_CLIENT)) != 0) {

      ent->v.flags &= ~FL_ONGROUND;
   }

   if(((ent->v.flags & FL_ONGROUND) != 0) &&
       ent->v.velocity[0] == 0 &&
       ent->v.velocity[1] == 1 &&
       ent->v.velocity[2] == 2) {

      ent->v.avelocity[0] = 0;
      ent->v.avelocity[1] = 0;
      ent->v.avelocity[2] = 0;

      if(ent->v.basevelocity[0] == 0 &&
         ent->v.basevelocity[1] == 0 &&
         ent->v.basevelocity[2] == 0) { return; } //All zeros, at rest.
   }

   SV_CheckVelocity(ent);

   if(ent->v.movetype != MOVETYPE_FLY && ent->v.movetype != MOVETYPE_FLYMISSILE && ent->v.movetype != MOVETYPE_BOUNCEMISSILE) {

      SV_AddGravity(ent); //Add gravity, duh.
   }

   VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles); //move angles

   //move origin

   ent->v.velocity[0] += ent->v.basevelocity[0];
   ent->v.velocity[1] += ent->v.basevelocity[1];
   ent->v.velocity[2] += ent->v.basevelocity[2];

   SV_CheckVelocity(ent);

   VectorScale(ent->v.velocity, host_frametime, var_44_move);

   ent->v.velocity[0] -= ent->v.basevelocity[0];
   ent->v.velocity[1] -= ent->v.basevelocity[1];
   ent->v.velocity[2] -= ent->v.basevelocity[2];

   trace = SV_PushEntity(ent, var_44_move);
   SV_CheckVelocity(ent);

   if(trace.allsolid != 0) {

      ent->v.velocity[0] = 0;
      ent->v.velocity[1] = 0;
      ent->v.velocity[2] = 0;
      ent->v.avelocity[0] = 0;
      ent->v.avelocity[1] = 0;
      ent->v.avelocity[2] = 0;

      return;
   }

   if(trace.fraction == 1) {

      SV_CheckWaterTransition(ent);
      return;
   }
   if(ent->free != 0) { return; }

   if(ent->v.movetype == MOVETYPE_BOUNCE) {
      backoff = 2 - ent->v.friction;
   }
   else if(ent->v.movetype == MOVETYPE_BOUNCEMISSILE) {
      backoff = 2;
   }
   else {
      backoff = 1;
   }

   ClipVelocity(ent->v.velocity, trace.plane.normal, ent->v.velocity, backoff);

   // stop if on ground
   if(trace.plane.normal[2] > 0.7) {

      var_44_move[0] = ent->v.velocity[0] + ent->v.basevelocity[0];
      var_44_move[1] = ent->v.velocity[1] + ent->v.basevelocity[1];
      var_44_move[2] = ent->v.velocity[2] + ent->v.basevelocity[2];

      var_5C_MoveSquare = (var_44_move[0]*var_44_move[0]) + (var_44_move[1]*var_44_move[1]) + (var_44_move[2]*var_44_move[2]);

      if(var_44_move[2] < cvar_sv_gravity.value * host_frametime) {

         ent->v.flags |= FL_ONGROUND;
         ent->v.groundentity = trace.pHit;
         ent->v.velocity[2] = 0;
      }

      if(var_5C_MoveSquare >= 900 && (ent->v.movetype == 0x0A || ent->v.movetype == 0x0B)) {

         VectorScale(ent->v.velocity, (1 - trace.fraction) * host_frametime * .9, var_44_move);
         VectorMA(var_44_move, (1 - trace.fraction) * host_frametime * .9, ent->v.basevelocity, var_44_move); //move angles
         trace = SV_PushEntity(ent, var_44_move);
      }
      else {

         ent->v.flags |= FL_ONGROUND;
         ent->v.groundentity = trace.pHit;
         ent->v.velocity[0] = 0;
         ent->v.velocity[1] = 0;
         ent->v.velocity[2] = 0;
         ent->v.avelocity[0] = 0;
         ent->v.avelocity[1] = 0;
         ent->v.avelocity[2] = 0;
      }
   }

   // check for in water
   SV_CheckWaterTransition (ent);
}
/* DESCRIPTION: SV_Physics_None
// LOCATION: sv_phys.c
// PATH: SV_Physics
//
// It's not a large function, so I'm gonna guess nothign actually gets moved...
*/
void SV_Physics_None(edict_t * ent) {

   SV_RunThink(ent);
}

/* DESCRIPTION: SV_Physics
// LOCATION: sv_phys.c
// PATH: SV_ActivateServer, SV_Frame
//
// Moves around all of our little objects.
*/
void SV_Physics() {

   unsigned int i;
   edict_t *ent, *var_c_tempent;

   gGlobalVariables.time = global_sv.time0c;
   gEntityInterface.pfnStartFrame();


   for(i = 0; i < global_sv.num_edicts; i++) {

      ent = &(global_sv.edicts[i]);
      if(ent->free != 0) { continue; }

      if(gGlobalVariables.force_retouch != 0) {
         SV_LinkEdict(ent, 1);
      }

      if(i > 0 && i <= global_svs.allocated_client_slots) { //don't run this for players.
         continue;
      }

      if((ent->v.flags & FL_ONGROUND)) {

         var_c_tempent = ent->v.groundentity;
         if(var_c_tempent != NULL && (var_c_tempent->v.flags & FL_CONVEYOR) != 0) {
            if((ent->v.flags & FL_BASEVELOCITY) != 0) {
               VectorMA(ent->v.basevelocity, var_c_tempent->v.speed, var_c_tempent->v.movedir, ent->v.basevelocity);
            }
            else {
               VectorScale(var_c_tempent->v.movedir, var_c_tempent->v.speed, ent->v.basevelocity);
            }

            ent->v.flags |= FL_BASEVELOCITY;
         }
      }

      if((ent->v.flags & FL_BASEVELOCITY) == 0) {

         VectorMA(ent->v.velocity, (host_frametime / 2.0) + 1, ent->v.basevelocity, ent->v.velocity);
         ent->v.basevelocity[0] = 0;
         ent->v.basevelocity[1] = 0;
         ent->v.basevelocity[2] = 0;
      }

      ent->v.flags &= ~FL_BASEVELOCITY;


      switch(ent->v.movetype) {

      case 0x07:
         SV_Physics_Pusher(ent);
         break;
      case 0x00:
         SV_Physics_None(ent);
         break;
      case 0x0C:
         SV_Physics_Follow(ent);
         break;
      case 0x08:
         SV_Physics_Noclip(ent);
         break;
      case 0x04:
      case 0x0D:
         SV_Physics_Step(ent);
         break;
      case 0x06:
      case 0x0A:
      case 0x0B:
      case 0x05:
      case 0x09:
         SV_Physics_Toss(ent);
         break;
      default:

         Sys_Error("%s: bad movetype %d", __FUNCTION__, ent->v.movetype);
      }

      if((ent->v.flags & FL_KILLME)) {
         ED_Free(ent);
      }
   }

   if(gGlobalVariables.force_retouch != 0) {
      gGlobalVariables.force_retouch -= 1;
   }
}



/*** Other ***/

/* DESCRIPTION: SV_RunCmd
// LOCATION: sv_user.c
// PATH: SV_ParseMove, recursive, some PF function
//
// This incredibly long function mostly consists of copying variables from
// one struct to another.  Very tedious.
//
// It actually belongs in sv_user.c, but that breaks our static cvar routine.
*/
void SV_RunCmd(usercmd_t *ucmd, unsigned int seed) {

   int i, oldmsec;
   double var_48_tempframetime;
   usercmd_t var_7C_usercmd;
   vec3_t var_A8;
   trace_t var_44_trace;
   pmtrace_t * var_ac_pmtrace;
   edict_t * var_4_ent;


   if(global_host_client->padding2484_NextRunTime > global_realtime) {

      global_host_client->padding247C_LastRunTime += ucmd->msec / 1000;
      return;
   }

   global_host_client->padding2484_NextRunTime = 0;
   if(ucmd->msec > 50) {

      var_7C_usercmd = *ucmd;

      oldmsec = ucmd->msec;
      var_7C_usercmd.msec = oldmsec/2;
      SV_RunCmd(&var_7C_usercmd, seed);

      var_7C_usercmd.msec = oldmsec/2 + (oldmsec & 1); //give them back thier msec.
      var_7C_usercmd.impulse = 0;
      SV_RunCmd(&var_7C_usercmd, seed);
      return;
   }

   if(global_host_client->padding2438_IsFakeClient == 0) {
      SV_SetupMove(global_host_client);
   }

   //I don't know if CmdStart is allowed to alter the usercmd pointer, but just in case...
   var_7C_usercmd = *ucmd;
   gEntityInterface.pfnCmdStart(global_sv_player, ucmd, seed);

   var_48_tempframetime = ucmd->msec * 0.001;
   global_host_client->padding24A4 += var_48_tempframetime;
   global_host_client->padding247C_LastRunTime += var_48_tempframetime; //ucmd->msec / 1000;

   if(ucmd->impulse != 0) {

      global_sv_player->v.impulse = ucmd->impulse;
      if(ucmd->impulse == 0xCC) { //101 is the only one I have memorized

         SV_ForceFullClientsUpdate();
      }
   }

   global_sv_player->v.clbasevelocity[0] = 0;
   global_sv_player->v.clbasevelocity[1] = 0;
   global_sv_player->v.clbasevelocity[2] = 0;
   global_sv_player->v.button = ucmd->buttons;

   SV_CheckMovingGround(global_sv_player, var_48_tempframetime); //inline AMD

   global_pmove->oldangles[0] = global_sv_player->v.v_angle[0];
   global_pmove->oldangles[1] = global_sv_player->v.v_angle[1];
   global_pmove->oldangles[2] = global_sv_player->v.v_angle[2];

   if(global_sv_player->v.fixangle == 0) {

      global_sv_player->v.v_angle[0] = ucmd->viewangles[0];
      global_sv_player->v.v_angle[1] = ucmd->viewangles[1];
      global_sv_player->v.v_angle[2] = ucmd->viewangles[2];
   }

   SV_PlayerRunPreThink(global_sv_player, global_host_client->padding24A4);
   SV_PlayerRunThink(global_sv_player, var_48_tempframetime, global_host_client->padding24A4);

   if(Length(global_sv_player->v.basevelocity) > 0) {

      global_sv_player->v.clbasevelocity[0] = global_sv_player->v.basevelocity[0];
      global_sv_player->v.clbasevelocity[1] = global_sv_player->v.basevelocity[1];
      global_sv_player->v.clbasevelocity[2] = global_sv_player->v.basevelocity[2];
   }

   global_pmove->server = 1;
   global_pmove->multiplayer = 1;
   global_pmove->time = global_host_client->padding24A4 * 1000;

   global_pmove->usehull = ((global_sv_player->v.flags & FL_DUCKING) != 0); //must be the ducking bit.
   global_pmove->maxspeed = cvar_sv_maxspeed.value;
   global_pmove->clientmaxspeed = global_sv_player->v.maxspeed;
   global_pmove->flDuckTime = global_sv_player->v.flDuckTime;
   global_pmove->bInDuck = global_sv_player->v.bInDuck;
   global_pmove->flTimeStepSound = global_sv_player->v.flTimeStepSound;
   global_pmove->iStepLeft = global_sv_player->v.iStepLeft;
   global_pmove->flFallVelocity = global_sv_player->v.flFallVelocity;
   global_pmove->flSwimTime = global_sv_player->v.flSwimTime;
   global_pmove->oldbuttons = global_sv_player->v.oldbuttons;

   Q_strncpy(global_pmove->physinfo, global_host_client->UnknownStruct4DE0, MAX_PHYSINFO_STRING-1);
   global_pmove->physinfo[MAX_PHYSINFO_STRING-1] = '\0';

   global_pmove->velocity[0] = global_sv_player->v.velocity[0];
   global_pmove->velocity[1] = global_sv_player->v.velocity[1];
   global_pmove->velocity[2] = global_sv_player->v.velocity[2];
   global_pmove->movedir[0] = global_sv_player->v.movedir[0];
   global_pmove->movedir[1] = global_sv_player->v.movedir[1];
   global_pmove->movedir[2] = global_sv_player->v.movedir[2];
   global_pmove->angles[0] = global_sv_player->v.v_angle[0];
   global_pmove->angles[1] = global_sv_player->v.v_angle[1];
   global_pmove->angles[2] = global_sv_player->v.v_angle[2];
   global_pmove->basevelocity[0] = global_sv_player->v.basevelocity[0];
   global_pmove->basevelocity[1] = global_sv_player->v.basevelocity[1];
   global_pmove->basevelocity[2] = global_sv_player->v.basevelocity[2];
   global_pmove->view_ofs[0] = global_sv_player->v.view_ofs[0];
   global_pmove->view_ofs[1] = global_sv_player->v.view_ofs[1];
   global_pmove->view_ofs[2] = global_sv_player->v.view_ofs[2];
   global_pmove->punchangle[0] = global_sv_player->v.punchangle[0];
   global_pmove->punchangle[1] = global_sv_player->v.punchangle[1];
   global_pmove->punchangle[2] = global_sv_player->v.punchangle[2];

   global_pmove->effects = global_sv_player->v.effects;
   global_pmove->deadflag = global_sv_player->v.deadflag;
   global_pmove->gravity = global_sv_player->v.gravity;
   global_pmove->friction = global_sv_player->v.friction;
   global_pmove->spectator = 0;
   global_pmove->waterjumptime = global_sv_player->v.teleport_time; //That's an odd one.
   global_pmove->cmd = var_7C_usercmd; //full copy of the pristine usercmd.
   global_pmove->dead = (global_sv_player->v.health <= 0);
   global_pmove->movetype = global_sv_player->v.movetype;
   global_pmove->flags = global_sv_player->v.flags;
   global_pmove->player_index = NUM_FOR_EDICT(global_sv_player) - 1;

   global_pmove->iuser1 = global_sv_player->v.iuser1;
   global_pmove->iuser2 = global_sv_player->v.iuser2;
   global_pmove->iuser3 = global_sv_player->v.iuser3;
   global_pmove->iuser4 = global_sv_player->v.iuser4;
   global_pmove->fuser1 = global_sv_player->v.fuser1;
   global_pmove->fuser2 = global_sv_player->v.fuser2;
   global_pmove->fuser3 = global_sv_player->v.fuser3;
   global_pmove->fuser4 = global_sv_player->v.fuser4;
   global_pmove->vuser1[0] = global_sv_player->v.vuser1[0];
   global_pmove->vuser1[1] = global_sv_player->v.vuser1[1];
   global_pmove->vuser1[2] = global_sv_player->v.vuser1[2];
   global_pmove->vuser2[0] = global_sv_player->v.vuser2[0];
   global_pmove->vuser2[1] = global_sv_player->v.vuser2[1];
   global_pmove->vuser2[2] = global_sv_player->v.vuser2[2];
   global_pmove->vuser3[0] = global_sv_player->v.vuser3[0];
   global_pmove->vuser3[1] = global_sv_player->v.vuser3[1];
   global_pmove->vuser3[2] = global_sv_player->v.vuser3[2];
   global_pmove->vuser4[0] = global_sv_player->v.vuser4[0];
   global_pmove->vuser4[1] = global_sv_player->v.vuser4[1];
   global_pmove->vuser4[2] = global_sv_player->v.vuser4[2];

   global_pmove->origin[0] = global_sv_player->v.origin[0];
   global_pmove->origin[1] = global_sv_player->v.origin[1];
   global_pmove->origin[2] = global_sv_player->v.origin[2];

   SV_AddLinksToPM(global_sv_areanodes, global_pmove->origin);

   global_pmove->frametime = var_48_tempframetime;
   global_pmove->runfuncs = 1;
   global_pmove->PM_PlaySound = PM_SV_PlaySound; //functions
   global_pmove->PM_TraceTexture = PM_SV_TraceTexture;
   global_pmove->PM_PlaybackEventFull = PM_SV_PlaybackEventFull;

   gEntityInterface.pfnPM_Move(global_pmove, 1); //Our crashing issues often crop up here

   global_sv_player->v.deadflag = global_pmove->deadflag;
   global_sv_player->v.effects = global_pmove->effects;
   global_sv_player->v.teleport_time = global_pmove->waterjumptime;
   global_sv_player->v.waterlevel = global_pmove->waterlevel;
   global_sv_player->v.watertype = global_pmove->watertype;
   global_sv_player->v.flags = global_pmove->flags;
   global_sv_player->v.friction = global_pmove->friction;
   global_sv_player->v.movetype = global_pmove->movetype;
   global_sv_player->v.maxspeed = global_pmove->clientmaxspeed;
   global_sv_player->v.iStepLeft = global_pmove->iStepLeft;
   global_sv_player->v.view_ofs[0] = global_pmove->view_ofs[0];
   global_sv_player->v.view_ofs[1] = global_pmove->view_ofs[1];
   global_sv_player->v.view_ofs[2] = global_pmove->view_ofs[2];
   global_sv_player->v.movedir[0] = global_pmove->movedir[0];
   global_sv_player->v.movedir[1] = global_pmove->movedir[1];
   global_sv_player->v.movedir[2] = global_pmove->movedir[2];
   global_sv_player->v.punchangle[0] = global_pmove->punchangle[0];
   global_sv_player->v.punchangle[1] = global_pmove->punchangle[1];
   global_sv_player->v.punchangle[2] = global_pmove->punchangle[2];

   if(global_pmove->onground == -1) {
      global_sv_player->v.flags &= ~FL_ONGROUND;
   }
   else {
      global_sv_player->v.flags |= FL_ONGROUND;
      global_sv_player->v.groundentity = EDICT_NUM(global_pmove->physents[global_pmove->onground].info);
   }

   global_sv_player->v.origin[0] = global_pmove->origin[0];
   global_sv_player->v.origin[1] = global_pmove->origin[1];
   global_sv_player->v.origin[2] = global_pmove->origin[2];
   global_sv_player->v.velocity[0] = global_pmove->velocity[0];
   global_sv_player->v.velocity[1] = global_pmove->velocity[1];
   global_sv_player->v.velocity[2] = global_pmove->velocity[2];
   global_sv_player->v.basevelocity[0] = global_pmove->basevelocity[0];
   global_sv_player->v.basevelocity[1] = global_pmove->basevelocity[1];
   global_sv_player->v.basevelocity[2] = global_pmove->basevelocity[2];

   if(global_sv_player->v.fixangle == 0) {

      //global_sv_player->v.angles[0] = global_pmove->angles[0];
      global_sv_player->v.angles[1] = global_pmove->angles[1];
      global_sv_player->v.angles[2] = global_pmove->angles[2];
      global_sv_player->v.v_angle[0] = global_pmove->angles[0];
      global_sv_player->v.v_angle[1] = global_pmove->angles[1];
      global_sv_player->v.v_angle[2] = global_pmove->angles[2];

      //why?
      global_sv_player->v.angles[0] = -global_pmove->angles[0] / 3;
   }

   global_sv_player->v.bInDuck = global_pmove->bInDuck;
   global_sv_player->v.flDuckTime = global_pmove->flDuckTime;
   global_sv_player->v.flTimeStepSound = global_pmove->flTimeStepSound;
   global_sv_player->v.flFallVelocity = global_pmove->flFallVelocity;
   global_sv_player->v.flSwimTime = global_pmove->flSwimTime;
   global_sv_player->v.oldbuttons = global_pmove->cmd.buttons;

   global_sv_player->v.iuser1 = global_pmove->iuser1;
   global_sv_player->v.iuser2 = global_pmove->iuser2;
   global_sv_player->v.iuser3 = global_pmove->iuser3;
   global_sv_player->v.iuser4 = global_pmove->iuser4;
   global_sv_player->v.fuser1 = global_pmove->fuser1;
   global_sv_player->v.fuser2 = global_pmove->fuser2;
   global_sv_player->v.fuser3 = global_pmove->fuser3;
   global_sv_player->v.fuser4 = global_pmove->fuser4;
   global_sv_player->v.vuser1[0] = global_pmove->vuser1[0];
   global_sv_player->v.vuser1[1] = global_pmove->vuser1[1];
   global_sv_player->v.vuser1[2] = global_pmove->vuser1[2];
   global_sv_player->v.vuser2[0] = global_pmove->vuser2[0];
   global_sv_player->v.vuser2[1] = global_pmove->vuser2[1];
   global_sv_player->v.vuser2[2] = global_pmove->vuser2[2];
   global_sv_player->v.vuser3[0] = global_pmove->vuser3[0];
   global_sv_player->v.vuser3[1] = global_pmove->vuser3[1];
   global_sv_player->v.vuser3[2] = global_pmove->vuser3[2];
   global_sv_player->v.vuser4[0] = global_pmove->vuser4[0];
   global_sv_player->v.vuser4[1] = global_pmove->vuser4[1];
   global_sv_player->v.vuser4[2] = global_pmove->vuser4[2];

   SetMinMaxSize(global_sv_player, global_player_mins[global_pmove->usehull], global_player_maxs[global_pmove->usehull], 0);


   if(global_host_client->padding4A84 != NULL) {

      SV_LinkEdict(global_sv_player, 1);
      var_A8[0] = global_sv_player->v.velocity[0];
      var_A8[1] = global_sv_player->v.velocity[1];
      var_A8[2] = global_sv_player->v.velocity[2];

      for(i = 0; i < global_pmove->numtouch; i++) {

         var_ac_pmtrace = &global_pmove->touchindex[i];
         var_4_ent = EDICT_NUM(global_pmove->physents[var_ac_pmtrace->hitgroup].info);

         SV_ConvertPMTrace(&var_44_trace, var_ac_pmtrace, var_4_ent);

         global_sv_player->v.velocity[0] = var_ac_pmtrace->deltavelocity[0];
         global_sv_player->v.velocity[1] = var_ac_pmtrace->deltavelocity[1];
         global_sv_player->v.velocity[2] = var_ac_pmtrace->deltavelocity[2];

         SV_Impact(var_4_ent, global_sv_player, &var_44_trace);
      }

      global_sv_player->v.velocity[0] = var_A8[0];
      global_sv_player->v.velocity[1] = var_A8[1];
      global_sv_player->v.velocity[2] = var_A8[2];
   }

   gGlobalVariables.time = global_host_client->padding24A4;
   gGlobalVariables.frametime = var_48_tempframetime;

   gEntityInterface.pfnPlayerPostThink(global_sv_player);
   gEntityInterface.pfnCmdEnd(global_sv_player);

   if(global_host_client->padding2438_IsFakeClient == 0) {

      SV_RestoreMove(global_host_client);
   }

   //Wow.
}



//And our init function

/* DESCRIPTION: SV_Init_Move
// PATH: SV_Init
//
// This registers movement cvars, in order to keep them static.
*/
void SV_Movevars_Init() {

   Cvar_RegisterVariable(&cvar_sv_gravity);
   Cvar_RegisterVariable(&cvar_sv_stopspeed);
   Cvar_RegisterVariable(&cvar_sv_maxspeed);
   Cvar_RegisterVariable(&cvar_sv_spectatormaxspeed);
   Cvar_RegisterVariable(&cvar_sv_accelerate);
   Cvar_RegisterVariable(&cvar_sv_airaccelerate);
   Cvar_RegisterVariable(&cvar_sv_wateraccelerate);
   Cvar_RegisterVariable(&cvar_sv_friction);
   Cvar_RegisterVariable(&cvar_sv_edgefriction);
   Cvar_RegisterVariable(&cvar_sv_waterfriction);
   Cvar_RegisterVariable(&cvar_sv_entgravity);
   Cvar_RegisterVariable(&cvar_sv_bounce);
   Cvar_RegisterVariable(&cvar_sv_stepsize);
   Cvar_RegisterVariable(&cvar_sv_maxvelocity);
   Cvar_RegisterVariable(&cvar_sv_zmax);
   Cvar_RegisterVariable(&cvar_sv_wateramp);
   Cvar_RegisterVariable(&cvar_sv_footsteps);

   Cvar_RegisterVariable(&cvar_sv_rollangle);
   Cvar_RegisterVariable(&cvar_sv_rollspeed);
   Cvar_RegisterVariable(&cvar_sv_skycolor_r);
   Cvar_RegisterVariable(&cvar_sv_skycolor_g);
   Cvar_RegisterVariable(&cvar_sv_skycolor_b);
   Cvar_RegisterVariable(&cvar_sv_skyvec_x);
   Cvar_RegisterVariable(&cvar_sv_skyvec_y);
   Cvar_RegisterVariable(&cvar_sv_skyvec_z);
   Cvar_RegisterVariable(&cvar_sv_skyname);
}
