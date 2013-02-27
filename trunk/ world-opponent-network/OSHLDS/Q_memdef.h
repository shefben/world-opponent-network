/* The Quakeworld code doesn't ever call memcpy directly.  Instead it
// calls q_memcpy, which is #defined into regular old memcpy.  Other
// mem functions get this treatment too.  I'm guessing it's so they can do
// a bunch of ASSERTs in debugging, so I'll keep it around.  Less for me
// to write anyway.
*/

#ifndef OSHLDS_Q_MEMFUNCTIONS_HEADER
#define OSHLDS_Q_MEMFUNCTIONS_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Basic mem functions
#define Q_memset(d, f, c)        memset((d), (f), (c))
#define Q_memcpy(d, s, c)        memmove((d), (s), (c)) //A couple times I noticed memcpy was being used where memmove should've been.  This is a hack.
#define Q_memmove(d, s, c)       memmove((d), (s), (c))
#define Q_memcmp(m1, m2, c)      memcmp((m1), (m2), (c))
#define Q_strcpy(d, s)           strcpy((d), (s))
#define Q_strncpy(d, s, n)       strncpy((d), (s), (n))
#define Q_strlen(s)              strlen(s)
#define Q_strstr(s1, s2)         strstr(s1, s2)

#define Q_strrchr(s, c)          strrchr((s), (c))
#define Q_strcat(d, s)           strcat((d), (s))
#define Q_strcmp(s1, s2)         strcmp((s1), (s2))
#define Q_strcmp_ptr             strcmp
#define Q_strncmp(s1, s2, n)     strncmp((s1), (s2), (n))
#if(defined(_MSC_VER) && _MSC_VER <= 1200) //means an old version of VS.  I know 6.0 requires this.
 #define Q_strcasecmp(s1, s2)     stricmp((s1), (s2))
#else
 #define Q_strcasecmp(s1, s2)     strcasecmp((s1), (s2))
#endif
#define Q_strncasecmp(s1, s2, n) strncasecmp((s1), (s2), (n))

//The mallocs.  Can be handy for using debug malloc libs.
#define Q_Malloc(s1)             malloc(s1)
#define Q_Realloc(s1, s2)        realloc(s1, s2)
#define Q_Calloc(s1, s2)         calloc(s1, s2)
#define Q_Free(s1)               free(s1)
#define Q_Strdup(s1)             strdup(s1)

//These two were actually written out.  Do they differ from the standard calls?
#define Q_atoi(x) atoi(x)
#define Q_atof(x) atof(x)

//THESE I added because I kept using them even though they're not really handy to override.
#define Q_sprintf sprintf

#if((defined(_MSC_VER) && _MSC_VER <= 1200) || __BORLANDC__)
 #define Q_snprintf _snprintf //Turns out this one IS handy.
 #define Q_vsnprintf _vsnprintf
#else
 #define Q_snprintf snprintf
 #define Q_vsnprintf vsnprintf
#endif
#define Q_strchr strchr
#define Q_strtok strtok
#endif
