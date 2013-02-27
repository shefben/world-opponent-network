#include <ctype.h>

#include "info.h"
#include "report.h"
#include "Q_memdef.h"


/*** GLOBAL LOCAL GLOBAL (YOU'LL SEE) ***/
static char serverinfo[0x200];

/*** FUNCTIONS ***/

char * Info_Serverinfo() { return(serverinfo); }

//Most of these came straight from QW.  I did go over them though.
qboolean Info_IsKeyImportant( const char *key ) {

   //maybe do memcmp to be faster.  Would it be faster?
   if(key[0] == '*' ||
      Q_strcmp(key, "name") == 0 ||
      Q_strcmp(key, "model") == 0 ||
      Q_strcmp(key, "rate") == 0 ||
      Q_strcmp(key, "cl_updaterate") == 0 ||
      Q_strcmp(key, "cl_lw") == 0 ||
      Q_strcmp(key, "cl_lc") == 0 ||
      Q_strcmp(key, "tracker") == 0) {

      return(1);
   }

   return(0);
}
char * Info_FindLargestKey(char *s) { //This one looks smashable

   char key[256];
   char value[256];
   char * o;
   int  l;
   static char largest_key[256];
   int largest_size = 0;

   *largest_key = '\0';

   if(*s == '\\') { s++; }
   while(*s != '\0') {

      int size = 0;

      o = key;
      while(*s != '\0' && *s != '\\') {
         *o++ = *s++;
      }

      l = o - key;
      *o = '\0';
      size = strlen(key);

      if(*s == '\0') {
         return(largest_key);
      }

      o = value;
      s++;
      while(*s != '\0' && *s != '\\') {
         *o++ = *s++;
      }
      *o = '\0';

      if(*s != '\0') { s++; }

      size += strlen(value);

      if((size > largest_size) && Info_IsKeyImportant(key) == 0) {
         largest_size = size;
         strcpy(largest_key, key);
      }
   }

   return(largest_key);
}
void Info_Print(char *s) {

   char key[1024];
   char value[1024];
   char * o;
   int  l;

   if(*s == '\\') { s++; }
   while(*s != '\0') {

      o = key;
      while(*s != '\0' && *s != '\\') {
         *o++ = *s++;
      }

      l = o - key;
      if(l < 20) {
         memset (o, ' ', 20-l);
         key[20] = '\0';
      }
      else {
         *o = '\0';
      }
      Con_Printf("%s", key);

      if(*s == '\0') {
         Con_Printf("%s: Key without value (%s).\n", __FUNCTION__, key);
         return;
      }

      o = value;
      s++;
      while(*s != '\0' && *s != '\\') {
         *o++ = *s++;
      }
      *o = '\0';

      if(*s != '\0') { s++; }

      Con_Printf("%s\n", value);
   }
}
void Info_RemoveKey(char *s, const char *key) {

   char * start;
   char pkey[1024];
   char value[1024];
   char * o;

   if(strstr(key, "\\")) {
      Con_Printf("%s: Slashes detected in key.\n", __FUNCTION__);
      return;
   }

   while(1) {

      start = s;
      if(*s == '\\') { s++; }
      o = pkey;
      while(*s != '\\') {

         if(*s == '\0') {
            return;
         }
         *o++ = *s++;
      }
      *o = '\0';
      s++;

      o = value;
      while(*s != '\\' && *s != '\0') {
         if(*s == '\0') { //Wait, how does this happen again?
            return;
         }
         *o++ = *s++;
      }
      *o = '\0';

      if(Q_strcmp(key, pkey) == 0) {

         Q_strcpy(start, s);
         return;
      }

      if(*s == '\0') {
         return;
      }
   }
}
void Info_RemovePrefixedKeys(char *start, char prefix) {

   char *s;
   char pkey[1024];
   char value[1024];
   char *o;

   s = start;

   while(1) {

      if(*s == '\\') { s++; }
      o = pkey;
      while(*s != '\\') {

         if(*s == '\0') {
            return;
         }
         *o++ = *s++;
      }
      *o = 0;
      s++;

      o = value;
      while(*s != '\\' && *s != '\0') {

         if(*s == '\0') {
            return;
         }
         *o++ = *s++;
      }
      *o = '\0';

      if(pkey[0] == prefix) {

         Info_RemoveKey(start, pkey);
         s = start;
      }

      if(*s == '\0') {
         return;
      }
   }

}


/* DESCRIPTION: Info_ValueForKey
// LOCATION: QW
// PATH: SDK, others
//
// We've seen the key/pairs littering quake engines before.  This just
// digs out a key from a buffer, and returns it to the requester.
//
// This is not thread safe, obviously.  With four return buffers, you can call
// this a few times before you'd have to store the key contents somewhere, but
// generally the key is pretty quickly atoi'd.
*/
HLDS_DLLEXPORT const char * Info_ValueForKey(const char *s, const char *key) {

   char pkey[1024];
   static char value[4][1024];
   static int valueindex = 0;
   char * o;

   valueindex = (valueindex + 1) % 4;
   if(*s == '\\') { s++; } //We don't want to search for the slash now.
   while(1) {

      o = pkey;
      while(*s != '\\') {

         if(*s == '\0' || o >= pkey + sizeof(pkey) - 3) {
            *(value[valueindex]) = '\0';
            return(value[valueindex]);
         }
         *o++ = *s++;
      }
      *o = '\0';
      s++;

      o = value[valueindex];

      while(*s != '\\' && *s != '\0') {
         if(*s == '\0' || o >= value[valueindex] + sizeof(value[0]) - 3) {
            *(value[valueindex]) = '\0';
            return(value[valueindex]);
         }
         *o++ = *s++;
      }
      *o = '\0';

      if(Q_strcmp(key, pkey) == 0) {
         return(value[valueindex]);
      }
      if(*s == '\0') {
         *(value[valueindex]) = '\0';
         return(value[valueindex]);
      }
      s++;
   }
}

void Info_SetValueForStarKey(char *s, const char *key, const char *value, int maxsize) {

   unsigned int c;
   int IsName;
   int IsTeam; //may get uninit warning, is safe.
   char newv[1024];
   const char * v;


   if(strstr(key, "\\") != NULL || strstr(value, "\\") != NULL) {
      Con_Printf("%s: Slashes detected in arguments.\n", __FUNCTION__);
      return;
   }
   if(strstr(key, "\"") != NULL || strstr(value, "\"") != NULL) {
      Con_Printf("%s: Quotes detected in arguments.\n", __FUNCTION__);
      return;
   }

   if(strlen(key) >= MAX_INFO_KEY) {
      Con_Printf("%s: Keys above 64 bytes are not allowed.\n", __FUNCTION__);
      return;
   }

   v = Info_ValueForKey(s, key);
   if(*v != '\0') {

      // Key exists, make sure we have enough room for new value.
      if((signed)(strlen(value) - strlen(v) + strlen(s) + 1) > maxsize) {

         //Not enough room.  Maybe we can clean something out?
         if(*(Info_ValueForKey(s, "*ver")) != '\0') {

            Con_Printf("%s: Not enough room to change key %s.  Clearing old values.\n", __FUNCTION__, key);
            Info_RemoveKey(s, "*ver");
            Info_SetValueForStarKey(s, key, value, maxsize); //try again
            return;
         }
         Con_Printf("%s: Not enough room to change key %s.\n", __FUNCTION__, key);
         return;
      }
   }

   Info_RemoveKey(s, key);
   if(value == NULL || value[0] == '\0') { return; }

   c = Q_snprintf(newv, sizeof(newv)-1, "\\%s\\%s", key, value);
   if(c > sizeof(newv)-1) { //overflow HERE too.  We know key isn't 64 bytes, so at least that's in our favor.  Of course, due to other limits, this shouldn't really be possible...
      newv[sizeof(newv)-1] = '\0';
      c = sizeof(newv)-1;
   }

   if((signed)(c + strlen(s) + 1) > maxsize) {

      Con_Printf("%s: Key pair exceeded maxsize value.\n", __FUNCTION__);
      return;
   }

   // only copy ascii values
   s += strlen(s);
   v = newv;

   if(Q_strcasecmp(key, "name") == 0) { IsName = 1; } //IsTeam won't be checked if this is true
   else if(Q_strcasecmp(key, "team") == 0) { IsName = 0; IsTeam = 1; }
   else { IsName = 0; IsTeam = 0; }

   while(*v != '\0') {
      c = *v;
      *v++;

      // client only allows highbits on name
      if(IsName == 0) {
         if(isprint(c) == 0) { c = '_'; } //Continuing is nicest, but I personally would like to know if this occurs.

         // auto lowercase team
         if(IsTeam != 0) {
            c = tolower(c);
         }
      }
      else {
         if(c <= 13) { c = '_'; } //I guess names cannot be below 13, but all other values are golden...
      }

      *s = c;
      s++;
   }
   *s = '\0';
}
void Info_SetValueForKey(char *s, const char *key, const char *value, int maxsize) {

   if(key[0] == '*') {
      Con_Printf ("%s: Can't set values of star keys.\n", __FUNCTION__);
      return;
   }

   Info_SetValueForStarKey(s, key, value, maxsize);
}

//Info_WriteToFile is in QW, but is unused by us

