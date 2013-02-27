#include "messages.h"
#include "delta.h"
#include "NET.h"
#include "VecMath.h"
#include "report.h"
#include "Q_memdef.h"


/*** STRUCTS ***/
typedef struct bf_read_s { //0x18

   unsigned int Readcount;
   sizebuf_t * sz;
   uint32 p2; //unused?
   unsigned int BytesRead;
   unsigned int BitsRead;
   byte * ByteStream;
} bf_read_t;
typedef struct bf_write_s { //0x0C

   unsigned int BitsWritten;
   byte * ByteStream;
   sizebuf_t * sz;
} bf_write_t;


/*** GLOBALS ***/
unsigned int global_msg_readcount;
qboolean global_msg_badread;


/*** LOCAL GLOBALS ***/
static bf_read_t bfread; //should be all zeros, will need re-initing though.
static bf_write_t bfwrite;

static const uint32 bittable[] = {
   0x00000001, 0x00000002, 0x00000004, 0x00000008,
   0x00000010, 0x00000020, 0x00000040, 0x00000080,
   0x00000100, 0x00000200, 0x00000400, 0x00000800,
   0x00001000, 0x00002000, 0x00004000, 0x00008000,
   0x00010000, 0x00020000, 0x00040000, 0x00080000,
   0x00100000, 0x00200000, 0x00400000, 0x00800000,
   0x01000000, 0x02000000, 0x04000000, 0x08000000,
   0x10000000, 0x20000000, 0x40000000, 0x80000000,
   0x00000000 }; //You need a table for that, eh?
static const uint32 rowbittable[] = {
   0x00000000, 0x00000001, 0x00000003, 0x00000007,
   0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
   0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
   0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
   0x0000FFFF, 0x0001FFFF, 0x0003FFFF, 0x0007FFFF,
   0x000FFFFF, 0x001FFFFF, 0x003FFFFF, 0x007FFFFF,
   0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
   0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF,
   0xFFFFFFFF };
static const uint32 invbittable[] = {
   0xFFFFFFFE, 0xFFFFFFFD, 0xFFFFFFFB, 0xFFFFFFF7,
   0xFFFFFFEF, 0xFFFFFFDF, 0xFFFFFFBF, 0xFFFFFF7F,
   0xFFFFFEFF, 0xFFFFFDFF, 0xFFFFFBFF, 0xFFFFF7FF,
   0xFFFFEFFF, 0xFFFFDFFF, 0xFFFFBFFF, 0xFFFF7FFF,
   0xFFFEFFFF, 0xFFFDFFFF, 0xFFFBFFFF, 0xFFF7FFFF,
   0xFFEFFFFF, 0xFFDFFFFF, 0xFFBFFFFF, 0xFF7FFFFF,
   0xFEFFFFFF, 0xFDFFFFFF, 0xFBFFFFFF, 0xF7FFFFFF,
   0xEFFFFFFF, 0xDFFFFFFF, 0xBFFFFFFF, 0x7FFFFFFF,
   0xFFFFFFFF };


/*** FUNCTIONS ***/

/* Reading and writing are both applied to sizebuf structures.  Writing
// is pretty simple since the sizebuf struct records its own size;
// reading on the other hand requires a global in order to read things in
// succession.
//
// I don't like these, make no mistake of it.  I'm used to *(int *) casts
// and I can accept unions, so I usually won't use writes.  I don't like reads
// because of their design.
*/
void MSG_BeginReading() {

   global_msg_readcount = 0;
   global_msg_badread = 0;
}

int MSG_ReadChar() {

   if(global_msg_readcount >= global_net_message.cursize) {
      global_msg_badread = 1;
      return(-1);
   }

   global_msg_readcount++;
   return((int)(global_net_message.data[global_msg_readcount-1]));
}
int MSG_ReadByte() {

   if(global_msg_readcount >= global_net_message.cursize) {
      global_msg_badread = 1;
      return(-1);
   }

   global_msg_readcount++;
   return((unsigned int)(global_net_message.data[global_msg_readcount-1]));
}
int MSG_ReadShort() {

   if(global_msg_readcount+1 >= global_net_message.cursize) {
      global_msg_badread = 1;
      return(-1);
   }

   global_msg_readcount += 2;

   return((int)(letohost32(*(short int *)(&global_net_message.data[global_msg_readcount-2]))));
}
int MSG_ReadLong() {

   if(global_msg_readcount+3 >= global_net_message.cursize) {
      global_msg_badread = 1;
      return(-1);
   }

   global_msg_readcount += 4;

   return((letohost32(*(int *)(&global_net_message.data[global_msg_readcount-4]))));
}
float MSG_ReadFloat() {

   union floatlong {
      float f;
      int l;
   };

   #if(defined(_MSC_VER) || defined(__BORLANDC__))
    union floatlong ret;
   #endif

   if (global_msg_readcount+3 >= global_net_message.cursize) {
      global_msg_badread = 1;
      return(-1);
   }

   global_msg_readcount += 4;

   #if(defined(_MSC_VER) || defined(__BORLANDC__))
      //unfortunately certain less than intelligent compilers do not like my typedeffing ways.  Silly compilers, always trying to protect me from myself.
      ret.l = letohost32(*(int *)(&global_net_message.data[global_msg_readcount-4]));
      return(ret.f);
   #else
    return(((union floatlong)(letohost32(*(int *)(&global_net_message.data[global_msg_readcount-4])))).f);
   #endif

}

char * MSG_ReadStringStatic(qboolean BrakeForReturns) {

   static char string[2048];
   unsigned int l;
   int c;

   l = 0;

   do {
      c = MSG_ReadChar();

      if(BrakeForReturns != 0 && (c == '\n' || c == '\r')) {
         if(l == 0) { continue; } //We'd be returning an empty string.
         break;
      }
      if(c == -1 || c == 0) { break; }
      string[l] = (char)c;
      l++;
   } while(l < sizeof(string)-1);

   string[l] = '\0';

   return(string);
}
char * MSG_ReadString() { return(MSG_ReadStringStatic(0)); }
char * MSG_ReadStringLine() { return(MSG_ReadStringStatic(1)); }

//my version--returns number of chars written (not including null).
int MSG_ReadStringCore(char * buffer, unsigned int buflen, qboolean BrakeForReturns) {

   const char * src;
   unsigned int i;

   if(buffer == NULL || buflen < 1) { Sys_Error("MSG_ReadStringThreadSafe: Bad arguments."); }

   buflen--; //reserving null.

   src = global_net_message.data + global_msg_readcount;
   i = 0;

   while(i < buflen) {

      if(global_msg_readcount >= global_net_message.cursize) { //Whoops, too big.
         global_msg_badread = 1;
         break;
      }
      if(*src == '\0' || (BrakeForReturns != 0 && (*src == '\r' || *src == '\n'))) {
         break;
      }
      i++;
      global_msg_readcount++;
      *buffer = *src;
      buffer++;
      src++;
   }

   *buffer = '\0';
   return(i);
}
int MSG_ReadStringThreadSafe(char * buffer, unsigned int buflen) { return(MSG_ReadStringCore(buffer, buflen, 0)); }
int MSG_ReadStringLineThreadSafe(char * buffer, unsigned int buflen) { return(MSG_ReadStringCore(buffer, buflen, 1)); }

int MSG_ReadBuf(int len, void *data) {

   if(data == NULL || len < 1) { Sys_Error("MSG_ReadBuf: Bad arguments."); }

   if(global_msg_readcount+len > global_net_message.cursize) {
      global_msg_badread = 1;
      return(-1);
   }

   Q_memcpy(data, global_net_message.data, len);
   global_msg_readcount += len;

   return(1);
}


float MSG_ReadHiresAngle() {
   return(MSG_ReadShort() * (360.0/65536));
}
float MSG_ReadAngle() {
   return(MSG_ReadChar() * (360.0/256));
}

void MSG_WriteChar(sizebuf_t *sb, unsigned int c) {
   byte *buf;

   buf = (byte *)SZ_GetSpace(sb, 1);
   buf[0] = (byte)c;
}
void MSG_WriteChar_C(sizebuf_t *sb, unsigned char c) {
   byte *buf;

   buf = (byte *)SZ_GetSpace(sb, 1);
   buf[0] = c;
}
void MSG_WriteByte(sizebuf_t *sb, unsigned int c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 1);
   buf[0] = (byte)c;
}
void MSG_WriteByte_C(sizebuf_t *sb, byte c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 1);
   buf[0] = c;
}

void MSG_WriteShort(sizebuf_t *sb, int c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 2);
   *(word *)buf = hosttole16((word)c);
}
void MSG_WriteShort_S(sizebuf_t *sb, unsigned short int c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 2);
   *(word *)buf = hosttole16(c);
}
void MSG_WriteWord(sizebuf_t *sb, unsigned int c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 2);
   *(word *)buf = hosttole16((word)c);
}
void MSG_WriteWord_S(sizebuf_t *sb, word c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 2);
   *(word *)buf = hosttole16(c);
}

void MSG_WriteLong (sizebuf_t *sb, uint32 c) {
   byte    *buf;

   buf = (byte *)SZ_GetSpace (sb, 4);
   *(ulong *)buf = hosttole32(c);
}
void MSG_WriteFloat(sizebuf_t *sb, float f) {

   //Unfortunately I'm not confident that the same tricks work with
   //floating point numbers...
   union floatlong {
      float f;
      int l;
   };
   #if(defined(_MSC_VER) || defined(__BORLANDC__))
    union floatlong ret;
   #endif

   char * buf;


   buf = (byte *)SZ_GetSpace (sb, 4);

   /*cast the float to the given union, pull out that union's int member
   //hope there aren't any implied conversions.  This worked in mingw,
   //but it's possible that there could be some compiler dependancy...
   //There are.
   */
   #if(defined(_MSC_VER) || defined(__BORLANDC__))
    ret.f = f;
    *(int *)buf = hosttole32(ret.l);
   #else
    *(int *)buf = hosttole32( ((union floatlong)f).l );
   #endif
}
void MSG_WriteString(sizebuf_t *sb, const char *s) {

   if(s == NULL) { SZ_Write(sb, "", 1); }
   else { SZ_Write(sb, s, Q_strlen(s)+1); }
}
void MSG_WriteBuf(sizebuf_t *sb, int len, const void *buf) {

   if(buf == NULL) {
      Con_Printf("MSG_WriteBuf: Passed null buffer");
      return;
   }
   SZ_Write(sb, buf, len);
}

//unused
void MSG_WriteVec3Coord() { printf("%s called", __FUNCTION__); exit(99); }
void MSG_IsBitWriting() { printf("%s called", __FUNCTION__); exit(99); }
void MSG_ReadVec3Coord() { printf("%s called", __FUNCTION__); exit(99); }
void MSG_IsBitReading() { printf("%s called", __FUNCTION__); exit(99); }

void MSG_BitOpsInit() {

   Q_memset(&bfread,  0, sizeof(bfread));
   Q_memset(&bfwrite, 0, sizeof(bfwrite));
}


void MSG_StartBitReading(sizebuf_t *sb) {

   bfread.Readcount = global_msg_readcount;
   bfread.p2 = global_msg_readcount;
   bfread.BytesRead = 0;
   bfread.BitsRead = 0;
   bfread.ByteStream = sb->data + global_msg_readcount;
   bfread.sz = sb;
   bfread.Readcount++;

   if(sb->cursize < bfread.Readcount) {
      global_msg_badread = 1;
   }
}
int MSG_CurrentBit() {

   //Returns the number of BITS read.
   //This includes the stuff read before calling StartBitReading it seems.
   if(bfread.sz == NULL) {
      return(global_msg_readcount * 8);
   }

   return((bfread.BytesRead * 8) + bfread.BitsRead);
}
int MSG_ReadOneBit() {

   int var_4;

   if(global_msg_badread != 0) { return(1); }

   if(bfread.BitsRead > 7) {

      bfread.BitsRead = 0;
      bfread.Readcount++;
      bfread.BytesRead++;
      bfread.ByteStream++;
   }

   if(bfread.sz->cursize < bfread.Readcount) {
      global_msg_badread = 1;
      return(1);
   }

   //Good ol 'return 1 if the bit is set'.
   var_4 = (bfread.ByteStream[0] & bittable[bfread.BitsRead]) != 0;
   bfread.BitsRead++;
   return(var_4);
}
int MSG_ReadBits(unsigned int count) {

   int var_4;
   int var_8;
   int var_C;

   if(global_msg_badread != 0) { return(1); }

   if(bfread.BitsRead > 7) {

      bfread.BitsRead = 0;
      bfread.Readcount++;
      bfread.BytesRead++;
      bfread.ByteStream++;
   }

   var_C = count + bfread.BitsRead;
   if(var_C > 32) {

      var_4 = (*(uint32 *)(bfread.ByteStream)) >> bfread.BitsRead;
      (*(uint32 *)(bfread.ByteStream))++;

      var_8 = (*(uint32 *)(bfread.ByteStream));
      var_C &= 7;
      var_4 |= (var_8 & rowbittable[var_C]) << (32 - bfread.BitsRead);

      bfread.Readcount += 4;
      bfread.BytesRead += 4;
      bfread.BitsRead = var_C;
   }
   else {

      var_4 = ((*(uint32 *)(bfread.ByteStream)) >> bfread.BitsRead) & rowbittable[count];

      if((var_C & 7) == 0) {

         bfread.BitsRead = 8;
         var_C = (var_C >> 3) - 1;
      }
      else {

         bfread.BitsRead = var_C & 7;
         var_C = var_C >> 3;
      }

      bfread.Readcount += var_C;
      bfread.BytesRead += var_C;
      bfread.ByteStream += var_C;
   }

   if(bfread.sz->cursize < bfread.Readcount) {
      global_msg_badread = 1;
      return(1);
   }

   return(var_4);
}
int MSG_ReadBitData(byte * outData, unsigned int ByteCount) {

   unsigned int i;

   for(i = 0; i < ByteCount; i++, outData++) {

      *outData = (byte)MSG_ReadBits(8);
   }

   return(ByteCount);
}
int MSG_PeekBits(unsigned int count) {

   int var_4;
   bf_read_t oldbfread;


   //Quick & dirty.
   oldbfread = bfread;
   var_4 = MSG_ReadBits(count);
   bfread = oldbfread;

   return(var_4);
}
int MSG_ReadSBits(unsigned int count) {

   int var_4, var_8;

   var_8 = MSG_ReadOneBit();
   var_4 = MSG_ReadBits(count-1);
   if(var_8) { var_4 = -var_4; }

   return(var_4);
}
float MSG_ReadBitAngle(unsigned int count) {

   int var_8;
   float var_c; //?

   var_c = 1 << count;
   var_8 = MSG_ReadBits(count);

   return(var_8 * (360.0 / var_c)); //That's some heavy float perversion.
}
void MSG_EndBitReading(sizebuf_t *sb) {

   if(sb->cursize < bfread.Readcount) {
      global_msg_badread = 1;
   }
   global_msg_readcount = bfread.Readcount;
   //Although I can't imagine any way you can use the data,
   //bfread.BytesRead is NOT zerod out.  Probably wouldn't hurt though.

   bfread.sz = NULL;
   bfread.p2 = 0;
   bfread.BytesRead = 0;
   bfread.BitsRead = 0;
   bfread.ByteStream = NULL;
}

//writing
void MSG_StartBitWriting(sizebuf_t *sb) {

   bfwrite.BitsWritten = 0;
   bfwrite.sz = sb;
   bfwrite.ByteStream = sb->data + sb->cursize;
}
void MSG_WriteOneBit(int i) { //'i' is treated as being zero or nonzero.

   if(bfwrite.BitsWritten > 7) { //How bout some more

      SZ_GetSpace(bfwrite.sz, 1);
      bfwrite.BitsWritten = 0;
      bfwrite.ByteStream++;
   }

   if((bfwrite.sz->overflow & 2) != 0) { return; }

   if(i == 0) {
      *(bfwrite.ByteStream) &= invbittable[bfwrite.BitsWritten];
   }
   else {
      *(bfwrite.ByteStream) |= bittable[bfwrite.BitsWritten];
   }
   bfwrite.BitsWritten++;
}
void MSG_WriteBits(uint32 in, int count) {

   unsigned int i, j, k;

   i = 0;

   if(count < 32 && (in >> count) != 0) {
      in = rowbittable[count];
   }
   if(bfwrite.BitsWritten > 7) { //How bout some more

      i = 1;
      bfwrite.BitsWritten = 0;
      bfwrite.ByteStream++;
   }

   j = bfwrite.BitsWritten + count;
   if(j <= 32) {

      k = j >> 3; //number of bytes we'll need
      j = j & 7;
      if(j == 0) { k--; }

      SZ_GetSpace(bfwrite.sz, i+k);
      (*(uint32 *)(bfwrite.ByteStream)) &= rowbittable[bfwrite.BitsWritten];
      (*(uint32 *)(bfwrite.ByteStream)) |= (in << bfwrite.BitsWritten);

      if(j == 0) { bfwrite.BitsWritten = 8; }
      else { bfwrite.BitsWritten = j; }
      bfwrite.ByteStream += k;
   }
   else {

      SZ_GetSpace(bfwrite.sz, i+4);
      (*(uint32 *)(bfwrite.ByteStream)) &= rowbittable[bfwrite.BitsWritten];
      (*(uint32 *)(bfwrite.ByteStream)) |= (in << bfwrite.BitsWritten);

      in = in >> (32 - bfwrite.BitsWritten);
      bfwrite.BitsWritten = (j & 7);

      bfwrite.ByteStream += 4; //moving up four bytes, or one int.
     // (*(uint32 *)(bfwrite.ByteStream)) = 0;
     // (*(uint32 *)(bfwrite.ByteStream)) |= in;
      (*(uint32 *)(bfwrite.ByteStream)) = in;

      //I just thought of something.  What if this is the last byte?  We could be
      //ANDing the first three bytes of some other string.
   }
}
void MSG_WriteSBits(int32 in, int count) {

   int var_C_mask;


   if(count > 31) { //all bits not used.

      var_C_mask = (1 << (count-1)) - 1;
      if(in > var_C_mask) {
         in = var_C_mask;
      }
      else if(in < -var_C_mask) {
         in = -var_C_mask;
      }
   }

   if(in >= 0) {
      MSG_WriteOneBit(0);
   }
   else {
      MSG_WriteOneBit(1);
      in = -in;
   }

   MSG_WriteBits(in, count-1);
}
void MSG_WriteBitAngle(float in, int count) {

   uint32 var_c, var_8;
   int var_20;


   if(count > 31) {
      Sys_Error("%s: Can't write bit angle with 32 bits precision.", __FUNCTION__);
   }

   var_c = 1 << count;
   var_8 = var_c - 1;

   var_20 = (int)(((float)var_c * in) / 360.0);
   var_20 &= var_8;

   MSG_WriteBits(var_20, count);
}
void MSG_WriteBitCoord(float c) {

   int var_4;
   int var_14, var_20;


   if(c < 0) { var_4 = 1; c = -c; }
   else { var_4 = 0; }

   var_14 = c;
   var_20 = ((int)(c * 8)) & 7;

   MSG_WriteOneBit(var_14);
   MSG_WriteOneBit(var_20);

   if(var_14 == 0 && var_20 == 0) { return; }
   MSG_WriteOneBit(var_4);

   if(var_14 != 0) {
      MSG_WriteBits(var_14, 0x0C);
   }
   if(var_20 != 0) {
      MSG_WriteBits(var_20, 3);
   }
}
void MSG_WriteBitVec3Coord(const vec3_t v) {

   int x, y, z;


   if((v[0] >= 0.125) || (v[0] <= -0.125)) { x = 1; }
   else { x = 0; }
   if((v[1] >= 0.125) || (v[1] <= -0.125)) { y = 1; }
   else { y = 0; }
   if((v[2] >= 0.125) || (v[2] <= -0.125)) { z = 1; }
   else { z = 0; }

   MSG_WriteOneBit(x);
   MSG_WriteOneBit(y);
   MSG_WriteOneBit(z);

   if(x != 0) { MSG_WriteBitCoord(v[0]); }
   if(y != 0) { MSG_WriteBitCoord(v[1]); }
   if(z != 0) { MSG_WriteBitCoord(v[2]); }
}
void MSG_WriteBitString(const char * str) {

   const char * ptr;


   for(ptr = str; *ptr != '\0'; ptr++) {

      MSG_WriteBits(*ptr, 8);
   }

   //and the terminating null
   MSG_WriteBits(0, 8);
}
void MSG_WriteBitData(const char * str, int size) {

   int i;
   const char * ptr;


   for(i = 0, ptr = str; i < size; i++, ptr++) {
      MSG_WriteBits(*ptr, 8);
   }
}
void MSG_EndBitWriting() {

   if((bfwrite.sz->overflow & 2) != 0) { return; }

   *(bfwrite.ByteStream) &= ((unsigned)0xFF >> (8 - bfwrite.BitsWritten));

   SZ_GetSpace(bfwrite.sz, 1); //Seems odd, we also increment a var before leaving.  Must be a #define.
   Q_memset(&bfwrite, 0, sizeof(bfwrite));
}

void MSG_ReadUsercmd(usercmd_t * arg_0, usercmd_t * arg_4) {

   sv_builddelta_t * var_4_delta;

   var_4_delta = SV_LookupDelta("usercmd_t");

   MSG_StartBitReading(&global_net_message);
   DELTA_ParseDelta((char *)arg_4, (char *)arg_0, var_4_delta);
   MSG_EndBitReading(&global_net_message);

   NormalizeAngles(arg_0->viewangles);
}


//lifted directly from QW. *shudder* floating point muls and divs...
void MSG_WriteAngle(sizebuf_t *sb, float f) {

   MSG_WriteByte(sb, (int)(f*(256/360)) & 0xFF);
}
void MSG_WriteHiresAngle(sizebuf_t *sb, float f) {

   MSG_WriteShort(sb, (int)(f*(65536/360)) & 0xFFFF);
}

void MSG_WriteCoord(sizebuf_t *sb, float f) {
   MSG_WriteShort(sb, (int)(f*8.0));
}
