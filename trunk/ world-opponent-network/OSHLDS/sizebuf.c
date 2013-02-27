#include "sizebuf.h"
#include "HUNK.h"
#include "Q_memdef.h"
#include "report.h"


/*** FUNCTIONS ***/

/* DESCRIPTION: SZ_Clear
// LOCATION: common.c
// PATH: all over
//
// "Wipes" the allocated memory.  Which basically translates into we have X
// bytes here and 0 are used.
*/
void SZ_Clear(sizebuf_t *buf) {

   buf->cursize = 0;
   buf->overflow = 0;
}

/* DESCRIPTION: SZ_GetSpace
// LOCATION: common.c
// PATH: SZ and MSG functons
//
// I believe this returns a pointer to a chunk of memory within the hunk that's
// being reallocated to another purpose.  Kind've gerrymandering the sizebuf_t
// structure.  it's NOT returning the number of free bytes.
//
*/
void *SZ_GetSpace(sizebuf_t *buf, unsigned int length) {

   void *data;

   if(buf->cursize + length > buf->maxsize) { //This is usually bad...
      if((buf->overflow & 1) == 0 && length > buf->maxsize) {
         Sys_Error("SZ_GetSpace overflowed.  Total %u free %u (wanted %u)", buf->maxsize, buf->maxsize - buf->cursize, length);
      }
      else {
         Con_Printf("SZ_GetSpace overflowed.  Total %u free %u (wanted %u).  Wiping.\n", buf->maxsize, buf->maxsize - buf->cursize, length);
         SZ_Clear(buf);
         buf->overflow |= 2;
      }
   }

   data = buf->data + buf->cursize;
   buf->cursize += length;

   return(data);
}

/* DESCRIPTION: SZ_Write
// LOCATION: common.c
// PATH: many places
//
// Memcpy.  A wrapper for a wrapper.  Maybe it should be inline...
*/
void SZ_Write(sizebuf_t *buf, const void *data, int length) {

   Q_memcpy(SZ_GetSpace(buf,length),data,length);
}

/* DESCRIPTION: SZ_Print
// LOCATION: common.c
// PATH: cmd and cvar functions
//
// Allocates and writes STRING data to a sizebuf_t object.
// For some reason it's written like a strcat and will remove a null
// that precedes the area it wants to overwrite...
//
// I really need to rewrite this once I have a better understanding of what it does.
*/
void SZ_Print(sizebuf_t *buf, const char *data) {

   int len = Q_strlen(data)+1;
   byte *msg;

   if (buf->cursize == 0 || buf->data[buf->cursize-1] != '\0') {
      Q_memcpy((byte *)(SZ_GetSpace(buf, len)), data, len); // no null to strcat
   } else {
      msg = (byte *)SZ_GetSpace(buf, len-1);
      if (msg == buf->data)   {
         msg++;
      } //In case the thing overflows.
      Q_memcpy(msg-1,data,len); // write over trailing 0
   }
}

/* DESCRIPTION: SZ_Alloc
// LOCATION: not found
// PATH: Host_Init->Cbuf_Init->SZ_Alloc
//
// Appears to be a wrapper for Hunk_AllocName with a minimum size of 256 bytes.
*/
void SZ_Alloc(char* name, sizebuf_t *buf, int startsize) {


  if (startsize < 256) {
      startsize = 256;
   } //No, I don't know why yet

   buf->data = (byte *)Hunk_AllocName(startsize, name);

   buf->maxsize = startsize;
   buf->cursize = 0;
   buf->overflow = 0;
}
