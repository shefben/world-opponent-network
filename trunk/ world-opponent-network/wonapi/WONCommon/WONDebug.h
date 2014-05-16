#ifndef __WON_WONDEBUG_H__
#define __WON_WONDEBUG_H__
#include "WONShared.h"

#include <string>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONDebug
{
public:
	static void SetStackFileName(const std::string &theName);
	static const std::string& GetStackFileName();

	static int DumpStackToFile(void *theInfo = NULL);
};

#ifdef WIN32
#define WON_BEGIN_DEBUG __try {
#define WON_END_DEBUG } __except(WONDebug::DumpStackToFile(GetExceptionInformation())) { }
#else
#define WON_BEGIN_DEBUG
#define WON_END_DEBUG
#endif

}

#endif