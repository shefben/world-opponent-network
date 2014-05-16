#ifndef __WON_LobbyConfig_H__
#define __WON_LobbyConfig_H__

#include "WONConfig/ConfigObject.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyConfig : public ConfigObject
{
public:
	LobbyConfig();
	virtual bool PostParse(ConfigParser &theParser);

	static std::string mDefaultFileName;

	WONTypes::StringList mDirServersList;
	WONTypes::StringList mRoomDirServersList;
	WONTypes::StringList mAuthServers;
	WONTypes::StringList mBigFiles;

	std::string          mProductName;
	std::wstring         mProductDirectory;
	std::string          mVersion;
	std::string          mLanVersion;
	std::string          mLanguage;
	std::string          mResourceDirectory;
	bool                 mIsCRCCheck;
	bool                 mIsCDKeyCheck;
	bool                 mIsPublic;
	unsigned short       mLANBroadcastPort;
	bool                 mUseLocalIP;
	bool                 mNoPingInProgress;
	unsigned short		 mLobbyPort;
	bool                 mCheckForInternet;
	unsigned short		 mPingCeiling;
	bool				 mAllowDuplicateNames;
	bool				 mAppendNumberToDefaultLanName;
	bool				 mAllowJoinGameWithoutPing;

	typedef std::map<std::string,std::string,StringLessNoCase> LobbyDefineMap;
	LobbyDefineMap mDefineMap;


	typedef LobbyConfig*(*ConfigAllocator)();
	static ConfigAllocator mAllocator;
	static LobbyConfig* DefaultAllocator();

	virtual bool HandleKeyVal(ConfigParser &theParser, const std::string &theKey, StringParser &theVal);

public:
	static LobbyConfig* GetLobbyConfig();
	static void SetAllocator(ConfigAllocator theAllocator) { mAllocator = theAllocator; }
	static void SetDefaultFileName(const std::string &theName) { mDefaultFileName = theName; }

};
typedef SmartPtr<LobbyConfig> LobbyConfigPtr;

};

#endif