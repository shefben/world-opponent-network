#ifndef __WON_WONSHARED_H__
#define __WON_WONSHARED_H__
#include <stdio.h>

#include "WONSharedCustom.h" // library user can override this file to disable extra warnings, etc... (default is a blank file)

#ifdef _MSC_VER

#pragma warning(disable:4503)
#pragma warning(disable:4786)
#pragma warning(4: 4018 4114 4146 4244 4245)
#include <string> // need to include string before warning 4786 gets re-enabled somewhere (not sure how it's re-enabled)

#elif !defined(WIN32) && defined(_WIN32)
#define WIN32

#endif // _MSV_VER

#ifdef __WON_MASTER_CPP_BUILD__
#ifndef __WON_MASTER_CPP__
#error You only need to compile the master cpp files (i.e. Ad.cpp, Auth.cpp, Common.cpp, Crypt.cpp, etc...)  You may need to reload the project now in order to get the error to go away.
#endif // WON_MASTER_CPP
#endif // __WON_MASTER_CPP_BUILD__

#include "WONSharedCustom.h" // (need to include this twice since <string> turns some warnings back on 

#endif
