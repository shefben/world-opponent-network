/*

attribute stuff.  I only really use __noreturn; for some reason this screws up
compiling on my Fedora install if included before ctype.h (but including system
headers before custom ones is the proper way anyway).
*/

#ifndef HLDS_LINUX_GCCOPS
#define HLDS_LINUX_GCCOPS

#if __GNUC__ >= 3

//# define inline      inline __attribute__ ((always_inline))
# define __pure      __attribute__ ((pure))
//# define __const   __attribute__ ((const)) //causes trouble
# define __noreturn   __attribute__ ((noreturn))
# define __malloc   __attribute__ ((malloc))
# define __must_check   __attribute__ ((warn_unused_result))
# define __deprecated   __attribute__ ((deprecated))
# define __used      __attribute__ ((used))
# define __unused   __attribute__ ((unused))
# define __packed   __attribute__ ((packed))
# define likely(x)   __builtin_expect (!!(x), 1)
# define unlikely(x)   __builtin_expect (!!(x), 0)

#else

#if _MSC_VER <= 1200 //VS6 tested, fails without this
 # define inline __inline
 # define __noreturn __declspec(noreturn)
#else
 # define __noreturn
#endif


# define __pure      /* no pure */
# define __malloc   /* no malloc */
# define __must_check   /* no warn_unused_result */
# define __deprecated   /* no deprecated */
# define __used      /* no used */
# define __unused   /* no unused */
# define __packed   /* no packed */
# define likely(x)   (x)
# define unlikely(x)   (x)

#endif

#endif
