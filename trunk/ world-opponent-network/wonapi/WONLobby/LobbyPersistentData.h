#ifndef __WON_LOBBYPERSISTENTDATA_H__
#define __WON_LOBBYPERSISTENTDATA_H__

#include "WONCommon/FileReader.h"
#include "WONCommon/FileWriter.h"
#include "WONGUI/GUIString.h"
#include "WONSocket/IPAddr.h"
#include "WONCrypt/Blowfish.h"
#include <map>
#include <set>
#include <vector>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyFriend;
class LobbyServer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServerListFilterSet
{
protected:
	typedef std::list<GUIString> FilterList;
	typedef std::map<GUIString,FilterList::iterator> FilterMap; 

	FilterMap mFilterMap;
	FilterList mFilterList;

public:
	typedef FilterList::iterator iterator;
	typedef FilterList::const_iterator const_iterator;
	
	void insert(const GUIString &theStr);
	bool erase(const GUIString &theStr);
	void clear();

	iterator begin() { return mFilterList.begin(); }
	const_iterator begin() const { return mFilterList.begin(); }
	iterator end() { return mFilterList.end(); }
	const_iterator end() const { return mFilterList.end(); }
	bool empty() const { return mFilterMap.empty(); }
	int size() const { return mFilterMap.size(); }
};

typedef std::map<GUIString,ServerListFilterSet,GUIStringLessNoCase> ServerListFilterMap;

class LobbyPersistentData : public RefCount
{
public:
	struct UserInfo
	{
		int mFileId;
		time_t mSysTOUTime;
		time_t mGameTOUTime;

		time_t mLastUseTime;
		GUIString mUserName;
		GUIString mPassword;


		UserInfo() : mLastUseTime(0), mFileId(0), mSysTOUTime(0), mGameTOUTime(0) { }
	};
	typedef std::map<GUIString,UserInfo,GUIStringLessNoCase> UserInfoMap;
	typedef std::map<IPAddr,GUIString> DirectConnectMap;
	typedef std::map<GUIString,time_t,GUIStringLessNoCase> LanNameInfoMap;


protected:
	typedef std::set<int> FileIdSet;
	FileIdSet mFileIdSet;
	int GetFreeFileId();
	int mCurFileId;

	struct ReconnectInfo
	{
		time_t mLastUseTime;
		int mReconnectId;

		ReconnectInfo() : mLastUseTime(0) { }
	};
	typedef std::map<IPAddr,ReconnectInfo> ReconnectInfoMap;



	// IGNORE
	typedef std::set<GUIString,GUIStringLessNoCase> IgnoreNameSet;
	IgnoreNameSet mIgnoreNameSet;

	// BLOCK
	typedef std::set<unsigned long> BlockedIdSet;
	BlockedIdSet mBlockedIdSet;

	// FRIEND
	typedef SmartPtr<LobbyFriend> LobbyFriendPtr;
	typedef std::map<unsigned long, LobbyFriendPtr> LobbyFriendMap;
	LobbyFriendMap mFriendMap;

	//Server List
	ServerListFilterMap mDefaultServerListFilterMap;
	ServerListFilterMap mServerListFilterMap;

	// OPTIONS
	bool mFilterDirtyWords;
	bool mDoAutoAFK;
	bool mShowCrossPromotion;
	GUIString mDefaultTabName;
	bool mLobbySoundEffects;
	bool mLobbyMusic;
	bool mChatSoundEffects;
	bool mStagingSoundEffects;
	bool mShowConfirmations;
	bool mAutoPingGames;
	bool mShowGamesInProgress;
	bool mAnonymousToFriends;
	unsigned short mMaxPings;
	unsigned short mLanBroadcastPort;
	GUIString mLanName;

	bool mGlobalLobbySoundEffects; 	// from the global file as opposed to the user file
	bool mGlobalLobbyMusic; 		// from the global file as opposed to the user file
	long mNetworkAdapter;			

	// Lobby Memory
	GUIString mDefLobbyServer;
	GUIString mDefLobbyGroup;

	bool mNeedWriteGlobal;
	bool mNeedWriteUser;



	static LobbyPersistentData* GetPersistentData();



protected:
	UserInfoMap mUserInfoMap;
	ReconnectInfoMap mReconnectInfoMap;
	Blowfish mEncryptKey;
	LanNameInfoMap	mLanNameInfoMap;
	DirectConnectMap mDirectConnectMap;

	GUIString ReadAsciiString(FileReader &theReader);
	GUIString ReadWideString(FileReader &theReader);
	GUIString ReadEncryptedWideString(FileReader &theReader);

	void WriteAsciiString(FileWriter &theWriter, const GUIString &theStr);
	void WriteWideString(FileWriter &theWriter, const GUIString &theStr);
	void WriteEncryptedWideString(FileWriter &theWriter, const GUIString &theStr);

	GUIString GenerateDefaultName();
	void RemoveExtraUserNames();
	std::string GetUserFileName(int theFileId);

	int ReadFileSignature(FileReader &theReader);
	void WriteFileSignature(FileWriter &theWriter, int theVersion);

	void RemoveExtraLanNames();
	void SetLastUsedLanName();

	void WriteLanData(FileWriter &theWriter);
	void ReadLanData(FileReader &theReader);

	void ReadUserData();
	void WriteUserData();

	void ReadGlobalData();
	void WriteGlobalData();
	void RestoreDefaultUserOptions();

	friend class WONLobbyPrv;
	LobbyPersistentData();

public:
	// Internet Info
	// Note that AddUserInfo, GetCurUserInfo, and GetUserInfo can all return NULL
	static UserInfo* AddUserInfo(const GUIString &theUserName, const GUIString &thePassword, bool mustExistAlready = false);
	static UserInfo* GetCurUserInfo();
	static const UserInfoMap& GetUserInfoMap();
	static UserInfo* GetUserInfo(const GUIString &theUserName);
	static GUIString GetPassword(const GUIString &theUserName);
	static void SetCurUser(UserInfo *theUser);

	// Ignoring
	static const IgnoreNameSet& GetIgnoreNameSet();
	static bool IsIgnored(const GUIString &theName);
	static bool IsBlocked(unsigned long theId);
	static void SetIgnore(const GUIString &theName, bool isIgnored);
	static void SetBlocked(unsigned long theId, bool isBlocked);

	// Friends
	// Note that GetFriend and AddFriend can both return NULL
	static const LobbyFriendMap& GetFriendMap();
	static LobbyFriend* GetFriend(unsigned long theWONUserId);
	static LobbyFriend* AddFriend(const GUIString& theClientName, unsigned long theWONUserId);
	static void RemoveFriend(unsigned long theWONUserId);

	// Lan Name
	static const GUIString& GetLanName();
	static void SetLanName(const GUIString &theName);
	static void RemoveLanName();
	static const LanNameInfoMap& GetLanNameInfoMap();

	// Direct Connections
	static void AddDirectConnection(const IPAddr& theAddress, const GUIString& theDescription);
	static void RemoveDirectConnection(const IPAddr& theAddress);
	static const DirectConnectMap& GetDirectConnectMap();

	// RoutingServer
	static void AddReconnectId(const IPAddr &theAddr, int theReconnectId);
	static int GetReconnectId(const IPAddr &theAddr);
	static void SetDefLobbyRoom(const GUIString& theServer, const GUIString& theGroup);
	static const GUIString& GetDefLobbyServer(); 
	static const GUIString& GetDefLobbyGroup();

	// ServerListCtrl
	static const ServerListFilterSet& GetServerListFilterSet(const GUIString &theColumnName);
	static void AddServerListFilter(const GUIString &theColumnName, const GUIString &theFilterString);
	static void RemoveServerListFilter(const GUIString &theColumnName, const GUIString &theFilterString);
	static void AddDefaultServerListFilters(const GUIString &theColumn, const GUIString &theStrings, bool addIfColumnExists); // comma delimited list
	static ServerListFilterSet& GetDefaultServerListFilters(const GUIString &theColumn);
	static void ResetServerListFilters();

	// Options page options
	static const GUIString& GetDefaultTabName();
	static bool GetFilterDirtyWords();
	static bool GetDoAutoAFK();
	static bool GetShowCrossPromotion();
	static bool GetLobbySoundEffects();
	static bool GetChatSoundEffects();
	static bool GetStagingSoundEffects();
	static bool GetLobbyMusic();
	static bool GetShowConfirmations();
	static bool GetAutoPingGames();
	static bool GetShowGamesInProgress();
	static bool GetAnonymousToFriends();
	static unsigned short GetMaxPings();
	static unsigned short GetLanBroadcastPort();
	static long GetNetworkAdapter();

	static void SetFilterDirtyWords(bool filter);
	static void SetDoAutoAFK(bool doAutoAFK);
	static void SetShowCrossPromotion(bool show);
	static void SetDefaultTabName(const GUIString& theTabName);
	static void SetLobbySoundEffects(bool play);
	static void SetChatSoundEffects(bool play);
	static void SetStagingSoundEffects(bool play);
	static void SetLobbyMusic(bool play);
	static void SetShowConfirmations(bool show);
	static void SetAutoPingGames(bool ping);
	static void SetShowGamesInProgress(bool show);
	static void SetAnonymousToFriends(bool anonymous);
	static void SetMaxPings(unsigned short theMaxPings);
	static void SetLanBroadcastPort(unsigned short thePort);
	static void SetNetworkAdapter();

	
	// Storing
	static void ReadGlobal();
	static void WriteGlobal(bool force);
	static void ReadUser();
	static void WriteUser(bool force);
	static void RestoreUserDefaults();
};
typedef SmartPtr<LobbyPersistentData> LobbyPersistentDataPtr;
	
}; // namespace WONAPI

#endif