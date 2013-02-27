#include <math.h>

#include "Mod.h"
#include "common.h"
#include "crc.h"
#include "cvar.h"
#include "draw.h"
#include "Modding.h"
#include "NET.h"
#include "R.h"
#include "Tex.h"
#include "sys.h"
#include "VecMath.h"
#include "report.h"
#include "Q_memdef.h"

// This file handles various modeling functions.
// Unknown error is causing crashes in Physics.

#define MAX_MOD_KNOWN 0x400

static model_t mod_known[MAX_MOD_KNOWN];
static int mod_numknown;
static model_t *loadmodel;
static byte *mod_base;
static char loadname[64];
static char * wadpath = NULL;

static int r_pixbytes = 1;
static crc_info_t mod_known_info[MAX_MOD_KNOWN]; //static

static int tested = 0;
static int ad_enabled = 0;
static tempwad_t  ad_wad;

static int gPVSRowBytes = 0;
static byte * gPAS = NULL;
static byte * gPVS = NULL;
static byte mod_novis[0x400]; //MAX_MAP_LEAFS/8

static cvar_t cvar_r_wadtextures  = { "r_wadtextures", "0", FCVAR_NONE, 0.0, NULL };


//Models

/* DESCRIPTION: Mod_ClearAll
// LOCATION: global_svmode/sw_model.c
// PATH: Host_ClearMemory
//
// Merely alters some status flags...
*/
void Mod_ClearAll() {

   int i;
   model_t *mod;

   for(i = 0, mod = mod_known; i < mod_numknown; i++, mod++) {
      if(mod->loadstate != 0x03) {
         mod->loadstate = 0x02;
      }
   }
}

/* DESCRIPTION: Mod_FillInCRCInfo
// LOCATION:
// PATH: Mod_FindName
//
// Zeros something out...
*/
void Mod_FillInCRCInfo(int bool2, int index) {

   crc_info_t * CRC;

   CRC = &(mod_known_info[index]);

   CRC->padding00 = bool2;
   CRC->padding04 = 0;
   CRC->padding08 = 0;
}

/* DESCRIPTION: Mod_FindName
// LOCATION: global_svmode/sw_model.c
// PATH: local to MOD functions
//
// Returns a pointer to the model struct with the passed name.
// If that model doesn't exist, space is made for that model,
// and the returned model's flags indicate that the model hasn't
// been loaded into memory.
*/
model_t * Mod_FindName(int bool2, char * name) {

   int i;
   int var_c;
   model_t *mod;

   if(*name == '\0') {
      Sys_Error ("%s: NULL name", __FUNCTION__);
   }

   var_c = -1;

   for(i = 0, mod = mod_known; i < mod_numknown; i++, mod++) {

      if(Q_strcasecmp(mod->name, name) == 0) { break; }

      if(mod->loadstate == 2 && (var_c == -1 || (mod->modeltype != 0x02 && mod->modeltype != 0x03))) {
         var_c = i;
      }
   }

   if(i == mod_numknown) {

      if(mod_numknown >= MAX_MOD_KNOWN) {
         if(var_c == -1) {
            Sys_Error("%s: mod_numknown == MAX_MOD_KNOWN", __FUNCTION__);
         }
         else {
            mod = &(mod_known[var_c]);
            Con_Printf("%s: Reusing model %s, MAX_MOD_KNOWN reached.", __FUNCTION__, mod->name);
            Mod_FillInCRCInfo(bool2, var_c);
         }
      }
      else {

         Mod_FillInCRCInfo(bool2, mod_numknown);
         mod_numknown++;
      }

        //memset(mod, 0, sizeof(model_t)); //I think it's a good idea, but it's not in the asm.
      Q_strncpy(mod->name, name, 63);
      mod->name[63] = '\0';
      if(mod->loadstate != 0x03) { mod->loadstate = 0x01; }
   }

   return(mod);
}

/* DESCRIPTION: Mod_ForName
// LOCATION: global_svmode/sw_model.c
// PATH: PF_precache_model_I, SV_SpawnServer
//
// Loads a model.  The first bool signals whether this call is allowed to
// return empty handed.  The second one I don't know about yet.
*/
model_t * Mod_LoadModel(model_t *, int, int);
model_t * Mod_ForName(char * name, int crash, int bool2) {

   model_t * mod;

   mod = Mod_FindName(bool2, name);

   if(mod == NULL) { return(NULL); }
   return(Mod_LoadModel(mod, crash, bool2));
}


/* DESCRIPTION: Mod_MakeHull0
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// I'm still a little fuzzy on how hulls work, but this function--which
// is identical to the QW counterpart--is clearly making one.
*/
void Mod_MakeHull0() {

   mnode_t *in, *child;
   dclipnode_t *out;
   int i, j, count;
   hull_t *hull;

   hull = &loadmodel->hulls[0];

   in = loadmodel->nodes;
   count = loadmodel->numnodes;
   out = (dclipnode_t *)Hunk_AllocName(count * sizeof(dclipnode_t), loadname);

   hull->clipnodes = out;
   hull->firstclipnode = 0;
   hull->lastclipnode = count-1;
   hull->planes = loadmodel->planes;

   for(i = 0; i < count; i++, out++, in++) {

      out->planenum = in->plane - loadmodel->planes;
      for(j = 0; j < 2; j++) {

         child = in->children[j];
         if(child->contents < 0) { out->children[j] = child->contents; }
         else { out->children[j] = child - loadmodel->nodes; }
      }
   }
}

/* DESCRIPTION: RadiusFromBounds
// LOCATION: sw_model.c
// PATH: Mod_LoadBrushModel
//
// This shouldn't be too hard, but as I have a block against the '?' operator
// I'm not going to tinker with it.
*/
float RadiusFromBounds(vec3_t mins, vec3_t maxs) {

   int i;
   vec3_t corner;

   for(i=0 ; i<3 ; i++) {

       corner[i] = fabs(mins[i]) > fabs(maxs[i]) ? fabs(mins[i]) : fabs(maxs[i]);
   }

   return(Length(corner));
}

/* DESCRIPTION: RadiusFromBounds
// LOCATION: sw_model.c
// PATH: Mod_LoadNodes, recursive
//
// Not much to this.  Assigns the 'parent' field in a model to something that
// isn't stupid.
*/
void Mod_SetParent(mnode_t *node, mnode_t *parent) {

   node->parent = parent;
   if(node->contents < 0) { return; }

   Mod_SetParent(node->children[0], node);
   Mod_SetParent(node->children[1], node);
}
//This hasn't been reviewed.
void CalcSurfaceExtents(msurface_t *s) {

   float   mins[2], maxs[2], val;
   int i,j, e;
   mvertex_t   *v;
   mtexinfo_t   *tex;
   int bmins[2], bmaxs[2];

   mins[0] = mins[1] = 999999; // This is correct
   maxs[0] = maxs[1] = -99999; // (if odd)

   tex = s->texinfo;

   for (i=0 ; i<s->numedges ; i++)
   {
      e = loadmodel->surfedges[s->firstedge+i];
      if (e >= 0)
         v = &loadmodel->vertexes[loadmodel->edges[e].v[0]];
      else
         v = &loadmodel->vertexes[loadmodel->edges[-e].v[1]];

      for (j=0 ; j<2 ; j++)
      {
         val = v->position[0] * tex->vecs[j][0] +
            v->position[1] * tex->vecs[j][1] +
            v->position[2] * tex->vecs[j][2] +
            tex->vecs[j][3];
         if (val < mins[j])
            mins[j] = val;
         if (val > maxs[j])
            maxs[j] = val;
      }
   }

   for (i=0 ; i<2 ; i++)
   {
      bmins[i] = floor(mins[i]/16);
      bmaxs[i] = ceil(maxs[i]/16);

      s->texturemins[i] = bmins[i] * 16;
      s->extents[i] = (bmaxs[i] - bmins[i]) * 16;
//      if ( !(tex->flags & TEX_SPECIAL) && s->extents[i] > 256)
//         SV_Error ("Bad surface extents");
   }
}



//The brush model family

/* DESCRIPTION: Mod_LoadEntities
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Copy memory from the file, paste it into the hunk.  For some reason, we
// also extract the key/pair "wad".
*/
void Mod_LoadEntities(lump_t *l) {

   const char * var_4;

   if(l->filelen == 0) {
      loadmodel->entities = NULL;
      return;
   }

   loadmodel->entities = Hunk_AllocName(l->filelen + 1, loadname);
   Q_memcpy(loadmodel->entities, mod_base + l->fileofs, l->filelen);
   loadmodel->entities[l->filelen] = '\0';

   var_4 = COM_Parse(loadmodel->entities);
   while(1) {

      if(var_4 == NULL || var_4[0] == '\0' || global_com_token[0] == '}') { return; }

      if(Q_strcmp("wad", global_com_token) == 0) {

         var_4 = COM_Parse(var_4);
         if(wadpath != NULL) {
            Q_Free(wadpath);
         }

         //Given the rest of the memory #defines, we probably should just malloc and copy the string ourselves.
         wadpath = strdup(global_com_token);
         if(wadpath == NULL) {

            Sys_Error("%s: Call to strdup failed: probably out of memory.", __FUNCTION__);
         }
      }

      var_4 = COM_Parse(var_4);
   }
}

/* DESCRIPTION: Mod_LoadPlanes
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Copies planes from the file into the hunk, linked up to the model struct.
// Some of the plane data is generated; the planes stored in the file only
// contain part of the info.
*/
void Mod_LoadPlanes(lump_t *l) {

   int i, j;
   mplane_t   *out;
   cplane_t *in; //Not certain on the plane types...
   int count;
   int bits;

   in = (cplane_t *)(mod_base + l->fileofs);

   if(l->filelen % sizeof(cplane_t)) {
      Sys_Error("%s: funny lump size in %s\n", __FUNCTION__, loadmodel->name);
   }

   count = l->filelen / sizeof(cplane_t);
   out = Hunk_AllocName(count * 2 * sizeof(mplane_t), loadname);

   loadmodel->planes = out;
   loadmodel->numplanes = count;

   for(i = 0; i < count; i++, in++, out++) {

      bits = 0;
      for(j = 0; j < 3; j++) {

         out->normal[j] = letohost32(in->normal[j]); //little endian conversion should be just dandy on a float.
         if(out->normal[j] < 0) {
            bits |= (1 << j);
         }
      }

      out->dist = letohost32(in->dist);
      out->type = letohost32(in->type);
      out->signbits = bits;
   }
}

void Mod_AdInit() {

   int var_4;
   char * var_8;
   static char filename_36[0x104];


   tested = 1;

   var_4 = COM_CheckParmCase("-ad");
   if(var_4 == 0) { return; }

   var_8 = global_com_argv[var_4];
   if(var_8 == NULL) { return; }

   snprintf(filename_36, sizeof(filename_36)-1, "%s", var_8);

   var_4 = COM_FileSize(filename_36);
   if(var_4 <= 0) {
      Con_Printf("Couldn't open file specified by \"-ad\" switch.\n");
   }

   Draw_CacheWadInit(filename_36, 0x10, &ad_wad);
   ad_enabled = 1;
}
void Mod_AdSwap(texture_t * tex, int size, int count) {

   int i;
   texture_t * var_10;
   TextureStruct1_t * var_C;
   byte * var_8; //Three at a time.


   if(tested == 0) { return; }

   var_10 = Draw_CacheGet(&ad_wad, Draw_CacheIndex(&ad_wad, "img"));
   if(var_10 == NULL) { return; }

   Q_memcpy(tex+1, var_10+1, size);
   var_C = (TextureStruct1_t *)((byte *)tex + sizeof(texture_t) + size + 2);
   var_8 = ((byte *)var_10 + sizeof(texture_t) + size + 2);

   for(i = 0; i < count; i++) {

      var_C[i].values[3] = 0;
      var_C[i].values[2] = var_8[3*i + 0];
      var_C[i].values[1] = var_8[3*i + 1];
      var_C[i].values[0] = var_8[3*i + 2];
   }
}

/* DESCRIPTION: Mod_LoadTextures
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Things start out easily, if convoluted, enough.  We grab some memory and
// start copying data over.  First, the structs we know... Then, data that's
// after the structs we know.  And then more data after the data we know, sans
// any pleasant accessing mechanism.
//
// And then it gets weird.
//
// Known file structure:
// dmiptexlump_t = start of segment
// miptex_t = offset from segment start, repeated for N textures
// pixels--variable size texture, follows miptex_t.
// 2 byte size counter
// dragons--three byte struct * 2 byte counter follows.  Colors I think.
//
*/
void Mod_LoadTextures(lump_t *l) {

   int      i, j, pixels, num, max, altmax;
   miptex_t   *mt;
   texture_t   *tx, *tx2;
   texture_t   *anims[10];
   texture_t   *altanims[10];
   dmiptexlump_t *m;

   int var_10_sizeofsomething;
   char * var_80_unknownstruct;
   TextureStruct1_t * var_84_unknownstruct;


   double var_553CC;
   double var_553D8;
   byte var_553C8[350000]; //Give or take.  This is a BAD thing though.


   var_553CC = 0;
   var_553D8 = Sys_FloatTime();

   if(tested == 0) {
      Mod_AdInit();
   }

   if(l->filelen == 0) {
      loadmodel->textures = NULL; //174
      return;
   }

   m = (dmiptexlump_t *)(mod_base + l->fileofs);

   m->nummiptex = letohost32(m->nummiptex);

   loadmodel->numtextures = m->nummiptex;
   loadmodel->textures = Hunk_AllocName(m->nummiptex * sizeof(texture_t) , loadname);

   for(i = 0; i < m->nummiptex; i++) {

      m->dataofs[i] = letohost32(m->dataofs[i]);
      if(m->dataofs[i] == -1) { continue; }

      mt = (miptex_t *)((byte *)m + m->dataofs[i]);

      if(cvar_r_wadtextures.value != 0 || letohost32(mt->offsets[0] == 0)) {

         if(var_553CC == 0) {

            TEX_InitFromWad(wadpath);
            TEX_AddAnimatingTextures();
            var_553CC = 1;
         }

         if(TEX_LoadLump(mt, var_553C8) == 0) {
            m->dataofs[i] = -1;
            continue;
         }
         mt = (miptex_t *)var_553C8;
      }

      for(j = 0; j < MIPLEVELS; j++) {
         mt->offsets[j] = letohost32(mt->offsets[j]);
      }

      mt->width = letohost32(mt->width);
      mt->height = letohost32(mt->height);

      if((mt->width & 15) != 0 || (mt->height & 15) != 0) {
         Sys_Error("Texture %s is not correctly aligned.\n", mt->name);
      }

      pixels = mt->width * mt->height / 64 * 85;

      var_10_sizeofsomething = ((TextureStruct2_t *)((((byte *)(mt+1)) + pixels)))->NumStructs * sizeof(TextureStruct1_t);

      tx = Hunk_AllocName(sizeof(texture_t) + pixels + var_10_sizeofsomething + 2, loadname);
      loadmodel->textures[i] = tx;
      Q_memcpy(tx->name, mt->name, sizeof(tx->name));

      if(strchr(tx->name, '~') != NULL) {
         tx->name[2] = ' '; //Iunno.
      }

      tx->width = mt->width;
      tx->height = mt->height;

      for(j = 0; j < MIPLEVELS; j++) {
         tx->offsets[j] = mt->offsets[j] + sizeof(texture_t) - sizeof(miptex_t); //which is +18h overall
      }
      // the pixels immediately follow the structures
      memcpy(tx+1, mt+1, pixels+2); //ugh, pointer math. tx+1 == the byte AFTER the entire tx structure.

      var_80_unknownstruct = (char *)(((byte *)(mt+1)) + pixels + 2); //source
      var_84_unknownstruct = (TextureStruct1_t *)(((byte *)tx+1) + pixels + 2); //destination

//      if(Q_strncmp("sky", mt->name, 3) == 0) { R_InitSky(); } //R_initsky is empty

      for(j = 0; j < var_10_sizeofsomething/8; j++) {
         var_84_unknownstruct[i].values[3] = 0;
         var_84_unknownstruct[i].values[2] = texgammatable[var_80_unknownstruct[3*i] & 0xFF]; //char to short
         var_84_unknownstruct[i].values[1] = texgammatable[var_80_unknownstruct[3*i + 1] & 0xFF];
         var_84_unknownstruct[i].values[0] = texgammatable[var_80_unknownstruct[3*i + 2] & 0xFF];
      }

      if(ad_enabled != 0 && Q_strcasecmp("DEFAULT", tx->name) == 0) {

         Mod_AdSwap(tx, pixels, var_10_sizeofsomething/8);
      }
   }

   if(var_553CC != 0) { TEX_CleanupWadInfo(); }

   // sequence the animations
   for(i = 0; i < m->nummiptex; i++) {

      tx = loadmodel->textures[i];

      if(tx == NULL || (tx->name[0] != '+' && tx->name[0] != '-') || tx->anim_next != NULL) {
         continue; // The last one means it was already sequenced
      }

      // find the number of frames in the animation
      memset(anims, 0, sizeof(anims));
      memset(altanims, 0, sizeof(altanims));

      max = tx->name[1];
      altmax = 0;
      if(max >= 'a' && max <= 'z') { max -= 'a' - 'A'; } //Last I checked there's a ctype function for this...

      if(max >= '0' && max <= '9') {
         max -= '0';
         altmax = 0;
         anims[max] = tx;
         max++;
      }
      else if(max >= 'A' && max <= 'J') {
         altmax = max - 'A';
         max = 0;
         altanims[altmax] = tx;
         altmax++;
      }
      else {
         Sys_Error("Bad animating texture %s.", tx->name);
      }

      for(j = i+1; j < m->nummiptex; j++) {

         tx2 = loadmodel->textures[j];

         if(tx2 == NULL || (tx2->name[0] != '+' && tx2->name[0] != '-') || (Q_strcmp(tx2->name+2, tx->name+2) != 0)) {
            continue;
         }

         num = tx2->name[1];
         if(num >= 'a' && num <= 'z') { num -= 'a' - 'A'; }

         if(num >= '0' && num <= '9') {
            num -= '0';
            anims[num] = tx2;
            if(num+1 > max) {
               max = num + 1;
            }
         }
         else if(num >= 'A' && num <= 'J') {
            num = num - 'A';
            altanims[num] = tx2;
            if(num+1 > altmax) {
               altmax = num+1;
            }
         }
         else {
            Sys_Error("Bad animating texture %s", tx->name);
         }
      }

      // link them all together
      for(j = 0; j < max; j++) {

         tx2 = anims[j];
         if(tx2 == NULL) {
            Sys_Error ("Missing frame %i of %s", j, tx->name);
         }
         tx2->anim_total = max;
         tx2->anim_min = j;
         tx2->anim_max = (j+1);
         tx2->anim_next = anims[(j+1)%max];
         if(altmax != 0) {
            tx2->alternate_anims = altanims[0];
         }
      }
      for(j = 0; j < altmax; j++) {

         tx2 = altanims[j];
         if(tx2 == NULL) {
            Sys_Error ("Missing frame %i of %s", j, tx->name);
         }
         tx2->anim_total = altmax;
         tx2->anim_min = j;
         tx2->anim_max = (j+1);
         tx2->anim_next = altanims[ (j+1)%altmax ];
         if(max != 0) {
            tx2->alternate_anims = anims[0];
         }
      }
   }

  // Con_Printf("Loaded texture in %f us.\n", Sys_FloatTime() - var_553D8);
}

/* DESCRIPTION: Mod_LoadVertexes
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Loads some data, simply and elegantly.
*/
void Mod_LoadVertexes(lump_t *l) {

   dvertex_t * in;
   mvertex_t * out;
   int i, count;

   in = (dvertex_t *)(mod_base + l->fileofs);

   if(l->filelen % sizeof(dvertex_t) != 0) {
      Sys_Error("%s: funny lump size in %s", __FUNCTION__, loadmodel->name);
   }

   count = l->filelen / sizeof(dvertex_t);
   out = Hunk_AllocName(count * sizeof(mvertex_t), loadname);

   loadmodel->vertexes = out;
   loadmodel->numvertexes = count;

   for(i = 0; i < count; i++, in++, out++) {
      out->position[0] = letohost32(in->point[0]);
      out->position[1] = letohost32(in->point[1]);
      out->position[2] = letohost32(in->point[2]);
   }
}

/* DESCRIPTION: Mod_LoadVisibility
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Loads some data as well.  Much needed break after textures.
*/
void Mod_LoadVisibility(lump_t *l) {

   if(l->filelen == 0) {
      loadmodel->visdata = NULL;
      return;
   }

   loadmodel->visdata = Hunk_AllocName(l->filelen, loadname);
   Q_memcpy(loadmodel->visdata, mod_base + l->fileofs, l->filelen);
}

/* DESCRIPTION: Mod_LoadNodes
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Loads data from a file and ports it to the internal structure used by HL.
*/
void Mod_LoadNodes(lump_t *l) {

   int i, j, count, p;
   dnode_t *in;
   mnode_t *out;

   in = (void *)(mod_base + l->fileofs);
   if(l->filelen % sizeof(dnode_t) != 0) {
      Sys_Error("%s: funny lump size in %s\n", __FUNCTION__, loadmodel->name);
      return;
   }
   count = l->filelen / sizeof(dnode_t);
   out = Hunk_AllocName(count * sizeof(mnode_t), loadname);

   loadmodel->nodes = out;
   loadmodel->numnodes = count;

   for(i = 0; i < count; i++, in++, out++) {

      for(j = 0; j < 3; j++) {
         out->minmaxs[j] = letohost16(in->mins[j]);
         out->minmaxs[3+j] = letohost16(in->maxs[j]);
      }

      p = letohost32(in->planenum);
      out->plane = loadmodel->planes + p;

      out->firstsurface = letohost16(in->firstface);
      out->numsurfaces = letohost16(in->numfaces);

      for(j = 0; j < 2; j++) {

         p = letohost16(in->children[j]);
         if(p >= 0) {
            out->children[j] = loadmodel->nodes + p;
         }
         else {
            out->children[j] = (mnode_t *)(loadmodel->leafs + (-1 - p));
         }
      }
   }

   if(count != 0) {
      Mod_SetParent(loadmodel->nodes, NULL);   // sets nodes and leafs
   }
}

/* DESCRIPTION: Mod_LoadTexinfo
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// More loading function.  This one isn't too bad, though the bit with
// mipadjusts was different from QW.
*/
void Mod_LoadTexinfo(lump_t *l) {

   dtexinfo_t *in;
   mtexinfo_t *out;
   int i, j, count;
   int miptex;
   float   len1, len2;

   in = (dtexinfo_t *)(mod_base + l->fileofs);

   if(l->filelen % sizeof(dtexinfo_t) != 0) {
      Sys_Error("%s: funny lump size in %s", __FUNCTION__, loadmodel->name);
   }

   count = l->filelen / sizeof(dtexinfo_t);
   out = Hunk_AllocName(count * sizeof(mtexinfo_t), loadname);

   loadmodel->texinfo = out;
   loadmodel->numtexinfo = count;

   for(i = 0; i < count; i++, in++, out++) {
#if 0 //HL uses this, but they look equivalent to me.
      for (j=0 ; j<8 ; j++)
         out->vecs[0][j] = letohost32(in->vecs[0][j]);
      len1 = Length (in->vecs[0]);
      len2 = Length (in->vecs[1]);
#else
      for (j=0 ; j<4 ; j++) {
         out->vecs[0][j] = letohost32(in->vecs[0][j]);
         out->vecs[1][j] = letohost32(in->vecs[1][j]);
      }
      len1 = Length (out->vecs[0]);
      len2 = Length (out->vecs[1]);
#endif
      len1 = (len1+len2)/2;

      if(len1 < .32) {
         out->mipadjust = 4;
      }
      else if(len1 < .49) {
         out->mipadjust = 3;
      }
      else if(len1 < .99) {
         out->mipadjust = 2;
      }
      else {
         out->mipadjust = 1;
      }

      miptex = letohost32(in->miptex);
      out->flags = letohost32(in->flags);

      if(loadmodel->textures == NULL) {

         out->texture = global_r_notexture_mip; // checkerboard texture
         out->flags = 0;
      }
      else {

         if(miptex >= loadmodel->numtextures) {
            Sys_Error("%s: miptex >= loadmodel->numtextures", __FUNCTION__);
         }
         out->texture = loadmodel->textures[miptex];

         if(out->texture == NULL) {
            out->texture =  global_r_notexture_mip; // texture not found
            out->flags = 0;
         }
      }
   }
}

/* DESCRIPTION: Mod_LoadFaces
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// More loading.  The end had several differences.
*/
void Mod_LoadFaces(lump_t *l) {

   dface_t *in;
   msurface_t *out;
   int i, count, surfnum;
   int planenum, side;

   in = (dface_t *)(mod_base + l->fileofs);
   if(l->filelen % sizeof(dface_t)) {
      Sys_Error("%s: funny lump size in %s",loadmodel->name, __FUNCTION__);
   }
   count = l->filelen / sizeof(dface_t);
   out = Hunk_AllocName(count * sizeof(msurface_t), loadname);

   loadmodel->surfaces = out;
   loadmodel->numsurfaces = count;

   for(surfnum = 0; surfnum < count; surfnum++, in++, out++) {

      out->firstedge = letohost32(in->firstedge);
      out->numedges = letohost16(in->numedges);
      out->flags = 0;
      out->pdecals = NULL;

      planenum = letohost16(in->planenum);
      side = letohost16(in->side);
      if(side != 0) {
         out->flags |= 2; //SURF_PLANEBACK
      }

      out->plane = loadmodel->planes + planenum;
      out->texinfo = loadmodel->texinfo + letohost16(in->texinfo);

      CalcSurfaceExtents(out);

      // lighting info

      for(i = 0; i < MAXLIGHTMAPS; i++) {
         out->styles[i] = in->styles[i];
      }

      i = letohost32(in->lightofs);
      if(i == -1) {
         out->samples = NULL;
      }
      else {
         out->samples = (color24_t *)loadmodel->lightdata + i;
      }

      // set the drawing flags flag

      if(Q_strncmp(out->texinfo->texture->name, "sky", 3) == 0) {

         out->flags |= 0x24; //SURF_DRAWSKY | SURF_DRAWTILED
         continue;
      }
      if(Q_strncmp(out->texinfo->texture->name, "scroll", 6) == 0) {

         out->flags |= 0x20;
         out->extents[0] = out->texinfo->texture->width;
         out->extents[1] = out->texinfo->texture->height;
         continue;
      }
      if(out->texinfo->texture->name[0] == '!' ||
         Q_strncmp(out->texinfo->texture->name, "laser", 5) == 0 ||
         Q_strncmp(out->texinfo->texture->name, "water", 5) == 0) {

         out->flags |= 0x30; //(SURF_DRAWTURB | SURF_DRAWTILED)
         for(i = 0; i < 2; i++) {
            out->extents[i] = 16384;
            out->texturemins[i] = -8192;
            out->texinfo->flags |= 1; //odd place...
         }
         continue;
      }

      if((out->texinfo->flags & 1) != 0) {

         out->flags |= 0x20;
         out->extents[0] = out->texinfo->texture->width;
         out->extents[1] = out->texinfo->texture->height;
      }
   }
}

/* DESCRIPTION: Mod_LoadFaces
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Some nice, easy memcpying.
*/
void Mod_LoadLighting(lump_t *l) {

   if(l->filelen == 0) {
      loadmodel->lightdata = NULL;
      return;
   }
   loadmodel->lightdata = Hunk_AllocName( l->filelen, loadname);
   memcpy(loadmodel->lightdata, mod_base + l->fileofs, l->filelen);
}

/* DESCRIPTION: Mod_LoadFaces
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Loading.  QW has a branch, depending on if it's loading a HL node or not.
*/
void Mod_LoadClipnodes(lump_t *l) {

   dclipnode_t *in, *out;
   int i, count;
   hull_t *hull;

   in = (void *)(mod_base + l->fileofs);

   if(l->filelen % sizeof(dclipnode_t)) {
      Sys_Error("%s: funny lump size in %s\n", __FUNCTION__, loadmodel->name);
      return;
   }

   count = l->filelen / sizeof(dclipnode_t);
   out = Hunk_AllocName ( count*sizeof(dclipnode_t), loadname);

   loadmodel->clipnodes = out;
   loadmodel->numclipnodes = count;


   hull = &loadmodel->hulls[1];
   hull->clipnodes = out;
   hull->firstclipnode = 0;
   hull->lastclipnode = count-1;
   hull->planes = loadmodel->planes;
   hull->clip_mins[0] = -16;
   hull->clip_mins[1] = -16;
   hull->clip_mins[2] = -36;
   hull->clip_maxs[0] = 16;
   hull->clip_maxs[1] = 16;
   hull->clip_maxs[2] = 36;

   hull = &loadmodel->hulls[2];
   hull->clipnodes = out;
   hull->firstclipnode = 0;
   hull->lastclipnode = count-1;
   hull->planes = loadmodel->planes;
   hull->clip_mins[0] = -32;
   hull->clip_mins[1] = -32;
   hull->clip_mins[2] = -32;
   hull->clip_maxs[0] = 32;
   hull->clip_maxs[1] = 32;
   hull->clip_maxs[2] = 32;

   hull = &loadmodel->hulls[3];
   hull->clipnodes = out;
   hull->firstclipnode = 0;
   hull->lastclipnode = count-1;
   hull->planes = loadmodel->planes;
   hull->clip_mins[0] = -16;
   hull->clip_mins[1] = -16;
   hull->clip_mins[2] = -18;
   hull->clip_maxs[0] = 16;
   hull->clip_maxs[1] = 16;
   hull->clip_maxs[2] = 18;

   for(i = 0; i < count; i++, out++, in++) {

      out->planenum = letohost32(in->planenum);
      out->children[0] = letohost16(in->children[0]);
      out->children[1] = letohost16(in->children[1]);
   }
}

/* DESCRIPTION: Mod_LoadLeafs
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Loading.  Seems to match QW.
*/
void Mod_LoadLeafs(lump_t *l) {

   dleaf_t *in;
   mleaf_t *out;
   int i, j, count, p;

   in = (void *)(mod_base + l->fileofs);
   if(l->filelen % sizeof(dleaf_t) != 0) {
      Sys_Error("%s: funny lump size in %s\n", __FUNCTION__, loadmodel->name);
      return;
   }

   count = l->filelen / sizeof(dleaf_t);
   out = Hunk_AllocName(count * sizeof(mleaf_t), loadname);

   loadmodel->leafs = out;
   loadmodel->numleafs = count;

   for(i = 0; i < count; i++, in++, out++) {

      for(j = 0; j < 3 ; j++) {
         out->minmaxs[j] = letohost16(in->mins[j]);
         out->minmaxs[3+j] = letohost16(in->maxs[j]);
      }

      p = letohost32(in->contents);
      out->contents = p;

      out->firstmarksurface = loadmodel->marksurfaces + letohost16(in->firstmarksurface);
      out->nummarksurfaces = letohost16(in->nummarksurfaces);

      p = letohost32(in->visofs);
      if(p == -1) { out->compressed_vis = NULL; }
      else { out->compressed_vis = loadmodel->visdata + p; }

      out->efrags = NULL;

      for(j = 0; j < 4; j++) {
         out->ambient_sound_level[j] = in->ambient_level[j];
      }
   }
}

/* DESCRIPTION: Mod_LoadMarksurfaces
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Loads some data.  The shorts make it seem odd, but it's really
// a very nice, simple function.
*/
void Mod_LoadMarksurfaces(lump_t *l) {

   int i, j, count;
   short *in;
   msurface_t **out;

   in = (short *)(mod_base + l->fileofs);

   if(l->filelen % sizeof(short)) {
      Sys_Error("%s: funny lump size in %s", __FUNCTION__, loadmodel->name);
   }

   count = l->filelen / sizeof(short);
   out = Hunk_AllocName(count * sizeof(msurface_t), loadname);

   loadmodel->marksurfaces = out;
   loadmodel->nummarksurfaces = count;

   for(i = 0; i < count; i++) {

      j = letohost16(in[i]);
      if (j >= loadmodel->numsurfaces) {
         Sys_Error("%s: bad surface number", __FUNCTION__);
      }
      out[i] = loadmodel->surfaces + j;
   }
}

/* DESCRIPTION: Mod_LoadEdges
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// Same as in QW.  You know the routine by now.
*/
void Mod_LoadEdges(lump_t *l) {

   dedge_t *in;
   medge_t *out;
   int i, count;

   in = (dedge_t *)(mod_base + l->fileofs);
   if(l->filelen % sizeof(dedge_t) != 0) {
      Sys_Error("%s: funny lump size in %s", __FUNCTION__, loadmodel->name);
   }
   count = l->filelen / sizeof(dedge_t);
   out = Hunk_AllocName((count + 1) * sizeof(medge_t), loadname);

   loadmodel->edges = out;
   loadmodel->numedges = count;

   for(i = 0; i < count; i++, in++, out++) {

      out->v[0] = (unsigned short)letohost16(in->v[0]);
      out->v[1] = (unsigned short)letohost16(in->v[1]);
   }
}

/* DESCRIPTION: Mod_LoadSurfedges
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// And now we're playing with longs.  I miss the structs, personally.
*/
void Mod_LoadSurfedges(lump_t *l) {

   int i, count;
   int *in, *out;

   in = (int *)(mod_base + l->fileofs);
   if(l->filelen % sizeof(int)) {
      Sys_Error("%s: funny lump size in %s", __FUNCTION__, loadmodel->name);
   }
   count = l->filelen / sizeof(int);
   out = Hunk_AllocName(count * sizeof(int), loadname);

   loadmodel->surfedges = out;
   loadmodel->numsurfedges = count;

   for(i = 0; i < count; i++) {
      out[i] = letohost32(in[i]);
   }
}

/* DESCRIPTION: Mod_LoadSubmodels
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadBrushModel
//
// The last one, and it's a weird one.  QW has a lot of unnecessary stuff
// dealing with Hexen.
*/
void Mod_LoadSubmodels(lump_t *l) {

   dq1model_t *in;
   dmodel_t   *out;
   int i, j, count;

   in = (dq1model_t *)(mod_base + l->fileofs);

   if(l->filelen % sizeof(dq1model_t)) {
      Sys_Error("%s: funny lump size in %s\n", __FUNCTION__, loadmodel->name);
      return;
   }

   count = l->filelen / sizeof(dq1model_t);
   out = Hunk_AllocName(count * sizeof(dmodel_t), loadname);

   loadmodel->submodels = out;
   loadmodel->numsubmodels = count;

   for(i = 0; i < count; i++, in++, out++) {

      for(j = 0; j < 3; j++) { // spread the mins / maxs by a pixel
         out->mins[j] = letohost32(in->mins[j]) - 1;
         out->maxs[j] = letohost32(in->maxs[j]) + 1;
         out->origin[j] = letohost32(in->origin[j]);
      }

      for(j = 0 ; j < MAX_MAP_HULLSDQ1; j++) {
         out->headnode[j] = letohost32(in->headnode[j]);
      }
      /* //looks interesting, but is not included.
         for ( ; j<MAX_MAP_HULLSM ; j++)
            out->headnode[j] = 0;
         for (j=0 ; j<3 ; j++)
            out->hullavailable[j] = true;
         for ( ; j<MAX_MAP_HULLSM ; j++)
            out->hullavailable[j] = false;
      */
      out->visleafs = letohost32(in->visleafs);
      out->firstface = letohost32(in->firstface);
      out->numfaces = letohost32(in->numfaces);
   }
}


/* DESCRIPTION: Mod_LoadBrushModel
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_LoadModel
//
// Goes through a looong list of functions loading entities.
// This function, at its core, seems to handle the header of the file.
*/
//broken
void Mod_LoadBrushModel(model_t *mod, void *buffer) {

   int i, j;
   char name[32];
   dheader_t *header;
   dmodel_t * submodel;


   loadmodel->modeltype = 0; //mod_brush

   header = (dheader_t *)buffer;
   i = letohost32(header->version);

   if(i != 0x1D && i != 0x1E) {
      Sys_Error("Mod_LoadBrushModel: %s has wrong version number (%X should be 0x1D or 0x1E)\n", mod->name, i);
      return;
   }

   mod_base = (byte *)buffer;

   //This makes the entire header the correct endian.  It's probably
   //bad practice, but it works.  I should make a function do it...
   for(i = 0; (unsigned)i < sizeof(dheader_t) / 4; i++) {
      ((int *)header)[i] = letohost32(((int *)header)[i]);
   }

   Mod_LoadVertexes(&header->lumps[LUMP_VERTEXES]);
   Mod_LoadEdges(&header->lumps[LUMP_EDGES]);
   Mod_LoadSurfedges(&header->lumps[LUMP_SURFEDGES]);
   Mod_LoadEntities(&header->lumps[LUMP_ENTITIES]);
   Mod_LoadTextures(&header->lumps[LUMP_TEXTURES]);
   Mod_LoadLighting(&header->lumps[LUMP_LIGHTING]);
   Mod_LoadPlanes(&header->lumps[LUMP_PLANES]);
   Mod_LoadTexinfo(&header->lumps[LUMP_TEXINFO]);
   Mod_LoadFaces(&header->lumps[LUMP_FACES]);
   Mod_LoadMarksurfaces(&header->lumps[LUMP_MARKSURFACES]);
   Mod_LoadVisibility(&header->lumps[LUMP_VISIBILITY]);
   Mod_LoadLeafs(&header->lumps[LUMP_LEAFS]);
   Mod_LoadNodes(&header->lumps[LUMP_NODES]);
   Mod_LoadClipnodes(&header->lumps[LUMP_CLIPNODES]);
   Mod_LoadSubmodels(&header->lumps[LUMP_MODELS]);

   Mod_MakeHull0();

   mod->numframes = 2;
   mod->flags = 0;

   for(i = 0; i < mod->numsubmodels; i++) {

      submodel = &(mod->submodels[i]);
      mod->hulls[0].firstclipnode = submodel->headnode[0];

      for(j = 1; j < MAX_MAP_HULLSDQ1; j++) {

         mod->hulls[j].firstclipnode = submodel->headnode[j];
         mod->hulls[j].lastclipnode = mod->numclipnodes-1;
      }

      mod->firstmodelsurface = submodel->firstface;
      mod->nummodelsurfaces = submodel->numfaces;

      mod->maxs[0] = submodel->maxs[0];
      mod->maxs[1] = submodel->maxs[1];
      mod->maxs[2] = submodel->maxs[2];
      mod->mins[0] = submodel->mins[0];
      mod->mins[1] = submodel->mins[1];
      mod->mins[2] = submodel->mins[2];

      mod->radius = RadiusFromBounds(mod->mins, mod->maxs);

      mod->numleafs = submodel->visleafs;

      if(i < mod->numsubmodels - 1) {

         snprintf(name, sizeof(name), "*%i", i+1);
         loadmodel = Mod_FindName(0, name);
         *loadmodel = *mod;
         Q_strncpy(loadmodel->name, name, sizeof(loadmodel->name) - 1);
         loadmodel->name[sizeof(loadmodel->name) - 1] = '\0';
         mod = loadmodel;
      }
   }
}

//aliases
/* DESCRIPTION: Mod_LoadAliasFrame
// LOCATION: sw_model.c
// PATH: Mod_LoadAlias(Group/Model)
//
// Plays around with data structures I don't understand in ways I can recreate.
*/
void * Mod_LoadAliasFrame(void * pin, int *pframeindex, int numv, dtrivertx_t *pbboxmin, dtrivertx_t *pbboxmax, aliashdr_t *pheader, char *name) {

   dtrivertx_t   *pframe, *pinframe;
   int i, j, k;
   daliasframe_t *pdaliasframe;


   pdaliasframe = (daliasframe_t *)pin;
   Q_strcpy(name, pdaliasframe->name);

   for (i=0 ; i<3 ; i++) {

      pbboxmin->v[i] = pdaliasframe->bboxmin.v[i];
      pbboxmax->v[i] = pdaliasframe->bboxmax.v[i];
   }

   pinframe = (dtrivertx_t *)(pdaliasframe + 1);
   pframe = Hunk_AllocName(numv * sizeof(dtrivertx_t), loadname);

   *pframeindex = (byte *)pframe - (byte *)pheader;

   for(j = 0; j < numv; j++) {

      pframe[j].lightnormalindex = pinframe[j].lightnormalindex;
      for(k = 0; k < 3; k++) {
         pframe[j].v[k] = pinframe[j].v[k];
      }
   }

   pinframe += numv;

   return((void *)pinframe);
}

/* DESCRIPTION: Mod_LoadAliasGroup
// LOCATION: sw_model.c
// PATH: Mod_LoadAliasModel
//
// Similar to the above function, but a layer above it.
*/
void * Mod_LoadAliasGroup(void * pin, int *pframeindex, int numv, dtrivertx_t *pbboxmin, dtrivertx_t *pbboxmax, aliashdr_t *pheader, char *name) {

   daliasgroup_t *pingroup;
   maliasgroup_t *paliasgroup;
   int i, numframes;
   daliasinterval_t *pin_intervals;
   float *poutintervals;
   void *ptemp;


   pingroup = (daliasgroup_t *)pin;
   numframes = letohost32(pingroup->numframes);

   paliasgroup = Hunk_AllocName(sizeof(maliasgroup_t) + (numframes - 1) * sizeof(paliasgroup->frames[0]), loadname);
   paliasgroup->numframes = numframes;

   for(i = 0; i < 3; i++) {
      pbboxmin->v[i] = pingroup->bboxmin.v[i];
      pbboxmax->v[i] = pingroup->bboxmax.v[i];
   }

   *pframeindex = (byte *)paliasgroup - (byte *)pheader;

   pin_intervals = (daliasinterval_t *)(pingroup + 1);
   poutintervals = Hunk_AllocName(numframes * sizeof(float), loadname);
   paliasgroup->intervals = (byte *)poutintervals - (byte *)pheader;

   for(i = 0; i < numframes; i++) {

      *poutintervals = letohost32(pin_intervals->interval);

      if (*poutintervals <= 0.0) {
         Sys_Error("%s: interval <= 0.", __FUNCTION__);
      }

      poutintervals++;
      pin_intervals++;
   }

   ptemp = (void *)pin_intervals;

   for(i = 0; i < numframes; i++) {

      ptemp = Mod_LoadAliasFrame(ptemp,
                                 &paliasgroup->frames[i].frame,
                                 numv,
                                 &paliasgroup->frames[i].bboxmin,
                                 &paliasgroup->frames[i].bboxmax,
                                 pheader, name);
   }

   return(ptemp);
}

/* DESCRIPTION: Mod_LoadAliasSkin
// LOCATION: sw_model.c
// PATH: Mod_LoadAlias(Model/SkinGroup)
//
// This function copies memory.  It seems to work differently depending on how
// a global var that I assume means 'bit depth' is set.  QW has a more advanced
// version.
*/
void * Mod_LoadAliasSkin(void * pin, int *pskinindex, int skinsize, aliashdr_t *pheader) {


   byte   *pskin, *pinskin;
//   unsigned short   *pusskin;
//   unsigned int   *p32skin;

   pskin = Hunk_AllocName(skinsize * r_pixbytes, loadname);
   pinskin = (byte *)pin;

   *pskinindex = (byte *)pskin - (byte *)pheader;

   if(r_pixbytes == 1) {

      Q_memcpy(pskin, pinskin, skinsize);
   }
   else if(r_pixbytes == 2) {

      //An assignment to the stack was made, but nothing else. HL clearly doen't support this mode.
      Sys_Error("%s: r_pixbytes is two, HL doesn't alear to properly support that...\n", __FUNCTION__);
   }
   else {
      Sys_Error("%s: driver set invalid r_pixbytes: %d\n", __FUNCTION__, r_pixbytes);
   }

   pinskin += skinsize;

   return((void *)pinskin);
}

/* DESCRIPTION: Mod_LoadAliasSkinGroup
// LOCATION: sw_model.c
// PATH: Mod_LoadAliasModel
//
// And yet more data processing.
*/
void * Mod_LoadAliasSkinGroup(void * pin, int *pskinindex, int skinsize, aliashdr_t *pheader) {

   daliasskingroup_t      *pinskingroup;
   maliasskingroup_t      *paliasskingroup;
   int                  i, numskins;
   daliasskininterval_t   *pinskinintervals;
   float                  *poutskinintervals;
   void                  *ptemp;


   pinskingroup = (daliasskingroup_t *)pin;
   numskins = letohost32(pinskingroup->numskins);

   paliasskingroup = Hunk_AllocName(sizeof(maliasskingroup_t) +
                                    (numskins - 1) * sizeof(paliasskingroup->skindescs[0]),
                                    loadname);

   paliasskingroup->numskins = numskins;

   *pskinindex = (byte *)paliasskingroup - (byte *)pheader;

   pinskinintervals = (daliasskininterval_t *)(pinskingroup + 1);
   poutskinintervals = Hunk_AllocName(numskins * sizeof(float), loadname);

   paliasskingroup->intervals = (byte *)poutskinintervals - (byte *)pheader;

   for(i = 0; i < numskins; i++) {

      *poutskinintervals = letohost32(pinskinintervals->interval);

      if(*poutskinintervals <= 0) {
         Sys_Error("%s: interval <= 0.", __FUNCTION__);
      }

      poutskinintervals++;
      pinskinintervals++;
   }

   ptemp = (void *)pinskinintervals;

   for(i = 0; i < numskins; i++) {
      ptemp = Mod_LoadAliasSkin(ptemp, &paliasskingroup->skindescs[i].skin, skinsize, pheader);
   }

   return(ptemp);
}

void Mod_LoadAliasModel(model_t *mod, void *buffer) {

   int               i;
   mmdl_t            *pmodel;
   dmdl_t            *pinmodel;
   mstvert_t         *pstverts;
   dstvert_t         *pinstverts;
   aliashdr_t        *pheader;
   mtriangle_t       *ptri;
   dtriangle_t       *pintriangles;
   int               version, numframes, numskins;
   int               size;
   daliasframetype_t *pframetype;
   char              *pframetype2;
   daliasskintype_t  *pskintype;
   maliasskindesc_t  *pskindesc;
   int               skinsize;
   unsigned int      start, end, total;
   TextureStruct1_t  *unknown_var;


   start = Hunk_LowMark();

   pinmodel = (dmdl_t *)buffer;

   version = letohost32(pinmodel->version);
   if(version != ALIAS_VERSION) {
      Sys_Error("%s: %s has wrong version number (%i should be %i)\n", __FUNCTION__, mod->name, version, ALIAS_VERSION);
   }

//
// allocate space for a working header, plus all the data except the frames,
// skin and group info
//

   size = sizeof(aliashdr_t) +
          (letohost32(pinmodel->numframes) - 1) * sizeof(pheader->frames[0]) +
          sizeof(mmdl_t) +
          letohost32(pinmodel->numverts) * 2 * sizeof(mstvert_t) +
          letohost32(pinmodel->numtris) * sizeof(mtriangle_t);

   pheader = (aliashdr_t *)Hunk_AllocName(size, loadname);
   pmodel = (mmdl_t *) (((byte *)&pheader[1]) +
         ((letohost32(pinmodel->numframes) - 1) * sizeof(pheader->frames[0])));

   mod->flags = letohost32(pinmodel->flags);

//
// endian-adjust and copy the data, starting with the alias model header
//
   pmodel->boundingradius = letohost32(pinmodel->boundingradius);
   pmodel->numskins = letohost32(pinmodel->numskins);
   pmodel->skinwidth = letohost32(pinmodel->skinwidth);
   pmodel->skinheight = letohost32(pinmodel->skinheight);

   if(pmodel->skinheight > MAX_LBM_HEIGHT) {

      Sys_Error("%s: model %s has a skin taller than %d\n", __FUNCTION__, mod->name, MAX_LBM_HEIGHT);
   }

   pmodel->numverts = letohost32(pinmodel->numverts);

   if(pmodel->numverts <= 0) {

      Sys_Error("%s: model %s has no vertices\n", __FUNCTION__, mod->name);
   }

   if(pmodel->numverts > MAXALIASVERTS) {
      Sys_Error("%s: model %s has too many vertices\n", __FUNCTION__, mod->name);
   }

   pmodel->numtris = letohost32(pinmodel->numtris);

   if(pmodel->numtris <= 0) {
      Sys_Error("%s: model %s has no triangles\n", __FUNCTION__, mod->name);
   }

   pmodel->numframes = letohost32(pinmodel->numframes);
   pmodel->size = letohost32(pinmodel->size) * ALIAS_BASE_SIZE_RATIO;
   mod->synctype = letohost32(pinmodel->synctype);
   mod->numframes = pmodel->numframes;

   for(i = 0; i < 3; i++) {
      pmodel->scale[i] = letohost32(pinmodel->scale[i]);
      pmodel->scale_origin[i] = letohost32(pinmodel->scale_origin[i]);
      pmodel->eyeposition[i] = letohost32(pinmodel->eyeposition[i]);
   }

   numskins = pmodel->numskins;
   numframes = pmodel->numframes;

   if(pmodel->skinwidth & 0x03) {
      Sys_Error("%s: \"%s\" skinwidth not multiple of 4\n", __FUNCTION__, loadmodel->name);
   }

   pheader->model = (byte *)pmodel - (byte *)pheader;

//
// load the skins
//
   skinsize = pmodel->skinheight * pmodel->skinwidth;

   if(numskins < 1) {
      Sys_Error("%s: %s, invalid # of skins: %d\n", __FUNCTION__, loadmodel->name, numskins);
   }

   pskintype = (daliasskintype_t *)&pinmodel[1];
   pskindesc = Hunk_AllocName(numskins * sizeof(maliasskindesc_t), loadname);

   pheader->skindesc = (byte *)pskindesc - (byte *)pheader;

   for(i = 0; i < numskins; i++) {

      aliasskintype_t skintype;

      skintype = letohost32(pskintype->type);
      pskindesc[i].type = skintype;

      if(skintype == ALIAS_SKIN_SINGLE) {
         pskintype = (daliasskintype_t *)
               Mod_LoadAliasSkin(pskintype + 1,
                              &pskindesc[i].skin,
                              skinsize, pheader);
      }
      else {
         pskintype = (daliasskintype_t *)
               Mod_LoadAliasSkinGroup(pskintype + 1,
                                 &pskindesc[i].skin,
                                 skinsize, pheader);
      }
   }

//
// set base s and t vertices
//
   pstverts = (mstvert_t *)&pmodel[1];
   pinstverts = (dstvert_t *)pskintype;

   pheader->stverts = (byte *)pstverts - (byte *)pheader;

   for(i = 0; i < pmodel->numverts; i++) {

      pstverts[i].onseam = letohost32(pinstverts[i].onseam);

      // put s and t in 16.16 format
      pstverts[i].s = letohost32(pinstverts[i].s) << 16;
      pstverts[i].t = letohost32(pinstverts[i].t) << 16;
   }

//
// set up the triangles
//
   ptri = (mtriangle_t *)&pstverts[pmodel->numverts];
   pintriangles = (dtriangle_t *)&pinstverts[pmodel->numverts];

   pheader->triangles = (byte *)ptri - (byte *)pheader;

   for(i = 0; i < pmodel->numtris; i++) {

      int j;

      ptri[i].facesfront = letohost32(pintriangles[i].facesfront);

      for(j = 0; j < 3; j++) {

         ptri[i].xyz_index[j] = letohost32(pintriangles[i].vertindex[j]);
      }
   }

//
// load the frames
//
   if(numframes < 1) {
      Sys_Error("%s: %s, invalid # of frames: %d\n", __FUNCTION__, mod->name, numframes);
   }

   pframetype = (daliasframetype_t *)&pintriangles[pmodel->numtris];

   for(i = 0; i < numframes; i++) {

      aliasframetype_t frametype;

      frametype = letohost32(pframetype->type);
      pheader->frames[i].type = frametype;

      if(frametype == ALIAS_SINGLE) {

         pframetype = (daliasframetype_t *)Mod_LoadAliasFrame(pframetype + 1,
                              &pheader->frames[i].frame,
                              pmodel->numverts,
                              &pheader->frames[i].bboxmin,
                              &pheader->frames[i].bboxmax,
                              pheader, pheader->frames[i].name);
      }
      else {
         pframetype = (daliasframetype_t *)Mod_LoadAliasGroup(pframetype + 1,
                              &pheader->frames[i].frame,
                              pmodel->numverts,
                              &pheader->frames[i].bboxmin,
                              &pheader->frames[i].bboxmax,
                              pheader, pheader->frames[i].name);
      }

   /*   if (pframetype == NULL)
      {
         Con_Printf (S_ERROR "SWMod_LoadAliasModel: %s, couldn't load frame data\n", mod->name);
         Hunk_FreeToLowMark(start);
         return false;
      } */
   }

   mod->modeltype = 2; //mod_alias

   mod->mins[0] = mod->mins[1] = mod->mins[2] = -16;
   mod->maxs[0] = mod->maxs[1] = mod->maxs[2] = 16;

   //This section veers wildly off QW.
   unknown_var = Hunk_AllocName(0x800, loadname);
   pframetype2 = (char *)pframetype;

   for(i = 0; i < 0x100; i++) {

      unknown_var[i].values[0] = pframetype2[3*i + 2];
      unknown_var[i].values[1] = pframetype2[3*i + 1];
      unknown_var[i].values[2] = pframetype2[3*i + 0];
      unknown_var[i].values[3] = 0;
   }

   pheader->something = (byte *)unknown_var - (byte *)pheader;

//
// move the complete, relocatable alias model to the cache
//
   end = Hunk_LowMark();
   total = end - start;

   Cache_Alloc(&mod->cache, total, loadname);
   if(mod->cache.data == NULL) {
      return;
   }

   Q_memcpy(mod->cache.data, pheader, total);
   Hunk_FreeToLowMark(start);
}


//sprites

/* DESCRIPTION: Mod_LoadSpriteFrame
// LOCATION: gl_model, sw_model.c
// PATH: Mod_LoadSprite(Model/Group)
//
// Copies data into memory.  Yay.  The QW version is more detailed,
// featuring downscaling and multiple color depths.
*/
void * Mod_LoadSpriteFrame(void * pin, mspriteframe_t **ppframe) {

   dspriteframe_t *pinframe;
   mspriteframe_t *pspriteframe;
   int width, height, size, origin[2];

   pinframe = (dspriteframe_t *)pin;

   width = letohost32(pinframe->width);
   height = letohost32(pinframe->height);
   size = width * height;

   pspriteframe = (mspriteframe_t *)Hunk_AllocName(sizeof(mspriteframe_t) + size * r_pixbytes, loadname);
   Q_memset(pspriteframe, 0, sizeof(mspriteframe_t) + size * r_pixbytes);

   *ppframe = pspriteframe;

   pspriteframe->width = width;
   pspriteframe->height = height;
   origin[0] = letohost32(pinframe->origin[0]);
   origin[1] = letohost32(pinframe->origin[1]);

   pspriteframe->up = origin[1];
   pspriteframe->down = origin[1] - height;
   pspriteframe->left = origin[0];
   pspriteframe->right = width + origin[0];

   if(r_pixbytes == 1)   {

      Q_memcpy(&pspriteframe->pixels[0], (byte *)(pinframe + 1), size);
   }
/*   else if(r_pixbytes == 2) {

      else
      {
         ppixin = (unsigned char *)(pinframe + 1);
         p16out = (unsigned short *)&pspriteframe->pixels[0];
         for (i=0 ; i<size ; i++)
         {
            if (ppixin[i] == 255)
               p16out[i] = 0xffff;   //transparent.
            else
               p16out[i] = d_8to16table[ppixin[i]];
         }
      }
   } */
   else {
      Sys_Error("%s: driver set invalid r_pixbytes: %d\n", __FUNCTION__, r_pixbytes);
   }

   return (void *)(((byte *)pinframe) + sizeof(dspriteframe_t) + size);
}

/* DESCRIPTION: Mod_LoadSpriteGroup
// LOCATION: gl_model, sw_model.c
// PATH: Mod_LoadSpriteModel
//
// loadin loadin loadin, no real difference from QWin,
*/
void * Mod_LoadSpriteGroup(void * pin, mspriteframe_t **ppframe) {

   dspritegroup_t      *pingroup;
   mspritegroup_t      *pspritegroup;
   int               i, numframes;
   dspriteinterval_t   *pin_intervals;
   float            *poutintervals;
   void            *ptemp;

   pingroup = (dspritegroup_t *)pin;
   numframes = letohost32(pingroup->numframes);

   pspritegroup = (mspritegroup_t *)Hunk_AllocName(sizeof(mspritegroup_t) + (numframes - 1) * sizeof(pspritegroup->frames[0]), loadname);
   pspritegroup->numframes = numframes;
   *ppframe = (mspriteframe_t *)pspritegroup;

   pin_intervals = (dspriteinterval_t *)(pingroup + 1);
   poutintervals = Hunk_AllocName(numframes * sizeof(float), loadname);
   pspritegroup->intervals = poutintervals;

   for(i = 0; i < numframes; i++) {

      *poutintervals = letohost32(pin_intervals->interval);
      if(*poutintervals <= 0.0) {
         Sys_Error("%s: interval <= 0.", __FUNCTION__);
      }

      poutintervals++;
      pin_intervals++;
   }

   ptemp = (void *)pin_intervals;

   for(i = 0; i < numframes; i++) {
      ptemp = Mod_LoadSpriteFrame(ptemp, &pspritegroup->frames[i]);
   }

   return(ptemp);
}

/* DESCRIPTION: Mod_LoadSpriteModel
// LOCATION: gl_model, sw_model.c
// PATH: Mod_LoadModel
//
// This starts off easily enough, although there were both a few struct
// mismatches and one of those annoying non-QW block I think deal with color.
// The end result is pretty rickety.  Deal with it.
*/
void Mod_LoadSpriteModel(model_t *mod, void *buffer) {

   int               i;
   int               version;
   dsprite_t         *pin;
   msprite_t         *psprite;
   int               numframes;
   int               size1, size2;
   dspriteframetype_t   *pframetype;

   int var_24;
   byte * var_2c; //Each 'struct' here is three bytes, which would have to be packed otherwise.
   TextureStruct1_t * var_30;


   pin = (dsprite_t *)buffer;

   version = letohost32(pin->version);

   if(version != SPRITEHL_VERSION) {
      Sys_Error("%s: %s has wrong version number (%i should be %i)\n", __FUNCTION__, mod->name, version, SPRITEHL_VERSION);
   }

   numframes = letohost32(pin->numframes);
   size1 = ((short *)(pin+1))[0] * sizeof(psprite->frames)+ 2; //maybe?  It's that short * an eight byte struct plus two...
   size2 = sizeof(msprite_t) + (numframes - 1) * sizeof (psprite->frames);

   psprite = (msprite_t *)Hunk_AllocName(size1 + size2, loadname);
   mod->cache.data = (byte *)psprite;

   psprite->type = letohost32(pin->type);
   psprite->padding02 = letohost32(pin->padding0c);
   psprite->maxwidth = letohost32(pin->width);
   psprite->maxheight = letohost32(pin->height);
   psprite->beamlength = letohost32(pin->beamlength);
   mod->synctype = letohost32(pin->synctype);
   psprite->numframes = numframes;

   mod->mins[0] = mod->mins[1] = -psprite->maxwidth/2;
   mod->maxs[0] = mod->maxs[1] = psprite->maxwidth/2;
   mod->mins[2] = -psprite->maxheight/2;
   mod->maxs[2] = psprite->maxheight/2;

   psprite->padding10 = size2 + 2;
   var_2c = (((byte *)buffer) + sizeof(dsprite_t));
   var_30 = (TextureStruct1_t *)(((byte *)psprite) + size2);

   var_24 = *(short *)var_30 = *(short *)var_2c; //I'll tidy it up some other time.

   var_2c = ((byte *)var_2c) + 2;
   var_30 = (TextureStruct1_t *)(((byte *)var_30) + 2);

   for(i = 0; i < var_24; i++) {

      var_30[i].values[0] = var_2c[3*i + 0];
      var_30[i].values[1] = var_2c[3*i + 1];
      var_30[i].values[2] = var_2c[3*i + 2];
      var_30[i].values[3] = 0;
   }

//
// load the frames
//
   if(numframes < 1) {
      Sys_Error("%s: Invalid # of frames: %d\n", __FUNCTION__, numframes);
   }

   mod->flags = 0;
   mod->numframes = numframes;
   pframetype = (dspriteframetype_t *)(((byte *)buffer) + (3 * var_24) + sizeof(dsprite_t) + 2);

   for(i = 0; i < numframes; i++) {

      spriteframetype_t   frametype;

      frametype = hosttole32(pframetype->type);
      psprite->frames[i].type = frametype;

      if(frametype == SPR_SINGLE) {
         pframetype = (dspriteframetype_t *)Mod_LoadSpriteFrame(pframetype + 1, &psprite->frames[i].frameptr);
      }
      else {
         pframetype = (dspriteframetype_t *)Mod_LoadSpriteGroup(pframetype + 1, &psprite->frames[i].frameptr);
      }
   }

   mod->modeltype = 1; //mod_sprite
}


//studio (This is unfortunately not a QW function)
void Mod_LoadStudioModel(model_t *mod, void *buffer) {

   byte *pin, *pcache, *var_10_in, *var_14_out;
   studiohdr_t *pstudiohdr;
   mstudiotexture_t *pcache2;
   int version;
   int size, size2;
   int i, j;


   pin = (byte *)buffer;
   pstudiohdr = (studiohdr_t *)pin;
   version = letohost32(pstudiohdr->version);

   if(version != STUDIO_VERSION) {

      Q_memset(pstudiohdr, 0, sizeof(studiohdr_t));
      Q_strcpy(pstudiohdr->name, "bogus");
      pstudiohdr->texturedataindex = pstudiohdr->length = 0xF4;
   }

   mod->modeltype = 3;
   mod->flags = pstudiohdr->flags;

   size = (pstudiohdr->numtextures * /*sizeof( )*/ 0x500) + pstudiohdr->length;

   Cache_Alloc(&mod->cache, size, mod->name);
   if(mod->cache.data == NULL) { return; }


   pcache = (byte *)mod->cache.data;
   pcache2 = (mstudiotexture_t *)(pcache + pstudiohdr->textureindex);

   if(pstudiohdr->textureindex == 0) {

      Q_memcpy(pcache, pin, pstudiohdr->length);
      return;
   }

   Q_memcpy(pcache, pin, pstudiohdr->texturedataindex); //this looks wrong, but kinda makes sense.

   var_10_in = pin + pstudiohdr->texturedataindex;
   var_14_out = pcache + pstudiohdr->texturedataindex;

   for(i = 0; i < pstudiohdr->numtextures; i++) {

      pcache2[i].index = (byte *)var_14_out - (byte *)pcache;
      size2 = pcache2[i].width * pcache2[i].height;
      Q_memcpy(var_14_out, var_10_in, size2);

      var_10_in  = ((byte *)var_10_in)  + size2;
      var_14_out = ((byte *)var_14_out) + size2;

      for(j = 0; j < 0x100; j++) { //There are several 'max' values this could be.

         //This copying could be handled better, but it IS pretty compact.
         *(short int *)var_14_out = texgammatable[*var_10_in & 0xFF];
         var_14_out += 2;
         var_10_in++;
         *(short int *)var_14_out = texgammatable[*var_10_in & 0xFF];
         var_14_out += 2;
         var_10_in++;
         *(short int *)var_14_out = texgammatable[*var_10_in & 0xFF];
         var_14_out += 2;
         var_10_in++;
         *(short int *)var_14_out = 0;
         var_14_out += 2;
      }
   }
}



//other
void Mod_Init() {

   //SW_Mod_Init(); //does nothing
   Cvar_RegisterVariable(&cvar_r_wadtextures);
   Q_memset(mod_novis, 0xFF, sizeof(mod_novis));
}
void Mod_Shutdown() {

   if(wadpath != NULL) {
      Q_Free(wadpath);
      wadpath = NULL;
   }
}
void CM_FreePAS() {

   if(gPAS != NULL) {
      Q_Free(gPAS);
      gPAS = NULL;
   }
   if(gPVS != NULL) {
      Q_Free(gPVS);
      gPVS = NULL;
   }
}

void CM_DecompressPVS(byte * in, byte * out, int row) {

   int c;
   byte * pout;

   if(in == NULL) {
      Q_memcpy(out, mod_novis, row);
      return;
   }

   pout = out;
   out += row;

   do {

      if(in[0] != '\0') { //If you accidentally make this an == (resulting in the decompressed data being all 0s), the edicts dissapear.
         *pout = *in;
         pout++;
         in++;
         continue;
      }

      c = in[1];
      in += 2;
      while(c != '\0') { //Very basic compression.  Zeros are packed, all others aren't.
         *pout = '\0';
         pout++;
         c--;
      }

   } while(pout < out);
}
byte * Mod_DecompressVis(byte * in, model_t * model) {

   static byte decompressed_6[0x400];
   int row;

   row = (model->numleafs + 7) >> 3;

   if(in == NULL) { return(mod_novis); }

   CM_DecompressPVS(in, decompressed_6, row);
   return(decompressed_6);
}

byte * CM_LeafPAS(int leafindex) {

   if(gPAS == NULL) { return(mod_novis); }
   return(&gPAS[gPVSRowBytes * leafindex]);
}
byte * CM_LeafPVS(int leafindex) {

   if(gPVS == NULL) { return(mod_novis); }
   return(&gPVS[gPVSRowBytes * leafindex]);
}
byte * Mod_LeafPVS(mleaf_t *leaf, model_t *model) {

   if(leaf == model->leafs) { return(mod_novis); }

   if(gPVS == NULL) {
      return(Mod_DecompressVis(leaf->compressed_vis, model));
   }
   else {
      return(CM_LeafPVS(leaf - model->leafs));
   }
}

//gl_q2bsp.c
int CM_HeadnodeVisible(mnode_t *node, byte * visbits, int * arg_8) {

   int leafnum;


   if(node == NULL || node->contents == -2) { return(0); }

   if(node->contents < 0) { //is actually a leaf--what we wanted

      leafnum = ((mleaf_t *)node - (global_sv.worldmodel->leafs)) - 1;

      if((visbits[leafnum >> 3] & (1 << (leafnum & 7))) == 0) {
           return(0);
      }

      if(arg_8 == NULL) {
         *arg_8 = leafnum;
      }

      return(1);
   }

   if(CM_HeadnodeVisible(node->children[0], visbits, arg_8)) {
      return(1);
   }
   return(CM_HeadnodeVisible(node->children[1], visbits, arg_8));
}



//A bit more unique this time around, there are no structs,
//and we shift our object size regularly.
//Function is producing incorrect results, unable to confirm function is the cause though.
void CM_CalcPAS(model_t * model) { //worldmodel

   int i, j, k, l;
   int leafnum, rows, PVSRowNum, var_1C, count, count2;
   byte * PVSptr, * PASptr, * var_2C;
   unsigned int bits;


   Con_Printf("Building PAS...\n");
   CM_FreePAS();

   leafnum = model->numleafs + 1;
   rows = (model->numleafs + 7) / 8;
   gPVSRowBytes = (rows + 3) & 0xFFFFFFFC; //border alignment.
   PVSRowNum = gPVSRowBytes >> 2;

   gPVS = (byte *)Q_Calloc(gPVSRowBytes, leafnum);
   PVSptr = gPVS;
   count = 0;


   for(i = 0; i < leafnum; i++, PVSptr += gPVSRowBytes) {

      CM_DecompressPVS(model->leafs[i].compressed_vis, PVSptr, rows);

      if(i == 0) { continue; }
      for(j = 0; j < leafnum; j++) {

         if(((int)(PVSptr[j / 8]) & (1 << (j & 7))) != 0) { count++; }
      }
   }

   gPAS = (byte *)Q_Calloc(gPVSRowBytes, leafnum);
   count2 = 0;
   PVSptr = gPVS;
   PASptr = gPAS;


   for(i = 0; i < leafnum; i++, PVSptr += gPVSRowBytes, PASptr += 4*PVSRowNum) {

      Q_memcpy(PASptr, PVSptr, gPVSRowBytes);
      for(j = 0; j < gPVSRowBytes; j++) {

         bits = PVSptr[j];
         if(bits == 0) { continue; }

         for(k = 0; k < 8; k++) {

            if((bits & (1 << k)) == 0) { continue; }

            var_1C = (j << 3) + k + 1;
            if(var_1C >= leafnum || var_1C <= 0) { continue; }
            var_2C = gPVS + (var_1C * gPVSRowBytes);

            for(l = 0; l < PVSRowNum; l++) {
               ((uint32 *)PASptr)[l] |= ((uint32 *)var_2C)[l];
            }
         }
      }

      if(i == 0) { continue; }

      for(j = 0; j < leafnum; j++) {
         if(((int)(PASptr[j / 8]) & (1 << (j & 7))) != 0) { count2++; }
      }
   }

   Con_Printf("%s: Average leaves visible/audible/total %i %i %i\n", __FUNCTION__, count/leafnum, count2/leafnum, leafnum);
}

//Q1BSP_LeafnumForPoint
mleaf_t * Mod_PointInLeaf(vec3_t p, model_t *model) {

   mnode_t *node;
   float d;
   mplane_t *plane;


   if(model == NULL || model->nodes == NULL) {
      Sys_Error("Mod_PointInLeaf: bad model");
   }

   node = model->nodes;

   while(1) {

      if(node->contents < 0) { //That means it's a leaf and not a node.
         return((mleaf_t *)node);
      }

      plane = node->plane;

      if(plane->type > 2) {

         d = DotProduct(p, plane->normal) - plane->dist;
      }
      else {

         d = p[plane->type] - plane->dist;
      }

      if(d > 0) { node = node->children[0]; }
      else {      node = node->children[1]; }

      //We could add a paranoia safety here...
   }

   return(0); // never reached, shouldn't be possible...
}

void Mod_ChangeGame() { //No public address, not hijackable, probably not USED either.

   int i;
   model_t *temp;


   temp = mod_known;
   for(i = 0; i < mod_numknown; i++, temp++) {

      if(temp->modeltype == 3 && Cache_Check(&temp->cache) != 0) {
         Cache_Free(&temp->cache);
      }
      mod_known_info[i].padding08 = 0;
      mod_known_info[i].padding04 = 0;
   }
}

//global_svmodel
HLDS_DLLEXPORT void * Mod_Extradata(model_t *mod) {

   void *r;

   if(mod == NULL) { return(NULL); }

   r = Cache_Check(&mod->cache);
   if(r != NULL) {

      if(mod->modeltype == 0) { //mod_brush

         Sys_Error("%s: called with mod_brush.", __FUNCTION__);
      }
      return(r);
   }

   Mod_LoadModel(mod, 1, 0);

   if(mod->cache.data == NULL) {
      Sys_Error("%s: caching failed", __FUNCTION__);
   }
   return(mod->cache.data);
}



int R_StudioBodyVariations(model_t *mod) {

   int i;
   int count;
   studiohdr_t * var_4; //bit of a guess
   mstudiobodyparts_t * var_8;

   if(mod->modeltype != 3) { return(0); }

   var_4 = (studiohdr_t *)Mod_Extradata(mod);
   if(var_4 == NULL) { return(0); }

   count = 1;
   var_8 = (mstudiobodyparts_t *)(((byte *)var_4) + var_4->bodypartindex);

   for(i = 0; i < var_4->numbodyparts; i++) {
      count *= var_8[i].nummodels;
   }
   return(count);
}
int ModelFrameCount(model_t *mod) {

   int count;

   count = 1;
   if(mod != NULL) {
      if(mod->modeltype == 1)   {
         count = ((dsprite_t *)(mod->cache.data))->padding0c;
      }
      else if(mod->modeltype == 3) {
         count = R_StudioBodyVariations(mod);
      }

      if(count < 1) { count = 1; }
   }

   return(count);
}

//SDK called int (*pfnModelFrames) (int modelIndex);
HLDS_DLLEXPORT int ModelFrames(int modelIndex) {

   model_t * var_4;

   if(modelIndex <= 0) {

      Con_Printf("%s: Called with arg0 <= 0.\n", __FUNCTION__);
      return(1);
   }

   var_4 = global_sv.models[modelIndex];
   return(ModelFrameCount(var_4));
}


/* DESCRIPTION: Mod_LoadModel
// LOCATION: global_svmode/sw_model.c
// PATH: Mod_ForName, Mod_ExtraData
//
// Loads a model.  As in, actually OPENS the file and tosses the data to
// whatever parsing file is best suited.
*/
model_t * Mod_LoadModel(model_t *mod, int crash, int bool2) {

   int length;
   byte * buf;

   CRC32_t var_118;
   crc_info_t * CRC;


   if(mod->modeltype == 0x02 || mod->modeltype == 0x03) {
      if(Cache_Check(&mod->cache) != 0) { mod->loadstate = 0x00; return(mod); }
   }
   else if(mod->loadstate == 0x00 || mod->loadstate == 0x03) { return(mod); }


   // load the file
   //-steam check omitted, boo-urns steam.

   buf = COM_LoadFileForMe(mod->name, &length);

   /* The file has seek pointers, and the header is a decent size by default.
   // We SHOULD do some validation before dereferencing things left and right,
   // and this is a good place to do so.
   */
   if(buf == NULL || length < 4) {
      if(crash) { Sys_Error("%s: %s not found.", __FUNCTION__, mod->name); }
      return NULL;
   }

   // allocate a new model

   if(bool2 != 0) {

      //I don't like doing pointer arithmetic with structs, so if you don't get it, this *should* find the index of mod_known that mod corresponds to.
      CRC = &(mod_known_info[mod - mod_known]);
      if(CRC->padding00 != 0) {

         CRC32_Init(&var_118);
         CRC32_ProcessBuffer(&var_118, buf, length);
         var_118 = CRC32_Final(var_118);

         if(CRC->padding04 == 0) {
            CRC->padding04 = 1;
            CRC->padding08 = var_118;
         }
         else if(CRC->padding08 != var_118) {
            Sys_Error("%s: The model \"%s\" has been modified since the server was started.\nI'm going to BAIL now and let you handle it, 'cuz whatever's up, it's not good.", __FUNCTION__, mod->name);
         }
      }
   }

   if(cvar_developer.value != 0) {

      Con_Printf("%s: Loading %s...\n", __FUNCTION__, mod->name);
   }

   //This is currently safe, as mod->name CAN'T be more than 64 bytes.
   COM_FileBase(mod->name, loadname);
   loadmodel = mod;

   // fill it in
   mod->loadstate = 0;

   switch(letohost32(*(uint32 *)buf)) { //This is hacky...

   case 0x50534449: //PSDI (Damn, why don't we just do a string comparison?)
      Mod_LoadSpriteModel(mod, buf);
      break;

   case 0x54534449: //TSDI
      Mod_LoadStudioModel(mod, buf);
      break;

   case 0x4F504449: //OPDI
      Mod_LoadAliasModel(mod, buf);
      break;

   default:
      Mod_LoadBrushModel(mod, buf);
      break;
   }

   if(g_modfuncs.Function10 != NULL) {
      g_modfuncs.Function10(mod, buf);
   }

   if(buf != NULL) { //How could it?  We return if the file loading failed...
      COM_FreeFile(buf);
   }
   return(mod);
}

/*
Unlisted:
Mod_ValidateCRC
Mod_UnloadSpriteTextures
Mod_NeedCRC
Mod_MarkClient

Mod_ChangeGame

listed but unused:
Mod_Print

*/
