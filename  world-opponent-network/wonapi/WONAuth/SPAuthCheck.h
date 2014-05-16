#ifndef __WON_SPAUTHCHECK_H__
#define __WON_SPAUTHCHECK_H__
#include "WONShared.h"

#include "WONStatus.h"

namespace WONAPI
{

enum WONStatus;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SPAuthCheck
{
public:
	static void Init(const char *theProduct);
	static void SetCommunity(const wchar_t* theCommunity);
	static void SetProductDir(const wchar_t* thePath);
	static void SetOnlyForceCheck(bool onlyForceCheck);
	static void AddDirServer(const char *theAddr);
	static void StartChecking();
	static void Destroy();

	static bool HadError();
	static WONStatus GetStatus();
};

} // namespace WONAPI;

#endif