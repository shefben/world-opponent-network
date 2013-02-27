#include "crc.h"
#include "report.h"
#include "FS.hpp"
#include "Mod.h"

#include <stdio.h>
#include <string.h>

//None of these PITA functions are in QW, though I've played with MD5 before...
//If I know my programmers, they'll probably be simiar to the RFC.

// CRC (must match client code, otherwise maps are considered different)

static const CRC32_t CRC32_m_tab_0[0x100] = { //Heh, silly endianness...

   0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
   0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
   0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
   0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
   0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
   0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
   0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
   0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
   0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
   0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
   0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
   0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
   0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
   0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
   0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
   0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
   0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
   0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
   0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
   0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
   0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
   0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
   0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
   0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
   0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
   0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
   0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
   0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
   0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
   0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
   0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
   0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
   0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
   0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
   0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
   0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
   0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
   0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
   0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
   0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
   0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
   0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
   0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
   0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
   0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
   0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
   0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
   0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
   0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
   0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
   0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
   0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
   0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
   0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
   0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
   0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
   0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
   0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
   0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
   0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
   0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
   0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
   0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
   0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };

void CRC32_Init__FPUl(CRC32_t *pulCRC) {
   CRC32_Init(pulCRC);
}
HLDS_DLLEXPORT void CRC32_Init(CRC32_t *pulCRC) {

   *pulCRC = 0xFFFFFFFF;
} //Whew, that was hard.  Let's go grab some beers!

void CRC32_ProcessBuffer__FPUlPvi(CRC32_t *pulCRC, void *pBuffer, int nBuffer) {
   CRC32_ProcessBuffer(pulCRC, pBuffer, nBuffer);
}
HLDS_DLLEXPORT void CRC32_ProcessBuffer(CRC32_t *pulCRC, void *pBuffer, int nBuffer) {

   //It's pretty clear this uses a switch and I think a goto, neither of which
   //IDA appreciates.  I've restructured it as a result.

   CRC32_t CRC;
   unsigned char *ptr;
   unsigned int OddBits;
   int i;

   CRC = *pulCRC;
   ptr = (unsigned char *)pBuffer;

   if(nBuffer > 7) {

      OddBits = ((unsigned int)ptr) & 3;
      nBuffer -= OddBits;
      switch(OddBits) {

      case 3:
         CRC = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);
      case 2:
         CRC = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);
      case 1:
         CRC = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);
      }

      i = nBuffer >> 3;
      while(i--) {
         CRC ^= hosttole32(*(CRC32_t *)ptr);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC ^= hosttole32(*(CRC32_t *)(ptr + 4));
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
         ptr += 8;
      }

      nBuffer &= 7;
   }


   switch (nBuffer) {

   case 7:
      CRC = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);

   case 6:
      CRC = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);

   case 5:
      CRC = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);

   case 4:
      CRC ^= hosttole32(*(CRC32_t *)ptr);
      CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
      CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
      CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
      CRC = CRC32_m_tab_0[(unsigned char)CRC] ^ (CRC >> 8);
      *pulCRC = CRC;
      return;

   case 3:
      CRC  = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);

   case 2:
      CRC  = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);

   case 1:
      CRC  = CRC32_m_tab_0[*ptr++ ^ (unsigned char)CRC] ^ (CRC >> 8);

   case 0:
      *pulCRC = CRC;
      return;
   }
}

void CRC32_ProcessByte__FPUlUc(CRC32_t *pulCRC, unsigned char ch) {
   CRC32_ProcessByte(pulCRC, ch);
}
HLDS_DLLEXPORT void CRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch) {

   CRC32_t TempCRC;

   TempCRC = *pulCRC;
   TempCRC ^= ch;
   TempCRC = CRC32_m_tab_0[TempCRC & 0xFF] ^ (TempCRC >> 8); //&FF = mod tablesize
   *pulCRC = TempCRC;
}

CRC32_t CRC32_Final__FUl(CRC32_t pulCRC) {
   return(CRC32_Final(pulCRC));
}
HLDS_DLLEXPORT CRC32_t CRC32_Final(CRC32_t pulCRC) {
   return(~pulCRC);
}


//For some reason, neither CRC_Init or CRC_Final ar called.  Must be handled by caller.
int CRC_File(CRC32_t *crcvalue, char *pszFileName) {

   hl_file_t *file;
   char buffer[1024];
   int NumRead;

   file = FS_Open(pszFileName, "rb");
   if(file == NULL) {
      return(0);
   }

   while(1) {

      NumRead = FS_Read(buffer, 1, sizeof(buffer), file);

      if(NumRead > 0) {
         CRC32_ProcessBuffer(crcvalue, buffer, NumRead);
      }

      if(FS_EndOfFile(file) != 0) { break; }
      if(FS_IsOk(file) == 0) {
         FS_Close(file);
         return(0);
      }
   }

   FS_Close(file);
   return(1);
}
//broken?
int CRC_MapFile(CRC32_t *crcvalue, const char *pszFileName) {

   hl_file_t *file;
   char buffer[1024];
   int NumRead;
   unsigned int FileSize;
   unsigned int i;
   unsigned int SeekPointer;

   dheader_t header;

   file = FS_Open(pszFileName, "rb");
   if(file == NULL) {
      return(0);
   }

   SeekPointer = FS_Tell(file);
   NumRead = FS_Read((void *)&header, 1, sizeof(header), file);

   //Apparently, BSP headers are THAT big.
   //This next 'check' is a bit hacky, but it gets the job mostly done.
   if(NumRead != sizeof(header)) {
      Con_Printf("%s: Didn't read all of map %s's file header.\n", __FUNCTION__, pszFileName);
      FS_Close(file);
      return(0);
   }

   i = hosttole32(header.version);
   if(i != 0x1D && i != 0x1E) {
      Con_Printf("%s: Didn't load map %s.  Header version was 0x%X, should've been 0x1D or 0x1E.\n", __FUNCTION__, pszFileName, i);
      FS_Close(file);
      return(0);
   }

   for(i = 1; i < HEADER_LUMPS; i++) {

      FileSize = hosttole32(header.lumps[i].filelen);
      FS_Seek(file, SeekPointer + hosttole32(header.lumps[i].fileofs), SEEK_SET);

      //Unlike before, we only want to snag
      //data up to a certain point.  Knowing the total is therefore useful
      while(FileSize > 0) {

         if(FileSize >= sizeof(buffer)) {
            NumRead = FS_Read(buffer, 1, sizeof(buffer), file);
         }
         else {
            NumRead = FS_Read(buffer, 1, FileSize, file);
         }

         if(NumRead > 0) {
            FileSize -= NumRead;
            CRC32_ProcessBuffer(crcvalue, buffer, NumRead);
         }

         if(FS_EndOfFile(file) != 0) { break; }
         if(FS_IsOk(file) == 0) {
            FS_Close(file);
            return(0);
         }
      }
   }

   FS_Close(file);
   return(1);
}

unsigned char COM_BlockSequenceCRCByte(unsigned char *base, int length, int sequence) {

   CRC32_t CRC;
   char * ptr;
   char buffer[64];

   if (sequence < 0) { Sys_Error("sequence < 0 in COM_BlockSequenceCRCByte\n"); }

   ptr = (char *)CRC32_m_tab_0 + (sequence % 0x3FC);

   if(length > 60) { length = 60; }

   memcpy(buffer, base, length);

   //I don't know why we're tacking four bytes onto the end of our base...
   buffer[length+0] = ptr[0];
   buffer[length+1] = ptr[1];
   buffer[length+2] = ptr[2];
   buffer[length+3] = ptr[3];

   length += 4;

   CRC32_Init(&CRC);
   CRC32_ProcessBuffer(&CRC, buffer, length);
   CRC = CRC32_Final(CRC);

   return((unsigned char)CRC);
}


//MD5--You want to know how it works?  Read the RFCs.
//Decodes and Encodes don't appear to be used by HL.  Older standard?
//Well whatever it is, as long as the whole MD5 thing is my code,
//we should be good.
typedef unsigned char *POINTER;
typedef uint16 UINT2;
typedef uint32 UINT4;

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
//typedef struct {
//  UINT4 state[4];                                   /* state (ABCD) */
 // UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
//  unsigned char buffer[64];                         /* input buffer */
//} MD5_CTX;

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
//#define F(x, y, z) (z ^ (x & (y ^ z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

inline void Encode(unsigned char *, UINT4 *, unsigned int);
inline void Decode(UINT4 *, unsigned char const*, unsigned int);
inline void MD5_memcpy(POINTER, POINTER, unsigned int);
inline void MD5_memset(POINTER, int, unsigned int);

void MD5Init__FP12MD5Context_t(MD5Context_t * context) {
   MD5Init(context);
}
void MD5Init(MD5Context_t * context) {

   context->bits[0] = context->bits[1] = 0;
   context->buf[0] = 0x67452301;
   context->buf[1] = 0xefcdab89;
   context->buf[2] = 0x98badcfe;
   context->buf[3] = 0x10325476;
}

void MD5Update__FP12MD5Context_tPCUcUi(MD5Context_t * context, unsigned char const * input, unsigned int inputLen) {
   MD5Update(context, input, inputLen);
}
void MD5Update(MD5Context_t * context, unsigned char const * input, unsigned int inputLen) {

   unsigned int i, index, partLen;

   index = (unsigned int)((context->bits[0] >> 3) & 0x3F);

   if((context->bits[0] += ((UINT4)inputLen << 3)) <
     ((UINT4)inputLen << 3)) {
      context->bits[1]++;
   }
   context->bits[1] += ((UINT4)inputLen >> 29);
   partLen = 64 - index;

   if(inputLen >= partLen) {
      MD5_memcpy((POINTER)&context->in[index], (POINTER)input, partLen);
      MD5Transform(context->buf, context->in);

      for(i = partLen; i + 63 < inputLen; i += 64) {
         MD5Transform(context->buf, &input[i]);
      }

      index = 0;
   }
   else {
      i = 0;
   }

   MD5_memcpy((POINTER)&context->in[index], (POINTER)&input[i], inputLen-i);
}

void MD5Transform__FPUiPCUi(uint32 state[4], unsigned char const block[64]) {
   MD5Transform(state, block);
}
void MD5Transform(uint32 state[4], unsigned char const block[64]) {

   UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
   Decode (x, block, 64);

   /* Round 1 */
   FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
   FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
   FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
   FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
   FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
   FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
   FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
   FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
   FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
   FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
   FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
   FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
   FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
   FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
   FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
   FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

   /* Round 2 */
   GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
   GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
   GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
   GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
   GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
   GG (d, a, b, c, x[10], S22, 0x02441453); /* 22 */
   GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
   GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
   GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
   GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
   GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
   GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
   GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
   GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
   GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
   GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

   /* Round 3 */
   HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
   HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
   HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
   HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
   HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
   HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
   HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
   HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
   HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
   HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
   HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
   HH (b, c, d, a, x[ 6], S34, 0x04881d05); /* 44 */
   HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
   HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
   HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
   HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

   /* Round 4 */
   II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
   II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
   II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
   II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
   II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
   II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
   II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
   II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
   II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
   II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
   II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
   II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
   II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
   II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
   II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
   II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

   state[0] += a;
   state[1] += b;
   state[2] += c;
   state[3] += d;

   MD5_memset((POINTER)x, 0, sizeof (x));
}

void MD5Final__FPUcP12MD5Context_t(unsigned char digest[16], MD5Context_t *context) {
   MD5Final(digest, context);
}
void MD5Final(unsigned char digest[16], MD5Context_t *context) {

   unsigned char bits[8];
   unsigned int index, padLen;

   Encode(bits, context->bits, 8);

   index = (unsigned int)((context->bits[0] >> 3) & 0x3f);
   padLen = (index < 56) ? (56 - index) : (120 - index);
   MD5Update(context, PADDING, padLen);
   MD5Update(context, bits, 8);

   Encode(digest, context->buf, 16);
   MD5_memset((POINTER)context, 0, sizeof(*context));
}

//other
char * MD5_Print__FPUc(unsigned char const hash[16]) {
   return(MD5_Print(hash));
}
char * MD5_Print(unsigned char const hash[16]) {

   static char szReturn_57[64]; //33 should be enough actually.

   //memset(szReturn, 0, sizeof(szReturn_57));

   sprintf(szReturn_57, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
           hash[0], hash[1],  hash[2],  hash[3],  hash[4],  hash[5],  hash[6],  hash[7],
           hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]);

   return(szReturn_57);
}
char * MD5_Print_safe(unsigned char const hash[16], unsigned char out[33]) {

   sprintf(out, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
           hash[0], hash[1],  hash[2],  hash[3],  hash[4],  hash[5],  hash[6],  hash[7],
           hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15]);

   return(out);
}

int MD5_Hash_File__FPUcPciPUi(unsigned char digest[16], char *pszFileName, int bSeed, unsigned int seed[4]) {
   return(MD5_Hash_File(digest, pszFileName, bSeed, seed));
}
int MD5_Hash_File(unsigned char digest[16], char *pszFileName, int bSeed, unsigned int seed[4]) {

   hl_file_t *file;
   char buffer[1024];
   int NumRead;
   MD5Context_t MD5_Hash;

   //How about I NOT use fopen and say I did.
   //COM_OpenFile could be used,
   //though for large files we risk chewing up lots of memory.

   file = FS_Open(pszFileName, "rb");
   if(file == NULL) {
      return(0);
   }

   memset(&MD5_Hash, 0, sizeof(MD5Context_t));
   MD5Init(&MD5_Hash);

   if(bSeed != 0) {
      MD5Update(&MD5_Hash, (const unsigned char *)seed, 16);
   }

   while(1) {

      NumRead = FS_Read(buffer, 1, sizeof(buffer), file);

      if(NumRead > 0) {
         MD5Update(&MD5_Hash, buffer, NumRead);
      }

      if(FS_EndOfFile(file) != 0) { break; }
      if(FS_IsOk(file) == 0) {
         FS_Close(file);
         return(0);
      }
   }

   FS_Close(file);

   MD5Final(digest, &MD5_Hash);
   return(1);
}

int MD5_Hash_CachedFile__FPUcT0iiPUi(unsigned char digest[16], unsigned char *pCache, int nFileSize, int bSeed, unsigned int seed[4]) {
   return(MD5_Hash_CachedFile(digest, pCache, nFileSize, bSeed, seed));
}
int MD5_Hash_CachedFile(unsigned char digest[16], unsigned char *pCache, int nFileSize, int bSeed, unsigned int seed[4]) {

   MD5Context_t MD5_Hash;

   if(pCache == NULL) { return(0); }

   memset(&MD5_Hash, 0, sizeof(MD5Context_t));

   MD5Init(&MD5_Hash);
   if(bSeed != 0) {
      MD5Update(&MD5_Hash, (const unsigned char *)seed, 16);
   }
   MD5Update( &MD5_Hash, pCache, nFileSize);
   MD5Final(digest, &MD5_Hash);

   return(1);
}


inline void Encode(unsigned char * output, UINT4 * input, unsigned int len) {

   unsigned int i, j;

   for (i = 0, j = 0; j < len; i++, j += 4) {
      output[j]   = (unsigned char)((input[i])       & 0xff);
      output[j+1] = (unsigned char)((input[i] >> 8 ) & 0xff);
      output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
      output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
   }
}
inline void Decode(UINT4 * output, unsigned char const * input, unsigned int len) {

   unsigned int i, j;

   for (i = 0, j = 0; j < len; i++, j += 4) {
      output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
        (((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
   }
}
inline void MD5_memcpy(POINTER output, POINTER input, unsigned int len) {
   memcpy(output, input, len);
}
inline void MD5_memset(POINTER output, int value, unsigned int len) {
   memset(output, value, len);
}
