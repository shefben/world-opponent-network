#include "SV_rcon.h"
#include "cmd.h"
#include "cvar.h"
#include "crc.h"
#include "NET.h"
#include "SV_chals.h"
#include "report.h"

#include "inetaton.h"
#include "libcprops-0.1.8/avl.h"

#include <time.h>

/*** GLOBALS ***/
//vars
static cvar_t cvar_sv_password = { "sv_password", "", (FCVAR_SERVER | FCVAR_PROTECTED), 0, NULL };

static cvar_t cvar_rcon_password          = {          "rcon_password",   "", FCVAR_NONE,    0, NULL };
static cvar_t cvar_sv_rcon_minfailures    = {    "sv_rcon_minfailures",  "5", FCVAR_NONE,  5.0, NULL };
static cvar_t cvar_sv_rcon_maxfailures    = {    "sv_rcon_maxfailures", "10", FCVAR_NONE, 10.0, NULL };
static cvar_t cvar_sv_rcon_minfailuretime = { "sv_rcon_minfailuretime", "30", FCVAR_NONE, 30.0, NULL };
static cvar_t cvar_sv_rcon_banpenalty     = {     "sv_rcon_banpenalty", "30", FCVAR_NONE, 30.0, NULL };

static qboolean use_rcon = 0;
static byte rcon_password[16];

/*** FUNCTIONS ***/

void SV_Rcon_ChangePassword_f() {

   int i;
   const char * ptr;
   MD5Context_t md5hash;


   if(global_cmd_source != cmd_SERVER) { return; }

   if(Cmd_Argc() != 2) {

      Con_Printf("rcon is currently %sabled.\n", (use_rcon == 0) ? "dis" : "en");
      return;
   }

   ptr = Cmd_Argv(1);
   i = strlen(ptr);
   if(i < 1) {

      use_rcon = 0;
      return;
   }

   //This might be a good place to berate people who choose bad passwords.

   //hash it
   memset(&md5hash, 0, sizeof(md5hash));
   MD5Init(&md5hash);
   MD5Update(&md5hash, ptr, i);
   MD5Final(rcon_password, &md5hash);

   use_rcon = 1;
}

int SV_Rcon_Validate() {

   MD5Context_t md5hash;
   byte temp[16];
   const char * ptr;


   //First off, check challenges.  None of this matters if we can't verify the address.
   if(SV_CheckChallenge(&global_net_from, Q_atoi(Cmd_Argv(1))) == 0) {
      return(0);
   }


   //Now check the password.
   ptr = Cmd_Argv(2);
   memset(&md5hash, 0, sizeof(md5hash));

   MD5Init(&md5hash);
   MD5Update(&md5hash, ptr, strlen(ptr));
   MD5Final(temp, &md5hash);

   if(memcmp(temp, rcon_password, sizeof(rcon_password)) == 0) { //Good boy.
      return(1);
   }

   //If we are here, password matching failed.  I'm tired.  Screw this.
   //I'll add the ban checking code later.

   return(0);
}

void SV_Rcon() {

   unsigned int i, len, total;
   char temp[0x400];
   char * ptr;


   if(use_rcon == 0) { return; } //If we do this check later on, we can conceal the fact that we DO have an rcon password.  Is that worth it?

   total = Cmd_Argc();
   if(total < 4) { return; } //not enough args to be valid.

   if(SV_Rcon_Validate() == 0) { return; } //Failed.  The validator takes care of bans already.


   //An example: rcon <challence> <password> <command>.  The first three don't matter now.

   temp[0] = '\0';
   ptr = temp;
   len = 0;

   for(i = 3; i < total; i++) {

      len += Q_snprintf(ptr, sizeof(temp) - (len + 1), "%s ", Cmd_Argv(i));
      if(sizeof(temp)-1 < len) {
         temp[sizeof(temp)-1] = '\0';
         break;
      }
      ptr = len + temp;
   }

   Cmd_ExecuteString(temp, 1);
}

void SV_RconInit() {

   Cvar_RegisterVariable(&cvar_rcon_password);
   Cvar_RegisterVariable(&cvar_sv_rcon_minfailures);
   Cvar_RegisterVariable(&cvar_sv_rcon_maxfailures);
   Cvar_RegisterVariable(&cvar_sv_rcon_minfailuretime);
   Cvar_RegisterVariable(&cvar_sv_rcon_banpenalty);

   Cvar_RegisterVariable(&cvar_sv_password);

   Cmd_AddCommand("rcon_password", SV_Rcon_ChangePassword_f);
}
void SV_RconShutdown() {

   //I realize that, since the password is transmitted in plain text, that
   //zeroing out the hash to make further recovery difficult is pretty
   //pointless, as is perhaps using a hash altogether.  But it makes me
   //sleep better at night.

   //The opimizer shouldn't interfere.
   Q_memset(rcon_password, 0, sizeof(rcon_password));
}


//regular passwords
int ServerIsPassworded() {

   const char * c;

   c = cvar_sv_password.string;
   if(c[0] == '\0' || Q_memcmp(c, "none", sizeof("none")) == 0) { return(0); }
   return(1);
}
int CheckPassword(const char * pass) {

   const char * c;

   c = cvar_sv_password.string;
   if(c[0] == '\0' || Q_memcmp(c, "none", sizeof("none")) == 0 || Q_strcmp(pass, c) == 0) { return(1); }
   return(0);
}
