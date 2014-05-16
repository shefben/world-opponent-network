#ifndef __WON_MISCUTIL_H__
#define __WON_MISCUTIL_H__
#include "WONShared.h"
#include "WONCommon/Platform.h"

#include <string>

namespace WONAPI
{

typedef void (*BrowseCallback)(const std::string& sURL, void* pData);

void SetPreBrowseCallback(BrowseCallback pCallback, void* pData);
bool GetBrowserCommandLineFromRegistry(std::string& sBrowser);
bool Browse(const std::string& sURL);
DWORD LaunchSierraUp(const char* thePath=NULL);

}

#endif
