/*
Copyright (c) 2006 by Dan Kennedy.
Copyright (c) 2006 by Juliusz Chroboczek.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//Found a better one.  There are a lot of these around ya know.
#ifdef __MINGW32__

 #include "Q_memdef.h"
 #include "endianin.h"

 #include <ctype.h>
 #include <winsock2.h>

int inet_aton(const char *cp, struct in_addr *addr) {

   register uint32 val;
   register int32 base, n;
   register char c;
   uint32 parts[4];
   register uint32 *pp = parts;

   c = *cp;
   while(1) {
      /*
       * Collect number up to ``.''.
       * Values are specified as for C:
       * 0x=hex, 0=octal, isdigit=decimal.
       */
      if(!isdigit(c)) {
         return (0);
      }
      val = 0; base = 10;
      if(c == '0') {
         c = *++cp;
         if(c == 'x' || c == 'X') {
            base = 16, c = *++cp;
         }
         else {
            base = 8;
         }
      }
      while(1) {
         if(isascii(c) && isdigit(c)) {
            val = (val * base) + (c - '0');
            c = *++cp;
         }
         else if(base == 16 && isascii(c) && isxdigit(c)) {
            val = (val << 4) | (c + 10 - (islower(c) ? 'a' : 'A'));
            c = *++cp;
         }
         else {
            break;
         }
      }
      if(c == '.') {
         /*
          * Internet format:
          *    a.b.c.d
          *    a.b.c    (with c treated as 16 bits)
          *    a.b    (with b treated as 24 bits)
          */
         if(pp >= parts + 3) {
            return (0);
         }
         *pp++ = val;
         c = *++cp;
      }
      else {
         break;
      }
   }
   /*
    * Check for trailing characters.
    */
   if(c != '\0' && (!isascii(c) || !isspace(c))) {
      return (0);
   }
   /*
    * Concoct the address according to
    * the number of parts specified.
    */
   n = pp - parts + 1;
   switch(n) {

   case 0:
      return (0);        /* initial nondigit */

   case 1:                /* a -- 32 bits */
      break;

   case 2:                /* a.b -- 8.24 bits */
      if((val > 0xffffff) || (parts[0] > 0xff)) {
         return (0);
      }
      val |= parts[0] << 24;
      break;

   case 3:                /* a.b.c -- 8.8.16 bits */
      if((val > 0xffff) || (parts[0] > 0xff) || (parts[1] > 0xff)) {
         return (0);
      }
      val |= (parts[0] << 24) | (parts[1] << 16);
      break;

   case 4:                /* a.b.c.d -- 8.8.8.8 bits */
      if((val > 0xff) || (parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
         return (0);
      }
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
      break;
   }
   if(addr) {
      addr->s_addr = htonl(val);
   }
   return (1);
}

#endif
