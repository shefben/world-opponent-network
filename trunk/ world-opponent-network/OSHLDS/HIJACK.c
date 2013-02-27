#include <stdio.h>
#include <stdlib.h>
#include "HIJACK.h"
#include "cvar.h"
#include "pf.h"
#include "report.h"
#include "Mod.h"
#include "VecMath.h"
#include "R.h"



//non-hijacked \stuff
//Current status: all header files functional.  All C files work except SV_hlds.c
//SV_CreatePacketEntities is responsible for trouble.

//static void *enginesolib = NULL;

BlendingAPI_t * g_pSvBlendingAPI = NULL;
Mod_DLL_Functions_t gEntityInterface;
SecurityModule_t g_modfuncs;
g_vars_t gGlobalVariables;

client_t * host_client = NULL;
double host_frametime;
int giActive;


client_static_t cls;
server_static_t svs;
server_t sv;

double realtime;


int sv_playermodel;
vec3_t player_mins[5] = { { -16, -16, -36 },
                          { -16, -16, -18 },
                          {   0,   0,   0 },
                          { -32, -32, -32 },
                          {   0,   0,   0 } };
vec3_t player_maxs[5] = { {  16,  16,  36 },
                          {  16,  16,  18 },
                          {   0,   0,   0 },
                          {  32,  32,  32 },
                          {   0,   0,   0 } };

char com_clientfallback[0x104];
int allow_cheats;
netadr_t sUpdateIPAddress;
movevars_t movevars;

netadr_t net_from;
sizebuf_t net_message;
netadr_t net_local_adr;
int net_drop;
qboolean noip;

playermove_t * pmove = NULL;
playermove_t  g_svmove;
edict_t * sv_player = NULL;
int key_dest;
sv_delta_t * g_sv_delta = NULL;
int g_groupop;
int g_groupmask;
NEW_DLL_FUNCTIONS_t gNewDLLFunctions;
char * pr_strings = NULL;
int gfUseLANAuthentication;

char localinfo[0x8001];

char * wadpath = NULL;
char texgammatable[0x100];
texture_t * r_notexture_mip = NULL;
int r_pixbytes = 1;

int sv_decalnamecount = 0;
char (sv_decalnames)[0x200][0x11];

char (localmodels)[512][5];
int host_hunklevel;


edict_t ** g_moved_edict = NULL;
vec3_t * g_moved_from = NULL;

float (bonetransform)[0x80][3][4];
float (rotationmatrix)[3][4];

int g_iextdllMac;
DLLlist_t (g_rgextdll)[50];


gpnewusermsg_struct_t * sv_gpNewUserMsg = NULL;
gpnewusermsg_struct_t * sv_gpUserMsg = NULL;











//SV_ValidClientMulticast: Bad argument 132
