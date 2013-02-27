#include "R.h"
#include "cvar.h"
#include "Mod.h"
#include "Modding.h"
#include "NET.h"
#include "Q_memdef.h"
#include "report.h"


/*** SRUCTS ***/
typedef struct r_object_s { //0x44

   float padding00;
   int padding04;
   vec3_t padding08;
   vec3_t padding14;
   vec3_t padding20;

   uint32 padding2C;
   uint32 padding30; //likely a short
   model_t * padding34;
   uint32 nCurrentHull;
   uint32 nCurrentPlane;
   uint32 Total;
} r_object_t;


/*** GLOBAL GLOBALS ***/
vec_t bonetransform[0x80][3][4];
vec_t rotationmatrix[3][4];
BlendingAPI_t * g_pSvBlendingAPI = NULL;
texture_t * global_r_notexture_mip = NULL;


/*** GLOBALS ***/
static r_object_t rgStudioCache[0x10]; //0x44 each
static int r_cachecurrent;
static int nCurrentHull;
static int nCurrentPlane;

static hull_t cache_hull[0x80];
static hull_t studio_hull[0x80];
static mplane_t cache_planes[0x300];
static mplane_t studio_planes[0x300];
static uint32 studio_hull_hitgroup[0x80];
static uint32 cache_hull_hitgroup[0x80];
static dclipnode_t studio_clipnodes[6];

static studiohdr_t * pstudiohdr;


static cvar_t cvar_r_cachestudio  = {  "r_cachestudio", "1",   FCVAR_NONE, 1.0, NULL };
static cvar_t cvar_sv_clienttrace = { "sv_clienttrace", "1", FCVAR_SERVER, 1.0, NULL };
/*** LOCAL FUNCTIONS ***/
/* DESCRIPTION: SV_InitStudioHull
// PATH: R_StudioHull
//
// And thus we begin the descent in what I expect to be a horribly complicated
// group of static vars.  This one is nice enough.  We assign our vars things
// (remember, global vars are zerod out by default).
*/
void SV_InitStudioHull() {

   unsigned int i;


   if(studio_hull[0].planes != NULL) { return; }

   for(i = 0; i < 6; i++) {

      studio_clipnodes[i].planenum = i;
      studio_clipnodes[i].children[i&1] = -1;
      if(i == 5) {
         studio_clipnodes[i].children[(i+1)&1] = -2;
      }
      else {
         studio_clipnodes[i].children[(i+1)&1] = i+1;
      }
   }

   for(i = 0; i < 0x80; i++) {

      studio_hull[i].clipnodes = studio_clipnodes;
      studio_hull[i].planes = &(studio_planes[i*6]);
      studio_hull[i].firstclipnode = 0;
      studio_hull[i].lastclipnode = 5;
   }
}

/* DESCRIPTION: SV_SetStudioHullPlane
// PATH: R_StudioHull
//
// Okay, the obvious--it's altering the planes based on some bonetransform
// variable.  There are a lot of 'transforms' available, and they're gotten
// througha DLSYM call.  I don't know the details beyond that though.
*/
void SV_SetStudioHullPlane(mplane_t * pl, int arg_4, int arg_8, float arg_c) {

   pl->type = 5;

   pl->normal[0] = bonetransform[arg_4][0][arg_8];
   pl->normal[1] = bonetransform[arg_4][1][arg_8];
   pl->normal[2] = bonetransform[arg_4][2][arg_8];

   pl->dist = (pl->normal[0] * bonetransform[arg_4][0][3]) +
              (pl->normal[1] * bonetransform[arg_4][1][3]) +
              (pl->normal[2] * bonetransform[arg_4][2][3]) + arg_c;
}

/* DESCRIPTION: R_FlushStudioCache->R_InitStudioCache
// PATH: R_AddToStudioCache
//
// This sets some counters to zero.  Some horrible, private counters.
*/
void R_InitStudioCache() {

   Q_memset(rgStudioCache, 0, sizeof(rgStudioCache));
   r_cachecurrent = 0;
   nCurrentHull = 0;
   nCurrentPlane = 0;
}
void R_FlushStudioCache() { R_InitStudioCache(); }

/* DESCRIPTION: R_AddToStudioCache
// PATH: R_StudioHull
//
// A ton of largely undefined arguments are thrown into the first unique
// struct so far.
*/
void R_AddToStudioCache(float arg_0, int arg_4, vec3_t arg_8, vec3_t arg_C, vec3_t arg_10,
                        byte * arg_14, byte * arg_18, model_t * arg_1C, hull_t * arg_20, int arg_24) {

   r_object_t * ptr;


   if(arg_24 + nCurrentHull >= 0x80) { R_FlushStudioCache(); }

   r_cachecurrent++;
   ptr = &(rgStudioCache[r_cachecurrent & 0x0F]);

   ptr->padding00 = arg_0;
   ptr->padding04 = arg_4;
   ptr->padding08[0] = arg_8[0];
   ptr->padding08[1] = arg_8[1];
   ptr->padding08[2] = arg_8[2];
   ptr->padding14[0] = arg_C[0];
   ptr->padding14[1] = arg_C[1];
   ptr->padding14[2] = arg_C[2];
   ptr->padding20[0] = arg_10[0];
   ptr->padding20[1] = arg_10[1];
   ptr->padding20[2] = arg_10[2];

   Q_memcpy(&(ptr->padding2C), arg_14, 4);
   Q_memcpy(&(ptr->padding30), arg_18, 2);
   ptr->padding34 = arg_1C;
   ptr->nCurrentHull = nCurrentHull;
   ptr->nCurrentPlane = nCurrentPlane;

   Q_memcpy(&cache_hull[nCurrentHull], arg_20, arg_24 * sizeof(hull_t));
   Q_memcpy(&cache_planes[nCurrentPlane], studio_planes, arg_24 * sizeof(cplane_t) * 6);
   Q_memcpy(&cache_hull_hitgroup[nCurrentHull], studio_hull_hitgroup, arg_24 * sizeof(uint32));

   nCurrentHull += arg_24;
   nCurrentPlane += arg_24 * 6;
   ptr->Total = arg_24;
}

/* DESCRIPTION: R_CheckStudioCache
// PATH: R_StudioHull
//
// This appears to look for a matching whatever in the cache.Mod_Extradata
*/
r_object_t * R_CheckStudioCache(model_t * arg_0, float arg_4, int arg_8, vec3_t arg_C,
                                vec3_t arg_10, vec3_t arg_14, byte * arg_18, byte * arg_1C) {

   unsigned int i;
   r_object_t * ptr;


   for(i = 0; i < 0x10; i++) {

      ptr = &(rgStudioCache[(r_cachecurrent - i) & 0x0F]);
      if(ptr->padding34 == arg_0 && ptr->padding00 == arg_4 && ptr->padding04 == arg_8 &&
         VectorCompare(arg_C, ptr->padding08) && VectorCompare(arg_10, ptr->padding14) && VectorCompare(arg_14, ptr->padding20) &&
         Q_memcmp(&(ptr->padding2C), arg_18, 4) == 0 && Q_memcmp(&(ptr->padding30), arg_1C, 2) == 0) {

         return(ptr);
      }
   }
   return(NULL);
}

/* DESCRIPTION: R_StudioHull
// PATH: HullForStudioModel
//
// Oh, my head... This in a roundabout complicated way assigns numbers to things
// so that it all works somehow.  If we're LUCKY whatever we want is in the cache.
*/
hull_t * R_StudioHull(model_t * arg_0, float arg_4, int arg_8, vec3_t arg_C, vec3_t arg_10,
                      vec3_t arg_14, byte * arg_18, byte * arg_1C, int * OUT_arg_20_total, edict_t * OUT_arg_24, int arg_isCS) {

   int i, j, var_20;
   vec3_t var_18;
   r_object_t * var_1c_r_obj;
   mstudiobbox_t * var_c;

   var_1c_r_obj = NULL;
   SV_InitStudioHull();
   if(cvar_r_cachestudio.value != 0) {

      var_1c_r_obj = R_CheckStudioCache(arg_0, arg_4, arg_8, arg_C, arg_10, arg_14, arg_18, arg_1C);
      if(var_1c_r_obj != NULL) {

         Q_memcpy(studio_planes, &(cache_planes[var_1c_r_obj->nCurrentPlane]), var_1c_r_obj->Total * sizeof(mplane_t) * 6);
         Q_memcpy(studio_hull, &(cache_hull[var_1c_r_obj->nCurrentHull]), var_1c_r_obj->Total * sizeof(hull_t));
         Q_memcpy(studio_hull_hitgroup, &(cache_hull_hitgroup[var_1c_r_obj->nCurrentHull]), var_1c_r_obj->Total * sizeof(uint32));

         *OUT_arg_20_total = var_1c_r_obj->Total;
         return(studio_hull);
      } //else, continue like before.
   }

   pstudiohdr = Mod_Extradata(arg_0); //If this fails, Mod_ExtraData bails for us.
   var_18[0] = -arg_C[0];
   var_18[1] = arg_C[1];
   var_18[2] = arg_C[2];

   g_pSvBlendingAPI->BoneSetup(arg_0, arg_4, arg_8, var_18, arg_10, arg_18, arg_1C, -1, OUT_arg_24);

   var_c = (mstudiobbox_t *)(((byte *)pstudiohdr) + pstudiohdr->hitboxindex);
   for(i = j = 0; i < pstudiohdr->numhitboxes; i++, j+=6) {

      //So THIS is the loop that adds CS compatibility, or whatever.  Screw that.
      studio_hull_hitgroup[i] = var_c[i].group;

      SV_SetStudioHullPlane(&(studio_planes[j+0]), var_c[i].bone, 0, var_c[i].bbmax[0]);
      SV_SetStudioHullPlane(&(studio_planes[j+1]), var_c[i].bone, 0, var_c[i].bbmin[0]);
      SV_SetStudioHullPlane(&(studio_planes[j+2]), var_c[i].bone, 1, var_c[i].bbmax[1]);
      SV_SetStudioHullPlane(&(studio_planes[j+3]), var_c[i].bone, 1, var_c[i].bbmin[1]);
      SV_SetStudioHullPlane(&(studio_planes[j+4]), var_c[i].bone, 2, var_c[i].bbmax[2]);
      SV_SetStudioHullPlane(&(studio_planes[j+5]), var_c[i].bone, 2, var_c[i].bbmin[2]);

      studio_planes[j+0].dist += (abs(studio_planes[j+0].normal[0] * arg_14[0]) +
                                  abs(studio_planes[j+0].normal[1] * arg_14[1]) +
                                  abs(studio_planes[j+0].normal[2] * arg_14[2]));
      studio_planes[j+1].dist += (abs(studio_planes[j+1].normal[0] * arg_14[0]) +
                                  abs(studio_planes[j+1].normal[1] * arg_14[1]) +
                                  abs(studio_planes[j+1].normal[2] * arg_14[2]));
      studio_planes[j+2].dist += (abs(studio_planes[j+2].normal[0] * arg_14[0]) +
                                  abs(studio_planes[j+2].normal[1] * arg_14[1]) +
                                  abs(studio_planes[j+2].normal[2] * arg_14[2]));
      studio_planes[j+3].dist += (abs(studio_planes[j+3].normal[0] * arg_14[0]) +
                                  abs(studio_planes[j+3].normal[1] * arg_14[1]) +
                                  abs(studio_planes[j+3].normal[2] * arg_14[2]));
      studio_planes[j+4].dist += (abs(studio_planes[j+4].normal[0] * arg_14[0]) +
                                  abs(studio_planes[j+4].normal[1] * arg_14[1]) +
                                  abs(studio_planes[j+4].normal[2] * arg_14[2]));
      studio_planes[j+5].dist += (abs(studio_planes[j+5].normal[0] * arg_14[0]) +
                                  abs(studio_planes[j+5].normal[1] * arg_14[1]) +
                                  abs(studio_planes[j+5].normal[2] * arg_14[2]));

   }

   //Again, CS gets special treatment
   *OUT_arg_20_total = var_20 = pstudiohdr->numhitboxes;

   if(cvar_r_cachestudio.value != 0) {

      R_AddToStudioCache(arg_4, arg_8, arg_C, arg_10, arg_14, arg_18, arg_1C, arg_0, studio_hull, var_20);
   }

   return(studio_hull);
}

void R_StudioPlayerBlend(mstudioseqdesc_t * arg0, int * OUT_arg4, float * OUT_arg8) {



   *OUT_arg4 = *OUT_arg8 * 3;
   if(arg0->blendstart[0] > *OUT_arg4) {

      *OUT_arg8 = *OUT_arg8 - (arg0->blendstart[0] / 3);
      *OUT_arg4 = 0;
      return;
   }
   if(arg0->blendend[0] < *OUT_arg4) {

      *OUT_arg8 = *OUT_arg8 - (arg0->blendend[0] / 3);
      *OUT_arg4 = 0xFF;
      return;
   }

   if(arg0->blendend[0] - arg0->blendstart[0] < .1) {
      *OUT_arg4 = 0x7F;
   }
   else {
      *OUT_arg4 = ((*OUT_arg4 - arg0->blendstart[0]) * 255) / (arg0->blendend[0] - arg0->blendstart[0]);
   }

   *OUT_arg8 = 0;
}

/* DESCRIPTION: R_StudioCalcBoneAdj
// Location: SDK, sort've, as CalcBoneAdj
// PATH: SV_StudioSetupBones
//
// I'm *pretty* sure this is abusing angles.  Lots of 'em.  Why though is
// another question.  This function also pretty much defines floatng
// point hell.  Maybe the modders would know...
*/
void R_StudioCalcBoneAdj(float arg_0, float * arg_4, char * arg_8, char * arg_C, char arg_10) {

// motion flags
   #define STUDIO_X      0x0001
   #define STUDIO_Y      0x0002
   #define STUDIO_Z      0x0004
   #define STUDIO_XR      0x0008
   #define STUDIO_YR      0x0010
   #define STUDIO_ZR      0x0020
   #define STUDIO_LX      0x0040
   #define STUDIO_LY      0x0080
   #define STUDIO_LZ      0x0100
   #define STUDIO_AX      0x0200
   #define STUDIO_AY      0x0400
   #define STUDIO_AZ      0x0800
   #define STUDIO_AXR   0x1000
   #define STUDIO_AYR   0x2000
   #define STUDIO_AZR   0x4000
   #define STUDIO_TYPES   0x7FFF
   #define STUDIO_RLOOP   0x8000   // controller that wraps shortest distance


   char var_1;
   int i, bindex, temp, var_18, var_1C, var_20, var_24;
   float var_10_value;
   mstudiobonecontroller_t * var_14_bonecontroller;


   var_1 = arg_10;
   var_14_bonecontroller = (mstudiobonecontroller_t *)(((byte *)pstudiohdr) + pstudiohdr->bonecontrollerindex);
   for(i = 0; i < pstudiohdr->numbonecontrollers; i++) {

      bindex = var_14_bonecontroller[i].index;
      if(bindex <= 3) {

         // check for 360% wrapping
         if(var_14_bonecontroller[i].type & STUDIO_RLOOP) {

            temp = arg_8[bindex] - arg_C[bindex];
            if(temp < 0) { temp = -temp; }
            if(temp > 0x80) {

               temp = arg_8[i] + 0x80;
               if(temp < 0) { (var_18 = temp + 0xFF) >> 8; }
               else { var_18 = temp >> 8; }
               var_18 = temp - (var_18 << 8);

               temp = arg_C[i] + 0x80;
               if(temp < 0) { (var_1C = temp + 0xFF) >> 8; }
               else { var_1C = temp >> 8; }
               var_1C = temp - (var_1C << 8);

               var_10_value = (((arg_0 * var_18) + ((1.0 - arg_0) * var_1C) - 128.0) * (360.0/256.0)) + var_14_bonecontroller[i].start;
            }
            else {
               var_20 = arg_8[bindex];
               var_24 = arg_C[bindex];

               var_10_value = ((arg_0 * var_20) + ((1.0 - arg_0) * var_24) * (360.0/256.0)) + var_14_bonecontroller[i].start;
            }
         }
         else {

            var_20 = arg_8[bindex];
            var_24 = arg_C[bindex];

            var_10_value = (arg_0 * var_20) + ((1.0 - arg_0) * var_24) / 255.0;
            if(var_10_value < 0) { var_10_value = 0; }
            else if(var_10_value > 1) { var_10_value = 1; }

            var_10_value = ((1.0 - var_10_value) * var_14_bonecontroller[i].start) + (var_10_value * var_14_bonecontroller[i].end);
         }
      }
      else {

         var_10_value = var_1 / 64.0;
         if(var_10_value > 1) { var_10_value = 1; }

         var_10_value = ((1.0 - var_10_value) * var_14_bonecontroller[i].start) + (var_10_value * var_14_bonecontroller[i].end);
      }

      switch(var_14_bonecontroller[i].type & STUDIO_TYPES) {

      case STUDIO_XR:
      case STUDIO_YR:
      case STUDIO_ZR:
         arg_4[i] = var_10_value * (M_PI / 180.0);
         break;

      case STUDIO_X:
      case STUDIO_Y:
      case STUDIO_Z:
         arg_4[i] = var_10_value;
         break;
      }
   }
}

/* DESCRIPTION: R_StudioSlerpBones
// Location: SDK, as SlerpBones
// PATH: SV_StudioSetupBones
//
// A much nicer function that's almost identical to its SDK counterpart.
// Slerping it seems involves combining two bone lists.  Dunno how
// really, because this was the best kind of function--one already done.
*/
void R_StudioSlerpBones(vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s) {

   int i;
   vec4_t q3;
   float s1;


   if(s < 0) { s = 0; }
   else if(s > 1.0) { s = 1.0; }

   s1 = 1.0 - s;


   for(i = 0; i < pstudiohdr->numbones; i++) {

      QuaternionSlerp(q1[i], q2[i], s, q3);
      q1[i][0] = q3[0];
      q1[i][1] = q3[1];
      q1[i][2] = q3[2];
      q1[i][3] = q3[3];
      pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
      pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
      pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
   }
}

//This function is likely to have bad data
mstudioanim_t * R_GetAnim(model_t * arg_0, mstudioseqdesc_t *pseqdesc) {

   mstudioseqgroup_t *pseqgroup;
   dmodel_t * submods;


   pseqgroup = (mstudioseqgroup_t *)((byte *)pstudiohdr + pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

   if(pseqdesc->seqgroup == 0) {
      return(mstudioanim_t *)((byte *)pstudiohdr + pseqgroup->data + pseqdesc->animindex);
   }

   //new evidence points to model_t->submodels, but that doesn't explain the cache_check.
   submods = arg_0->submodels; //It's a guess, but there aren't many structs with pointers at 0x7c
   if(submods == NULL) {

      submods = Q_Malloc(sizeof(dmodel_t));
      CHECK_MEMORY_MALLOC(submods);
      Q_memset(submods, 0, sizeof(dmodel_t));

      arg_0->submodels = submods;
   }

   if(Cache_Check((cache_user_t *)(((byte *)submods) + pseqdesc->seqgroup)) == 0) {

      COM_LoadCacheFile(pseqgroup->name, &(((cache_user_t *)submods)[pseqdesc->seqgroup]));
   }

   return((mstudioanim_t *)((((cache_user_t *)submods)[pseqdesc->seqgroup].data) + pseqdesc->animindex));
}

/* DESCRIPTION: R_StudioCalcBoneQuaterion
// Location: SDK, as StudioCalcBoneQuaterion, and as a perfect match
// PATH: SV_StudioSetupBones
//
// "Get bone quaternions" according to the SDK.  At this point I've figured
// out a few things, namely QuaternionSlerp combines two bone 'lists'.  The
// rest appears to just be extracting and processing data.
*/
void R_StudioCalcBoneQuaterion(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q) {

   int                  j, k;
   vec4_t               q1, q2;
   vec3_t               angle1, angle2;
   mstudioanimvalue_t * panimvalue;

   for(j = 0; j < 3; j++) {

      if(panim->offset[j+3] == 0) {
         angle2[j] = angle1[j] = pbone->value[j+3]; // default;
      }
      else {
         panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j+3]);
         k = frame;

         // DEBUG
         if(panimvalue->num.total < panimvalue->num.valid) {
            k = 0;
         }
         while(panimvalue->num.total <= k)  {

            k -= panimvalue->num.total;
            panimvalue += panimvalue->num.valid + 1;

            // DEBUG
            if(panimvalue->num.total < panimvalue->num.valid) {
               k = 0;
            }
         }

         // Bah, missing blend!
         if(panimvalue->num.valid > k) {
            angle1[j] = panimvalue[k+1].value;

            if(panimvalue->num.valid > k + 1) {
               angle2[j] = panimvalue[k+2].value;
            }
            else {
               if(panimvalue->num.total > k + 1) {
                  angle2[j] = angle1[j];
               }
               else {
                  angle2[j] = panimvalue[panimvalue->num.valid+2].value;
               }
            }
         }
         else {
            angle1[j] = panimvalue[panimvalue->num.valid].value;
            if(panimvalue->num.total > k + 1) {
               angle2[j] = angle1[j];
            }
            else {
               angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
            }
         }
         angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
         angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
      }

      if(pbone->bonecontroller[j+3] != -1) {
         angle1[j] += adj[pbone->bonecontroller[j+3]];
         angle2[j] += adj[pbone->bonecontroller[j+3]];
      }
   }

   if(VectorCompare(angle1, angle2) == 0) {
      AngleQuaternion(angle1, q1);
      AngleQuaternion(angle2, q2);
      QuaternionSlerp(q1, q2, s, q);
   }
   else {
      AngleQuaternion(angle1, q);
   }
}

/* DESCRIPTION: R_StudioCalcBonePosition
// Location: SDK, as StudioCalcBonePosition
// PATH: SV_StudioSetupBones
//
// Based on the title, I'd say it calculates positions in R3 for the bones.
// But as I didn't have to write it--always a good thing--I can't say for sure.
*/
void R_StudioCalcBonePosition(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos) {

   int                  j, k;
   mstudioanimvalue_t * panimvalue;

   for(j = 0; j < 3; j++) {

      pos[j] = pbone->value[j]; // default;
      if(panim->offset[j] != 0) {

         panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);
         /*
         if (i == 0 && j == 0)
            Con_DPrintf("%d  %d:%d  %f\n", frame, panimvalue->num.valid, panimvalue->num.total, s );
         */

         k = frame;
         // DEBUG
         if(panimvalue->num.total < panimvalue->num.valid) {
            k = 0;
         }
         // find span of values that includes the frame we want
         while(panimvalue->num.total <= k) {
            k -= panimvalue->num.total;
            panimvalue += panimvalue->num.valid + 1;
            // DEBUG
            if(panimvalue->num.total < panimvalue->num.valid) {
               k = 0;
            }
         }
         // if we're inside the span
         if(panimvalue->num.valid > k) {
            // and there's more data in the span
            if(panimvalue->num.valid > k + 1) {
               pos[j] += (panimvalue[k+1].value * (1.0 - s) + s * panimvalue[k+2].value) * pbone->scale[j];
            }
            else {
               pos[j] += panimvalue[k+1].value * pbone->scale[j];
            }
         }
         else {
            // are we at the end of the repeating values section and there's another section with data?
            if(panimvalue->num.total <= k + 1) {
               pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
            }
            else {
               pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
            }
         }
      }
      if(pbone->bonecontroller[j] != -1 && adj) {
         pos[j] += adj[pbone->bonecontroller[j]];
      }
   }
}

/* DESCRIPTION: SV_StudioSetupBones
// Location: SDK, as SetupBones
// PATH: An annoying API.
//
// This 'sets up' bones.  I'm starting to get the picture, but I don't know
// enough to describe what's actualy going on.
*/
void SV_StudioSetupBones(model_t * arg_0, float arg_4, int arg_8_sequence,
                         vec3_t arg_C, vec3_t arg_10, byte * arg_14,
                         byte * arg_18, int arg_1C, edict_t * unused) {

   int               i, j, var_270, var_26C[0x80];
   float             var_c, var_10, var_30[8];

   vec_t              bonematrix[3][4];
   mstudiobone_t      *pbones;
   mstudioseqdesc_t   *pseqdesc;
   mstudioanim_t      *panim;

   #define MAXSTUDIOBONES 128
   static vec3_t      pos[MAXSTUDIOBONES];
   static vec4_t      q[MAXSTUDIOBONES];
   static vec3_t      pos2[MAXSTUDIOBONES];
   static vec4_t      q2[MAXSTUDIOBONES];


   var_270 = 0;
   if(arg_8_sequence < 0 || arg_8_sequence >= pstudiohdr->numseq) {
      Con_Printf("%s: Sequence %i/%i out of range for model %s.\n", __FUNCTION__, arg_8_sequence, pstudiohdr->numseq, pstudiohdr->name);
      arg_8_sequence = 0;
   }

   pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + arg_8_sequence;
   pbones = (mstudiobone_t *)((byte *)pstudiohdr + pstudiohdr->boneindex);

   panim = R_GetAnim(arg_0, pseqdesc);

   if(arg_1C < -1 || arg_1C >= pstudiohdr->numbones) { arg_1C = 0; }
   if(arg_1C == -1) {
      var_270 = pstudiohdr->numbones;
      for(i = 0; i < pstudiohdr->numbones; i++) {

         var_26C[(var_270 - i) - 1] = i;
      }
   }
   else {
      for(i = arg_1C; i != -1; i = pbones[i].parent) {

         var_26C[var_270] = i;
         var_270++;
      }
   }

   if(pseqdesc->numframes > 1) {
      var_c = (pseqdesc->numframes - 1) * arg_4 / 256.0;
   }
   else {
      var_c = 0;
   }

   var_10 = var_c - (int)var_c; //so, the decimal part.
   R_StudioCalcBoneAdj(0, var_30, arg_14, arg_14, 0);

   for(j = var_270 - 1; j >= 0; j--) {

      i = var_26C[j];
      R_StudioCalcBoneQuaterion((int)var_c, var_10, &(pbones[i]), &(panim[i]), var_30, q[i]);
      R_StudioCalcBonePosition((int)var_c, var_10, &(pbones[i]), &(panim[i]), var_30, pos[i]);
   }

   if(pseqdesc->numblends > 1) {

      //Waitaminute, why are we doing this over again?
      pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + arg_8_sequence;
      panim = R_GetAnim(arg_0, pseqdesc);

      panim += pstudiohdr->numbones;

      for(j = var_270 - 1; j >= 0; j--) {

         i = var_26C[j];
         R_StudioCalcBoneQuaterion((int)var_c, var_10, &(pbones[i]), &(panim[i]), var_30, q2[i]);
         R_StudioCalcBonePosition((int)var_c, var_10, &(pbones[i]), &(panim[i]), var_30, pos2[i]);
      }

      R_StudioSlerpBones(q, pos, q2, pos2, ((float)(arg_18[0])) / 255.0);
   }

   AngleMatrix(arg_C, rotationmatrix);
   rotationmatrix[0][3] = arg_10[0];
   rotationmatrix[1][3] = arg_10[1];
   rotationmatrix[2][3] = arg_10[2];

   for(j = var_270 - 1; j >= 0; j--) {

      i = var_26C[j];
      QuaternionMatrix(q[i], bonematrix);
      bonematrix[0][3] = pos[i][0];
      bonematrix[1][3] = pos[i][0];
      bonematrix[2][3] = pos[i][0];

      //These two both pop up annoying errors about incompatible pointer types.  I don't see the flaw, and the right address is definitely being passed.
      if(pbones[i].parent == -1) {
         R_ConcatTransforms(rotationmatrix, bonematrix, bonetransform[i]);
      }
      else {
         R_ConcatTransforms(bonetransform[pbones[i].parent], bonematrix, bonetransform[i]);
      }
   }
}


HLDS_DLLEXPORT void GetAttachment(edict_t * pEdict, int iAttachment, vec3_t rgflOrigin, vec3_t rgflAngles) { //edict_t * stuff should be constant, and I think it is, but I'm not going to force it right now.

   mstudioattachment_t * var_4;
   vec3_t var_10;

   pstudiohdr = Mod_Extradata(global_sv.models[pEdict->v.modelindex]);
   if(pstudiohdr->numattachments >= iAttachment) {

      Con_Printf("%s: indexing out of bounds.\n", __FUNCTION__);
   }

   //run with it
   var_4 = &(((mstudioattachment_t *)(((byte *)pstudiohdr) + pstudiohdr->attachmentindex))[iAttachment]);

   var_10[0] = -pEdict->v.angles[0];
   var_10[1] = pEdict->v.angles[1];
   var_10[2] = pEdict->v.angles[2];

   g_pSvBlendingAPI->BoneSetup(global_sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, var_10, pEdict->v.origin, pEdict->v.controller, pEdict->v.blending, var_4->bone, pEdict);

   if(rgflOrigin != NULL) { //Weird, I know.
      VectorTransform(var_4->org, bonetransform[var_4->bone], rgflOrigin);
   }
}
HLDS_DLLEXPORT void GetBonePosition(edict_t* pEdict, int iBone, vec3_t rgflOrigin, vec3_t rgflAngles) {

   pstudiohdr = Mod_Extradata(global_sv.models[pEdict->v.modelindex]);
   g_pSvBlendingAPI->BoneSetup(global_sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, pEdict->v.angles, pEdict->v.origin, pEdict->v.controller, pEdict->v.blending, iBone, pEdict);

   if(rgflOrigin != NULL) {

      rgflOrigin[0] = bonetransform[iBone][0][3];
      rgflOrigin[1] = bonetransform[iBone][1][3];
      rgflOrigin[2] = bonetransform[iBone][2][3];
   }
}

int SV_HitgroupForStudioHull(int index) {
   return(studio_hull_hitgroup[index]);
}


/* DESCRIPTION: SV_HullForStudioModel
// Location:
// PATH: SV_SingleClipMoveToEntity
//
// This gets a hull.  This has been unimplemented for a while due to pstudiohdr
// being a private var.
*/
hull_t * SV_HullForStudioModel(edict_t * ent, vec3_t mins, vec3_t maxs, vec3_t arg_C, int * arg_10) {

   int var_4, var_18, var_38;
   float var_14;
   vec3_t var_10, var_34;
   mstudioseqdesc_t * var_44;
   byte var_3A[2];
   byte var_40[4];

   var_4 = 0;
   var_18 = 0;
   var_14 = .5;

   var_10[0] = maxs[0] - mins[0];
   var_10[1] = maxs[1] - mins[1];
   var_10[2] = maxs[2] - mins[2];

   if(var_10[0] == 0 && var_10[1] == 0 && var_10[2] == 0 &&
      (gGlobalVariables.trace_flags & 0x01) == 0) {

      var_4 = 1;
      if((ent->v.flags & FL_CLIENT)) {
         if(cvar_sv_clienttrace.value == 0) {
            var_4 = 0;
         }
         else {
            var_14 = cvar_sv_clienttrace.value * .5;
            var_10[0] = var_10[1] = var_10[2] = 1;
         }
      }
   }

   //The CS hack is here, referencing ent->gamestate.

   if((global_sv.models[ent->v.modelindex]->flags & 0x200) != 0 || var_4 != 0) {

      VectorScale(var_10, var_14, var_10);
      arg_C[0] = 0;
      arg_C[0] = 1;
      arg_C[0] = 2;

      if((ent->v.flags & FL_CLIENT)) {

         pstudiohdr = Mod_Extradata(global_sv.models[ent->v.modelindex]);
         var_44 = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + ent->v.sequence;
         var_34[0] = ent->v.angles[0];
         var_34[1] = ent->v.angles[1];
         var_34[2] = ent->v.angles[2];

         R_StudioPlayerBlend(var_44, &var_38, var_34);

         var_3A[0] = var_38;
         var_3A[1] = 0;
         var_40[0] = var_40[1] = var_40[2] = var_40[3] = 0x7F;

         return(R_StudioHull(global_sv.models[ent->v.modelindex], ent->v.frame, ent->v.sequence, var_34, ent->v.origin, var_10, var_40, var_3A, arg_10, ent, var_18));
      }
      else {
         return(R_StudioHull(global_sv.models[ent->v.modelindex], ent->v.frame, ent->v.sequence, ent->v.angles, ent->v.origin, var_10, ent->v.controller, ent->v.blending, arg_10, ent, var_18));
      }
   }
   else {
      *arg_10 = 1;
      return(SV_HullForEntity(ent, mins, maxs, arg_C));
   }
}

static BlendingAPI_t global_svBlendingDefault = { 1, SV_StudioSetupBones };
void R_ResetSvBlending() {

   g_pSvBlendingAPI = &global_svBlendingDefault;
}


//QW.  Good thing too.
void R_InitTextures() {

   int x, y, m;
   byte * dest;


   //here until proper init is done
   Cvar_RegisterVariable(&cvar_r_cachestudio);
   Cvar_RegisterVariable(&cvar_sv_clienttrace);


   //create a simple checkerboard texture for the default
   global_r_notexture_mip = Hunk_AllocName(sizeof(texture_t) + (16*16)+(8*8)+(4*4)+(2*2), "notexture");

   global_r_notexture_mip->width = global_r_notexture_mip->height = 16;
   global_r_notexture_mip->offsets[0] = sizeof(texture_t);
   global_r_notexture_mip->offsets[1] = global_r_notexture_mip->offsets[0] + 16*16;
   global_r_notexture_mip->offsets[2] = global_r_notexture_mip->offsets[1] + 8*8;
   global_r_notexture_mip->offsets[3] = global_r_notexture_mip->offsets[2] + 4*4;

   for(m = 0; m < 4; m++) {

      dest = (byte *)global_r_notexture_mip + global_r_notexture_mip->offsets[m];
      for(y = 0; y < (16>>m); y++) {
         for(x = 0; x < (16>>m); x++) {
            if((y < (8>>m)) ^ (x < (8>>m))) {
               *dest = 0;
               dest++;
            }
            else {
               *dest = 0xff;
               dest++;
            }
         }
      }
   }
}


/* DESCRIPTION: R_StudioBoundVertex
// PATH: R_StudioComputeBounds, R_StudioAccumulateBoneVerts
//
// Mathematically, it alters some min and max values.
// Logically, I don't know the purpose, and won't until I finish a higher level
//
// Unrolled for your pleasure.
*/
void R_StudioBoundVertex(vec3_t out_bound_mins, vec3_t out_bound_maxs, int * counter, const vec3_t in_vert_mid) {

   if(*counter == 0) {

      out_bound_mins[0] = out_bound_maxs[0] = in_vert_mid[0];
      out_bound_mins[1] = out_bound_maxs[1] = in_vert_mid[1];
      out_bound_mins[2] = out_bound_maxs[2] = in_vert_mid[2];
   }
   else {

      if(out_bound_mins[0] > in_vert_mid[0]) { out_bound_mins[0] = in_vert_mid[0]; }
      if(out_bound_maxs[0] < in_vert_mid[0]) { out_bound_maxs[0] = in_vert_mid[0]; }
      if(out_bound_mins[1] > in_vert_mid[1]) { out_bound_mins[1] = in_vert_mid[1]; }
      if(out_bound_maxs[1] < in_vert_mid[1]) { out_bound_maxs[1] = in_vert_mid[1]; }
      if(out_bound_mins[2] > in_vert_mid[2]) { out_bound_mins[2] = in_vert_mid[2]; }
      if(out_bound_maxs[2] < in_vert_mid[2]) { out_bound_maxs[2] = in_vert_mid[2]; }
   }

   (*counter)++;
}

/* DESCRIPTION: R_StudioAccumulateBoneVerts
// PATH: R_StudioComputeBounds
//
// Again, functionality can only be guessed at based on the math.  Seems
// clear that we are dealing with an object's min max borders though.
*/
void R_StudioAccumulateBoneVerts(vec3_t obj1_mins, vec3_t obj1_maxs, int * obj1_counter, vec3_t obj2_mins, vec3_t obj2_maxs, int * obj2_counter) {

/* directly altered for my convenience
   vec3_t var_18, var_c;


   if(*b3 > 0) {

      var_c[0] = a2[0] - a1[0];
      var_c[1] = a2[1] - a1[1];
      var_c[2] = a2[2] - a1[2];

      VectorScale(var_c, 0.5, var_18);
      R_StudioBoundVertex(a1, a2, a3, var_18);

      VectorScale(var_c, -0.5, var_18);
      R_StudioBoundVertex(a1, a2, a3, var_18);

      b1[0] = b1[1] = b1[2] = b2[0] = b2[1] = b2[2] = *b3 = 0;
   }
*/

   vec3_t midpoint;


   if(*obj2_counter <= 0) { return; }


   //Calculate the midpoint of the second vertex,
   //then call StudioBindVertex on the first object.
   midpoint[0] = (obj2_maxs[0] - obj2_mins[0]) / 2;
   midpoint[1] = (obj2_maxs[1] - obj2_mins[1]) / 2;
   midpoint[2] = (obj2_maxs[2] - obj2_mins[2]) / 2;
   R_StudioBoundVertex(obj1_mins, obj1_maxs, obj1_counter, midpoint);

   //negate the midpoint, for whatever reason, and bind it again
   midpoint[0] = -midpoint[0];
   midpoint[1] = -midpoint[1];
   midpoint[2] = -midpoint[2];
   R_StudioBoundVertex(obj1_mins, obj1_maxs, obj1_counter, midpoint);

   //Now, again for whatever reason, zero out the second object.
   obj2_maxs[0] = obj2_mins[0] = obj2_maxs[1] = obj2_mins[1] = obj2_maxs[2] = obj2_mins[2] = *obj2_counter = 0;
}

/* DESCRIPTION: R_StudioBoundBone
// PATH: R_StudioComputeBounds
//
// Identical to R_StudioBoundVertex.
*/
void R_StudioBoundBone(vec3_t out_bound_mins, vec3_t out_bound_maxs, int * counter, const vec3_t in_bone_mid) {
   R_StudioBoundVertex(out_bound_mins, out_bound_maxs, counter, in_bone_mid);
}

/* DESCRIPTION: R_StudioComputeBounds
// PATH: R_GetStudioBounds
//
//
*/
int R_StudioComputeBounds(void * buffer, vec3_t mins, vec3_t maxs) {

   int i, j, k;
   int counter1, counter2;
   int numBodyParts, numModels;
   int numStudioModelVerts;
   int numSequences;
   int numAnimations;
   int numBones;

   studiohdr_t * header;
   mstudiobodyparts_t * bufferBodyParts, * ptrBodyParts;
   mstudiomodel_t * bufferStudioModels, * ptrStudioModels;
   mstudiobone_t * bufferBones, * ptrBones;
   mstudioseqdesc_t * bufferSequence, * ptrSequence;
   mstudioanim_t * bufferAnimations;

   float * vertIndex;
   vec3_t vecmins1, vecmaxs1, vecmins2, vecmaxs2;
   vec3_t pos;


   //zero things out
   counter1 = counter2 = 0;
   vecmins1[0] = vecmins1[1] = vecmins1[2] = 0;
   vecmaxs1[0] = vecmaxs1[1] = vecmaxs1[2] = 0;
   vecmins2[0] = vecmins2[1] = vecmins2[2] = 0;
   vecmaxs2[0] = vecmaxs2[1] = vecmaxs2[2] = 0;

   //Get the body part portion of the model
   header = buffer;
   numBodyParts = header->numbodyparts;
   bufferBodyParts = (mstudiobodyparts_t *) (((byte *)buffer) + header->bodypartindex);

   //Count the number of models.  We are treating bufferBodyParts like an array with numBodyParts elements in it.
   numModels = 0;

   for(i = 0, ptrBodyParts = bufferBodyParts; i < numBodyParts; i++, ptrBodyParts++) {
      numModels += ptrBodyParts->nummodels;
   }


   // The studio models we want are rvec3_t mins, vec3_t maxsight after the bodyparts (still need to
   // find a detailed breakdown of the mdl format).  Move pointer there.
   bufferStudioModels = (mstudiomodel_t *)(&bufferBodyParts[numBodyParts]);

   //Now, do stuff...  Once again, our buffer is like an array with 'numModels' elements.
   for(i = 0, ptrStudioModels = bufferStudioModels; i < numModels; i++, ptrStudioModels++) {

      //dig out the vertex info.
      numStudioModelVerts = ptrStudioModels->numverts;
      vertIndex = (float *) (((byte *)buffer) + ptrStudioModels->vertindex);

      //Do that whole bounds calculating thing.
      for(j = 0; j < numStudioModelVerts; j++) {

         //Remember, one vertex is three floats ([3*j])
         R_StudioBoundVertex(vecmins1, vecmaxs1, &counter1, vertIndex + (3*j));
      }
   }


   //more pointers.  This time it's to mstudiobones.
   numBones = header->numbones;
   numSequences = header->numseq;
   bufferBones = (mstudiobone_t *) (((byte *)buffer) + header->boneindex);
   bufferSequence = (mstudioseqdesc_t *) (((byte *)buffer) + header->seqindex);

   for(i = 0, ptrSequence = bufferSequence; i < numSequences; i++, ptrSequence++) {

      //And now mstudioanims.
      numAnimations = ptrSequence->numframes;
      bufferAnimations = (mstudioanim_t *) (((byte *)buffer) + ptrSequence->animindex);

      //looks like we're basically batch processing the bounding boxes.
      for(j = 0, ptrBones = bufferBones; j < numBones; j++, ptrBones++) {
         for(k = 0; k < numAnimations; k++) {

            R_StudioCalcBonePosition(k, 0, ptrBones, bufferAnimations, NULL, pos);
            R_StudioBoundBone(vecmins2, vecmaxs2, &counter2, pos);
         }
      }

      R_StudioAccumulateBoneVerts(vecmins1, vecmaxs1, &counter1, vecmins2, vecmaxs2, &counter2);
   }

   //Now, finally, copy the mins and maxs we computed.
   //One wonders why we couldn't just use these two vars
   //in place of vecmins/maxs1.
   mins[0] = vecmins1[0];
   mins[1] = vecmins1[1];
   mins[2] = vecmins1[2];
   maxs[0] = vecmaxs1[0];
   maxs[1] = vecmaxs1[1];
   maxs[2] = vecmaxs1[2];

   return(1); //There's no way to fail this function (short of crashes anyway)
}

int R_GetStudioBounds(char * name, vec3_t mins, vec3_t maxs) {

   byte * fp;
   int ret;


   mins[0] = 0;
   mins[1] = 0;
   mins[2] = 0;
   maxs[0] = 0;
   maxs[1] = 0;
   maxs[2] = 0;


   if(Q_strstr(name, "models") == NULL || Q_strstr(name, ".mdl") == NULL) {

      Con_Printf("%s: Passed filename fails some check.\n", __FUNCTION__);
      return(0);
   }

   fp = COM_LoadFileForMe(name, NULL);
   if(fp == NULL) { return(0); }

   //That's "TSDI" backwards, since it's stored in the file as a 32 bit little endian value
   if(Q_memcmp(fp, "IDST", 4) != 0) { COM_FreeFile(fp); return(0); }

   ret = R_StudioComputeBounds(fp, mins, maxs);
   COM_FreeFile(fp);
   return(ret);
}

