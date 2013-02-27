#ifndef __WE_HAVE_ALREADY_INCLUDED_PMANIP_H
#define __WE_HAVE_ALREADY_INCLUDED_PMANIP_H

#include "endianin.h"

//little endian convert

inline BYTE   ExtractAndReturnByte(const unsigned char * packet) {

   return(*(BYTE *)(packet));
}
inline UINT16 ExtractAndReturnWord(const unsigned char * packet) {

   return(letohost16(*(UINT16 *)(packet)));
}
inline UINT32 ExtractAndReturnDWord(const unsigned char * packet) {

   return(letohost32(*(UINT32 *)(packet)));
}

inline void ExtractByte(const unsigned char ** packet, unsigned int * variable) {

   *variable = (unsigned int)ExtractAndReturnByte(*packet);
	*packet += 1;
}
inline void ExtractWord(const unsigned char ** packet, unsigned int * variable) {

   *variable = (unsigned int)ExtractAndReturnWord(*packet);
	*packet += 2;
}
inline void ExtractDWord(const unsigned char ** packet, unsigned int * variable) {

   *variable = (unsigned int)ExtractAndReturnDWord(*packet);
	*packet += 4;
}


inline void WriteByte(unsigned char * packet, BYTE number) {

   *(BYTE *)packet = number;
}
inline void WriteWord(unsigned char * packet, UINT16 number) {

   *(UINT16 *)packet = hosttole16(number);
}
inline void WriteDWord(unsigned char * packet, UINT32 number) {

   *(UINT32 *)packet = hosttole32(number);
}

inline void WriteAndPassByte(unsigned char ** packet, unsigned int * variable) {

   WriteByte(*packet, (BYTE)*variable);
	*packet += 1;
}
inline void WriteAndPassWord(unsigned char ** packet, unsigned int * variable) {

   WriteWord(*packet, (UINT16)*variable);
	*packet += 2;
}
inline void WriteAndPassDWord(unsigned char ** packet, unsigned int * variable) {

   WriteDWord(*packet, (UINT32)*variable);
	*packet += 4;
}


//network order compliant

inline BYTE   ExtractAndReturnBytentohs(const unsigned char * packet) {

   return(*(BYTE *)(packet));
}
inline UINT16 ExtractAndReturnWordntohs(const unsigned char * packet) {

   return(betohost16(*(UINT16 *)(packet)));
}
inline UINT32 ExtractAndReturnDWordntohs(const unsigned char * packet) {

   return(betohost32(*(UINT32 *)(packet)));
}

inline void ExtractBytentohs(const unsigned char ** packet, unsigned int * variable) {

   *variable = (unsigned int)ExtractAndReturnBytentohs(*packet);
	*packet += 1;
}
inline void ExtractWordntohs(const unsigned char ** packet, unsigned int * variable) {

   *variable = (unsigned int)ExtractAndReturnWordntohs(*packet);
	*packet += 2;
}
inline void ExtractDWordntohs(const unsigned char ** packet, unsigned int * variable) {

   *variable = (unsigned int)ExtractAndReturnDWordntohs(*packet);
	*packet += 4;
}


inline void WriteBytentohs(unsigned char * packet, BYTE number) {

   *(BYTE *)packet = number;
}
inline void WriteWordntohs(unsigned char * packet, UINT16 number) {

   *(UINT16 *)packet = hosttobe16(number);
}
inline void WriteDWordntohs(unsigned char * packet, UINT32 number) {

   *(UINT32 *)packet = hosttobe32(number);
}

inline void WriteAndPassBytentohs(unsigned char ** packet, unsigned int * variable) {

   WriteBytentohs(*packet, (BYTE)*variable);
	*packet += 1;
}
inline void WriteAndPassWordntohs(unsigned char ** packet, unsigned int * variable) {

   WriteWordntohs(*packet, (UINT16)*variable);
	*packet += 2;
}
inline void WriteAndPassDWordntohs(unsigned char ** packet, unsigned int * variable) {

   WriteDWordntohs(*packet, (UINT32)*variable);
	*packet += 4;
}




inline void WriteStringA(unsigned char * packet, const char * buffer, unsigned int len) {
   memcpy(packet, buffer, len);
}
inline void WriteAndPassStringA(unsigned char ** packet, const char * buffer, unsigned int len) {
   WriteStringA(*packet, buffer, len);
   *packet += len;
}
inline void WriteStringW(unsigned char * packet, const char * buffer, unsigned int len) {
   unsigned int i = 0;
   while(i < len) {

      packet[0] = buffer[0];
      packet[1] = 0xFB;
      packet += 2;
      buffer++;
      i++;
   }
}
inline void WriteAndPassStringW(unsigned char ** packet, const char * buffer, unsigned int len) {
   WriteStringW(*packet, buffer, len);
   *packet += (len << 1);
}

int ExtractUnicodeString(const unsigned char **, unsigned int, unsigned char *, unsigned int);
int ExtractString(const unsigned char **, unsigned int, unsigned char *, unsigned int);

void PrintUnknownPacket(const unsigned char *, unsigned int);

#endif
