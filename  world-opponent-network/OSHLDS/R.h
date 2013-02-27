/*
*/

#ifndef OSHLDS_R_HEADER
#define OSHLDS_R_HEADER

#include "VecMath.h"

#include "endianin.h"
#include "ModCalls.h"

//studio.h stuff
#define STUDIO_VERSION 0x0A

#define MAXSTUDIOTRIANGLES   20000
#define MAXSTUDIOVERTS       2048
#define MAXSTUDIOSEQUENCES   256     // total animation sequences
#define MAXSTUDIOSKINS       100     // total textures
#define MAXSTUDIOSRCBONES    512     // bones allowed at source movement
#define MAXSTUDIOBONES       128     // total bones actually used
#define MAXSTUDIOMODELS      32      // sub-models per model
#define MAXSTUDIOBODYPARTS   32
#define MAXSTUDIOGROUPS      16
#define MAXSTUDIOANIMATIONS  512     // per sequence
#define MAXSTUDIOMESHES      256
#define MAXSTUDIOEVENTS      1024
#define MAXSTUDIOPIVOTS      256
#define MAXSTUDIOCONTROLLERS 8

// lighting options
#define STUDIO_NF_FLATSHADE  0x0001
#define STUDIO_NF_CHROME     0x0002
#define STUDIO_NF_FULLBRIGHT 0x0004

// motion flags
#define STUDIO_X      0x0001
#define STUDIO_Y      0x0002
#define STUDIO_Z      0x0004
#define STUDIO_XR     0x0008
#define STUDIO_YR     0x0010
#define STUDIO_ZR     0x0020
#define STUDIO_LX     0x0040
#define STUDIO_LY     0x0080
#define STUDIO_LZ     0x0100
#define STUDIO_AX     0x0200
#define STUDIO_AY     0x0400
#define STUDIO_AZ     0x0800
#define STUDIO_AXR    0x1000
#define STUDIO_AYR    0x2000
#define STUDIO_AZR    0x4000
#define STUDIO_TYPES  0x7FFF
#define STUDIO_RLOOP  0x8000   // controller that wraps shortest distance

// sequence flags
#define STUDIO_LOOPING  0x0001

// bone flags
#define STUDIO_HAS_NORMALS   0x0001
#define STUDIO_HAS_VERTICES  0x0002
#define STUDIO_HAS_BBOX      0x0004
#define STUDIO_HAS_CHROME    0x0008   // if any of the textures have chrome on them

#define RAD_TO_STUDIO  (32768.0/M_PI)
#define STUDIO_TO_RAD  (M_PI/32768.0)


typedef struct studiohdr_s { //F4

   int    id;
   int    version; //04

   char   name[64]; //08
   int    length; //48

   vec3_t eyeposition;   // ideal eye position
   vec3_t min;         // ideal movement hull size
   vec3_t max;

   vec3_t bbmin;         // clipping bounding box
   vec3_t bbmax;

   int    flags; //88

   int    numbones;         // bones
   int    boneindex;

   int    numbonecontrollers;      // bone controllers
   int    bonecontrollerindex;

   int    numhitboxes;         // complex bounding boxes
   int    hitboxindex;

   int    numseq;            // A4 animation sequences
   int    seqindex;

   int    numseqgroups;      // demand loaded sequences
   int    seqgroupindex;

   int    numtextures;       // raw textures
   int    textureindex;
   int    texturedataindex; //BC?

   int    numskinref;         // replaceable textures
   int    numskinfamilies;
   int    skinindex;

   int    numbodyparts; //CC
   int    bodypartindex;

   int    numattachments;      // queryable attachable points
   int    attachmentindex;

   int    soundtable;
   int    soundindex;
   int    soundgroups;
   int    soundgroupindex;

   int    numtransitions;      // animation node to animation node transition graph
   int    transitionindex;
} studiohdr_t;
typedef struct studioseqhdr_s {

   int  id;
   int  version;

   char name[64];
   int  length;
} studioseqhdr_t;
typedef struct mstudiobone_s { //0x70
   char  name[32];   // 00 bone name for symbolic links
   int   parent;      // 20 parent bone
   int   flags;      // 24 ??
   int   bonecontroller[6];   // 28 bone controller index, -1 == none
   float value[6];   // 40 default DoF values
   float scale[6];   // 58 scale for delta DoF values
} mstudiobone_t;
typedef struct mstudiobonecontroller_t {

   int   bone;   // -1 == 0
   int   type;   // X, Y, Z, XR, YR, ZR, M
   float start;
   float end;
   int   rest;   // byte index value at rest
   int   index;  // 0-3 user set controller, 4 mouth
} mstudiobonecontroller_t;
typedef struct mstudiobbox_s {
   int    bone;
   int    group;         // intersection group
   vec3_t bbmin;      // bounding box
   vec3_t bbmax;
} mstudiobbox_t;
typedef struct mstudioseqgroup_s {
   char         label[32];   // textual name
   char         name[64];   // file name
   cache_user_t cache;      // cache index pointer
   int          data;      // hack for group 0
} mstudioseqgroup_t;
typedef struct mstudioseqdesc_s {
   char   label[32];   // 00 sequence label

   float  fps;      // 20 frames per second
   int    flags;      // 24 looping/non-looping flags

   int    activity;
   int    actweight;

   int    numevents;
   int    eventindex;

   int    numframes;   // 38 number of frames per sequence

   int    numpivots;   // 3c number of foot pivots
   int    pivotindex;

   int    motiontype;
   int    motionbone;
   vec3_t linearmovement; //4c
   int    automoveposindex; //58
   int    automoveangleindex;

   vec3_t bbmin;      // 60 per sequence bounding box
   vec3_t bbmax; //6c

   int    numblends; //78
   int    animindex;      // mstudioanim_t pointer relative to start of sequence group data
                    // [blend][bone][X, Y, Z, XR, YR, ZR]

   int    blendtype[2];   // X, Y, Z, XR, YR, ZR
   float  blendstart[2];   // starting value
   float  blendend[2];   // ending value
   int    blendparent;

   int    seqgroup;      // sequence group for demand loading

   int    entrynode;      // transition node at entry
   int    exitnode;      // transition node at exit
   int    nodeflags;      // transition rules

   int    nextseq;      // auto advancing sequences
} mstudioseqdesc_t;
typedef struct mstudioevent_s {
   int  frame;
   int  event;
   int  type;
   char options[64];
} mstudioevent_t;
typedef struct mstudiopivot_s {
   vec3_t org;   // pivot point
   int    start;
   int    end;
} mstudiopivot_t;
typedef struct mstudioattachment_s { //0x58
   char   name[32]; //00
   int    type; //20
   int    bone; //24
   vec3_t org;   // 28attachment point
   vec3_t vectors[3]; //34
} mstudioattachment_t;
typedef struct mstudioanim_s {
   unsigned short offset[6];
} mstudioanim_t;
typedef union mstudioanimvalue_s {
   struct {
      byte   valid;
      byte   total;
   } num;
   short      value;
} mstudioanimvalue_t;


//struct
typedef struct BlendingAPI_s {

   int padding00;
   void (*BoneSetup)(model_t *, float, int, vec3_t, vec3_t, byte *, byte *, int, edict_t *); //SV_StudioSetupBones
} BlendingAPI_t;

//globals
extern texture_t * global_r_notexture_mip;

// These are here because of SV_CheckBlendingInterface.
// SV_CheckBlendingInterface CLEARLY belongs in R.c, but it uses DLL
// functions, which are better off in their own file.
extern vec_t bonetransform[0x80][3][4];
extern vec_t rotationmatrix[3][4];
extern BlendingAPI_t * g_pSvBlendingAPI;


//functions
int SV_HitgroupForStudioHull(int index);
hull_t * SV_HullForStudioModel(edict_t * ent, vec3_t mins, vec3_t maxs, vec3_t arg_C, int * arg_10);
int R_GetStudioBounds(char * name, vec3_t mins, vec3_t maxs);
hull_t * R_StudioHull(model_t * arg_0, float arg_4, int arg_8, vec3_t arg_C, vec3_t arg_10, vec3_t arg_14, byte * arg_18, byte * arg_1C, int * OUT_arg_20_total, edict_t * OUT_arg_24, int arg_isCS);


HLDS_DLLEXPORT void GetAttachment(edict_t *, int, vec3_t, vec3_t);
HLDS_DLLEXPORT void GetBonePosition(edict_t*, int, vec3_t, vec3_t);

void R_InitTextures();
void R_ResetSvBlending();
#endif
