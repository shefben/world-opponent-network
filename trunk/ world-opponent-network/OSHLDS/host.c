#include <limits.h>
#ifdef _MSC_VER
 #include <winsock2.h>
 #include <windows.h>
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
#endif

#include "host.h"
#include "banlist.h"
#include "com_file.h"
#include "cmd.h"
#include "cvar.h"
#include "draw.h"
#include "hpak.h"
#include "master.h"
#include "Modding.h"
#include "NET.h"
#include "pf.h"
#include "R.h"
#include "SV_frame.h"
#include "SV_hlds.h"
#include "SV_mcast.h"
#include "SV_user.h"
#include "sys.h"


//#include <dlfcn.h>


/*** SRUCTS ***/
typedef struct host_times_s {

   double timeold;
   double time1;
   double time2;
   double time3;
   double time4;
   double time5;

} host_times_t;





/*** GLOBALS ***/
static double sys_timescale = 1.0; //needs to be a CVAR but was not registered.
static double rolling_fps = 0;
static double startTime = 0;

static cvar_t cvar_HostMap                    = {                    "HostMap",  "C1A0",    FCVAR_NONE,     0, NULL };
static cvar_t cvar_voice_recordtofile         = {         "voice_recordtofile",      "",    FCVAR_NONE,     0, NULL };
static cvar_t cvar_voice_inputfromfile        = {        "voice_inputfromfile",      "",    FCVAR_NONE,     0, NULL };
static cvar_t cvar_host_killtime              = {              "host_killtime",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_sys_ticrate                = {                "sys_ticrate", "100.0",    FCVAR_NONE, 100.0, NULL };
static cvar_t cvar_fps_max                    = {                    "fps_max",  "72.0",    FCVAR_NONE,  72.0, NULL };
static cvar_t cvar_fps_modem                  = {                  "fps_modem",   "0.0", FCVAR_ARCHIVE,   0.0, NULL };
static cvar_t cvar_host_framerate             = {             "host_framerate",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_host_speeds                = {                "host_speeds",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_host_profile               = {               "host_profile",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_mp_logfile                 = {                 "mp_logfile",     "1",  FCVAR_SERVER,   1.0, NULL };
static cvar_t cvar_mp_logecho                 = {                 "mp_logecho",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_global_sv_log_onefile      = {      "global_sv_log_onefile",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_global_sv_log_singleplayer = { "global_sv_log_singleplayer",     "0",    FCVAR_NONE,   0.0, NULL };
static cvar_t cvar_global_sv_stats            = {            "global_sv_stats",     "1",    FCVAR_NONE,   1.0, NULL };
static cvar_t cvar_pausable                   = {                   "pausable",     "1",  FCVAR_SERVER,   1.0, NULL };
static cvar_t cvar_fs_preloader               = {               "fs_preloader",     "1",    FCVAR_NONE,   1.0, NULL };
static cvar_t cvar_skill                      = {                      "skill",     "1",    FCVAR_NONE,   1.0, NULL };
static cvar_t cvar_violence_hblood            = {            "violence_hblood",     "1",    FCVAR_NONE,   1.0, NULL };
static cvar_t cvar_violence_ablood            = {            "violence_ablood",     "1",    FCVAR_NONE,   1.0, NULL };
static cvar_t cvar_violence_hgibs             = {             "violence_hgibs",     "1",    FCVAR_NONE,   1.0, NULL };
static cvar_t cvar_violence_agibs             = {             "violence_agibs",     "1",    FCVAR_NONE,   1.0, NULL };
       cvar_t cvar_deathmatch                 = {                 "deathmatch",     "0",  FCVAR_SERVER,   0.0, NULL };
       cvar_t cvar_coop                       = {                       "coop",     "0",  FCVAR_SERVER,   0.0, NULL };


double host_frametime;
int host_hunklevel;

       cvar_t cvar_hostname  = {  "hostname", "Half-Life", FCVAR_NONE,   0, NULL };
       cvar_t cvar_developer = { "developer",         "2", FCVAR_NONE, 2.0, NULL };


/*** FUNCTIONS ***/

//do nothings
void Host_Demos_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_FindRecentSave() { printf("%s called", __FUNCTION__); exit(99); }
//void Host_GetBatchModList_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Getcertificate_f() { }
void Host_Interp_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Kick_f() {  printf("%s called", __FUNCTION__); exit(99); } //important, but doesn't look fun
void Host_KillVoice_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Load() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Loadgame_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_NextDemo() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Reconnect_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Reload_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_SaveAgeList() { printf("%s called", __FUNCTION__); exit(99); }
void Host_SaveGameDirectory() { printf("%s called", __FUNCTION__); exit(99); }
void Host_SavegameComment() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Savegame_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Soundfade_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Startdemos_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_Stopdemo_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_VoiceRecordStart_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_VoiceRecordStop_f() { printf("%s called", __FUNCTION__); exit(99); }
void Host_WriteCustomConfig() { printf("%s called", __FUNCTION__); exit(99); }
void Host_WriteConfiguration() { printf("%s called", __FUNCTION__); exit(99); }


//oddities
void GetStatsString(char * buffer, unsigned int size) {

   if(size < sizeof("disabled")) { return; }
   memcpy(buffer, "disabled", sizeof("disabled"));
}

void Host_CheckConnectionFailure() {

   //I don't see anything useful being done here.  I guess this actually
   //applies for clients.

}

void Host_CheckDynamicStructures() {

   unsigned int i;
   client_t * cl;


   if(global_svs.clients == NULL) { return; }
   for(i = 0, cl = global_svs.clients; i < global_svs.total_client_slots; i++, cl++) {

      if(cl->padding3480_frames != NULL) {

         SV_ClearFrames(&(cl->padding3480_frames));
      }
   }
}
void Host_ClearMemory() {

   CM_FreePAS();
   SV_ClearEntities();
   Mod_ClearAll();

   // host_hunklevel basically states 'anyting past this point is map specific,
   // and should be wiped when loading a new map'.


   if(host_hunklevel != 0) {
      Host_CheckDynamicStructures();
      Hunk_FreeToLowMark(host_hunklevel);
   }

   global_cls.padding340C = 0;
   SV_ClearCaches();
   Q_memset(&global_sv, 0, sizeof(server_t));
   SV_ClearClientStates();
}
void Host_ClearGameState() {

  // Host_ClearSaveDirectory();
   gEntityInterface.pfnResetGlobalState();
}

//Host_ClientCommands is in pf.c, needs movin'
//Host_ClearClients is in SV_hlds.c
//Host_Endgame seems unused
void Host_EndSection(const char *pszSectionName) {

   //This function is a weird one.  It's not documented, no examples of its use
   //exist, and I can't find the character strings it checks for defined anywhere.
   //It's not explicitly stated, but i don't see how this could possibly be
   //useful to anyone but the client.  And because of that, any actual meat
   //in the code was probably ifdef'd out.

   giActive = DLL_PAUSED;

   //The three valid strings:
   //"_oem_end_training"
   //"_oem_end_logo"
   //"_oem_end_demo"

   //They change the global 'giStateInfo', which is something I don't see
   //causing any great changes in the engine.

   //They then add 'disconnect' into the command buffer.  Which (assuming it's
   //linked to CL_Disconnect_f) does nothing.

   Con_Printf("%s called.\n", __FUNCTION__);
}

/* DESCRIPTION: Host_FilterTime
// PATH: _Host_Frame
//
// 'time' is the length of th next frame.  This function readies the engine
// for another frame of length time, or returns zero if not enough real world
// time has passed.  Or something.
*/
int Host_FilterTime(float time) {

   double var_4;
   static double oldglobal_realtime = 0;


   //Unsurprisingly, being a dedicated server simplifies things (about 2/3rds of the function).
   global_realtime += sys_timescale * time;

   var_4 = cvar_sys_ticrate.value;
   if(var_4 > 0 && (global_realtime - oldglobal_realtime) < (1.0 / (var_4 + 1.0))) { return(0); }

   host_frametime = global_realtime - oldglobal_realtime;
   oldglobal_realtime = global_realtime;

   if(host_frametime > .25) {
      host_frametime = .25;
   }
   else if(host_frametime < .001) {
      host_frametime = .001;
   }

   return(1);
}


void Host_ShutdownServer(int arg_0) {

   unsigned int i;


   if(global_sv.active == 0) { return; }

   SV_ServerShutdown();
   global_sv.active = 0; //do this AFTER ServerShutdown();


   //NET_ClearLagData(1, 1);


   for(i = 0, global_host_client = global_svs.clients; i < global_svs.allocated_client_slots; i++, global_host_client++) {
      if(global_host_client->padding00_used != 0 || global_host_client->padding0C_connected != 0) {

         SV_DropClient(global_host_client, arg_0, "Server shutting down.\n");
      }
   }

   SV_ClearEntities();
   SV_ClearCaches();
   FreeAllEntPrivateData();

   //Biggie
   Q_memset(&(global_sv), 0, sizeof(server_t));

   SV_ClearClientStates();
   Host_ClearClients();

   for(i = 0, global_host_client = global_svs.clients; i < global_svs.total_client_slots; i++, global_host_client++) {

      SV_ClearFrames(&(global_host_client->padding3480_frames));
   }
   Q_memset(global_svs.clients, 0, sizeof(client_t) * global_svs.total_client_slots);

   Master_Shutdown(0);
   HPAK_FlushHostQueue();
}

int Host_GetStartTime() {
   return(startTime);
}
void Host_InitializeGameDLL() {

   Cbuf_Execute();
   NET_Config(1);

   if(global_svs.pad00 != 0) { return; }
   global_svs.pad00 = 1;

   LoadEntityDLLs();

   gEntityInterface.pfnGameInit();
   gEntityInterface.pfnPM_Init(&global_svmove);
   gEntityInterface.pfnRegisterEncoders();

   SV_InitEncoders();
  // SV_DownloadSecurityModules();
   SV_GetPlayerHulls();
   SV_CheckBlendingInterface();
  // SV_CheckSaveGameCommentInterface();
   Cbuf_Execute(); //This line is, inevitebly, responsible for commandline arguments like +sv_lan 1
}

int Host_IsServerActive() {
   return(global_sv.active);
}
int Host_IsSinglePlayerGame() {
   return(0);
}

void Host_Map(int arg_0_unknown, char * arg_4, char * arg_8_map /*, int arg_c_IsSave*/) {

   gpnewusermsg_struct_t * ptr;


   Host_ShutdownServer(0);
   key_dest = 0;

   //Do these three if it's not a save
   Host_ClearGameState();
   SV_InactivateClients();
   global_svs.padding14 = 0;

   Q_strncpy(global_cls.paddingstr, arg_4, 0x3F);
   if(SV_SpawnServer(arg_0_unknown, arg_8_map, NULL) == 0) { return; }


   //This block is quite different for a save
      SV_LoadEntities();
      SV_ActivateServer(1);
      if(global_sv.active == 0) { return; }

      //if it's not a dedicated server, we do a bit more here

   if(global_sv_gpNewUserMsg != NULL) {
      if(global_sv_gpUserMsg == NULL) {
         global_sv_gpUserMsg = global_sv_gpNewUserMsg;
      }
      else {
         for(ptr = global_sv_gpUserMsg; ptr->next != NULL; ptr = ptr->next) { }
         ptr->next = global_sv_gpNewUserMsg;
      }

      global_sv_gpNewUserMsg = NULL;
   }

   //one last non-dedicated server only command is here.
}



void Host_Say(int TeamOnly) { //Boolean is unused, as this function only applies to whoever's monitoring the server.

   unsigned int i, len, len2;
   const char * message;
   char fullstring[0x80];
   client_t * client;
   client_t * source;


   if(Cmd_Argc() < 2) { return; }

   message = Cmd_Args();
   if(message == NULL || message[0] == '\0') { return; }
   len = Q_strlen(message);
   source = global_host_client;


   if(message[0] == '"') {
      if(len < 3 || message[len-1] != '"') { return; } //I don't know what else to do.

      message++;
      len -= 2;
   }

   len2 = Q_snprintf(fullstring, sizeof(fullstring)/2, "\x01" "<%s> ", cvar_hostname.string);

   if(len2 > sizeof(fullstring)/2) { //That's a long name.

      fullstring[(sizeof(fullstring)/2) - 2] = '>';
      fullstring[(sizeof(fullstring)/2) - 1] = ' ';
      len2 = sizeof(fullstring)/2;
   }
   if(sizeof(fullstring) - (len2 + 2) < len ) { //This means the actual message is too long.

      len = sizeof(fullstring) - (len2 + 2);
   }

   //Packetron! Assemble!
   Q_memcpy(fullstring + len2, message, len);
   fullstring[len2 + len] = '\n';
   fullstring[len2 + len + 1] = '\0';

   //And send.
   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0 || client->padding04 == 0 || client->padding2438_IsFakeClient != 0) { continue; }


      global_host_client = client;

      PF_MessageBegin_I(1, RegUserMsg("SayText", -1), NULL, &(global_sv.edicts[i+1]));
      PF_WriteByte_I(0);
      PF_WriteString_I(fullstring);
      PF_MessageEnd_I();
   }

   global_host_client = source;
}

/* DESCRIPTION: Host_Speeds
// PATH: _Host_Frame
//
// This function looks important, but it just prints a status update.
*/
void Host_Speeds(host_times_t * times) {

   unsigned int i, j;
   double time_1;
   double time_2;
   double time_3;
   double time_4;
   double time_5;
   double TimeTotal;
   double TimeTotalI;


   if(cvar_host_speeds.value == 0) { return; }


   time_1 = (times->time1 - times->timeold) * 1000;
   time_2 = (times->time2 - times->time1) * 1000;
   time_3 = (times->time3 - times->time2) * 1000;
   time_4 = (times->time4 - times->time3) * 1000;
   time_5 = (times->time5 - times->time4) * 1000;

   TimeTotal = times->time5 - times->timeold;
   if(TimeTotal < .001001001002) { TimeTotalI = 999; }
   else { TimeTotalI = 1 / TimeTotal; }

   j = 0;
   for(i = 0; i < global_sv.num_edicts; i++) {
      if(global_sv.edicts[i].free == 0) { j++; }
   }

   Con_Printf("%3i fps -- host(%3.0f) global_sv(%3.0f) cl(%3.0f) gfx(%3.0f) snd(%3.0f) ents(%d)", TimeTotalI, time_5, time_4, time_3, time_2, time_1, j);
}

void Host_UpdateScreen() { }
void Host_UpdateSounds() { }
/* DESCRIPTION: Host_UpdateStats
// PATH: _Host_Frame
//
// This function jumps through hurdles to find out how much processor time
// we are using.  Well I don't think that's too important.  Person wants
// to know, they can look that up themselves.  So it's a deadie.
*/
void Host_UpdateStats() {

   if(startTime == 0) { startTime = Sys_FloatTime(); }
}

/* DESCRIPTION: Host_ValidSave
// PATH: Host_Autosave_f
//
// 'validates' a save, which of course does nothing on a dedicated server.
*/
int Host_ValidSave() {

   Con_Printf("%s: Dedicated servers can't save games.\n", __FUNCTION__);
   return(0);
}
/* DESCRIPTION: Host_Version
// PATH: Host_Init
//
// This reads an inf file and digs out a 'version' parameter.  Not necessary.
*/
void Host_Version() { }






//console

void Host_Changelevel_f() {

   if(Cmd_Argc() < 2) {
      Con_Printf("changelevel <levelname>\n");
      return;
   }
   if(global_sv.active == 0) {
      Con_Printf("%s: Server not active.\n", __FUNCTION__);
      return;
   }
   if(PF_IsMapValid_I(Cmd_Argv(1)) == 0) {
      Con_Printf("%s: Map %s not found\n", Cmd_Argv(1));
      return;
   }

   SV_InactivateClients();
   SV_ServerShutdown();
   SV_SpawnServer(0, Cmd_Argv(1), Cmd_Argv(2));
   SV_LoadEntities();
   SV_ActivateServer(1);
}
void Host_Changelevel2_f() {

   Con_Printf("%s: Not usable in multiplayer game.\n", __FUNCTION__);
}


void Host_FullInfo_f() {

   unsigned int i;
   const char *ptr;
   char *ptr2;
   char key[0x200];
   char value[0x200];


   if(Cmd_Argc() != 2)   {
      Con_Printf("%s: This command takes two arguments.\n", __FUNCTION__);
      return;
   }

   ptr = Cmd_Argv(1);
   if(ptr[0] == '\\') { ptr++; }

   while(1) {
      if(ptr[0] == '\0') { break; }

      for(i = 0, ptr2 = key; i < sizeof(key)-1 && ptr[0] != '\0' && ptr[0] != '\\'; i++, ptr2++, ptr++) {
         *ptr2 = *ptr;
      }
      ptr2[0] = '\0';

      if(ptr[0] == '\0') {

         Con_Printf("%s: Missing value.\n", __FUNCTION__);
         return;
      }

      for(i = 0, ptr2 = value; i < sizeof(key)-1 && ptr[0] != '\0' && ptr[0] != '\\'; i++, ptr2++, ptr++) {
         *ptr2 = *ptr;
      }
      ptr2[0] = '\0';

      if(ptr[0] != '\0') { ptr++; }

      if(global_cmd_source == cmd_SERVER) {
         Info_SetValueForKey(global_cls.userinfo, key, value, 0x100);
         return;
      }

      Info_SetValueForKey(global_host_client->InfoKeyBuffer, key, value, 0x100);
      global_host_client->padding4BA4 = 1;
   }
}

void Host_KillServer_f() {

   if(global_cmd_source != cmd_CLIENT) { return; }

   if(global_sv.active) {
      Host_ShutdownServer(0);
   }
}

void Host_Kill_f() {

   if(global_cmd_source == cmd_SERVER) { return; }

   if(global_sv_player->v.health > 0) {

      gGlobalVariables.time = global_sv.time0c;
      gEntityInterface.pfnClientKill(global_sv_player);
   }
   else {

      SV_ClientPrintf("You're already dead, Jim.\n");
   }
}


void Host_Map_f() {

   int i;
   unsigned int len;
   char map[0x40];
   char map2[0x40];
   char * ptr;


   if(global_cmd_source != cmd_SERVER) { return; }

   len = 0;
   ptr = map;
   for(i = 0; i < Cmd_Argc(); i++) {

      len += Q_snprintf(ptr, sizeof(map) - (len + 1), "%s ", Cmd_Argv(i));
      if(sizeof(map)-1 < len ) {
         Con_Printf("%s: Map command string was too long", __FUNCTION__);
      }
      ptr = len + map;
   }

   ptr[-1] = '\n'; //This WAS a space.


   Q_strncpy(map2, Cmd_Argv(1), sizeof(map2)-1);
   map2[sizeof(map2)-1] = '\0';

   if(global_svs.pad00 == 0) {
      Host_InitializeGameDLL();
   }

   i = Q_strlen(map2);
   if(i > 4 && Q_memcmp(map2 + i - 4, ".bsp", 4) == 0) {
      map2[i - 4] = '\0';
   }


   if(PF_IsMapValid_I(map2) == 0) {

      Con_Printf("%s: Couldn't find map %s.\n", __FUNCTION__, map2);
      return;
   }

   //we can probably skip COM_CheckAuthenticationType

   Cvar_DirectSet(&cvar_HostMap, map2);
   Host_Map(0, map, map2);
}


void Host_Maps_f() {

   const char * ptr;


   if(Cmd_Argc() != 2) { return; }
   ptr = Cmd_Argv(1);
   if(ptr == NULL || ptr[0] == '\0') { return; }

   COM_ListMaps(ptr);
}
void Host_Motd_Write_f() { }
void Host_Motd_f() {

   //Reading in the MOTD each time is pretty stupid.  Let's ignore this for now.
//   cvar_motdfile.string;

}

void Host_Quit_f() { //This is a bit different now.  Since we block read, we need a more immediate quit.




}

void Host_Ping_f() {

   unsigned int i;
   client_t   *client;


   if(global_cmd_source == cmd_SERVER)   { return; }

   SV_ClientPrintf("Client ping times:\n");
   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0) { continue; }
      SV_ClientPrintf("%4i %s\n", SV_CalcPing(client), client->PlayerName);
   }
}
void Host_Restart_f() {

   char Map[sizeof(global_sv.name)];


   if(global_cmd_source == cmd_SERVER || global_sv.active == 0) { return; }

   strcpy(Map, global_sv.name);

   Host_ClearGameState();
   SV_InactivateClients();
   SV_ServerShutdown();
   SV_SpawnServer(0, Map, 0);
   SV_LoadEntities();
   SV_ActivateServer(1);
}

/* DESCRIPTION: Host_MESSAGE
// PATH: console
//
// Gotta say what you gotta say.
*/
void Host_Say_f() { Host_Say(0); }
void Host_Say_Team_f(void) { Host_Say(0); }
void Host_Tell_f() {


   unsigned int i, len, len2;
   const char * message;
   char fullstring[0x80];
   client_t * client;
   client_t * source;


   if(Cmd_Argc() < 3) { return; }

   message = Cmd_Args();
   if(message == NULL || message[0] == '\0') { return; }
   len = Q_strlen(message);
   source = global_host_client;


   if(message[0] == '"') {
      if(len < 3 || message[len-1] != '"') { return; } //I don't know what else to do.

      message++;
      len -= 2;
   }

   len2 = Q_snprintf(fullstring, sizeof(fullstring)/2, "%s TELL: ", cvar_hostname.string);

   if(len2 > sizeof(fullstring)/2) { //That's a long name.

      fullstring[(sizeof(fullstring)/2) - 2] = '>';
      fullstring[(sizeof(fullstring)/2) - 1] = ' ';
      len2 = sizeof(fullstring)/2;
   }
   if(sizeof(fullstring) - (len2 + 2) < len ) { //This means the actual message is too long.

      len = sizeof(fullstring) - (len2 + 2);
   }

   Q_memcpy(fullstring + len2, message, len);
   fullstring[len2 + len] = '\n';
   fullstring[len2 + len + 1] = '\0';

   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0 || client->padding04 == 0 || client->padding2438_IsFakeClient != 0 ||
         Q_strcasecmp(client->PlayerName, Cmd_Argv(1)) != 0) {

         continue;
      }

      global_host_client = client;

      PF_MessageBegin_I(1, RegUserMsg("SayText", -1), NULL, &(global_sv.edicts[i+1]));
      PF_WriteByte_I(0);
      PF_WriteString_I(fullstring);
      PF_MessageEnd_I();
   }

   global_host_client = source;
}

void Host_SetInfo_f() {

   if(global_cmd_source == cmd_SERVER) { return; } //The server has no client...

/* And because of that, why are we printing anything?
   if(Cmd_Argc() == 1) {
      Info_Print(global_cls.userinfo); //
      return;
   }

   if(Cmd_Argc() != 3) {
      Con_Printf("usage: setinfo [ <key> <value> ]\n");
      return;
   }
*/

   if(Cmd_Argc() != 3) { return; }

   else {

      Info_SetValueForKey(global_host_client->InfoKeyBuffer, Cmd_Argv(1), Cmd_Argv(2), 0x100);
      global_host_client->padding4BA4 = 1;
   }
}

void Host_Stats_f() {

   char buffer[0x200];


   if(global_cmd_source == cmd_SERVER) { return; }

   GetStatsString(buffer, sizeof(buffer));
   Con_Printf("%s", buffer);
}
void Host_Status_f() {

   client_t * client;
   unsigned int i, count, times, timem, timeh;

   void (*print)(const char *, ...); //Good idea, the other one sucked.


   if(global_cmd_source == cmd_CLIENT) {
      if(global_sv.active == 0) { return; }
      print = SV_ClientPrintf;
   }
   else { print = Con_Printf; }

   if(global_noip == 0) {
      print("tcp/ip:    %s\n", NET_AdrToString(global_net_local_adr));
   }
   print("hostname:  %s\n"
         "version:   %d\n"
         "protocol:  46\n"
         "map:       %s\n"
         "slot              name                 CD_Key     time           address\n",
         cvar_hostname.string, build_number(), global_sv.name);

   count = 0;
   for(i = 0, client = global_svs.clients; i < global_svs.allocated_client_slots; i++, client++) {

      if(client->padding00_used == 0) { continue; }

      count++;
      //It's pretty unlikely someone will be on for 4 bil seconds, so...
      //Let the optimizer handle it.  It's good at this sort of thing.
      times = global_realtime - client->netchan1C.first_received;
      timem = times / 60;
      timeh = timem / 60;
      times %= 60;
      timem %= 60;

      print("#%02u %32s %010i %3i:%02i:%02i %s\n",
            i, client->PlayerName, client->padding4A8C_PlayerUID, timeh, timem, times,
            (client->padding2438_IsFakeClient != 0 || client->padding243C_proxy != 0) ? "(fake)" : NET_AdrToString(client->netchan1C.remote_address));
   }

   print(" %i players (%i max)\n", count, global_svs.allocated_client_slots);
}


/* DESCRIPTION: Host_#Pause_f
// PATH: console
//
// Changes a boolean and sends out updates to everyone.  Most dedicated servers
// can't be paused, of course.
*/
void Host_Pause_f() {

   if(global_cmd_source != cmd_SERVER && cvar_pausable.value != 0)   {

      global_sv.padding04 = 1;
      MSG_WriteByte(&(global_sv.sizebuf3CC1C), 0x18);
      MSG_WriteByte(&(global_sv.sizebuf3CC1C), 1);
   }
}
void Host_TogglePause_f() {

   if(global_cmd_source != cmd_SERVER && cvar_pausable.value != 0)   {

      global_sv.padding04 = !global_sv.padding04;

      SV_BroadcastPrintf("%s %s the game\n", global_pr_strings + global_sv_player->v.netname, (global_sv.padding04 == 0) ? "unpaused" : "paused" );
      MSG_WriteByte(&(global_sv.sizebuf3CC1C), 0x18);
      MSG_WriteByte(&(global_sv.sizebuf3CC1C), global_sv.padding04);
   }
}
void Host_Unpause_f() {

   if(global_cmd_source != cmd_SERVER && cvar_pausable.value != 0)   {

      global_sv.padding04 = 0;
      MSG_WriteByte(&(global_sv.sizebuf3CC1C), 0x18);
      MSG_WriteByte(&(global_sv.sizebuf3CC1C), 0);
   }
}

/* DESCRIPTION: Host_Version_f
// PATH: console
//
// Simply prints version info.
*/
void Host_Version_f() {
   Con_Printf("OSHLDS VER %s BUILT " __DATE__ "--" __TIME__ " (46)\n", build_number());
}





void Host_ComputeFPS(double time) {

   //original
   //rolling_fps = (rolling_fps * .6) + (time * .4);

   //I don't like rolling, I like knowing
   rolling_fps = time;
}
void Host_GetHostInfo(float * fps, int * players, int * arg_8, int * playerMax, char * map) {

   if(rolling_fps > 0) {
      *fps = 1.0 / rolling_fps;
   }
   else {
      rolling_fps = 0;
      *fps = 0;
   }

   SV_CountPlayers(players);
   *playerMax = global_svs.allocated_client_slots;

   if(arg_8 != NULL) {
      *arg_8 = 0;
   }
   if(map != NULL) {
      strcpy(map, global_sv.name); //Assume correct size buffer.
   }
}

void Host_InitCommands() {

   Cmd_AddCommand("getcertificate", Host_Getcertificate_f);
//   Cmd_AddCommand("bgetmod", Host_GetBatchModList_f);
   Cmd_AddCommand("shutdownserver", Host_KillServer_f);
   Cmd_AddCommand("soundfade", Host_Soundfade_f);
   Cmd_AddCommand("status", Host_Status_f);
   Cmd_AddCommand("stat", Host_Status_f);
   Cmd_AddCommand("map", Host_Map_f);
   Cmd_AddCommand("maps", Host_Maps_f);
   Cmd_AddCommand("restart", Host_Restart_f);
//   Cmd_AddCommand("reload", Host_Reload_f);
   Cmd_AddCommand("changelevel", Host_Changelevel_f);
   Cmd_AddCommand("changelevel2", Host_Changelevel2_f);
   Cmd_AddCommand("reconnect", Host_Reconnect_f);
   Cmd_AddCommand("version", Host_Version_f);
   Cmd_AddCommand("say", Host_Say_f);
   Cmd_AddCommand("say_team", Host_Say_Team_f);
   Cmd_AddCommand("tell", Host_Tell_f);
   Cmd_AddCommand("kill", Host_Kill_f);
   Cmd_AddCommand("pause", Host_TogglePause_f);
   Cmd_AddCommand("setpause", Host_Pause_f);
   Cmd_AddCommand("unpause", Host_Unpause_f);
   Cmd_AddCommand("kick", Host_Kick_f);
   Cmd_AddCommand("ping", Host_Ping_f);
   Cmd_AddCommand("motd", Host_Motd_f);
   Cmd_AddCommand("motd_write", Host_Motd_Write_f);
   Cmd_AddCommand("stats", Host_Stats_f);
//   Cmd_AddCommand("load", Host_Loadgame_f);
//  Cmd_AddCommand("save", Host_Savegame_f);
//   Cmd_AddCommand("autosave", Host_AutoSave_f);
   Cmd_AddCommand("writecfg", Host_WriteCustomConfig);
//   Cmd_AddCommand("startdemos", Host_Startdemos_f);
//   Cmd_AddCommand("demos", Host_Demos_f);
//   Cmd_AddCommand("stopdemo", Host_Stopdemo_f);
   Cmd_AddCommand("setinfo", Host_SetInfo_f);
   Cmd_AddCommand("fullinfo", Host_FullInfo_f);
//   Cmd_AddCommand("mcache", Mod_Print);
   Cmd_AddCommand("interp", Host_Interp_f);
//   Cmd_AddCommand("setmaster", Master_SetMaster_f);
//   Cmd_AddCommand("heartbeat", Master_Heartbeat_f);

   Cvar_RegisterVariable(&cvar_HostMap);
   Cvar_RegisterVariable(&cvar_voice_recordtofile);
   Cvar_RegisterVariable(&cvar_voice_inputfromfile);
}
void Host_InitLocal() {

   Host_InitCommands();
   Cvar_RegisterVariable(&cvar_host_killtime);
   Cvar_RegisterVariable(&cvar_sys_ticrate);
   Cvar_RegisterVariable(&cvar_fps_max);
   Cvar_RegisterVariable(&cvar_fps_modem);
   Cvar_RegisterVariable(&cvar_hostname);
   Cvar_RegisterVariable(&cvar_host_framerate);
   Cvar_RegisterVariable(&cvar_host_speeds);
   Cvar_RegisterVariable(&cvar_host_profile);
   Cvar_RegisterVariable(&cvar_mp_logfile);
   Cvar_RegisterVariable(&cvar_mp_logecho);
   Cvar_RegisterVariable(&cvar_global_sv_log_onefile);
   Cvar_RegisterVariable(&cvar_global_sv_log_singleplayer);
   Cvar_RegisterVariable(&cvar_global_sv_stats);
   Cvar_RegisterVariable(&cvar_pausable);
   Cvar_RegisterVariable(&cvar_developer);
   Cvar_RegisterVariable(&cvar_fs_preloader);
   Cvar_RegisterVariable(&cvar_skill);
   Cvar_RegisterVariable(&cvar_deathmatch);
   Cvar_RegisterVariable(&cvar_coop);

   Cvar_RegisterVariable(&cvar_violence_hblood);
   Cvar_RegisterVariable(&cvar_violence_ablood);
   Cvar_RegisterVariable(&cvar_violence_hgibs);
   Cvar_RegisterVariable(&cvar_violence_agibs);


   sys_timescale = 1.0;
   SV_SetMaxclients();
}
void Host_Shutdown() {

   static int recurse = 0;
   unsigned int i;
   client_t * cl;

   if(recurse != 0) { return; }
   recurse = 1;

   //host_initialized 0;
   SV_ServerShutdown();

   //no security modules
   Cmd_RemoveGameCmds();
   Cmd_Shutdown();
   Cbuf_Shutdown();
   Cvar_Shutdown();
   HPAK_FlushHostQueue();
   SV_DeallocateDynamicData();
   FreeUserMsgs();
   for(i = 0, cl = global_svs.clients; i < global_svs.total_client_slots; i++, cl++) {
      SV_ClearFrames(&(cl->padding3480_frames));
   }

   SV_Shutdown();
  // SystemWrapper_ShutDown(); //I still don't know what this all is
   Master_Shutdown(1);
   NET_Shutdown();
   ReleaseEntityDlls();
   CM_FreePAS();

   //scr_disabled_for_loading ignored

   Mod_Shutdown();

   Draw_DecalShutdown();
   W_Shutdown();
   DELTA_Shutdown();

   global_realtime = 0;
   global_sv.time0c = 0;
}


static cvar_t console = { "console", "1", 1, 0, 0 };
static cvar_t suitvolume = { "suitvolume", "0.25", 1, 0, 0 };

/* DESCRIPTION: Host_Init
// LOCATION: cl_main.c (heavily inaccurate)
// PATH: Load__7CEnginebPcT2->Sys_InitGame__FPcT0Pvi->Host_Init
//
// Inits a lot of stuff.  Mallocs the initial hunk for example.
//
// It also references a logging function which I've ignored.  It's probably not vital...
*/
int Host_Init(quakeparms_t *parms) {

   host_parms = *parms;

   global_com_argc = parms->argc;
   global_com_argv = parms->argv;
   global_realtime = 0;

   Memory_Init(parms->membase, parms->memsize);

   //Voice_RegisterCvars();

   Cbuf_Init();
   Cmd_Init();
   Cvar_Init();

   Cvar_RegisterVariable(&console);

   Host_InitLocal();
   if(COM_CheckParm("-dev")) {
      Cvar_DirectSetValue(&cvar_developer, 1);
   }

   Banlist_CvarInit();
   //V_Init();
   //Chase_Init();

   COM_Init();
   //Host_ClearSaveDirectory();

   HPAK_Init();
   W_LoadWadFile("gfx.wad");
   W_LoadWadFile("fonts.wad");

   //Key_Init();
   Con_Init();
   Decal_Init();
   Mod_Init();
   NET_Init();
   Netchan_Init();
   DELTA_Init();
   SV_Init();
  // SystemWrapper_Init();
   Host_Version();

   R_InitTextures();
   HPAK_CheckIntegrity("custom");
 //  Q_memset(g_module, 0, sizeof(g_module));

   Cvar_RegisterVariable(&suitvolume);
   Cbuf_InsertText("exec valve.rc");

   Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
   host_hunklevel = Hunk_LowMark();
   giActive = DLL_ACTIVE;

   Cvar_DirectSetValue(&cvar_violence_hblood, 1);
   Cvar_DirectSetValue(&cvar_violence_hgibs, 1);
   Cvar_DirectSetValue(&cvar_violence_ablood, 1);
   Cvar_DirectSetValue(&cvar_violence_agibs, 1);

  // host_initialized = 1;
   return(1);
}

//There are many functions this calls that do NOTHING on a dedicated server.
void _Host_Frame(float arg_0) {

   static host_times_t host_times_142 = { 0, 0, 0, 0, 0, 0 };

   if(Host_FilterTime(arg_0) == 0) { return; }

   Host_ComputeFPS(host_frametime);
   Cbuf_Execute();
   host_times_142.time1 = Sys_FloatTime();
   SV_Frame();
   host_times_142.time2 = Sys_FloatTime();

   Host_CheckConnectionFailure();
   host_times_142.time3 = Sys_FloatTime();
   host_times_142.time4 = Sys_FloatTime();
   host_times_142.time5 = Sys_FloatTime();

   Host_Speeds(&host_times_142);
   //Host_UpdateStats();

   //host_killtime
}
int Host_Frame(float arg_0, int arg_4, int * arg_8) {

   if(giActive != DLL_CLOSE) {
      giActive = arg_4;
   }

   *arg_8 = 0;
   _Host_Frame(arg_0);

 //  Cbuf_Execute();

   return(giActive);
}
