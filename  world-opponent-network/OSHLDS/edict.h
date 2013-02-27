#ifndef OSHLDS_EDICT_HEADER
#define OSHLDS_EDICT_HEADER

#include "endianin.h"
#include "links.h"
#include "ModCalls.h"

#define MAX_ENT_LEAFS 48 //defd as 16 in QW

#define Q1CONTENTS_EMPTY -1
#define Q1CONTENTS_SOLID -2
#define Q1CONTENTS_WATER -3
#define Q1CONTENTS_SLIME -4
#define Q1CONTENTS_LAVA  -5
#define Q1CONTENTS_SKY   -6
#define DIST_EPSILON (0.03125)

//edict.v.flags
#define FL_FLY               0x00000001 //Changes the SV_Movestep() behavior to not need to be on ground
#define FL_SWIM            0x00000002 // Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define FL_CONVEYOR        0x00000004
#define FL_CLIENT          0x00000008
#define FL_INWATER         0x00000010
#define FL_MONSTER         0x00000020
#define FL_GODMODE         0x00000040
#define FL_NOTARGET        0x00000080
#define FL_SKIPLOCALHOST   0x00000100 // Don't send entity to local host, it's predicting this entity itself
#define FL_ONGROUND        0x00000200 // At rest / on the ground
#define FL_PARTIALGROUND   0x00000400 // not all corners are valid
#define FL_WATERJUMP       0x00000800 // player jumping out of water
#define FL_FROZEN          0x00001000 // Player is frozen for 3rd person camera
#define FL_FAKECLIENT      0x00002000 // JAC: fake client, simulated server side; don't send network messages to them
#define FL_DUCKING         0x00004000 // Player flag -- Player is fully crouched
#define FL_FLOAT            0x00008000 // Apply floating force to this entity when in water
#define FL_GRAPHED         0x00010000 // worldgraph has this ent listed as something that blocks a connection
#define FL_IMMUNE_WATER    0x00020000
#define FL_IMMUNE_SLIME    0x00040000
#define FL_IMMUNE_LAVA     0x00080000
#define FL_PROXY           0x00100000 // This is a spectator proxy
#define FL_ALWAYSTHINK     0x00200000 // Brush model flag -- call think every frame regardless of nextthink - ltime (for constantly changing velocity/path)
#define FL_BASEVELOCITY    0x00400000 // Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_MONSTERCLIP     0x00800000 // Only collide in with monsters who have FL_MONSTERCLIP set
#define FL_ONTRAIN         0x01000000 // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_WORLDBRUSH      0x02000000 // Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_SPECTATOR       0x04000000 // This client is a spectator, don't run touch functions, etc.

#define FL_CUSTOMENTITY    0x20000000 // This is a custom entity
#define FL_KILLME          0x40000000 // This entity is marked for death -- This allows the engine to kill ents at the appropriate time
#define FL_DORMANT         0x80000000 // Entity is dormant, no updates to client

//Many things in const.h need to be implemented.  Many of them here.
//movetypes:
#define MOVETYPE_NONE           0x00   // never moves
#define MOVETYPE_ANGLENOCLIP    0x01
#define MOVETYPE_ANGLECLIP      0x02
#define MOVETYPE_WALK           0x03   // Player only - moving on the ground
#define MOVETYPE_STEP           0x04   // gravity, special edge handling -- monsters use this
#define MOVETYPE_FLY            0x05   // No gravity, but still collides with stuff
#define MOVETYPE_TOSS           0x06   // gravity/collisions
#define MOVETYPE_PUSH           0x07   // no clip to world, push and crush
#define MOVETYPE_NOCLIP         0x08   // No gravity, no collisions, still do velocity/avelocity
#define MOVETYPE_FLYMISSILE     0x09   // extra size to monsters
#define MOVETYPE_BOUNCE         0x0A   // Just like Toss, but reflect velocity when contacting surfaces
#define MOVETYPE_BOUNCEMISSILE  0x0B   // bounce w/o gravity
#define MOVETYPE_FOLLOW         0x0C   // track movement of aiment
#define MOVETYPE_PUSHSTEP       0x0D   // BSP model that needs physics/world collisions (uses nearest hull for world collision)
//The missing one is 'swim', number 14, which must be a QW only movetype.

//solids:
#define SOLID_NOT      0      // no interaction with other objects
#define SOLID_TRIGGER  1      // touch on edge, but not blocking
#define SOLID_BBOX     2      // touch on edge, block
#define SOLID_SLIDEBOX 3      // touch on edge, but not an onground
#define SOLID_BSP      4      // bsp clip, touch on edge, block

//takedamage:
#define DAMAGE_NO   0
#define DAMAGE_YES  1
#define DAMAGE_AIM  2

//buttons (part of the SDK, we very rarely check these ourselves):
#define IN_ATTACK    0x00000001
#define IN_JUMP      0x00000002
#define IN_DUCK      0x00000004
#define IN_FORWARD   0x00000008
#define IN_BACK      0x00000010
#define IN_USE       0x00000020
#define IN_CANCEL    0x00000040
#define IN_LEFT      0x00000080
#define IN_RIGHT     0x00000100
#define IN_MOVELEFT  0x00000200
#define IN_MOVERIGHT 0x00000400
#define IN_ATTACK2   0x00000800
#define IN_RUN       0x00001000
#define IN_RELOAD    0x00002000
#define IN_ALT1      0x00004000
#define IN_SCORE     0x00008000 // means the scoreboard is displayed, I think it's the only one we care about.


typedef struct entvars_s {

   string_t classname; //00
   string_t globalname; //04

   vec3_t origin; //08
   vec3_t oldorigin;
   vec3_t velocity; //20
   vec3_t basevelocity;
   vec3_t clbasevelocity;  // Base velocity that was passed in to server physics so
                    //  client can predict conveyors correctly.  Server zeroes it, so we need to store here, too.
   vec3_t movedir; //44

   vec3_t angles; //50         // Model angles
   vec3_t avelocity;   //5c   // angle velocity (degrees per second)
   vec3_t punchangle;   //68   // auto-decaying view angle adjustment
   vec3_t v_angle;   //74   // Viewing angle (player only)

   // For parametric entities
   vec3_t endpos; //80
   vec3_t startpos; //8c
   float impacttime; //98
   float starttime; //9c

   int fixangle;      // 0:nothing, 1:force view angles, 2:add avelocity
   float idealpitch;
   float pitch_speed;
   float ideal_yaw;
   float yaw_speed; //b0

   int modelindex;
   string_t   model; //b8

   int viewmodel;      // player's viewmodel
   int weaponmodel;   // what other players see

   vec3_t      absmin; //c4      // BB max translated to world coord //c0
   vec3_t      absmax; //d0      // BB max translated to world coord //cc
   vec3_t      mins;   //dc   // local BB min,
   vec3_t      maxs;   //e8   // local BB max
   vec3_t      size;   //f4   // maxs - mins

   float      ltime; //100
   float      nextthink;

   int         movetype;
   int         solid; //10c, confirmed--odd since the others are 08 off...

   int         skin;
   int         body;         // sub-model selection for studiomodels
   int       effects;

   float      gravity;      // % of "normal" gravity
   float      friction;      // inverse elasticity of MOVETYPE_BOUNCE

   int         light_level; //124

   int         sequence;      // animation sequence
   int         gaitsequence;   // movement animation sequence for player (0 for none)
   float      frame;         // % playback position in animation sequences (0..255)
   float      animtime;      // world time when frame was set
   float      framerate; //138      // animation playback rate (-8x to 8x)
   byte      controller[4];   // bone controller setting (0..255)
   byte      blending[2];   // blending amount between sub-sequences (0..255)

   float      scale;         // sprite rendering scale (0..255)

   int         rendermode;
   float      renderamt; //14c
   vec3_t      rendercolor; //150
   int         renderfx; //15c

   float      health; //160
   float      frags;
   int         weapons;  // bit mask for available weapons
   float      takedamage;

   int         deadflag; //170
   vec3_t      view_ofs;   // eye position

   int         button; //180
   int         impulse;

   struct edict_s      *chain;         // Entity pointer when linked into a linked list
   struct edict_s      *dmg_inflictor;
   struct edict_s      *enemy;
   struct edict_s      *aiment;      // entity pointer when MOVETYPE_FOLLOW
   struct edict_s      *owner;
   struct edict_s      *groundentity;

   int         spawnflags; //1a0
   int         flags;

   int         colormap;      // lowbyte topcolor, highbyte bottomcolor
   int         team;

   float      max_health;
   float      teleport_time;
   float      armortype; //1b8
   float      armorvalue;
   int         waterlevel;
   int         watertype;

   string_t   target;
   string_t   targetname;
   string_t   netname; //1d0
   string_t   message;

   float      dmg_take;
   float      dmg_save;
   float      dmg;
   float      dmgtime; //1e4

   string_t   noise;
   string_t   noise1;
   string_t   noise2;
   string_t   noise3;

   float      speed; //1f8
   float      air_finished;
   float      pain_finished; //200
   float      radsuit_finished;

   struct edict_s      *pContainingEntity;

   int         playerclass;
   float      maxspeed; //210

   float      fov;
   int         weaponanim;

   int         pushmsec; //21c

   int         bInDuck;
   int         flTimeStepSound;
   int         flSwimTime; //228
   int         flDuckTime;
   int         iStepLeft;
   float      flFallVelocity;

   int         gamestate;

   int         oldbuttons;

   int         groupinfo; //240

   // For mods
   int         iuser1;
   int         iuser2;
   int         iuser3;
   int         iuser4; //250
   float      fuser1;
   float      fuser2;
   float      fuser3;
   float      fuser4;
   vec3_t      vuser1; //264
   vec3_t      vuser2; //270
   vec3_t      vuser3; //27c
   vec3_t      vuser4; //288
   struct edict_s      *euser1; //294
   struct edict_s      *euser2;
   struct edict_s      *euser3;
   struct edict_s      *euser4; //2a0
} entvars_t;
typedef struct edict_s { //Within QW and the SDK.

   qboolean   free; //00
   int        serialnumber; //04
   link_t     area;   //08         // linked to a division node or leaf

   int        headnode; //10         // -1 to use normal leaf check
   int        num_leafs; //14
   short      leafnums[MAX_ENT_LEAFS]; //18

   float      freetime; //78         // global_sv.time when the object was freed

   void *     pvPrivateData;   //7c   // Alloced and freed by engine, used by DLLs

   entvars_t  v;   //80         // C exported fields from progs
} edict_t;


int NUM_FOR_EDICT(const edict_t *);
edict_t * EDICT_NUM(const unsigned int);
void * GetEntityInit(const char *);

void ED_ClearEdict(edict_t *e);
edict_t * ED_Alloc();
void ED_Count();
const char * ED_ParseEdict(const char *data, edict_t *ent);
void ED_LoadFromFile(const char *data);
char * ED_NewString(const char * arg0_string);
void ED_Free(edict_t *ed);

void ReleaseEntityDLLFields(edict_t *);
void InitEntityDLLFields(edict_t *);
int EntOffsetOfPEntity(const edict_t *);

HLDS_DLLEXPORT edict_t * PEntityOfEntOffset(int iEntOffset);
HLDS_DLLEXPORT const char * SzFromIndex(int iString);
HLDS_DLLEXPORT struct entvars_s * GetVarsOfEnt(edict_t *pEdict);
#endif
