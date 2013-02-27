#ifndef OSHLDS_SIZEBUF_HEADER
#define OSHLDS_SIZEBUF_HEADER

#include "endianin.h"

typedef struct sizebuf_s {
   char * debugname;
   int overflow; //byte 1 = allowoverflow, byte 2 = overflowed
   byte *data;
   unsigned int maxsize;
   unsigned int cursize;
//   int packing;
//   int currentbit;
} sizebuf_t;

// functions

void SZ_Clear(sizebuf_t *);
void *SZ_GetSpace(sizebuf_t *, unsigned int);
void SZ_Write(sizebuf_t *, const void *, int);
void SZ_Print(sizebuf_t *, const char *);
void SZ_Alloc(char *, sizebuf_t *, int);

#endif
