#include "LobbyPersistentData.h"

#include "LobbyFriend.h"
#include "LobbyResource.h"
#include "WONLobbyPrv.h"

#include "WONAuth/CDKey.h"
#include "WONCommon/ReadBuffer.h"
#include "WONCommon/StringParser.h"
#include "WONCommon/WONFile.h"
#include "WONCommon/WriteBuffer.h"

#include "WONCommon/CRC.h"

#include <time.h>

using namespace WONAPI;

static int GLOBALDATA_VERSION = 2;
static int USERDATA_VERSION = 4;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListFilterSet::insert(const GUIString &theStr)
{
	std::pair<FilterMap::iterator,bool> aRet = mFilterMap.insert(FilterMap::value_type(theStr,mFilterList.end()));
	if(aRet.second)
		aRet.first->second = mFilterList.insert(mFilterList.end(), theStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ServerListFilterSet::erase(const GUIString &theStr)
{
	FilterMap::iterator anItr = mFilterMap.find(theStr);
	if(anItr!=mFilterMap.end())
	{
		mFilterList.erase(anItr->second);
		mFilterMap.erase(anItr);
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ServerListFilterSet::clear()
{
	mFilterMap.clear();
	mFilterList.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyPersistentData::LobbyPersistentData()
{
	WriteBuffer aBuf;
	aBuf.Reserve(8);
	aBuf.AppendLong(0x22fa655f);
	aBuf.AppendLong(CDKey::GetMachineId());

	mEncryptKey.SetKey(aBuf.data(),aBuf.length());
	RestoreDefaultUserOptions(); // put default option settings here

	mLanBroadcastPort = 8585;
	mNeedWriteGlobal = false;
	mNeedWriteUser = false;
	mGlobalLobbySoundEffects = true;
	mGlobalLobbyMusic = true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString LobbyPersistentData::GenerateDefaultName()
{
	GUIString aName = LobbyOptions_DefaultName_String;
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig!=NULL && !aConfig->mAppendNumberToDefaultLanName)
		return aName;

	// Hash the users IP address
	long theHost = IPAddr::GetLocalAddr().GetHost();

	CRC16 aCRC;
	aCRC.Put(&theHost, sizeof(theHost));
	unsigned short aCRCValue = aCRC.Get() % 10000;

	// Convert to a string and append to aName
	char aString[255];
	itoa(aCRCValue,aString,10);
	aName.append(aString);

	return aName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RestoreDefaultUserOptions()
{
	mDefLobbyServer.erase();
	mDefLobbyGroup.erase();

	if(mCurFileId!=0) // internet
		mDefaultTabName = LobbyGlobal_QuickPlay_String;
	else // lan
	{
		mDefaultTabName = LobbyGlobal_Lan_String;
		mLanName = GenerateDefaultName();
		
	}
	
	mFilterDirtyWords = true;
	mDoAutoAFK = true;
	mShowCrossPromotion = true;
	mLobbySoundEffects = true;
	mLobbyMusic = true;
	mShowConfirmations = true;
	mChatSoundEffects = true;
	mStagingSoundEffects = true;
	mAutoPingGames = true;
	mShowGamesInProgress = true;
	mAnonymousToFriends = false;

	mLanBroadcastPort = 8585;
	mMaxPings = 25;
	mNetworkAdapter = INADDR_ANY; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyPersistentData* LobbyPersistentData::GetPersistentData()
{
	if(gLobbyData.get()!=NULL)
		return gLobbyData->mPersistentData;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyPersistentData::UserInfo* LobbyPersistentData::AddUserInfo(const GUIString &theUserName, const GUIString &thePassword, bool mustExistAlready)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return NULL;

	if(!mustExistAlready)
		aData->RemoveExtraUserNames();

	UserInfoMap::iterator anItr = aData->mUserInfoMap.insert(UserInfoMap::value_type(theUserName,UserInfo())).first;
	UserInfo &aStruct = anItr->second;

	if(mustExistAlready && aStruct.mLastUseTime==0)
	{
		aData->mUserInfoMap.erase(anItr);
		return NULL;
	}

	if(aStruct.mFileId==0) // new user
	{
		aStruct.mFileId = aData->GetFreeFileId();
		aData->mFileIdSet.insert(aStruct.mFileId);
		WONFile aFile(aData->GetUserFileName(aStruct.mFileId));
		aFile.Remove();
	}


	aStruct.mLastUseTime = time(NULL);
	aStruct.mUserName = theUserName;
	aStruct.mPassword = thePassword;
	aData->mNeedWriteGlobal = true;

	return &aStruct;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyPersistentData::UserInfo* LobbyPersistentData::GetCurUserInfo()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return NULL;

	if (aData->mCurFileId==0)
		return NULL;

	UserInfoMap::iterator anItr = aData->mUserInfoMap.begin();
	for (; anItr != aData->mUserInfoMap.end(); ++anItr)
	{
		UserInfo* anInfo = &anItr->second;
		if (anInfo->mFileId == aData->mCurFileId)
			return anInfo;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const LobbyPersistentData::UserInfoMap& LobbyPersistentData::GetUserInfoMap()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mUserInfoMap;
	else
	{
		static UserInfoMap anEmptyMap;
		return anEmptyMap;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString LobbyPersistentData::GetPassword(const GUIString &theUserName)
{
	UserInfo *anInfo = GetUserInfo(theUserName);
	if(anInfo!=NULL)
		return anInfo->mPassword;
	else
		return GUIString::EMPTY_STR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyPersistentData::UserInfo* LobbyPersistentData::GetUserInfo(const GUIString &theUserName)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return NULL;

	UserInfoMap::iterator anItr = aData->mUserInfoMap.find(theUserName);
	if(anItr==aData->mUserInfoMap.end())
		return NULL;
	else
		return &anItr->second;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetCurUser(UserInfo *theUser)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if(theUser==NULL)
		aData->mCurFileId = 0;
	else
		aData->mCurFileId = theUser->mFileId;

	aData->ReadUserData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const LobbyPersistentData::IgnoreNameSet& LobbyPersistentData::GetIgnoreNameSet()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mIgnoreNameSet;
	else
	{
		static IgnoreNameSet anEmptySet;
		return anEmptySet;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::IsIgnored(const GUIString &theName)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return false;

	IgnoreNameSet::iterator anItr = aData->mIgnoreNameSet.find(theName);
	return anItr!=aData->mIgnoreNameSet.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::IsBlocked(unsigned long theId)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return false;

	BlockedIdSet::iterator anItr = aData->mBlockedIdSet.find(theId);
	return (anItr != aData->mBlockedIdSet.end());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetBlocked(unsigned long theId, bool isBlocked)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	bool needWrite = false;
	if (isBlocked)
		needWrite = aData->mBlockedIdSet.insert(theId).second;
	else
		needWrite = aData->mBlockedIdSet.erase(theId)>0;

	aData->mNeedWriteUser = needWrite;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetIgnore(const GUIString &theName, bool isIgnored)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	bool needWrite = false;
	if(isIgnored)
		needWrite = aData->mIgnoreNameSet.insert(theName).second;
	else
		needWrite = aData->mIgnoreNameSet.erase(theName)>0;

	aData->mNeedWriteUser = needWrite;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const LobbyPersistentData::LobbyFriendMap& LobbyPersistentData::GetFriendMap()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mFriendMap;
	else
	{
		static LobbyFriendMap anEmptyMap;
		return anEmptyMap;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RemoveFriend(unsigned long theWONUserId)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	aData->mNeedWriteUser = aData->mFriendMap.erase(theWONUserId)>0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyFriend* LobbyPersistentData::GetFriend(unsigned long theWONUserId)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return NULL;

	LobbyFriendMap::iterator anItr = aData->mFriendMap.find(theWONUserId);
	if (anItr == aData->mFriendMap.end())
		return NULL;
	else
		return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyFriend* LobbyPersistentData::AddFriend(const GUIString& theClientName, unsigned long theWONUserId)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return NULL;

	LobbyFriendMap::iterator anItr = aData->mFriendMap.insert(LobbyFriendMap::value_type(theWONUserId,NULL)).first;
	if(anItr->second.get()==NULL)
	{
		anItr->second = new LobbyFriend(theClientName, theWONUserId);
		aData->mNeedWriteUser  = true;
	}

	return anItr->second.get();
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
const GUIString& LobbyPersistentData::GetLanName() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mLanName; 
	else
		return GUIString::EMPTY_STR;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
const LobbyPersistentData::LanNameInfoMap& LobbyPersistentData::GetLanNameInfoMap() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mLanNameInfoMap;
	else
	{
		static LanNameInfoMap anEmptyMap;
		return anEmptyMap;
	}
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetLanName(const GUIString &theName)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if(!theName.length() || theName==aData->mLanName)
		return;

	aData->mLanNameInfoMap[theName] = time(NULL);
	aData->mLanName = theName;

	aData->RemoveExtraLanNames();
	aData->mNeedWriteUser  = true;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RemoveLanName()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	LanNameInfoMap::iterator anItr = aData->mLanNameInfoMap.find(aData->mLanName);
	if(anItr != aData->mLanNameInfoMap.end())
	{
		aData->mLanNameInfoMap.erase(anItr);
		aData->SetLastUsedLanName();
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::AddDirectConnection(const IPAddr& theAddress, const GUIString& theDescription)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	aData->mDirectConnectMap[theAddress] = theDescription;	
	aData->mNeedWriteUser  = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RemoveDirectConnection(const IPAddr& theAddress)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	DirectConnectMap::iterator anItr = aData->mDirectConnectMap.find(theAddress);
	if (anItr != aData->mDirectConnectMap.end())
	{
		aData->mDirectConnectMap.erase(anItr);
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const LobbyPersistentData::DirectConnectMap& LobbyPersistentData::GetDirectConnectMap() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mDirectConnectMap; 
	else
	{
		static DirectConnectMap anEmptyMap;
		return anEmptyMap;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::AddReconnectId(const IPAddr &theAddr, int theReconnectId)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	ReconnectInfoMap::iterator anItr = aData->mReconnectInfoMap.insert(ReconnectInfoMap::value_type(theAddr,ReconnectInfo())).first;
	ReconnectInfo &anInfo = anItr->second;

	anInfo.mLastUseTime = time(NULL);
	anInfo.mReconnectId = theReconnectId;

	aData->WriteUserData(); // this is important enough to always write out... also it's usually only needed after a crash
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyPersistentData::GetReconnectId(const IPAddr &theAddr)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return 0;

	ReconnectInfoMap::iterator anItr = aData->mReconnectInfoMap.find(theAddr);
	if(anItr==aData->mReconnectInfoMap.end())
		return 0;
	else
		return anItr->second.mReconnectId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetDefLobbyRoom(const GUIString& theServer, const GUIString& theGroup)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	// Only write data if the field has changed
	if (theServer != aData->mDefLobbyServer || theGroup != aData->mDefLobbyGroup)
	{
		aData->mDefLobbyServer = theServer;
		aData->mDefLobbyGroup = theGroup;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& LobbyPersistentData::GetDefLobbyServer() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mDefLobbyServer; 
	else
		return GUIString::EMPTY_STR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& LobbyPersistentData::GetDefLobbyGroup() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mDefLobbyGroup; 
	else
		return GUIString::EMPTY_STR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ServerListFilterSet& LobbyPersistentData::GetServerListFilterSet(const GUIString &theColumnName)
{
	LobbyPersistentData *aData = GetPersistentData();
	ServerListFilterMap::iterator anItr;
	if(aData!=NULL)
		anItr = aData->mServerListFilterMap.find(theColumnName);

	if(aData!=NULL && anItr!=aData->mServerListFilterMap.end())
		return anItr->second;
	else
	{
		static ServerListFilterSet anEmptySet;
		return anEmptySet;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::AddServerListFilter(const GUIString &theColumnName, const GUIString &theFilterString)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
	{
		aData->mServerListFilterMap[theColumnName].insert(theFilterString);
		aData->mNeedWriteUser  = true;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RemoveServerListFilter(const GUIString &theColumnName, const GUIString &theFilterString)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
	{
		ServerListFilterSet &aSet = aData->mServerListFilterMap[theColumnName];
		if(aSet.erase(theFilterString))
			aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::ResetServerListFilters()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
	{
		aData->mServerListFilterMap = aData->mDefaultServerListFilterMap;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::AddDefaultServerListFilters(const GUIString &theColumn, const GUIString &theStrings, bool addIfColumnExists)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	ServerListFilterSet &aSet =  aData->mDefaultServerListFilterMap[theColumn];
	if(!aSet.empty())
	{
		if(!addIfColumnExists)
			return;
	}

	std::wstring aStrList = theStrings;
	StringParser aParser(aStrList.c_str());
	while(true)
	{
		std::wstring aFilter;
		aParser.ReadValue(aFilter);
		if(aFilter.empty())
			break;

		aSet.insert(aFilter);
		if(!aParser.CheckNextChar(','))
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ServerListFilterSet& LobbyPersistentData::GetDefaultServerListFilters(const GUIString &theColumn)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mDefaultServerListFilterMap[theColumn];
	else
	{
		static ServerListFilterSet anEmptySet;
		return anEmptySet;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& LobbyPersistentData::GetDefaultTabName() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mDefaultTabName; 
	else
		return GUIString::EMPTY_STR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetFilterDirtyWords() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mFilterDirtyWords; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetDoAutoAFK() 
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mDoAutoAFK; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetShowCrossPromotion() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mShowCrossPromotion; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetLobbySoundEffects() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mLobbySoundEffects; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetChatSoundEffects() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mChatSoundEffects; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetStagingSoundEffects() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mStagingSoundEffects; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetLobbyMusic() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mLobbyMusic; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetShowConfirmations() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mShowConfirmations; 
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetAutoPingGames()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mAutoPingGames;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetShowGamesInProgress()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mShowGamesInProgress;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyPersistentData::GetAnonymousToFriends()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mAnonymousToFriends;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short LobbyPersistentData::GetMaxPings() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mMaxPings; 
	else
		return 50;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short LobbyPersistentData::GetLanBroadcastPort() 
{ 
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mLanBroadcastPort; 
	else
		return 8585;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
long LobbyPersistentData::GetNetworkAdapter()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		return aData->mNetworkAdapter; 
	else
		return INADDR_ANY;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetLanBroadcastPort(unsigned short thePort)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if(thePort != aData->mLanBroadcastPort)
	{
		aData->mLanBroadcastPort = thePort;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetFilterDirtyWords(bool filter)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (filter != aData->mFilterDirtyWords)
	{
		aData->mFilterDirtyWords = filter;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetDoAutoAFK(bool doAutoAFK)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (doAutoAFK != aData->mDoAutoAFK)
	{
		aData->mDoAutoAFK = doAutoAFK;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetShowCrossPromotion(bool show)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (show != aData->mShowCrossPromotion)
	{
		aData->mShowCrossPromotion = show;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetDefaultTabName(const GUIString& theTabName)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (theTabName != aData->mDefaultTabName)
	{
		aData->mDefaultTabName = theTabName;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetLobbySoundEffects(bool play)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (play != aData->mLobbySoundEffects)
	{
		aData->mLobbySoundEffects = play;
		aData->mNeedWriteUser  = true;
		aData->mNeedWriteGlobal  = play!=aData->mGlobalLobbySoundEffects;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetChatSoundEffects(bool play)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (play != aData->mChatSoundEffects)
	{
		aData->mChatSoundEffects = play;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetStagingSoundEffects(bool play)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (play != aData->mStagingSoundEffects)
	{
		aData->mStagingSoundEffects = play;
		aData->mNeedWriteUser  = true;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetLobbyMusic(bool play)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (play != aData->mLobbyMusic)
	{
		aData->mLobbyMusic = play;
		aData->mNeedWriteUser  = true;
		aData->mNeedWriteGlobal  = play!=aData->mGlobalLobbyMusic;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetShowConfirmations(bool show)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (show != aData->mShowConfirmations)
	{
		aData->mShowConfirmations = show;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetAutoPingGames(bool ping)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (ping != aData->mAutoPingGames)
	{
		aData->mAutoPingGames = ping;
		aData->mNeedWriteUser  = true;
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetShowGamesInProgress(bool show)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (show != aData->mShowGamesInProgress)
	{
		aData->mShowGamesInProgress = show;
		aData->mNeedWriteUser  = true;
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetAnonymousToFriends(bool anonymous)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if (anonymous != aData->mAnonymousToFriends)
	{
		aData->mAnonymousToFriends = anonymous;
		aData->mNeedWriteUser  = true;
	}
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetMaxPings(unsigned short theMaxPings)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if(theMaxPings!=aData->mMaxPings)
	{
		aData->mMaxPings = theMaxPings;
		aData->mNeedWriteUser  = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetNetworkAdapter()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData==NULL)
		return;

	if(aData->mNetworkAdapter!=IPAddr::GetLocalHost())
	{
		aData->mNetworkAdapter = IPAddr::GetLocalHost();
		aData->mNeedWriteGlobal = true;
	}

}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::ReadGlobal()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		aData->ReadGlobalData();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteGlobal(bool force)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
	{
		if(force || aData->mNeedWriteGlobal)
			aData->WriteGlobalData();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::ReadUser()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
		aData->ReadUserData();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteUser(bool force)
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
	{
		if(force || aData->mNeedWriteUser)
			aData->WriteUserData();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RestoreUserDefaults()
{
	LobbyPersistentData *aData = GetPersistentData();
	if(aData!=NULL)
	{
		int anOldAdapter = aData->mNetworkAdapter;

		aData->RestoreDefaultUserOptions();
		aData->mNeedWriteUser = true;
		aData->mNeedWriteGlobal = 
			aData->mGlobalLobbySoundEffects!=aData->mLobbySoundEffects || 
			aData->mGlobalLobbyMusic!=aData->mLobbyMusic ||
			aData->mNetworkAdapter != anOldAdapter;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string LobbyPersistentData::GetUserFileName(int theFileId)
{
	char aBuf[50];
	sprintf(aBuf,"_wonuser%d.dat",theFileId);
	return aBuf;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyPersistentData::GetFreeFileId()
{
	if(mFileIdSet.empty())
		return 1;

	FileIdSet::iterator anItr = mFileIdSet.begin();
	int anId = *anItr;
	while(anItr!=mFileIdSet.end() && anId==*anItr)
	{
		++anId;
		++anItr;
	}

	return anId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RemoveExtraLanNames()
{
	LobbyPersistentData *aData = GetPersistentData();

	const int aMaxNames = 50;
	if(mLanNameInfoMap.size() <= aMaxNames)
		return;

	typedef std::multimap<time_t,LanNameInfoMap::iterator> SortMap;
	SortMap aSortMap;

	LanNameInfoMap::iterator aNameMapItr = mLanNameInfoMap.begin();
	while(aNameMapItr!=mLanNameInfoMap.end())
	{
		aSortMap.insert(SortMap::value_type(aNameMapItr->second, aNameMapItr));
		++aNameMapItr;
	}

	SortMap::iterator aSortItr = aSortMap.begin();
	while(aSortItr!=aSortMap.end() && mLanNameInfoMap.size() > aMaxNames)
	{
		mLanNameInfoMap.erase(aSortItr->second);
		aSortItr++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::SetLastUsedLanName()
{
	LobbyPersistentData *aData = GetPersistentData();

	mLanName = GenerateDefaultName();

	LanNameInfoMap::iterator anItr;
	time_t aMaxTime = 0;
	for(anItr = mLanNameInfoMap.begin(); anItr != mLanNameInfoMap.end(); ++anItr)
	{
		if(aMaxTime==0 || anItr->second > aMaxTime)
		{
			mLanName = anItr->first;
			aMaxTime = anItr->second;
		}
	}

}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::RemoveExtraUserNames()
{
	const int aMaxNames = 50;
	if(mUserInfoMap.size() <= aMaxNames)
		return;

	typedef std::multimap<time_t,UserInfoMap::iterator> SortMap;
	SortMap aSortMap;

	UserInfoMap::iterator aNameMapItr = mUserInfoMap.begin();
	while(aNameMapItr!=mUserInfoMap.end())
	{
		if(aNameMapItr->second.mFileId==0)
			mUserInfoMap.erase(aNameMapItr++);
		else
		{
			aSortMap.insert(SortMap::value_type(aNameMapItr->second.mLastUseTime, aNameMapItr));
			++aNameMapItr;
		}
	}

	SortMap::iterator aSortItr = aSortMap.begin();
	while(aSortItr!=aSortMap.end() && mUserInfoMap.size() > aMaxNames)
	{
		UserInfo &aStruct = (aSortItr->second->second);
		WONFile aFile(GetUserFileName(aStruct.mFileId));
		aFile.Remove();
		mFileIdSet.erase(aStruct.mFileId);

		mUserInfoMap.erase(aSortItr->second);
		aSortItr++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString LobbyPersistentData::ReadAsciiString(FileReader &theReader)
{
	unsigned short aLength = theReader .ReadShort();
	GUIString aStr(aLength);
	for(int i=0; i<aLength; i++)
		aStr.append(theReader.ReadByte());

	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString LobbyPersistentData::ReadWideString(FileReader &theReader)
{
	unsigned short aLength = theReader.ReadShort();
	GUIString aStr(aLength);
	for(int i=0; i<aLength; i++)
		aStr.append(theReader.ReadShort());

	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString LobbyPersistentData::ReadEncryptedWideString(FileReader &theReader)
{
	unsigned short aLength = theReader.ReadShort();
	WriteBuffer aBuf;
	aBuf.Reserve(aLength);
	theReader.ReadBytes(aBuf.data(),aLength);

	ByteBufferPtr aDecrypt = mEncryptKey.Decrypt(aBuf.data(),aLength);
	if(aDecrypt.get()==NULL)
		return "";

	ReadBuffer aReadBuf(aDecrypt->data(), aDecrypt->length());
	try
	{
		std::wstring aWStr;
		aReadBuf.ReadWString(aWStr);
		return aWStr;
	}
	catch(ReadBufferException&)
	{
	}

	return "";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteAsciiString(FileWriter &theWriter, const GUIString &theString)
{
	std::string aStr = theString;
	theWriter.WriteShort(aStr.length());
	theWriter.WriteBytes(aStr.data(),aStr.length());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteWideString(FileWriter &theWriter, const GUIString &theString)
{
	theWriter.WriteShort(theString.length());
	for(int i=0; i<theString.length(); i++)
		theWriter.WriteShort(theString.at(i));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteEncryptedWideString(FileWriter &theWriter, const GUIString &theStr)
{
	WriteBuffer aBuf;
	std::wstring aStr = theStr;
	aBuf.AppendWString(aStr);

	ByteBufferPtr anEncrypt = mEncryptKey.Encrypt(aBuf.data(), aBuf.length());
	theWriter.WriteShort(anEncrypt->length());
	theWriter.WriteBytes(anEncrypt->data(), anEncrypt->length());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyPersistentData::ReadFileSignature(FileReader &theReader)
{
	try
	{
		char aSig[7];
		aSig[6] = '\0';
		theReader.ReadBytes(aSig,6);
		if(strcmp(aSig,"WONPER")!=0)
			return 0;

		return theReader.ReadLong();
	}
	catch(FileReaderException&)
	{
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteFileSignature(FileWriter &theWriter, int theVersion)
{
	try
	{
		theWriter.WriteBytes("WONPER",6);
		theWriter.WriteLong(theVersion);
	}
	catch(FileWriterException&)
	{
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::ReadGlobalData()
{
	FileReader aReader;
	if(!aReader.Open("_wonlobbypersistent.dat"))
		return;

	mUserInfoMap.clear();
	mNetworkAdapter = INADDR_ANY;

	int i;

	try
	{
		if(ReadFileSignature(aReader)!=GLOBALDATA_VERSION)
			return;

		unsigned short aNumUserNames = aReader.ReadShort();
		for(i=0; i<aNumUserNames; i++)
		{	
			GUIString aUserName = ReadWideString(aReader);

			UserInfoMap::iterator aUserItr = mUserInfoMap.insert(UserInfoMap::value_type(aUserName,UserInfo())).first;
			UserInfo &aStruct = aUserItr->second;

			aStruct.mUserName = aUserName;
			aStruct.mLastUseTime = aReader.ReadLong();
			aStruct.mFileId = aReader.ReadShort();
			aStruct.mSysTOUTime = aReader.ReadLong();
			aStruct.mGameTOUTime = aReader.ReadLong();
			aStruct.mPassword = ReadEncryptedWideString(aReader);

			mFileIdSet.insert(aStruct.mFileId);
		}

		mLobbySoundEffects = mGlobalLobbySoundEffects = aReader.ReadByte()!=0;
		LobbyGlobal_ButtonClick_Sound->SetMuted(!mLobbySoundEffects); // need this for login screen so login screen uses last value

		mLobbyMusic = mGlobalLobbyMusic = aReader.ReadByte()!=0;
		mNetworkAdapter = aReader.ReadLong();
	}
	catch(FileReaderException&)
	{
	}

	mNeedWriteGlobal = false;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteGlobalData()
{
	FileWriter aWriter;
	if(!aWriter.Open("_wonlobbypersistent.dat"))
		return;

	try
	{
		WriteFileSignature(aWriter,GLOBALDATA_VERSION);

		aWriter.WriteShort(mUserInfoMap.size());
		UserInfoMap::iterator aUserItr = mUserInfoMap.begin();
		while(aUserItr!=mUserInfoMap.end())
		{
			UserInfo &aStruct = aUserItr->second;

			WriteWideString(aWriter,aStruct.mUserName);
			aWriter.WriteLong(aStruct.mLastUseTime);
			aWriter.WriteShort(aStruct.mFileId);
			aWriter.WriteLong(aStruct.mSysTOUTime);
			aWriter.WriteLong(aStruct.mGameTOUTime);
			WriteEncryptedWideString(aWriter,aStruct.mPassword);

			++aUserItr;
		}

		aWriter.WriteByte(mLobbySoundEffects?1:0);
		aWriter.WriteByte(mLobbyMusic?1:0);
		aWriter.WriteLong(mNetworkAdapter);
	}
	catch(FileWriterException&)
	{
	}

	mGlobalLobbySoundEffects = mLobbySoundEffects;
	mGlobalLobbyMusic = mLobbyMusic;

	mNeedWriteGlobal = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::ReadUserData()
{
	RestoreDefaultUserOptions(); // restores default settings and such in case user doesn't have settings yet
	mReconnectInfoMap.clear();
	mIgnoreNameSet.clear();
	mFriendMap.clear();
	mLanNameInfoMap.clear();
	mDirectConnectMap.clear();
	mServerListFilterMap.clear();
	bool gotFilterEntires = false;

	FileReader aReader;
	std::string aFileName = GetUserFileName(mCurFileId);
	if(!aReader.Open(aFileName.c_str()))
	{
		mServerListFilterMap = mDefaultServerListFilterMap;
		return;
	}

	int i;

	try
	{
		if(ReadFileSignature(aReader)!=USERDATA_VERSION)
			return;

		if(mCurFileId==0)
			ReadLanData(aReader);

		unsigned short aNumReconnectIds = aReader.ReadShort();
		for(i=0; i<aNumReconnectIds; i++)
		{
			char aBuf[6];
			aReader.ReadBytes(aBuf,6);
			IPAddr anAddr;
			anAddr.SetSixByte(aBuf);

			ReconnectInfoMap::iterator anItr = mReconnectInfoMap.insert(ReconnectInfoMap::value_type(anAddr,ReconnectInfo())).first;
			anItr->second.mLastUseTime = aReader.ReadLong();
			anItr->second.mReconnectId = aReader.ReadLong();
		}

		unsigned short aNumIgnored = aReader.ReadShort();
		for(i=0; i<aNumIgnored; i++)
			mIgnoreNameSet.insert(ReadWideString(aReader));

		unsigned short aNumFriend = aReader.ReadShort();
		for (i=0; i<aNumFriend; i++)
		{
			std::wstring aName = ReadWideString(aReader);
			unsigned long anId = aReader.ReadLong();

			LobbyFriendPtr aFriend = new LobbyFriend(aName, anId);
			mFriendMap.insert(LobbyFriendMap::value_type(anId,aFriend));
		}

		unsigned short aNumBlocked = aReader.ReadShort();
		for (i=0; i<aNumBlocked; i++)
			mBlockedIdSet.insert(aReader.ReadLong());

		mFilterDirtyWords		= (aReader.ReadByte() == 1) ? true : false;
		mDoAutoAFK				= (aReader.ReadByte() == 1) ? true : false;
		mShowCrossPromotion		= (aReader.ReadByte() == 1) ? true : false;
		mAutoPingGames		= (aReader.ReadByte() == 1) ? true : false;
		mAnonymousToFriends		= (aReader.ReadByte() == 1) ? true : false;
		mShowGamesInProgress	= (aReader.ReadByte() == 1) ? true : false;

		mDefaultTabName = ReadWideString(aReader);
		mDefLobbyServer = ReadWideString(aReader);
		mDefLobbyGroup = ReadWideString(aReader);

		mLobbySoundEffects = (aReader.ReadByte() == 1) ? true : false;
		mLobbyMusic = (aReader.ReadByte() == 1) ? true : false;
		mChatSoundEffects = (aReader.ReadByte() == 1) ? true : false;
		mStagingSoundEffects = (aReader.ReadByte() == 1) ? true : false;
		mShowConfirmations = (aReader.ReadByte() == 1) ? true : false;
		mMaxPings = aReader.ReadShort();
	
		// Read ServerList filters
		unsigned short aNumFilter = aReader.ReadShort();
		gotFilterEntires = true;
		for(i=0; i<aNumFilter; i++)
		{
			GUIString aColumnName = ReadAsciiString(aReader);
			ServerListFilterSet &aSet = mServerListFilterMap[aColumnName];
			unsigned short aNumStrings = aReader.ReadShort();
			for(int j=0; j<aNumStrings; j++)
				aSet.insert(ReadWideString(aReader));
		}
	}
	catch(FileReaderException&)
	{
	}

	if(!gotFilterEntires)
		mServerListFilterMap = mDefaultServerListFilterMap;

	mNeedWriteGlobal = mLobbySoundEffects!=mGlobalLobbySoundEffects || mGlobalLobbyMusic!=mLobbyMusic;
	mNeedWriteUser = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteUserData()
{
	FileWriter aWriter;
	std::string aFileName = GetUserFileName(mCurFileId);
	if(!aWriter.Open(aFileName.c_str()))
		return;

	try
	{
		WriteFileSignature(aWriter,USERDATA_VERSION);

		if(mCurFileId==0) // lan options
			WriteLanData(aWriter);

		aWriter.WriteShort(mReconnectInfoMap.size());
		ReconnectInfoMap::iterator aReconnectItr = mReconnectInfoMap.begin();
		while(aReconnectItr!=mReconnectInfoMap.end())
		{
			ReconnectInfo &anInfo = aReconnectItr->second;
			aWriter.WriteBytes(aReconnectItr->first.GetSixByte(),6);
			aWriter.WriteLong(anInfo.mLastUseTime);
			aWriter.WriteLong(anInfo.mReconnectId);

			++aReconnectItr;
		}

		aWriter.WriteShort(mIgnoreNameSet.size());
		IgnoreNameSet::iterator anIgnoreItr = mIgnoreNameSet.begin();
		while(anIgnoreItr!=mIgnoreNameSet.end())
		{
			WriteWideString(aWriter,*anIgnoreItr);
			++anIgnoreItr;
		}

		aWriter.WriteShort(mFriendMap.size());
		LobbyFriendMap::iterator aFriendItr = mFriendMap.begin();
		while(aFriendItr!=mFriendMap.end())
		{
			WriteWideString(aWriter,aFriendItr->second->GetName());
			aWriter.WriteLong(aFriendItr->first);
			++aFriendItr;
		}

		aWriter.WriteShort(mBlockedIdSet.size());
		BlockedIdSet::iterator aBlockedIdItr = mBlockedIdSet.begin();
		while(aBlockedIdItr!=mBlockedIdSet.end())
		{
			aWriter.WriteLong(*aBlockedIdItr);
			++aBlockedIdItr;
		}

		aWriter.WriteByte(mFilterDirtyWords ? 1 : 0);
		aWriter.WriteByte(mDoAutoAFK ? 1 : 0);
		aWriter.WriteByte(mShowCrossPromotion ? 1 : 0);
		aWriter.WriteByte(mAutoPingGames ? 1: 0);
		aWriter.WriteByte(mAnonymousToFriends ? 1: 0);
		aWriter.WriteByte(mShowGamesInProgress ? 1: 0);
		WriteWideString(aWriter,mDefaultTabName);
		WriteWideString(aWriter,mDefLobbyServer);
		WriteWideString(aWriter,mDefLobbyGroup);

		aWriter.WriteByte(mLobbySoundEffects ? 1 : 0);
		aWriter.WriteByte(mLobbyMusic ? 1 : 0);
		aWriter.WriteByte(mChatSoundEffects ? 1: 0);
		aWriter.WriteByte(mStagingSoundEffects  ? 1: 0);
		aWriter.WriteByte(mShowConfirmations ? 1 : 0);
		aWriter.WriteShort(mMaxPings);

		// Write ServerList filters
		aWriter.WriteShort(mServerListFilterMap.size());
		ServerListFilterMap::iterator aFilterMapItr = mServerListFilterMap.begin();
		while(aFilterMapItr!=mServerListFilterMap.end())
		{
			WriteAsciiString(aWriter,aFilterMapItr->first);
			ServerListFilterSet &aSet = aFilterMapItr->second;
			if(!aSet.empty())
			{
				aWriter.WriteShort(aSet.size());
				ServerListFilterSet::iterator aFilterSetItr = aSet.begin();
				while(aFilterSetItr!=aSet.end())
				{
					WriteWideString(aWriter,*aFilterSetItr);
					++aFilterSetItr;
				}
			}

			++aFilterMapItr;
		}
	}
	catch(FileWriterException&)
	{
	}

	mNeedWriteUser = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::ReadLanData(FileReader &theReader)
{
	// Note: This is only called from ReadUserData
	FileReader &aReader = theReader;

	unsigned short aNumUserNames = aReader.ReadShort();
	int i;

	for(i=0; i<aNumUserNames; i++)
	{	
		GUIString aName = ReadWideString(aReader);
		time_t aTime = aReader.ReadLong();
		mLanNameInfoMap[aName] = aTime;
	}
	SetLastUsedLanName();

	unsigned short aNumConnections = aReader.ReadShort();
	for(i=0; i<aNumConnections; i++)
	{
		IPAddr anAddress;
		char aBuf[6];
		aReader.ReadBytes(aBuf,6);
		anAddress.SetSixByte(aBuf);
		GUIString aDescription = ReadWideString(aReader);

		mDirectConnectMap[anAddress] = aDescription;
	}

	mLanBroadcastPort = theReader.ReadShort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPersistentData::WriteLanData(FileWriter &theWriter)
{
	FileWriter &aWriter = theWriter;

	theWriter.WriteShort(mLanNameInfoMap.size());
	LanNameInfoMap::iterator anItr = mLanNameInfoMap.begin();
	while(anItr!=mLanNameInfoMap.end())
	{
		WriteWideString(theWriter,anItr->first);
		theWriter.WriteLong(anItr->second);

		++anItr;
	}

	theWriter.WriteShort(mDirectConnectMap.size());
	DirectConnectMap::iterator aConnectItr = mDirectConnectMap.begin();
	while(aConnectItr!=mDirectConnectMap.end())
	{
		theWriter.WriteBytes(aConnectItr->first.GetSixByte(),6);
		WriteWideString(theWriter,aConnectItr->second);
		++aConnectItr;
	}

	theWriter.WriteShort(mLanBroadcastPort);
}