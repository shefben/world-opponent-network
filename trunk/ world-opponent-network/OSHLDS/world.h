/* Straight out of QW.  World has been stripped of some physics work,
// but there's a lot of overlap between it and general movement.
*/
#ifndef OSHLDS_WORLD_HEADER
#define OSHLDS_WORLD_HEADER

#include "edict.h"
#include "endianin.h"
#include "links.h"
#include "Mod.h"
#include "SV_move.h"

typedef struct decal_s {
   struct decal_s    *pnext;    // linked list for each surface
   struct msurface_s *psurface; // Surface id for persistence / unlinking
   short dx;                    // Offsets into surface texture (in texture coordinates, so we don't need floats)
   short dy;
   short texture;               // Decal texture
   byte  scale;                 // Pixel scale
   byte  flags;                 // Decal flags
   short entityIndex;           // Entity this is attached to
} decal_t;
typedef struct areanode_s { //0x20

   int    axis;      // -1 = leaf node
   float  dist;
   struct areanode_s * children[2];
   link_t trigger_edicts;
   link_t solid_edicts;
} areanode_t;

//vars
extern int global_g_groupop;
extern int global_g_groupmask;
extern edict_t ** g_moved_edict;
extern vec3_t * g_moved_from;
extern areanode_t global_sv_areanodes[32];
extern const vec3_t global_player_mins[5];
extern const vec3_t global_player_maxs[5];

//functions
hull_t * SV_HullForEntity(edict_t *, vec3_t, vec3_t, vec3_t);
hull_t * SV_HullForBsp(edict_t *, const vec3_t, const vec3_t, vec3_t);
int SV_PointContents(const vec3_t);
int SV_RecursiveHullCheck(hull_t *, int, float, float, vec_t *, vec_t *, trace_t *);

void SV_LinkEdict(edict_t *, qboolean);
void SV_UnlinkEdict(edict_t *);
void SV_ClipToLinks(areanode_t *, moveclip_t *);
void SV_ClipToWorldBrush(areanode_t *, moveclip_t *);
void SV_AddLinksToPM(areanode_t *, vec3_t);

void SV_ClearWorld();
#endif
