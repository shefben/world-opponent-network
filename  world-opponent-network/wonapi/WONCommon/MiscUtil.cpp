//----------------------------------------------------------------------------
// MiscUtil.cpp
//
// This file was cribbed (and modified several times) from AdBanner.
//----------------------------------------------------------------------------
#include "MiscUtil.h"

using namespace std;
namespace WONAPI
{

BrowseCallback gPreBrowseCallback = NULL;
void*          gPreBrowseData     = NULL;


//----------------------------------------------------------------------------
// Set the callback to invoke before kicking off the browser.
//----------------------------------------------------------------------------
void SetPreBrowseCallback(BrowseCallback pCallback, void* pData)
{
	gPreBrowseCallback = pCallback;
	gPreBrowseData = pData;
}

//----------------------------------------------------------------------------
// Start the user's browser and take them to the supplied URL.
//----------------------------------------------------------------------------
bool Browse(const std::string& sURL)
{
#ifdef WIN32_NOT_XBOX
	string              sBrowser;
	BOOL                bOK;
	STARTUPINFO         aStartupInfo;
	PROCESS_INFORMATION aProcessInfo;
	string              sCommandLine = TEXT(" ");

	// Inform watchers that we are about to start the browser.
	if (gPreBrowseCallback)
		(*gPreBrowseCallback)(sURL, gPreBrowseData);

	GetBrowserCommandLineFromRegistry(sBrowser);
	if (sBrowser.length() == 0)
		return FALSE;

	sCommandLine += sURL;
	memset(&aStartupInfo, 0, sizeof(STARTUPINFO));
	aStartupInfo.cb = sizeof(STARTUPINFO);

	bOK = CreateProcess(sBrowser.c_str(), (LPSTR)sCommandLine.c_str(), NULL, NULL, FALSE, 0, NULL,
						NULL, &aStartupInfo, &aProcessInfo);

	if (bOK == TRUE)
	{
		CloseHandle(aProcessInfo.hThread);
		CloseHandle(aProcessInfo.hProcess);
	}

	return bOK == TRUE;
#else
	return false;
#endif
}


//----------------------------------------------------------------------------
// Fetch the user's default browser.
//----------------------------------------------------------------------------
bool GetBrowserCommandLineFromRegistry(std::string& sBrowser)
{
#ifdef WIN32_NOT_XBOX
	LONG  nLong;
	HKEY  hKey;
	TCHAR sValue[MAX_PATH];
	DWORD nValueLength = sizeof(sValue);
	TCHAR sShortPath[MAX_PATH];

	sBrowser = TEXT("");

	// Find out what the preferred browser is by looking to see if .html is 
	// associated with NetscapeMarkup or htmlfile.
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, TEXT(".html"), 0, KEY_QUERY_VALUE, &hKey)  != ERROR_SUCCESS)
		return false;

	nLong = RegQueryValueEx(hKey, TEXT(""), NULL, NULL, (LPBYTE) sValue, &nValueLength);
	RegCloseKey(hKey);
	if (nLong != ERROR_SUCCESS)
		return false;

	string sTemp = sValue;
	sTemp += TEXT("\\shell\\open\\command");
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sTemp.c_str(), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
		return false;

	nValueLength = sizeof(sValue);
	nLong = RegQueryValueEx(hKey, "", NULL, NULL, (LPBYTE)sValue, &nValueLength);
	RegCloseKey(hKey);
	if (nLong != ERROR_SUCCESS)
		return false;

	sTemp = sValue;

	int nLocation = sTemp.find('\"');
	if (nLocation == 0)
	{
		sTemp.erase(0, 1); // Remove the leading quote.
		nLocation = sTemp.find('\"', 1);
		if (nLocation != -1)
			sTemp.erase(nLocation, sTemp.length() - nLocation); // Remove the traling quote (and anything following it).

		DWORD nRetValue = GetShortPathName(sTemp.c_str(), sShortPath, MAX_PATH);
		if (nRetValue == 0 || nRetValue >= MAX_PATH)
			return false;
		else
			sBrowser = sShortPath;
	}
	else
	{
		if (nLocation != -1)
		{
			sTemp.erase(nLocation, sTemp.length() - nLocation);
		}

		nLocation = sTemp.find("%1");
		if (nLocation != -1)
		{
			sTemp.erase(nLocation, sTemp.length() - nLocation);
		}

		sBrowser = sTemp;

		UINT nLen = sBrowser.length();
		while (nLen > 0 && (sBrowser[nLen - 1] == ' ' || 
			sBrowser[nLen - 1] == '\t' || sBrowser[nLen - 1] == '\n'))
		{
			sBrowser.erase(nLen - 1);
			nLen = sBrowser.length();
		}
	}

	return sBrowser.length() > 0;
#else
	return false;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//  in: thePath = path for sierra up, defaults to executable directory
// out: theError = Why the app couldn't start
DWORD LaunchSierraUp(const char* thePath)
{
#ifdef WIN32_NOT_XBOX
	char sPath[MAX_PATH];
	GetModuleFileName(NULL, sPath, sizeof(sPath));

	// Setup out parameters from CreateProcess
	PROCESS_INFORMATION aProcessInfo;
	STARTUPINFO aStartInfo;	
	memset(&aStartInfo, 0, sizeof(STARTUPINFO));
	aStartInfo.cb = sizeof(STARTUPINFO);

	// Remove the file name.
	char* pBS = strrchr(sPath, '\\');
	if (pBS)
		pBS[1] = 0;

	// Construct the path to SierraUp
	std::string aCommandLine;
	if (thePath == NULL)
		aCommandLine = sPath;
	else
		aCommandLine = thePath;

	// Append executable name
	aCommandLine += std::string("SierraUp.exe");

	// Launch Sierra Up
	if (!::CreateProcess(NULL, (char*) aCommandLine.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &aStartInfo, &aProcessInfo))
		return GetLastError();

	// Success
	return 0;
#else
	return 0;
#endif
}

} // namespace