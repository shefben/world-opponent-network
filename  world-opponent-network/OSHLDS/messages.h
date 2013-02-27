/* These MSG functions are mostly a step above the sizebuf ones.
// They write bits and bytes as needed to sizebufs, always writing
// things to the bit stream in little Endian.
//
// The most complicated functions do bit reading/writing.
*/

#ifndef OSHLDS_MESSAGES_HEADER
#define OSHLDS_MESSAGES_HEADER

#include "sizebuf.h"
#include "delta.h"
#include "endianin.h"

//globals
extern unsigned int global_msg_readcount;
extern qboolean global_msg_badread;

// functions
void MSG_BitOpsInit(); //renamed

void MSG_BeginReading();

int MSG_ReadChar();
int MSG_ReadByte();
int MSG_ReadShort();
int MSG_ReadLong();
float MSG_ReadFloat();
char * MSG_ReadString();
char * MSG_ReadStringLine();

int MSG_ReadStringThreadSafe(char *, unsigned int);
int MSG_ReadStringLineThreadSafe(char *, unsigned int);

int MSG_ReadBuf(int, void *);

int MSG_CurrentBit();
int MSG_ReadBits(unsigned int);
int MSG_ReadSBits(unsigned int);
float MSG_ReadHiresAngle();
float MSG_ReadAngle();
float MSG_ReadBitAngle(unsigned int);

void MSG_ReadUsercmd(usercmd_t *, usercmd_t *);

void MSG_WriteChar(sizebuf_t *, unsigned int);
void MSG_WriteChar_C(sizebuf_t *, unsigned char);
void MSG_WriteByte(sizebuf_t *, unsigned int);
void MSG_WriteByte_C(sizebuf_t *, byte);
void MSG_WriteShort(sizebuf_t *, int);
void MSG_WriteShort_S(sizebuf_t *, unsigned short int);
void MSG_WriteWord(sizebuf_t *, unsigned int);
void MSG_WriteWord_S(sizebuf_t *, word);
void MSG_WriteLong (sizebuf_t *, uint32);
void MSG_WriteFloat(sizebuf_t *, float);
void MSG_WriteString(sizebuf_t *, const char *);
void MSG_WriteBuf(sizebuf_t *, int, const void *);
void MSG_WriteAngle(sizebuf_t *, float);
void MSG_WriteHiresAngle(sizebuf_t *, float);
void MSG_WriteCoord(sizebuf_t *, float);

void MSG_StartBitReading(sizebuf_t *);
int MSG_ReadBits(unsigned int);
int MSG_ReadBitData(byte *, unsigned int);
void MSG_EndBitReading(sizebuf_t *);

void MSG_StartBitWriting(sizebuf_t *);
void MSG_WriteBits(uint32, int);
void MSG_WriteSBits(int32, int);
void MSG_WriteBitAngle(float, int);
void MSG_WriteBitVec3Coord(const vec3_t);
void MSG_WriteBitString(const char *);
void MSG_WriteBitData(const char *, int);
void MSG_EndBitWriting();
#endif
