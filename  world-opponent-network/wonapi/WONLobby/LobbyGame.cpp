#include "LobbyGame.h"
#include "LobbyConfig.h"
#include "LobbyClient.h"
#include "LobbyGameMsg.h"
#include "LobbyGroup.h"
#include "LobbyMisc.h"
#include "LobbyPlayer.h"
#include "LobbyResource.h"
#include "LobbyStaging.h"
#include "LobbyTypes.h"

#include "WONCommon/ReadBuffer.h"
#include "WONCommon/WriteBuffer.h"

#include <time.h>

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame::LobbyGameFactory LobbyGame::mGameFactory = LobbyGame::DefaultGameFactory;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame::LobbyGame() 
{
	mGameType = LobbyGameType_None;
	mInviteOnly = false;
	mAskToJoin = false;
	mInProgress = false;
	mHasPassword = false;
	mIAmInvited = false;
	mIAmCaptain = false;
	mDetailsVisible = false;
	mHasDetails = false;
	mHadSuccessfulPing = false;

	mSkillLevel = LobbySkillLevel_None;
	mGroupId = LobbyServerId_Invalid;
	mCaptainId = LobbyServerId_Invalid;

	mNumPlayers = 0;
	mMaxPlayers = 0;

	mPing = LobbyPing_None;
	mLastHeartbeat = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame::~LobbyGame()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::CopyFrom(LobbyGame *theGame)
{
	mName = theGame->mName;
	mPassword = theGame->mPassword;
	mHasPassword = theGame->HasPassword();
	mInviteOnly = theGame->mInviteOnly;
	mAskToJoin = theGame->mAskToJoin;
	mInProgress = theGame->mInProgress;
	mIAmInvited = theGame->mIAmInvited;
	mIAmCaptain = theGame->mIAmCaptain;
	mDetailsVisible = theGame->mDetailsVisible;
	mHasDetails = theGame->mHasDetails;
	mHadSuccessfulPing = theGame->mHadSuccessfulPing;
	mLastHeartbeat = theGame->mLastHeartbeat;

	mNumPlayers = theGame->mNumPlayers;
	mMaxPlayers = theGame->mMaxPlayers;

	mGameType = theGame->mGameType;
	mSkillLevel = theGame->mSkillLevel;
	mClientList = theGame->mClientList;

	mIPAddr = theGame->mIPAddr;
	mCaptainId = theGame->mCaptainId;
	mGroupId = theGame->mGroupId;
	mPing = theGame->mPing;

	CopyFromHook(theGame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* LobbyGame::CreateGame(LobbyGameType theType)
{
	LobbyGame *aGame = mGameFactory();
	aGame->SetGameType(theType);
	return aGame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::SetClientList(LobbyClientList *theList)
{
	mClientList = theList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* LobbyGame::GetClient(unsigned short theId)
{
	if(mClientList.get()!=NULL)
		return mClientList->GetClient(theId);
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::DeletePlayers()
{
	if(mClientList.get()==NULL)
		return;

	const LobbyClientMap &aMap = mClientList->GetClientMap();
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
		anItr->second->SetPlayer(NULL);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::IsSameGame(LobbyGame *theGame)
{
	if(theGame->mGameType!=mGameType)
		return false;

	if(mGameType==LobbyGameType_Internet)
		return theGame->mGroupId==mGroupId;
	else
		return theGame->mIPAddr==mIPAddr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::CheckRoomSpecFlags(int theFlags)
{
	if(theFlags&LobbyRoomSpecFlag_Internet)
	{
		if(mGameType==LobbyGameType_Internet)
			return true;
	}
	else if(theFlags&LobbyRoomSpecFlag_Lan)
	{
		if(mGameType==LobbyGameType_Lan || mGameType==LobbyGameType_DirectConnect)
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::IsPinging()
{
	return mPing==LobbyPing_Pinging || mPing==LobbyPing_PingingDetails;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::IsPingingDetails()
{
	return mPing==LobbyPing_PingingDetails;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::PreNetJoinGame()
{
	return PreNetJoinGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::PostNetJoinGame()
{
	return PostNetJoinGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::PreNetCreateGame()
{
	return PreNetCreateGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::PostNetCreateGame()
{
	return PostNetCreateGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::PreNetLeaveGame()
{
	PreNetLeaveGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::PostNetLeaveGame()
{
	PostNetLeaveGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::InitFromLobbyGroup(LobbyGroup *theGroup)
{
	mName = theGroup->GetName();
	mHasPassword = theGroup->IsPasswordProtected();
	mInviteOnly = theGroup->IsInviteOnly();
	mAskToJoin = theGroup->IsAskToJoin();
	mGroupId = theGroup->GetGroupId();
	mCaptainId = theGroup->GetCaptainId();
	mClientList = theGroup->GetClientList();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::WriteSummary(WriteBuffer &theMsg)
{
	if(mGameType==LobbyGameType_Internet)
		theMsg.AppendBytes(mIPAddr.GetSixByte(),6);
	else
		theMsg.AppendShort(LobbyMisc::GetLanProductId());
	
	theMsg.AppendBool(mInProgress);
	if(mGameType!=LobbyGameType_Internet)
		theMsg.AppendWString(mName);

	theMsg.AppendByte(mSkillLevel);
	if(mGameType!=LobbyGameType_Internet)
	{
		unsigned char aProtectionFlags = 0;
		if(!mPassword.empty()) aProtectionFlags |= 0x01;
		if(mInviteOnly) aProtectionFlags |= 0x02;
		if(mAskToJoin) aProtectionFlags |= 0x04;
		theMsg.AppendByte(aProtectionFlags);
	}

	theMsg.AppendShort(mNumPlayers);
	theMsg.AppendShort(mMaxPlayers);

	WriteSummaryHook(theMsg);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::ReadSummary(ReadBuffer &theMsg)
{
	try
	{
		if(mGameType==LobbyGameType_Internet)
			mIPAddr.SetSixByte(theMsg.ReadBytes(6));
		else
		{
			unsigned short aLanProductId = theMsg.ReadShort();
			if(aLanProductId!=LobbyMisc::GetLanProductId())
				return false;
		}
			
		mInProgress = theMsg.ReadBool();
		if(mGameType!=LobbyGameType_Internet)
		{
			std::wstring aName;
			theMsg.ReadWString(aName);
			mName = aName;
		}

		mSkillLevel = (LobbySkillLevel)theMsg.ReadByte();
		if(mSkillLevel<LobbySkillLevel_None || mSkillLevel>=LobbySkillLevel_Max)
			mSkillLevel = LobbySkillLevel_None;

		if(mGameType!=LobbyGameType_Internet)
		{
			unsigned char aProtectionFlags = theMsg.ReadByte();
			mHasPassword = (aProtectionFlags & 0x01)?true:false;
			mInviteOnly = (aProtectionFlags & 0x02)?true:false;
			mAskToJoin = (aProtectionFlags & 0x04)?true:false;
		}

		mNumPlayers = theMsg.ReadShort();
		mMaxPlayers = theMsg.ReadShort();

		return ReadSummaryHook(theMsg);
	}
	catch(ReadBufferException&)
	{
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::ReadSummary(const ByteBuffer *theData)
{
	if(theData==NULL)
		return false;
	
	ReadBuffer aMsg(theData->data(),theData->length());
	return ReadSummary(aMsg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::WriteDetails(WriteBuffer &theMsg)
{
	WriteDetailsHook(theMsg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::ReadDetails(ReadBuffer &theMsg)
{
	try
	{
		if(!ReadDetailsHook(theMsg))
			return false;
	}
	catch(ReadBufferException&)
	{
		return false;
	}

	mHasDetails = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ByteBufferPtr LobbyGame::GetJoinGameRequest()
{
	WriteBuffer aBuf;
	aBuf.AppendByte(LobbyGameMsg_JoinRequest);
	aBuf.AppendShort(mPing);
	GetJoinGameRequestHook(aBuf);
	return aBuf.ToByteBuffer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::HandleHostGame(LobbyClient *myClient)
{
	LobbyPlayerPtr aPlayer = LobbyPlayer::CreatePlayer();
	myClient->SetPlayer(aPlayer);
	HandleHostGameHook(myClient);

	mNumPlayers = 1;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::IsEveryoneReady()
{
	const LobbyClientMap &aMap = mClientList->GetClientMap();
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
	{
		LobbyClient *aClient = anItr->second;
		if(aClient->IsPlayer() && !aClient->IsCaptain(true) && !aClient->IsPlayerReady())
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::CanStartGame()
{
	if(!IsEveryoneReady())
		return false;

	return CanStartGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::CanReady()
{
	return CanReadyHook();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::TryStartGame()
{
	return TryStartGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::NotifyStartGame()
{
	NotifyStartGameHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::HandlePlayerJoined(LobbyClient *theClient)
{
	HandlePlayerJoinedHook(theClient);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::HandlePlayerLeft(LobbyClient *theClient)
{
	LobbyPlayerPtr aPlayer = theClient->GetPlayer();
	if(aPlayer.get()==NULL)
		return;

	mNumPlayers--;
	theClient->SetPlayer(NULL);

	HandlePlayerLeftHook(theClient,aPlayer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
short LobbyGame::HandleJoinGameRequest(ReadBuffer &theMsg, LobbyClient *theClient, WriteBuffer &theReply)
{
	LobbyPlayerPtr aPlayer;

	if(IsGameFull())
		return LobbyGameStatus_GameFull;
	else if(mInProgress)
		return LobbyGameStatus_GameInProgress;

	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig!=NULL && !aConfig->mAllowDuplicateNames)
	{
		// check if the name is already used
		LobbyClientMap::const_iterator aClientItr = mClientList->GetClientMap().begin();
		for(; aClientItr != mClientList->GetClientMap().end(); ++aClientItr)
		{
			if (aClientItr->second->GetName() == theClient->GetName() && aClientItr->second.get() != theClient)
				return LobbyGameStatus_DuplicateName;
		}
	}
	


	aPlayer = LobbyPlayer::CreatePlayer();
	theClient->SetPlayer(aPlayer);

	try
	{
		unsigned short aPing = theMsg.ReadShort();	
		aPlayer->SetPing(aPing);

		short aStatus = HandleJoinGameRequestHook(theMsg, theClient);
		if(aStatus!=LobbyGameStatus_Success)
		{
			theClient->SetPlayer(NULL);
			return aStatus;
		}
	}
	catch(ReadBufferException&)
	{
		theClient->SetPlayer(NULL);
		return LobbyGameStatus_UnpackFailure;
	}


	// Send PlayerList back
	mNumPlayers++;	
	theReply.AppendShort(mNumPlayers); 

	LobbyPlayerList aList(mClientList);	
	while(aList.HasMorePlayers())
	{
		LobbyClient *aClient;
		LobbyPlayer *aPlayer = aList.GetNextPlayer(&aClient);
		if(aPlayer!=NULL)
		{
			theReply.AppendShort(aClient->GetClientId());
			aPlayer->WriteData(theReply);
		}
	}
	
	GetJoinGameReplyHook(theClient,theReply);
	return LobbyGameStatus_Success;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ByteBufferPtr LobbyGame::HandleJoinGameRequest(ReadBuffer &theMsg, LobbyClient *theClient)
{
	WriteBuffer aBuf;
	aBuf.AppendByte(LobbyGameMsg_JoinReply);
	aBuf.AppendShort(0); // reserve space for status

	short aStatus  = HandleJoinGameRequest(theMsg,theClient,aBuf);
	aBuf.SetShort(1,aStatus);

	return aBuf.ToByteBuffer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
short LobbyGame::HandleJoinGameReply(ReadBuffer &theMsg)
{
	short aStatus = LobbyGameStatus_None;
	try
	{
		aStatus = theMsg.ReadShort();

		if(aStatus!=LobbyGameStatus_Success)
			return aStatus;

		unsigned short aNumPlayers = theMsg.ReadShort();
		for(int i=0; i<aNumPlayers; i++)
		{
			unsigned short aClientId = theMsg.ReadShort();
			LobbyClient *aClient = GetClient(aClientId);
			if(aClient!=NULL)
			{
				LobbyPlayerPtr aPlayer = LobbyPlayer::CreatePlayer();
				if(aPlayer->ReadData(theMsg))
					aClient->SetPlayer(aPlayer);
			}
		}

		HandleJoinGameReplyHook(theMsg);
	}
	catch(ReadBufferException&)
	{
		aStatus = LobbyGameStatus_UnpackFailure;
	}

	return aStatus;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGame::HandleGameMessage(unsigned char theMsgType, ReadBuffer &theMsg, LobbyClient *theSender)
{
	HandleGameMessageHook(theMsgType,theMsg,theSender);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::IsGameFull()
{
	return mMaxPlayers>0 && mNumPlayers>=mMaxPlayers;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGame::IsOkToJoin()
{
	return GetOkToJoinStatus()==LobbyGameStatus_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
short LobbyGame::GetOkToJoinStatus()
{
	if(IsGameFull())
		return LobbyGameStatus_GameFull;

	if(mInviteOnly && !IAmInvited())
		return LobbyGameStatus_NotInvited;

	if(mInProgress) 
		return LobbyGameStatus_GameInProgress;

	return GetOkToJoinStatusHook();
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
short LobbyGame::GetOkToJoinStatusHook()
{
	return LobbyGameStatus_Success;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString LobbyGame::GetStatusString(short theStatus)
{
	switch(theStatus)
	{
		case LobbyGameStatus_Success: return LobbyGameStatus_Success_String;
		case LobbyGameStatus_GameFull: return LobbyGameStatus_GameFull_String;
		case LobbyGameStatus_GameInProgress: return LobbyGameStatus_GameInProgress_String;
		case LobbyGameStatus_UnpackFailure: return LobbyGameStatus_UnpackFailure_String;
		case LobbyGameStatus_NotInvited: return LobbyGameStatus_NotInvited_String;
		case LobbyGameStatus_CaptainRejectedYou: return LobbyGameStatus_CaptainRejectedYou_String;
		case LobbyGameStatus_InvalidPassword: return LobbyGameStatus_InvalidPassword_String;
		case LobbyGameStatus_DuplicateName: return LobbyGameStatus_DuplicateName_String;
	}

	GUIString aString = GetStatusStringHook(theStatus);
	if(!aString.empty())
		return aString;

	char aBuf[50];
	sprintf(aBuf,"%d",theStatus);
	return aBuf;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGameList::LobbyGameList(LobbyGameType theType)
{
	mGameType = theType; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGameList::~LobbyGameList()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGameList::Clear()
{
	mIdMap.clear();
	mAddrMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGameList::AddGame(LobbyGame *theGame)
{
	theGame->SetGameType(mGameType);
	if(mGameType==LobbyGameType_Internet)
		mIdMap[theGame->GetGroupId()] = theGame;
	else
		mAddrMap[theGame->GetIPAddr()] = theGame;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* LobbyGameList::AddGameById(unsigned short theId, bool *isModify)
{
	IdGameMap::iterator anItr = mIdMap.insert(IdGameMap::value_type(theId,NULL)).first;
	if(anItr->second.get()==NULL)
	{
		anItr->second = LobbyGame::CreateGame(mGameType);
		anItr->second->SetGroupId(theId);
		if(isModify!=NULL)
			*isModify = false;
	}
	else if(isModify!=NULL)
		*isModify = true;

	return anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* LobbyGameList::AddGameByAddr(const IPAddr &theAddr, bool *isModify)
{
	AddrGameMap::iterator anItr = mAddrMap.insert(AddrGameMap::value_type(theAddr,NULL)).first;
	if(anItr->second.get()==NULL)
	{
		anItr->second = LobbyGame::CreateGame(mGameType);
		anItr->second->SetIPAddr(theAddr);
		if(isModify!=NULL)
			*isModify = false;
	}
	else if(isModify!=NULL)
		*isModify = true;

	return anItr->second;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* LobbyGameList::GetGameById(unsigned short theId)
{
	IdGameMap::iterator anItr = mIdMap.find(theId);
	if(anItr!=mIdMap.end())
		return anItr->second;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* LobbyGameList::GetGameByAddr(const IPAddr &theAddr)
{
	AddrGameMap::iterator anItr = mAddrMap.find(theAddr);
	if(anItr!=mAddrMap.end())
		return anItr->second;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGamePtr LobbyGameList::RemoveGameById(unsigned short theId)
{
	IdGameMap::iterator anItr = mIdMap.find(theId);
	if(anItr==mIdMap.end())
		return NULL;


	LobbyGamePtr aGame = anItr->second;
	aGame->SetPing(LobbyPing_None); // mark as not needing a ping anymore
	mIdMap.erase(anItr);
	return aGame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGamePtr LobbyGameList::RemoveGameByAddr(const IPAddr &theAddr)
{
	AddrGameMap::iterator anItr = mAddrMap.find(theAddr);
	if(anItr==mAddrMap.end())
		return NULL;

	LobbyGamePtr aGame = anItr->second;
	aGame->SetPing(LobbyPing_None); // mark as not needing a ping anymore
	mAddrMap.erase(anItr);
	return aGame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyGameList::Rewind()
{
	if(mGameType==LobbyGameType_Internet)
		mIdItr = mIdMap.begin();
	else
		mAddrItr = mAddrMap.begin();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGameList::HasMoreGames()
{
	if(mGameType==LobbyGameType_Internet)
		return mIdItr!=mIdMap.end();
	else
		return mAddrItr!=mAddrMap.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* LobbyGameList::NextGame()
{
	if(mGameType==LobbyGameType_Internet)
		return (mIdItr++)->second;
	else
		return (mAddrItr++)->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGameList::IsEmpty()
{
	if(mGameType==LobbyGameType_Internet)
		return mIdMap.empty();
	else
		return mAddrMap.empty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyGameList::PurgeGamesWithNoHeartbeat(int theNumSeconds)
{
	if(mGameType==LobbyGameType_Internet)
		return false;

	time_t aTime = time(NULL);

	int oldSize = mAddrMap.size();

	AddrGameMap::iterator anItr = mAddrMap.begin();
	bool hadDelete = false;
	while(anItr!=mAddrMap.end())
	{
		LobbyGame *aGame = anItr->second;
		if(aTime - aGame->GetLastHeartbeat() > theNumSeconds)
		{
			aGame->SetPing(LobbyPing_None); // mark as not needing a ping anymore
			mAddrMap.erase(anItr++);
		}
		else
			++anItr;
	}

	return mAddrMap.size() < oldSize;
}
