#include <ctype.h>

#include "delta.h"
#include "cmd.h"
#include "NET.h"
#include "pf.h"
#include "secmod.h"
#include "SV_rcon.h"
#include "report.h"
#include "Q_memdef.h"

//description_t == sv_builddelta_t?
//struct delta_s = what I call sv_builddelta_t, Eh, it happens.
/*** structs ***/
typedef struct definitionkeypair_s {

   char * name;
   unsigned int value;
} definitionkeypair_t;
typedef struct dstruct2_s { //Something, who knows.  Until later...

   int padding00;
   delta_object_3_t * desc;
} dstruct2_t;


/*** globals ***/
sv_delta_t * g_sv_delta = NULL;


/*** functions ***/

//misc
/* DESCRIPTION: DELTA_ReverseLinks
// PATH: DELTA_BuildFromLinks
//
// This plays around with pointers.
*/
void DELTA_ReverseLinks(global_sv_delta2_t ** d) {

   global_sv_delta2_t * var_4, * var_8, * var_c;


   var_c = NULL;
   var_4 = *d;

   while(var_4 != NULL) {

      var_8 = var_4->next;
      var_4->next = var_c;
      var_c = var_4;
      var_4 = var_8;
   }
   *d = var_c;
}
/* DESCRIPTION: DELTA_PrintStats
// PATH: DELTA_DumpStats_f
//
// Handily prints things out so that I can identify them.
*/
void DELTA_PrintStats(char * name, sv_builddelta_t * bd) {

   unsigned int i;


   if(bd == NULL) { return; }

   Con_Printf("Stats for \"%s\":\n", name);

   for(i = 0; i < bd->num_deltas; i++) {

      bd->deltas[i].padding3C_sendcount = 0;
      bd->deltas[i].padding40 = 0;
      Con_Printf("  %02i % 10s:  s % 5i r % 5i\n", i, bd->deltas[i].name, bd->deltas[i].padding3C_sendcount, bd->deltas[i].padding40);
   }

   Con_Printf("End of stats\n", name);
}



//clears
/* DESCRIPTION: DELTA_ClearLinks
// PATH: DELTA_BuildFromLinks
//
// Frees up a bunch of memory.
*/
void DELTA_ClearLinks(global_sv_delta2_t ** d) {

   global_sv_delta2_t * var_4, * var_8;


   var_4 = *d;

   while(var_4 !=NULL) {

      var_8 = var_4->next;
      Q_Free(var_4);
      var_4 = var_8;
   }
   *d = NULL;
}
/* DESCRIPTION: DELTA_ClearFlags
// PATH: DELTA_Check/WriteDelta
//
// Sets one short deeply embedded in a few structs to zero
*/
void DELTA_ClearFlags(sv_builddelta_t * bd) {

   unsigned int i;
   delta_object_3_t * tempdelta;

   for(i = 0; i < bd->num_deltas; i++) {

      tempdelta = &(bd->deltas[i]);
      tempdelta->flags = 0;
   }
}
/* DESCRIPTION: DELTA_ClearStats
// PATH: DELTA_ClearStats_f
//
// Zeros out data in a struct
*/
void DELTA_ClearStats(sv_builddelta_t * bd) {

   unsigned int i;


   if(bd == NULL) { return; }

   for(i = 0; i < bd->num_deltas; i++) {

      bd->deltas[i].padding3C_sendcount = 0;
      bd->deltas[i].padding40 = 0;
   }
}
/* DESCRIPTION: DELTA_FreeDescription
// PATH: SV_Shutdown, DELTA_ClearRegistrations
//
// This function frees memory.
*/
void DELTA_FreeDescription(sv_builddelta_t ** bd) { //class is guess at this point.

   if(bd == NULL || *bd == NULL) { return; }

   if((*bd)->padding00) { Q_Free((*bd)->deltas); }
   Q_Free(*bd);
   *bd = NULL;
}




//defs
typedef struct defll_s {

   struct defll_s * next;
   char * name;
   unsigned int num_defs;
   definitionkeypair_t * definitions;
} defll_t;
defll_t * g_defs = NULL;

/* DESCRIPTION: DELTA_AddDefinition
// PATH: DELTA_Init
//
// We have a global linked list.  Run through it.  If you find a match,
// assign it the data.  If you don't find a match, malloc a new one.
*/
void DELTA_AddDefinition(const char * itemname, definitionkeypair_t * passeddefs, unsigned int totalarrayelements) {

   defll_t * ptr;


   for(ptr = g_defs; ptr != NULL; ptr = ptr->next) {
      if(Q_strcasecmp(itemname, ptr->name) == 0) {

         ptr->num_defs = totalarrayelements;
         ptr->definitions = passeddefs;
         return;
      }
   }

   //We didn't find it, so add another element to the linked list.
   ptr = Q_Malloc(sizeof(defll_t));

   ptr->name = strdup(itemname);
   if(ptr->name == NULL) { Sys_Error("%s: malloc failure.", __FUNCTION__); }
   ptr->num_defs = totalarrayelements;
   ptr->definitions = passeddefs;

   //link it
   ptr->next = g_defs;
   g_defs = ptr;
}
/* DESCRIPTION: DELTA_FindDefinition
// PATH: DELTA_ParseDescription
//
// We have a global linked list again.  Run through it.  If you find a match,
// return the data.  If not, return NULL.
*/
definitionkeypair_t * DELTA_FindDefinition(char * name, unsigned int * out_index) {

   defll_t * ptr;


   for(ptr = g_defs; ptr != NULL; ptr = ptr->next) {
      if(Q_strcasecmp(name, ptr->name) == 0) {

         *out_index = ptr->num_defs;
         return(ptr->definitions);
      }
   }

   *out_index = 0;
   return(NULL);
}
/* DESCRIPTION: DELTA_ClearDefinitions
// PATH: DELTA_Shutdown
//
// We have a... Oh heck, you can figure it out.
*/
void DELTA_ClearDefinitions() {

   defll_t * ptr, * ptr2;


   ptr = g_defs;
   while(ptr != NULL) {

      ptr2 = ptr;
      ptr = ptr->next;
      Q_Free(ptr2->name);
      Q_Free(ptr2);
   }
   g_defs = NULL;
}




//encoders
typedef struct encll_s {

   struct encll_s * next;
   char * name;
   void (*Encoder)(struct sv_builddelta_s *, const char *, const char *);
} encll_t;
encll_t * g_encoders = NULL;

/* DESCRIPTION: DELTA_AddEncoder
// PATH: SDK (as pfnDeltaAddEncoder)
//
// Mallocs a node and adds it to a linked list.  There's nothing in place to
// prevent duplicate entries.
*/
HLDS_DLLEXPORT void DELTA_AddEncoder(char *name, void(*conditionalencode)(struct sv_builddelta_s *, const char *, const char *)) {

   encll_t * ptr;

   ptr = (encll_t *)Q_Malloc(sizeof(encll_t));
   if(ptr == NULL) { Sys_Error("%s: malloc failure.", __FUNCTION__); }

   ptr->name = strdup(name);
   if(ptr->name == NULL) { Sys_Error("%s: malloc failure.", __FUNCTION__); }

   ptr->Encoder = conditionalencode;
   ptr->next = g_encoders;
   g_encoders = ptr;
}
/* DESCRIPTION: DELTA_LookupEncoder
// PATH: SV_InitEncoders
//
// Returns a pointer to a function that has a certain name.
// I tried to properly cast the return value, but I couldn't figure out
// how to properly cast a returned function.
*/
void * DELTA_LookupEncoder(char * name) {

   encll_t * ptr;

   for(ptr = g_encoders; ptr != NULL; ptr = ptr->next) {

      if(Q_strcasecmp(name, ptr->name) == 0) {
         return(ptr->Encoder);
      }
   }

   return(NULL);
}
/* DESCRIPTION: DELTA_ClearEncoders
// PATH: DELTA_Shutdown
//
// Frees all memory allocated to this minimal lost of function pointers.
*/
void DELTA_ClearEncoders() {

   encll_t * ptr, * ptr2;


   ptr = g_encoders;
   while(ptr != NULL) {

      ptr2 = ptr;
      ptr = ptr->next;
      Q_Free(ptr2->name);
      Q_Free(ptr2);
   }
   g_encoders = NULL;
}




//registry
typedef struct regll_s {

   struct regll_s * next;
   char * name;
   sv_builddelta_t * deltatable;
} regll_t;
regll_t * g_deltaregistry = NULL;

/* DESCRIPTION: RegisterDescription
// PATH: none, which seems odd...
//
// Mallocs a new node.  Yawn.  Seen it all before.
*/
void DELTA_RegisterDescription(char * name) {

   regll_t * ptr;

   ptr = (regll_t *)Q_Malloc(sizeof(regll_t));
   if(ptr == NULL) { Sys_Error("%s: malloc failure.", __FUNCTION__); }

   ptr->name = strdup(name);
   if(ptr->name == NULL) { Sys_Error("%s: malloc failure.", __FUNCTION__); }

   ptr->deltatable = NULL;
   ptr->next = g_deltaregistry;
   g_deltaregistry = ptr;
}
/* DESCRIPTION: DELTA_LookupRegistration
// PATH: MSG_WriteUsercmd
//
// Return the named item.  Not much to it.
*/
sv_builddelta_t * DELTA_LookupRegistration(char * name) {

   regll_t * ptr;


   for(ptr = g_deltaregistry; ptr != NULL; ptr = ptr->next) {
      if(Q_strcasecmp(name, ptr->name) == 0) {

         return(ptr->deltatable);
      }
   }

   return(NULL);
}
/* DESCRIPTION: DELTA_ClearRegistrations
// PATH: DELTA_Shutdown
//
// Frees all memory allocated.  Noteworthy differences: doesn't free the
// delta table directly.
*/
void DELTA_ClearRegistrations() {

   regll_t * ptr, * ptr2;


   ptr = g_deltaregistry;
   while(ptr != NULL) {

      ptr2 = ptr;
      ptr = ptr->next;

      if(ptr2->deltatable != NULL) { DELTA_FreeDescription(&(ptr2->deltatable)); }
      Q_Free(ptr2->name);
      Q_Free(ptr2);
   }
   g_deltaregistry = NULL;
}
/* DESCRIPTION: DELTA_ClearStats_f
// PATH: console command
//
// I don't actually know if there's anything significant about this function,
// or if all it does is reset some pointless counters...
*/
void DELTA_ClearStats_f() {

   regll_t * ptr;


   Con_Printf("%s: Clearing delta stats.\n", __FUNCTION__);

   for(ptr = g_deltaregistry; ptr != NULL; ptr = ptr->next) {
      DELTA_ClearStats(ptr->deltatable);
   }
}
/* DESCRIPTION: DELTA_DumpStats_f
// PATH: console command
//
// Prints out some funny looking numbers.
*/
void DELTA_DumpStats_f() {

   regll_t * ptr;


   Con_Printf("%s: Printing out some crazy gibberish:\n", __FUNCTION__);

   for(ptr = g_deltaregistry; ptr != NULL; ptr = ptr->next) {
      DELTA_PrintStats(ptr->name, ptr->deltatable);
   }
}




//finds
/* DESCRIPTION: DELTA_FindField
// PATH: SDK->DELTA_Set/UnsetField
//
// Finds the INDEX (HL seems to take great care in ensuring mod writers
// don't get to much about with this data easily) corresponding to
// the passed name.  This is NOT the "FindField" mod writers get.
*/
delta_object_3_t * DELTA_FindField(sv_builddelta_t * bd, const char * fieldname) {

   unsigned int i;
   delta_object_3_t * tempdelta;

   for(i = 0; i < bd->num_deltas; i++) {

      tempdelta = &(bd->deltas[i]);
      if(Q_strcasecmp(fieldname, tempdelta->name) == 0) { return(tempdelta); }
   }

   Con_Printf("%s: Couldn't find %s.\n", __FUNCTION__, fieldname);
   return(NULL);
}
/* DESCRIPTION: DELTA_FindFieldIndex
// PATH: SDK
//
// Finds the INDEX (HL seems to take great care in ensuring mod writers
// don't get to much about with this data easily) corresponding to
// the passed name.  This is the function MOD writers know as FindField.
*/
HLDS_DLLEXPORT unsigned int DELTA_FindFieldIndex(sv_builddelta_t * bd, const char * fieldname) {

   unsigned int i;
   delta_object_3_t * tempdelta;

   for(i = 0; i < bd->num_deltas; i++) {

      tempdelta = &(bd->deltas[i]);
      if(Q_strcasecmp(fieldname, tempdelta->name) == 0) { return(i); }
   }

   Con_Printf("%s: Couldn't find %s.\n", __FUNCTION__, fieldname);
   return(0xFFFFFFFF);
}
/* DESCRIPTION: DELTA_FindOffset
// PATH: DELTA_ParseField
//
// Goes through a list of small, little understood eight byte structures
// looking for a matching name.
*/
int DELTA_FindOffset(int max, definitionkeypair_t * defs, char * token) {

   int i;


   for(i = 0; i < max; i++) {

      if(Q_strcasecmp(defs[i].name, token) == 0) {
         return(defs[i].value);
      }
   }

   Sys_Error("%s: Couldn't find offset for %s,", __FUNCTION__, token);
}




//counts
/* DESCRIPTION: DELTA_CountLinks
// PATH: DELTA_BuildFromLinks
//
// Adds up the number of delta->next's we go through.
*/
unsigned int DELTA_CountLinks(const global_sv_delta2_t * d) {

   int i;


   i = 0;
   while(d != NULL) {

      d = d->next;
      i++;
   }
   return(i);
}
/* DESCRIPTION: DELTA_CountSendFields
// LOCATION:
// PATH: DELTA_Check/Writedelta
//
// Counts.  This identifies the first bit in 'flags' as a 'SendField'.
*/
unsigned int DELTA_CountSendFields(sv_builddelta_t * bd) {

   unsigned int i, total;


   total = 0;

   for(i = 0; i < bd->num_deltas; i++) {
      if((bd->deltas[i].flags & 0x01) != 0) {
         bd->deltas[i].padding3C_sendcount++;
         total++;
      }
   }
   return(total);
}




/*processing*/
//mark
/* DESCRIPTION: DELTA_Set/UnsetField
// PATH: SDK
//
// Adjusts one teensy little bit in one of the builddelta's deltas.
*/
HLDS_DLLEXPORT void DELTA_SetField(sv_builddelta_t * bd, const char * fieldname) {

   delta_object_3_t * tempdelta;


   tempdelta = DELTA_FindField(bd, fieldname);
   if(tempdelta != NULL) {
      tempdelta->flags |= 0x01;
   }
}
HLDS_DLLEXPORT void DELTA_UnsetField(sv_builddelta_t * bd, const char * fieldname) {

   delta_object_3_t * tempdelta;


   tempdelta = DELTA_FindField(bd, fieldname);
   if(tempdelta != NULL) {
      tempdelta->flags &= ~0x01;
   }
}
/* DESCRIPTION: DELTA_Set/UnsetFieldByIndex
// PATH: SDK
//
// Adjusts one teensy little bit in one of the builddelta's deltas.  This one
// however doesn't have to go hunting for the right index.
*/
HLDS_DLLEXPORT void DELTA_SetFieldByIndex(sv_builddelta_t * bd, unsigned int fieldnunber) {
   bd->deltas[fieldnunber].flags |= 0x01;
}
HLDS_DLLEXPORT void DELTA_UnsetFieldByIndex(sv_builddelta_t * bd, unsigned int fieldnunber) {
   bd->deltas[fieldnunber].flags &= ~0x01;
}
/* DESCRIPTION: DELTA_MarkSendFields
// PATH: DELTA_Check/WriteDelta
//
// This identifies arg_0 and 4 as character arrays, perhaps loaded files,
// and the delta structs we have as being pointers to data within them.
//
// If the two blocks of data differ in certain areas (old state/new state?), we
// set a bit that tells the engine to send the client the newer data.
*/
void DELTA_MarkSendFields(char * arg_0, char * arg_4, sv_builddelta_t * bd) {

   int var_20;
   int i;
   delta_object_3_t * tempdelta;
   int var_1c_ID;

   var_20 = bd->num_deltas;
   for(i = 0; i < var_20; i++) {

      tempdelta = &(bd->deltas[i]);
      var_1c_ID = tempdelta->padding00 & 0x7FFFFFFF;

      switch(var_1c_ID) {

      case 0x01:
         if(arg_0[tempdelta->padding24] != arg_4[tempdelta->padding24]) {
            tempdelta->flags |= 0x01;
         }
         break;
      case 0x02:
         if(Q_memcmp(arg_0+tempdelta->padding24, arg_4+tempdelta->padding24, 2) != 0) {
            tempdelta->flags |= 0x01;
         }
         break;

      case 0x04:
      case 0x08:
      case 0x10:
         if(Q_memcmp(arg_0+tempdelta->padding24, arg_4+tempdelta->padding24, 4) != 0) {
            tempdelta->flags |= 0x01;
         }
         break;

      case 0x20:


         //Messy.  We are comparing floats that we have multiplied by 100 and THEN cast to ints.
         if( ((int)((*(float *)(arg_0+tempdelta->padding24)) * 100.0)) !=
             ((int)((*(float *)(arg_4+tempdelta->padding24)) * 100.0)) ) {

            tempdelta->flags |= 0x01;
         }
         break;

      case 0x40:

         if( ((int)((*(float *)(arg_0+tempdelta->padding24)) * 1000.0)) !=
             ((int)((*(float *)(arg_4+tempdelta->padding24)) * 1000.0)) ) {

            tempdelta->flags |= 0x01;
         }
         break;

      case 0x80: //This appears to be a Q_strcasecmp, written out.
         if(Q_strcasecmp(arg_0+tempdelta->padding24, arg_4+tempdelta->padding24) != 0) {
            tempdelta->flags |= 0x01;
         }
         break;

      default:
         Con_Printf("%s: Bad field type %i.\n", __FUNCTION__, var_1c_ID);
      }
   }

   if(bd->ConditionalEncoder != NULL) {
      bd->ConditionalEncoder(bd, arg_0, arg_4);
   }
}
/* DESCRIPTION: DELTA_SetSendFlagBit
// PATH: DELTA_WriteDelta
//
// This is a weird one.  We are passed two buffers---one is an eight byte buffer
// that we write bits into, the other is an int where we write the position
// of the last set bit.
//
// We can conclude from this that there is a max DELTA limit of 64 per
// builddelta.  It's hard to explain what the function does--look at it instead.
// It's weird, but not verycomplicated.
*/
void DELTA_SetSendFlagBits(sv_builddelta_t * bd, byte * outgoing_bytes, int * totalbytes) {

   //totalbytes MUST be representabe by three bits.
   //ougoing_bytes appears to be a fixed sixe of eight.
   int var_18_total, var_8, var_c;
   unsigned int var_14, var_10;
   delta_object_3_t * tempdelta;

   Q_memset(outgoing_bytes, 0, 0x08);
   var_18_total = bd->num_deltas;
   var_c = -1;

   for(var_8 = var_18_total - 1; var_8 >= 0; var_8--) {

      tempdelta = &(bd->deltas[var_8]);
      if((tempdelta->flags & 0x01) != 0) {

         if(var_c == -1) { var_c = var_8; }
         var_10 = var_8 >> 3; //bytes
         var_14 = var_8 & 0x07; //bits
         outgoing_bytes[var_10] |= (1 << var_14); //set the appropriate bit.  Possible endian trouble based on the ASM here.
      }
   }

   *totalbytes = 1 + (var_c>>3);
}
/* DESCRIPTION: DELTA_CheckDelta
// PATH: SV_WriteClientdataToMessage
//
// resets and remarks various bits in preparation for data sendage.
*/
unsigned int DELTA_CheckDelta(char * arg_0, char * arg_4, sv_builddelta_t * bd) {

   DELTA_ClearFlags(bd);
   DELTA_MarkSendFields(arg_0, arg_4, bd);

   return(DELTA_CountSendFields(bd));
}
/* DESCRIPTION: DELTA_TestDelta
// PATH: SV_FindBestBaseline
//
// Similar again to MarkSendFields, the main mystery here is in the
// returns.  It is not known why we are summing up various elements, or
// really what we are summing up.
*/
int DELTA_TestDelta(char * arg_0, char * arg_4, sv_builddelta_t * bd) {

   int var_20, var_2C, var_30;
   int i;
   int var_1c_ID;
   delta_object_3_t * tempdelta;


   var_2C = 0;
   var_30 = -1;
   var_20 = bd->num_deltas;
   for(i = 0; i < var_20; i++) {

      tempdelta = &(bd->deltas[i]);
      var_1c_ID = tempdelta->padding00 & 0x7FFFFFFF;

      switch(var_1c_ID) {

      case 0x01:
         if(arg_0[tempdelta->padding24] != arg_4[tempdelta->padding24]) {
            var_2C += tempdelta->padding2C;
            var_30 = i;
         }
         break;

      case 0x02:
         if(*(uint16 *)(arg_0+tempdelta->padding24) != *(uint16 *)(arg_4+tempdelta->padding24)) {
            var_2C += tempdelta->padding2C;
            var_30 = i;
         }
         break;

      case 0x04:
      case 0x08:
      case 0x10:
         if(*(uint32 *)(arg_0+tempdelta->padding24) != *(uint32 *)(arg_4+tempdelta->padding24)) {
            var_2C += tempdelta->padding2C;
            var_30 = i;
         }
         break;

      case 0x20:
         if( ((int)((*(float *)(arg_0+tempdelta->padding24)) * 100.0)) !=
             ((int)((*(float *)(arg_4+tempdelta->padding24)) * 100.0)) ) {
            var_2C += tempdelta->padding2C;
            var_30 = i;
         }
         break;

      case 0x40:
         if( ((int)((*(float *)(arg_0+tempdelta->padding24)) * 1000.0)) !=
             ((int)((*(float *)(arg_4+tempdelta->padding24)) * 1000.0)) ) {

            var_2C += tempdelta->padding2C;
            var_30 = i;
         }
         break;

      case 0x80: //This appears to be a Q_strcasecmp, written out.
         if(Q_strcasecmp(arg_0+tempdelta->padding24, arg_4+tempdelta->padding24) != 0) {
            tempdelta->flags |= 0x01;
            var_2C += (strlen(arg_4+tempdelta->padding24)+1) * 8;
         }
         break;

      default:
         Con_Printf("%s: Bad field type %i.\n", __FUNCTION__, var_1c_ID);
      }
   }

   if(var_30 != -1) {
      var_2C += var_30  * 72;
   }
   return(var_2C);
}


//write
/* DESCRIPTION: DELTA_WriteMarkedFields
// PATH: DELTA_WriteDelta
//
// Several parallels to MarkSendFields are here.  arg_4 is identified as being
// the 'newer' data.
*/
void DELTA_WriteMarkedFields( /*char * unused_arg_0,*/ char * arg_4, sv_builddelta_t * bd) {

   int var_2C;
   int i;
   int var_10_ID, var_c_MSB, temp;
   char * var_20;
   delta_object_3_t * tempdelta;


   var_2C = bd->num_deltas;
   for(i = 0; i < var_2C; i++) {

      tempdelta = &(bd->deltas[i]);
      if((tempdelta->flags & 0x01) == 0) { continue; }

      var_c_MSB = tempdelta->padding00 & 0x80000000;
      var_10_ID = tempdelta->padding00 & 0x7FFFFFFF;

      switch(var_10_ID) {

      case 0x01:
         if(var_c_MSB) {
            MSG_WriteSBits((int32)((signed char)(tempdelta->padding30 * (signed)arg_4[tempdelta->padding24])), tempdelta->padding2C);
         }
         else {
            MSG_WriteBits((uint32)((byte)(tempdelta->padding30 * arg_4[tempdelta->padding24])), tempdelta->padding2C);
         }
         break;

      case 0x02:
         if(var_c_MSB) {
            MSG_WriteSBits((int32)((int16)(tempdelta->padding30 * (*(int16 *)(arg_4+tempdelta->padding24)))), tempdelta->padding2C);
         }
         else {
            MSG_WriteBits((uint32)((uint16)(tempdelta->padding30 * (*(uint16 *)(arg_4+tempdelta->padding24)))), tempdelta->padding2C);
         }
         break;

      case 0x04:
         temp = (*(float *)(arg_4+tempdelta->padding24)) * tempdelta->padding30;

         if(var_c_MSB) {
            MSG_WriteSBits(temp, tempdelta->padding2C);
         }
         else {
            MSG_WriteBits((unsigned)temp, tempdelta->padding2C);
         }
         break;

      case 0x08:
         if(var_c_MSB) {
            MSG_WriteSBits((tempdelta->padding30 * (*(int32 *)(arg_4+tempdelta->padding24))), tempdelta->padding2C);
         }
         else {
            MSG_WriteBits((tempdelta->padding30 * (*(uint32 *)(arg_4+tempdelta->padding24))), tempdelta->padding2C);
         }
         break;

      case 0x10:
         MSG_WriteBitAngle((*(float *)(arg_4+tempdelta->padding24)), tempdelta->padding2C);
         break;

      case 0x20:
         MSG_WriteSBits((int32)(global_sv.time0c * 100.0) - (int32)((*(float *)(arg_4+tempdelta->padding24)) * 100.0), 8);
         break;

      case 0x40:
         MSG_WriteSBits((int32)(global_sv.time0c * tempdelta->padding30) - (int32)((*(float *)(arg_4+tempdelta->padding24)) * tempdelta->padding30), tempdelta->padding2C);
         break;

      case 0x80:
         //How in the hell could var_20 ever ever ever legitamitely equal NULL?
         for(var_20 = arg_4+tempdelta->padding24; var_20 != NULL && *var_20 != '\0'; var_20++) {

            MSG_WriteBits((signed)(*var_20), 8);
         }
         MSG_WriteBits(0, 8); //Yes, we DO write the null.
         break;

      default:
         Con_Printf("%s: Unknown send field type %i.\n", __FUNCTION__, var_10_ID);
      }
   }
}
/* DESCRIPTION: DELTA_WriteDelta
// PATH: lots
//
// Gets bits, then writes them.  Interesting things to note: passed a function...
// The char *s are opaque values.  They are structs, and the structure's...
// structure is defined in those delta files.  Since the structure isn't known
// at compiletime, we have to slowly dig out the values we want with the
// definition we're given.
//
// Wow.  There's gotta be a better way.
*/
int DELTA_WriteDelta(char * arg_0, char * arg_4, int force, sv_builddelta_t * bd, void (*function)()) {

   unsigned int fields, var_8, i;
   byte var_18[0x08], * var_1c;


   DELTA_ClearFlags(bd);
   DELTA_MarkSendFields(arg_0, arg_4, bd);
   fields = DELTA_CountSendFields(bd);
   if(fields == 0 && force == 0) { return(1); }

   DELTA_SetSendFlagBits(bd, var_18, &var_8);

   if(function != NULL) { function(); }

   MSG_WriteBits(var_8, 3);
   var_1c = var_18;

   for(i = 0; i < var_8; i++, var_1c++) {
      MSG_WriteBits(*var_1c, 8);
   }

   DELTA_WriteMarkedFields(arg_4, bd);
   return(1);
}


//parse
/* DESCRIPTION: DELTA_BuildFromLinks
// PATH: DELTA_ParseDescription
//
// This function's purpose is unknown, but it does give the dimensions of
// some unpopulated structures.
*/
sv_builddelta_t * DELTA_BuildFromLinks(global_sv_delta2_t ** d) {

   unsigned int var_14;
   int var_18;

   global_sv_delta2_t * var_10;

   sv_builddelta_t * var_c;
   delta_object_3_t * var_4, * var_8;

   var_c = Q_Malloc(sizeof(sv_builddelta_t));
   Q_memset(var_c, 0, sizeof(sv_builddelta_t));

   DELTA_ReverseLinks(d);
   var_14 = DELTA_CountLinks(*d);

   var_4 = Q_Malloc(var_14 * sizeof(delta_object_3_t));
   Q_memset(var_4, 0, sizeof(delta_object_3_t));

   for(var_10 = *d, var_18 = 0; var_10 != NULL; var_10 = var_10->next) {

      var_8 = &var_4[var_18];
      var_18++;
      Q_memcpy(var_8, var_10->data, 0x44);
      Q_Free(var_10->data);
      var_10->data = NULL;
   }

   DELTA_ClearLinks(d);
   var_c->padding00 = 1;
   var_c->num_deltas = var_14;
   var_c->deltas = var_4;

   return(var_c);
}
/* DESCRIPTION: DELTA_ParseType
// PATH: SV_ParseField
//
// Parses data for a token, and translates the token into a bit.
// This function kindly names all of the 0x01,2,4,etc values.
*/
int DELTA_ParseType(uint32 * OUT_type, const char ** data) { //The int * could be a delta_t *

   while(1) {

      *data = COM_Parse(*data);
      if(global_com_token[0] == '\0') {

         Con_Printf("%s: Expecting fieldtype info.\n", __FUNCTION__);
         return(0);
      }

      if(Q_strcasecmp("|", global_com_token) == 0) { continue; }
      if(Q_strcasecmp(",", global_com_token) == 0) { return(1); }
      if(Q_strcasecmp("DT_SIGNED", global_com_token) == 0) {
         *OUT_type |= 0x80000000;
         continue;
      }
      if(Q_strcasecmp("DT_BYTE", global_com_token) == 0) {
         *OUT_type |= 0x01;
         continue;
      }
      if(Q_strcasecmp("DT_SHORT", global_com_token) == 0) {
         *OUT_type |= 0x02;
         continue;
      }
      if(Q_strcasecmp("DT_FLOAT", global_com_token) == 0) {
         *OUT_type |= 0x04;
         continue;
      }
      if(Q_strcasecmp("DT_INTEGER", global_com_token) == 0) {
         *OUT_type |= 0x08;
         continue;
      }
      if(Q_strcasecmp("DT_ANGLE", global_com_token) == 0) {
         *OUT_type |= 0x10;
         continue;
      }
      if(Q_strcasecmp("DT_TIMEWINDOW_8", global_com_token) == 0) {
         *OUT_type |= 0x20;
         continue;
      }
      if(Q_strcasecmp("DT_TIMEWINDOW_BIG", global_com_token) == 0) {
         *OUT_type |= 0x40;
         continue;
      }
      if(Q_strcasecmp("DT_STRING", global_com_token) == 0) {
         *OUT_type |= 0x80;
         continue;
      }

      Sys_Error("%s: Unknown type \"%s\".", __FUNCTION__, global_com_token);
   }
}
/* DESCRIPTION: DELTA_ParseField
// PATH: DELTA_ParseOneField
//
// By now you should know the routine. Call COM_Parse a bunch and strcmp
// a bunch until various fields are filled.
*/
int DELTA_ParseField(int num_array, definitionkeypair_t * def_array, dstruct2_t * arg_8, const char ** data) {

   int var_4;


   if(Q_strcasecmp("DEFINE_DELTA", global_com_token) == 0) { var_4 = 0; }
   else if(Q_strcasecmp("DEFINE_DELTA_POST", global_com_token) == 0) { var_4 = 1; }
   else { Sys_Error("%s: unrecognized field \"%s\".", __FUNCTION__, global_com_token); }


   *data = COM_Parse(*data);
   if(Q_strcasecmp("(", global_com_token) != 0) {
      Sys_Error("%s: Expecting \"(\", got %s.", __FUNCTION__, global_com_token);
   }

   *data = COM_Parse(*data);
   if(global_com_token[0] == '\0') {
      Sys_Error("%s: Expecting \"(\", got %s.", __FUNCTION__, global_com_token);
   }

   Q_strncpy(arg_8->desc->name, global_com_token, sizeof(arg_8->desc->name)-1);
   arg_8->desc->name[sizeof(arg_8->desc->name)-1] = '\0';
   arg_8->desc->padding24 = DELTA_FindOffset(num_array, def_array, global_com_token);

   *data = COM_Parse(*data);
   if(DELTA_ParseType(&(arg_8->desc->padding00), data) == 0) { return(0); }

   *data = COM_Parse(*data);
   arg_8->desc->padding28 = 1;
   arg_8->desc->padding2C = Q_atoi(global_com_token);

   *data = COM_Parse(*data);
   *data = COM_Parse(*data);
   arg_8->desc->padding30 = Q_atof(global_com_token);

   if(var_4 == 0) {
      arg_8->desc->padding34 = 1;
   }
   else {

      *data = COM_Parse(*data);
      *data = COM_Parse(*data);
      arg_8->desc->padding34 = Q_atof(global_com_token);
   }

   *data = COM_Parse(*data);
   if(Q_strcasecmp(")", global_com_token) != 0) {
      Con_Printf("%s: Expecting \")\", got %s.", __FUNCTION__, global_com_token);
      return(0);
   }

   *data = COM_Parse(*data);
   if(Q_strcasecmp(",", global_com_token) != 0) {
      COM_UngetToken();
   }

   return(1);
}
/* DESCRIPTION: DELTA_ParseOneField
// PATH: DELTA_ParseDescription
//
// This seems to exist to prep data for a call to DELTA_ParseField.
*/
int DELTA_ParseOneField(const char ** arg_0, global_sv_delta2_t ** arg_4, int arg_8, definitionkeypair_t * arg_c) {

   dstruct2_t var_8;
   global_sv_delta2_t * var_4 = NULL;


   while(1) {

      if(Q_strcasecmp("}", global_com_token) == 0) {

         COM_UngetToken();
         return(1);
      }
      *arg_0 = COM_Parse(*arg_0);
      if(global_com_token[0] == '\0') { return(1); }


      var_8.padding00 = 0;
      var_8.desc = (delta_object_3_t *)Q_Malloc(sizeof(delta_object_3_t));
      Q_memset(var_8.desc, 0, sizeof(delta_object_3_t));

      if(DELTA_ParseField(arg_8, arg_c, &var_8, arg_0) == 0) { return(0); }

      var_4 = Q_Malloc(sizeof(sv_delta_t));
      var_4->data = var_8.desc;
      var_4->next = (*arg_4);
      (*arg_4) = var_4;
   }
}
/* DESCRIPTION: DELTA_SkipDescription
// PATH: DELTA_ParseDescription
//
// This blazes through tokens until hitting '}'.  Then it returns.
*/
void DELTA_SkipDescription(const char ** arg_0) {

   *arg_0 = COM_Parse(*arg_0);
   while(global_com_token[0] != '\0') {

      if(Q_strcasecmp("}", global_com_token) == 0) { return; }
      *arg_0 = COM_Parse(*arg_0);
   }

   Sys_Error("%s: Error skipping description.", __FUNCTION__);
}
/* DESCRIPTION: DELTA_ParseDescription
// PATH: DELTA_Load
//
// Bit odd, mostly just seems to parse tokens and copy them around.
*/
int DELTA_ParseDescription(const char * searchstring, sv_builddelta_t ** arg_4, const byte * file) {

   global_sv_delta2_t * var_8 = NULL;
   definitionkeypair_t * var_c;
   int var_50 = 0;
   char var_2C[0x20] = "";
   char var_4C[0x20];
   const char * var_4;

   if(arg_4 == NULL) { Sys_Error("%s: NULL description_t.", __FUNCTION__); }
   if(file == NULL || file[0] == '\0') { Sys_Error("%s: empty data stream.", __FUNCTION__); }

   *arg_4 = NULL;

   var_4 = file;
   while(1) {

      var_4 = COM_Parse(var_4);
      if(global_com_token[0] == '\0') { break; }

      if(Q_strcasecmp(searchstring, global_com_token) != 0) {
         DELTA_SkipDescription(&var_4);
         continue;
      }

      var_c = DELTA_FindDefinition(global_com_token, &var_50);
      if(var_c == NULL) { Sys_Error("%s: unknown data type %s.", __FUNCTION__, global_com_token); }

      var_4 = COM_Parse(var_4);
      if(global_com_token[0] == '\0') { Sys_Error("%s: unknown decoder %s.", __FUNCTION__, global_com_token); }

      if(Q_strcasecmp("none", global_com_token) != 0) {

         strncpy(var_4C, global_com_token, sizeof(var_4C)-1);
         var_4C[sizeof(var_4C)-1] = '\0';

         var_4 = COM_Parse(var_4);
         if(global_com_token[0] == '\0') { Sys_Error("%s: Expecting decoder.", __FUNCTION__); }
         strncpy(var_2C, global_com_token, sizeof(var_2C)-1);
         var_2C[sizeof(var_2C)-1] = '\0';
      }

      while(1) {

         var_4 = COM_Parse(var_4);
         if(global_com_token[0] == '\0') { break; }
         if(Q_strcasecmp("}", global_com_token) == 0) { break; }
         if(Q_strcasecmp("{", global_com_token) != 0) {
            Con_Printf("%s: Expecting \"{\", got %s.", __FUNCTION__, global_com_token);
            return(0);
         }
         if(DELTA_ParseOneField(&var_4, &var_8, var_50, var_c) == 0) { return(0); }
      }
   }

   (*arg_4) = DELTA_BuildFromLinks(&var_8);

   if(var_2C[0] != '\0') {

      Q_strncpy((*arg_4)->name, var_2C, sizeof((*arg_4)->name)-1);
      (*arg_4)->name[sizeof((*arg_4)->name)-1] = '\0';
      (*arg_4)->ConditionalEncoder = NULL;
   }
   return(1);
}
/* DESCRIPTION: DELTA_Load
// PATH: SV_InitDeltas->SV_RegisterDelta
//
//Loads a file and hands it off to the parsing.
*/
int DELTA_Load(char * arg_0, sv_builddelta_t ** arg_4, char * filename) {

   byte * file;
   int i;

   file = COM_LoadFileForMe(filename, NULL);
   if(file == NULL) {
      Sys_Error("%s: Couldn't load file %s.", __FUNCTION__, filename);
   }

   i = DELTA_ParseDescription(arg_0, arg_4, file);

   if(file != NULL) { //Why shouldn't it?
      COM_FreeFile(file);
   }

   return(i);
}
/* DESCRIPTION: DELTA_ParseDelta
// PATH: MSG_ReadUsercmd
//
// The most complicated of all of the delta parsing/writing functions,
// and the one I'm least sure of.  Plays fast and loose with pointers and
// casts, but what I said for the earlier functions holds here too.
*/
int DELTA_ParseDelta(char * arg_0, char * arg_4, sv_builddelta_t * bd) {

   int var_58_bitcount;
   unsigned int i, var_24_max;
   int var_28_ID, var_2C_msb, var_1C_bit;

   unsigned int var_14; //temp int
   char var_10[8], *var_20; //temp char *
   delta_object_3_t * tempdelta;


   var_58_bitcount = MSG_CurrentBit();
   Q_memset(var_10, 0, sizeof(var_10));

   var_14 = MSG_ReadBits(3);
   var_20 = var_10;
   for(i = 0; i < var_14; var_20++, i++) {
      *var_20 = MSG_ReadBits(8);
   }



   var_24_max = bd->num_deltas;
   for(i = 0; i < var_24_max; i++) {

      tempdelta = &(bd->deltas[i]);
      var_28_ID = tempdelta->padding00 & 0x7FFFFFFF;
      var_1C_bit = var_10[i/8] & (1 << (i%8));

      if(var_1C_bit) {

         tempdelta->padding40++;
         var_2C_msb = tempdelta->padding00 & 0x80000000;

         switch(var_28_ID) {

         case 0x01:
            if(var_2C_msb) { //Of interest: the asm converts to an int between multiplying and dividing, possibly losing some precision
               (*(signed char *)(arg_4+tempdelta->padding24)) = MSG_ReadSBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            else {
               (*(byte *)(arg_4+tempdelta->padding24)) = MSG_ReadBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            break;

         case 0x02:
            if(var_2C_msb) {
               (*(int16 *)(arg_4+tempdelta->padding24)) = MSG_ReadSBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            else {
               (*(uint16 *)(arg_4+tempdelta->padding24)) = MSG_ReadBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            break;

         case 0x04: //fild instead of fld.  Hmmmm...
            if(var_2C_msb) {
               (*(float *)(arg_4+tempdelta->padding24)) = MSG_ReadSBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            else {
               (*(float *)(arg_4+tempdelta->padding24)) = MSG_ReadBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            break;

         case 0x08:
            if(var_2C_msb) {
               (*(int32 *)(arg_4+tempdelta->padding24)) = MSG_ReadSBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            else {
               (*(uint32 *)(arg_4+tempdelta->padding24)) = MSG_ReadBits(tempdelta->padding2C) / tempdelta->padding30 * tempdelta->padding34;
            }
            break;

         case 0x10:
            (*(float *)(arg_4+tempdelta->padding24)) = MSG_ReadBitAngle(tempdelta->padding2C);
            break;

         case 0x20: //both of these call upon that odd byte_13AA20 structure.  Seems the best thing I can do is pretend I never saw it.
            (*(float *)(arg_4+tempdelta->padding24)) = (100.0 - MSG_ReadSBits(8)) / 100.0;
            break;

         case 0x40:
            (*(float *)(arg_4+tempdelta->padding24)) = (tempdelta->padding30 - MSG_ReadSBits(tempdelta->padding2C)) / tempdelta->padding30;
            break;

         case 0x80:

            var_20 = (arg_4+tempdelta->padding24);
            while(1) {

               var_14 = (byte)MSG_ReadBits(8);
               *var_20 = (byte)var_14;
               if((byte)var_14 == '\0') { break; }
               var_20++;
            }
            break;

         default:
            Con_Printf("%s: unparseable field type %i.\n", __FUNCTION__, var_28_ID);
         }
      }
      else {

         switch(var_28_ID) {

         case 0x01:
            arg_4[tempdelta->padding24] = arg_0[tempdelta->padding24];
            break;

         case 0x02:
            (*(uint16 *)(arg_4+tempdelta->padding24) = *(uint16 *)(arg_0+tempdelta->padding24));
            break;

         case 0x08:
            (*(uint32 *)(arg_4+tempdelta->padding24) = *(uint32 *)(arg_0+tempdelta->padding24));
            break;

         case 0x04:
         case 0x10:
         case 0x20:
         case 0x40:
            (*(float *)(arg_4+tempdelta->padding24) = *(float *)(arg_0+tempdelta->padding24));
            break;

         case 0x80:
            strcpy(arg_4+tempdelta->padding24, arg_0+tempdelta->padding24);
            break;

         default:
            Con_Printf("%s: unparseable field type %i.\n", __FUNCTION__, var_28_ID);
         }
      }
   }

   return(MSG_CurrentBit() - var_58_bitcount);
}


definitionkeypair_t g_EventDataDefinition[] = { { "entindex",  0x04 },
                                                { "origin[0]", 0x08 },
                                                { "origin[1]", 0x0C },
                                                { "origin[2]", 0x10 },
                                                { "angles[0]", 0x14 },
                                                { "angles[1]", 0x18 },
                                                { "angles[2]", 0x1C },
                                                { "fparam1",   0x30 },
                                                { "fparam2",   0x34 },
                                                { "iparam1",   0x38 },
                                                { "iparam2",   0x3C },
                                                { "bparam1",   0x40 },
                                                { "bparam2",   0x44 },
                                                { "ducking",   0x2C } }; //0x0e
definitionkeypair_t g_EntityDataDefinition[] = { { "startpos[0]",     0xE4  },
                                                 { "startpos[1]",     0xE8  },
                                                 { "startpos[2]",     0xEC  },
                                                 { "endpos[0]",       0xF0  },
                                                 { "endpos[1]",       0xF4  },
                                                 { "endpos[2]",       0xF8  },
                                                 { "impacttime",      0xFC  },
                                                 { "starttime",       0x100 },
                                                 { "origin[0]",       0x10  },
                                                 { "origin[1]",       0x14  },
                                                 { "origin[2]",       0x18  },
                                                 { "angles[0]",       0x1C  },
                                                 { "angles[1]",       0x20  },
                                                 { "angles[2]",       0x24  },
                                                 { "modelindex",      0x28  },
                                                 { "frame",           0x30  }, //0x10
                                                 { "movetype",        0x58  },
                                                 { "colormap",        0x34  },
                                                 { "skin",            0x38  },
                                                 { "solid",           0x3A  },
                                                 { "scale",           0x40  },
                                                 { "effects",         0x3C  },
                                                 { "sequence",        0x2C  },
                                                 { "animtime",        0x5C  },
                                                 { "framerate",       0x60  },
                                                 { "controller[0]",   0x68  },
                                                 { "controller[1]",   0x69  },
                                                 { "controller[2]",   0x6A  },
                                                 { "controller[3]",   0x6B  },
                                                 { "blending[0]",     0x6C  },
                                                 { "blending[1]",     0x6D  },
                                                 { "body",            0x64  }, //0x20
                                                 { "owner",           0x98  },
                                                 { "rendermode",      0x48  },
                                                 { "renderamt",       0x4C  },
                                                 { "renderfx",        0x54  },
                                                 { "rendercolor.r",   0x50  },
                                                 { "rendercolor.g",   0x51  },
                                                 { "rendercolor.b",   0x52  },
                                                 { "weaponmodel",     0xB4  },
                                                 { "gaitsequence",    0xB8  },
                                                 { "mins[0]",         0x7C  },
                                                 { "mins[1]",         0x80  },
                                                 { "mins[2]",         0x84  },
                                                 { "maxs[0]",         0x88  },
                                                 { "maxs[1]",         0x8C  },
                                                 { "maxs[2]",         0x90  },
                                                 { "aiment",          0x94  }, //0x30
                                                 { "basevelocity[0]", 0xBC  },
                                                 { "basevelocity[1]", 0xC0  },
                                                 { "basevelocity[2]", 0xC4  },
                                                 { "friction",        0x9C  },
                                                 { "gravity",         0xA0  },
                                                 { "spectator",       0xB0  },
                                                 { "velocity[0]",     0x70  },
                                                 { "velocity[1]",     0x74  },
                                                 { "velocity[2]",     0x78  },
                                                 { "team",            0xA4  },
                                                 { "playerclass",     0xA8  },
                                                 { "health",          0xAC  },
                                                 { "usehull",         0xC8  },
                                                 { "oldbuttons",      0xCC  },
                                                 { "onground",        0xD0  },
                                                 { "iStepLeft",       0xD4  }, //0x40
                                                 { "flFallVelocity",  0xD8  },
                                                 { "weaponanim",      0xE0  },
                                                 { "eflags",          0x44  },
                                                 { "iuser1",          0x104 },
                                                 { "iuser2",          0x108 },
                                                 { "iuser3",          0x10C },
                                                 { "iuser4",          0x110 },
                                                 { "fuser1",          0x114 },
                                                 { "fuser2",          0x118 },
                                                 { "fuser3",          0x11C },
                                                 { "fuser4",          0x120 },
                                                 { "vuser1[0]",       0x124 },
                                                 { "vuser1[1]",       0x128 },
                                                 { "vuser1[2]",       0x12C },
                                                 { "vuser2[0]",       0x130 },
                                                 { "vuser2[1]",       0x134 }, //0x50
                                                 { "vuser2[2]",       0x138 },
                                                 { "vuser3[0]",       0x13C },
                                                 { "vuser3[1]",       0x140 },
                                                 { "vuser3[2]",       0x144 },
                                                 { "vuser4[0]",       0x148 },
                                                 { "vuser4[1]",       0x14C },
                                                 { "vuser4[2]",       0x150 } }; //0x57
definitionkeypair_t g_UsercmdDataDefinition[] = { { "lerp_msec",          0x00 },
                                                  { "msec",               0x02 },
                                                  { "lightlevel",         0x1C },
                                                  { "viewangles[0]",      0x04 },
                                                  { "viewangles[1]",      0x08 },
                                                  { "viewangles[2]",      0x0C },
                                                  { "buttons",            0x1E },
                                                  { "forwardmove",        0x10 },
                                                  { "sidemove",           0x14 },
                                                  { "upmove",             0x18 },
                                                  { "impulse",            0x20 },
                                                  { "weaponselect",       0x21 },
                                                  { "impact_index",       0x24 },
                                                  { "impact_position[0]", 0x28 },
                                                  { "impact_position[1]", 0x2C },
                                                  { "impact_position[2]", 0x30 } }; //0x10
definitionkeypair_t g_WeaponDataDefinition[] = { { "m_iId",                   0x00 },
                                                 { "m_iClip",                 0x04 },
                                                 { "m_flNextPrimaryAttack",   0x08 },
                                                 { "m_flNextSecondaryAttack", 0x0C },
                                                 { "m_flTimeWeaponIdle",      0x10 },
                                                 { "m_fInReload",             0x14 },
                                                 { "m_fInSpecialReload",      0x18 },
                                                 { "m_flNextReload",          0x1C },
                                                 { "m_flPumpTime",            0x20 },
                                                 { "m_fReloadTime",           0x24 },
                                                 { "m_fAimedDamage",          0x28 },
                                                 { "m_fNextAimBonus",         0x2C },
                                                 { "m_fInZoom",               0x30 },
                                                 { "m_iWeaponState",          0x34 },
                                                 { "iuser1",                  0x38 },
                                                 { "iuser2",                  0x3C }, //0x10
                                                 { "iuser3",                  0x40 },
                                                 { "iuser4",                  0x44 },
                                                 { "fuser1",                  0x48 },
                                                 { "fuser2",                  0x4C },
                                                 { "fuser3",                  0x50 },
                                                 { "fuser4",                  0x54 } }; //0x16
definitionkeypair_t g_ClientDataDefinition[] = { { "origin[0]",       0x00  },
                                                 { "origin[1]",       0x04  },
                                                 { "origin[2]",       0x08  },
                                                 { "velocity[0]",     0x0C  },
                                                 { "velocity[1]",     0x10  },
                                                 { "velocity[2]",     0x14  },
                                                 { "viewmodel",       0x18  },
                                                 { "punchangle[0]",   0x1C  },
                                                 { "punchangle[1]",   0x20  },
                                                 { "punchangle[2]",   0x24  },
                                                 { "flags",           0x28  },
                                                 { "waterlevel",      0x2C  },
                                                 { "watertype",       0x30  },
                                                 { "view_ofs[0]",     0x34  },
                                                 { "view_ofs[1]",     0x38  },
                                                 { "view_ofs[2]",     0x3C  }, //0x10
                                                 { "health",          0x40  },
                                                 { "bInDuck",         0x44  },
                                                 { "weapons",         0x48  },
                                                 { "flTimeStepSound", 0x4C  },
                                                 { "flDuckTime",      0x50  },
                                                 { "flSwimTime",      0x54  },
                                                 { "waterjumptime",   0x58  },
                                                 { "maxspeed",        0x5C  },
                                                 { "m_iId",           0x68  },
                                                 { "ammo_nails",      0x70  },
                                                 { "ammo_shells",     0x6C  },
                                                 { "ammo_cells",      0x74  },
                                                 { "ammo_rockets",    0x78  },
                                                 { "m_flNextAttack",  0x7C  },
                                                 { "physinfo",        0x8C  },
                                                 { "fov",             0x60  }, //0x20
                                                 { "weaponanim",      0x64  },
                                                 { "tfstate",         0x80  },
                                                 { "pushmsec",        0x84  },
                                                 { "deadflag",        0x88  },
                                                 { "iuser1",          0x18C },
                                                 { "iuser2",          0x190 },
                                                 { "iuser3",          0x194 },
                                                 { "iuser4",          0x198 },
                                                 { "fuser1",          0x19C },
                                                 { "fuser2",          0x1A0 },
                                                 { "fuser3",          0x1A4 },
                                                 { "fuser4",          0x1A8 },
                                                 { "vuser1[0]",       0x1AC },
                                                 { "vuser1[1]",       0x1B0 },
                                                 { "vuser1[2]",       0x1B4 },
                                                 { "vuser2[0]",       0x1B8 }, //0x30
                                                 { "vuser2[1]",       0x1BC },
                                                 { "vuser2[2]",       0x1C0 },
                                                 { "vuser3[0]",       0x1C4 },
                                                 { "vuser3[1]",       0x1C8 },
                                                 { "vuser3[2]",       0x1CC },
                                                 { "vuser4[0]",       0x1D0 },
                                                 { "vuser4[1]",       0x1D4 },
                                                 { "vuser4[2]",       0x1D8 } }; //0x38

void DELTA_Init() {

   Cmd_AddCommand("delta_stats", DELTA_DumpStats_f);
   Cmd_AddCommand("delta_clear", DELTA_ClearStats_f);

   //Replace all these with appropriate sizeof() once you know it all works.
   DELTA_AddDefinition("clientdata_t", g_ClientDataDefinition, 0x38);
   DELTA_AddDefinition("weapon_data_t", g_WeaponDataDefinition, 0x16);
   DELTA_AddDefinition("usercmd_t", g_UsercmdDataDefinition, 0x10);
   DELTA_AddDefinition("entity_state_t", g_EntityDataDefinition, 0x57);
   DELTA_AddDefinition("entity_state_player_t", g_EntityDataDefinition, 0x57);
   DELTA_AddDefinition("custom_entity_state_t", g_EntityDataDefinition, 0x57);
   DELTA_AddDefinition("event_t", g_EventDataDefinition, 0x0E);
}
void DELTA_Shutdown() {

   DELTA_ClearEncoders();
   DELTA_ClearDefinitions();
   DELTA_ClearRegistrations();
}



//SV_ -- A few functions that seem related, but aren't in the same general area

//sv_delta_t * g_sv_delta = NULL;


/* DESCRIPTION: SV_RegisterDelta
// PATH: SV_InitDeltas
//
// Adds various 'deltas' to a linked list.  It appears that the
// builddelta_t struct is the only struct known outside of the
// DELTA functions themselves.
*/
void SV_RegisterDelta(char * searchstring, char * filename) {

   sv_builddelta_t * var_4;
   sv_delta_t * var_8;

   if(DELTA_Load(searchstring, &var_4, filename) == 0) {
      Sys_Error("%s: Error parsing %s.", __FUNCTION__, filename);
   }

   var_8 = (sv_delta_t *)Q_Malloc(sizeof(sv_delta_t));
   CHECK_MEMORY_MALLOC(var_8);

   var_8->filename = Q_Strdup(filename);
   var_8->name = Q_Strdup(searchstring);
   CHECK_MEMORY_MALLOC(var_8->filename);
   CHECK_MEMORY_MALLOC(var_8->name);

   var_8->padding0C = var_4;

   var_8->next = g_sv_delta;
   g_sv_delta = var_8;
}
/* DESCRIPTION: SV_LookupDelta
// PATH: SV_InitDeltas, MSG_ReadUserCmd
//
// Goes through the linked list a looking for a delta.
*/
sv_builddelta_t * SV_LookupDelta(const char * deltaName) {

   sv_delta_t * var_4;

   for(var_4 = g_sv_delta; var_4 != NULL; var_4 = var_4->next) {
      if(Q_strcasecmp(var_4->name, deltaName) == 0) {
         return(var_4->padding0C);
      }
   }

   //error path
   Sys_Error("%s: Couldn't find \"%s\".", __FUNCTION__, deltaName);
}

static delta_object_3_t g_MetaDescription[] = { {   8,      "fieldType"  ,  0, 1, 0, 32,    1.0, 1.0, 0, 0, 0, 0 },
                                                { 128,      "fieldName"  ,  4, 1, 0,  8,    1.0, 1.0, 0, 0, 0, 0 },
                                                {   8,      "fieldOffset", 36, 1, 0, 16,    1.0, 1.0, 0, 0, 0, 0 },
                                                {   8,      "fieldSize"  , 40, 1, 0,  8,    1.0, 1.0, 0, 0, 0, 0 },
                                                {   8, "significant_bits", 44, 1, 0,  8,    1.0, 1.0, 0, 0, 0, 0 },
                                                {   4,      "premultiply", 48, 1, 0, 32, 4000.0, 1.0, 0, 0, 0, 0 },
                                                {   4,     "postmultiply", 52, 1, 0, 32, 4000.0, 1.0, 0, 0, 0, 0 } };




static sv_builddelta_t g_MetaDelta = { 0, 7, "", NULL, g_MetaDescription };


/* DESCRIPTION: SV_WriteDeltaDescriptionsToClient
// PATH: SV_SendServerinfo
//
// This writes deltas.  I'd like to draw attention to the bottom, where
// I'm at a complete loss as to what's going on.  Funny.  Everything was making
// perfect sense until tis connection bridged DELTA_Load with some 44 byte
// structs.
*/
void SV_WriteDeltaDescriptionsToClient(sizebuf_t * message) {

   int i, total;
   delta_object_3_t tempdelta;
   sv_delta_t * var_C;


   Q_memset(&tempdelta, 0, sizeof(delta_object_3_t));

   for(var_C = g_sv_delta; var_C != NULL; var_C = var_C->next) {

      MSG_WriteByte(message, 0x0E);
      MSG_WriteString(message, var_C->name);

      total = var_C->padding0C->num_deltas;

      MSG_StartBitWriting(message);
      MSG_WriteBits(total, 0x10);
      for(i = 0; i < total; i++) {

         //This confuses me, but everything somehow works.
         //Something to more thouroughly examine in a debugger.

         //The void *s will remind me in case I forget by the time I use gpp
         DELTA_WriteDelta((void *)&tempdelta, (void *)&(var_C->padding0C->deltas[i]), 1, &g_MetaDelta, NULL);
      }
      MSG_EndBitWriting(message);
   }
}


/* DESCRIPTION: SV_InitEncoders
// PATH: Host_InitializeGameDLL
//
// This loops through the list of delta things.  For each one it checks
// to see if there's a matching encoder, and if so, assigns its value
// to the structure's encoder var for convenient access.
*/
void SV_InitEncoders() {

   sv_builddelta_t * var_4;
   sv_delta_t * var_8;


   for(var_8 = g_sv_delta; var_8 != NULL; var_8 = var_8->next) {

      var_4 = var_8->padding0C;
      if(var_4->name[0] != '\0') {

         var_4->ConditionalEncoder = DELTA_LookupEncoder(var_4->name);
      }
   }
}
void SV_Shutdown() { //This probably needs to be moved or renamed.

   sv_delta_t * var_4, * var_8;


   SV_RconShutdown();
   //call to SV_ShutdownSteamValidation ignored

   var_4 = g_sv_delta;
   while(1) {

      if(var_4 == NULL) { break; }
      var_8 = var_4->next;

      if(var_4->padding0C != NULL) {
         DELTA_FreeDescription(&(var_4->padding0C));
      }
      Q_Free(var_4->name);
      Q_Free(var_4->filename);
      Q_Free(var_4);

      var_4 = var_8;

   }

   g_sv_delta = NULL;

   SV_ClearSecurityModule(&(global_svs.module80[0]));
   SV_ClearSecurityModule(&(global_svs.module80[1]));
   SV_ClearSecurityModule(&(global_svs.module80[2]));
   SV_ClearSecurityModule(&(global_svs.module80[3]));
}

/* DESCRIPTION: SV_WriteDeltaHeader
// PATH: SV_CreatePacketEntities, SV_InvokeCallback
//
// The name of the function provides all of the clues on this one.
// It's not a struct, broken down and sent over, and it's not apparent
// what any of the values are right now.
*/
void SV_WriteDeltaHeader(uint32 arg_0, uint32 arg_4, uint32 arg_8, uint32 * arg_C, uint32 arg_10, uint32 arg_14, uint32 arg_18, uint32 arg_1C) {

   uint32 var_4;


   var_4 = arg_0 - *arg_C;
   if(arg_18 == 0) {

      MSG_WriteBits((arg_4 != 0), 1);
      if(var_4 > 0 && var_4 < 0x40) {
         MSG_WriteBits(0, 1);
         MSG_WriteBits(var_4, 6);
      }
      else {
         MSG_WriteBits(1, 1);
         MSG_WriteBits(arg_0, 0x0B);
      }
   }
   else {

      if(var_4 == 1) {
         MSG_WriteBits(1, 1);
      }
      else {

         MSG_WriteBits(0, 1);
         if(var_4 > 0 && var_4 < 0x40) {
            MSG_WriteBits(0, 1);
            MSG_WriteBits(var_4, 6);
         }
         else {
            MSG_WriteBits(1, 1);
            MSG_WriteBits(arg_0, 0x0B);
         }
      }
   }

   *arg_C = arg_0;

   if(arg_4 == 0) {

      MSG_WriteBits((arg_8 != 0), 1);
      if(global_sv.padding3bc60_struct->padding00 != 0) {

         if(arg_10 == 0) {
            MSG_WriteBits(0, 1);
         }
         else {

            MSG_WriteBits(1, 1);
            MSG_WriteBits(arg_14, 6);
         }
      }

      if(arg_18 != 0 && arg_10 == 0) {

         if(arg_1C == 0) {
            MSG_WriteBits(0, 1);
         }
         else {

            MSG_WriteBits(1, 1);
            MSG_WriteBits(arg_1C, 6);
         }
      }
   }
}

//Normally the private vars are used instead of the search functions.
//Naturally I used the search functions since I couldn't grab the private vars.
//It's permissible to switch them back.

sv_builddelta_t * g_pplayerdelta = NULL;
sv_builddelta_t * g_pentitydelta = NULL;
sv_builddelta_t * g_pcustomentitydelta = NULL;
sv_builddelta_t * g_pclientdelta = NULL;
sv_builddelta_t * g_pweapondelta = NULL;
sv_builddelta_t * g_peventdelta = NULL;
void SV_InitDeltas() {

   Con_Printf("%s: Initializing deltas.\n", __FUNCTION__);

   SV_RegisterDelta("clientdata_t", "delta.lst");
   SV_RegisterDelta("entity_state_t", "delta.lst");
   SV_RegisterDelta("entity_state_player_t", "delta.lst");
   SV_RegisterDelta("custom_entity_state_t", "delta.lst");
   SV_RegisterDelta("usercmd_t", "delta.lst");
   SV_RegisterDelta("weapon_data_t", "delta.lst");
   SV_RegisterDelta("event_t", "delta.lst");

   g_pplayerdelta = SV_LookupDelta("entity_state_player_t");
   g_pentitydelta = SV_LookupDelta("entity_state_t");
   g_pcustomentitydelta = SV_LookupDelta("custom_entity_state_t");
   g_pclientdelta = SV_LookupDelta("clientdata_t");
   g_pweapondelta = SV_LookupDelta("weapon_data_t");
   g_peventdelta = SV_LookupDelta("event_t");

   if(g_pplayerdelta == NULL) {
      Sys_Error("%s: Encoder for entity_state_player_t was not found.");
   }
   if(g_pentitydelta == NULL) {
      Sys_Error("%s: Encoder for entity_state_t was not found.");
   }
   if(g_pcustomentitydelta == NULL) {
      Sys_Error("%s: Encoder for custom_entity_state_t was not found.");
   }
   if(g_pclientdelta == NULL) {
      Sys_Error("%s: Encoder for clientdata_t was not found.");
   }
   if(g_pweapondelta == NULL) {
      Sys_Error("%s: Encoder for weapon_data_t was not found.");
   }
   if(g_peventdelta == NULL) {
      Sys_Error("%s: Encoder for event_t was not found.");
   }
}


