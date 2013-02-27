/* Header file lovingly exported from the SDK.
// CRCs are annoying, and QW doesn't have them.
*/
#ifndef HLDS_LINUX_CRC_HIJACK
#define HLDS_LINUX_CRC_HIJACK

#include "endianin.h"
#include "ModCalls.h"

//CRC hash
typedef uint32 CRC32_t;
typedef struct crc_info_s {

   qboolean padding00; //Seems to be a boolean that determines whether this resource has a CRC.
   qboolean padding04; //Seems to indicate that a CRC is loaded
   CRC32_t padding08; //Seems to be the actual CRC
} crc_info_t;

// MD5 Hash
typedef struct MD5Context_s { //identical to the MD5_CTX I've used before
   uint32 buf[4];
   uint32 bits[2];
   byte   in[64];

} MD5Context_t;


HLDS_DLLEXPORT void CRC32_Init(CRC32_t *);
HLDS_DLLEXPORT void CRC32_ProcessByte(CRC32_t *, unsigned char);
HLDS_DLLEXPORT void CRC32_ProcessBuffer(CRC32_t *, void *, int);
HLDS_DLLEXPORT CRC32_t CRC32_Final(CRC32_t);




int CRC_File(CRC32_t *, char *);
int CRC_MapFile(CRC32_t *, const char *);
unsigned char COM_BlockSequenceCRCByte(unsigned char *, int, int);


void MD5Init(MD5Context_t *);
void MD5Update(MD5Context_t *, unsigned char const *, unsigned int);
void MD5Transform(uint32 state[4], unsigned char const block[64]);
void MD5Final(unsigned char digest[16], MD5Context_t *);


int MD5_Hash_File(unsigned char digest[16], char *, int, unsigned int seed[4]);
int MD5_Hash_CachedFile(unsigned char digest[16], unsigned char *, int, int, unsigned int seed[4]);
char *MD5_Print(unsigned char const hash[16]);
char *MD5_Print_safe(unsigned char const hash[16], unsigned char out[33]);

#endif
