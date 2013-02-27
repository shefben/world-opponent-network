//using namespace std; //I actually prefer to insert this in all decs.
#include "input.h"

/* What window does what:
//
//stdscr: nothing directly, unless you count input.
//StatsWindow: Updates every second unless menu is running.
//OutputWindow: Updates only when menu isn't running.
//
//BaseMenuWindow: All menu windows derived from this.  Separates menu from stdscr.
//QuitWindow: Small window for prompting a quit.
*/



/******** Constructor/Destructor ********/

InputOutputStats::InputOutputStats() {

   TotalMods = 0;

   CurrentTime = (unsigned int)time(NULL);
   debuglevel = 1;
   filedebuglevel = 0;
   debuglog = NULL;
   port = 27011;
   ConfigFile = "modlist.cfg";

}
int InputOutputStats::ProcessGameFileInput(char modname) {

   struct config_t cfg;
   struct config_setting_t *setting = NULL;
   config_init(&cfg);
   WON2Modlist_t modtoken;

   if(!config_read_file(&cfg, namefile.c_str())) {
      printf("Config file parsing failed.  Error on line %d: %s\n", cfg.error_line, cfg.error_text);
      config_destroy(&cfg);
      return(1);
   }
    setting = config_lookup(&cfg, "uniqueid");
    if(setting != NULL)
         modtoken->uniqueid = config_setting_get_string(setting);
    setting = config_lookup(&cfg, "game");
    if(setting != NULL)
         modtoken->game = config_setting_get_string(setting);
    setting = config_lookup(&cfg, "url_info");
    if(setting != NULL)
         modtoken->url_info = config_setting_get_string(setting);
    setting = config_lookup(&cfg, "url_dl");
    if(setting != NULL)
         modtoken->url_dl = config_setting_get_string(setting);
    setting = config_lookup(&cfg, "version");
    if(setting != NULL)
         modtoken->version = config_setting_get_string(setting);
    setting = config_lookup(&cfg, "size");
    if(setting != NULL)
         modtoken->size = config_setting_get_int(setting);
    setting = config_lookup(&cfg, "type");
    if(setting != NULL)
         modtoken->type = config_setting_get_string(setting);

         ptrMods->AddMod(modname, modtoken);
}

/* DESCRIPTION: ProcessConfigFileInput
//
// I don't know much about libconfig, but it compiled nicely enough.
// Most of this is borrowed from the examples.  I may, someday, convert
// all of the startup stuff to libraries like this.
//
*/
int InputOutputStats::ProcessConfigFileInput() {

   struct config_t cfg;
   struct config_setting_t *setting = NULL;
   int i;
   int j;
   const char *modname;
   config_init(&cfg);

   if(!config_read_file(&cfg, ConfigFile.c_str())) {
      printf("Config file parsing failed.  Error on line %d: %s\n", cfg.error_line, cfg.error_text);
      config_destroy(&cfg);
      return(1);
   }

   setting = config_lookup(&cfg, "application.timeout");
   if(setting != NULL) { boolServersTimeout = config_setting_get_bool(setting); }

     setting = config_lookup(&cfg, "application.verboseness");
   if(setting != NULL) {
      debuglevel = config_setting_get_int(setting);
      if((debuglevel < 0) || (debuglevel > 3)) {
         printf("\"verboseness\" must be a value between 0 and 3.\n");
         debuglevel = 1;
      }
      else {
         printf("Setting console debug level to \"%i\".\n", debuglevel);
      }
   }

   setting = config_lookup(&cfg, "file.name");
   if(setting != NULL) {
      addresses = config_setting_get_string(setting);
      if(strlen(addresses) > 0) {
         DebugFile.assign(addresses);
      }
      else {
         printf("file.name appears to be empty.  Using defaults");
      }
   }
   setting = config_lookup(&cfg, "file.logginglevel");
   if(setting != NULL) {
      filedebuglevel = config_setting_get_int(setting);
      if(filedebuglevel < 0) {
         printf("\"logginglevel\" shouldn't be negative.\n");
         filedebuglevel = 0;
      }
      else if(filedebuglevel > 0) {
         debuglog = fopen(DebugFile.c_str(), "wb");
         if(debuglog == NULL) {
            perror(NULL);
            filedebuglevel = -1; }
         else {
            printf("Setting file debug level to \"%i\".\n", filedebuglevel);
         }
      }
   }

   setting = config_lookup(&cfg, "mods");
   if(setting != NULL) {
      i = config_setting_length(setting);
      printf("%i mod entries.\n", i);
      while(i > 0) {
         i--;
         modname = config_setting_get_string_elem(setting, i);
         WON2Modlist_t modtoken;
         j = processgamefileinput

         switch(j) {
         case 1:
            printf("Mod failed to load.\n");
         case 0:
            printf("Mod added.\n");
            break;
         }
      }
   }

   setting = config_lookup(&cfg, "bans");
   if(setting != NULL) {
      i = config_setting_length(setting);
      printf("%i bans listed.\n", i);
      while(i > 0) {
         i--;
         j = ptrBanlist->ParseBanString(config_setting_get_string_elem(setting, i), 1);
         switch(j) {
         case 0:
            printf("Ban added.\n");
            break;
         case 1:
            printf("Ban entry already exists.  ");
         default:
            printf("Ban not added.\n");
            break;
         }
      }
   }

   config_destroy(&cfg);
   return(0);
}

/* DESCRIPTION: ProcessMOTDFileInput
//
// Load text from file, pipe text to MOTD.  Nothing to it.
//
*/
int InputOutputStats::ProcessMODFileInput(char name) {

   FILE *fp;
   char s[256]; //this hard limit is encoded in several places.
   int i;

   fp = fopen(MOTDfile.c_str(), "rb");
   if(fp == NULL) { return(1); }


   i = fread(s, 1, 255, fp);
   s[i] = '\0';

   ptrSocketClass->UpdateMOTD(s);
   fclose(fp);
   return(0);
}
