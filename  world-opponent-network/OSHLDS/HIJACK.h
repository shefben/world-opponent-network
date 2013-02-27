/* All that hijacking is here now
*/

#ifndef HLDS_LINUX_ENGINE_HIJACK
#define HLDS_LINUX_ENGINE_HIJACK

#include "endianin.h"
#include "Q_memdef.h"
#include "report.h"
#include "common.h"
#include "NET.h"
#include "delta.h"
#include "SV_hlds.h"
#include "cmd.h"
#include "edict.h"

/* Engine function type stuff.  The mod needs the addresses of the engine, well,
// the engine needs mod addresses as well.  Note eiface.h and cbase.h/cpp.
*/
typedef void SAVERESTOREDATA; //It's a large struct but I don't feel like digging it out

typedef void KeyValueData;
typedef void TYPEDESCRIPTION;

typedef struct DLLlist_s {

   void * dll;
#ifdef USING_UNIX_BABY
   void * SymbolTable; //unused on unix
#else
   void * SymbolTable;
#endif
   int NumSymbols;
} DLLlist_t;

typedef struct Mod_DLL_Functions_s { //50 functions, 200 bytes

   void         MOD_STD_CALLING_IMPORTED (*pfnGameInit)                 (void);
   int          MOD_STD_CALLING_IMPORTED (*pfnSpawn)                    (edict_t *pent);
   void         MOD_STD_CALLING_IMPORTED (*pfnThink)                    (edict_t *pent);
   void         MOD_STD_CALLING_IMPORTED (*pfnUse)                      (edict_t *pentUsed, edict_t *pentOther);
   void         MOD_STD_CALLING_IMPORTED (*pfnTouch)                    (edict_t *pentTouched, edict_t *pentOther);
   void         MOD_STD_CALLING_IMPORTED (*pfnBlocked)                  (edict_t *pentBlocked, edict_t *pentOther);
   void         MOD_STD_CALLING_IMPORTED (*pfnKeyValue)                 (edict_t *pentKeyvalue, KeyValueData *pkvd);
   void         MOD_STD_CALLING_IMPORTED (*pfnSave)                     (edict_t *pent, SAVERESTOREDATA *pSaveData);
   int          MOD_STD_CALLING_IMPORTED (*pfnRestore)                  (edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnSetAbsBox)                (edict_t *pent);
   void         MOD_STD_CALLING_IMPORTED (*pfnSaveWriteFields)          (SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
   void         MOD_STD_CALLING_IMPORTED (*pfnSaveReadFields)           (SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
   void         MOD_STD_CALLING_IMPORTED (*pfnSaveGlobalState)          (SAVERESTOREDATA *);
   void         MOD_STD_CALLING_IMPORTED (*pfnRestoreGlobalState)       (SAVERESTOREDATA *);
   void         MOD_STD_CALLING_IMPORTED (*pfnResetGlobalState)         (void);
   qboolean     MOD_STD_CALLING_IMPORTED (*pfnClientConnect)            (edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
   void         MOD_STD_CALLING_IMPORTED (*pfnClientDisconnect)         (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnClientKill)               (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnClientPutInServer)        (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnClientCommand)            (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnClientUserInfoChanged)    (edict_t *pEntity, char *infobuffer);
   void         MOD_STD_CALLING_IMPORTED (*pfnServerActivate)           (edict_t *pEdictList, int edictCount, int clientMax);
   void         MOD_STD_CALLING_IMPORTED (*pfnServerDeactivate)         (void);
   void         MOD_STD_CALLING_IMPORTED (*pfnPlayerPreThink)           (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnPlayerPostThink)          (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnStartFrame)               (void);
   void         MOD_STD_CALLING_IMPORTED (*pfnParmsNewLevel)            (void);
   void         MOD_STD_CALLING_IMPORTED (*pfnParmsChangeLevel)         (void);
   const char * MOD_STD_CALLING_IMPORTED (*pfnGetGameDescription)       (void);
   void         MOD_STD_CALLING_IMPORTED (*pfnPlayerCustomization)      (edict_t *pEntity, customization_t *pCustom);
   void         MOD_STD_CALLING_IMPORTED (*pfnSpectatorConnect)         (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnSpectatorDisconnect)      (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnSpectatorThink)           (edict_t *pEntity);
   void         MOD_STD_CALLING_IMPORTED (*pfnSys_Error)                (const char *error_string);
   void         MOD_STD_CALLING_IMPORTED (*pfnPM_Move)                  (playermove_t *ppmove, qboolean server);
   void         MOD_STD_CALLING_IMPORTED (*pfnPM_Init)                  (playermove_t *ppmove);
   char         MOD_STD_CALLING_IMPORTED (*pfnPM_FindTextureType)       (char *name);
   void         MOD_STD_CALLING_IMPORTED (*pfnSetupVisibility)          (edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas);
   void         MOD_STD_CALLING_IMPORTED (*pfnUpdateClientData)         (const edict_t *ent, int sendweapons, clientdata_t *cd);
   int          MOD_STD_CALLING_IMPORTED (*pfnAddToFullPack)            (entity_state_t *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
   void         MOD_STD_CALLING_IMPORTED (*pfnCreateBaseline)           (int player, int eindex, entity_state_t *baseline, edict_t *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
   void         MOD_STD_CALLING_IMPORTED (*pfnRegisterEncoders)         (void );
   int          MOD_STD_CALLING_IMPORTED (*pfnGetWeaponData)            (edict_t *player, weapon_data_t *info);
   void         MOD_STD_CALLING_IMPORTED (*pfnCmdStart)                 (const edict_t *player, const usercmd_t *cmd, unsigned int random_seed);
   void         MOD_STD_CALLING_IMPORTED (*pfnCmdEnd)                   (const edict_t *player);
   int          MOD_STD_CALLING_IMPORTED (*pfnConnectionlessPacket)     (const netadr_t *net_from, const char *args, char *response_buffer, int *response_buffer_size);
   int          MOD_STD_CALLING_IMPORTED (*pfnGetHullBounds)            (int hullnumber, float *mins, float *maxs);
   void         MOD_STD_CALLING_IMPORTED (*pfnCreateInstancedBaselines) (void);
   int          MOD_STD_CALLING_IMPORTED (*pfnInconsistentFile)         (const edict_t *player, const char *filename, char *disconnect_message);
   int          MOD_STD_CALLING_IMPORTED (*pfnAllowLagCompensation)     (void);
} Mod_DLL_Functions_t;

typedef struct SecurityModule_s { //unknown; not even known to be a security module.

   void (*Function1)  (void);
   void (*Function2)  (void);
   void (*Function3)  (void);
   void (*Function4)  (void);
   void (*Function5)  (void);
   void (*Function6)  (void);
   void (*Function7)  (netchan_t *, sizebuf_t *);
   int  (*Function8)  (netchan_t *, sizebuf_t *); //1c
   void (*Function9)  (void);
   void (*Function10) (model_t *, byte *);
   void (*Function11) (void);
   void (*Function12) (void);
   void (*Function13) (void);
   void (*Function14) (void);
   void (*Function15) (void);
   void (*Function16) (void);
   void (*Function17) (void);
   void (*Function18) (int); //44
   void (*Function19) (void);
   void (*Function20) (byte *, int); //4c
   void (*Function21) (void);
   void (*Function22) (void);
   void (*Function23) (void);
   void (*Function24) (void);
   void (*Function25) (void);
   void (*Function26) (void);
   void (*Function27) (void);
   void (*Function28) (void);
   void (*Function29) (void);
} SecurityModule_t;

typedef struct NEW_DLL_FUNCTIONS_s{ //Three functions.
   // Called right before the object's memory is freed.
   // Calls its destructor.
   void MOD_STD_CALLING_IMPORTED (*pfnOnFreeEntPrivateData)(edict_t *pEnt);
   void MOD_STD_CALLING_IMPORTED (*pfnGameShutdown)(void);
   int  MOD_STD_CALLING_IMPORTED (*pfnShouldCollide)( edict_t *pentTouched, edict_t *pentOther );
} NEW_DLL_FUNCTIONS_t;

typedef struct BlendingAPI_s {

   int padding00;
   void (*BoneSetup)(model_t *, float, int, vec3_t, vec3_t, byte *, byte *, int, edict_t *); //SV_StudioSetupBones

} BlendingAPI_t;




//History:  Until this was standalone, a lot of variables were placed here
//and dynamically linked to.  ALmost all of these followed the same format--
//a pointer to the object, with the HIJACK define dereferencing the pointer.
//I did it that way so that I could simly remove the dereferencing and have them
//be global variables when it was time to unhijack everything and make a
//standalone release.  It worked.  These vars DO need to be moved
//to appropriate files, but for now, it works.

/*** EXTERN ***/
extern BlendingAPI_t * g_pSvBlendingAPI;

extern Mod_DLL_Functions_t gEntityInterface;
extern SecurityModule_t g_modfuncs;
extern g_vars_t gGlobalVariables;


extern client_t * host_client; //can NOT be released
extern double host_frametime;
extern int giActive;

extern netadr_t net_local_adr;
extern netadr_t net_from;
extern sizebuf_t net_message;
extern int net_drop;
extern qboolean noip;

extern client_static_t cls;

extern server_static_t svs;
extern server_t sv;

extern edict_t * sv_player;
extern int key_dest;
extern sv_delta_t * g_sv_delta;

extern int g_groupop;
extern int g_groupmask;
extern NEW_DLL_FUNCTIONS_t gNewDLLFunctions;
extern char * pr_strings;

extern char localinfo[0x8001];


extern double realtime;

extern edict_t ** g_moved_edict;
extern vec3_t * g_moved_from;

extern char * wadpath; // Path of wad files associated with current BSP
extern char texgammatable[0x100];
extern texture_t * r_notexture_mip;
extern int r_pixbytes;

extern int sv_playermodel;
extern vec3_t player_mins[5]; //These two may be constants. vec3_t[4]
extern vec3_t player_maxs[5];

extern char com_clientfallback[0x104];
extern int allow_cheats;
extern netadr_t sUpdateIPAddress;
extern movevars_t movevars;

extern playermove_t * pmove;
extern playermove_t  g_svmove;

extern int gfUseLANAuthentication;
extern gpnewusermsg_struct_t * sv_gpNewUserMsg;
extern gpnewusermsg_struct_t * sv_gpUserMsg;

extern int sv_decalnamecount;
extern char (sv_decalnames)[0x200][0x11];

extern vec_t (bonetransform)[0x80][3][4];
extern vec_t (rotationmatrix)[3][4];

extern char (localmodels)[512][5];
extern int host_hunklevel;

extern int g_iextdllMac;
extern DLLlist_t (g_rgextdll)[50];

#endif
