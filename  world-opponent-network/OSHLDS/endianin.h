/* Endianness.  HL swaps a lot of bytes and oes a lot of non-network
// order transmissions.  But it's all inline, as it should be.
// These macros were lifted from the stack implementations
// and altered to let us sing both ways.
//
// This file also contains variable typedefs, but NOT classes.
*/

#ifndef HLDS_LINUX_ENGINE_ENDIAN
#define HLDS_LINUX_ENGINE_ENDIAN

//A flag that I use to debug.  Paranoia code blocks should all
//be expendible.
#define HLDS_PARANOIA 1


//some basic and well known conventions.  Todo:  play with preprocessor sizeof decs
//to solidly force 8/16/32 bit var types and whatnot.

typedef unsigned char BYTE;
typedef unsigned char byte;
typedef unsigned short int uint16;
typedef unsigned short int UINT16;
typedef unsigned short int word;
typedef unsigned short int WORD;
typedef signed short int int16;
typedef signed short int INT16;
typedef unsigned long int uint32;
//typedef unsigned long int UINT32;
typedef unsigned long int ulong;
typedef unsigned long int ULONG;
typedef signed long int int32;
//typedef signed long int INT32;
#ifdef _WIN32

 #ifdef __GNUC__
  typedef signed long long int64;
  typedef signed long long INT64;
  typedef unsigned long long uint64;
  typedef unsigned long long UINT64;
 #else
  typedef __int64 int64;
  typedef __int64 INT64;
 // typedef __uint64 uint64;
 // typedef __uint64 UINT64;

 #endif
#endif
//Some lesser known Quake conventions
typedef int BOOL;
typedef int qboolean;

//A few other typedefs that work best when in a standalone header
typedef int string_t; //The number of bytes away from an arbitrary string.  To make life difficult.
typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

// Control (by default you should be able to override these, but for the lazy,
// the compiler can decide.

#if ((defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)) || (!defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)))

   /* manual deciding */
   #define HLDS_WE_ARE_USING_LITTLE_ENDIAN
   //#define HLDS_WE_ARE_USING_BIG_ENDIAN

#elif (defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN))
   #define HLDS_WE_ARE_USING_BIG_ENDIAN

#elif (!defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN))
   #define HLDS_WE_ARE_USING_LITTLE_ENDIAN
#endif


#if !defined(HLDS_WE_ARE_USING_BIG_ENDIAN) && !defined(HLDS_WE_ARE_USING_LITTLE_ENDIAN)
   #error "Endianness not determined.  Manually decide your platform around line 40 of this file."

# elif defined(HLDS_WE_ARE_USING_BIG_ENDIAN) && defined(HLDS_WE_ARE_USING_LITTLE_ENDIAN)
   #error "Both endians defined.  I don't know how you got here, but fix it.

#else

   #define FlipBytes32(A) ((((uint32)(A) & 0xff000000) >> 24) | (((uint32)(A) & 0x00ff0000) >> 8)  | (((uint32)(A) & 0x0000ff00) << 8)  | (((uint32)(A) & 0x000000ff) << 24))
   #define FlipBytes16(A) ((((uint16)(A) & 0xff00) >> 8) | (((uint16)(A) & 0x00ff) << 8))

   #if defined(HLDS_WE_ARE_USING_LITTLE_ENDIAN)

      #define letohost32(A) A
      #define letohost16(A) A
      #define betohost32(A) FlipBytes32(A)
      #define betohost16(A) FlipBytes16(A)

      #define hosttole32(A) A
      #define hosttole16(A) A
      #define hosttobe32(A) FlipBytes32(A)
      #define hosttobe16(A) FlipBytes16(A)

   #else

      #define letohost32(A) FlipBytes32(A)
      #define letohost16(A) FlipBytes16(A)
      #define betohost32(A) A
      #define betohost16(A) A

      #define hosttole32(A) FlipBytes32(A)
      #define hosttole16(A) FlipBytes16(A)
      #define hosttobe32(A) A
      #define hosttobe16(A) A

   #endif

#endif


#endif
