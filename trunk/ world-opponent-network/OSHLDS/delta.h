/* Deltas are a bit of a mystery to me, but I don't write mods.  What I do know
// is they're a pain, and they involve a lot of low level bit manipulation.
// Typecasts are common (good thing char * to anything is still legal), and
// necessary--the data actually has identifiers that tell the program what to
// cast it as.
//
// More work than it looked like at first.
*/

#ifndef OSHLDS_DELTA_HEADER
#define OSHLDS_DELTA_HEADER

#include "sizebuf.h"
#include "endianin.h"
#include "ModCalls.h"

// structures
typedef struct usercmd_s { //0x34

   short   lerp_msec;      // Interpolation time on client padding2440
   byte    msec;           // Duration in ms of command 2442
   vec3_t   viewangles;     // Command view angles. 2444

// intended velocities
   float   forwardmove;    // Forward velocity. 2450
   float   sidemove;       // Sideways velocity. 2454
   float   upmove;         // Upward velocity. 2458
   byte   lightlevel;     // Light level at spot where we are standing. 245c
   unsigned short  buttons;  // Attack buttons 245e
   byte    impulse;          // Impulse command issued. 2460
   byte   weaponselect;   // Current weapon id 2461

// Experimental player impact stuff.
   int      impact_index; //2464
   vec3_t   impact_position; //2468
} usercmd_t;
typedef struct delta_object_3_s { //0x44

   uint32 padding00;
   char name[0x20];
   uint32 padding24; //Appears to be an offset in a block of data.
   uint16 padding28;
   uint16 padding2A;
   uint32 padding2C;
   float  padding30;
   float  padding34;
   uint16 flags;
   uint16 padding3A;
   uint32 padding3C_sendcount; //stats
   uint32 padding40; //stats
} delta_object_3_t;

typedef struct sv_builddelta_s { //0x30

   uint32 padding00; //00, the msb is a boolean of some sort, the rest is a type ID.
   uint32 num_deltas; //04
   char name[0x20]; //08
   void (*ConditionalEncoder)(struct sv_builddelta_s *, const char *, const char *); //28
   delta_object_3_t * deltas;
} sv_builddelta_t;


typedef struct global_sv_delta_s {

   struct global_sv_delta_s * next;
   char * name;
   char * filename;
   sv_builddelta_t * padding0C;

} sv_delta_t;
typedef struct global_sv_delta2_s {

   struct global_sv_delta2_s * next;
   delta_object_3_t * data;
} global_sv_delta2_t;


//These are 'used' a lot in the DELTA functions, but I don't use the defines.
//Todo: replace the explicit values with the defines.
#define DELTA_DT_BYTE           0x00000001
#define DELTA_DT_SHORT          0x00000002
#define DELTA_DT_FLOAT          0x00000004
#define DELTA_DT_INTEGER        0x00000008
#define DELTA_DT_ANGLE          0x00000010
#define DELTA_DT_TIMEWINDOW_8   0x00000020
#define DELTA_DT_TIMEWINDOW_BIG 0x00000040
#define DELTA_DT_STRING         0x00000080

#define DELTA_DT_SIGNED         0x80000000

void DELTA_Init();
unsigned int DELTA_CheckDelta(char *, char *, sv_builddelta_t *);
int DELTA_TestDelta(char *, char *, sv_builddelta_t *);
int DELTA_WriteDelta(char *, char *, int, sv_builddelta_t *, void (*)());
int DELTA_ParseDelta(char *, char *, sv_builddelta_t *);
sv_builddelta_t * SV_LookupDelta(const char *);
void SV_WriteDeltaDescriptionsToClient(sizebuf_t *);
void SV_WriteDeltaHeader(uint32, uint32, uint32, uint32 *, uint32, uint32, uint32, uint32);
void SV_InitEncoders();

HLDS_DLLEXPORT void DELTA_SetField(sv_builddelta_t *, const char *);
HLDS_DLLEXPORT void DELTA_UnsetField(sv_builddelta_t *, const char *);
HLDS_DLLEXPORT void DELTA_AddEncoder(char *, void (*)(struct sv_builddelta_s *, const char *, const char *));
HLDS_DLLEXPORT unsigned int DELTA_FindFieldIndex(sv_builddelta_t *, const char *);
HLDS_DLLEXPORT void DELTA_SetFieldByIndex(sv_builddelta_t *, unsigned int);
HLDS_DLLEXPORT void DELTA_UnsetFieldByIndex(sv_builddelta_t *, unsigned int);

void SV_InitDeltas();
void SV_Shutdown();
void DELTA_Shutdown();

#if 0 //It's an optimization; we don't need it
extern sv_builddelta_t * g_pplayerdelta;
extern sv_builddelta_t * g_pentitydelta;
extern sv_builddelta_t * g_pcustomentitydelta;
extern sv_builddelta_t * g_pclientdelta;
extern sv_builddelta_t * g_pweapondelta;
extern sv_builddelta_t * g_peventdelta;
#endif

#endif
