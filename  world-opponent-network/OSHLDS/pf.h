/* Includes various functions that make up the mod API interface.
// Well where should they go?  Most don't have a good place, so they go here.
*/
#ifndef OSHLDS_PF_HEADER
#define OSHLDS_PF_HEADER

#include "endianin.h"
#include "cvar.h"
#include "edict.h"
#include "world.h"
#include "ModCalls.h"
#include "NET.h"

//note to self: protocol.h defines a lot of the packet indentifiers.  The early ones like svc_nop are a good match, the later ones have no basis in our reality.  Better than nothing though.
   //msg_dest values are defined in const.h

   #define MSG_BROADCAST      0  // unreliable to all
   #define MSG_ONE            1  // reliable to one (msg_entity)
   #define MSG_ALL            2  // reliable to all
   #define MSG_INIT           3  // write to the init string
   #define MSG_PVS            4  // Ents in PVS of org
   #define MSG_PAS            5  // Ents in PAS of org
   #define MSG_PVS_R          6  // Reliable to PVS
   #define MSG_PAS_R          7  // Reliable to PAS
   #define MSG_ONE_UNRELIABLE 8  // Send to one client, but don't put in reliable stream, put in unreliable datagram ( could be dropped )
   #define MSG_SPEC           9  // Sends to all spectator proxies

HLDS_DLLEXPORT int   PF_walkmove_I(edict_t *, float, float, int);
HLDS_DLLEXPORT int   PF_precache_model_I(char*);
HLDS_DLLEXPORT int   PF_precache_sound_I(char*);
HLDS_DLLEXPORT void  PF_setmodel_I(edict_t *, const char *);
HLDS_DLLEXPORT int   PF_modelindex(char *);
HLDS_DLLEXPORT void  PF_setsize_I(edict_t *e, const vec3_t rgflMin, const vec3_t rgflMax);
HLDS_DLLEXPORT void  PF_changelevel_I(char *, char *);
HLDS_DLLEXPORT void  PF_setspawnparms_I(edict_t *);
HLDS_DLLEXPORT void  SaveSpawnParms(edict_t *);
HLDS_DLLEXPORT float PF_vectoyaw_I(const vec3_t);
HLDS_DLLEXPORT void  PF_vectoangles_I(const vec3_t, vec3_t);
HLDS_DLLEXPORT void  PF_changeyaw_I(edict_t *);
HLDS_DLLEXPORT void  PF_changepitch_I(edict_t *);
HLDS_DLLEXPORT edict_t * FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue);
HLDS_DLLEXPORT int   GetEntityIllum(edict_t* pEnt);
HLDS_DLLEXPORT edict_t * FindEntityInSphere(edict_t *pEdictStartSearchAfter, const vec3_t org, float rad);
HLDS_DLLEXPORT edict_t * PF_checkclient_I(edict_t *pEdict);
HLDS_DLLEXPORT edict_t * PVSFindEntities(edict_t *pplayer);
HLDS_DLLEXPORT void  PF_makevectors_I(const vec3_t rgflVector);
HLDS_DLLEXPORT edict_t * PF_Spawn_I();
HLDS_DLLEXPORT void  PF_Remove_I(edict_t *);
HLDS_DLLEXPORT edict_t * CreateNamedEntity(int);
HLDS_DLLEXPORT void  PF_makestatic_I(edict_t *);
HLDS_DLLEXPORT qboolean PF_checkbottom_I(edict_t *);
HLDS_DLLEXPORT int  PF_droptofloor_I(edict_t* e);
HLDS_DLLEXPORT int  PF_walkmove_I(edict_t *, float, float, int);
HLDS_DLLEXPORT void PF_setorigin_I(edict_t *e, const vec3_t rgflOrigin);
HLDS_DLLEXPORT void PF_sound_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch);
HLDS_DLLEXPORT void PF_ambientsound_I(edict_t *entity, vec3_t pos, const char *samp, float vol, float attenuation, int fFlags, int pitch);
HLDS_DLLEXPORT void PF_traceline_DLL(vec3_t, vec3_t, int, edict_t *, TraceResult_t *);
HLDS_DLLEXPORT void PF_TraceToss_DLL(edict_t *, edict_t *, TraceResult_t *);
HLDS_DLLEXPORT int  TraceMonsterHull(edict_t *, vec3_t, vec3_t, int, edict_t *, TraceResult_t *);
HLDS_DLLEXPORT void TraceHull(vec3_t, vec3_t, int, int, edict_t *, TraceResult_t *);
HLDS_DLLEXPORT void TraceModel(vec3_t, vec3_t, int, edict_t *, TraceResult_t *);
HLDS_DLLEXPORT const char * TraceTexture(edict_t *, vec3_t, vec3_t);
HLDS_DLLEXPORT void TraceSphere(/*vec3_t, vec3_t, int, float, edict_t *, TraceResult_t **/);
HLDS_DLLEXPORT void PF_aim_I(edict_t *, float, vec3_t);
HLDS_DLLEXPORT void PF_localcmd_I(char *);
HLDS_DLLEXPORT void PF_localexec_I();
HLDS_DLLEXPORT void PF_stuffcmd_I(edict_t *, char *, ...);
HLDS_DLLEXPORT void PF_particle_I(const vec3_t org, const vec3_t dir, float color, float count);
HLDS_DLLEXPORT void PF_lightstyle_I(int style, char* val);
HLDS_DLLEXPORT int  PF_DecalIndex(const char *name);
HLDS_DLLEXPORT int  PF_pointcontents_I(const vec3_t rgflVector);
HLDS_DLLEXPORT void PF_MessageBegin_I(int, int, const float *, edict_t *);
HLDS_DLLEXPORT void PF_MessageEnd_I(void);
HLDS_DLLEXPORT void PF_WriteByte_I(int);
HLDS_DLLEXPORT void PF_WriteChar_I(int);
HLDS_DLLEXPORT void PF_WriteShort_I(int);
HLDS_DLLEXPORT void PF_WriteLong_I(int);
HLDS_DLLEXPORT void PF_WriteAngle_I(float);
HLDS_DLLEXPORT void PF_WriteCoord_I(float);
HLDS_DLLEXPORT void PF_WriteString_I(const char *);
HLDS_DLLEXPORT void PF_WriteEntity_I(int);
HLDS_DLLEXPORT void * PvAllocEntPrivateData(edict_t *, long);
HLDS_DLLEXPORT void * PvEntPrivateData(edict_t *);
HLDS_DLLEXPORT void FreeEntPrivateData(edict_t *);
int  IndexOfEdict(const edict_t *pEdict);
edict_t * PEntityOfEntIndex(int iEntIndex);
HLDS_DLLEXPORT edict_t * FindEntityByVars(struct entvars_s* pvars);
HLDS_DLLEXPORT void * GetModelPtr(edict_t* pEdict);
HLDS_DLLEXPORT int   RegUserMsg(const char *, int);
void FreeUserMsgs();
HLDS_DLLEXPORT void  PF_setview_I(const edict_t *, const edict_t *);
HLDS_DLLEXPORT float PF_Time();
HLDS_DLLEXPORT void  PF_crosshairangle_I(const edict_t *, float, float);
HLDS_DLLEXPORT void  PF_FadeVolume(const edict_t *, int, int, int, int);
HLDS_DLLEXPORT void  PF_SetClientMaxspeed(edict_t *, float);
HLDS_DLLEXPORT void  PF_RunPlayerMove_I(edict_t *, const vec3_t, float, float, float, unsigned short, byte, byte);
HLDS_DLLEXPORT int   PF_NumberOfEntities_I();
HLDS_DLLEXPORT char * PF_GetInfoKeyBuffer_I(edict_t *);
HLDS_DLLEXPORT const char * PF_InfoKeyValue_I(char *infobuffer, char *key);
HLDS_DLLEXPORT void  PF_SetKeyValue_I(char *infobuffer, char *key, char *value);
HLDS_DLLEXPORT void  PF_SetClientKeyValue_I(int clientIndex, char *infobuffer, char *key, char *value);
HLDS_DLLEXPORT int   PF_IsMapValid_I(const char *);
HLDS_DLLEXPORT void  PF_StaticDecal(const float *, int, int, int);
HLDS_DLLEXPORT int   PF_precache_generic_I(char *);
HLDS_DLLEXPORT int   PF_GetPlayerUserId(edict_t *);
HLDS_DLLEXPORT void  PF_BuildSoundMsg_I(edict_t *, int, const char *, float, float, int, int, int, int, const vec3_t, edict_t *);
HLDS_DLLEXPORT int   PF_IsDedicatedServer();
HLDS_DLLEXPORT unsigned int PF_GetPlayerWONId(edict_t *);
HLDS_DLLEXPORT void PF_RemoveKey_I(char *s, const char *key);
HLDS_DLLEXPORT const char * PF_GetPhysicsKeyValue(const edict_t *pClient, const char *key);
HLDS_DLLEXPORT void PF_SetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value);
HLDS_DLLEXPORT const char * PF_GetPhysicsInfoString(const edict_t *pClient);
HLDS_DLLEXPORT unsigned short EV_Precache(int type, const char * psz);
HLDS_DLLEXPORT int  SV_CheckVisibility(edict_t *entity, unsigned char *pset);
HLDS_DLLEXPORT int  PF_GetCurrentPlayer();
HLDS_DLLEXPORT int  PF_CanSkipPlayer(const edict_t *);
HLDS_DLLEXPORT void PF_SetGroupMask(int, int);
HLDS_DLLEXPORT int  PF_CreateInstancedBaseline(int, entity_state_t *);
HLDS_DLLEXPORT void PF_Cvar_DirectSet(cvar_t *, char *);
HLDS_DLLEXPORT void PF_ForceUnmodified(FORCE_TYPE, vec3_t, vec3_t, const char *);
HLDS_DLLEXPORT void PF_GetPlayerStats(const edict_t *t, int *, int *);
HLDS_DLLEXPORT qboolean Voice_GetClientListening(int iReceiver, int iSender);
HLDS_DLLEXPORT qboolean Voice_SetClientListening(int iReceiver, int iSender, qboolean bListen);
HLDS_DLLEXPORT const char * PF_GetPlayerAuthId(edict_t *e);
HLDS_DLLEXPORT int  AllocEngineString(const char *);
HLDS_DLLEXPORT void ServerPrint(const char *szMsg);
void FreeAllEntPrivateData();

void PF_Init();
#endif
