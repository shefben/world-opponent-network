/* I'd like to put all code that is related to setting up mods here.
// This'd include the remaining functions in HIJACK.c, as well as what's
// here now, testing to ensure the structures match.
*/

#ifndef OSHLDS_MODDING_HEADER
#define OSHLDS_MODDING_HEADER

/* Calling conventions:  Still haven't figured them out.  There's a line,
// #define DLLEXPORT __stdcall in the SDK, but the disassembly of hl.dll
// disagrees.  It's also possible that its our calling that's wrong
// somewhere.
*/

#include "crc.h"
#include "common.h"
#include "cvar.h"
#include "edict.h"
#include "Mod.h"
#include "ModCalls.h"
#include "NET.h"
#include "report.h"
#include "SV_hlds.h"
#include "world.h"

//Types related to unfinished SDK functions (most likely knowm by other names)
typedef void SAVERESTOREDATA; //It's a large struct but I don't feel like digging it out
typedef void KeyValueData;
typedef void TYPEDESCRIPTION;

typedef struct enginefuncs_s { //sdk, duh.  MODS call these from US.

   HLDS_DLLEXPORT int           (*pfnPrecacheModel)             (char* s);
   HLDS_DLLEXPORT int           (*pfnPrecacheSound)             (char* s);
   HLDS_DLLEXPORT void          (*pfnSetModel)                  (edict_t *e, const char *m);
   HLDS_DLLEXPORT int           (*pfnModelIndex)                (char *m);
   HLDS_DLLEXPORT int           (*pfnModelFrames)               (int modelIndex);
   HLDS_DLLEXPORT void          (*pfnSetSize)                   (edict_t *e, const vec3_t rgflMin, const vec3_t rgflMax);
   HLDS_DLLEXPORT void          (*pfnChangeLevel)               (char* s1, char* s2);
   HLDS_DLLEXPORT void          (*pfnGetSpawnParms)             (edict_t *ent);
   HLDS_DLLEXPORT void          (*pfnSaveSpawnParms)            (edict_t *ent);
   HLDS_DLLEXPORT float         (*pfnVecToYaw)                  (const float *rgflVector);
   HLDS_DLLEXPORT void          (*pfnVecToAngles)               (const float *rgflVectorIn, float *rgflVectorOut);
   HLDS_DLLEXPORT void          (*pfnMoveToOrigin)              (edict_t *ent,const vec3_t pflGoal, float dist, int iMoveType);
   HLDS_DLLEXPORT void          (*pfnChangeYaw)                 (edict_t* ent);
   HLDS_DLLEXPORT void          (*pfnChangePitch)               (edict_t* ent);
   HLDS_DLLEXPORT edict_t *     (*pfnFindEntityByString)        (edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue);
   HLDS_DLLEXPORT int           (*pfnGetEntityIllum)            (edict_t* pEnt);
   HLDS_DLLEXPORT edict_t *     (*pfnFindEntityInSphere)        (edict_t *pEdictStartSearchAfter, const vec3_t org, float rad);
   HLDS_DLLEXPORT edict_t *     (*pfnFindClientInPVS)           (edict_t *pEdict);
   HLDS_DLLEXPORT edict_t *     (*pfnEntitiesInPVS)             (edict_t *pplayer);
   HLDS_DLLEXPORT void          (*pfnMakeVectors)               (const vec3_t rgflVector);
   HLDS_DLLEXPORT void          (*pfnAngleVectors)              (const vec3_t rgflVector, float *forward, float *right, float *up);
   HLDS_DLLEXPORT edict_t *     (*pfnCreateEntity)              (void);
   HLDS_DLLEXPORT void          (*pfnRemoveEntity)              (edict_t* e);
   HLDS_DLLEXPORT edict_t *     (*pfnCreateNamedEntity)         (int className);
   HLDS_DLLEXPORT void          (*pfnMakeStatic)                (edict_t *ent);
   HLDS_DLLEXPORT int           (*pfnEntIsOnFloor)              (edict_t *e);
   HLDS_DLLEXPORT int           (*pfnDropToFloor)               (edict_t* e);
   HLDS_DLLEXPORT int           (*pfnWalkMove)                  (edict_t *ent, float yaw, float dist, int iMode);
   HLDS_DLLEXPORT void          (*pfnSetOrigin)                 (edict_t *e, const vec3_t rgflOrigin);
   HLDS_DLLEXPORT void          (*pfnEmitSound)                 (edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch);
   HLDS_DLLEXPORT void          (*pfnEmitAmbientSound)          (edict_t *entity, vec3_t pos, const char *samp, float vol, float attenuation, int fFlags, int pitch);
   HLDS_DLLEXPORT void          (*pfnTraceLine)                 (vec3_t v1, vec3_t v2, int fNoMonsters, edict_t *pentToSkip, TraceResult_t *ptr);
   HLDS_DLLEXPORT void          (*pfnTraceToss)                 (edict_t* pent, edict_t* pentToIgnore, TraceResult_t *ptr);
   HLDS_DLLEXPORT int           (*pfnTraceMonsterHull)          (edict_t *pEdict, vec3_t v1, vec3_t v2, int fNoMonsters, edict_t *pentToSkip, TraceResult_t *ptr);
   HLDS_DLLEXPORT void          (*pfnTraceHull)                 (vec3_t v1, vec3_t v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult_t *ptr);
   HLDS_DLLEXPORT void          (*pfnTraceModel)                (vec3_t v1, vec3_t v2, int hullNumber, edict_t *pent, TraceResult_t *ptr);
   HLDS_DLLEXPORT const char *  (*pfnTraceTexture)              (edict_t *pTextureEntity, vec3_t v1, vec3_t v2 );
   HLDS_DLLEXPORT void          (*pfnTraceSphere)               (vec3_t v1, vec3_t v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult_t *ptr);
   HLDS_DLLEXPORT void          (*pfnGetAimVector)              (edict_t* ent, float speed, vec3_t rgflReturn);
   HLDS_DLLEXPORT void          (*pfnServerCommand)             (char* str);
   HLDS_DLLEXPORT void          (*pfnServerExecute)             (void);
   HLDS_DLLEXPORT void          (*pfnClientCommand)             (edict_t* pEdict, char* szFmt, ...);
   HLDS_DLLEXPORT void          (*pfnParticleEffect)            (const vec3_t org, const vec3_t dir, float color, float count);
   HLDS_DLLEXPORT void          (*pfnLightStyle)                (int style, char* val);
   HLDS_DLLEXPORT int           (*pfnDecalIndex)                (const char *name);
   HLDS_DLLEXPORT int           (*pfnPointContents)             (const vec3_t rgflVector);
   HLDS_DLLEXPORT void          (*pfnMessageBegin)              (int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);
   HLDS_DLLEXPORT void          (*pfnMessageEnd)                (void);
   HLDS_DLLEXPORT void          (*pfnWriteByte)                 (int iValue);
   HLDS_DLLEXPORT void          (*pfnWriteChar)                 (int iValue);
   HLDS_DLLEXPORT void          (*pfnWriteShort)                (int iValue);
   HLDS_DLLEXPORT void          (*pfnWriteLong)                 (int iValue);
   HLDS_DLLEXPORT void          (*pfnWriteAngle)                (float flValue);
   HLDS_DLLEXPORT void          (*pfnWriteCoord)                (float flValue);
   HLDS_DLLEXPORT void          (*pfnWriteString)               (const char *sz);
   HLDS_DLLEXPORT void          (*pfnWriteEntity)               (int iValue);
   HLDS_DLLEXPORT void          (*pfnCVarRegister)              (cvar_t *pCvar);
   HLDS_DLLEXPORT float         (*pfnCVarGetFloat)              (const char *szVarName);
   HLDS_DLLEXPORT const char *  (*pfnCVarGetString)             (const char *szVarName);
   HLDS_DLLEXPORT void          (*pfnCVarSetFloat)              (const char *szVarName, float flValue);
   HLDS_DLLEXPORT void          (*pfnCVarSetString)             (const char *szVarName, const char *szValue);
   HLDS_DLLEXPORT void          (*pfnAlertMessage)              (ALERT_TYPE atype, char *szFmt, ...);
   HLDS_DLLEXPORT void          (*pfnEngineFprintf)             (FILE *pfile, char *szFmt, ...);
   HLDS_DLLEXPORT void *        (*pfnPvAllocEntPrivateData)     (edict_t *pEdict, long cb);
   HLDS_DLLEXPORT void *        (*pfnPvEntPrivateData)          (edict_t *pEdict);
   HLDS_DLLEXPORT void          (*pfnFreeEntPrivateData)        (edict_t *pEdict);
   HLDS_DLLEXPORT const char *  (*pfnSzFromIndex)               (int iString);
   HLDS_DLLEXPORT int           (*pfnAllocString)               (const char *szValue);
   HLDS_DLLEXPORT entvars_t *   (*pfnGetVarsOfEnt)              (edict_t *pEdict);
   HLDS_DLLEXPORT edict_t *     (*pfnPEntityOfEntOffset)        (int iEntOffset);
   HLDS_DLLEXPORT int           (*pfnEntOffsetOfPEntity)        (const edict_t *pEdict);
   HLDS_DLLEXPORT int           (*pfnIndexOfEdict)              (const edict_t *pEdict);
   HLDS_DLLEXPORT edict_t *     (*pfnPEntityOfEntIndex)         (int iEntIndex);
   HLDS_DLLEXPORT edict_t *     (*pfnFindEntityByVars)          (struct entvars_s* pvars);
   HLDS_DLLEXPORT void *        (*pfnGetModelPtr)               (edict_t* pEdict);
   HLDS_DLLEXPORT int           (*pfnRegUserMsg)                (const char *pszName, int iSize);
   HLDS_DLLEXPORT void          (*pfnAnimationAutomove)         (const edict_t* pEdict, float flTime);
   HLDS_DLLEXPORT void          (*pfnGetBonePosition)           (const edict_t* pEdict, int iBone, float *rgflOrigin, float *rgflAngles);
   HLDS_DLLEXPORT void *        (*pfnFunctionFromName)          (const char *pName);
   HLDS_DLLEXPORT const char *  (*pfnNameForFunction)           (void * function);
   HLDS_DLLEXPORT void          (*pfnClientPrintf)              (edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg); // JOHN: engine callbacks so game DLL can print messages to individual clients
   HLDS_DLLEXPORT void          (*pfnServerPrint)               (const char *szMsg);
   HLDS_DLLEXPORT const char *  (*pfnCmd_Args)                  (void);      // these 3 added
   HLDS_DLLEXPORT const char *  (*pfnCmd_Argv)                  (int argc);   // so game DLL can easily
   HLDS_DLLEXPORT int           (*pfnCmd_Argc)                  (void);      // access client 'cmd' strings
   HLDS_DLLEXPORT void          (*pfnGetAttachment)             (const edict_t *pEdict, int iAttachment, vec3_t rgflOrigin, vec3_t rgflAngles );
   HLDS_DLLEXPORT void          (*pfnCRC32_Init)                (CRC32_t *pulCRC);
   HLDS_DLLEXPORT void          (*pfnCRC32_ProcessBuffer)       (CRC32_t *pulCRC, void *p, int len);
   HLDS_DLLEXPORT void          (*pfnCRC32_ProcessByte)         (CRC32_t *pulCRC, unsigned char ch);
   HLDS_DLLEXPORT CRC32_t       (*pfnCRC32_Final)               (CRC32_t pulCRC);
   HLDS_DLLEXPORT long          (*pfnRandomLong)                (long lLow, long lHigh);
   HLDS_DLLEXPORT float         (*pfnRandomFloat)               (float flLow, float flHigh);
   HLDS_DLLEXPORT void          (*pfnSetView)                   (const edict_t *pClient, const edict_t *pViewent);
   HLDS_DLLEXPORT float         (*pfnTime)                      (void);
   HLDS_DLLEXPORT void          (*pfnCrosshairAngle)            (const edict_t *pClient, float pitch, float yaw);
   HLDS_DLLEXPORT byte *        (*pfnLoadFileForMe)             (const char *filename, int *pLength);
   HLDS_DLLEXPORT void          (*pfnFreeFile)                  (byte *buffer);
   HLDS_DLLEXPORT void          (*pfnEndSection)                (const char *pszSectionName); // trigger_endsection
   HLDS_DLLEXPORT int           (*pfnCompareFileTime)           (char *filename1, char *filename2, int *iCompare);
   HLDS_DLLEXPORT void          (*pfnGetGameDir)                (char *szGetGameDir);
   HLDS_DLLEXPORT void          (*pfnCvar_RegisterVariable)     (cvar_t *variable);
   HLDS_DLLEXPORT void          (*pfnFadeClientVolume)          (const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds);
   HLDS_DLLEXPORT void          (*pfnSetClientMaxspeed)         (edict_t *pEdict, float fNewMaxspeed); //It lies.  We are changing the entvars, which is part of the edict, and therefore the edict is not constant.
   HLDS_DLLEXPORT edict_t *     (*pfnCreateFakeClient)          (const char *netname);   // returns NULL if fake client can't be created
   HLDS_DLLEXPORT void          (*pfnRunPlayerMove)             (edict_t * fakeclient, const vec3_t viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec);
   HLDS_DLLEXPORT int           (*pfnNumberOfEntities)          (void);
   HLDS_DLLEXPORT char *        (*pfnGetInfoKeyBuffer)          (edict_t *e);   // passing in NULL gets the serverinfo
   HLDS_DLLEXPORT char *        (*pfnInfoKeyValue)              (char *infobuffer, char *key);
   HLDS_DLLEXPORT void          (*pfnSetKeyValue)               (char *infobuffer, char *key, char *value);
   HLDS_DLLEXPORT void          (*pfnSetClientKeyValue)         (int clientIndex, char *infobuffer, char *key, char *value);
   HLDS_DLLEXPORT int           (*pfnIsMapValid)                (char *filename);
   HLDS_DLLEXPORT void          (*pfnStaticDecal)               (const float *origin, int decalIndex, int entityIndex, int modelIndex);
   HLDS_DLLEXPORT int           (*pfnPrecacheGeneric)           (char* s);
   HLDS_DLLEXPORT int           (*pfnGetPlayerUserId)           (edict_t *e); // returns the server assigned userid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients
   HLDS_DLLEXPORT void          (*pfnBuildSoundMsg)             (edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const vec3_t pOrigin, edict_t *ed);
   HLDS_DLLEXPORT int           (*pfnIsDedicatedServer)         (void);// is this a dedicated server?
   HLDS_DLLEXPORT cvar_t *      (*pfnCVarGetPointer)            (const char *szVarName);
   HLDS_DLLEXPORT unsigned int  (*pfnGetPlayerWONId)            (edict_t *e); // returns the server assigned WONid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients

   // YWB 8/1/99 TFF Physics additions
   HLDS_DLLEXPORT void            (*pfnInfo_RemoveKey)          (char *s, const char *key);
   HLDS_DLLEXPORT const char *    (*pfnGetPhysicsKeyValue)      (const edict_t *pClient, const char *key);
   HLDS_DLLEXPORT void            (*pfnSetPhysicsKeyValue)      (const edict_t *pClient, const char *key, const char *value);
   HLDS_DLLEXPORT const char *    (*pfnGetPhysicsInfoString)    (const edict_t *pClient);
   HLDS_DLLEXPORT unsigned short  (*pfnPrecacheEvent)           (int type, const char*psz);
   HLDS_DLLEXPORT void            (*pfnPlaybackEvent)           (int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);

   HLDS_DLLEXPORT unsigned char * (*pfnSetFatPVS)               (vec3_t org);
   HLDS_DLLEXPORT unsigned char * (*pfnSetFatPAS)               (vec3_t org);

   HLDS_DLLEXPORT int             (*pfnCheckVisibility )        (const edict_t *entity, unsigned char *pset);

   HLDS_DLLEXPORT void            (*pfnDeltaSetField)           (struct sv_builddelta_s *pFields, const char *fieldname);
   HLDS_DLLEXPORT void            (*pfnDeltaUnsetField)         (struct sv_builddelta_s *pFields, const char *fieldname);
   HLDS_DLLEXPORT void            (*pfnDeltaAddEncoder)         (char *name, void (*conditionalencode)(struct sv_builddelta_s *pFields, const unsigned char *from, const unsigned char *to));
   HLDS_DLLEXPORT int             (*pfnGetCurrentPlayer)        (void);
   HLDS_DLLEXPORT int             (*pfnCanSkipPlayer)           (const edict_t *player);
   HLDS_DLLEXPORT int             (*pfnDeltaFindField)          (struct sv_builddelta_s *pFields, const char *fieldname);
   HLDS_DLLEXPORT void            (*pfnDeltaSetFieldByIndex)    (struct sv_builddelta_s *pFields, int fieldNumber);
   HLDS_DLLEXPORT void            (*pfnDeltaUnsetFieldByIndex)  (struct sv_builddelta_s *pFields, int fieldNumber);

   HLDS_DLLEXPORT void            (*pfnSetGroupMask)            (int mask, int op);

   HLDS_DLLEXPORT int             (*pfnCreateInstancedBaseline) (int classname, entity_state_t *baseline);
   HLDS_DLLEXPORT void            (*pfnCvar_DirectSet)          (struct cvar_s *var, char *value);

   // Forces the client and server to be running with the same version of the specified file
   //  ( e.g., a player model ).
   // Calling this has no effect in single player
   HLDS_DLLEXPORT void            (*pfnForceUnmodified)         (FORCE_TYPE type, vec3_t mins, vec3_t maxs, const char *filename);
   HLDS_DLLEXPORT void            (*pfnGetPlayerStats)          (const edict_t *pClient, int *ping, int *packet_loss);
   HLDS_DLLEXPORT void            (*pfnAddServerCommand)        (char *cmd_name, void (*function) (void));

   // For voice communications, set which clients hear eachother.
   // NOTE: these functions take player entity indices (starting at 1).
   HLDS_DLLEXPORT qboolean        (*pfnVoice_GetClientListening) (int iReceiver, int iSender);
   HLDS_DLLEXPORT qboolean        (*pfnVoice_SetClientListening) (int iReceiver, int iSender, qboolean bListen);
   HLDS_DLLEXPORT const char *    (*pfnGetPlayerAuthId)          (edict_t *e);

   //No functions below this line are assigned values in the HLDS version 1110.
} enginefuncs_t;
typedef struct NEW_DLL_FUNCTIONS_s{ //Three functions.
   // Called right before the object's memory is freed.
   // Calls its destructor.
   HLDS_DLLEXPORT void  (*pfnOnFreeEntPrivateData)(edict_t *pEnt);
   HLDS_DLLEXPORT void  (*pfnGameShutdown)(void);
   HLDS_DLLEXPORT int   (*pfnShouldCollide)( edict_t *pentTouched, edict_t *pentOther );
} NEW_DLL_FUNCTIONS_t;
typedef struct Mod_DLL_Functions_s { //50 functions, 200 bytes.  WE call these from MODS.

   HLDS_DLLIMPORT void         (*pfnGameInit)                 (void);
   HLDS_DLLIMPORT int          (*pfnSpawn)                    (edict_t *pent);
   HLDS_DLLIMPORT void         (*pfnThink)                    (edict_t *pent);
   HLDS_DLLIMPORT void         (*pfnUse)                      (edict_t *pentUsed, edict_t *pentOther);
   HLDS_DLLIMPORT void         (*pfnTouch)                    (edict_t *pentTouched, edict_t *pentOther);
   HLDS_DLLIMPORT void         (*pfnBlocked)                  (edict_t *pentBlocked, edict_t *pentOther);
   HLDS_DLLIMPORT void         (*pfnKeyValue)                 (edict_t *pentKeyvalue, KeyValueData *pkvd);
   HLDS_DLLIMPORT void         (*pfnSave)                     (edict_t *pent, SAVERESTOREDATA *pSaveData);
   HLDS_DLLIMPORT int          (*pfnRestore)                  (edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
   HLDS_DLLIMPORT void         (*pfnSetAbsBox)                (edict_t *pent);
   HLDS_DLLIMPORT void         (*pfnSaveWriteFields)          (SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
   HLDS_DLLIMPORT void         (*pfnSaveReadFields)           (SAVERESTOREDATA *, const char *, void *, TYPEDESCRIPTION *, int);
   HLDS_DLLIMPORT void         (*pfnSaveGlobalState)          (SAVERESTOREDATA *);
   HLDS_DLLIMPORT void         (*pfnRestoreGlobalState)       (SAVERESTOREDATA *);
   HLDS_DLLIMPORT void         (*pfnResetGlobalState)         (void);
   HLDS_DLLIMPORT qboolean     (*pfnClientConnect)            (edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
   HLDS_DLLIMPORT void         (*pfnClientDisconnect)         (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnClientKill)               (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnClientPutInServer)        (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnClientCommand)            (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnClientUserInfoChanged)    (edict_t *pEntity, char *infobuffer);
   HLDS_DLLIMPORT void         (*pfnServerActivate)           (edict_t *pEdictList, int edictCount, int clientMax);
   HLDS_DLLIMPORT void         (*pfnServerDeactivate)         (void);
   HLDS_DLLIMPORT void         (*pfnPlayerPreThink)           (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnPlayerPostThink)          (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnStartFrame)               (void);
   HLDS_DLLIMPORT void         (*pfnParmsNewLevel)            (void);
   HLDS_DLLIMPORT void         (*pfnParmsChangeLevel)         (void);
   HLDS_DLLIMPORT const char * (*pfnGetGameDescription)       (void);
   HLDS_DLLIMPORT void         (*pfnPlayerCustomization)      (edict_t *pEntity, customization_t *pCustom);
   HLDS_DLLIMPORT void         (*pfnSpectatorConnect)         (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnSpectatorDisconnect)      (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnSpectatorThink)           (edict_t *pEntity);
   HLDS_DLLIMPORT void         (*pfnSys_Error)                (const char *error_string);
   HLDS_DLLIMPORT void         (*pfnPM_Move)                  (playermove_t *ppmove, qboolean server);
   HLDS_DLLIMPORT void         (*pfnPM_Init)                  (playermove_t *ppmove);
   HLDS_DLLIMPORT char         (*pfnPM_FindTextureType)       (char *name);
   HLDS_DLLIMPORT void         (*pfnSetupVisibility)          (edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas);
   HLDS_DLLIMPORT void         (*pfnUpdateClientData)         (const edict_t *ent, int sendweapons, clientdata_t *cd);
   HLDS_DLLIMPORT int          (*pfnAddToFullPack)            (entity_state_t *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
   HLDS_DLLIMPORT void         (*pfnCreateBaseline)           (int player, int eindex, entity_state_t *baseline, edict_t *entity, int playermodelindex, const vec3_t player_mins, const vec3_t player_maxs);
   HLDS_DLLIMPORT void         (*pfnRegisterEncoders)         (void );
   HLDS_DLLIMPORT int          (*pfnGetWeaponData)            (edict_t *player, weapon_data_t *info);
   HLDS_DLLIMPORT void         (*pfnCmdStart)                 (const edict_t *player, const usercmd_t *cmd, unsigned int random_seed);
   HLDS_DLLIMPORT void         (*pfnCmdEnd)                   (const edict_t *player);
   HLDS_DLLIMPORT int          (*pfnConnectionlessPacket)     (const netadr_t *global_net_from, const char *args, char *response_buffer, int *response_buffer_size);
   HLDS_DLLIMPORT int          (*pfnGetHullBounds)            (int hullnumber, const vec3_t mins, const vec3_t maxs);
   HLDS_DLLIMPORT void         (*pfnCreateInstancedBaselines) (void);
   HLDS_DLLIMPORT int          (*pfnInconsistentFile)         (const edict_t *player, const char *filename, char *disconnect_message);
   HLDS_DLLIMPORT int          (*pfnAllowLagCompensation)     (void);
} Mod_DLL_Functions_t;
typedef struct SecurityModule_s { //unknown everything.  Might as well remove.

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

typedef struct g_vars_s {

   float     time;
   float     frametime;
   float     force_retouch;
   string_t  mapname;
   string_t  startspot; //10
   float     deathmatch;
   float     coop;
   float     teamplay;
   float     serverflags;//20
   float     found_secrets;
   vec3_t    v_forward;//28
   vec3_t    v_up;//34
   vec3_t    v_right;//40
   float     trace_allsolid;//4c Why are these floats anyway?  Shouldn't they be bools?
   float     trace_startsolid;
   float     trace_fraction;
   vec3_t    trace_endpos;//58
   vec3_t    trace_plane_normal;//64
   float     trace_plane_dist;//70
   edict_t * trace_ent;
   float     trace_inopen; //78
   float     trace_inwater;
   int       trace_hitgroup; //80
   int       trace_flags;
   int       msg_entity;
   int       cdAudioTrack; //8C Not really verifiable.
   int       maxClients;
   int       maxEntities;
   char *    pStringBase;
   void *    pSaveData; //9C
   vec3_t    vecLandmarkOffset;

} g_vars_t;


//Large tables of vars.  Gotta go somewhere.
extern Mod_DLL_Functions_t gEntityInterface;
extern g_vars_t gGlobalVariables;
extern NEW_DLL_FUNCTIONS_t gNewDLLFunctions;
extern SecurityModule_t g_modfuncs;


void * GetDispatch(const char *);
void LoadThisDll(const char *);
void LoadEntityDLLs();
void ReleaseEntityDlls();

int TestStructureAlignment();
void SV_CheckBlendingInterface();

#endif
