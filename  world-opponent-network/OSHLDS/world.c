#include <math.h>

#include "world.h"

#include "cvar.h"
#include "host.h"
#include "Modding.h"
#include "NET.h"
#include "R.h"
#include "random.h"
#include "SV_mcast.h"
#include "SV_move.h"
#include "SV_user.h"
#include "sys.h"
#include "VecMath.h"
#include "report.h"
#include "Q_memdef.h"

/* This is handy, don't delete it.
   qboolean   free; //00
   int serialnumber; //04
   link_t      area;   //08         // linked to a division node or leaf

   int         headnode; //10         // -1 to use normal leaf check
   int         num_leafs; //14
   short      leafnums[MAX_ENT_LEAFS]; //18

   float      freetime; //78         // global_sv.time when the object was freed

   void*      pvPrivateData;   //7c

   entvars_t   v;
   ^
   string_t   classname; //80
   string_t   globalname; //84

   vec3_t      origin; //88 !
   vec3_t      oldorigin; //94
   vec3_t      velocity; //a0
   vec3_t      basevelocity; //ac
   vec3_t      clbasevelocity; //b8
   vec3_t      movedir; //c4

   vec3_t      angles; //d0
   vec3_t      avelocity;   //dc
   vec3_t      punchangle;   //e8
   vec3_t      v_angle;      //f4

   // For parametric entities
   vec3_t      endpos; //100
   vec3_t      startpos; //10c
   float      impacttime; //118
   float      starttime; //11c

   int         fixangle;
   float      idealpitch;
   float      pitch_speed;
   float      ideal_yaw;
   float      yaw_speed; //130

   int         modelindex;
   string_t   model;

   int         viewmodel;
   int         weaponmodel; //140

   vec3_t      absmin;
   vec3_t      absmax;
   vec3_t      mins;   //15c !
   vec3_t      maxs;   //168 !
   vec3_t      size;

   float      ltime;
   float      nextthink; //184

   int         movetype; //188
   int         solid; //18c !

   int         skin; //190
   int         body;
   int       effects;

   float      gravity;
   float      friction; /1a0

   int         light_level;

   int         sequence;
   int         gaitsequence;
   float      frame; //1b0
   float      animtime;
   float      framerate;
   byte      controller[4];
   byte      blending[2]; //1c0

   float      scale;

   int         rendermode;
   float      renderamt; //1cc
   vec3_t      rendercolor; //1d0
   int         renderfx;

   float      health; //1e0
   float      frags;
   int         weapons;
   float      takedamage; //1ec

   int         deadflag; //1f0
   vec3_t      view_ofs; //1f4

   int         button; //200
   int         impulse;

   struct edict_s      *chain; //208
   struct edict_s      *dmg_inflictor;
   struct edict_s      *enemy;
   struct edict_s      *aiment;
   struct edict_s      *owner; //218
   struct edict_s      *groundentity;

   int         spawnflags;
   int         flags;

   int         colormap; //228
   int         team;

   float      max_health;
   float      teleport_time;
   float      armortype; //238
   float      armorvalue;
   int         waterlevel;
   int         watertype;

   string_t   target; //248
   string_t   targetname;
   string_t   netname;
   string_t   message;

   float      dmg_take; //258
   float      dmg_save;
   float      dmg;
   float      dmgtime;

   string_t   noise; //268
   string_t   noise1;
   string_t   noise2;
   string_t   noise3;

   float      speed; //278
   float      air_finished;
   float      pain_finished;
   float      radsuit_finished;

   struct edict_s      *pContainingEntity; //288

   int         playerclass;
   float      maxspeed;

   float      fov;
   int         weaponanim;

   int         pushmsec; //29c

   int         bInDuck;
   int         flTimeStepSound;
   int         flSwimTime;
   int         flDuckTime;
   int         iStepLeft; //2b0
   float      flFallVelocity;

   int         gamestate;

   int         oldbuttons;

   int         groupinfo; //2c0

   // For mods
   int         iuser1;
   int         iuser2;
   int         iuser3;
   int         iuser4; //2d0
   float      fuser1;
   float      fuser2;
   float      fuser3;
   float      fuser4; //2e0
   vec3_t      vuser1; //2e4
   vec3_t      vuser2; //2f0
   vec3_t      vuser3; //2fc
   vec3_t      vuser4; //308
   struct edict_s      *euser1; //314
   struct edict_s      *euser2;
   struct edict_s      *euser3;
   struct edict_s      *euser4; //320
*/

/*** GLOBAL GLOBALS ***/
int global_g_groupop;
int global_g_groupmask;
edict_t ** g_moved_edict = NULL;
vec3_t * g_moved_from = NULL;

areanode_t global_sv_areanodes[32]; //for a total of 0x400 bytes, I'd like to have this static actually.
const vec3_t global_player_mins[5] = { { -16, -16, -36 },
                                       { -16, -16, -18 },
                                       {   0,   0,   0 },
                                       { -32, -32, -32 },
                                       {   0,   0,   0 } };
const vec3_t global_player_maxs[5] = { {  16,  16,  36 },
                                       {  16,  16,  18 },
                                       {   0,   0,   0 },
                                       {  32,  32,  32 },
                                       {   0,   0,   0 } };


/*** Globals ***/
static int         global_sv_numareanodes;

static hull_t      box_hull;
static hull_t      beam_hull;
static dclipnode_t box_clipnodes[6];
static mplane_t    box_planes[6];
static mplane_t    beam_planes[6];


/*** Functions ***/
void SV_HullForBeam() { printf("%s called", __FUNCTION__); exit(99); }  //Unused I think.



//Hulls are annoying.  They are very vital to collision detection though.
hull_t * SV_HullForBox(vec3_t boxmins, vec3_t boxmaxs) {

   box_planes[0].dist = boxmaxs[0];
   box_planes[1].dist = boxmins[0];
   box_planes[2].dist = boxmaxs[1];
   box_planes[3].dist = boxmins[1];
   box_planes[4].dist = boxmaxs[2];
   box_planes[5].dist = boxmins[2];

   return(&box_hull);
}
hull_t * SV_HullForBsp(edict_t * ent, const vec3_t mins, const vec3_t maxs, vec3_t offsets) {

   vec3_t var_14;
   hull_t * var_8;
   model_t * var_4;


   var_4 = global_sv.models[ent->v.modelindex];
   if(var_4 == NULL) {

      Sys_Error("SV_HullForBsp: Hit a null when trying to access model index %i.", ent->v.modelindex);
   }
   if(var_4->modeltype != 0) {

      Sys_Error("SV_HullForBsp: var_4->padding44 was not equal to zero, which is somehow bad.", ent->v.modelindex);
   }

   var_14[0] = maxs[0] - mins[0];
   var_14[1] = maxs[1] - mins[1];
   var_14[2] = maxs[2] - mins[2];

   if(var_14[0] < 8.0) {

      var_8 = &(var_4->hulls[0]);

      offsets[0] = var_8->clip_mins[0];
      offsets[1] = var_8->clip_mins[1];
      offsets[2] = var_8->clip_mins[2];
   }
   else {
      if(var_14[0] < 36.0) {
         if(var_14[2] < 36.0) {
            var_8 = &(var_4->hulls[3]);
         }
         else {
            var_8 = &(var_4->hulls[1]);
         }
      }
      else {
         var_8 = &(var_4->hulls[2]);
      }

      offsets[0] = var_8->clip_mins[0] - mins[0];
      offsets[1] = var_8->clip_mins[1] - mins[1];
      offsets[2] = var_8->clip_mins[2] - mins[2];
   }

   offsets[0] += ent->v.origin[0];
   offsets[1] += ent->v.origin[1];
   offsets[2] += ent->v.origin[2];

   return(var_8);
}
hull_t * SV_HullForEntity(edict_t * ent, vec3_t mins, vec3_t maxs, vec3_t offsets) {

   hull_t * var_1c;
   vec3_t boxmins;
   vec3_t boxmaxs;

   if(ent->v.solid == SOLID_BSP) {
      if(ent->v.movetype != MOVETYPE_PUSH && ent->v.movetype != MOVETYPE_PUSHSTEP) {
         Sys_Error("%s: SOLID_BSP without MOVETYPE_PUSH", __FUNCTION__);
      }
      var_1c = SV_HullForBsp(ent, mins, maxs, offsets);
   }
   else {

      boxmins[0] = ent->v.mins[0] - maxs[0];
      boxmins[1] = ent->v.mins[1] - maxs[1];
      boxmins[2] = ent->v.mins[2] - maxs[2];

      boxmaxs[0] = ent->v.maxs[0] - mins[0];
      boxmaxs[1] = ent->v.maxs[1] - mins[1];
      boxmaxs[2] = ent->v.maxs[2] - mins[2];

      var_1c = SV_HullForBox(boxmins, boxmaxs);
      offsets[0] = ent->v.origin[0];
      offsets[1] = ent->v.origin[1];
      offsets[2] = ent->v.origin[2];
   }

   return(var_1c);
}


//contents
/* DESCRIPTION: SV_HullPointContents
// LOCATION: q1bsp.c (as Q1_HullPointContents)
// PATH: Called only by a few odd SV functions.  Was decd static in QW.
//
// Does some sort of manipulation in order to retrieve data from the
// formless massless entity known as the game world.
*/
int SV_HullPointContents(hull_t * hull, int num, const vec3_t p) {

   float d;
   dclipnode_t * node;
   mplane_t * plane;

   while(num >= 0) {

      if(num < hull->firstclipnode || num > hull->lastclipnode) {
         Sys_Error("%s: bad node number", __FUNCTION__);
      }

      node = &(hull->clipnodes[num]);
      plane = &(hull->planes[node->planenum]);

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
/* DESCRIPTION: SV_LinkContents
// LOCATION: I think tere was one, search turns up nothing.
// PATH: SV_PointContents, recursive.
//
// Don't remember, doesn't look that easy to figure out either.
//
*/
int SV_LinkContents(areanode_t * anode, const vec3_t point) {

   static vec3_t origin = { 0, 0, 0 };
   link_t * l, * next;
   edict_t * touch;
   model_t * var_30;
   vec3_t var_20;
   vec3_t var_2C;
   hull_t * var_14;


   for(l = anode->solid_edicts.next; l != &(anode->solid_edicts); l = next) {

      next = l->next;
      touch = ((edict_t *)((byte *)l - (int)&(((edict_t *)0)->area)));

      if(touch->v.solid != 0) { continue; }

      if(touch->v.groupinfo != 0) {
         if((global_g_groupop == 0 && (touch->v.groupinfo & global_g_groupmask) == 0) ||
            (global_g_groupop == 1 && (touch->v.groupinfo & global_g_groupmask) != 0)) {
            continue;
         }
      }

      var_30 = global_sv.models[touch->v.modelindex];
      if(var_30 == NULL || var_30->modeltype != 0) {
         continue;
      }

      if((point[0] > touch->v.absmax[0]) ||
         (point[1] > touch->v.absmax[1]) ||
         (point[2] > touch->v.absmax[2]) ||
         (point[0] < touch->v.absmin[0]) ||
         (point[1] < touch->v.absmin[1]) ||
         (point[2] < touch->v.absmin[2])) {
         continue;
      }

      if(touch->v.skin < -100 || touch->v.skin > 100) {

         Con_Printf("%s: General warning about some variable being out of range.", __FUNCTION__);
      }

      var_14 = SV_HullForBsp(touch, origin, origin, var_20);
      var_2C[0] = point[0] - var_20[0];
      var_2C[1] = point[1] - var_20[1];
      var_2C[2] = point[2] - var_20[2];

      if(SV_HullPointContents(var_14, var_14->firstclipnode, var_2C) != -1) {
         return(touch->v.skin);
      }
   }

   if(anode->axis == -1) { return(-1); }
   if(point[anode->axis] > anode->dist) {
      return(SV_LinkContents(anode->children[0], point));
   }
   if(point[anode->axis] < anode->dist) {
      return(SV_LinkContents(anode->children[1], point));
   }
   return(-1);
}
/* DESCRIPTION: SV_PointContents
// LOCATION: Don't remember it being helpful, but q1bsp.c
// PATH: looooots
//
// Mostly just calls Sother functions.  The actual logic isn't
// decypherable at this time.
*/
int SV_PointContents(const vec3_t point) {

   int var_4;
   int var_8;

   var_4 = SV_HullPointContents(&(global_sv.worldmodel->hulls[0]), 0, point);

   if(var_4 == -2) { return(var_4); }

   if(var_4 <= -9 && var_4 >= -14) {
      var_4 = -3;
   }

   var_8 = SV_LinkContents(global_sv_areanodes, point);

   if(var_8 == -1) { return(var_4); }
   return(var_8);
}

/* DESCRIPTION: SV_RecursiveHullCheck
// LOCATION: q1bsp.c (as Q1BSP_RecursiveHullCheck--I actually finished this function before finding it in there...)
// PATH:
//
// Seems to be responsible for some sort of collision detection.  Owing to
// its recursive nature, and my lack of familiarity with 3D collision
// detection, I really can't elaborate.
*/
int SV_RecursiveHullCheck(hull_t * arg_0_hull, int arg_4_num, float arg_8_0, float arg_c_1, vec_t * arg_10_start, vec_t * arg_14_end, trace_t * arg_18_trace) {

   dclipnode_t   * var_4_clipnode;
   mplane_t * var_8_plane;
   float var_c_t1;
   float var_10_t2;
   float var_14_frac;
   int var_18;
   vec3_t var_24_mid;
   float var_2c_midf;
   int var_28;

   if(arg_4_num < 0) {
      if(arg_4_num == Q1CONTENTS_SOLID) {
         arg_18_trace->startsolid = 1;
      }
      else {
         arg_18_trace->allsolid = 0;

         if(arg_4_num == Q1CONTENTS_EMPTY) {
            arg_18_trace->inopen = 1;
         }
         else if(arg_4_num != -15) {
            arg_18_trace->inwater = 1;
         }
      }
      return(1);
   }

   if(arg_4_num < arg_0_hull->firstclipnode || arg_4_num > arg_0_hull->lastclipnode || arg_0_hull->planes == NULL) {
      Sys_Error("%s: Bad node number.", __FUNCTION__);
   }

   //listed as point distances in QW.
   var_4_clipnode = &(arg_0_hull->clipnodes[arg_4_num]);
   var_8_plane = &(arg_0_hull->planes[var_4_clipnode->planenum]);
/*
    var_4_clipnode = arg_0_hull->clipnodes + arg_4_num;
   var_8_plane = arg_0_hull->planes + var_4_clipnode->planenum;
*/


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
      return(SV_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[0], arg_8_0, arg_c_1, arg_10_start, arg_14_end, arg_18_trace));
   }
   if(var_c_t1 < 0 && var_10_t2 < 0) {
      return(SV_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[1], arg_8_0, arg_c_1, arg_10_start, arg_14_end, arg_18_trace));
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
   for(var_18 = 0; var_18 <= 2; var_18++) {
      var_24_mid[var_18] = ((arg_14_end[var_18] - arg_10_start[var_18]) * var_14_frac) + arg_10_start[var_18];
   }

   if(var_c_t1 < 0) { var_28 = 1; }
   else { var_28 = 0; }

   if(SV_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[var_28], arg_8_0, var_2c_midf, arg_10_start, var_24_mid, arg_18_trace) == 0) {
      return(0);
   }
   if(SV_HullPointContents(arg_0_hull, var_4_clipnode->children[var_28 ^ 1], var_24_mid) != Q1CONTENTS_SOLID) {
      return(SV_RecursiveHullCheck(arg_0_hull, var_4_clipnode->children[var_28 ^ 1], var_2c_midf, arg_c_1, var_24_mid, arg_14_end, arg_18_trace));
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

      if(SV_HullPointContents(arg_0_hull, arg_0_hull->firstclipnode, var_24_mid) != Q1CONTENTS_SOLID) {

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
      for(var_18 = 0; var_18 < 3; var_18++) {
         var_24_mid[var_18] = ((arg_14_end[var_18] - arg_10_start[var_18]) * var_14_frac) + arg_10_start[var_18];
      }
   }
}


//touching.
/* DESCRIPTION: SV_TouchLinks
// LOCATION: world.c
// PATH: SV_LinkEdict, recursive
//
// Does some sort of manipulation in order to retrieve data from the
// formless massless entity known as the game world.
//
// Extra for loop in QW code, claimed to be needed for some special cases.
// Won't copy over though.  I feel it's worth noting.
*/
void SV_TouchLinks(edict_t *ent, areanode_t *node) {

   link_t  * l, * next;
   edict_t * touch;
   model_t * var_10;
   hull_t * var_18;

   vec3_t offsets;
   vec3_t offsets2;

   for(l = node->trigger_edicts.next; l != &(node->trigger_edicts); l = next) {

      //if(linkcount == MAX_NODELINKS) { break; }
      next = l->next;
      touch = ((edict_t *)((byte *)l - (int)&(((edict_t *)0)->area)));

      if(touch == ent) { continue; }
      if(touch->v.groupinfo != 0 && ent->v.groupinfo != 0) {

         if((global_g_groupop == 0 && (touch->v.groupinfo & ent->v.groupinfo) == 0) ||
            (global_g_groupop == 1 && (touch->v.groupinfo & ent->v.groupinfo) != 0)) {
            continue;
         }
      }
      if(touch->v.solid != SOLID_TRIGGER) { continue; }

      if((ent->v.absmin[0] > touch->v.absmax[0]) ||
         (ent->v.absmin[1] > touch->v.absmax[1]) ||
         (ent->v.absmin[2] > touch->v.absmax[2]) ||
         (ent->v.absmax[0] < touch->v.absmin[0]) ||
         (ent->v.absmax[1] < touch->v.absmin[1]) ||
         (ent->v.absmax[2] < touch->v.absmin[2])) {
         continue;
      }

      var_10 = global_sv.models[touch->v.modelindex];
      if(var_10 != NULL && var_10->modeltype == 0) {

         var_18 = SV_HullForBsp(touch, ent->v.mins, ent->v.maxs, offsets);
         offsets2[0] = ent->v.origin[0] - offsets[0];
         offsets2[1] = ent->v.origin[1] - offsets[1];
         offsets2[2] = ent->v.origin[2] - offsets[2];

         if(SV_HullPointContents(var_18, var_18->firstclipnode, offsets2) != -2) { continue; }
      }

      gGlobalVariables.time = global_sv.time0c;
      gEntityInterface.pfnTouch(touch, ent);
   }

   if(node->axis == -1) { return; }

   if(ent->v.absmax[node->axis] > node->dist) {
      SV_TouchLinks(ent, node->children[0]);
   }
   if(ent->v.absmin[node->axis] < node->dist) {
      SV_TouchLinks(ent, node->children[1]);
   }
}
/* DESCRIPTION: SV_CreateAreaNode
// LOCATION: world.c
// PATH: SV_LinkEdict, recursive
//
// I don't really know...
*/
void SV_FindTouchedLeafs(edict_t *ent, mnode_t *node, int * arg_8) {

   mplane_t *splitplane;
   mleaf_t *leaf;
   int sides;
   int leafnum;

   if(node->contents == -2) { return; }

   if(node->contents < 0) {
      if(ent->num_leafs > 0x2F) {
         ent->num_leafs = 0x31;
      }
      else {
         leaf = (mleaf_t *)node;
         leafnum = leaf - global_sv.worldmodel->leafs - 1; //very odd results...

         ent->leafnums[ent->num_leafs] = leafnum;
         ent->num_leafs++;
      }
      return;
   }

   splitplane = node->plane;

   sides = BOX_ON_PLANE_SIDE(ent->v.absmin, ent->v.absmax, splitplane);

   if(sides == 3 && *arg_8 == -1) {
      //I think this line means "(address of node - address of worldmodel) / sizeof(node)".
      //Quirky.  And produces very odd assembly.
      *arg_8 = node - global_sv.worldmodel->nodes;
   }

   //It's possible to take both paths.
   if((sides & 1) != 0) {
      SV_FindTouchedLeafs(ent, node->children[0], arg_8);
   }
   if((sides & 2) != 0) {
      SV_FindTouchedLeafs(ent, node->children[1], arg_8);
   }
}


//links
/* DESCRIPTION: SV_Link/Unlink_Edict
// LOCATION: world.c
// PATH: Link_Edict = many, Unlink_Edict = Link_Edict and ED_Free
//
// Self-Explanatory.  Manages a linked list for Edicts.  The logic
// behind deciding WHERE to link it in is a mystery to me though.
*/
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers) {

   static int semaphore = 0;
   int var_8;
   areanode_t * node;

   if(ent->area.prev != NULL) { SV_UnlinkEdict(ent); }
   if(ent == global_sv.edicts || ent->free != 0) { return; }

   gEntityInterface.pfnSetAbsBox(ent);

   if(ent->v.movetype == MOVETYPE_FOLLOW && ent->v.aiment != NULL) {

      ent->headnode = ent->v.aiment->headnode;
      ent->num_leafs = ent->v.aiment->num_leafs;
      Q_memcpy(ent->leafnums, ent->v.aiment->leafnums, sizeof(ent->leafnums));
   }
   else {

      ent->headnode = 0;
      ent->num_leafs = 0;

      var_8 = -1;

      if(ent->v.modelindex != 0) {

         SV_FindTouchedLeafs(ent, global_sv.worldmodel->nodes, &var_8);
      }
      if(ent->num_leafs > 0x30) {

         ent->headnode = var_8;
         ent->num_leafs = 0;
         Q_memset(ent->leafnums, -1, sizeof(ent->leafnums));
      }
   }

   if(ent->v.solid == SOLID_NOT && ent->v.skin >= -1) { return; }

   if(ent->v.solid == SOLID_BSP && global_sv.models[ent->v.modelindex] == NULL && *(global_pr_strings + ent->v.model) == '\0') {

      Con_Printf("%s: Leaving early.  Some model wasn't found so we're not gonna use that ent.\n", __FUNCTION__);
      return;
   }

   node = global_sv_areanodes;
   while (1) {

      if(node->axis == -1) { break; }

      if(ent->v.absmin[node->axis] > node->dist) {
         node = node->children[0];
      }
      else if(ent->v.absmax[node->axis] < node->dist) {
         node = node->children[1];
      }
      else {
         break;
      }
   }

   // link it in
   if(ent->v.solid == SOLID_TRIGGER) {
      InsertLinkBefore(&ent->area, &node->trigger_edicts);
   }
   else {
      InsertLinkBefore(&ent->area, &node->solid_edicts);
   }

   // if touch_triggers, touch all entities at this node and decend for more
   if(touch_triggers != 0 && semaphore == 0) {

      //For actual multithreading, this is a bad idea.  But that's what
      //the asm says to do, and I know the DS isn't multithreaded.
      semaphore = 1;
      SV_TouchLinks(ent, global_sv_areanodes);
      semaphore = 0;
   }
}
void SV_UnlinkEdict(edict_t *ent) {

   if(ent->area.prev == NULL) { return; }

   RemoveLink(&ent->area);
   ent->area.prev = ent->area.next = NULL;
}
void SV_AddLinksToPM_(areanode_t * nodes, vec3_t low, vec3_t high) {

   int i, var_24_edictnum;
   link_t * ptr, * ptr2;
   edict_t * touch;
   physent_t * var_18_newphysent, *var_14_physent2;
   float * var_1C_vecmins, * var_20_vecmaxs; //todo, figure out nuances of this typecast.


   for(ptr = nodes->solid_edicts.next; ptr != &(nodes->solid_edicts); ptr = ptr2) {

      ptr2 = ptr->next;
      touch = ((edict_t *)((byte *)ptr - (int)&(((edict_t *)0)->area)));

      if(touch->v.groupinfo != 0) {
         if((global_g_groupop == 0 && (touch->v.groupinfo & global_sv_player->v.groupinfo) == 0) ||
            (global_g_groupop == 1 && (touch->v.groupinfo & global_sv_player->v.groupinfo) != 0)) {

            continue;
         }
      }

      if(touch->v.owner == global_sv_player) { continue; }
      switch(touch->v.solid) { //I wonder how that looks in asm?  Probably no faster from the if/else branching.

         case 0:
         case 2:
         case 3:
         case 4:
            break;
         default:
            continue;
      }

      var_24_edictnum = NUM_FOR_EDICT(touch);
      var_18_newphysent = &(global_pmove->visents[global_pmove->numvisent]);
      global_pmove->numvisent++;

      SV_CopyEdictToPhysent(var_18_newphysent, var_24_edictnum, touch);

      if(touch->v.solid == SOLID_NOT && (touch->v.skin == 0 || touch->v.modelindex == 0)) { continue; }
      if((touch->v.flags & FL_MONSTERCLIP) != 0 && touch->v.solid == SOLID_BSP) { continue; }
      if(touch == global_sv_player) { continue; }
      if((touch->v.flags & FL_CLIENT) != 0 && touch->v.health <= 0) { continue; }
      if(touch->v.mins[2] == 0 && touch->v.maxs[2] == 1) { continue; }
      if(Length(touch->v.size) == 0) { continue; } //This could be opti'd.

      var_1C_vecmins = touch->v.absmin;
      var_20_vecmaxs = touch->v.absmax;

      if((touch->v.flags & FL_CLIENT) != 0) {
         SV_GetTrueMinMax(var_24_edictnum-1, &var_1C_vecmins, &var_20_vecmaxs);
      }

      for(i = 0; i < 3; i++) { //asm indicated opposite direction.  I tried it both ways and commented out with no noticeable effect.
         if(var_1C_vecmins[i] > high[i] || var_20_vecmaxs[i] < low[i]) { break; }
      }
      if(i != 3) { continue; }

      if(touch->v.solid == SOLID_NOT && touch->v.skin == -16) {

         if(global_pmove->nummoveent < 0x40) {
            var_14_physent2 = &(global_pmove->moveents[global_pmove->nummoveent]);
            global_pmove->nummoveent++;
         }
         else {
            Con_Printf("%s: global_pmove->nummoveent >= MAX_MOVEENTS", __FUNCTION__);
            continue;
         }
      }
      else {
         if(global_pmove->numphysent < 600) {
            var_14_physent2 = &(global_pmove->physents[global_pmove->numphysent]);
            global_pmove->numphysent++;
         }
         else {
            Con_Printf("%s: global_pmove->numphysent >= MAX_PHYSENTS", __FUNCTION__);
            break;
         }
      }

      //at last...
      *var_14_physent2 = *var_18_newphysent;
   }


   if(nodes->axis != -1) {

      if(high[nodes->axis] > nodes->dist) {
         SV_AddLinksToPM_(nodes->children[0], low, high);
      }
      if(low[nodes->axis] < nodes->dist) {
         SV_AddLinksToPM_(nodes->children[1], low, high);
      }
   }
}
void SV_AddLinksToPM(areanode_t * nodes, vec3_t orig) {

   int i;
   vec3_t var_10;
   vec3_t var_1C;


   Q_memset(&(global_pmove->physents[0]), 0, sizeof(physent_t));
   Q_memset(&(global_pmove->visents[0]), 0, sizeof(physent_t));
   global_pmove->physents[0].model = global_sv.worldmodel;

   if(global_sv.worldmodel != NULL) {
      Q_strncpy(global_pmove->physents[0].name, global_sv.worldmodel->name, 0x1F);
   }

   global_pmove->physents[0].origin[0] = 0;
   global_pmove->physents[0].origin[1] = 0;
   global_pmove->physents[0].origin[2] = 0;
   global_pmove->physents[0].info = 0;
   global_pmove->physents[0].solid = SOLID_BSP;
   global_pmove->physents[0].movetype = MOVETYPE_NONE;
   global_pmove->physents[0].takedamage = 1;
   global_pmove->physents[0].blooddecal = 0;

   global_pmove->numphysent = 1;
   global_pmove->numvisent = 1;

   global_pmove->visents[0] = global_pmove->physents[0]; //struct copy
   global_pmove->nummoveent = 0;


   for(i = 0; i < 3; i++) {

      var_10[i] = orig[i] - 256;
      var_1C[i] = orig[i] + 256;
   }

   SV_AddLinksToPM_(nodes, var_10, var_1C);
}


//clipping
/* DESCRIPTION: SV_ClipToLinks
// LOCATION: world.c
// PATH: SV_Move, recursive
//
// Based on the vague description in QW,
// I'd say that it looks at our object that is moving and
// sees if it's running into any other objects along the way.
*/
void SV_ClipToLinks(areanode_t *node, moveclip_t *clip) {

   link_t *l;
   link_t *next;
   edict_t *touch;
   trace_t trace;


   //touch linked edicts
   for(l = node->solid_edicts.next; l != &(node->solid_edicts); l = next) {

      next = l->next;
      //This dangerous line assumes that 'l' is part of an edict_t struct, and
      //that by going back eight bytes we can et a pointer to that edict.
      //This is perfectly valid in that case, though I don't like doing it.
      touch = ((edict_t *)((byte *)l - (int)&(((edict_t *)0)->area)));

      if(touch->v.groupinfo != 0 &&
         clip->passedict != NULL && clip->passedict->v.groupinfo != 0) {

         if((global_g_groupop == 0 && (touch->v.groupinfo & clip->passedict->v.groupinfo) == 0) ||
            (global_g_groupop == 1 && (touch->v.groupinfo & clip->passedict->v.groupinfo) != 0)) {
            continue;
         }
      }

      if(touch->v.solid == SOLID_NOT || touch == clip->passedict) {
         continue;
      }
      if(touch->v.solid == SOLID_TRIGGER) {
         Sys_Error("Trigger in clipping list");
      }

      //Mod functions
      if(gNewDLLFunctions.pfnShouldCollide != NULL) {
         if(gNewDLLFunctions.pfnShouldCollide(touch, clip->passedict) == 0) { return; }
      }

      if(touch->v.solid == SOLID_BSP) {
         if((touch->v.flags & FL_MONSTERCLIP) != 0 && clip->hullnum == 0) {
            continue;
         }
      }
      else {
         if(clip->type1 == 1 && touch->v.movetype == MOVETYPE_PUSHSTEP) {
            continue;
         }
      }

      if(clip->type2 != 0 && touch->v.rendermode != 0 && (touch->v.flags & FL_WORLDBRUSH) == 0) {
         continue;
      }

      if(clip->boxmins[0] > touch->v.absmax[0] ||
         clip->boxmins[1] > touch->v.absmax[1] ||
         clip->boxmins[2] > touch->v.absmax[2] ||
         clip->boxmaxs[0] < touch->v.absmin[0] ||
         clip->boxmaxs[1] < touch->v.absmin[1] ||
         clip->boxmaxs[2] < touch->v.absmin[2]) {

         continue;
      }

      if(clip->passedict != NULL && clip->passedict->v.size[0] != 0 && touch->v.size[0] == 0) {
         continue; //points never interact.  But if they are points, shouldn't both size[] checks be for 0?
      }

      if(clip->trace.allsolid != 0) {
         return;
      }
      if(clip->passedict != NULL) {
         if(touch->v.owner == clip->passedict) {
            continue;
         }
         if(clip->passedict->v.owner == touch) {
            continue;
         }
      }

      if((touch->v.flags & FL_MONSTER) != 0) {
         SV_ClipMoveToEntity(&trace, touch, clip->start, clip->mins2, clip->maxs2, clip->end);
      }
      else {
         SV_ClipMoveToEntity(&trace, touch, clip->start, clip->mins, clip->maxs, clip->end);
      }

      if(trace.allsolid != 0 || trace.startsolid != 0 || trace.fraction < clip->trace.fraction) {

         trace.pHit = (edict_t *)touch;
         if(clip->trace.startsolid != 0) {
            clip->trace = trace;
            clip->trace.startsolid = 1;
         }
         else {
            clip->trace = trace;
         }
      }
//      else if(trace.startsolid != 0) { //Not possible.  Silly HL writers.
//
//      }
   }

   //That was fun.  Let's go grab some beers.
   if(node->axis == -1) { return; }

   if(clip->boxmaxs[node->axis] > node->dist) {
      SV_ClipToLinks(node->children[0], clip);
   }
   if(clip->boxmins[node->axis] < node->dist) {
      SV_ClipToLinks(node->children[1], clip);
   }
}
void SV_ClipToWorldBrush(areanode_t *node, moveclip_t *clip) {

   link_t *l;
   link_t *next;
   edict_t *touch;
   trace_t trace;


   for(l = node->solid_edicts.next; l != &(node->solid_edicts); l = next) {

      next = l->next;
      touch = ((edict_t *)((byte *)l - (int)&(((edict_t *)0)->area)));

      if(touch->v.solid != SOLID_BSP || touch == clip->passedict || (touch->v.flags & FL_WORLDBRUSH) == 0) {
         continue;
      }
      if(clip->boxmins[0] > touch->v.absmax[0] ||
         clip->boxmins[1] > touch->v.absmax[1] ||
         clip->boxmins[2] > touch->v.absmax[2] ||
         clip->boxmaxs[0] < touch->v.absmin[0] ||
         clip->boxmaxs[1] < touch->v.absmin[1] ||
         clip->boxmaxs[2] < touch->v.absmin[2]) {

         continue;
      }
      if(clip->trace.allsolid != 0) {
         return;
      }

      SV_ClipMoveToEntity(&trace, touch, clip->start, clip->mins, clip->maxs, clip->end);

      if(trace.allsolid != 0 || trace.startsolid != 0 || trace.fraction < clip->trace.fraction) {

         trace.pHit = (edict_t *)touch;
         if(clip->trace.startsolid != 0) {
            clip->trace = trace;
            clip->trace.startsolid = 1;
         }
         else {
            clip->trace = trace;
         }
      }
   }

   if(node->axis == -1) { return; }

   if(clip->boxmaxs[node->axis] > node->dist) {
      SV_ClipToWorldBrush(node->children[0], clip);
   }
   if(clip->boxmins[node->axis] < node->dist) {
      SV_ClipToWorldBrush(node->children[1], clip);
   }
}


/* DESCRIPTION: SV_CreateAreaNode
// LOCATION: world.c
// PATH: SV_ClearWorld, recursive
//
// Creates area nodes... A better question is what is an area node,
// and I think the answer is part of the map.
*/
areanode_t * SV_CreateAreaNode(int depth, vec3_t mins, vec3_t maxs) {

   areanode_t *anode;
   vec3_t size;
   vec3_t mins1, maxs1, mins2, maxs2;

   anode = &global_sv_areanodes[global_sv_numareanodes];
   global_sv_numareanodes++;

   ClearLink(&anode->trigger_edicts);
   ClearLink(&anode->solid_edicts);

   if(depth == 4) { //AREA DEPTH

      anode->axis = -1;
      anode->children[0] = anode->children[1] = NULL;
      return(anode);
   }

   //vector subtract
   size[0] = maxs[0] - mins[0];
   size[1] = maxs[1] - mins[1];
   size[2] = maxs[2] - mins[2];

   if(size[0] > size[1]) { anode->axis = 0; }
   else { anode->axis = 1; }

   anode->dist = 0.5 * (maxs[anode->axis] + mins[anode->axis]);

   //VectorCopies
   mins1[0] = mins[0];
   mins1[1] = mins[1];
   mins1[2] = mins[2];

   mins2[0] = mins[0];
   mins2[1] = mins[1];
   mins2[2] = mins[2];

   maxs1[0] = maxs[0];
   maxs1[1] = maxs[1];
   maxs1[2] = maxs[2];

   maxs2[0] = maxs[0];
   maxs2[1] = maxs[1];
   maxs2[2] = maxs[2];

   maxs1[anode->axis] = mins2[anode->axis] = anode->dist;

   anode->children[0] = SV_CreateAreaNode(depth+1, mins2, maxs2);
   anode->children[1] = SV_CreateAreaNode(depth+1, mins1, maxs1);

   return(anode);
}

void SV_InitBoxHull() {

   int i;
   int side;

   box_hull.clipnodes = box_clipnodes;
   box_hull.planes = box_planes;
   box_hull.firstclipnode = 0;
   box_hull.lastclipnode = 5;

   beam_hull = box_hull;
   beam_hull.planes = beam_planes;
   //beam_hull and beam_planes I think are expendable.

   for(i = 0; i < 6; i++) {

      box_clipnodes[i].planenum = i;

      side = i&1;

      box_clipnodes[i].children[side] = Q1CONTENTS_EMPTY;

      if(i != 5) { box_clipnodes[i].children[side^1] = i + 1; }
      else { box_clipnodes[i].children[side^1] = Q1CONTENTS_SOLID; }

      box_planes[i].type = (i>>1);
      box_planes[i].normal[i>>1] = 1.0;
      beam_planes[i].type = 5;
   }
}
void SV_ClearWorld() {

   SV_InitBoxHull();

   memset(global_sv_areanodes, 0, 0x400);
   global_sv_numareanodes = 0;

   SV_CreateAreaNode(0, global_sv.worldmodel->mins, global_sv.worldmodel->maxs);
}



