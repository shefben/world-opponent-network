/* This file, in an effort to try piecing together the calling conventions
// used by mods, defines two vars.  That's it.
//
// It's in this file instead of Modding because Modding.h includes a lot
// of other headers.  And we don't want to risk roundabout dependencies.
*/

#ifndef OSHLDS_CALLING_CONVENTIONS_HEADER
#define OSHLDS_CALLING_CONVENTIONS_HEADER

// *Current notes: structure alignment (how much padding they get to reach a
// boundary) is 8.  GCC says it uses 8 by default; VS has more control over
// that, but lists 8 as default and it's the default for mods themselves)
// *The difference between a working MSVC6 DLL and a nonworking one is the
// optimization "Frame Pointer Omission".  Nobody seems to know why.
// *Traces through the few working functions indicate EBX ESI EDI are correctly
// popped.  Subtle corruptions seems unlikely.

//Even if these end up being useless, it's still an easy way to ID mod functions.
#ifdef _WIN32
 #define  HLDS_DLLEXPORT  __declspec(dllexport)
 #define  HLDS_DLLIMPORT  __declspec(dllimport)
#else
 #define  HLDS_DLLEXPORT
 #define  HLDS_DLLIMPORT
#endif

#endif
