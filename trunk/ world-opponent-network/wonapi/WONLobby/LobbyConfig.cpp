#include "LobbyConfig.h"

#include "WONLobbyPrv.h"

#include "WONCommon/RegKey.h"

using namespace WONAPI;
using namespace std;

LobbyConfig::ConfigAllocator LobbyConfig::mAllocator = LobbyConfig::DefaultAllocator;
LobbyConfig* LobbyConfig::DefaultAllocator() { return new LobbyConfig; }
std::string LobbyConfig::mDefaultFileName = "WONLobby.cfg";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyConfig::LobbyConfig(void)
{
	RegisterStringListValue("DirServer",mDirServersList,NULL,true);
	RegisterStringListValue("RoomDirServer",mRoomDirServersList,NULL,true);
	RegisterStringListValue("AuthServers",mAuthServers,NULL,true);
	RegisterStringListValue("BigFile",mBigFiles,NULL,true);

	RegisterStringValue("ProductName",mProductName);
	RegisterWStringValue("ProductDirectory",mProductDirectory);
	RegisterStringValue("ResourceDirectory",mResourceDirectory);
	RegisterStringValue("Version",mVersion);
	RegisterStringValue("LanVersion",mLanVersion);
	RegisterStringValue("Language",mLanguage,"");
	RegisterBoolValue("CRCCheck",mIsCRCCheck,true);
	RegisterBoolValue("CDKeyCheck",mIsCDKeyCheck,true);
	RegisterBoolValue("Public",mIsPublic,false);
	RegisterUShortValue("LanBroadcastPort",mLANBroadcastPort,8585);
	RegisterBoolValue("NoPingInProgress",mNoPingInProgress,true);
	RegisterUShortValue("LobbyPort",mLobbyPort,8888);
	RegisterBoolValue("CheckForInternet",mCheckForInternet,true);
	RegisterUShortValue("PingCeiling",mPingCeiling,1000);
	RegisterBoolValue("AllowDuplicateNames",mAllowDuplicateNames,true);
	RegisterBoolValue("AppendNumberToDefaultLanName",mAppendNumberToDefaultLanName,true);
	RegisterBoolValue("AllowJoinGameWithoutPing",mAllowJoinGameWithoutPing,true);

	// Check registry for UseLocalIP option
	bool aDefUseLocalIP = false;
	RegKey aKey(REGKEY_LOCAL_MACHINE,"SOFTWARE\\Sierra\\WONLobby");
	unsigned long aVal = 0;
	if(aKey.GetValue("UseLocalIP", aVal)==RegKey::Ok)
		aDefUseLocalIP = aVal!=0;

#ifdef _DEBUG
	RegisterBoolValue("UseLocalIP",mUseLocalIP,aDefUseLocalIP);
#else
	mUseLocalIP = aDefUseLocalIP;
	static bool aFakeUseLocalIP;
	RegisterBoolValue("UseLocalIP",aFakeUseLocalIP,aDefUseLocalIP); // don't let user set UseLocalIP value using the config file
#endif

}
 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyConfig* LobbyConfig::GetLobbyConfig()
{
	if(gLobbyData.get()!=NULL)
		return gLobbyData->mLobbyConfig;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyConfig::HandleKeyVal(ConfigParser &theParser, const std::string &theKey, StringParser &theVal)
{
	if(theKey=="LOBBYDEFINE")
	{
		std::string aKey, aVal;
		theVal.ReadString(aKey,true);
		theVal.ReadString(aVal);
		if(!aKey.empty())
			mDefineMap[aKey] = aVal;
	}
	else
		return ConfigObject::HandleKeyVal(theParser,theKey,theVal);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyConfig::PostParse(ConfigParser &theParser)
{
	if(mDirServersList.empty())
	{
		theParser.AbortRead("No Directory Servers specified.");
		return false;
	}

	if(mProductName.empty())
	{
		theParser.AbortRead("No product name specified.");
		return false;
	}

	if(mVersion.empty())
	{
		theParser.AbortRead("No version specified.");
		return false;
	}

	if(mLanguage.empty())
	{
		theParser.AbortRead("No language specified.");
		return false;
	}

	if(mLanVersion.empty())
		mLanVersion = mVersion;

	if(mProductDirectory.empty())
		mProductDirectory = wstring(L"/") + StringToWString(mProductName);

	mDefineMap["Product"] = mProductName;

	return true;
}