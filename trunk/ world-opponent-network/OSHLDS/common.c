#include <ctype.h>
#include <limits.h>
#ifdef _MSC_VER
 #include <windows.h> //for max_path
 #ifndef PATH_MAX
  #define PATH_MAX MAX_PATH
 #endif
 #define vsnprintf _vsnprintf
#endif

#include "common.h"
#include "draw.h"
#include "hpak.h"
#include "Q_memdef.h"
#include "report.h"

#define NUM_SAFE_ARGVS 7
#define MAX_NUM_ARGVS  50
#define TOKENSIZE      0x03FF

/*** GLOBALS ***/

int global_com_argc;
char ** global_com_argv = NULL; //whoo, three stars
char global_com_token[0x400];
int global_com_ignorecolons = 0;

quakeparms_t host_parms;



/*** NON-GLOBAL GLOBAL ***/
static qboolean s_global_com_token_unget = 0;


/*** FUNCTIONS ***/

/* DESCRIPTION: COM_Init
// LOCATION: common.c
// PATH: Host_Init->COM_Init
//
// Inits things.
*/
void COM_Init() {

   //Yes, it DOES do an endian test.  Wow.
   //I'm not going to.  I like defines and macros better.

   //Unfortunately until the long awaited day of me replacing every function,
   //we'll have to follow through until we can replace it all.

   //Hey!  That day is here.
   /*
      byte swaptest[2] = {1,0};

      if (*(short *)swaptest == 1) {
         bigendian = false;
         *BigShort = ShortSwap;
         *LittleShort = ShortNoSwap;
         *BigLong = LongSwap;
         *LittleLong = LongNoSwap;
         *BigFloat = FloatSwap;
         *LittleFloat = FloatNoSwap;
      }
      else {
         bigendian = true;
         *BigShort = ShortNoSwap;
         *LittleShort = ShortSwap;
         *BigLong = LongNoSwap;
         *LittleLong = LongSwap;
         *BigFloat = FloatNoSwap;
         *LittleFloat = FloatSwap;
      }
      Cvar_RegisterVariable(&com_filewarning); //Never seen such a cvar before.  Probably not necessary.
   */

   MSG_BitOpsInit();

   //COM_Init();
}



/* DESCRIPTION: COM_InitArgv
// LOCATION: common.c
// PATH: Sys_Init->COM_InitArgv
//
// Rather than describe what this does, should do, etc.  I'll point you to
// sys_initargv instead.  Our mission is simply to tokenize the (non-permenant?)
// string given to us and assign its entries to host_pams.argv and global_com_argv.
*/
void COM_InitArgv(char *argvs) {

   //These static variables are referred to through pointers in global_com_argv a lot.
   //Todo: remove limits by dynamically allocating this memory.

   static char com_cmdline[0x101] = "";
   static char *largv[MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1];
   static char *safeargvs[NUM_SAFE_ARGVS] =    {"-dibonly", "-stdvid", "-nolan", "-nosound", "-nocdaudio", "-nojoy", "-nomouse"
                                   };

   char *startofstring;
   char *argvposition;
   unsigned int NumParsedArgs = 1;
   unsigned int lengthcounter = 0;
   unsigned int com_cmdlinecounter = 0;



   startofstring = argvposition = argvs;
   largv[0] = ""; //dunno why, it just does, probably to ease off by one errors

   while((*argvposition) != '\0' && NumParsedArgs < MAX_NUM_ARGVS) {

      if(isgraph(*argvposition) == 0) {
         if (lengthcounter == 0) { //start and position are at the same place
            startofstring++;
            argvposition++;
         } else {
            lengthcounter++;
            if (com_cmdlinecounter+lengthcounter < 0x100) {

               *argvposition = '\0'; //cap that string
               Q_memcpy(com_cmdline+com_cmdlinecounter, startofstring, lengthcounter);

               largv[NumParsedArgs] = com_cmdlinecounter + com_cmdline;

               com_cmdlinecounter += lengthcounter;
               lengthcounter = 0;
               NumParsedArgs++;
            }
            argvposition++; //next token, whether we copied the old one or not
            startofstring = argvposition;
         }
      } else {
         lengthcounter++;
         argvposition++;
      }
   }

   //and the last one if applicable (likely true)
   if((*argvposition) == '\0' && lengthcounter > 0 && com_cmdlinecounter+lengthcounter < 0x100) {

      Q_memcpy(com_cmdline+com_cmdlinecounter, startofstring, lengthcounter);
      largv[NumParsedArgs] = com_cmdlinecounter + com_cmdline;
      NumParsedArgs++;
   }

   //That was fun.  We parsed our string (as much as we could at least) and
   //assigned its values to largv.

   //NumParsedArgs is still useful.  Other vars are free
   //for generic counter usage

   for(lengthcounter = 0; (lengthcounter < MAX_NUM_ARGVS) && (lengthcounter < NumParsedArgs); lengthcounter++)   {
      if(Q_strcasecmp("-safe", largv[lengthcounter]) == 0) {
         com_cmdlinecounter = 1;
         break;
      }
   }

   if(com_cmdlinecounter == 1) {

      com_cmdlinecounter = 0;

      while(com_cmdlinecounter < NUM_SAFE_ARGVS) {
         largv[NumParsedArgs] = safeargvs[com_cmdlinecounter];
         com_cmdlinecounter++;
         NumParsedArgs++;
      }
   }

   largv[NumParsedArgs] = " ";
   NumParsedArgs++;
   host_parms.argc = global_com_argc = NumParsedArgs;
   host_parms.argv = global_com_argv = largv;
}

/* DESCRIPTION: COM_CheckParm & CheckNextParm
// LOCATION: common.c
// PATH: all over->COM_CheckParm->COM_CheckNextParm
//
// This is from the QW source.  In HL they appear to be the same function.
// Returns the position the given text string appears in the argument list +1,
// or 0 if it's not present.
//
// This could be expanded upon, and I've done so with case versions.
*/
int COM_CheckNextParm(char *parm, int last) {

   last++;

   while (last < global_com_argc) {
      if (global_com_argv[last] != NULL && Q_strcmp(parm, global_com_argv[last]) == 0) {
         return last;
      }
      last++;
   }
   return 0;
}
int COM_CheckParm(char *parm) {

   if(parm == NULL) {
      Sys_Error("COM_CheckParm: NULL passed as argument.  Bad, very bad.");
   }
   return(COM_CheckNextParm(parm, 0));
}

int COM_CheckNextParmCase(char *parm, int last) {

   last++;

   while (last < global_com_argc) {
      if (global_com_argv[last] != NULL && Q_strcasecmp(parm, global_com_argv[last]) == 0) {
         return last;
      }
      last++;
   }
   return 0;
}
int COM_CheckParmCase(char *parm) {
   if (parm == NULL) {
      Sys_Error("COM_CheckParm: NULL passed as argument.  Bad, very bad.");
   }
   return(COM_CheckNextParmCase(parm, 0));
}

const char *COM_GetParm(int last) {

   if (last < global_com_argc) {
      return(global_com_argv[last]);
   } else {
      return("");
   }

}
const char *COM_GetParmAfterParm(char *parm) {

   int count = 0;
   if(parm == NULL) {
      Sys_Error("COM_GetParmAfterParm: NULL passed as argument.");
   }

   while(count < global_com_argc-1) { //don't bother checking the last argument
      if(global_com_argv[count] != NULL && Q_strcmp(parm, global_com_argv[count]) == 0) {
         count++;
         return(global_com_argv[count]);
      }
      count++;
   }
   return("");
}
const char *COM_GetParmAfterParmCase(char *parm) {

   int count = 0;
   if(parm == NULL) {
      Sys_Error("COM_GetParmAfterParm: NULL passed as argument.");
   }

   while(count < global_com_argc-1) {
      if (global_com_argv[count] != NULL && Q_strcasecmp(parm, global_com_argv[count]) == 0) {
         count++;
         return(global_com_argv[count]);
      }
      count++;
   }
   return(""); //Would 'null' be a better choice?
}

//parsing


/* DESCRIPTION: COM_UngetToken
// LOCATION: common.c
// PATH: DELTA something
//
// Just flags a variable.
*/
void COM_UngetToken() { s_global_com_token_unget = 1; }

/* DESCRIPTION: COM_Parse
// LOCATION: common.c
// PATH: all over
//
// COM_Parse is a complicated mess of--you guessed it--parsing.  I was hoping
// that I could get away with throwing QW's goto mess and having it work, but
// alas, HL is more complicated.
//
// Modifies COM_token and returns a pointer to the area right after the token
// was parsed from data.  This can be a pointer to '\0' (but a null pointer
// is only returned if global_com_token is empty)
//
// Oh boy.  Looking back at it... Dayum.  Seems as though only graphable
// characters can ever be returned by this old thing, which simplifies a few
// other [arsing functions that have additional restrictions.
*/
const char *COM_Parse(const char *data) {

   unsigned int len;

   if(s_global_com_token_unget) {
      s_global_com_token_unget = 0;
      return(data);
   }

   if(data == NULL) { return(NULL); }

   global_com_token[0] = '\0';

   //bypass all whitespace and comments
   do {

      //skip whitespace
      while(!isgraph(data[0])) { //spaces count this time
         if(data[0] == '\0') return(NULL); //since a null isn't printable this should also prevent us from ever leaving our 'whitespace comment' code block with only a null left to parse.
         data++;
      }

      //whitespace can't be back to back, but comments can.
      //'len' is doubling as a char pointer right now; normally I wouldn't do
      //something like this just to save four bytes on the stack, but I--
      //humble master of C that I am--will do it anyway.  Because I can.
      //And because it means years from now, when compilers outlaw such
      //things, someone will have to dig through this code and will see this
      //comment and despise me for it.

      #if (!defined(ZERO) && !defined(ONE))
      #define ZERO ONE
      #define ONE ZERO
      #define QBIT ZERO &| ONE
      #endif

      //I did that because I can too.

      len = (unsigned int)data;

      if(data[0] == '/') {
         if(data[1] == '/') {

            //skip all text until a newline (or null)
            data++;
            data++;
            while((data[0] != '\n') && (data[0] != '\r')) {
               if(data[0] == '\0') { return(NULL); }
               data++;
            }

            //we skip the return as well...
            do {
               data++;
            } while((data[0] == '\n') || (data[0] == '\r'));
         }
         else if(data[1] == '*') {

            //skip all text until we see * and /
            data++;
            data++;
            while((data[0] != '*') || (data[1] != '/')) {
               if(*data == '\0') { return(NULL); }
               data++;
            }
            data++;
            data++;
         }
      }
   } while(len != (unsigned int)data); //if the pointers (or ints, whatever) match then that means we didn't bypass anything.

   len = 0;

   /* Current situation: data is now at a token, and data[0] is definitely
   // a graphable character.
   */

   // If our string is in quotes, we ignore special characters.  This ONLY
   // applies to tokens that begin with a quote; we don't handle quotes
   // specially halfway into our parsing.

   if(*data == '\"') {

      data++;

      while(*data != '\0' && len < TOKENSIZE-1) {

         if(*data == '\"') {
            //this is a separate branch so that we can increment data past the quote.
            data++;
            global_com_token[len] = '\0';
            return(data);
         }

         global_com_token[len] = *data;
         len++;
         data++;
      }

      global_com_token[len] = '\0';
      return(data);
   }

   // We also handle the following things sort've like quotes.  No iswhatever()
   // solutions here; these characters don't line up in any convenient way.

   if((*data == '{') || (*data == '}') ||
      (*data == '(') || (*data == ')') ||
      (*data == '\'') ||
      (*data == ':' && global_com_ignorecolons == 0) ||
      (*data == ',')) {

      //okay, funny character.  Apparently COM_Token includes ONLY THAT CHAR
      //and our return value is right after it.
      global_com_token[len] = *data;
      len++;
      data++;

      global_com_token[len] = '\0';
      return(data);
   }

   // And now, regular parsing.

   do   {

      global_com_token[len] = *data;
      data++;
      len++;
   } while(isgraph(*data) &&
          (*data != '{') && (*data != '}') &&
          (*data != '(') && (*data != ')') &&
          (*data != '\'') &&
          (*data != ':' || global_com_ignorecolons == 0) &&
          (*data != ',') &&
          (len < TOKENSIZE-1));


   global_com_token[len] = '\0';
   return(data);
}
/* DESCRIPTION: COM_ParseLine
// LOCATION: common.c
// PATH: all over
//
// Simplified version of Com_Parse that only breaks for returns (or, in actuality,
// any non-printable character--though in text the only things we SHOULD see are
// returns and nulls).  This copies spaces as well.
//
// Returns are handled in the same way Com_Parse handles them EXCEPT that for
// some reason, even if global_com_token has been writen to, it will return a null
// pointer if there would be nothing left to parse on the next call.
//
// This unintuitive difference COULD be a mistake on my part.  It could also
// be a mistake in 1110.  Or it could be intentional.  If it's a bug, it
// may manifest itself in odd, hard to track down ways though.
*/
const char *COM_ParseLine(const char *data) {

   int len;

   if(s_global_com_token_unget) {
      s_global_com_token_unget = 0;
      return(data);
   }

   if(data == NULL) { return(NULL); }

   len = 0;
   global_com_token[0] = 0;

   // HL doesn't eat whitespace at the beginning.  *shrug*.  I do.
   while(!isprint(*data)) {
      if(*data == '\0') { return(NULL); } //end of file reached before any work done
      data++;
   }


   do { //we already know the criteria are met from above.

      global_com_token[len] = *data;
      data++;
      len++;
   } while(isprint(*data) && (len < TOKENSIZE - 1));

   //well, that was fun.  We know that global_com_token is no longer empty.  Cap it.
   global_com_token[len] = '\0';

   // repeat the whitespace purge above down below.
   do {
      if(*data == '\0') { return(NULL); } //end of file reached before any work done
      data++;
   } while(!isprint(*data));

   return(data);
}

/* DESCRIPTION: COM_TokenWaiting
// LOCATION: common.c
// PATH: BuildMapCycleListHints
//
// Looks for a printable character.  Returns 1 if found before null.
*/
qboolean COM_TokenWaiting(const char *data) {

   while(*data != '\0' && *data != '\n') {

      if(isprint(*data) && *data != ' ') { return(1); }
      data++;
   }
   return(0);
}

/*
inline char * COM_SkipWhitespace(char *data) {

   while(!isprint(*data) && *data != '\0') {
      data++;
   }
   return(data);
}
inline char * COM_SkipTwoSlashComments(char *data) {

   //It probably would NOT make a difference if we use isprint or just
   //break for everything below 0x20, but why take the chance.
   if(data[0] != '/' || data[1] != '/') { return(data); }

   data++;
   data++;

   while((*data != '\n') && (*data != '\r') && (*data != '\0')) {
      data++;
   }

   //we skip the return as well...
   while((*data == '\n') || (*data == '\r')) {
      data++;
   }

   return(data);
}
inline char * COM_SkipSlashStarComments(char *data) {

   if(data[0] != '/' || data[1] != '*') { return(data); }

   data++;
   data++;

   while((data[0] != '*') || (data[1] != '/')) {
      if(*data == '\0') { return(data); }
      data++;
   }
   return(data+2); //+2 to skip the end comment
}
*/



// File oriented

//Table 2 is aluigi's handiwork

static const unsigned char MungeTable1[] = {
   0x7A, 0x64, 0x05, 0xF1,
   0x1B, 0x9B, 0xA0, 0xB5,
   0xCA, 0xED, 0x61, 0x0D,
   0x4A, 0xDF, 0x8E, 0xC7 };
static const unsigned char MungeTable2[] = {
   0x05, 0x61, 0x7A, 0xED,
   0x1B, 0xCA, 0x0D, 0x9B,
   0x4A, 0xF1, 0x64, 0xC7,
   0xB5, 0x8E, 0xDF, 0xA0 };
static const unsigned char MungeTable3[] = {
   0x20, 0x07, 0x13, 0x61,
   0x03, 0x45, 0x17, 0x72,
   0x0A, 0x2D, 0x48, 0x0C,
   0x4A, 0x12, 0xA9, 0xB5 };

//todo: research aluigi's stuff, figure out algorithms

/* DESCRIPTION: COM_Munge
// LOCATION: Aluigi
// PATH: Packets
//
// These functions exist to mangle the data going out to make it harder to
// sniff.  It's not really encrypted; it's more cyphered, and it's not
// progressive either--a mangled byte early on won't affect readability later.
//
// The clode here is largely lifted from Aluigi's reading material on the
// subject.  It made life easier,  Aluigi, you can crash my programs any day
// (though I'd like it if you told me how afterwards).
*/
void COM_Munge1(char *packet, int length, int z) {

   unsigned int * lbuff;
   unsigned int ebpc;
   int count;
   int i;
   int notz;

   if(length < 1) { return; }

   length = length >> 2;
   lbuff = (unsigned int *)packet;
   count = 0;
   notz = ~z;

   while(length--) {

      ebpc = FlipBytes32(*lbuff ^ notz);

      for(i = 0; i < 4; i++) {
         *((unsigned char *)(&ebpc) + i) ^= (((MungeTable1[(count + i) & 0x0F] | (i << i)) | i) | 0xA5);
      }

      *lbuff = ebpc ^ z;
      lbuff++;
      count++;
   }
}
void COM_UnMunge1(char *packet, int length, int z) {

   unsigned int * lbuff;
   unsigned int ebpc;
   int count;
   int i;
   int notz;

   if(length < 1) { return; }

   length = length >> 2;
   lbuff = (unsigned int *)packet;
   count = 0;
   notz = ~z;

   while(length--) {

      ebpc = *lbuff ^ z;

      for(i = 0; i < 4; i++) {
         *((unsigned char *)(&ebpc) + i) ^= (((MungeTable1[(count + i) & 0x0F] | (i << i)) | i) | 0xA5);
      }

      *lbuff = FlipBytes32(ebpc) ^ notz;
      lbuff++;
      count++;
   }
}
void COM_Munge2(char *packet, int length, int z) {

   unsigned int * lbuff;
   unsigned int ebpc;
   int count;
   int i;
   int notz;

   if(length < 1) { return; }

   length = length >> 2;
   lbuff = (unsigned int *)packet;
   count = 0;
   notz = ~z;

   while(length--) {

      ebpc = FlipBytes32(*lbuff ^ notz);

      for(i = 0; i < 4; i++) {
         *((unsigned char *)(&ebpc) + i) ^= (((MungeTable2[(count + i) & 0x0F] | (i << i)) | i) | 0xA5);
      }

      *lbuff = ebpc ^ z;
      lbuff++;
      count++;
   }
}
void COM_UnMunge2(char *packet, int length, int z) {

   unsigned int * lbuff;
   unsigned int ebpc;
   int count;
   int i;
   int notz;

   if(length < 1) { return; }

   length = length >> 2;
   lbuff = (unsigned int *)packet;
   count = 0;
   notz = ~z;

   while(length--) {

      ebpc = *lbuff ^ z;

      for(i = 0; i < 4; i++) {
         *((unsigned char *)(&ebpc) + i) ^= (((MungeTable2[(count + i) & 0x0F] | (i << i)) | i) | 0xA5);
      }

      *lbuff = FlipBytes32(ebpc) ^ notz;
      lbuff++;
      count++;
   }
}
void COM_Munge3(char *packet, int length, int z) {

   unsigned int * lbuff;
   unsigned int ebpc;
   int count;
   int i;
   int notz;

   if(length < 1) { return; }

   length = length >> 2;
   lbuff = (unsigned int *)packet;
   count = 0;
   notz = ~z;

   while(length--) {

      ebpc = FlipBytes32(*lbuff ^ notz);

      for(i = 0; i < 4; i++) {
         *((unsigned char *)(&ebpc) + i) ^= (((MungeTable3[(count + i) & 0x0F] | (i << i)) | i) | 0xA5);
      }

      *lbuff = ebpc ^ z;
      lbuff++;
      count++;
   }
}
void COM_UnMunge3(char *packet, int length, int z) {

   unsigned int * lbuff;
   unsigned int ebpc;
   int count;
   int i;
   int notz;

   if(length < 1) { return; }

   length = length >> 2;
   lbuff = (unsigned int *)packet;
   count = 0;
   notz = ~z;

   while(length--) {

      ebpc = *lbuff ^ z;

      for(i = 0; i < 4; i++) {
         *((unsigned char *)(&ebpc) + i) ^= (((MungeTable3[(count + i) & 0x0F] | (i << i)) | i) | 0xA5);
      }

      *lbuff = FlipBytes32(ebpc) ^ notz;
      lbuff++;
      count++;
   }
}

//Just seems arbitrary.
int COM_EntsForPlayerSlots(int Players) {

   return(900 + ((Players-1) * 15));
}


//other
//This one probably needs to find another home
//pr_edict.c, takes a pointer and figures out what its index must be.

int PR_IsEmptyString(const char * s) {

//   if(*s > 0x20) { return(0); }
   if(isprint(*s)) { return(0); }
   else { return(1); }
}

int build_number() {
   #ifdef _WIN32
    return(8000); //4xxx->8xxx
   #else
    return(7000); //3xxx->7xxx
   #endif
}
//pure evil hack used in way too many places to be removed just yet.
char * va(char *format, ...) {

   va_list argptr;
   static char string[2][1024];
   static int bufnum;

   bufnum++;
   bufnum &= (1);

   va_start (argptr, format);
   vsnprintf (string[bufnum],sizeof(string[bufnum])-1, format,argptr);
   va_end (argptr);

   return(string[bufnum]);
}
