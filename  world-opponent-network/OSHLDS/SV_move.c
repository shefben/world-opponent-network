#include "SV_move.h"

#include "host.h"
#include "Modding.h"
#include "NET.h"
#include "R.h"
#include "random.h"
#include "SV_mcast.h"
#include "SV_phys.h"
#include "world.h"

/*** Things with 'Move' in the title ***/
/* DESCRIPTION: SV_ClipMoveToEntity/SV_SingleClipMoveToEntity
// LOCATION: world.c
// PATH: Mostly in this file.
//
// SV_ClipMoveToEntity is a proxy (and only access point) for the next one.
//
// The function seems to be related to collision detection.
*/
void SV_SingleClipMoveToEntity(edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, trace_t * Outgoing_Trace) {

   vec3_t var_c;
   vec3_t var_18;
   vec3_t var_24;
   hull_t * var_28;
//   int var_2c;
   int var_30;
   int var_34;
   int var_38;

   vec3_t forward;
   vec3_t right;
   vec3_t up;
   vec3_t var_68;
   int var_6c;
   trace_t var_a4;
   vec3_t var_bc;
   vec3_t var_c8;
   vec3_t var_d4;

   Q_memset(Outgoing_Trace, 0, sizeof(trace_t));

   Outgoing_Trace->fraction = 1.0;
   Outgoing_Trace->allsolid = 1;

   Outgoing_Trace->endpos[0] = end[0];
   Outgoing_Trace->endpos[1] = end[1];
   Outgoing_Trace->endpos[2] = end[2];

   if(global_sv.models[ent->v.modelindex]->modeltype == 3) {
      //taken: 1616, -528, 127.97 endpos

      var_28 = SV_HullForStudioModel(ent, mins, maxs, var_c, &var_30);
   }
   else {
      var_28 = SV_HullForEntity(ent, mins, maxs, var_c);
      var_30 = 1; //if model?
   }

   VectorSubtract(start, var_c, var_18);
   VectorSubtract(end, var_c, var_24);

   if((ent->v.solid != SOLID_BSP) ||
      (ent->v.angles[0] == 0 && ent->v.angles[1] == 0 && ent->v.angles[2] == 0)) {

      //taken
      var_38 = 0; //rotated prob
   }
   else { var_38 = 1; }

   if(var_38) { //This is another function in QW.

      AngleVectors(ent->v.angles, forward, right, up); //v.angles is def +58.

      VectorCopy(var_18, var_68);

      var_18[0] = DotProduct(var_68, forward);
      var_18[1] = -DotProduct(var_68, right);
      var_18[2] = DotProduct(var_68, up);

      VectorCopy(var_24, var_68);

      var_24[0] = DotProduct(var_68, forward);
      var_24[1] = -DotProduct(var_68, right);
      var_24[2] = DotProduct(var_68, up);
   } //Curiously enough, I'm taking linear algebra right now.  I hate it.

   //= 20
   if(var_30 == 1) {
      SV_RecursiveHullCheck(var_28, var_28->firstclipnode, 0, 1.0, var_18, var_24, Outgoing_Trace);
   }
   else {
      for(var_6c = 0, var_34 = 0; var_34 < var_30; var_34++) {

         Q_memset(&var_a4, 0, sizeof(trace_t));
         var_a4.fraction = 1.0;
         var_a4.allsolid = 1;
         VectorCopy(end, var_a4.endpos);

         SV_RecursiveHullCheck(&(var_28[var_34]), var_28[var_34].firstclipnode, 0, 1.0, var_18, var_24, &var_a4);

         if(var_34 != 0 && var_a4.startsolid == 0 && var_a4.allsolid == 0 &&
            var_a4.fraction > Outgoing_Trace->fraction) { continue; }

         if(Outgoing_Trace->startsolid == 0) {
            Q_memcpy(Outgoing_Trace, &var_a4, sizeof(trace_t));
         }
         else {
            Q_memcpy(Outgoing_Trace, &var_a4, sizeof(trace_t)); //Probably could be a bit more efficient here.
            Outgoing_Trace->startsolid = 1;
         }
         var_6c = var_34;
      }
      Outgoing_Trace->hitgroup = SV_HitgroupForStudioHull(var_6c);
   }

   if(Outgoing_Trace->fraction != 1.0) {
      if(var_38 != 0) {

         AngleVectorsTranspose(ent->v.angles, var_68, var_bc, var_c8);

         VectorCopy(Outgoing_Trace->plane.normal, var_d4);
         Outgoing_Trace->plane.normal[0] = DotProduct(var_d4, var_68);
         Outgoing_Trace->plane.normal[1] = -DotProduct(var_d4, var_bc);
         Outgoing_Trace->plane.normal[2] = DotProduct(var_d4, var_c8);
      }
      //Scaling, clearly.
      Outgoing_Trace->endpos[0] = start[0] + (Outgoing_Trace->fraction * (end[0] - start[0]));
      Outgoing_Trace->endpos[1] = start[1] + (Outgoing_Trace->fraction * (end[1] - start[1]));
      Outgoing_Trace->endpos[2] = start[2] + (Outgoing_Trace->fraction * (end[2] - start[2]));
   }
   if(Outgoing_Trace->fraction < 1.0 || Outgoing_Trace->startsolid != 0) {
      Outgoing_Trace->pHit = ent;
   }
}
trace_t * SV_ClipMoveToEntity(trace_t * Outgoing_Trace, edict_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end) {

   SV_SingleClipMoveToEntity(ent, start, mins, maxs, end, Outgoing_Trace);
   return(Outgoing_Trace);
}


/* DESCRIPTION: SV_MoveBounds
// LOCATION: world.c
// PATH:SV_Move (and SV_MoveNoEnts)
//
// There are a lot of entities.  Do we really need to test and see if we
// collided against each and every one?  No, we don't, and I *think*
// that's what this function is for.
//
// We can force HL to check against all entities by setting the values to
// really high numbers.  Said so in the QW code...
*/
void SV_MoveBounds(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, vec3_t boxmins, vec3_t boxmaxs) {

   int i;

   for(i=0 ; i<3 ; i++) {
      if (end[i] > start[i]) {
         boxmins[i] = start[i] + mins[i] - 1;
         boxmaxs[i] = end[i] + maxs[i] + 1;
      }
      else {
         boxmins[i] = end[i] + mins[i] - 1;
         boxmaxs[i] = start[i] + maxs[i] + 1;
      }
   }
}

/* DESCRIPTION: SV_Move
// LOCATION: world.c
// PATH: A few places in the game logic
//
// Seems to be responsible for handling movements in the virtual world.
// Returns a trace_t struct, which is actually performed with some compiler
// magic--arg0 is used to store the pointer, and it's copied upon the
// function's return.
*/
trace_t SV_Move(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type, edict_t *passedict, int hullnumthing) {

   moveclip_t clip;
   vec3_t var_94_end; //identical to declaring vec_t var_94[3]
   float var_98;

   memset(&clip, 0, sizeof(moveclip_t));
   SV_ClipMoveToEntity(&(clip.trace), global_sv.edicts, start, mins, maxs, end);

   if(clip.trace.fraction != 0) {

      var_94_end[0] = clip.trace.endpos[0];
      var_94_end[1] = clip.trace.endpos[1];
      var_94_end[2] = clip.trace.endpos[2];

      clip.end = var_94_end;
      var_98 = clip.trace.fraction;
      clip.trace.fraction = 1.0;
      clip.start = start;
      clip.mins = mins;
      clip.maxs = maxs;
      clip.type1 = type & 0xff; //Dunno why...
      clip.type2 = type << 8;
      clip.passedict = passedict;
      clip.hullnum = hullnumthing;

      if(type == 2) {

         clip.mins2[0] = -15.0;
         clip.mins2[1] = -15.0;
         clip.mins2[2] = -15.0;
         clip.maxs2[0] =  15.0;
         clip.maxs2[1] =  15.0;
         clip.maxs2[2] =  15.0;
      }
      else {

         clip.mins2[0] = mins[0];
         clip.mins2[1] = mins[1];
         clip.mins2[2] = mins[2];
         clip.maxs2[0] = maxs[0];
         clip.maxs2[1] = maxs[1];
         clip.maxs2[2] = maxs[2];
      }

      SV_MoveBounds(start, clip.mins2, clip.maxs2, var_94_end, clip.boxmins, clip.boxmaxs);
      SV_ClipToLinks(global_sv_areanodes, &clip);

      clip.trace.fraction *= var_98;

      gGlobalVariables.trace_ent = clip.trace.pHit;
   }
   return(clip.trace);
}/* DESCRIPTION: SV_MoveNoEnts
// LOCATION:
// PATH: PF_walkmove_I->SV_movetest
//
// Same as above, but does not check for collision with entities.
*/
trace_t SV_MoveNoEnts(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type, edict_t *passedict) {

   moveclip_t clip;
   vec3_t tempend; //identical to declaring vec_t var_94[3]
   float tempfraction;


   memset(&clip, 0, sizeof(moveclip_t));
   SV_ClipMoveToEntity(&(clip.trace), global_sv.edicts, start, mins, maxs, end);

   if(clip.trace.fraction != 0) {

      tempend[0] = clip.trace.endpos[0];
      tempend[1] = clip.trace.endpos[1];
      tempend[2] = clip.trace.endpos[2];

      clip.end = tempend;
      tempfraction = clip.trace.fraction;
      clip.trace.fraction = 1.0;
      clip.start = start;
      clip.mins = mins;
      clip.maxs = maxs;
      clip.type1 = type & 0xff;
      clip.type2 = type << 8;
      clip.passedict = passedict;
      clip.hullnum = 0;

      clip.mins2[0] = mins[0];
      clip.mins2[1] = mins[1];
      clip.mins2[2] = mins[2];
      clip.maxs2[0] = maxs[0];
      clip.maxs2[1] = maxs[1];
      clip.maxs2[2] = maxs[2];

      SV_MoveBounds(start, clip.mins2, clip.maxs2, tempend, clip.boxmins, clip.boxmaxs);
      SV_ClipToWorldBrush(global_sv_areanodes, &clip);

      clip.trace.fraction *= tempfraction;
      gGlobalVariables.trace_ent = clip.trace.pHit;
   }
   return(clip.trace);
}

void PF_WaterMove(edict_t * ent) {

   double var_10;
   int var_4_flags;
   int var_8_waterlevel;
   int var_c_watertype;
  // double var_14; //some sort of damage counter, but it keeps not being used...


   if(ent->v.movetype == MOVETYPE_NOCLIP) {

      ent->v.air_finished = 12 + global_sv.time0c;
      return;
   }

   if(ent->v.health <= 0) { return; }

   if(ent->v.deadflag == 0) {
      var_10 = 3;
   }
   else {
      var_10 = 1;
   }

   var_4_flags = ent->v.flags;
   var_8_waterlevel = ent->v.waterlevel;
   var_c_watertype = ent->v.watertype;

   //This area is almost but not quite entirely unlike the drowning code in the SDK.
   if((var_4_flags & (FL_IMMUNE_WATER | FL_GODMODE)) == 0) { //Gods don't drown, unless they're Bruce Willis

      if(((var_4_flags & FL_SWIM) && var_8_waterlevel > var_10) ||
         var_8_waterlevel <= var_10) {

         if(ent->v.air_finished > global_sv.time0c && ent->v.pain_finished > global_sv.time0c) {

            ent->v.dmg += 2;
            if(ent->v.dmg < 15) {

               ent->v.dmg = 10; //wha?
            }

            //var_14 = ent->v.dmg;
            ent->v.pain_finished = 1 + global_sv.time0c;
         }
      }
      else {

        // if(ent->v.air_finished <= global_sv.time0c) {
        //    if(ent->v.air_finished <= 0 + global_sv.time0c) { }
        // }

         ent->v.air_finished = 12 + global_sv.time0c;
         ent->v.dmg = 2;
      }
   }

   //This is asmess
   if(var_8_waterlevel == 0) {

      if((var_4_flags & FL_INWATER) != 0) {

         //Leave the water.
         switch(RandomLong(0, 3)) {

         case 0:
            SV_StartSound(0, ent, 4, "player/pl_wade1.wav", 255, 0.8, 0, 100);
            break;

         case 1:
            SV_StartSound(0, ent, 4, "player/pl_wade2.wav", 255, 0.8, 0, 100);
            break;

         case 2:
            SV_StartSound(0, ent, 4, "player/pl_wade3.wav", 255, 0.8, 0, 100);
            break;

         case 3:
            SV_StartSound(0, ent, 4, "player/pl_wade4.wav", 255, 0.8, 0, 100);
            break;
         }

         ent->v.flags = var_4_flags & ~FL_INWATER;
      }
      ent->v.air_finished = 12 + global_sv.time0c;
      return;
   }


   if(var_c_watertype == Q1CONTENTS_LAVA) { //aah, it stings and burns
      if(((var_4_flags & (FL_IMMUNE_LAVA | FL_GODMODE)) == 0) &&
         ent->v.dmgtime < global_sv.time0c) {

         if(ent->v.radsuit_finished < global_sv.time0c) {
            ent->v.dmgtime = global_sv.time0c + 0.2;
         }
         else {
            ent->v.dmgtime = global_sv.time0c + 1.0;
         }
        // var_14 = var_8_waterlevel * 10;
      }
   }
   else if(var_c_watertype == Q1CONTENTS_SLIME) {

      if(((var_4_flags & (FL_IMMUNE_SLIME | FL_GODMODE)) == 0) &&
           ent->v.dmgtime < global_sv.time0c &&
           ent->v.radsuit_finished < global_sv.time0c) {

         ent->v.dmgtime = global_sv.time0c + 1.0;
        // var_14 = var_8_waterlevel * 4;
      }
   }

   if((var_4_flags & FL_INWATER) == 0) {

      if(var_c_watertype == Q1CONTENTS_WATER) {

         switch(RandomLong(0, 3)) {

         case 0:
            SV_StartSound(0, ent, 4, "player/pl_wade1.wav", 255, 0.8, 0, 100);
            break;

         case 1:
            SV_StartSound(0, ent, 4, "player/pl_wade2.wav", 255, 0.8, 0, 100);
            break;

         case 2:
            SV_StartSound(0, ent, 4, "player/pl_wade3.wav", 255, 0.8, 0, 100);
            break;

         case 3:
            SV_StartSound(0, ent, 4, "player/pl_wade4.wav", 255, 0.8, 0, 100);
            break;
         }
      }

      ent->v.flags = var_4_flags | FL_INWATER;
      ent->v.dmgtime = 0;
   }

   if((var_4_flags & FL_WATERJUMP) == 0) {

      VectorMA(ent->v.velocity, (ent->v.waterlevel * -0.8 * host_frametime), ent->v.velocity, ent->v.velocity);
   }
}


/* DESCRIPTION: SV_movestep/movetest
// LOCATION: sv_move.c
// PATH: PF_walkmove_I
//
// (from QW): Called by monster program code.
// The move will be adjusted for slopes and stairs, but if the move isn't
// possible, no move is done, false is returned, and
// pr_global_struct->trace_normal is set to the normal of the blocking wall
*/
qboolean SV_movestep(edict_t *ent, vec3_t move, qboolean relink) {

   int i, var_6C_hullnum;
   float dz;
   trace_t trace;
   vec3_t oldorg, neworg, end;
   edict_t * enemy;


   if(ent->v.flags & FL_MONSTERCLIP) { //Don't know what one has to do with the other.
      var_6C_hullnum = 1;
   }
   else {
      var_6C_hullnum = 0;
   }

   oldorg[0] = ent->v.origin[0];
   oldorg[1] = ent->v.origin[1];
   oldorg[2] = ent->v.origin[2];

   neworg[0] = ent->v.origin[0] + move[0];
   neworg[1] = ent->v.origin[1] + move[1];
   neworg[2] = ent->v.origin[2] + move[2];


   //well, try it.  Flying and swimming monsters are easiest.
   if(ent->v.flags & (FL_SWIM | FL_FLY)) {

      // try one move with vertical motion, then one without
      for(i = 0; i < 2; i++) {

         neworg[0] = ent->v.origin[0] + move[0];
         neworg[1] = ent->v.origin[1] + move[1];
         neworg[2] = ent->v.origin[2] + move[2];

         enemy = ent->v.enemy;
         if(i == 0 && enemy != NULL) {

            dz = ent->v.origin[2] - enemy->v.origin[2];

            if(dz > 40) { neworg[2] -= 8; }
            else if(dz < 30) { neworg[2] += 8; }
         }

         trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, neworg, 0, ent, var_6C_hullnum);

         if(trace.fraction == 1) { //yaay

            global_g_groupmask = ent->v.groupinfo;

            if((ent->v.flags & FL_SWIM) && SV_PointContents(trace.endpos) == Q1CONTENTS_EMPTY) {
               return(0); //That move takes us out of the water.  Apparently though, it's okay to travel into solids, lava, sky, etc :)
            }

            ent->v.origin[0] = trace.endpos[0];
            ent->v.origin[1] = trace.endpos[1];
            ent->v.origin[2] = trace.endpos[2];

            if(relink != 0) {
               SV_LinkEdict(ent, 1);
            }
            return(1);
         }
         else {

            if(enemy == NULL) { break; }
         }
      }

      return(0);
   }
   else {

      dz = cvar_sv_stepsize.value;
      neworg[2] += dz;
      end[0] = neworg[0];
      end[1] = neworg[1];
      end[2] = neworg[2];
      end[2] -= dz*2;

      trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, 0, ent, var_6C_hullnum);
      if(trace.allsolid != 0) { return(0); }

      if(trace.startsolid != 0) {

         neworg[2] -= dz;

         trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, 0, ent, var_6C_hullnum);

         if(trace.allsolid != 0 || trace.startsolid != 0) { return(0); }
      }

      if(trace.fraction == 1) {
         if(ent->v.flags & FL_PARTIALGROUND) { //Whoa, where'd the ground go?

            ent->v.origin[0] += move[0];
            ent->v.origin[1] += move[1];
            ent->v.origin[2] += move[2];

            if(relink != 0) {
               SV_LinkEdict(ent, 1);
            }
            ent->v.flags &= ~FL_ONGROUND;
            return(1);
         }

         return(0); //Fell off like a green koopa.
      }
      else {

         ent->v.origin[0] = trace.endpos[0];
         ent->v.origin[1] = trace.endpos[1];
         ent->v.origin[2] = trace.endpos[2];

         if(SV_CheckBottom(ent) == 0) {
            if(ent->v.flags & FL_PARTIALGROUND) {

               if(relink != 0) {
                  SV_LinkEdict(ent, 1);
               }
               return(1);
            }

            ent->v.origin[0] = oldorg[0];
            ent->v.origin[1] = oldorg[1];
            ent->v.origin[2] = oldorg[2];

            return(0);
         }
         else {

            if(ent->v.flags & FL_PARTIALGROUND) {
               ent->v.flags &= ~FL_PARTIALGROUND;
            }

            ent->v.groundentity = trace.pHit;

            if(relink != 0) {
               SV_LinkEdict(ent, 1);
            }
            return(1);
         }
      }
   }
}
qboolean SV_movetest(edict_t *ent, vec3_t move, qboolean relink) {

   float temp;
   vec3_t oldorg, neworg, end;
   trace_t trace;


   oldorg[0] = ent->v.origin[0];
   oldorg[1] = ent->v.origin[1];
   oldorg[2] = ent->v.origin[2];

   neworg[0] = ent->v.origin[0] + move[0];
   neworg[1] = ent->v.origin[1] + move[1];
   neworg[2] = ent->v.origin[2] + move[2];

   temp = cvar_sv_stepsize.value;

   neworg[2] += temp;
   end[0] = neworg[0];
   end[1] = neworg[1];
   end[2] = neworg[2];
   end[2] -= temp*2;

   trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, 0, ent);

   if(trace.allsolid != 0) { return(0); }
   if(trace.startsolid != 0) {

      neworg[2] -= temp;

      trace = SV_MoveNoEnts(neworg, ent->v.mins, ent->v.maxs, end, 0, ent);

      if(trace.allsolid != 0 || trace.startsolid != 0) { return(0); }
   }

   if(trace.fraction == 1) {
      if(ent->v.flags & FL_PARTIALGROUND) {
         ent->v.origin[0] += move[0];
         ent->v.origin[1] += move[1];
         ent->v.origin[2] += move[2];

         if(relink != 0) {
            SV_LinkEdict(ent, 1);
         }
         ent->v.flags &= ~FL_ONGROUND;
         return(1);
      }

      return(0);
   }
   else {

      ent->v.origin[0] = trace.endpos[0];
      ent->v.origin[1] = trace.endpos[1];
      ent->v.origin[2] = trace.endpos[2];

      if(SV_CheckBottom(ent) == 0) {
         if(ent->v.flags & FL_PARTIALGROUND) {

            if(relink != 0) {
               SV_LinkEdict(ent, 1);
            }
            return(1);
         }

         ent->v.origin[0] = oldorg[0];
         ent->v.origin[1] = oldorg[1];
         ent->v.origin[2] = oldorg[2];

         return(0);
      }
      else {

         if(ent->v.flags & FL_PARTIALGROUND) {
            ent->v.flags &= ~FL_PARTIALGROUND;
         }

         ent->v.groundentity = trace.pHit;

         if(relink != 0) {
            SV_LinkEdict(ent, 1);
         }
         return(1);
      }
   }
}

void SV_FixCheckBottom(edict_t *ent) {

   Con_Printf("%s called.\n", __FUNCTION__);
   ent->v.flags |= FL_PARTIALGROUND;
}
qboolean SV_StepDirection(edict_t *ent, float yaw, float dist) {

   int ret;
   vec3_t move;


   yaw = yaw * M_PI * 2.0 / 360.0;
   move[0] = cos(yaw) * dist;
   move[1] = sin(yaw) * dist;
   move[2] = 0;

   ret = SV_movestep(ent, move, 0);
   SV_LinkEdict(ent, 1);

   return(ret);
}
qboolean SV_FlyDirection(edict_t *ent, vec3_t move) {

   int ret;


   ret = SV_movestep(ent, move, 0);
   SV_LinkEdict(ent, 1);

   return(ret);
}

void SV_NewChaseDir2(edict_t *actor, vec3_t destination, float dist) {

   float deltax, deltay;
   float tempdir, olddir, turnaround;
   vec3_t d;

   olddir = anglemod(((int)(actor->v.ideal_yaw / 45.0)) * 45.0); //So, we're shaving down some of the precision.  Ohkay.
   turnaround = anglemod(olddir - 180);

   deltax = destination[0] - actor->v.origin[0];
   deltay = destination[1] - actor->v.origin[1];

   if(deltax > 10) { d[1]= 0; }
   else if(deltax < -10) { d[1]= 180; }
   else { d[1] = -1; } //DI_NODIR, so in this function, -1 is a reserved 'null' like value.

   if(deltay < -10) { d[2] = 270; }
   else if(deltay > 10) { d[2] = 90; }
   else { d[2] = -1; }

   // try direct route
   if(d[1] != -1 && d[2] != -1) {

      if(d[1] == 0) {
         if(d[2] == 90) { tempdir = 45; }
         else { tempdir = 315; }
      }
      else {
         if(d[2] == 90) { tempdir = 135; }
         else { tempdir = 215; }
      }

      if(tempdir != turnaround && SV_StepDirection(actor, tempdir, dist) != 0) {
         return;
      }
   }

   // try other directions
   if(RandomLong(0, 1) != 0 || fabs(deltay) > fabs(deltax)) { //These were originally cast as int before compared.  I cannot think of any compelling reason to do so.

      tempdir = d[1];
      d[1] = d[2];
      d[2] = tempdir;
   }

   if(d[1] != -1 && d[1] != turnaround && SV_StepDirection(actor, d[1], dist) != 0) {
      return;
   }
   if(d[2] != -1 && d[2] != turnaround && SV_StepDirection(actor, d[2], dist) != 0) {
      return;
   }

   /* there is no direct path to the player, so pick another direction */
   if(olddir != -1 && SV_StepDirection(actor, olddir, dist) != 0) {
      return;
   }

   //Fine, just run somewhere.
   if(RandomLong(0, 1) != 0) {
      for(tempdir = 0; tempdir <= 315; tempdir += 45) {
         if(tempdir != turnaround && SV_StepDirection(actor, tempdir, dist) != 0) {
            return;
         }
      }
   }
   else {
      for(tempdir = 315; tempdir >= 0; tempdir -= 45) {
         if(tempdir != turnaround && SV_StepDirection(actor, tempdir, dist) != 0) {
            return;
         }
      }
   }

   //We tried.  Run backwards.  THAT ought to work...
   if(turnaround != -1 && SV_StepDirection(actor, turnaround, dist) != 0) {
      return;
   }

   //Well, we're stuck somehow.
   actor->v.ideal_yaw = olddir;

   // if a bridge was pulled out from underneath a monster, it may not have
   // a valid standing position at all.

   if(SV_CheckBottom(actor) == 0) {
      SV_FixCheckBottom(actor);
   }
}


// MoveToOrigin stuff, accd to sdk, and why would it lie.
#define MOVE_START_TURN_DIST 64 // when this far away from moveGoal, start turning to face next goal
#define MOVE_STUCK_DIST      32 // if a monster can't step this far, it is stuck.
#define MOVE_NORMAL           0 // normal move in the direction monster is facing
#define MOVE_STRAFE           1 // moves in direction specified, no matter which way monster is facing
HLDS_DLLEXPORT void SV_MoveToOrigin_I(edict_t *ent, const vec3_t pflGoal, float dist, int iMoveType) {

   vec3_t var_c_temp;


   var_c_temp[0] = pflGoal[0];
   var_c_temp[1] = pflGoal[1];
   var_c_temp[2] = pflGoal[2];

   if(ent->v.flags & (FL_FLY | FL_SWIM | FL_ONGROUND)) {
      if(iMoveType == MOVE_NORMAL) {

         if(SV_StepDirection(ent, ent->v.ideal_yaw, dist) == 0) {
            SV_NewChaseDir2(ent, var_c_temp, dist);
         }
      }
      else {

         var_c_temp[0] -= ent->v.origin[0];
         var_c_temp[1] -= ent->v.origin[1];

         if(ent->v.flags & (FL_FLY | FL_SWIM)) {
            var_c_temp[2] -= ent->v.origin[2];
         }
         else { //If on ground, I guess we just don't worry about the third axis.  Sorta makes sense.
            var_c_temp[2] = 0;
         }

         VectorNormalize(var_c_temp);
         VectorScale(var_c_temp, dist, var_c_temp);
         SV_FlyDirection(ent, var_c_temp);
      }
   }
}



/*** Traces ***/

/* DESCRIPTION: SV_SetGlobalTrace
// LOCATION:
// PATH: Functions that are about to hand themselves over to mods.
//
// Copies variables to a global variable structure so that a mod
// knows what it is dealing with.
*/
void SV_SetGlobalTrace(trace_t * tr) {

   gGlobalVariables.trace_allsolid = tr->allsolid;
   gGlobalVariables.trace_startsolid = tr->startsolid;
   gGlobalVariables.trace_fraction = tr->fraction;
   gGlobalVariables.trace_inopen = tr->inopen;
   gGlobalVariables.trace_inwater = tr->inwater;

   gGlobalVariables.trace_endpos[0] = tr->endpos[0];
   gGlobalVariables.trace_endpos[1] = tr->endpos[1];
   gGlobalVariables.trace_endpos[2] = tr->endpos[2];
   gGlobalVariables.trace_plane_normal[0] = tr->plane.normal[0];
   gGlobalVariables.trace_plane_normal[1] = tr->plane.normal[1];
   gGlobalVariables.trace_plane_normal[2] = tr->plane.normal[2];
   gGlobalVariables.trace_plane_dist = tr->plane.dist;

   if(tr->pHit == NULL) {
      gGlobalVariables.trace_ent = global_sv.edicts;
   }
   else {
      gGlobalVariables.trace_ent = tr->pHit;
   }

   gGlobalVariables.trace_hitgroup = tr->hitgroup;
}

/* DESCRIPTION: SV_Trace_Toss
// LOCATION: sv_phys.c
// PATH: PF_TraceToss_Shared
//
// Say I throw a grenade.  Where will it go?  This function finds out by
// simulating ten seconds' worth of frames.
*/
trace_t * SV_Trace_Toss(trace_t * Outgoing_Trace, edict_t *tossent, edict_t *ignore) {

   int i;
   vec3_t var_378_end;
   vec3_t var_36C_move;
   trace_t var_360_trace;
   edict_t var_324_TempEdict;
   edict_t * var_328_TempEdictPtr;
   double var_380_host_frametime;

   var_380_host_frametime = host_frametime;
   host_frametime = .05;

   Q_memcpy(&var_324_TempEdict, tossent, sizeof(edict_t));

   var_328_TempEdictPtr = &var_324_TempEdict;

//   while(1) //a sanity check to avoid tracing more than ten seconds was present in QW, I think it's a good idea.  This may need to be adjusted to account for the alterable frametimes.
   for (i = 0; i < 200; i++)
   {
      SV_CheckVelocity(var_328_TempEdictPtr);
      SV_AddGravity(var_328_TempEdictPtr);
      VectorMA(var_328_TempEdictPtr->v.angles, host_frametime, var_328_TempEdictPtr->v.avelocity, var_328_TempEdictPtr->v.angles);
      VectorScale(var_328_TempEdictPtr->v.velocity, host_frametime, var_36C_move);
      VectorAdd(var_328_TempEdictPtr->v.origin, var_36C_move, var_378_end);

      var_360_trace = SV_Move(var_328_TempEdictPtr->v.origin, var_328_TempEdictPtr->v.mins, var_328_TempEdictPtr->v.maxs, var_378_end, 0, var_328_TempEdictPtr, 0);
      VectorCopy(var_360_trace.endpos, var_328_TempEdictPtr->v.origin);

      if(var_360_trace.pHit != NULL && var_360_trace.pHit != ignore) { break; }
   }

   host_frametime = var_380_host_frametime;
   Q_memcpy(Outgoing_Trace, &var_360_trace, sizeof(trace_t));
   return(Outgoing_Trace);
}
