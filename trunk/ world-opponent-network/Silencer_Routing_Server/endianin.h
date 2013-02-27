/* Endianness.  And typedefs.  Never know when you need one of those.  Still
// looking for a nice, uniform and portable fix here.
*/


#ifndef __WE_HAVE_ALREADY_INCLUDED_ENDIAN
#define __WE_HAVE_ALREADY_INCLUDED_ENDIAN

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
typedef unsigned int uint32;
typedef unsigned int UINT32;
typedef unsigned long int ulong;
typedef unsigned long int ULONG;
typedef signed int int32;
typedef signed int INT32;

//Some lesser known Quake conventions I occasionally use
typedef int BOOL;
typedef int qboolean;



// Control (you should be able to override these, but for the lazy,
// the compiler can decide)

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
