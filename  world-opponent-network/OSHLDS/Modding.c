#include <stdio.h>
#include <limits.h>

#ifdef _WIN32
 #ifdef _MSC_VER
  #include <winsock2.h>
 #endif
 #include <windows.h>
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#else
 #include <dlfcn.h>
#endif

#include "Modding.h"
#include "cmd.h"
#include "host.h"
#include "pf.h"
#include "R.h"
#include "random.h"
#include "SV_mcast.h"
#include "SV_pkt.h"
#include "SV_pktcl.h"

/*** STRUCTS ***/
typedef struct studiofuncs_s { //I couldn't find whoever actually uses this.

   HLDS_DLLEXPORT void * (*calloc)(size_t, size_t);
   HLDS_DLLEXPORT void * (*cache_check)(cache_user_t *);
   HLDS_DLLEXPORT void   (*loadcachefile)(const char *, cache_user_t *);
   HLDS_DLLEXPORT void * (*extradata)(model_t *);

} studiofuncs_t;
typedef struct DLLlist_s {
   void * dll;
#ifdef _WIN32
   void * SymbolTable; //Don't know how to get this in Windows
#else
   void * SymbolTable; //unused on unix
#endif
   int NumSymbols;
} DLLlist_t;


HLDS_DLLEXPORT const char * NameForFunction(void *);
HLDS_DLLEXPORT void * FunctionFromName(const char *);

//unimplemented sdk functions.
HLDS_DLLEXPORT void AnimationAutomove(const edict_t * pEdict, float flTime) { } //naturally empty
HLDS_DLLEXPORT void EngineFprintf(FILE *pfile, char *szFmt, ...) {
   Sys_Error("%s: Obsolete API.\n(I can implement it, but you better have a DAMN good reason first)\n", __FUNCTION__);
}


/*** GLOBAL GLOBALS ***/
Mod_DLL_Functions_t gEntityInterface;
g_vars_t gGlobalVariables;
NEW_DLL_FUNCTIONS_t gNewDLLFunctions;
SecurityModule_t g_modfuncs;


/*** LOCAL GLOBALS ***/
//Table of all exported functions.  Naturally we must include headers for 'em all.
static enginefuncs_t g_engfuncsExportedToDlls = {

   PF_precache_model_I,
   PF_precache_sound_I,
   PF_setmodel_I,
   PF_modelindex,
   ModelFrames,
   PF_setsize_I,
   PF_changelevel_I,
   PF_setspawnparms_I,
   SaveSpawnParms,
   PF_vectoyaw_I,
   PF_vectoangles_I,
   SV_MoveToOrigin_I,
   PF_changeyaw_I,
   PF_changepitch_I,
   FindEntityByString,
   GetEntityIllum,
   FindEntityInSphere,
   PF_checkclient_I,
   PVSFindEntities,
   PF_makevectors_I,
   AngleVectors,
   PF_Spawn_I,
   PF_Remove_I,
   CreateNamedEntity,
   PF_makestatic_I,
   PF_checkbottom_I,
   PF_droptofloor_I,
   PF_walkmove_I,
   PF_setorigin_I,
   PF_sound_I,
   PF_ambientsound_I,
   PF_traceline_DLL,
   PF_TraceToss_DLL,
   TraceMonsterHull,
   TraceHull,
   TraceModel,
   TraceTexture,
   TraceSphere,
   PF_aim_I,
   PF_localcmd_I,
   PF_localexec_I,
   PF_stuffcmd_I,
   PF_particle_I,
   PF_lightstyle_I,
   PF_DecalIndex,
   PF_pointcontents_I,
   PF_MessageBegin_I,
   PF_MessageEnd_I,
   PF_WriteByte_I,
   PF_WriteChar_I,
   PF_WriteShort_I,
   PF_WriteLong_I,
   PF_WriteAngle_I,
   PF_WriteCoord_I,
   PF_WriteString_I,
   PF_WriteEntity_I,
   CVarRegister,
   CVarGetFloat,
   CVarGetString,
   CVarSetFloat,
   CVarSetString,
   AlertMessage,
   EngineFprintf,
   PvAllocEntPrivateData,
   PvEntPrivateData,
   FreeEntPrivateData,
   SzFromIndex,
   AllocEngineString,
   GetVarsOfEnt,
   PEntityOfEntOffset,
   EntOffsetOfPEntity,
   IndexOfEdict,
   PEntityOfEntIndex,
   FindEntityByVars,
   GetModelPtr,
   RegUserMsg,
   AnimationAutomove,
   GetBonePosition,
   FunctionFromName,
   NameForFunction,
   ClientPrintf,
   ServerPrint,
   Cmd_Args,
   Cmd_Argv,
   Cmd_Argc,
   GetAttachment,
   CRC32_Init,
   CRC32_ProcessBuffer,
   CRC32_ProcessByte,
   CRC32_Final,
   RandomLong,
   RandomFloat,
   PF_setview_I,
   PF_Time,
   PF_crosshairangle_I,
   COM_LoadFileForMe,
   COM_FreeFile,
   Host_EndSection,
   COM_CompareFileTime,
   COM_GetGameDir,
   Cvar_RegisterVariable,
   PF_FadeVolume,
   PF_SetClientMaxspeed,
   PF_CreateFakeClient_I,
   PF_RunPlayerMove_I,
   PF_NumberOfEntities_I,
   PF_GetInfoKeyBuffer_I,
   PF_InfoKeyValue_I,
   PF_SetKeyValue_I,
   PF_SetClientKeyValue_I,
   PF_IsMapValid_I,
   PF_StaticDecal,
   PF_precache_generic_I,
   PF_GetPlayerUserId,
   PF_BuildSoundMsg_I,
   PF_IsDedicatedServer,
   CVarGetPointer,
   PF_GetPlayerWONId,
   PF_RemoveKey_I,
   PF_GetPhysicsKeyValue,
   PF_SetPhysicsKeyValue,
   PF_GetPhysicsInfoString,
   EV_Precache,
   EV_Playback,
   SV_FatPVS,
   SV_FatPAS,
   SV_CheckVisibility,
   DELTA_SetField,
   DELTA_UnsetField,
   DELTA_AddEncoder,
   PF_GetCurrentPlayer,
   PF_CanSkipPlayer,
   DELTA_FindFieldIndex,
   DELTA_SetFieldByIndex,
   DELTA_UnsetFieldByIndex,
   PF_SetGroupMask,
   PF_CreateInstancedBaseline,
   PF_Cvar_DirectSet,
   PF_ForceUnmodified,
   PF_GetPlayerStats,
   Cmd_AddGameCommand,
   Voice_GetClientListening,
   Voice_SetClientListening,
   PF_GetPlayerAuthId
};
static studiofuncs_t server_studio_api = {

   calloc,
   Cache_Check,
   COM_LoadCacheFile,
   Mod_Extradata
};

static unsigned int g_iextdllMac;
static DLLlist_t g_rgextdll[50];

/*** FUNCTIONS ***/
/* DESCRIPTION: FindAddressInTable, NameForFunction
// Runs: Init, maybe, if mod is built with debug define
//
// This function actually looks to make sure WE can see a function in a mod.
// Unix never calls this.  Can we yank symbol tables in Unix?
// However it's done, we're SUPPOSSED to look for the function name of
// the corresponding address, which we can't do, because I don't care to.
// The mod doesn't care about the name, only that the return is non-NULL.
*/
const char * FindAddressInTable(DLLlist_t * dll, void * function) {

/*
   unsigned int i;
   for(i = 0; i < dll->NumSymbols; i++) {

      if(dll->SymbolTable[i] == function) { return(dll->SymbolTable[i]); }
   }

   return(NULL);
*/
   return("");
}
const char * NameForFunction(void * function) {

   unsigned int i;
   const char * address;


   for(i = 0; i < g_iextdllMac; i++) {

      address = FindAddressInTable(&(g_rgextdll[i]), function);
      if(address != NULL) { return(address); }
   }

   Con_Printf("%s: Can't find address: %08lx", __FUNCTION__, function);
   return(NULL);
}
//This function again requires those stupid tables.
//Which means it must not be sued on unix.
//Used as wrapper for GetDispatch until such time when symbol tables can be extracted.
//If they should be at all.
HLDS_DLLEXPORT void * FunctionFromName(const char *pName) {
   return(GetDispatch(pName));
}

/* DESCRIPTION: GetDispatch
// Runs: When a function is needed from another file.
//
// Basically a wrapper for dlsym/GetProcAddress, depending on the OS used.
*/
void * GetDispatch(const char * name) {

   unsigned int i;
   void * function;


   for(i = 0; i < g_iextdllMac; i++) {

#ifdef _WIN32
      function = GetProcAddress(g_rgextdll[i].dll, name);
#else
      function = dlsym(g_rgextdll[i].dll, name);
#endif

      if(function != NULL) { return(function); }
   }
   return(NULL);
}

/* DESCRIPTION: LoadThisDll
// Runs: When a DLL is needed, which is usually around startup
//
// This serves both as a wrapper for dlopen/LoadLibrary and also will bive thw
// API to the DLL.
*/
void LoadThisDll(const char * name) {

   void * dll;
   void (*function)(enginefuncs_t *, g_vars_t *);


   if(g_iextdllMac > 49) {
      Con_Printf("%s: Too many DLLs, somehow.  Didn't load %s.\n", __FUNCTION__, name);
      return;
   }

   #ifdef _WIN32

   dll = LoadLibrary(name);
   if(dll == NULL) {
      Con_Printf("%s: Couldn't open dll for some reason.\n", __FUNCTION__);
      return;
   }

   function = (void *)GetProcAddress(dll, "GiveFnptrsToDll");
   if(function == NULL) {
      Con_Printf("%s: Couldn't load pointer to init function.\n", __FUNCTION__);
      FreeLibrary(dll);
      return;
   }

   function(&g_engfuncsExportedToDlls, &gGlobalVariables);

   g_rgextdll[g_iextdllMac].dll = dll;
   g_rgextdll[g_iextdllMac].SymbolTable = NULL;
   g_rgextdll[g_iextdllMac].NumSymbols = 0;

   g_iextdllMac++;

   #else

   dll = dlopen(name, RTLD_NOW);
   if(dll == NULL) {
      Con_Printf("%s: Couldn't open dll because %s.\n", __FUNCTION__, dlerror());
      return;
   }

   function = dlsym(dll, "GiveFnptrsToDll");
   if(function == NULL) {
      Con_Printf("%s: Couldn't load pointer to init function.\n", __FUNCTION__);
      dlclose(dll);
      return;
   }

   function(&g_engfuncsExportedToDlls, &gGlobalVariables);

   g_rgextdll[g_iextdllMac].dll = dll;
   g_rgextdll[g_iextdllMac].SymbolTable = NULL;
   g_rgextdll[g_iextdllMac].NumSymbols = 0;

   g_iextdllMac++;

   #endif
}
/* DESCRIPTION: LoadEntityDLLs
// Runs: During program startup.
//
// This extracts the DLL from the liblist.gam file (if available) and loads it.
// We can load 50 DLLs at once (wow, what a limit), and in theory, we should
// extract the symbol tables as well.  The most important thing however is
// probably getting the API.  With that, we init the mod and exchange function
// addresses.
*/
void LoadEntityDLLs() {

   unsigned int size;
   char DefaultDir[PATH_MAX];
   char SearchPath[PATH_MAX];
   char DLLPath[PATH_MAX];
   const char * GameDLL;
   const char * GameDir;
   byte * fp;

   int ver;
   void (*disp)(NEW_DLL_FUNCTIONS_t *, int *);

   //There are two 'versions' of this function.  Both want to see the
   //number '140' passed to them, but one is by reference and
   //the other is by value.
   int (*api2)(Mod_DLL_Functions_t *, int *);
   int (*api)(Mod_DLL_Functions_t *, int);


   SV_ResetModInfo();
   g_iextdllMac = 0;
   Q_memset(g_rgextdll, 0, 0x258);

   // Check for modified game
   COM_GetGameDirSize(DefaultDir, sizeof(DefaultDir));
   GameDir = COM_GetParmAfterParmCase("-game");
   if(GameDir[0] != '\0' && Q_strcmp(GameDir, DefaultDir) != 0) {

      strncpy(DefaultDir, GameDir, sizeof(DefaultDir)-1);
      DefaultDir[sizeof(DefaultDir)] = '\0';
      SV_SetUsingMod();
   }

   fp = COM_LoadFileForMe("liblist.gam", &size);

   if(fp == NULL) { //find DLLs manually then

      Con_Printf("%s: liblist.gam not found.\n", __FUNCTION__);
      #ifdef _WIN32
      Q_snprintf(SearchPath, sizeof(SearchPath)-1, "%s/dlls/*.dll", DefaultDir);
      #else
      Q_snprintf(SearchPath, sizeof(SearchPath)-1, "%s/dlls/*.so", DefaultDir);
      #endif
      SearchPath[sizeof(SearchPath)-1] = '\0';

      GameDLL = Sys_FindFirst(SearchPath, NULL);
      while(GameDLL != NULL) {

         Q_snprintf(DLLPath, sizeof(DLLPath)-1, "%s\\dlls\\%s", DefaultDir, GameDLL); //Windows cares about slashes.  Boost does not.
         DLLPath[sizeof(DLLPath)-1] = '\0';
         LoadThisDll(DLLPath);
         GameDLL = Sys_FindNext(NULL);
      }
      Sys_FindClose();
   }
   else {

      if(size > 10000) {
         Sys_Error("%s: liblist.gam seems a bit oversized, and possibly corrupt  Fix it.\n", __FUNCTION__);
      }

      SV_LoadModDataFromFile(fp, DefaultDir);
      COM_FreeFile(fp);
   }

   Q_memset(&gNewDLLFunctions, 0, sizeof(NEW_DLL_FUNCTIONS_t));
   disp = GetDispatch("GetNewDLLFunctions");

   if(disp != NULL) {
      ver = 1;
      disp(&gNewDLLFunctions, &ver);
   }

   api2 = GetDispatch("GetEntityAPI2");
   if(api2 != NULL) {

      ver = 140;
      if(api2(&gEntityInterface, &ver) == 0) {

         Sys_Error("%s: Failed to load API.\n", __FUNCTION__);
      }
   }
   else {

      api = GetDispatch("GetEntityAPI");
      if(api == NULL) {
         Sys_Error("%s: Failed to load APIloading function.\n", __FUNCTION__);
      }
      if(api(&gEntityInterface, 140) == 0) {
         Sys_Error("%s: Failed to load API.\n", __FUNCTION__);
      }
   }

   Con_Printf("%s: loaded \"%s\".\n", __FUNCTION__, gEntityInterface.pfnGetGameDescription());
}
/* DESCRIPTION: ReleaseEntityDlls
// Runs: Shutdown
//
// releases the linked DLLs and frees any memory associated with them.
// This should probably numm out the DLL functions too.
*/
void ReleaseEntityDlls() {

   DLLlist_t * dll, * dllmax;


   dll = &g_rgextdll[0];
   dllmax = &g_rgextdll[g_iextdllMac];
   if(global_svs.pad00 == 0) { return; }

   FreeAllEntPrivateData();

   if(gNewDLLFunctions.pfnGameShutdown != NULL) {
      gNewDLLFunctions.pfnGameShutdown();
   }

   Cvar_UnlinkExternals();

   while(dll < dllmax) { //Clever, but is it really faster?

     #ifdef _WIN32
      FreeLibrary(dll->dll);
     #else
      dlclose(dll->dll);
     #endif

      dll->dll = 0;
      if(dll->SymbolTable != NULL) {
         Q_Free(dll->SymbolTable);
         dll->SymbolTable = NULL;
      }
      dll++;
   }

   global_svs.pad00 = 0;
}




/* DESCRIPTION: ReleaseEntityDlls
// Runs: Init
//
// There's one small 'api' like thing with a single function.  This gets that
// function.
//
// The purpose of the function isn't wholly known (aside from the obvious).
// I suspect only client DLLs count for this.  But we get to implement our
// own default function in case this fails--which has mismatched arguments...
*/
void SV_CheckBlendingInterface() {

   unsigned int i;
   int (*function)(int, BlendingAPI_t *, studiofuncs_t *, vec_t var1[3][4], vec_t var2[80][3][4]);


   R_ResetSvBlending();
   for(i = 0; i < g_iextdllMac; i++) {

     #ifdef _WIN32
      function = (int (*)(int, BlendingAPI_t *, studiofuncs_t *, vec_t var1[3][4], vec_t var2[80][3][4]))(GetProcAddress(g_rgextdll[i].dll, "Server_GetBlendingInterface"));
     #else
      function = (int (*)(int, BlendingAPI_t *, studiofuncs_t *, vec_t var1[3][4], vec_t var2[80][3][4]))dlsym(g_rgextdll[i].dll, "Server_GetBlendingInterface");
     #endif

      if(function == NULL) { continue; }

      if(function(1, g_pSvBlendingAPI, &server_studio_api, rotationmatrix, bonetransform) != 0) {
         return;
      }

      //else
      Con_Printf("%s: Call to Server_GetBlendingInterface failed.", __FUNCTION__);
      R_ResetSvBlending();
   }
}

/* DESCRIPTION: TestStructureAlignment
// Runs: During program startup.
//
// Several structures must be aligned perfectly to work with mods.
// Windows and Unix handle doubles differently, while there's an obvious
// difference between 32 bit and 64 bit instruction sets.
// This code will warn you if you've made such an error.  It also
// serves to dissuade tinkerers from messing with things they ought not to.
*/
int TestStructureAlignment() {

   int i;
   int ret = 1;

   // This code may look bastardly, but it gets the job done.
   // In theory, the optimizer SHOULD get rid of ALL of this and simply
   // replace this function with the return value and any printfs.
   // I haven't checked to see though.

   i = sizeof(playermove_t);
   if(i != 0x4F5CC) {
      printf("%s: sizeof(playermove_t) was 0x%X, should be 0x4F5CC.\n", __FUNCTION__, i);
      ret = 0;
   }

   i = ((int)&(((playermove_t *)0)->player_index));
   if(i != 0x0) {
      printf("%s: playermove_t->player_index has offset 0x%X, should be 0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->server));
   if(i != 0x4) {
      printf("%s: playermove_t->server has offset 0x%X, should be 4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->multiplayer));
   if(i != 0x8) {
      printf("%s: playermove_t->multiplayer has offset 0x%X, should be 8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->time));
   if(i != 0x0C) {
      printf("%s: playermove_t->time has offset 0x%X, should be 0x0C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->frametime));
   if(i != 0x10) {
      printf("%s: playermove_t->frametime has offset 0x%X, should be 0x10.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->forward));
   if(i != 0x14) {
      printf("%s: playermove_t->forward has offset 0x%X, should be 0x14.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->right));
   if(i != 0x20) {
      printf("%s: playermove_t->right has offset 0x%X, should be 0x20.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->up));
   if(i != 0x2C) {
      printf("%s: playermove_t->up has offset %X, should be 0x2C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->origin));
   if(i != 0x38) {
      printf("%s: playermove_t->origin has offset %i, should be 0x38.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->angles));
   if(i != 0x44) {
      printf("%s: playermove_t->angles has offset %i, should be 0x44.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->oldangles));
   if(i != 0x50) {
      printf("%s: playermove_t->oldangles has offset %i, should be 0x50.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->velocity));
   if(i != 0x5C) {
      printf("%s: playermove_t->velocity has offset %i, should be 0x5C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->movedir));
   if(i != 0x68) {
      printf("%s: playermove_t->movedir has offset %i, should be 0x68.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->basevelocity));
   if(i != 0x74) {
      printf("%s: playermove_t->basevelocity has offset %i, should be 0x74.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->view_ofs));
   if(i != 0x80) {
      printf("%s: playermove_t->view_ofs has offset %i, should be 0x80.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->flDuckTime));
   if(i != 0x8c) {
      printf("%s: playermove_t->flDuckTime has offset %i, should be 0x8c.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->bInDuck));
   if(i != 0x90) {
      printf("%s: playermove_t->bInDuck has offset %i, should be 0x90.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->flTimeStepSound));
   if(i != 0x94) {
      printf("%s: playermove_t->flTimeStepSound has offset %i, should be 0x94.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->iStepLeft));
   if(i != 0x98) {
      printf("%s: playermove_t->iStepLeft has offset %i, should be 0x98.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->flFallVelocity));
   if(i != 0x9C) {
      printf("%s: playermove_t->flFallVelocity; has offset %i, should be 0x9C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->punchangle));
   if(i != 0xA0) {
      printf("%s: playermove_t->punchangle has offset %i, should be 0xA0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->flSwimTime));
   if(i != 0xAC) {
      printf("%s: playermove_t->flSwimTime has offset %i, should be 0xAC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->flNextPrimaryAttack));
   if(i != 0xB0) {
      printf("%s: playermove_t->flNextPrimaryAttack has offset %i, should be 0xB0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->effects));
   if(i != 0xB4) {
      printf("%s: playermove_t->effects has offset %i, should be 0xB4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->flags));
   if(i != 0xB8) {
      printf("%s: playermove_t->flags has offset %i, should be 0xB8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->usehull));
   if(i != 0xBC) {
      printf("%s: playermove_t->usehull has offset %i, should be 0xBC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->gravity));
   if(i != 0xC0) {
      printf("%s: playermove_t->gravity has offset %i, should be 0xC0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->friction));
   if(i != 0xC4) {
      printf("%s: playermove_t->friction has offset %i, should be 0xC4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->oldbuttons));
   if(i != 0xC8) {
      printf("%s: playermove_t->oldbuttons has offset %i, should be 0xC8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->waterjumptime));
   if(i != 0xCC) {
      printf("%s: playermove_t->waterjumptime has offset %i, should be 0xCC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->dead));
   if(i != 0xD0) {
      printf("%s: playermove_t->dead has offset %i, should be 0xD0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->deadflag));
   if(i != 0xD4) {
      printf("%s: playermove_t->deadflag has offset %i, should be 0xD4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->spectator));
   if(i != 0xD8) {
      printf("%s: playermove_t->spectator has offset %i, should be 0xD8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->movetype));
   if(i != 0xDC) {
      printf("%s: playermove_t->movetype has offset %i, should be 0xDC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->onground));
   if(i != 0xE0) {
      printf("%s: playermove_t->onground has offset %i, should be 0xE0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->waterlevel));
   if(i != 0xE4) {
      printf("%s: playermove_t->waterlevel has offset %i, should be 0xE4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->watertype));
   if(i != 0xE8) {
      printf("%s: playermove_t->watertype has offset %i, should be 0xE8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->oldwaterlevel));
   if(i != 0xEC) {
      printf("%s: playermove_t->oldwaterlevel has offset %i, should be 0xEC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->sztexturename));
   if(i != 0xF0) {
      printf("%s: playermove_t->sztexturename has offset %i, should be 0xF0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->chtexturetype));
   if(i != 0x1F0) {
      printf("%s: playermove_t->chtexturetype has offset %i, should be 0x1F0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->maxspeed));
   if(i != 0x1F4) {
      printf("%s: playermove_t->maxspeed has offset %i, should be 0x1F4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->clientmaxspeed));
   if(i != 0x1F8) {
      printf("%s: playermove_t->clientmaxspeed has offset %i, should be 0x1F8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->iuser1));
   if(i != 0x1FC) {
      printf("%s: playermove_t->iuser1 has offset %i, should be 0x1FC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->iuser2));
   if(i != 0x200) {
      printf("%s: playermove_t->iuser2 has offset %i, should be 0x200.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->iuser3));
   if(i != 0x204) {
      printf("%s: playermove_t->iuser3 has offset %i, should be 0x204.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->iuser4));
   if(i != 0x208) {
      printf("%s: playermove_t->iuser4 has offset %i, should be 0x208.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->fuser1));
   if(i != 0x20C) {
      printf("%s: playermove_t->fuser1 has offset %i, should be 0x20C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->fuser2));
   if(i != 0x210) {
      printf("%s: playermove_t->fuser2 has offset %i, should be 0x210.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->fuser3));
   if(i != 0x214) {
      printf("%s: playermove_t->fuser3 has offset %i, should be 0x214.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->fuser4));
   if(i != 0x218) {
      printf("%s: playermove_t->fuser4 has offset %i, should be 0x218.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->vuser1));
   if(i != 0x21C) {
      printf("%s: playermove_t->vuser1 has offset %i, should be 0x21C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->vuser2));
   if(i != 0x228) {
      printf("%s: playermove_t->vuser2 has offset %i, should be 0x228.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->vuser3));
   if(i != 0x234) {
      printf("%s: playermove_t->vuser3 has offset %i, should be 0x234.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->vuser4));
   if(i != 0x240) {
      printf("%s: playermove_t->vuser4 has offset %i, should be 0x240.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->numphysent));
   if(i != 0x24C) {
      printf("%s: playermove_t->numphysent has offset %i, should be 0x24C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->physents));
   if(i != 0x250) {
      printf("%s: playermove_t->physents has offset %i, should be 0x250.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->nummoveent));
   if(i != 0x20F50) {
      printf("%s: playermove_t->nummoveent has offset %i, should be 0x20F50.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->moveents));
   if(i != 0x20F54) {
      printf("%s: playermove_t->moveents has offset %i, should be 0x20F54.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->numvisent));
   if(i != 0x24754) {
      printf("%s: playermove_t->numvisent has offset %i, should be 0x24754.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->visents));
   if(i != 0x24758) {
      printf("%s: playermove_t->visents has offset %i, should be 0x24758.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->cmd));
   if(i != 0x45458) {
      printf("%s: playermove_t->cmd has offset %i, should be 0x45458.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->numtouch));
   if(i != 0x4548C) {
      printf("%s: playermove_t->numtouch has offset %i, should be 0x4548C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->touchindex));
   if(i != 0x45490) {
      printf("%s: playermove_t->touchindex has offset %i, should be 0x45490.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->physinfo));
   if(i != 0x4F3F0) {
      printf("%s: playermove_t->physinfo has offset %i, should be 0x4F3F0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->movevars));
   if(i != 0x4F4F0) {
      printf("%s: playermove_t->movevars has offset %i, should be 0x4F4F0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->player_mins));
   if(i != 0x4F4F4) {
      printf("%s: playermove_t->player_mins has offset %i, should be 0x4F4F4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->player_maxs));
   if(i != 0x4F524) {
      printf("%s: playermove_t->player_maxs has offset %i, should be 0x4F524.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_Info_ValueForKey));
   if(i != 0x4F554) {
      printf("%s: playermove_t->PM_Info_ValueForKey() has offset %i, should be 0x4F554.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_Particle));
   if(i != 0x4F558) {
      printf("%s: playermove_t->PM_Particle() has offset %i, should be 0x4F558.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TestPlayerPosition));
   if(i != 0x4F55C) {
      printf("%s: playermove_t->PM_TestPlayerPosition() has offset %i, should be 0x4F55C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->Con_NPrintf));
   if(i != 0x4F560) {
      printf("%s: playermove_t->Con_NPrintf() has offset %i, should be 0x4F560.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->Con_DPrintf));
   if(i != 0x4F564) {
      printf("%s: playermove_t->Con_DPrintf() has offset %i, should be 0x4F5684.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->Con_Printf));
   if(i != 0x4F568) {
      printf("%s: playermove_t->Con_Printf() has offset %i, should be 0x4F568.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->Sys_FloatTime));
   if(i != 0x4F56C) {
      printf("%s: playermove_t->Sys_FloatTime() has offset %i, should be 0x4F56C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_StuckTouch));
   if(i != 0x4F570) {
      printf("%s: playermove_t->PM_StuckTouch() has offset %i, should be 0x4F570.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_PointContents));
   if(i != 0x4F574) {
      printf("%s: playermove_t->PM_PointContents() has offset %i, should be 0x4F574.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TruePointContents));
   if(i != 0x4F578) {
      printf("%s: playermove_t->PM_TruePointContents() has offset %i, should be 0x4F578.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_HullPointContents));
   if(i != 0x4F57C) {
      printf("%s: playermove_t->PM_HullPointContents() has offset %i, should be 0x4F57C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_PlayerTrace));
   if(i != 0x4F580) {
      printf("%s: playermove_t->PM_PlayerTrace() has offset %i, should be 0x4F580.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TraceLine));
   if(i != 0x4F584) {
      printf("%s: playermove_t->PM_TraceLine() has offset %i, should be 0x4F584.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->RandomLong));
   if(i != 0x4F588) {
      printf("%s: playermove_t->RandomLong() has offset %i, should be 0x4F588.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->RandomFloat));
   if(i != 0x4F58C) {
      printf("%s: playermove_t->RandomFloat() has offset %i, should be 0x4F58C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_GetModelType));
   if(i != 0x4F590) {
      printf("%s: playermove_t->PM_GetModelType() has offset %i, should be 0x4F590.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_GetModelBounds));
   if(i != 0x4F594) {
      printf("%s: playermove_t->PM_GetModelBounds() has offset %i, should be 0x4F594.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_HullForBsp));
   if(i != 0x4F598) {
      printf("%s: playermove_t->PM_HullForBsp() has offset %i, should be 0x4F598.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TraceModel));
   if(i != 0x4F59C) {
      printf("%s: playermove_t->PM_TraceModel() has offset %i, should be 0x4F59C.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->COM_FileSize));
   if(i != 0x4F5A0) {
      printf("%s: playermove_t->COM_FileSize() has offset %i, should be 0x4F5A0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->COM_LoadFile));
   if(i != 0x4F5A4) {
      printf("%s: playermove_t->COM_LoadFile() has offset %i, should be 0x4F5A4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->COM_FreeFile));
   if(i != 0x4F5A8) {
      printf("%s: playermove_t->COM_FreeFile() has offset %i, should be 0x4F5A8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->memfgets));
   if(i != 0x4F5AC) {
      printf("%s: playermove_t->memfgets() has offset %i, should be 0x4F5Ac.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->runfuncs));
   if(i != 0x4F5B0) {
      printf("%s: playermove_t->runfuncs has offset %i, should be 0x4F5B0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_PlaySound));
   if(i != 0x4F5B4) {
      printf("%s: playermove_t->PM_PlaySound() has offset %i, should be 0x4F5B4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TraceTexture));
   if(i != 0x4F5B8) {
      printf("%s: playermove_t->PM_TraceTexture() has offset %i, should be 0x4F5B8.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_PlaybackEventFull));
   if(i != 0x4F5BC) {
      printf("%s: playermove_t->PM_PlaybackEventFull() has offset %i, should be 0x4F5BC.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_PlayerTraceEx));
   if(i != 0x4F5C0) {
      printf("%s: playermove_t->PM_PlayerTraceEx() has offset %i, should be 0x4F5B0.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TestPlayerPositionEx));
   if(i != 0x4F5C4) {
      printf("%s: playermove_t->PM_TestPlayerPositionEx() has offset %i, should be 0x4F5C4.\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((playermove_t *)0)->PM_TraceLineEx));
   if(i != 0x4F5C8) {
      printf("%s: playermove_t->PM_TraceLineEx() has offset %i, should be 0x4F5C8.\n", __FUNCTION__, i);
      ret = 0;
   }

   //Now for edicts and entvars
   i = sizeof(edict_t);
   if(i != 0x324) {
      printf("%s: sizeof(edict_t) was 0x%X, should be 0x320.\n", __FUNCTION__, i);
      ret = 0;
   }

   i = ((int)&(((edict_t *)0)->free));
   if(i != 0x0) {
      printf("%s: edict_t->free has offset 0x%X, should be 0x0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->serialnumber));
   if(i != 0x4) {
      printf("%s: edict_t->serialnumber has offset 0x%X, should be 0x4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->area));
   if(i != 0x8) {
      printf("%s: edict_t->area has offset 0x%X, should be 0x8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->headnode));
   if(i != 0x10) {
      printf("%s: edict_t->headnode has offset 0x%X, should be 0x10\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->num_leafs));
   if(i != 0x14) {
      printf("%s: edict_t->num_leafs has offset 0x%X, should be 0x14\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->leafnums));
   if(i != 0x18) {
      printf("%s: edict_t->leafnums has offset 0x%X, should be 0x18\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->freetime));
   if(i != 0x78) {
      printf("%s: edict_t->freetime has offset 0x%X, should be 0x78\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->pvPrivateData));
   if(i != 0x7c) {
      printf("%s: edict_t->pvPrivateData has offset 0x%X, should be 0x7c\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((edict_t *)0)->v));
   if(i != 0x80) {
      printf("%s: edict_t->v has offset 0x%X, should be 0x80\n", __FUNCTION__, i);
      ret = 0;
   }

   i = sizeof(entvars_t);
   if(i != 0x2A4) {
      printf("%s: sizeof(entvars_t) was 0x%X, should be 0x2A4.\n", __FUNCTION__, i);
      ret = 0;
   }

   i = ((int)&(((entvars_t *)0)->classname));
   if(i != 0x0) {
      printf("%s: entvars_t->classname has offset 0x%X, should be 0x0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->globalname));
   if(i != 0x4) {
      printf("%s: entvars_t->globalname has offset 0x%X, should be 0x4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->origin));
   if(i != 0x8) {
      printf("%s: entvars_t->origin has offset 0x%X, should be 0x8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->oldorigin));
   if(i != 0x14) {
      printf("%s: entvars_t->oldorigin has offset 0x%X, should be 0x14\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->velocity));
   if(i != 0x20) {
      printf("%s: entvars_t->velocity has offset 0x%X, should be 0x20\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->basevelocity));
   if(i != 0x2C) {
      printf("%s: entvars_t->basevelocity has offset 0x%X, should be 0x2C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->clbasevelocity));
   if(i != 0x38) {
      printf("%s: entvars_t->clbasevelocity has offset 0x%X, should be 0x38\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->movedir));
   if(i != 0x44) {
      printf("%s: entvars_t->movedir has offset 0x%X, should be 0x44\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->angles));
   if(i != 0x50) {
      printf("%s: entvars_t->angles has offset 0x%X, should be 0x50\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->avelocity));
   if(i != 0x5C) {
      printf("%s: entvars_t->avelocity has offset 0x%X, should be 0x5C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->punchangle));
   if(i != 0x68) {
      printf("%s: entvars_t->punchangle has offset 0x%X, should be 0x68\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->v_angle));
   if(i != 0x74) {
      printf("%s: entvars_t->v_angle has offset 0x%X, should be 0x74\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->endpos));
   if(i != 0x80) {
      printf("%s: entvars_t->endpos has offset 0x%X, should be 0x80\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->startpos));
   if(i != 0x8C) {
      printf("%s: entvars_t->startpos has offset 0x%X, should be 0x8C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->impacttime));
   if(i != 0x98) {
      printf("%s: entvars_t->impacttime has offset 0x%X, should be 0x98\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->starttime));
   if(i != 0x9C) {
      printf("%s: entvars_t->starttime has offset 0x%X, should be 0x9C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->fixangle));
   if(i != 0xA0) {
      printf("%s: entvars_t->fixangle has offset 0x%X, should be 0xA0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->idealpitch));
   if(i != 0xA4) {
      printf("%s: entvars_t->idealpitch has offset 0x%X, should be 0xA4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->pitch_speed));
   if(i != 0xA8) {
      printf("%s: entvars_t->pitch_speed has offset 0x%X, should be 0xA8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->ideal_yaw));
   if(i != 0xAC) {
      printf("%s: entvars_t->ideal_yaw has offset 0x%X, should be 0xAC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->yaw_speed));
   if(i != 0xB0) {
      printf("%s: entvars_t->yaw_speed has offset 0x%X, should be 0xB0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->modelindex));
   if(i != 0xB4) {
      printf("%s: entvars_t->modelindex has offset 0x%X, should be 0xB4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->model));
   if(i != 0xB8) {
      printf("%s: entvars_t->model has offset 0x%X, should be 0xB8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->viewmodel));
   if(i != 0xBC) {
      printf("%s: entvars_t->viewmodel has offset 0x%X, should be 0xBC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->weaponmodel));
   if(i != 0xC0) {
      printf("%s: entvars_t->weaponmodel has offset 0x%X, should be 0xC0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->absmin));
   if(i != 0xC4) {
      printf("%s: entvars_t->absmin has offset 0x%X, should be 0xC4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->absmax));
   if(i != 0xD0) {
      printf("%s: entvars_t->absmax has offset 0x%X, should be 0xD0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->mins));
   if(i != 0xDC) {
      printf("%s: entvars_t->mins has offset 0x%X, should be 0xDC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->maxs));
   if(i != 0xE8) {
      printf("%s: entvars_t->maxs has offset 0x%X, should be 0xE8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->size));
   if(i != 0xF4) {
      printf("%s: entvars_t->size has offset 0x%X, should be 0xF4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->ltime));
   if(i != 0x100) {
      printf("%s: entvars_t->ltime has offset 0x%X, should be 0x100\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->nextthink));
   if(i != 0x104) {
      printf("%s: entvars_t->nextthink has offset 0x%X, should be 0x104\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->movetype));
   if(i != 0x108) {
      printf("%s: entvars_t->movetype has offset 0x%X, should be 0x108\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->solid));
   if(i != 0x10C) {
      printf("%s: entvars_t->solid has offset 0x%X, should be 0x10C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->skin));
   if(i != 0x110) {
      printf("%s: entvars_t->skin has offset 0x%X, should be 0x110\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->body));
   if(i != 0x114) {
      printf("%s: entvars_t->body has offset 0x%X, should be 0x114\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->effects));
   if(i != 0x118) {
      printf("%s: entvars_t->effects has offset 0x%X, should be 0x118\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->gravity));
   if(i != 0x11C) {
      printf("%s: entvars_t->gravity has offset 0x%X, should be 0x11C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->friction));
   if(i != 0x120) {
      printf("%s: entvars_t->friction has offset 0x%X, should be 0x120\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->light_level));
   if(i != 0x124) {
      printf("%s: entvars_t->light_level has offset 0x%X, should be 0x124\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->sequence));
   if(i != 0x128) {
      printf("%s: entvars_t->sequence has offset 0x%X, should be 0x128\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->gaitsequence));
   if(i != 0x12C) {
      printf("%s: entvars_t->gaitsequence has offset 0x%X, should be 0x12C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->frame));
   if(i != 0x130) {
      printf("%s: entvars_t->frame has offset 0x%X, should be 0x130\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->animtime));
   if(i != 0x134) {
      printf("%s: entvars_t->animtime has offset 0x%X, should be 0x134\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->framerate));
   if(i != 0x138) {
      printf("%s: entvars_t->framerate has offset 0x%X, should be 0x138\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->controller));
   if(i != 0x13C) {
      printf("%s: entvars_t->controller has offset 0x%X, should be 0x13C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->blending));
   if(i != 0x140) {
      printf("%s: entvars_t->blending has offset 0x%X, should be 0x140\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->scale));
   if(i != 0x144) {
      printf("%s: entvars_t->scale has offset 0x%X, should be 0x144\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->rendermode));
   if(i != 0x148) {
      printf("%s: entvars_t->rendermode has offset 0x%X, should be 0x148\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->renderamt));
   if(i != 0x14C) {
      printf("%s: entvars_t->renderamt has offset 0x%X, should be 0x14C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->rendercolor));
   if(i != 0x150) {
      printf("%s: entvars_t->rendercolor has offset 0x%X, should be 0x150\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->renderfx));
   if(i != 0x15C) {
      printf("%s: entvars_t->renderfx has offset 0x%X, should be 0x15C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->health));
   if(i != 0x160) {
      printf("%s: entvars_t->health has offset 0x%X, should be 0x160\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->frags));
   if(i != 0x164) {
      printf("%s: entvars_t->frags has offset 0x%X, should be 0x164\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->weapons));
   if(i != 0x168) {
      printf("%s: entvars_t->weapons has offset 0x%X, should be 0x168\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->takedamage));
   if(i != 0x16C) {
      printf("%s: entvars_t->takedamage has offset 0x%X, should be 0x16C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->deadflag));
   if(i != 0x170) {
      printf("%s: entvars_t->deadflag has offset 0x%X, should be 0x170\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->view_ofs));
   if(i != 0x174) {
      printf("%s: entvars_t->view_ofs has offset 0x%X, should be 0x174\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->button));
   if(i != 0x180) {
      printf("%s: entvars_t->button has offset 0x%X, should be 0x180\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->impulse));
   if(i != 0x184) {
      printf("%s: entvars_t->impulse has offset 0x%X, should be 0x184\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->chain));
   if(i != 0x188) {
      printf("%s: entvars_t->chain has offset 0x%X, should be 0x188\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->dmg_inflictor));
   if(i != 0x18C) {
      printf("%s: entvars_t->dmg_inflictor has offset 0x%X, should be 0x18C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->enemy));
   if(i != 0x190) {
      printf("%s: entvars_t->enemy has offset 0x%X, should be 0x190\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->aiment));
   if(i != 0x194) {
      printf("%s: entvars_t->aiment has offset 0x%X, should be 0x194\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->owner));
   if(i != 0x198) {
      printf("%s: entvars_t->owner has offset 0x%X, should be 0x198\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->groundentity));
   if(i != 0x19C) {
      printf("%s: entvars_t->groundentity has offset 0x%X, should be 0x19C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->spawnflags));
   if(i != 0x1A0) {
      printf("%s: entvars_t->spawnflags has offset 0x%X, should be 0x1A0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->flags));
   if(i != 0x1A4) {
      printf("%s: entvars_t->flags has offset 0x%X, should be 0x1A4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->colormap));
   if(i != 0x1A8) {
      printf("%s: entvars_t->colormap has offset 0x%X, should be 0x1A8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->team));
   if(i != 0x1AC) {
      printf("%s: entvars_t->team has offset 0x%X, should be 0x1AC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->max_health));
   if(i != 0x1B0) {
      printf("%s: entvars_t->max_health has offset 0x%X, should be 0x1B0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->teleport_time));
   if(i != 0x1B4) {
      printf("%s: entvars_t->teleport_time has offset 0x%X, should be 0x1B4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->armortype));
   if(i != 0x1B8) {
      printf("%s: entvars_t->armortype has offset 0x%X, should be 0x1B8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->armorvalue));
   if(i != 0x1BC) {
      printf("%s: entvars_t->armorvalue has offset 0x%X, should be 0x1BC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->waterlevel));
   if(i != 0x1C0) {
      printf("%s: entvars_t->waterlevel has offset 0x%X, should be 0x1C0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->watertype));
   if(i != 0x1C4) {
      printf("%s: entvars_t->watertype has offset 0x%X, should be 0x1C4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->target));
   if(i != 0x1C8) {
      printf("%s: entvars_t->target has offset 0x%X, should be 0x1C8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->targetname));
   if(i != 0x1CC) {
      printf("%s: entvars_t->targetname has offset 0x%X, should be 0x1CC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->netname));
   if(i != 0x1D0) {
      printf("%s: entvars_t->netname has offset 0x%X, should be 0x1D0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->message));
   if(i != 0x1D4) {
      printf("%s: entvars_t->message has offset 0x%X, should be 0x1D4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->dmg_take));
   if(i != 0x1D8) {
      printf("%s: entvars_t->dmg_take has offset 0x%X, should be 0x1D8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->dmg_save));
   if(i != 0x1DC) {
      printf("%s: entvars_t->dmg_save has offset 0x%X, should be 0x1DC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->dmg));
   if(i != 0x1E0) {
      printf("%s: entvars_t->dmg has offset 0x%X, should be 0x1E0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->dmgtime));
   if(i != 0x1E4) {
      printf("%s: entvars_t->dmgtime has offset 0x%X, should be 0x1E4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->noise));
   if(i != 0x1E8) {
      printf("%s: entvars_t->noise has offset 0x%X, should be 0x1E8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->noise1));
   if(i != 0x1EC) {
      printf("%s: entvars_t->noise1 has offset 0x%X, should be 0x1EC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->noise2));
   if(i != 0x1F0) {
      printf("%s: entvars_t->noise2 has offset 0x%X, should be 0x1F0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->noise3));
   if(i != 0x1F4) {
      printf("%s: entvars_t->noise3 has offset 0x%X, should be 0x1F4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->speed));
   if(i != 0x1F8) {
      printf("%s: entvars_t->speed has offset 0x%X, should be 0x1F8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->air_finished));
   if(i != 0x1FC) {
      printf("%s: entvars_t->air_finished has offset 0x%X, should be 0x1FC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->pain_finished));
   if(i != 0x200) {
      printf("%s: entvars_t->pain_finished has offset 0x%X, should be 0x200\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->radsuit_finished));
   if(i != 0x204) {
      printf("%s: entvars_t->radsuit_finished has offset 0x%X, should be 0x204\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->pContainingEntity));
   if(i != 0x208) {
      printf("%s: entvars_t->pContainingEntity has offset 0x%X, should be 0x208\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->playerclass));
   if(i != 0x20C) {
      printf("%s: entvars_t->playerclass has offset 0x%X, should be 0x20C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->maxspeed));
   if(i != 0x210) {
      printf("%s: entvars_t->maxspeed has offset 0x%X, should be 0x210\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->fov));
   if(i != 0x214) {
      printf("%s: entvars_t->fov has offset 0x%X, should be 0x214\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->weaponanim));
   if(i != 0x218) {
      printf("%s: entvars_t->weaponanim has offset 0x%X, should be 0x218\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->pushmsec));
   if(i != 0x21C) {
      printf("%s: entvars_t->pushmsec has offset 0x%X, should be 0x21C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->bInDuck));
   if(i != 0x220) {
      printf("%s: entvars_t->bInDuck has offset 0x%X, should be 0x220\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->flTimeStepSound));
   if(i != 0x224) {
      printf("%s: entvars_t-> has offset 0x%X, should be 0x224\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->flSwimTime));
   if(i != 0x228) {
      printf("%s: entvars_t->flTimeStepSound has offset 0x%X, should be 0x228\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->flDuckTime));
   if(i != 0x22C) {
      printf("%s: entvars_t->flDuckTime has offset 0x%X, should be 0x22C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->iStepLeft));
   if(i != 0x230) {
      printf("%s: entvars_t->iStepLeft has offset 0x%X, should be 0x230\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->flFallVelocity));
   if(i != 0x234) {
      printf("%s: entvars_t->flFallVelocity has offset 0x%X, should be 0x234\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->gamestate));
   if(i != 0x238) {
      printf("%s: entvars_t->gamestate has offset 0x%X, should be 0x238\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->oldbuttons));
   if(i != 0x23C) {
      printf("%s: entvars_t->oldbuttons has offset 0x%X, should be 0x23C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->groupinfo));
   if(i != 0x240) {
      printf("%s: entvars_t->groupinfo has offset 0x%X, should be 0x240\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->iuser1));
   if(i != 0x244) {
      printf("%s: entvars_t->iuser1 has offset 0x%X, should be 0x244\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->iuser2));
   if(i != 0x248) {
      printf("%s: entvars_t->iuser2 has offset 0x%X, should be 0x248\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->iuser3));
   if(i != 0x24C) {
      printf("%s: entvars_t->iuser3 has offset 0x%X, should be 0x24C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->iuser4));
   if(i != 0x250) {
      printf("%s: entvars_t->iuser4 has offset 0x%X, should be 0x250\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->fuser1));
   if(i != 0x254) {
      printf("%s: entvars_t->fuser1 has offset 0x%X, should be 0x254\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->fuser2));
   if(i != 0x258) {
      printf("%s: entvars_t->fuser2 has offset 0x%X, should be 0x258\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->fuser3));
   if(i != 0x25C) {
      printf("%s: entvars_t->fuser3 has offset 0x%X, should be 0x25C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->fuser4));
   if(i != 0x260) {
      printf("%s: entvars_t->fuser4 has offset 0x%X, should be 0x260\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->vuser1));
   if(i != 0x264) {
      printf("%s: entvars_t->vuser1 has offset 0x%X, should be 0x264\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->vuser2));
   if(i != 0x270) {
      printf("%s: entvars_t->vuser2 has offset 0x%X, should be 0x270\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->vuser3));
   if(i != 0x27C) {
      printf("%s: entvars_t->vuser3 has offset 0x%X, should be 0x27C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->vuser4));
   if(i != 0x288) {
      printf("%s: entvars_t->vuser4 has offset 0x%X, should be 0x288\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->euser1));
   if(i != 0x294) {
      printf("%s: entvars_t->euser1 has offset 0x%X, should be 0x294\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->euser2));
   if(i != 0x298) {
      printf("%s: entvars_t->euser2 has offset 0x%X, should be 0x298\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->euser3));
   if(i != 0x29C) {
      printf("%s: entvars_t->euser3 has offset 0x%X, should be 0x29C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((entvars_t *)0)->euser4));
   if(i != 0x2A0) {
      printf("%s: entvars_t->euser4 has offset 0x%X, should be 0x2A0\n", __FUNCTION__, i);
      ret = 0;
   }

   //gvars.  There shouldn't be any surprises here unless someone tries to redefine vec3_t
   i = sizeof(g_vars_t);
   if(i != 0xAC) {
      printf("%s: sizeof(g_vars_t) was 0x%X, should be 0xAc.\n", __FUNCTION__, i);
      ret = 0;
   }

   i = ((int)&(((g_vars_t *)0)->time));
   if(i != 0x0) {
      printf("%s: g_vars_t->time has offset 0x%X, should be 0x0\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->frametime));
   if(i != 0x4) {
      printf("%s: g_vars_t->frametime has offset 0x%X, should be 0x4\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->force_retouch));
   if(i != 0x8) {
      printf("%s: g_vars_t->force_retouch has offset 0x%X, should be 0x8\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->mapname));
   if(i != 0xC) {
      printf("%s: g_vars_t->mapname has offset 0x%X, should be 0xC\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->startspot));
   if(i != 0x10) {
      printf("%s: g_vars_t->startspot has offset 0x%X, should be 0x10\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->deathmatch));
   if(i != 0x14) {
      printf("%s: g_vars_t->deathmatch has offset 0x%X, should be 0x14\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->coop));
   if(i != 0x18) {
      printf("%s: g_vars_t->coop has offset 0x%X, should be 0x18\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->teamplay));
   if(i != 0x1C) {
      printf("%s: g_vars_t->teamplay has offset 0x%X, should be 0x1C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->serverflags));
   if(i != 0x20) {
      printf("%s: g_vars_t->serverflags has offset 0x%X, should be 0x20\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->found_secrets));
   if(i != 0x24) {
      printf("%s: g_vars_t->found_secrets has offset 0x%X, should be 0x24\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->v_forward));
   if(i != 0x28) {
      printf("%s: g_vars_t->v_forward has offset 0x%X, should be 0x28\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->v_up));
   if(i != 0x34) {
      printf("%s: g_vars_t->v_up has offset 0x%X, should be 0x34\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->v_right));
   if(i != 0x40) {
      printf("%s: g_vars_t->v_right has offset 0x%X, should be 0x40\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_allsolid));
   if(i != 0x4C) {
      printf("%s: g_vars_t->trace_allsolid has offset 0x%X, should be 0x4C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_startsolid));
   if(i != 0x50) {
      printf("%s: g_vars_t->trace_startsolid has offset 0x%X, should be 0x50\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_fraction));
   if(i != 0x54) {
      printf("%s: g_vars_t->trace_fraction has offset 0x%X, should be 0x54\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_endpos));
   if(i != 0x58) {
      printf("%s: g_vars_t->trace_endpos has offset 0x%X, should be 0x58\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_plane_normal));
   if(i != 0x64) {
      printf("%s: g_vars_t->trace_plane_normal has offset 0x%X, should be 0x64\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_plane_dist));
   if(i != 0x70) {
      printf("%s: g_vars_t->trace_plane_dist has offset 0x%X, should be 0x70\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_ent));
   if(i != 0x74) {
      printf("%s: g_vars_t->trace_ent has offset 0x%X, should be 0x74\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_inopen));
   if(i != 0x78) {
      printf("%s: g_vars_t->trace_inopen has offset 0x%X, should be 0x78\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_inwater));
   if(i != 0x7C) {
      printf("%s: g_vars_t->trace_inwater has offset 0x%X, should be 0x7C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_hitgroup));
   if(i != 0x80) {
      printf("%s: g_vars_t->trace_hitgroup has offset 0x%X, should be 0x80\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->trace_flags));
   if(i != 0x84) {
      printf("%s: g_vars_t->trace_flags has offset 0x%X, should be 0x84\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->msg_entity));
   if(i != 0x88) {
      printf("%s: g_vars_t->msg_entity has offset 0x%X, should be 0x88\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->cdAudioTrack));
   if(i != 0x8C) {
      printf("%s: g_vars_t->cdAudioTrack has offset 0x%X, should be 0x8C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->maxClients));
   if(i != 0x90) {
      printf("%s: g_vars_t->maxClients has offset 0x%X, should be 0x90\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->maxEntities));
   if(i != 0x94) {
      printf("%s: g_vars_t->maxEntities has offset 0x%X, should be 0x94\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->pStringBase));
   if(i != 0x98) {
      printf("%s: g_vars_t->pStringBase has offset 0x%X, should be 0x98\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->pSaveData));
   if(i != 0x9C) {
      printf("%s: g_vars_t->pSaveData has offset 0x%X, should be 0x9C\n", __FUNCTION__, i);
      ret = 0;
   }
   i = ((int)&(((g_vars_t *)0)->vecLandmarkOffset));
   if(i != 0xA0) {
      printf("%s: g_vars_t->vecLandmarkOffset has offset 0x%X, should be 0xA0\n", __FUNCTION__, i);
      ret = 0;
   }


   //There are more that ought to be done.  I'll add them as I see them.

   if(ret == 0) {
      printf("\nA structure in your executable is not aligned properly.\n"
             "Struct alignment rules can change between compilers and OSes,\n"
             "but as we have no control over the mod DLL's development,\n"
             "we must emulate their alignment.  Failure to do so WILL cause crashes.\n"
             "Tell the current maintainer what compiler you used, preferrably how he can\n"
             "identify it within the code, and the above error messages, and it'll be added.\n"
             "Or, do it yourself, and upload the changes.  It's open source after all.\n");

      return(0);
   }

   return(1);
}
