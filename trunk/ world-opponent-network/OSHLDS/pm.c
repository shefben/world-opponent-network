#include <time.h>

#include "pm.h"
#include "Mod.h"
#include "R.h"
#include "random.h"
#include "SV_hlds.h"
#include "SV_move.h"
#include "SV_phys.h"
#include "sys.h"
#include "VecMath.h"
#include "Q_memdef.h"
#include "report.h"

//Empty and test functions
HLDS_DLLEXPORT void CL_Particle() { } //These things are always empty.
HLDS_DLLEXPORT void Con_NPrintf() { } //This was already empty
//void our_PM_TestPlayerPosition() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_StuckTouch() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_PointContents() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_TruePointContents() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_HullPointContents() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_PlayerTrace() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_TraceLine() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_GetModelType() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_GetModelBounds() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_HullForBsp() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_TraceModel() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_PlayerTraceEx() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_TestPlayerPositionEx() { printf("%s called", __FUNCTION__); exit(99); }
//void our_PM_TraceLineEx() { printf("%s called", __FUNCTION__); exit(99); }


//Gs
static hull_t      box_hull_0;
static dclipnode_t box_clipnodes_0[6];
static mplane_t    box_planes_0[6];



//These two are almost identical to the world.c version.
HLDS_DLLEXPORT int  our_PM_HullPointContents(hull_t * arg_0_hull, int num, vec3_t p) {

   float d;
   dclipnode_t * node;
   mplane_t * plane;


   //Say hello to the difference.
   if(arg_0_hull->firstclipnode >= arg_0_hull->lastclipnode) {
      return(Q1CONTENTS_EMPTY);
   }


   while(num >= 0) {

      if(num < arg_0_hull->firstclipnode || num > arg_0_hull->lastclipnode) {
         Sys_Error("%s: bad node number", __FUNCTION__);
      }

      node = &(arg_0_hull->clipnodes[num]);
      plane = &(arg_0_hull->planes[node->planenum]);

      if(plane->type < 3) {
         d = p[plane->type] - plane->dist;
      }
      else {
         d = ((plane->normal[0] * p[0]) +
              (plane->normal[1] * p[1]) +
              (plane->normal[2] * p[2]) -
               plane->dist);
      }

      if(d < 0) {
         num = node->children[1];
      }
      else {
         num = node->children[0];
      }
   }

   return(num);
}
int  our_PM_RecursiveHullCheck(hull_t * arg_0_hull, int arg_4_num, float arg_8_0, float arg_c_1, vec_t * arg_10_start, vec_t * arg_14_end, pmtrace_t * arg_18_trace) {

   int var_18_i, var_28;
   dclipnode_t * var_4_clipnode;
   mplane_t * var_8_plane;
   float var_c_t1;
   float var_10_t2;
   float var_14_frac;
   float var_2c_midf;
   vec3_t var_24_mid;



   if(arg_4_num < 0) {
      if(arg_4_num == Q1CONTENTS_SOLID) {
         arg_18_trace->startsolid = 1;
      }
      else {
         arg_18_trace->allsolid = 0;

         if(arg_4_num == Q1CONTENTS_EMPTY) {
            arg_18_trace->inopen = 1;
         }
         else {
            arg_18_trace->inwater = 1;
         }
      }
      return(1);
   }
   if(arg_0_hull->firstclipnode >= arg_0_hull->lastclipnode) { //Okay, a little different
      arg_18_trace->allsolid = 0;
      arg_18_trace->inopen = 1;
      return(1);
   }

   var_4_clipnode = &(arg_0_hull->clipnodes[arg_4_num]);
   var_8_plane = &(arg_0_hull->planes[var_4_clipnode->planenum]);
   if(var_8_plane->type < 3) {

      var_c_t1 = arg_10_start[var_8_plane->type] - var_8_plane->dist;
      var_10_t2 = arg_14_end[var_8_plane->type] - var_8_plane->dist;
   }
   else {

      var_c_t1 =  ((var_8_plane->normal[0] * arg_10_start[0]) +
                   (var_8_plane->normal[1] * arg_10_start[1]) +
                   (var_8_plane->normal[2] * arg_10_start[2])) - var_8_plane->dist;
      var_10_t2 = ((var_8_plane->normal[0] * arg_14_end[0]) +
                   (var_8_plane->normal[1] * arg_14_end[1]) +
                   (var_8_plane->normal[2] * arg_14_end[2])) - var_8_plane->dist;
   }

   if(var_c_t1 >= 0 && var_10_t2 >= 0) {
      return(our_PM_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[0], arg_8_0, arg_c_1, arg_10_start, arg_14_end, arg_18_trace));
   }
   if(var_c_t1 < 0 && var_10_t2 < 0) {
      return(our_PM_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[1], arg_8_0, arg_c_1, arg_10_start, arg_14_end, arg_18_trace));
   }

   if(var_c_t1 < 0) {
      var_14_frac = (var_c_t1 + DIST_EPSILON) / (var_c_t1 - var_10_t2);
   }
   else {
      var_14_frac = (var_c_t1 - DIST_EPSILON) / (var_c_t1 - var_10_t2);
   }

   if(var_14_frac < 0) { var_14_frac = 0; }
   else if(var_14_frac > 1) { var_14_frac = 1; }

   var_2c_midf = ((arg_c_1 - arg_8_0) * var_14_frac) + arg_8_0;
   for(var_18_i = 0; var_18_i <= 2; var_18_i++) {
      var_24_mid[var_18_i] = ((arg_14_end[var_18_i] - arg_10_start[var_18_i]) * var_14_frac) + arg_10_start[var_18_i];
   }

   if(var_c_t1 < 0) { var_28 = 1; }
   else { var_28 = 0; }

   if(our_PM_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[var_28], arg_8_0, var_2c_midf, arg_10_start, var_24_mid, arg_18_trace) == 0) {
      return(0);
   }
   if(our_PM_HullPointContents(arg_0_hull, var_4_clipnode->children[var_28 ^ 1], var_24_mid) != Q1CONTENTS_SOLID) {
      return(our_PM_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[var_28 ^ 1], var_2c_midf, arg_c_1, var_24_mid, arg_14_end, arg_18_trace));
   }
   if(arg_18_trace->allsolid != 0) { return(0); }

   if(var_28 == 0) {

      arg_18_trace->plane.normal[0] = var_8_plane->normal[0];
      arg_18_trace->plane.normal[1] = var_8_plane->normal[1];
      arg_18_trace->plane.normal[2] = var_8_plane->normal[2];
      arg_18_trace->plane.dist = var_8_plane->dist;
   }
   else {

      arg_18_trace->plane.normal[0] = -var_8_plane->normal[0];
      arg_18_trace->plane.normal[1] = -var_8_plane->normal[1];
      arg_18_trace->plane.normal[2] = -var_8_plane->normal[2];
      arg_18_trace->plane.dist = -var_8_plane->dist;
   }

   while(1) {

      if(our_PM_HullPointContents(arg_0_hull, arg_0_hull->firstclipnode, var_24_mid) != Q1CONTENTS_SOLID) {

         arg_18_trace->fraction = var_2c_midf;
         arg_18_trace->endpos[0] = var_24_mid[0];
         arg_18_trace->endpos[1] = var_24_mid[1];
         arg_18_trace->endpos[2] = var_24_mid[2];
         return(0);
      }
      var_14_frac -= 0.1;
      if(var_14_frac < 0) {
         arg_18_trace->fraction = var_2c_midf;
         arg_18_trace->endpos[0] = var_24_mid[0];
         arg_18_trace->endpos[1] = var_24_mid[1];
         arg_18_trace->endpos[2] = var_24_mid[2];
         Con_Printf("%s: Went past 0 during check.\n", __FUNCTION__);
         return(0);

      }

      var_2c_midf = ((arg_c_1 - arg_8_0) * var_14_frac) + arg_8_0;
      for(var_18_i = 0; var_18_i < 3; var_18_i++) {
         var_24_mid[var_18_i] = ((arg_14_end[var_18_i] - arg_10_start[var_18_i]) * var_14_frac) + arg_10_start[var_18_i];
      }
   }
}

HLDS_DLLEXPORT int  our_PM_GetModelType(model_t * mod) {
   return(mod->modeltype);
}
HLDS_DLLEXPORT void our_PM_GetModelBounds(model_t * mod, vec3_t mins, vec3_t maxs) {

   mins[0] = mod->mins[0];
   mins[1] = mod->mins[1];
   mins[2] = mod->mins[2];
   maxs[0] = mod->maxs[0];
   maxs[1] = mod->maxs[1];
   maxs[2] = mod->maxs[2];
}

HLDS_DLLEXPORT int our_PM_TruePointContents(vec3_t p) {

   hull_t * tmp;


   tmp = global_pmove->physents[0].model->hulls;
   if(tmp == NULL) { return(Q1CONTENTS_SOLID); }

   return(our_PM_HullPointContents(tmp, tmp->firstclipnode, p));
}

HLDS_DLLEXPORT hull_t * our_PM_HullForBsp(physent_t *pe, vec3_t offset) {

   hull_t * var_4_hull;


   switch(global_pmove->usehull) {

   case 1:
      var_4_hull = &(pe->model->hulls[3]);
      break;

   case 2:
      var_4_hull = &(pe->model->hulls[0]);
      break;

   case 3:
      var_4_hull = &(pe->model->hulls[2]);
      break;

   case 0:
   default:
      var_4_hull = &(pe->model->hulls[1]);
   }

   offset[0] = var_4_hull->clip_mins[0] - global_player_mins[global_pmove->usehull][0];
   offset[1] = var_4_hull->clip_mins[1] - global_player_mins[global_pmove->usehull][1];
   offset[2] = var_4_hull->clip_mins[2] - global_player_mins[global_pmove->usehull][2];

   offset[0] += pe->origin[0];
   offset[1] += pe->origin[1];
   offset[2] += pe->origin[2];

   return(var_4_hull);
}
hull_t * our_PM_HullForStudioModel(model_t * ent, vec3_t pos, float frame, int seq, vec3_t angles, vec3_t orig, byte * controller, byte * blending, int * arg_whatever) {

   vec3_t var_c;


   var_c[0] = global_player_maxs[global_pmove->usehull][0] - global_player_mins[global_pmove->usehull][0];
   var_c[1] = global_player_maxs[global_pmove->usehull][1] - global_player_mins[global_pmove->usehull][1];
   var_c[2] = global_player_maxs[global_pmove->usehull][2] - global_player_mins[global_pmove->usehull][2];

   VectorScale(var_c, 0.5, var_c);

   pos[0] = 0;
   pos[1] = 0;
   pos[2] = 0;

   return(R_StudioHull(ent, frame, seq, angles, orig, var_c, controller, blending, arg_whatever, NULL, 0));
}
hull_t * our_PM_HullForBox(vec3_t mins, vec3_t maxs) {

   box_planes_0[0].dist = maxs[0];
   box_planes_0[1].dist = mins[0];
   box_planes_0[2].dist = maxs[1];
   box_planes_0[3].dist = mins[1];
   box_planes_0[4].dist = maxs[2];
   box_planes_0[5].dist = mins[2];

   return(&box_hull_0);
}

pmtrace_t our__PM_PlayerTrace(vec3_t start, vec3_t end, int traceFlags, int numphysent, physent_t * physents, int ignore_pe, int (*pfnIgnore)(physent_t *)) {

   unsigned int i, j, var_D4_HasAngles, var_D8_hullcount, var_110_index;
   pmtrace_t var_48;
   pmtrace_t var_8C;
   pmtrace_t var_154;
   physent_t * var_B8_phent;
   hull_t * var_B4_hull;
   vec3_t var_98, var_A4, var_B0, var_C4_mins, var_D0_maxs;
   vec3_t var_E8_forward, var_F4_right, var_100_up, var_10C_temp;


   Q_memset(&var_8C, 0, sizeof(var_8C));
   var_8C.ent = -1;
   var_8C.fraction = 1;
   var_8C.endpos[0] = end[0];
   var_8C.endpos[1] = end[1];
   var_8C.endpos[2] = end[2];

   for(i = 0; (signed)i < numphysent; i++) {

      var_B8_phent = &(physents[i]);
      if(i != 0 && (traceFlags & 0x80) != 0) { break; }

      if(pfnIgnore != NULL) {
         if(pfnIgnore(var_B8_phent)) { continue; }
      }
      else if(ignore_pe != -1) {
         if(ignore_pe == (signed)i) { continue; }
      }
      if(var_B8_phent->model != NULL && var_B8_phent->solid == 0 && var_B8_phent->skin != 0) { continue; }
      if((traceFlags & 0x04) != 0 && var_B8_phent->rendermode == 0) { continue; }

      var_98[0] = var_B8_phent->origin[0];
      var_98[1] = var_B8_phent->origin[1];
      var_98[2] = var_B8_phent->origin[2];
      var_D8_hullcount = 1;

      if(var_B8_phent->model == NULL) {
         if(var_B8_phent->studiomodel == NULL) {

            var_C4_mins[0] = var_B8_phent->mins[0] - global_player_maxs[global_pmove->usehull][0];
            var_C4_mins[1] = var_B8_phent->mins[1] - global_player_maxs[global_pmove->usehull][1];
            var_C4_mins[2] = var_B8_phent->mins[2] - global_player_maxs[global_pmove->usehull][2];
            var_D0_maxs[0] = var_B8_phent->maxs[0] - global_player_mins[global_pmove->usehull][0];
            var_D0_maxs[1] = var_B8_phent->maxs[1] - global_player_mins[global_pmove->usehull][1];
            var_D0_maxs[2] = var_B8_phent->maxs[2] - global_player_mins[global_pmove->usehull][2];

            var_B4_hull = our_PM_HullForBox(var_C4_mins, var_D0_maxs);
         }
         else {

            if(traceFlags & 1) { continue; }
            if(var_B8_phent->studiomodel->modeltype != 3 ||
              ((var_B8_phent->studiomodel->flags & 0x200) == 0 && (global_pmove->usehull != 2 || (traceFlags & 0x02) != 0))) {

               //This block is identical to the above block with PM_HullForBox.
               var_C4_mins[0] = var_B8_phent->mins[0] - global_player_maxs[global_pmove->usehull][0];
               var_C4_mins[1] = var_B8_phent->mins[1] - global_player_maxs[global_pmove->usehull][1];
               var_C4_mins[2] = var_B8_phent->mins[2] - global_player_maxs[global_pmove->usehull][2];
               var_D0_maxs[0] = var_B8_phent->maxs[0] - global_player_mins[global_pmove->usehull][0];
               var_D0_maxs[1] = var_B8_phent->maxs[1] - global_player_mins[global_pmove->usehull][1];
               var_D0_maxs[2] = var_B8_phent->maxs[2] - global_player_mins[global_pmove->usehull][2];

               var_B4_hull = our_PM_HullForBox(var_C4_mins, var_D0_maxs);
            }
            else {
               var_B4_hull = our_PM_HullForStudioModel(var_B8_phent->studiomodel, var_98, var_B8_phent->frame, var_B8_phent->sequence, var_B8_phent->angles, var_B8_phent->origin, var_B8_phent->controller, var_B8_phent->blending, &var_D8_hullcount);
            }
         }
      }
      else {

         switch(global_pmove->usehull) {

         case 1:
            var_B4_hull = &(var_B8_phent->model->hulls[3]);
            break;

         case 2:
            var_B4_hull = &(var_B8_phent->model->hulls[0]);
            break;

         case 3:
            var_B4_hull = &(var_B8_phent->model->hulls[2]);
            break;

         case 0:
         default:
            var_B4_hull = &(var_B8_phent->model->hulls[1]);
         }

         var_98[0] = var_B4_hull->clip_mins[0] - global_player_mins[global_pmove->usehull][0];
         var_98[1] = var_B4_hull->clip_mins[1] - global_player_mins[global_pmove->usehull][1];
         var_98[2] = var_B4_hull->clip_mins[2] - global_player_mins[global_pmove->usehull][2];

         //I think we can just += this above, as var_98 should already hold these values.
         var_98[0] += var_B8_phent->origin[0];
         var_98[1] += var_B8_phent->origin[1];
         var_98[2] += var_B8_phent->origin[2];

      }

      //These branches have now merged, but I am tired.  Finish later.

      var_A4[0] = start[0] - var_98[0];
      var_A4[1] = start[1] - var_98[1];
      var_A4[2] = start[2] - var_98[2];
      var_B0[0] = end[0] - var_98[0];
      var_B0[1] = end[1] - var_98[1];
      var_B0[2] = end[2] - var_98[2];


      if(var_B8_phent->solid == SOLID_BSP && (var_B8_phent->angles[0] != 0 || var_B8_phent->angles[1] != 0 || var_B8_phent->angles[2] != 0)) {
         var_D4_HasAngles = 1;
      }
      else {
         var_D4_HasAngles = 0;
      }
      if(var_D4_HasAngles != 0) {

         AngleVectors(var_B8_phent->angles, var_E8_forward, var_F4_right, var_100_up);

         var_10C_temp[0] = var_A4[0];
         var_10C_temp[1] = var_A4[1];
         var_10C_temp[2] = var_A4[2];
         var_A4[0] =   (var_10C_temp[0] * var_E8_forward[0]) + (var_10C_temp[1] * var_E8_forward[1]) + (var_10C_temp[2] * var_E8_forward[2]);
         var_A4[1] = -((var_10C_temp[0] * var_F4_right[0]) + (var_10C_temp[1] * var_F4_right[1]) + (var_10C_temp[2] * var_F4_right[2]));
         var_A4[2] =   (var_10C_temp[0] * var_100_up[0]) + (var_10C_temp[1] * var_100_up[1]) + (var_10C_temp[2] * var_100_up[2]);

         var_10C_temp[0] = var_B0[0];
         var_10C_temp[1] = var_B0[1];
         var_10C_temp[2] = var_B0[2];
         var_B0[0] =   (var_10C_temp[0] * var_E8_forward[0]) + (var_10C_temp[1] * var_E8_forward[1]) + (var_10C_temp[2] * var_E8_forward[2]);
         var_B0[1] = -((var_10C_temp[0] * var_F4_right[0]) + (var_10C_temp[1] * var_F4_right[1]) + (var_10C_temp[2] * var_F4_right[2]));
         var_B0[2] =   (var_10C_temp[0] * var_100_up[0]) + (var_10C_temp[1] * var_100_up[1]) + (var_10C_temp[2] * var_100_up[2]);
      }

      Q_memset(&var_48, 0, sizeof(var_48));
      var_48.allsolid = 1;
      var_48.fraction = 1;
      var_48.endpos[0] = end[0];
      var_48.endpos[1] = end[1];
      var_48.endpos[2] = end[2];

      if(var_D8_hullcount < 1) {
         var_48.allsolid = 0;
      }
      else if(var_D8_hullcount == 1) {
         our_PM_RecursiveHullCheck(var_B4_hull, var_B4_hull->firstclipnode, 0, 1, var_A4, var_B0, &var_48);
      }
      else {

         for(var_110_index = 0, j = 0; j < var_D8_hullcount; var_48.hitgroup = SV_HitgroupForStudioHull(var_110_index), j++) {

            Q_memset(&var_154, 0, sizeof(var_154));
            var_154.allsolid = 1;
            var_154.fraction = 1;
            var_154.endpos[0] = end[0];
            var_154.endpos[1] = end[1];
            var_154.endpos[2] = end[2];

            our_PM_RecursiveHullCheck(&var_B4_hull[j], var_B4_hull[j].firstclipnode, 0, 1, var_A4, var_B0, &var_154);

            if(j == 0 || var_154.allsolid != 0 || var_154.startsolid != 0 || var_154.fraction < var_48.fraction) {

               if(var_48.startsolid == 0) {
                  var_48 = var_154;
               }
               else {
                  var_48 = var_154;
                  var_48.startsolid = 1;
               }

               var_110_index = j;
            }
         }
      }


      if(var_48.allsolid != 0) {
         var_48.startsolid = 1;
      }
      if(var_48.startsolid != 0) {
         var_48.fraction = 0;
      }

      if(var_48.startsolid != 1) {
         if(var_D4_HasAngles != 0) {

            AngleVectorsTranspose(var_B8_phent->angles, var_E8_forward, var_F4_right, var_100_up);

            //Hey, we're not doing the negative this time.
            var_10C_temp[0] = var_48.plane.normal[0];
            var_10C_temp[1] = var_48.plane.normal[1];
            var_10C_temp[2] = var_48.plane.normal[2];
            var_48.plane.normal[0] = (var_10C_temp[0] * var_E8_forward[0]) + (var_10C_temp[1] * var_E8_forward[1]) + (var_10C_temp[2] * var_E8_forward[2]);
            var_48.plane.normal[1] = (var_10C_temp[0] * var_F4_right[0]) + (var_10C_temp[1] * var_F4_right[1]) + (var_10C_temp[2] * var_F4_right[2]);
            var_48.plane.normal[2] = (var_10C_temp[0] * var_100_up[0]) + (var_10C_temp[1] * var_100_up[1]) + (var_10C_temp[2] * var_100_up[2]);
         }

         //Methinks we could improve speeds by handling the fraction = 0 case w/0 muls.
         var_48.endpos[0] = ((end[0] - start[0]) * var_48.fraction) + start[0];
         var_48.endpos[1] = ((end[1] - start[1]) * var_48.fraction) + start[1];
         var_48.endpos[2] = ((end[2] - start[2]) * var_48.fraction) + start[2];
      }

      if(var_48.fraction < var_8C.fraction) {

         var_8C = var_48;
         var_8C.ent = i;
      }
   }

   return(var_8C);
}
HLDS_DLLEXPORT pmtrace_t our_PM_PlayerTrace(vec3_t start, vec3_t end, int traceFlags, int ignore_pe) {
   return(our__PM_PlayerTrace(start, end, traceFlags, global_pmove->numphysent, (global_pmove->physents), ignore_pe, NULL));
}
HLDS_DLLEXPORT pmtrace_t our_PM_PlayerTraceEx(vec3_t start, vec3_t end, int traceFlags, int (*pfnIgnore)(physent_t *)) {
   return(our__PM_PlayerTrace(start, end, traceFlags, global_pmove->numphysent, (global_pmove->physents), -1, pfnIgnore));
}

//flags is a little vague on this.  Appears 0x01 == ignoremonsters, 0x0101 == ignoreglass
HLDS_DLLEXPORT pmtrace_t * our_PM_TraceLine(vec3_t start, vec3_t end, int flags, int usehull, int ignore_pe) {

   int i;
   static pmtrace_t tr;


   i = global_pmove->usehull;
   global_pmove->usehull = usehull;

   if(flags == 0) {
      tr = our__PM_PlayerTrace(start, end, 0, global_pmove->numphysent, global_pmove->physents, ignore_pe, NULL);
   }
   else if(flags == 1) {
      tr = our__PM_PlayerTrace(start, end, 0, global_pmove->numvisent, global_pmove->visents, ignore_pe, NULL);
   }
   //Not having a catch-all case unnerves me.

   global_pmove->usehull = i;
   return(&tr);
}
HLDS_DLLEXPORT pmtrace_t * our_PM_TraceLineEx(vec3_t start, vec3_t end, int flags, int usehull, int (*pfnIgnore)(physent_t *pe)) {

   int i;
   static pmtrace_t tr;


   i = global_pmove->usehull;
   global_pmove->usehull = usehull;

   if(flags == 0) {
      tr = our_PM_PlayerTraceEx(start, end, 0, pfnIgnore);
   }
   else {
      tr = our__PM_PlayerTrace(start, end, 0, global_pmove->numvisent, global_pmove->visents, -1, pfnIgnore);
   }

   global_pmove->usehull = i;
   return(&tr);
}

HLDS_DLLEXPORT float our_PM_TraceModel(physent_t *pEnt, vec3_t start, vec3_t end, trace_t *trace) {

   int hullnum;
   hull_t * var_4_hull;
   vec3_t var_14, var_20, var_2C;


   hullnum = global_pmove->usehull;
   global_pmove->usehull = 2;

   var_4_hull = our_PM_HullForBsp(pEnt, var_2C);

   global_pmove->usehull = hullnum;

   var_14[0] = start[0] - var_2C[0];
   var_14[1] = start[1] - var_2C[1];
   var_14[2] = start[2] - var_2C[2];
   var_20[0] = end[0] - var_2C[0];
   var_20[1] = end[1] - var_2C[1];
   var_20[2] = end[2] - var_2C[2];

   //SV, that's right
   SV_RecursiveHullCheck(var_4_hull, var_4_hull->firstclipnode, 0, 1, var_14, var_20, trace);
   trace->pHit = NULL;
   return(trace->fraction);
}

int our__PM_TestPlayerPosition(vec3_t pos, pmtrace_t *ptrace, int (*pfnIgnore)(physent_t *)) {

   unsigned int i, j, var_48;
   int g_contentsresult;
   pmtrace_t var_8C;
   physent_t * ent;
   hull_t * var_3C;
   vec3_t var_14_mins, var_20_maxs, var_2C, var_38, var_98, var_A4, var_B0, var_BC;


   var_8C = our_PM_PlayerTrace(global_pmove->origin, global_pmove->origin, 0, -1);
   if(ptrace != NULL) {
      *ptrace = var_8C;
   }

   for(i = 0; (signed)i < global_pmove->numphysent; i++) {

      ent = &(global_pmove->physents[i]);
      if(pfnIgnore != NULL && pfnIgnore(ent) != 0) { continue; }
      if(ent->model != NULL && ent->solid == 0 && ent->skin != 0) { continue; }

      var_38[0] = ent->origin[0];
      var_38[1] = ent->origin[1];
      var_38[2] = ent->origin[2];
      var_48 = 1;

      if(ent->model != NULL) {
         var_3C = our_PM_HullForBsp(ent, var_38);
      }
      else if(ent->studiomodel == NULL || ent->studiomodel->modeltype != 3 ||
              ((ent->studiomodel->flags & 0x200) == 0 && global_pmove->usehull != 2)) {


         var_14_mins[0] = ent->mins[0] - global_player_maxs[global_pmove->usehull][0];
         var_14_mins[1] = ent->mins[1] - global_player_maxs[global_pmove->usehull][1];
         var_14_mins[2] = ent->mins[2] - global_player_maxs[global_pmove->usehull][2];
         var_20_maxs[0] = ent->maxs[0] - global_player_mins[global_pmove->usehull][0];
         var_20_maxs[1] = ent->maxs[1] - global_player_mins[global_pmove->usehull][1];
         var_20_maxs[2] = ent->maxs[2] - global_player_mins[global_pmove->usehull][2];

         var_3C = our_PM_HullForBox(var_14_mins, var_20_maxs);
      }
      else {

         var_3C = our_PM_HullForStudioModel(ent->studiomodel, var_38, ent->frame, ent->sequence, ent->angles, ent->origin, ent->controller, ent->blending, &var_48);
      }


      var_2C[0] = pos[0] - var_38[0];
      var_2C[1] = pos[1] - var_38[1];
      var_2C[2] = pos[2] - var_38[2];

      if(ent->solid == SOLID_BSP && (var_2C[0] != 0 || var_2C[1] != 0 || var_2C[2] != 0)) {

         AngleVectors(ent->angles, var_98, var_A4, var_B0);

         var_BC[0] = var_2C[0];
         var_BC[1] = var_2C[1];
         var_BC[2] = var_2C[2];

         var_2C[0] =   (var_BC[0] * var_98[0]) + (var_BC[1] * var_98[1]) + (var_BC[2] * var_98[2]);
         var_2C[1] = -((var_BC[0] * var_A4[0]) + (var_BC[1] * var_A4[1]) + (var_BC[2] * var_A4[2]));
         var_2C[2] =   (var_BC[0] * var_B0[0]) + (var_BC[1] * var_B0[1]) + (var_BC[2] * var_B0[2]);
      }

      if(var_48 == 1) {

         g_contentsresult = our_PM_HullPointContents(var_3C, var_3C->firstclipnode, var_2C);
         if(g_contentsresult == -2) { return(i); }
      }
      else {

         for(j = 0; j < var_48; j++) {

            g_contentsresult = our_PM_HullPointContents(&(var_3C[j]), var_3C[j].firstclipnode, var_2C);
            if(g_contentsresult == -2) { return(i); }
         }
      }
   }

   return(-1);
}
HLDS_DLLEXPORT int our_PM_TestPlayerPosition(vec3_t pos, pmtrace_t * ptrace) {
   return(our__PM_TestPlayerPosition(pos, ptrace, NULL));
}
HLDS_DLLEXPORT int our_PM_TestPlayerPositionEx(vec3_t pos, pmtrace_t *ptrace, int (*pfnIgnore)(physent_t *)) {
   return(our__PM_TestPlayerPosition(pos, ptrace, pfnIgnore));
}

//This was in the SDK for some reason.
int  our_PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity) {

   int i;


   for(i = 0; i < global_pmove->numtouch; i++) {
      if(global_pmove->touchindex[i].ent == tr.ent) {
         break;
      }
   }

   if(i != global_pmove->numtouch) { // Already in list.
      return(0);
   }

   tr.deltavelocity[0] = impactvelocity[0];
   tr.deltavelocity[1] = impactvelocity[1];
   tr.deltavelocity[2] = impactvelocity[2];

   if(global_pmove->numtouch >= MAX_PHYSENTS) {
      Con_Printf("%s: Too many entities were touched.\n", __FUNCTION__);
      return(0);
   }

   global_pmove->touchindex[global_pmove->numtouch++] = tr;
   return(1);
}
HLDS_DLLEXPORT void our_PM_StuckTouch(int hitent, pmtrace_t *ptraceresult) {

  // edict_t * ent;


   if(global_pmove->server == 0) { return; }

  // ent = EDICT_NUM(global_pmove->physents[hitent].info);
   our_PM_AddToTouched(*ptraceresult, global_pmove->velocity);
}

int  our_PM_LinkContents(vec3_t p, int * ret) {

   int i, var_1C, var_20;
   physent_t * var_8_pent;
   hull_t * var_18_hull;
   vec3_t var_14;


   for(i = 1; i < global_pmove->numphysent; i++) {

      var_8_pent = &(global_pmove->physents[i]);
      if(var_8_pent->model == NULL || var_8_pent->solid != 0) { continue; } //On the second check, two of the engines conflicted.  One of 'em must be wrong.

      var_18_hull = var_8_pent->model->hulls;
      var_14[0] = p[0] - var_8_pent->origin[0];
      var_14[1] = p[1] - var_8_pent->origin[1];
      var_14[2] = p[2] - var_8_pent->origin[2];

      var_20 = our_PM_HullPointContents(var_18_hull, var_18_hull->firstclipnode, var_14);
      if(var_20 == Q1CONTENTS_EMPTY) { continue; }

      //Found what we wanted.
      if(ret != NULL) { *ret = var_8_pent->info; }

      return(var_8_pent->skin);
   }

   return(Q1CONTENTS_EMPTY);
}
HLDS_DLLEXPORT int  our_PM_PointContents(vec3_t p, int *truecontents) {

   int var_C_hullpointcont;
   int var_10;

   hull_t * var_4_hull;


   var_4_hull = global_pmove->physents[0].model->hulls;
   if(var_4_hull == NULL) { //Waitaminute, this shouldn't be possible.

      if(truecontents != NULL) { *truecontents = Q1CONTENTS_EMPTY; }
      return(Q1CONTENTS_SOLID);
   }

   var_C_hullpointcont = our_PM_HullPointContents(var_4_hull, var_4_hull->firstclipnode, p);

   if(truecontents != NULL) { *truecontents = var_C_hullpointcont; }
   if(var_C_hullpointcont <= -9 && var_C_hullpointcont >= -14) {
      var_C_hullpointcont = Q1CONTENTS_WATER;
   }

   if(var_C_hullpointcont == -2) { return(var_C_hullpointcont); }

   var_10 = our_PM_LinkContents(p, 0);

   if(var_10 == -1) { return(var_C_hullpointcont); }
   return(var_10);
}

//An oddity
HLDS_DLLEXPORT char * our_memfgets(byte *pMemFile, int fileSize, int *pFilePos, char *pBuffer, int bufferSize) {

   unsigned int i, FilePos, MaxLen;
   byte * ptr;


   if(pMemFile == NULL || pFilePos == NULL || pBuffer == NULL) {

      Con_Printf("%s: Called with NULL arguments.\n", __FUNCTION__);
      return(NULL);
   }

   FilePos = *pFilePos;
   if(FilePos >= (unsigned)fileSize || bufferSize < 2) {

      Con_Printf("%s: End of file (or impossibly small buffer).\n", __FUNCTION__);
      return NULL;
   }


   ptr = pMemFile + FilePos;
   if((signed)(fileSize - FilePos) > bufferSize - 1) {
      MaxLen = bufferSize - 1;
   }
   else {
      MaxLen = fileSize - FilePos;
   }

   //Due to the earlier checks, there's no way we can have less than one byte of pre-null termination room.
   for(i = 0; i < MaxLen; i++, ptr++) {

      if(*ptr == '\n') { i++; break; } //The extra ++ is to copy the \n.
   }

   Q_memcpy(pBuffer, pMemFile + FilePos, i);
   pBuffer[i] = '\0';

   *pFilePos += i;
   return(pBuffer);
}




void our_PM_InitBoxHull() {

   int i;
   int side;

   box_hull_0.clipnodes = box_clipnodes_0;
   box_hull_0.planes = box_planes_0;
   box_hull_0.firstclipnode = 0;
   box_hull_0.lastclipnode = 5;

   for(i = 0; i < 6; i++) {

      box_clipnodes_0[i].planenum = i;

      side = i&1;

      box_clipnodes_0[i].children[side] = Q1CONTENTS_EMPTY;

      if(i != 5) { box_clipnodes_0[i].children[side^1] = i + 1; }
      else { box_clipnodes_0[i].children[side^1] = Q1CONTENTS_SOLID; }

      box_planes_0[i].type = (i>>1);
      box_planes_0[i].normal[i>>1] = 1.0;
   }
}
void our_PM_Init(playermove_t * ppmove) {

   unsigned int i;


   our_PM_InitBoxHull();
   for(i = 0; i < 4; i++) {

      ppmove->player_mins[i][0] = global_player_mins[i][0];
      ppmove->player_mins[i][1] = global_player_mins[i][1];
      ppmove->player_mins[i][2] = global_player_mins[i][2];
      ppmove->player_maxs[i][0] = global_player_maxs[i][0];
      ppmove->player_maxs[i][1] = global_player_maxs[i][1];
      ppmove->player_maxs[i][2] = global_player_maxs[i][2];
   }

   ppmove->movevars = &movevars;

   ppmove->PM_Info_ValueForKey = Info_ValueForKey;
   ppmove->PM_Particle = CL_Particle;
   ppmove->PM_TestPlayerPosition = our_PM_TestPlayerPosition;
   ppmove->Con_NPrintf = Con_NPrintf;
   ppmove->Con_DPrintf = Con_Printf;
   ppmove->Con_Printf = Con_Printf;
   ppmove->Sys_FloatTime = Sys_FloatTime;
   ppmove->PM_StuckTouch = our_PM_StuckTouch;
   ppmove->PM_PointContents = our_PM_PointContents;
   ppmove->PM_TruePointContents = our_PM_TruePointContents;
   ppmove->PM_HullPointContents = our_PM_HullPointContents;
   ppmove->PM_PlayerTrace = our_PM_PlayerTrace;
   ppmove->PM_TraceLine = our_PM_TraceLine;
   ppmove->RandomLong = RandomLong;
   ppmove->RandomFloat = RandomFloat;
   ppmove->PM_GetModelType = our_PM_GetModelType;
   ppmove->PM_GetModelBounds = our_PM_GetModelBounds;
   ppmove->PM_HullForBsp = our_PM_HullForBsp;
   ppmove->PM_TraceModel = our_PM_TraceModel;
   ppmove->COM_FileSize = COM_FileSize;
   ppmove->COM_LoadFile = COM_LoadFile;
   ppmove->COM_FreeFile = COM_FreeFile;
   ppmove->memfgets = our_memfgets;
   ppmove->PM_PlayerTraceEx = our_PM_PlayerTraceEx;
   ppmove->PM_TestPlayerPositionEx = our_PM_TestPlayerPositionEx;
   ppmove->PM_TraceLineEx = our_PM_TraceLineEx;
}


//trace converting
void SV_ConvertPMTrace(trace_t * out_trace, pmtrace_t * in_trace, edict_t * ent) {

   memcpy(out_trace, in_trace, 0x30); //Everything up to ent is the same.
   out_trace->hitgroup = in_trace->hitgroup;
   out_trace->pHit = ent;
}
