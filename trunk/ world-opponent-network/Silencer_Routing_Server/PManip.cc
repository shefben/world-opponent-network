#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "PManip.h"

// Extract functions are a bit odd in Silencer.  They return nothing, instead
// they alter both variables passed to them--one stores the extracted value,
// and the other is our packet pointer, which is incremented past the var.

int ExtractUnicodeString(const unsigned char ** packet, unsigned int len, unsigned char * buffer, unsigned int buflength) {

   unsigned int LengthOfString;
   unsigned int counter;

   if(len < 2) return(-1);
   ExtractWord(packet, &LengthOfString);

   if(LengthOfString > buflength || (LengthOfString << 1) > len) { return(-1); }

   counter = 0;

   while(counter < LengthOfString) {

      *buffer = **packet;

      buffer++;
      counter++;
      *packet += 2;
   }
   return(LengthOfString);
}
int ExtractString(const unsigned char ** packet, unsigned int len, unsigned char * buffer, unsigned int buflength) {

   unsigned int LengthOfString;

   if(len < 2) return(-1);
   ExtractWord(packet, &LengthOfString);

   if(LengthOfString > buflength || LengthOfString > len) { return(-1); }

   //We know it'll fit.  Copy it in...
   memcpy(buffer, *packet, LengthOfString);
   *packet += LengthOfString;

   return(LengthOfString);
}


void PrintUnknownPacket(const unsigned char * buffer,  unsigned int bytesreceived) {

   unsigned int i = 0;
   char lineinhex[80] = "";
   char lineintext[80] = "";
   char temp[5] = "";

   while(i < bytesreceived) {
      sprintf(temp, "%2x ", buffer[i]);
      strcat(lineinhex, temp);
      if(isprint(buffer[i])) {
         strncat(lineintext, (const char *)buffer+i, 1);
      }
      else {
         strcat(lineintext, ".");
      }
      i++;
      if(i == bytesreceived) {
         while(i%10 != 0) {
            strcat(lineinhex, "   ");
            strcat(lineintext, " ");
            i++;
         }
      }
      if(i%10 == 0) {
         printf("0x %s %s | %i\n", lineinhex, lineintext, i);
         lineinhex[0] = '\0';
         lineintext[0] = '\0';
      }
   }
}
