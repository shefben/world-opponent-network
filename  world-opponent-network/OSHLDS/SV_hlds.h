/* The SV_ functions are pretty much a good chunk of code, divided into
// separate segments that can't be easily distinguished by name.  They use many
// global variables and will often build onto each other in a very heirarchial
// manner.  Most of them handle structs which, unfortunately, are very very
// different from what's in QW.  Piecing together the structs alone has taken
// a lot of the time.
//
// Slowly but surely, I'm taking chunks of related SV functions and dumping
// them into their own file.  This one is STILL chaotic though.
*/

#ifndef OSHLDS_SV_HEADER
#define OSHLDS_SV_HEADER

#include "endianin.h"
#include "NET.h"
#include "pm.h"


#define MAX_PHYSENTS 600
#define MAX_MOVEENTS 64
#define MAX_PHYSINFO_STRING 256

#define EF_BRIGHTFIELD 0x01 // swirling cloud of particles
#define EF_MUZZLEFLASH 0x02 // single frame ELIGHT on entity attachment 0
#define EF_BRIGHTLIGHT 0x04 // DLIGHT centered at entity origin
#define EF_DIMLIGHT    0x08 // player flashlight
#define EF_INVLIGHT    0x10 // get lighting from ceiling
#define EF_NOINTERP    0x20 // don't interpolate the next frame
#define EF_LIGHT       0x40 // rocket flare glow sprite
#define EF_NODRAW      0x80 // don't draw entity
//structs
/*
typedef struct clearpacketentities3_s { //0x154 I think

   char padding1[0x154];
} clearpacketentities3_t;
typedef struct clearpacketentities2_s {

   uint32 padding00; //a counter probably
   uint32 padding04;
   uint32 padding08;
   uint32 padding0C;
   uint32 padding10;
   uint32 padding14;
   uint32 padding18;
   uint32 padding1C;
   uint32 padding20;
   clearpacketentities3_t * padding24;
   uint32 padding28;
   uint32 padding2C;
} clearpacketentities2_t;
typedef struct clearpacketentities_s {

   uint32 padding00;
   uint32 padding04;
   uint32 padding08;
   uint32 padding0C;
   char padding1[0x17D0];
   uint32 padding17E0;
   uint32 padding17E4;
   clearpacketentities2_t * padding17E8;
   uint32 padding17EC;
} clearpacketentities_t;
*/
typedef struct filteredip_s {
   struct filteredip_s *next;
   netadr_t   adr;
   netadr_t   adrmask;
} filteredip_t;
typedef struct KeyValueData_s { //SDK
   char   *szClassName;   // in: entity classname
   char   *szKeyName;      // in: name of key
   char   *szValue;      // in: value of key
   long   fHandled;      // out: DLL sets to true if key-value pair was understood
} KeyValueData_t;
typedef struct physent_s { //e0

   char           name[32];             // 00 Name of model, or "player" or "world".
   int            player;              //20
   vec3_t         origin;               // 24 Model's origin in world coordinates.
   struct model_s *model;                // 30 only for bsp models
   struct model_s *studiomodel;         // 34 SOLID_BBOX, but studio clip intersections.
   vec3_t         mins, maxs;             // 38 44 only for non-bsp models
   int            info;                // 50 For client or server to use to identify (index into edicts or cl_entities)
   vec3_t         angles;               // 54 rotated entities need this info for hull testing to work.

   int            solid;              // 60 Triggers and func_door type WATER brushes are SOLID_NOT
   int            skin;                 // BSP Contents for such things like fun_door water brushes.
   int            rendermode;           // So we can ignore glass

   // Complex collision detection.
   float          frame;
   int            sequence; //70
   byte           controller[4]; //74
   byte           blending[2]; //78

   int            movetype; //7c
   int            takedamage;
   int            blooddecal;
   int            team;
   int            classnumber; //8c

   // For mods
   int            iuser1;
   int            iuser2;
   int            iuser3;
   int            iuser4; //9c
   float          fuser1;
   float          fuser2;
   float          fuser3;
   float          fuser4; //ac
   vec3_t         vuser1; //b0
   vec3_t         vuser2; //bc
   vec3_t         vuser3; //c8
   vec3_t         vuser4; //d4
} physent_t;
typedef struct playermove_s {        //SDK, different than in QW.  0x4F5CC bytes.  That's a lot.

   int      player_index;       // 00 So we don't try to run the PM_CheckStuck nudging too quickly.
   qboolean server;             // 04 For debugging, are we running physics code on server side?

   qboolean multiplayer;        // 08 1 == multiplayer server
   float    time;               // 0c global_realtime on host, for reckoning duck timing
   float    frametime;          // 10 Duration of this frame

   vec3_t   forward, right, up; // 14 20 2c// Vectors for angles
   // player state
   vec3_t   origin;             // 38 Movement origin.
   vec3_t   angles;             // 44 Movement view angles.
   vec3_t   oldangles;          // 50 Angles before movement view angles were looked at.
   vec3_t   velocity;           // 5c Current movement direction.
   vec3_t   movedir;            // 68 For waterjumping, a forced forward velocity so we can fly over lip of ledge.
   vec3_t   basevelocity;       // 74 Velocity of the conveyor we are standing, e.g.

   // For ducking/dead
   vec3_t   view_ofs;           // 80 Our eye position.
   float    flDuckTime;         // 8c Time we started duck
   qboolean bInDuck;            // 90 In process of ducking or ducked already?

   // For walking/falling
   int      flTimeStepSound;    // Next time we can play a step sound
   int      iStepLeft;

   float    flFallVelocity;     // 9c
   vec3_t   punchangle;         // a0

   float    flSwimTime;         // ac

   float    flNextPrimaryAttack;

   int      effects;            // b4 MUZZLE FLASH, e.g.

   int      flags;              // b8 FL_ONGROUND, FL_DUCKING, etc.
   int      usehull;            // bc 0 = regular player hull, 1 = ducked player hull, 2 = point hull
   float    gravity;            // c0 Our current gravity and friction.
   float    friction;
   int      oldbuttons;         // c8 Buttons last usercmd
   float    waterjumptime;      // Amount of time left in jumping out of water cycle.
   qboolean dead;               // d0 Are we a dead player?
   int      deadflag;
   int      spectator;          // d8 Should we use spectator physics model?
   int      movetype;           // Our movement type, NOCLIP, WALK, FLY.  Form of: MOVETYPE_WALK (sdk)

   int      onground;           // e0 In an attempt to debug Windows DLLs, this struct was checked and it is confirmed on Windows
   int      waterlevel;
   int      watertype;
   int      oldwaterlevel;

   char     sztexturename[256]; //f0
   char     chtexturetype;      //1f0

   float    maxspeed;
   float    clientmaxspeed;     //1f8 Player specific maxspeed

   // For mods
   int      iuser1;
   int      iuser2;             //200
   int      iuser3;
   int      iuser4;
   float    fuser1;
   float    fuser2;             //210
   float    fuser3;
   float    fuser4;
   vec3_t   vuser1;             //21c
   vec3_t   vuser2;             //228
   vec3_t   vuser3;             //234
   vec3_t   vuser4;             //240
   // world state
   // Number of entities to clip against.
   int       numphysent;                //24c
   physent_t physents[MAX_PHYSENTS];    //250 +20D00?
   // Number of momvement entities (ladders)
   int       nummoveent;                //20f50
   // just a list of ladders
   physent_t moveents[MAX_MOVEENTS];    //20f54 + 3800

   // All things being rendered, for tracing against things you don't actually collide with
   int       numvisent;                 //24754
   physent_t visents[MAX_PHYSENTS];     //24758

   // input to run through physics.
   usercmd_t cmd;                       //45458

   // Trace results for objects we collided with.
   int       numtouch;                  //4548c matches SDK
   pmtrace_t touchindex[MAX_PHYSENTS];  //45490 + 9F60

   char      physinfo[ MAX_PHYSINFO_STRING ]; // 4F3F0 Physics info string

   struct movevars_s * movevars;        //4F4F0
   vec3_t    player_mins[4];            //4F4F4
   vec3_t    player_maxs[4];            //4f524

   // Common functions
   HLDS_DLLEXPORT const char * (*PM_Info_ValueForKey)(const char *s, const char *key);
   HLDS_DLLEXPORT void         (*PM_Particle)(vec3_t origin, int color, float life, int zpos, int zvel);
   HLDS_DLLEXPORT int          (*PM_TestPlayerPosition) (vec3_t pos, pmtrace_t *ptrace);
   HLDS_DLLEXPORT void         (*Con_NPrintf)(int idx, const char *fmt, ...);
   HLDS_DLLEXPORT void         (*Con_DPrintf)(const char *fmt, ...);
   HLDS_DLLEXPORT void         (*Con_Printf)(const char *fmt, ...);
   HLDS_DLLEXPORT double       (*Sys_FloatTime)(void);
   HLDS_DLLEXPORT void         (*PM_StuckTouch)(int hitent, pmtrace_t *ptraceresult);
   HLDS_DLLEXPORT int          (*PM_PointContents)(vec3_t p, int *truecontents);
   HLDS_DLLEXPORT int          (*PM_TruePointContents)(vec3_t p);
   HLDS_DLLEXPORT int          (*PM_HullPointContents)(struct hull_s *hull, int num, vec3_t p);
   HLDS_DLLEXPORT pmtrace_t    (*PM_PlayerTrace)(vec3_t start, vec3_t end, int traceFlags, int ignore_pe);
   HLDS_DLLEXPORT pmtrace_t *  (*PM_TraceLine)(vec3_t start, vec3_t end, int flags, int usehull, int ignore_pe);
   HLDS_DLLEXPORT long         (*RandomLong)(long lLow, long lHigh);
   HLDS_DLLEXPORT float        (*RandomFloat)(float flLow, float flHigh);
   HLDS_DLLEXPORT int          (*PM_GetModelType)(struct model_s *mod);
   HLDS_DLLEXPORT void         (*PM_GetModelBounds)(struct model_s *mod, vec3_t mins, vec3_t maxs);
   HLDS_DLLEXPORT hull_t *     (*PM_HullForBsp)(physent_t *pe, vec3_t offset);
   HLDS_DLLEXPORT float        (*PM_TraceModel)(physent_t *pEnt, vec3_t start, vec3_t end, trace_t *trace);
   HLDS_DLLEXPORT int          (*COM_FileSize)(char *filename);
   HLDS_DLLEXPORT byte *       (*COM_LoadFile)(const char *path, int usehunk, int *pLength);
   HLDS_DLLEXPORT void         (*COM_FreeFile)(byte *buffer);
   HLDS_DLLEXPORT char *       (*memfgets)(byte *pMemFile, int fileSize, int *pFilePos, char *pBuffer, int bufferSize);

   // Functions
   // Run functions for this frame?
   HLDS_DLLEXPORT qboolean      runfuncs;
   HLDS_DLLEXPORT void         (*PM_PlaySound)(int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch);
   HLDS_DLLEXPORT const char * (*PM_TraceTexture)(int ground, vec3_t vstart, vec3_t vend);
   HLDS_DLLEXPORT void         (*PM_PlaybackEventFull)(int flags, int clientindex, unsigned short eventindex, float delay, vec3_t origin, vec3_t angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);

   HLDS_DLLEXPORT pmtrace_t    (*PM_PlayerTraceEx)(vec3_t start, vec3_t end, int traceFlags, int (*pfnIgnore)(physent_t *pe));
   HLDS_DLLEXPORT int          (*PM_TestPlayerPositionEx)(vec3_t pos, pmtrace_t *ptrace, int (*pfnIgnore)(physent_t *pe));
   HLDS_DLLEXPORT pmtrace_t *  (*PM_TraceLineEx)(vec3_t start, vec3_t end, int flags, int usehull, int (*pfnIgnore)(physent_t *pe));
} playermove_t;
typedef struct gpnewusermsg_struct_s { //unknown

   int32 padding00_id; //not larger than 1 byte
   int32 padding04_size; // ""
   union { //This is just plain evil.
      int32 asInts[4];
      char asString[16];
   } padding08_name;
   struct gpnewusermsg_struct_s * next; //18
   int32 padding1C; // Never seen referenced.
} gpnewusermsg_struct_t;


//vars

extern playermove_t * global_pmove;
extern playermove_t   global_svmove;
extern edict_t * global_sv_player;
extern gpnewusermsg_struct_t * global_sv_gpNewUserMsg;
extern gpnewusermsg_struct_t * global_sv_gpUserMsg;

extern char global_localinfo[0x8001];
extern int key_dest; //Unclear purpose


//Functions
void SV_Init();
void SV_CountPlayers(int *);
void SV_InvokeCallback();
void SV_SetCallback(uint32, uint32, uint32, uint32 *, uint32, uint32);
void SV_SetNewInfo(uint32);
void SV_SetMaxclients();
void SV_ServerShutdown();
void SV_GetPlayerHulls();
int SV_ModelIndex(char *);
void SV_InactivateClients();
int SV_SpawnServer(int, const char *, const char *);

void SV_ActivateServer(int);

void SV_LoadEntities();
void SV_AllocPacketEntities(client_frame_t *, int);
void SV_DeallocateDynamicData();


void SV_ClearCaches();
void Host_ClearClients();
void SV_ClearClientStates();
void SV_ClearEntities();
void SV_ClearFrames(client_frame_t **);
void SV_ClearPacketEntities(client_frame_t *);

HLDS_DLLEXPORT unsigned char * SV_FatPVS(vec3_t);
HLDS_DLLEXPORT unsigned char * SV_FatPAS(vec3_t);

HLDS_DLLEXPORT edict_t * PF_CreateFakeClient_I(const char *);
HLDS_DLLEXPORT const char * PM_SV_TraceTexture(int, vec3_t, vec3_t);


#endif
