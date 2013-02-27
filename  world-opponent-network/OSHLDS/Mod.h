/* Relates to MODELS, not MODIFICATIONS.  QW has extensive examples here.
*/
#ifndef OSHLDS_MODEL_HEADER
#define OSHLDS_MODEL_HEADER

#include "endianin.h"
#include "HUNK.h"
#include "ModCalls.h"

// d* structures refer to how something is stored in a file
// m* structs refer to how it is stored in memory.
// Some of them may be backwards, and a lot of these may be unused.

//Several of these structs have cross dependencies, which must be resolved
//by NOT using the typedef'd name.

//Many of these structs can and probably should go into the code instead of the header.

#define   MAX_MAP_HULLSDQ1   4
#define   MAX_MAP_HULLSDH2   8
#define   MAX_MAP_HULLSM     16

#define   MAX_MAP_MODELS     256
#define   MAX_MAP_BRUSHES    0x8000
#define   MAX_MAP_ENTITIES   1024
#define   MAX_MAP_ENTSTRING  65536

#define   MAX_MAP_PLANES       65636*2
#define   MAX_MAP_NODES        65535
#define   MAX_MAP_CLIPNODES    65535
#define   MAX_MAP_LEAFS        65535
#define   MAX_MAP_VERTS        65535
#define   MAX_MAP_FACES        65535
#define   MAX_MAP_MARKSURFACES 65535
#define   MAX_MAP_TEXINFO      4096
#define   MAX_MAP_EDGES        256000
#define   MAX_MAP_SURFEDGES    512000
#define   MAX_MAP_MIPTEX       0x200000
#define   MAX_MAP_LIGHTING     0x100000
#define   MAX_MAP_VISIBILITY   0x200000

// key / value pair sizes

#define   MAX_KEY    32
#define   MAX_VALUE  1024


#define   LUMP_ENTITIES     0
#define   LUMP_PLANES       1
#define   LUMP_TEXTURES     2
#define   LUMP_VERTEXES     3
#define   LUMP_VISIBILITY   4
#define   LUMP_NODES        5
#define   LUMP_TEXINFO      6
#define   LUMP_FACES        7
#define   LUMP_LIGHTING     8
#define   LUMP_CLIPNODES    9
#define   LUMP_LEAFS        10
#define   LUMP_MARKSURFACES 11
#define   LUMP_EDGES        12
#define   LUMP_SURFEDGES    13
#define   LUMP_MODELS       14

#define   HEADER_LUMPS      15

typedef enum { ST_SYNC=0, ST_RAND } synctype_t;
typedef enum { ALIAS_SINGLE=0, ALIAS_GROUP, ALIAS_GROUP_SWAPPED = 16777216 } aliasframetype_t;
typedef enum { ALIAS_SKIN_SINGLE=0, ALIAS_SKIN_GROUP } aliasskintype_t;
typedef enum { SPR_SINGLE=0, SPR_GROUP, SPR_ANGLED } spriteframetype_t;


/*** STRUCTS ***/

typedef struct lump_s {
   int fileofs, filelen;
} lump_t;
typedef struct dheader_s { //no counterpart
   int version;
   lump_t lumps[HEADER_LUMPS];
} dheader_t;
typedef struct dmiptexlump_s { //0x14 //no counterpart

   int         nummiptex;
   int         dataofs[4];      // [nummiptex]
} dmiptexlump_t;

typedef struct dnode_s {

   int   planenum;
   short children[2];   // negative numbers are -(leafs+1), not nodes
   short mins[3];      // for sphere culling
   short maxs[3];
   unsigned short firstface;
   unsigned short numfaces;   // counting both sides
} dnode_t;
typedef struct mnode_s {

// common with leaf
   int contents;      // 0, to differentiate from leafs
   int visframe;      // node needs to be traversed if current

   short minmaxs[6];      // for bounding box culling

   struct mnode_s *parent;

// node specific
   struct mplane_s *plane;
   struct mnode_s *children[2]; //1c

   unsigned short firstsurface; //24
   unsigned short numsurfaces;
} mnode_t;

#define   NUM_AMBIENTS 4
typedef struct dleaf_s {

   int   contents;
   int   visofs;            // -1 = no visibility info

   short mins[3];         // for frustum culling
   short maxs[3];

   unsigned short firstmarksurface;
   unsigned short nummarksurfaces;

   byte ambient_level[NUM_AMBIENTS];
} dleaf_t;
typedef struct mleaf_s { //0x30

// common with node
   int    contents;      // wil be a negative contents number
   int    visframe;      // node needs to be traversed if current

   short  minmaxs[6];    // for bounding box culling

   struct mnode_s *parent;

// leaf specific
   byte   *compressed_vis; //18
   struct efrag_s *efrags;

   struct msurface_s **firstmarksurface;
   int    nummarksurfaces;
   int    key;             // BSP sequence number for leaf's contents
   byte   ambient_sound_level[4];
} mleaf_t;

typedef struct plane_s { //0x10 pmplane

   vec3_t normal;
   float   dist;
} plane_t;
typedef struct cplane_s {//0x14

   vec3_t normal;
   float   dist;
   int type;
} cplane_t;
typedef struct mplane_s { //sizeof 0x14

   vec3_t   normal; //00
   float   dist; //0c, sounds right.
   byte   type;   //10      // for texture axis selection and fast side tests
   byte   signbits;      // signx + signy<<1 + signz<<1
   byte   pad[2];
} mplane_t;

typedef struct dclipnode_s { //sixeof 0x08

   int      planenum;
   short      children[2];   // negative numbers are contents
} dclipnode_t;
typedef struct hull_s { //sizeof 0x28

   dclipnode_t * clipnodes;
   mplane_t    * planes;
   int firstclipnode;
   int lastclipnode;
   vec3_t clip_mins;
   vec3_t clip_maxs;
} hull_t;

typedef struct dvertex_s {
   float point[3];
} dvertex_t;
typedef struct mvertex_s { //same thing, different 'naming'.
   vec3_t position;
} mvertex_t;

typedef struct dedge_s {
   unsigned short v[2];      // vertex numbers
} dedge_t;
typedef struct medge_s {
   unsigned short v[2];
   unsigned int   cachededgeoffset;
} medge_t;

typedef struct color24_s {
   byte r, g, b;
} color24_t;
typedef struct colorVec_s {
   unsigned long int r, g, b, a;
} colorVec_t;
typedef struct PackedColorVec_s {
   unsigned short int r, g, b, a;
} PackedColorVec_t;

typedef struct texture_s {

   char        name[16]; //00
   unsigned    width, height; //10
   int         anim_total;   //18         // total tenths in sequence ( 0 = no)
   int         anim_min, anim_max; //1C      // time for this frame min <=time< max
   struct texture_s * anim_next; //24      // in the animation sequence
   struct texture_s * alternate_anims; //28   // bmodels in frame 1 use these
   unsigned    offsets[4]; //2c      // four mip maps stored
   unsigned    paloffset; //3c
} texture_t;

typedef struct dtexinfo_s {
   float vecs[2][4];     // [s/t][xyz offset]
   int   miptex;
   int   flags;
} dtexinfo_t;
typedef struct mtexinfo_s {

   float     vecs[2][4]; // [s/t] unit vectors in world space.
                         // [i][3] is the s/t offset relative to the origin.
                         // s or t = dot(3Dpoint,vecs[i])+vecs[i][3]
   float     mipadjust;  // ?? mipmap limits for very small surfaces
   texture_t * texture;
   int       flags;      // sky or slime, no lightmap or 256 subdivision
} mtexinfo_t;

typedef struct msurface_s {

   int visframe;      // should be drawn when node is crossed
   int dlightframe;   // last frame the surface was checked by an animated light
   int dlightbits;      // dynamically generated. Indicates if the surface illumination
                        // is modified by an animated light.

   mplane_t * plane;         // pointer to shared plane
   int flags;         // see SURF_ #defines

   int firstedge;   // look up in model->surfedges[], negative numbers
   int numedges;   // are backwards edges

// surface generation data
   struct surfcache_s * cachespots[4]; //1c This struct and whatnot is never used in the code.

   short texturemins[2]; //2c // smallest s/t position on the surface.
   short extents[2]; //30     // ?? s/t texture size, 1..256 for all non-sky surfaces

   mtexinfo_t * texinfo; //34

// lighting info
   byte      styles[4]; // index into d_lightstylevalue[] for animated lights
                        // no one surface can be effected by more than 4
                        // animated lights.
   color24_t * samples;

   struct decal_s * pdecals;
} msurface_t;

typedef struct dmodel_s { //0x40

   float mins[3], maxs[3];
   float origin[3]; //18
   int   headnode[4]; //24
   int   visleafs; //34      // not including the solid leaf 0
   int   firstface, numfaces;
} dmodel_t;
typedef struct model_s {

   char name[64]; //00
   int loadstate; //40, was bool, is now thought to be enumerated

   int modeltype; //44, the enum modtype_t?  That enum will no doubt have changed.
//   int fromgame;  fromgame_t
   int numframes; //48
   synctype_t synctype; //dunno what.

   int flags; //50
//   int engineflags;
//   int particleeffect;
//   int particletrail;
//   int traildefaultindex; //64

// models:
   vec3_t mins;//54
   vec3_t maxs;//60
   float radius; //6c

// solid:
//   qboolean   clipbox;
//   vec3_t clipmins; //88
//   vec3_t clipmaxs; //94

// brush:
   int firstmodelsurface; //70
   int nummodelsurfaces;

   int numsubmodels;
   dmodel_t *submodels; //7c, listed as mmodel_t in QW

   int numplanes; //80
   mplane_t *planes;

   int numleafs;
   mleaf_t *leafs;

   int numvertexes;
   mvertex_t *vertexes;

   int numedges;
   medge_t *edges;

   int numnodes; //a0
   mnode_t *nodes;

   int numtexinfo;
   mtexinfo_t *texinfo;

   int numsurfaces;
   msurface_t *surfaces;

   int numsurfedges;
   int *surfedges;

   int numclipnodes; //c0
   dclipnode_t *clipnodes;

   int nummarksurfaces;
   msurface_t **marksurfaces;

   hull_t hulls[4]; //d0, [16] in QW

   int numtextures; //170
   texture_t **textures;

   byte * visdata;
//   void *vis;
   byte * lightdata;
//   byte *deluxdata;
//   struct q3lightgridinfo_t *lightgrid;
   char * entities; //180

//   void *terrain;

//   unsigned int checksum;
//   unsigned   int checksum2;

//   bspfuncs_t funcs;

   cache_user_t cache;      // only access through Mod_Extradata
} model_t;

#define   MIPLEVELS   4
typedef struct miptex_s { //0x28

   char      name[16]; //00
   unsigned   width, height; //10
   unsigned   offsets[MIPLEVELS]; //18      // four mip maps stored
} miptex_t;


#define   MAX_MAP_HULLSDQ1   4
typedef struct dq1model_s { //40 structure confirmed.

   float      mins[3], maxs[3];
   float      origin[3]; //18
   int         headnode[MAX_MAP_HULLSDQ1]; //24
   int         visleafs; //34      // not including the solid leaf 0
   int         firstface, numfaces; //38
} dq1model_t;

typedef struct TextureStruct1_s {

   short values[4]; //00, I'm thinking it might be an RGBA color struct.
} TextureStruct1_t;
//This is a THREE BYTE struct.  It is used by files--HL later pads it into the struct above.
//I do not use it because I don't like packing.  I instead treat the data like an array.
typedef struct TextureStruct1alt_s {

   char values[3]; //00
} TextureStruct1alt_t;
typedef struct TextureStruct2_s {

   short NumStructs;
   TextureStruct1_t structs[];
} TextureStruct2_t;

#define   MAXLIGHTMAPS   4
typedef struct dface_s {
   short      planenum;
   short      side;

   int        firstedge;      // we must support > 64k edges
   short      numedges;
   short      texinfo;

// lighting info
   byte       styles[MAXLIGHTMAPS];
   int        lightofs;      // start of [numstyles*surfsize] samples
} dface_t;

#define   MAXALIASVERTS   2000
#define   ALIAS_VERSION   6
#define   ALIAS_BASE_SIZE_RATIO   (1.0 / 11.0)
typedef struct dmdl_s { //sizeof(54h)
   int         ident; //00
   int         version; //04 *
   vec3_t      scale; //08
   vec3_t      scale_origin; //14
   float       boundingradius; //20
   vec3_t      eyeposition; //24
   int         numskins; //30
   int         skinwidth; //34
   int         skinheight; //38
   int         numverts; //3c *
   int         numtris; //40 *
   int         numframes; //44 *
   synctype_t  synctype; //48
   int         flags; //4c
   float      size; //50
} dmdl_t;
typedef struct mmdl_s {
   int         ident;
   int         version;
   vec3_t      scale;
   vec3_t      scale_origin;
   float       boundingradius;
   vec3_t      eyeposition;
   int         numskins;
   int         skinwidth;
   int         skinheight;
   int         numverts;
   //int         numstverts; //Doesn't appear to be in HL.
   int         numtris;
   int         numframes;
   synctype_t  synctype;
   int         flags;
   float       size;
} mmdl_t;


#define   MAX_LBM_HEIGHT 480
#define   MAX_SKINS      32
typedef struct mtriangle_s { //This is NOT what QW uses.
   int facesfront;
   int xyz_index[3];
   //int st_index[3];

   //int pad[2];
} mtriangle_t;
typedef struct dtriangle_s {
   int facesfront;
   int vertindex[3];
} dtriangle_t;

typedef struct dstvert_s {
   int onseam;
   int s;
   int t;
} dstvert_t;
typedef struct mstvert_s { //same.
   int onseam;
   int s;
   int t;
} mstvert_t;

typedef struct dtrivertx_s {
   byte v[3];
   byte lightnormalindex;
} dtrivertx_t;

typedef struct dmd2stvert_s {
   short s;
   short t;
} dmd2stvert_t;

typedef struct dmd2triangle_s {
   short               xyz_index[3];
   short               st_index[3];
} dmd2triangle_t;


#define DT_FACES_FRONT 0x0010

typedef struct daliasframe_s { //0x18
   dtrivertx_t bboxmin;   // lightnormal isn't used
   dtrivertx_t bboxmax;   // lightnormal isn't used
   char        name[16];  // frame name from grabbing
} daliasframe_t;

typedef struct daliasinterval_s {
   float interval;
} daliasinterval_t;

typedef struct daliasskininterval_s {
   float interval;
} daliasskininterval_t;

typedef struct daliasframetype_s {
   aliasframetype_t type;
} daliasframetype_t;

typedef struct daliasskintype_s {
   aliasskintype_t type;
} daliasskintype_t;

typedef struct maliasskindesc_s {
   aliasskintype_t      type;
   void            *pcachespot;
   int               skin;
} maliasskindesc_t;

typedef struct maliasframedesc_s {


   aliasframetype_t type;

   int              firstpose;
   int              numposes;
   float            interval;
   dtrivertx_t      bboxmin;
   dtrivertx_t      bboxmax;

   vec3_t           scale;
   vec3_t           scale_origin;

   int              frame;
   char             name[16];
} maliasframedesc_t;

typedef struct aliashdr_s {

   int model;
   int stverts;
   int skindesc;
     int triangles;
     int something; //colors?  Something to do with textures?
     maliasframedesc_t frames[1];
/* //This appears to be very wrong.
   int         ident;
   int         version;
   vec3_t      scale;
   vec3_t      scale_origin;
   float      boundingradius;
   vec3_t      eyeposition;
   int         numskins;
   int         skinwidth;
   int         skinheight;
   int         numverts;
   int         numtris;
   int         numframes;
   synctype_t   synctype;
   int         flags;
   float      size;
#ifdef SWQUAKE
   int               model;
   int               stverts;
   int               skindesc;
#endif
   int               numposes;
   int               poseverts;
   int               posedata;   // numposes*poseverts trivert_t

   int               baseposedata; //original verts for triangles to reference
   int               triangles; //we need tri data for shadow volumes

   int               commands;   // gl command list with embedded s/t
   int               gl_texturenum[MAX_SKINS][4];
   int               texels[MAX_SKINS];
   maliasframedesc_t   frames[];   // variable sized
*/
} aliashdr_t;

typedef struct maliasgroupframedesc_s {
   dtrivertx_t bboxmin;
   dtrivertx_t bboxmax;
   int         frame;
} maliasgroupframedesc_t;

typedef struct daliasgroup_s { //0x0C
   int         numframes;
   dtrivertx_t bboxmin;   // lightnormal isn't used
   dtrivertx_t bboxmax;   // lightnormal isn't used
} daliasgroup_t;
typedef struct maliasgroup_s { //0x14

   int numframes;
   int intervals;
   maliasgroupframedesc_t frames[1];
} maliasgroup_t;

typedef struct daliasskingroup_s { //0x04
   int numskins;
} daliasskingroup_t;
typedef struct maliasskingroup_s {
   int               numskins;
   int               intervals;
   maliasskindesc_t   skindescs[1];
} maliasskingroup_t;

#define SPRITEHL_VERSION 2

typedef struct mspriteframe_s { //0x20

   int   width;
   int   height;
   float   up, down, left, right;
   int   gl_texturenum;
   byte  pixels[4];

} mspriteframe_t;
typedef struct dspriteframe_s { //0x10

   int origin[2]; //00
   int width; //08
   int height; //0c
} dspriteframe_t;

typedef struct dspriteinterval_s {
   float interval;
} dspriteinterval_t;

typedef struct dspriteframetype_s {
   spriteframetype_t type;
} dspriteframetype_t;

typedef struct mspriteframedesc_s {

   spriteframetype_t type;
   mspriteframe_t    *frameptr;
} mspriteframedesc_t;

typedef struct dsprite_s { //0x28

   int   ident;
   int   version;
   int   type;
   int padding0c; //size value
   float boundingradius;
   int   width;
   int   height;
   int   numframes;
   float beamlength;
   synctype_t synctype;
} dsprite_t;
typedef struct msprite_s { //Inaccurate.

   short type;
   short padding02;
   int   maxwidth;
   int   maxheight;
   int   numframes;
   int   padding10;
   float beamlength;
   int   padding18;
   mspriteframedesc_t frames[1];
   /*
   int               type;
   int               maxwidth;
   int               maxheight;
   int               numframes;
   int padding10;
   int padding14;
   float            beamlength;
   mspriteframedesc_t   frames[1]; //1C
   */
} msprite_t;

typedef struct dspritegroup_s {
   int numframes;
} dspritegroup_t;
typedef struct mspritegroup_s { //0x0C

   int   numframes;
   float * intervals;
   mspriteframe_t *frames[1];
} mspritegroup_t;

typedef struct mstudiobodyparts_s { //0x4C

   char name[64]; //00
   int  nummodels; //40
   int  base; //44
   int  modelindex; //48  index into models array
} mstudiobodyparts_t;
typedef struct mstudiotexture_s {

   char name[64]; //00
   int  flags; //40
   int  width; //44
   int  height; //48
   int  index; //4c
} mstudiotexture_t;

typedef struct mstudiomodel_s { //0x70

	char				name[64]; //00

	int					type; //40

	float				boundingradius; //44

	int					nummesh; //48
	int					meshindex; //4C

	int					numverts;		// 50 number of unique vertices
	int					vertinfoindex;	// 54 vertex bone info
	int					vertindex;		// 58 vertex vec3_t
	int					numnorms;		// 5c number of unique surface normals
	int					norminfoindex;	// 60 normal bone info
	int					normindex;		// 64 normal vec3_t

	int					numgroups;		// 68 deformation groups
	int					groupindex; // 6c
} mstudiomodel_t;

//functions
void Mod_Init();
void Mod_ClearAll();
void Mod_Shutdown();

model_t * Mod_ForName(char *, int, int);
mleaf_t * Mod_PointInLeaf(vec3_t, model_t *);
byte *Mod_LeafPVS(mleaf_t *, model_t *);
HLDS_DLLEXPORT void * Mod_Extradata(model_t *);
int CM_HeadnodeVisible(mnode_t *, byte *, int *);

byte * CM_LeafPAS(int);
byte * CM_LeafPVS(int);
void CM_CalcPAS(model_t *);
void CM_FreePAS();

//sdk
HLDS_DLLEXPORT int ModelFrames(int);
#endif
