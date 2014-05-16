#include "RoutingLogic.h"

#include "AddFriendDialog.h"
#include "AdminActionDialog.h"
#include "BadUserDialog.h"
#include "ChatCommandParser.h"
#include "ChatCtrl.h"
#include "CreateRoomDialog.h"
#include "LobbyClient.h"
#include "LobbyConfig.h"
#include "LobbyDialog.h"
#include "LobbyEvent.h"
#include "LobbyFriend.h"
#include "LobbyGame.h"
#include "LobbyGroup.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyServer.h"
#include "LobbyStagingPrv.h"
#include "PasswordDialog.h"
#include "RoomDialog.h"

#include "WONAuth/AuthContext.h"
#include "WONDir/GetDirOp.h"
#include "WONDB/GetUserIdFromNameOp.h"
#include "WONGUI/GUIConfig.h"
#include "WONRouting/RoutingBanClientOp.h"
#include "WONRouting/RoutingClientFlagsChangedOp.h"
#include "WONRouting/RoutingClientJoinedGroupOp.h"
#include "WONRouting/RoutingClientJoinedServerOp.h"
#include "WONRouting/RoutingClientLeftGroupOp.h"
#include "WONRouting/RoutingClientLeftServerOp.h"
#include "WONRouting/RoutingConnection.h"
#include "WONRouting/RoutingCreateGroupOp.h"
#include "WONRouting/RoutingDataObjectCreatedOp.h"
#include "WONRouting/RoutingDataObjectDeletedOp.h"
#include "WONRouting/RoutingGetBadUserListOp.h"
#include "WONRouting/RoutingGetGroupListReqOp.h"
#include "WONRouting/RoutingGetUserStatusOp.h"
#include "WONRouting/RoutingGroupCaptChangedOp.h"
#include "WONRouting/RoutingGroupCreatedOp.h"
#include "WONRouting/RoutingGroupDeletedOp.h"
#include "WONRouting/RoutingGroupInvitationOp.h"
#include "WONRouting/RoutingGroupJoinAttemptOp.h"
#include "WONRouting/RoutingJoinGroupOp.h"
#include "WONRouting/RoutingMemberFlagsChangedOp.h"
#include "WONRouting/RoutingMuteClientOp.h"
#include "WONRouting/RoutingPeerChatOp.h"
#include "WONRouting/RoutingPeerDataOp.h"
#include "WONRouting/RoutingReadDataObjectOp.h"
#include "WONRouting/RoutingRegisterClientOp.h"
#include "WONRouting/RoutingYouWereBannedOp.h"
#include "WONRouting/RoutingYouWereMutedOp.h"
#include "WONRouting/RoutingServerAlertOp.h"
#include "WONRouting/RoutingServShutdownStartOp.h"
#include "WONRouting/RoutingServShutdownAbortOp.h"


using namespace WONAPI;
typedef LogicCompletion<RoutingLogic> RoutingCompletion;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingOpCompletion : public LogicCompletionBase
{
private:
	typedef void(RoutingLogic::*Callback)(RoutingOp*, LobbyServer *theServer);
	Callback mCallback;
	RoutingLogicPtr mClass;
	
public:
	RoutingOpCompletion(Callback theCallback, RoutingLogic* theClass) : mCallback(theCallback), mClass(theClass) { }

	virtual void DoComplete(AsyncOp *theOp) 
	{ 
		if(mClass->IsKilled() || theOp->Killed())
			return;

		RoutingOp *anOp = (RoutingOp*)theOp;
		LobbyServer *aServer = mClass->GetServer(anOp);
		if(aServer!=NULL)
			((mClass.get())->*mCallback)(anOp,aServer); 
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{
class AutoKillPendingConnection
{
public:
	RoutingLogic *mLogic;

	AutoKillPendingConnection(RoutingLogic *theLogic) : mLogic(theLogic) { }
	~AutoKillPendingConnection() { mLogic->KillPendingConnection(); }
};
} // namespace WONAPI
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoutingLogic::RoutingLogic()
{
	mServerList = new LobbyServerList;	
	mGameList = new LobbyGameList(LobbyGameType_Internet);
	mGameObjectName = "G";
	mIsManualAFK = false;
	mIsAutoAFK = false;
	mGotGameList = false;
	mWantToBeModerator = false;
	mLastRoomFetchTime = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoutingLogic::~RoutingLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::Init(ComponentConfig *theConfig)
{
	WONComponentConfig_Get(theConfig,mAddFriendDialog,"AddFriendDialog");
	WONComponentConfig_Get(theConfig,mAdminActionDialog,"AdminActionDialog");
	WONComponentConfig_Get(theConfig,mBadUserDialog,"BadUserDialog");
	WONComponentConfig_Get(theConfig,mCreateRoomDialog,"CreateRoomDialog");
	WONComponentConfig_Get(theConfig,mPasswordDialog,"PasswordDialog");
	WONComponentConfig_Get(theConfig,mRoomDialog,"RoomDialog");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int RoutingLogic::GetSpecFlags(unsigned short theId)
{
	if(mChatGroup.get()!=NULL && mChatGroup->GetGroupId()==theId)
		return LobbyRoomSpecFlag_Chat | LobbyRoomSpecFlag_Internet;
	else if(mGameGroup.get()!=NULL && mGameGroup->GetGroupId()==theId)
		return LobbyRoomSpecFlag_Game | LobbyRoomSpecFlag_Internet;
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int RoutingLogic::GetSpecFlags(LobbyGroup *theGroup, LobbyServer *theServer)
{
	if(theServer!=mServer || theGroup==NULL)
		return 0;

	return GetSpecFlags(theGroup->GetGroupId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::CloseCompletion(AsyncOp *theOp)
{
	SocketOp *anOp = (SocketOp*)theOp;
	if(anOp->GetSocket()==GetConnection())
	{
		LobbyEvent::BroadcastEvent(new LeftRoomEvent(LobbyRoomSpecFlag_Internet,true));

		if(mGameGroup.get()!=NULL)
			LobbyStagingPrv::NotifyGameDisconnect(false);

		KillConnection();
		LobbyDialog::DoErrorDialog(RoutingLogic_DisconnectedTitle_String,RoutingLogic_DisconnectedDetails_String); 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::LocateFriendCompletion(AsyncOp *theOp)
{	
	RoutingGetUserStatusOp* anOp = (RoutingGetUserStatusOp*)theOp;

	LobbyServer *aServer = mServerList->GetServer(anOp->GetDestAddr());
	time_t aTime = time(NULL);

	LobbyFriendPtr aDoneFriend;
	int aNumDone = 0;

	// update the flags and last known location for all friends
	RoutingGetUserStatusOp::FriendStatusMap::const_iterator aStatusItr = anOp->GetFriendStatusMap().begin();
	for (; aStatusItr != anOp->GetFriendStatusMap().end(); ++aStatusItr)
	{
		LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aStatusItr->first);
		if (aFriend==NULL || !aFriend->IsSearching())
			continue;

		bool doneSearching = false;
		if (aStatusItr->second != 0 && aServer!=NULL)
		{
			aFriend->NotifyOnServer(aServer->GetIPAddr(),aServer->GetName(),aStatusItr->second);
			doneSearching = true;
		}

		if(aFriend->DecrementOps()<=0)
			doneSearching = true;

		if(doneSearching)
		{
			aDoneFriend = aFriend;
			aNumDone++;
			aFriend->EndSearch(aTime);
			if(aFriend->ReportLocationOnFind())
				LobbyEvent::BroadcastEvent(new AnnounceFriendEvent(aFriend, aFriend->GetReportLocationRoomSpec(), LobbyChangeType_Modify));
		}
	}

	if(aNumDone>1)
		LobbyEvent::BroadcastEvent(LobbyEvent_SyncFriendsList);
	else if(aNumDone==1 && aDoneFriend.get())
		LobbyEvent::BroadcastEvent(new FriendChangedEvent(aDoneFriend, LobbyChangeType_Modify));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::TimerEvent()
{
	if(mIsManualAFK || !IsConnected())
		return;

	DWORD aLastInputTime = LobbyScreen::GetWindowManager()->GetLastUserInputTick();
	DWORD aTick = GetTickCount();

	if(aTick - aLastInputTime > 300000)
	{
		if(!mIsAutoAFK)
		{
			RoutingConnection *aConnection = GetConnection();
			if(aConnection==NULL)
				return;

			aConnection->SetClientFlags(RoutingClientFlag_IsAway,RoutingClientFlag_IsAway,0,0);
			mIsAutoAFK = true;
		}
	}
	else if(mIsAutoAFK)
	{
		RoutingConnection *aConnection = GetConnection();
		if(aConnection==NULL)
			return;

		aConnection->SetClientFlags(RoutingClientFlag_IsAway,0,0,0);
		mIsAutoAFK = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ChatCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	if(theServer!=mServer.get() || (mChatGroup.get()==NULL && mGameGroup.get()==NULL))
		return;

	RoutingPeerChatOp *anOp = (RoutingPeerChatOp*)theOp;
	
	const RoutingRecipientList &aList = anOp->GetRecipients();
	if(aList.empty())
		return;

	int aDestId = aList.front();
	LobbyClient *aSender = theServer->GetClient(anOp->GetSenderId());
	if(aSender==NULL)
		return;

	if(aSender->IsIgnored() || aSender->IsBlocked())
		return;
 
	LobbyGroup *aDestGroup = NULL;
	LobbyClient *aDestClient = NULL;

	int aRoomSpecFlags = LobbyRoomSpecFlag_Internet; 
	if(anOp->GetFlags() & LobbyChatFlag_TeamChat)
	{
		if(mGameGroup.get()==NULL)
			return;

		aRoomSpecFlags |= LobbyRoomSpecFlag_Game | LobbyRoomSpecFlag_Team;
	}
	else if(aDestId < RoutingId_ClientMin)
	{
		aRoomSpecFlags = GetSpecFlags(aDestId);
		if(aRoomSpecFlags==0)
			return;
	}
	else
	{
		aDestClient = theServer->GetClient(aDestId);
		if(aDestClient==NULL)
			return;
	}


	if(anOp->IsWhisper())
	{
		bool toMe = aDestClient!=NULL && aDestClient->GetClientId()==theServer->GetMyClientId();
		if(toMe)
		{
			if(LobbyPersistentData::GetChatSoundEffects())
				LobbyMisc::PlaySound(LobbyGlobal_WhisperRecv_Sound);

			ChatCommandParser *aParser = LobbyMisc::GetChatCommandParser();
			if(aParser!=NULL)
				aParser->SetLastWhisperClientName(aSender->GetName());
		}
	}

	if((anOp->GetFlags()&LobbyChatFlag_IsWarning) && LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	LobbyEvent::BroadcastEvent(new RecvChatEvent(aRoomSpecFlags, anOp->GetFlags(), aSender,anOp->GetText(),aDestClient));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::PeerDataCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingPeerDataOp *anOp = (RoutingPeerDataOp*)theOp;
	if(theServer!=mServer.get() || mGameGroup.get()==NULL)
		return;

	LobbyClient *aClient = mGameGroup->GetClient(anOp->GetSenderId());
	if(aClient==NULL || aClient->GetClientId()==mServer->GetMyClientId()) // ignore messages from myself, those are not sent to me through the routing server
		return;

	LobbyStagingPrv::HandleGameMessage(anOp->GetData(),aClient);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientFlagsChangedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingClientFlagsChangedOp *anOp = (RoutingClientFlagsChangedOp*)theOp;

	LobbyClient *aClient = theServer->GetClient(anOp->GetClientId());
	if(aClient==NULL)
		return;
	
	int aRoomSpecFlags = LobbyRoomSpecFlag_Internet;

	aClient->SetClientFlags(anOp->GetNewClientFlags());
	LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,aRoomSpecFlags,LobbyChangeType_Modify));

	// Check friend anonymous
	if(!aClient->IsFriend())
		return;
	
	LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
	if(aFriend==NULL)
		return;

	if(aFriend->IsAnonymous()!=aClient->IsAnonymous())
	{
		aFriend->SetAnonymous(aClient->IsAnonymous());
		LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend, LobbyChangeType_Modify));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::MemberFlagsChangedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingMemberFlagsChangedOp *anOp = (RoutingMemberFlagsChangedOp*)theOp;

	LobbyGroup *aGroup = theServer->GetGroup(anOp->GetGroupId());
	if(aGroup==NULL)
		return;

	LobbyClient *aClient = aGroup->GetClient(anOp->GetClientId());
	if(aClient==NULL)
		return;

	aClient->SetMemberFlags(anOp->GetNewMemberFlags(),aGroup->IsGameRoom());	

	if(theServer==mServer.get())
	{
		int aRoomSpecFlags = GetSpecFlags(aGroup,theServer);
		if(aRoomSpecFlags!=0)			
			LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,aRoomSpecFlags,LobbyChangeType_Modify));	
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::CaptainChangedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingGroupCaptainChangedOp *anOp = (RoutingGroupCaptainChangedOp*)theOp;
	LobbyGroup *aGroup = theServer->GetGroup(anOp->GetGroupId());
	if(aGroup==NULL)
		return;

	LobbyClient *anOldCaptain = NULL;
	LobbyClient *aNewCaptain = aGroup->SetCaptain(anOp->GetNewCaptainId(),&anOldCaptain);

	if(theServer==mServer.get())
	{
		int aRoomSpecFlags = GetSpecFlags(aGroup,theServer);
		if(aRoomSpecFlags!=0)
		{			
			if(anOldCaptain!=NULL)
				LobbyEvent::BroadcastEvent(new ClientChangedEvent(anOldCaptain,aRoomSpecFlags,LobbyChangeType_Modify));	

			if(aNewCaptain!=NULL)
				LobbyEvent::BroadcastEvent(new ClientChangedEvent(aNewCaptain,aRoomSpecFlags,LobbyChangeType_Modify));	

			if(aGroup==mGameGroup.get() && anOldCaptain!=NULL) // can't change captain -> game dissolved
				LobbyStagingPrv::NotifyGameDisconnect();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientJoinedServerCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingClientJoinedServerOp *anOp = (RoutingClientJoinedServerOp*)theOp;
	LobbyClient* aClient = theServer->AddClient(anOp->GetClientInfo());

	// Check friend
	LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
	if(aFriend==NULL)
		return;

	aClient->SetIsFriend(true);
	aFriend->NotifyOnServer(theServer->GetIPAddr(),theServer->GetName(),0);
	aFriend->SetAnonymous(aClient->IsAnonymous());

	LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend, LobbyChangeType_Modify));
	if(theServer==mServer.get())
	{
		if(!aClient->IsAnonymous())
		{
			if(LobbyPersistentData::GetChatSoundEffects())
				LobbyMisc::PlaySound(LobbyGlobal_FriendJoined_Sound);

			LobbyEvent::BroadcastEvent(new AnnounceFriendEvent(aFriend, LobbyRoomSpecFlag_Internet, LobbyChangeType_Add));
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientLeftServerCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingClientLeftServerOp *anOp = (RoutingClientLeftServerOp*)theOp;
	LobbyClientPtr aClient = theServer->RemoveClient(anOp->GetClientId());
	if(aClient.get()==NULL)
		return;

	if(!aClient->IsFriend())
		return;

	// Check friend
	LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
	if(aFriend==NULL)
		return;

	aFriend->NotifyLeftServer();
	LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend, LobbyChangeType_Modify));
	if(theServer==mServer.get())
	{
		if(!aClient->IsAnonymous())
		{
			if(LobbyPersistentData::GetChatSoundEffects())
				LobbyMisc::PlaySound(LobbyGlobal_FriendLeft_Sound);

			LobbyEvent::BroadcastEvent(new AnnounceFriendEvent(aFriend, LobbyRoomSpecFlag_Internet, LobbyChangeType_Delete));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientJoinedGroupCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingClientJoinedGroupOp *anOp = (RoutingClientJoinedGroupOp*)theOp;	
	LobbyGroup *aGroup = theServer->GetGroup(anOp->GetGroupId());
	if(aGroup==NULL)
		return;

	LobbyClient *aClient = theServer->GetClient(anOp->GetClientId());
	if(aClient==NULL)
		return;

//	aGroup->AddClient(aClient,anOp->GetMemberFlags());
	theServer->AddClientToGroup(aClient,anOp->GetMemberFlags(),aGroup); // call this function so the server's user count is updated properly

	int aFlags = GetSpecFlags(aGroup,theServer);
	if(aFlags!=0)
		LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,aFlags,LobbyChangeType_Add));

	LobbyEvent::BroadcastEvent(new GroupChangedEvent(aGroup, theServer, LobbyChangeType_Modify));

	SyncClientWithFriends(aClient);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientLeftGroupCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingClientLeftGroupOp *anOp = (RoutingClientLeftGroupOp*)theOp;

	LobbyGroup *aGroup = theServer->GetGroup(anOp->GetGroupId());
	if(aGroup==NULL)
		return;

//	LobbyClientPtr aClient = aGroup->RemoveClient(anOp->GetClientId());
	LobbyClientPtr aClient = theServer->RemoveClientFromGroup(anOp->GetClientId(),aGroup); // call this function so the server's user count is updated
	if(aClient.get()==NULL)
		return;

	int aFlags = GetSpecFlags(aGroup,theServer);
	if(aFlags!=0)
	{
		LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,aFlags,LobbyChangeType_Delete));
		if(aFlags&LobbyRoomSpecFlag_Game)
			LobbyStagingPrv::NotifyClientLeftGame(aClient);
	}

	LobbyEvent::BroadcastEvent(new GroupChangedEvent(aGroup, theServer, LobbyChangeType_Modify));

	SyncClientWithFriends(aClient);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientBannedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingYouWereBannedOp *anOp = (RoutingYouWereBannedOp*)theOp;
	unsigned short aGroupId = anOp->GetGroupId();

	if(!anOp->GetAmBanned())
		return;

	ClientModeratedEvent::ModerationAction anAction;
	if(anOp->GetBanTime()==0 || anOp->GetBanTime()>=10)
		anAction = ClientModeratedEvent::Action_Ban;
	else
		anAction = ClientModeratedEvent::Action_Kick;

	GUIString aRoomName;
	LobbyRoomSpec aRoomSpec;

	// Figure out what we were banned from
	if(aGroupId==RoutingId_Global)
	{
		aRoomSpec = 0;
		aRoomName = theServer->GetName();
	}
	else if(mGameGroup.get()!=NULL && aGroupId==mGameGroup->GetGroupId())
	{
		aRoomSpec = LobbyRoomSpecFlag_Game;
		aRoomName = mGameGroup->GetName();
	}
	else if(mChatGroup.get()!=NULL && aGroupId==mChatGroup->GetGroupId())
	{
		aRoomSpec = LobbyRoomSpecFlag_Chat;
		aRoomName = mChatGroup->GetName();
	}
	else
		return; // don't care about this group

	// Leave the room we were banned from
	LobbyClient *myClient = theServer->GetMyClient();
	if(aRoomSpec.IsGameRoom())
	{
		LobbyStagingPrv::NotifyClientKicked(myClient,(anAction==ClientModeratedEvent::Action_Ban));
		return;
	}

	LobbyEvent::BroadcastEvent(new ClientModeratedEvent(myClient, LobbyRoomSpecFlag_Internet | aRoomSpec, aRoomName, anAction, anOp->GetBanTime(), true, anOp->GetBanComment()));
	if(aRoomSpec.IsChatRoom())
	{
		if(mGameGroup.get()==NULL) // no more groups
			KillConnection();
		else
		{
			mChatGroup = NULL;
			LobbyEvent::BroadcastEvent(new LeftRoomEvent(LobbyRoomSpecFlag_Internet | LobbyRoomSpecFlag_Chat,false,false));
		}
	}
	else // leave server
	{
		bool inGame = mGameGroup.get()!=NULL;
		if(inGame)
			LobbyStaging::LeaveGame(false); // prevent game disconnect message

		KillConnection();
	
		if(inGame)
			LobbyScreen::ShowLobbyTab(); // make sure they see the message that they were kicked from the server
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ClientMutedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingYouWereMutedOp *anOp = (RoutingYouWereMutedOp*)theOp;
	unsigned short aGroupId = anOp->GetGroupId();

	ClientModeratedEvent::ModerationAction anAction;
	if(anOp->GetAmMuted())
		anAction = ClientModeratedEvent::Action_Mute;
	else
		anAction = ClientModeratedEvent::Action_Unmute;

	GUIString aRoomName;
	LobbyRoomSpec aRoomSpec;

	// Figure out where we were muted
	if(aGroupId==RoutingId_Global)
	{
		aRoomSpec = 0;
		aRoomName = theServer->GetName();
	}
	else if(mGameGroup.get()!=NULL && aGroupId==mGameGroup->GetGroupId())
	{
		aRoomSpec = LobbyRoomSpecFlag_Game;
		aRoomName = mGameGroup->GetName();
	}
	else if(mChatGroup.get()!=NULL && aGroupId==mChatGroup->GetGroupId())
	{
		aRoomSpec = LobbyRoomSpecFlag_Chat;
		aRoomName = mChatGroup->GetName();
	}
	else
		return; // don't care about this group

	LobbyClient *myClient = theServer->GetMyClient();
	LobbyEvent::BroadcastEvent(new ClientModeratedEvent(myClient, LobbyRoomSpecFlag_Internet | aRoomSpec, aRoomName, anAction, anOp->GetMuteTime(), true, anOp->GetMuteComment()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ServerAlertCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingServerAlertOp* anOp = (RoutingServerAlertOp*)theOp;
	if(LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	LobbyEvent::BroadcastEvent(new AlertEvent(LobbyRoomSpecFlag_Internet, AlertEvent::Action_Alert, 0, anOp->GetAlertText()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ServerShutdownStartedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingServerShutdownStartedOp* anOp = (RoutingServerShutdownStartedOp*)theOp;
	if(LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	LobbyEvent::BroadcastEvent(new AlertEvent(LobbyRoomSpecFlag_Internet, AlertEvent::Action_ShutdownStarted, anOp->GetSecondsUntilShutdown(), anOp->GetAlertText()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ServerShutdownAbortedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingServerShutdownAbortedOp* anOp = (RoutingServerShutdownAbortedOp*)theOp;
	if(LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	LobbyEvent::BroadcastEvent(new AlertEvent(LobbyRoomSpecFlag_Internet, AlertEvent::Action_ShutdownAborted, 0, anOp->GetAlertText()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::DataObjectCreatedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	if(theServer!=mServer.get())
		return;

	RoutingDataObjectCreatedOp* anOp = (RoutingDataObjectCreatedOp*)theOp;
	if (anOp->GetDataType() != mGameObjectName)
		return;

	LobbyGroup *aGroup = theServer->GetGroup(anOp->GetLinkId());
	if(aGroup==NULL)
		return;
		
	bool isModify = false;
	LobbyGame *aGame = mGameList->AddGameById(aGroup->GetGroupId(),&isModify);
	if(!aGame->ReadSummary(anOp->GetData()))
	{
		LobbyGamePtr aSaveGame = aGame; // hold on to a reference
		mGameList->RemoveGameById(aGroup->GetGroupId());

		if(isModify)
			LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Delete));

		return;
	}

	if(!isModify)
	{
		aGame->InitFromLobbyGroup(aGroup);
		if(mGameGroup.get()!=NULL) // see if this is the summary for our game
		{
			LobbyGame *myGame = LobbyStaging::GetGame();
			if(myGame!=NULL && myGame->IAmCaptain() && aGame->GetGroupId()==myGame->GetGroupId())
				aGame->SetIAmInvited(true);
		}
		if(LobbyPersistentData::GetAutoPingGames())
			LobbyMisc::QueuePing(aGame);
	}

	LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,isModify?LobbyChangeType_Modify:LobbyChangeType_Add));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::DataObjectDeletedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	if(theServer!=mServer.get())
		return;

	RoutingDataObjectDeletedOp* anOp = (RoutingDataObjectDeletedOp*)theOp;
	if (anOp->GetDataType() != mGameObjectName)
		return;

	// Find this game and remove it
	LobbyGamePtr aGame = mGameList->RemoveGameById(anOp->GetLinkId());
	if(aGame.get()==NULL)
		return;

	LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Delete));
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static GUIString GetGameName() // for testing
{
	static GUIString words[] = 
	{
		"Server",
		"EmpireEarth",
		"Clan",
		"DogHouse",
		"Sierra",
		"v1.64",
		"Stainless",
		"Boston",
		"John's",
		"Brian",
		"Ross",
		"House of Pain",
		"SmackDown",
		"Lay",
		"the"
	};

	static int aNumWords = sizeof(words)/sizeof(words[0]);
	GUIString aString;

	int aNum = rand()%3+2;
	for(int i=0; i<aNum; i++)
	{
		if(i!=0)
			aString.append(' ');
		
		aString.append(words[rand()%aNumWords]);
	}

	return aString;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GenerateGameList(LobbyGameList *theList)
{
	// for testing (insert lots 'o games)
	int aNumGames = 10000;
	IPAddr anAddr = "noodle:12345";
	for(int i=0; i<aNumGames; i++)
	{
		LobbyGamePtr aGame = LobbyGame::CreateGame(LobbyGameType_Internet);
		aGame->SetName(GetGameName());
		int aMaxPlayers = rand()%100 + 1;
		int aNumPlayers = rand()%(aMaxPlayers+1);
		aGame->SetMaxPlayers(aMaxPlayers);
		aGame->SetNumPlayers(aNumPlayers);
		aGame->SetPing(rand()%1000);
		if(rand()%30==0)
			aGame->SetPing(LobbyPing_Failed);

		if(rand()%10==0)
			aGame->SetInProgress(true);

		int aProtection = rand()%10;
		switch(aProtection)
		{
			case 0: aGame->SetHasPassword(true); break;
			case 1: aGame->SetAskToJoin(true); break;
			case 2: aGame->SetInviteOnly(true); break;
			case 3: aGame->SetInviteOnly(true); aGame->SetIAmInvited(true); break;
		}

		aGame->SetSkillLevel((LobbySkillLevel)(rand()%5));
		aGame->SetIPAddr(anAddr);
		aGame->SetGroupId(i);

		theList->AddGame(aGame);		
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ReadDataObjectCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	if(theServer!=mServer.get())
		return;

	mGameList->Clear();
	RoutingReadDataObjectOp* anOp = (RoutingReadDataObjectOp*)theOp;

	bool autoPingGames = LobbyPersistentData::GetAutoPingGames();
	RoutingDataObjectList::const_iterator anItr = anOp->GetDataObjects().begin();
	for (; anItr != anOp->GetDataObjects().end(); ++anItr)
	{
		RoutingDataObject *anObject = *anItr;

		LobbyGroup *aGroup = theServer->GetGroup(anObject->mLinkId);
		if(aGroup==NULL)
			continue;

		LobbyGamePtr aGame = LobbyGame::CreateGame(LobbyGameType_Internet);
		aGame->InitFromLobbyGroup(aGroup);
		if(!aGame->ReadSummary(anObject->mData))
			continue;
		
		mGameList->AddGame(aGame);
	
		if(autoPingGames)
			LobbyMisc::QueuePing(aGame);
	}

	// GenerateGameList is just for testing large server lists
	//GenerateGameList(mGameList);

	LobbyEvent::BroadcastEvent(new SyncGameListEvent(mGameList));

	mGotGameList = true;
	if(mGetGameListOp.get()!=NULL)
		mGetGameListOp->Finish(WS_Success);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GroupCreatedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingGroupCreatedOp *anOp = (RoutingGroupCreatedOp*)theOp;
	LobbyGroup *aGroup = theServer->AddGroup(anOp->GetGroupInfo());
	if (aGroup != NULL)
		LobbyEvent::BroadcastEvent(new GroupChangedEvent(aGroup, theServer, LobbyChangeType_Add));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GroupDeletedCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	RoutingGroupDeletedOp *anOp = (RoutingGroupDeletedOp*)theOp;
	LobbyGroupPtr aGroup = theServer->RemoveGroup(anOp->GetGroupId());
	if(aGroup.get()!=NULL)
		LobbyEvent::BroadcastEvent(new GroupChangedEvent(aGroup, theServer, LobbyChangeType_Delete));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GroupInvitationCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	if(theServer!=mServer.get())
		return;

	RoutingGroupInvitationOp *anOp = (RoutingGroupInvitationOp*)theOp;

	LobbyClient *aClient = theServer->GetClient(anOp->GetCaptainId());
	if(aClient==NULL || aClient->IsIgnored() || aClient->IsBlocked())
		return;

	LobbyGame *aGame = mGameList->GetGameById(anOp->GetGroupId());
	if(aGame==NULL)
		return;

	aGame->SetIAmInvited(anOp->GetAmInvited());
	LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Modify));

	if(LobbyPersistentData::GetStagingSoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_InviteRecv_Sound);

	LobbyEvent::BroadcastEvent(new GroupInvitationEvent(LobbyRoomSpecFlag_Internet, aGame->GetName(),aGame->GetGroupId(),aClient,anOp->GetAmInvited(),anOp->GetComment(),false));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GroupJoinAttemptCompletion(RoutingOp *theOp, LobbyServer *theServer)
{
	if(theServer!=mServer.get() || mGameGroup.get()==NULL)
		return;

	RoutingGroupJoinAttemptOp *anOp = (RoutingGroupJoinAttemptOp*)theOp;
	if(anOp->GetGroupId()!=mGameGroup->GetGroupId())
		return;

	LobbyClient *aClient = theServer->GetClient(anOp->GetClientId());
	if(aClient==NULL || aClient->IsIgnored() || aClient->IsBlocked())
		return; // FIXME: reject?

	if(LobbyPersistentData::GetStagingSoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_AskToJoinRecv_Sound);

	int aFlags = LobbyRoomSpecFlag_Internet | LobbyRoomSpecFlag_Game;
	LobbyEvent::BroadcastEvent(new GroupJoinAttemptEvent(aFlags,aClient,anOp->GetComment()));	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::DoAdminActionDialog(LobbyClientAction theAction, int theRoomSpecFlags, LobbyClient *theClient)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	LobbyClient *myClient = mServer->GetMyClient();
	if(myClient==NULL)
		return;

	// Do Dialog
	bool amModerator = myClient->IsModerator() || myClient->IsAdmin();
	if(amModerator)
	{
		mAdminActionDialog->Reset(theAction,theClient);
		int aResult = LobbyDialog::DoDialog(mAdminActionDialog);
		if(aResult!=ControlId_Ok)
			return;

		aConnection = GetConnection(); // recheck connection
		if(aConnection==NULL)
			return;
	}

	// Figure out context
	LobbyClient *anUpToDateClient = mServer->GetClientByWONId(theClient->GetWONUserId());
	int aGroupId = RoutingId_Global;
	if(theRoomSpecFlags & LobbyRoomSpecFlag_Game)
	{
		if(mGameGroup.get()==NULL)
			return;
		aGroupId = mGameGroup->GetGroupId();
	}
	else if(theRoomSpecFlags & LobbyRoomSpecFlag_Chat)
	{
		if(mChatGroup.get()==NULL)
			return;
		aGroupId = mChatGroup->GetGroupId();
	}

	GUIString anExplanation; 
	unsigned long aNumSeconds;
	if(amModerator)
	{
		if(mAdminActionDialog->GetApplyToServer())
			aGroupId = RoutingId_Global;
		else if(aGroupId==RoutingId_Global && theAction!=LobbyClientAction_Warn)
			return;

		anExplanation = mAdminActionDialog->GetExplanation();
		aNumSeconds = mAdminActionDialog->GetNumSeconds();
	}
	else // captain
	{
		if(aGroupId==RoutingId_Global) // can only apply to room if captain
			return;

		if(theAction==LobbyClientAction_Kick)
			aNumSeconds = 1;
		else
			aNumSeconds = 0; // infinite
	}


	RoutingOpPtr anOpToRun;
	GUIString aTitle;
	switch(theAction)
	{
		case LobbyClientAction_Ban: 
		case LobbyClientAction_Kick:
		{	
			if(!amModerator && (theRoomSpecFlags & LobbyRoomSpecFlag_Game))
			{
				// Captain just does a game kick
				LobbyStaging::KickClient(theClient,theAction==LobbyClientAction_Ban);
				return;
			}

			RoutingBanClientOpPtr anOp = new RoutingBanClientOp(aConnection);

			if (theAction == LobbyClientAction_Ban)
			{
				anOp->SetBanComment(anExplanation);
				anOp->SetBanTime(aNumSeconds);
				aTitle = RoutingLogic_BanClientTitle_String;
			}
			else if (theAction == LobbyClientAction_Kick)
			{
				anOp->SetBanTime(1);
				aTitle = RoutingLogic_KickClientTitle_String;
			}

			anOp->SetGroupId(aGroupId);
			anOpToRun = anOp;

			if (anUpToDateClient!=NULL)
				anOp->SetClientId(anUpToDateClient->GetClientId());
			else
			{
				if (theClient->GetCDKeyId() != 0)
					anOp->SetWONId(theClient->GetCDKeyId(), true);
				else if (theClient->GetWONUserId() != 0)
					anOp->SetWONId(theClient->GetWONUserId(),true);
			}
		}
		break;

		case LobbyClientAction_Mute:
		case LobbyClientAction_Unmute: 
		{
			RoutingMuteClientOpPtr anOp = new RoutingMuteClientOp(aConnection);
			anOp->SetMuteComment(anExplanation);
			anOp->SetMuteTime(aNumSeconds);
			anOp->SetGroupId(aGroupId);
			anOpToRun = anOp;

			if (anUpToDateClient!=NULL)
				anOp->SetClientId(anUpToDateClient->GetClientId());
			else
			{
				if (theClient->GetCDKeyId() != 0)
					anOp->SetWONId(theClient->GetCDKeyId(), true);
				else if (theClient->GetWONUserId() != 0)
					anOp->SetWONId(theClient->GetWONUserId(),true);
			}

			if (theAction == LobbyClientAction_Unmute)
			{
				aTitle = RoutingLogic_UnmuteClientTitle_String;
				anOp->SetIsMuted(false);
			}
			else
			{
				aTitle = RoutingLogic_MuteClientTitle_String;
				anOp->SetIsMuted(true);
			}
		}
		break;


		case LobbyClientAction_Warn:
			if(anUpToDateClient!=NULL)
				aConnection->SendChat(anExplanation,RoutingChatFlag_IsWarning,anUpToDateClient->GetClientId());
			return;
	}

	if(anOpToRun.get()==NULL)
		return;

	if(amModerator)
		LobbyDialog::DoStatusOp(anOpToRun,aTitle,RoutingLogic_ModerateDetails_String,CloseStatusDialogType_Success);
	else
		RunOp(anOpToRun,NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::DoRoomDialog()
{
	LobbyDialog::DoDialog(mRoomDialog);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClientList* RoutingLogic::GetClientList()
{
	if(mServer.get()!=NULL)
		return mServer->GetClientList();
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GetBadUsers(bool mutes, bool global, int theRoomSpecFlags)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	LobbyClient *myClient = mServer->GetMyClient();
	if(myClient==NULL)
		return;

	bool amModerator = myClient->IsModerator() || myClient->IsAdmin();


	RoutingBadUserListType aListType = mutes?RoutingBadUserListType_MutedUsers:RoutingBadUserListType_BannedUsers;
	unsigned short aGroupId = RoutingId_Global;
	if(!global)
	{
		LobbyGroup *aGroup = GetGroupBySpec(theRoomSpecFlags);
		if(aGroup==NULL)
			return;

		aGroupId = aGroup->GetGroupId();
	}

	RoutingGetBadUserListOpPtr anOp = new RoutingGetBadUserListOp(aConnection,aListType,aGroupId,
		RoutingGetBadUserList_IncludeExpirationDiff | RoutingGetBadUserList_IncludeModeratorWONUserId |
		RoutingGetBadUserList_IncludeModeratorName | RoutingGetBadUserList_IncludeModeratorComment);

	LobbyDialog::DoStatusOp(anOp,RoutingLogic_GetBadUsersTitle_String,RoutingLogic_GetBadUsersDetails_String,CloseStatusDialogType_Success);
	if(!anOp->Succeeded())
		return;

	mBadUserDialog->SetUserList(anOp->GetUserList(), !amModerator);

	int aResult = LobbyDialog::DoDialog(mBadUserDialog);
	if(aResult!=ControlId_Ok)
		return;		

	aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	BadUserDialog::IdList aList;
	mBadUserDialog->GetIds(aList);
	if(aList.empty())
		return;

	BadUserDialog::IdList::iterator anItr = aList.begin();
	while(anItr!=aList.end())
	{
		if(aListType==RoutingBadUserListType_BannedUsers)
			aConnection->BanClientByWONId(aGroupId,false,0,L"",*anItr);
		else if(aListType==RoutingBadUserListType_MutedUsers)
			aConnection->MuteClientByWONId(aGroupId,false,0,L"",*anItr);

		++anItr;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GetGroupListReqCompletion(AsyncOp *theOp)
{
	RoutingGetGroupListReqOp *anOp = (RoutingGetGroupListReqOp*)theOp;
	LobbyServerList *aList = LobbyMisc::GetLobbyServerList();
	if(aList==NULL)
		return;

	LobbyServer *aServer = aList->GetServer(anOp->GetAddr());
	if(aServer==NULL)
		return;

	aServer->SetQueryingGroups(false);
	if(!anOp->Succeeded())
	{
		mLastRoomFetchTime = 0;
		aServer->SetLastGroupQueryTime(0);
		// mark error?
	}

	aServer->UpdateGroups(anOp->GetGroupMap(),false);
	if(!aServer->IsConnected() && anOp->GetClientCount()>0)
		aServer->SetNumClients(anOp->GetClientCount());

	LobbyEvent::BroadcastEvent(new ServerChangedEvent(aServer,LobbyChangeType_Modify));
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::GetRooms()
{
	ServerContextPtr aRoomDirServers = LobbyMisc::GetRoomDirServers();
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aRoomDirServers.get()==NULL || aConfig==NULL)
		return;

	time_t aTime = time(NULL);
	if(aTime - mLastRoomFetchTime >= 300 || mServerList->GetServerMap().empty())
	{
		GetDirOpPtr anOp = new GetDirOp(aRoomDirServers);
		anOp->SetPath(aConfig->mProductDirectory);
		anOp->SetFlags(DIR_GF_DECOMPSERVICES | DIR_GF_ADDDISPLAYNAME | DIR_GF_SERVADDNETADDR | DIR_GF_ADDDODATA | DIR_GF_ADDDOTYPE);
		anOp->AddDataType("_C"); // get number of clients
		anOp->AddDataType("_L"); // get server language

		LobbyDialog::DoStatusOp(anOp,LobbyContainer_GettingRoomsTitle_String,LobbyContainer_GettingRoomsDetails_String,CloseStatusDialogType_Success);
		if(!anOp->Succeeded())
			return;

		if(anOp->GetDirEntityList().empty())
			aRoomDirServers->NotifyFailed(anOp->GetCurAddr());

		mServerList->Update(anOp->GetDirEntityList());
		mLastRoomFetchTime = aTime;
	}

	aTime = time(NULL);
	const LobbyServerMap &aMap = mServerList->GetServerMap();
	LobbyServerMap::const_iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		LobbyServer *aServer = anItr->second;
		if(!aServer->IsConnected() && aTime-aServer->GetLastGroupQueryTime()>=30)
			aServer->ClearGroups(); // clear old group information

		++anItr;
	}

	LobbyEvent::BroadcastEvent(LobbyEvent_SetRoomList);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::IsConnected()
{
	return mServer.get()!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoutingConnection* RoutingLogic::GetConnection()
{
	if(mServer.get()==NULL)
		return NULL;
	else
		return mServer->GetConnection();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroup* RoutingLogic::GetGroupBySpec(int theRoomSpecFlags)
{
	if(theRoomSpecFlags & LobbyRoomSpecFlag_Chat)
		return mChatGroup;
	else if(theRoomSpecFlags & LobbyRoomSpecFlag_Game)
		return mGameGroup;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGroup* RoutingLogic::GetGroup(int theId)
{
	if(mServer.get()==NULL)
		return NULL;
	else
		return mServer->GetGroup(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer* RoutingLogic::GetServer(const IPAddr &theAddr)
{
	return mServerList->GetServer(theAddr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer* RoutingLogic::GetServer(RoutingOp *theOp)
{
	if(mServer.get()!=NULL && theOp->GetConnection()==mServer->GetConnection())
		return mServer;
	else if(mPendingServer.get()!=NULL && theOp->GetConnection()==mPendingServer->GetConnection())
		return mPendingServer;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleControlEvent(int theControlId)
{
	switch(theControlId)
	{
		case ID_ChangeRoom: 
			GetRooms(); 
			DoRoomDialog(); 
			break;

		case ID_AddFriend: HandleAddFriend(); break;
		case ID_AnonymousToFriendsCheck: HandleAnonymousModeChange(); break;
		case ID_FilterDirtyWordsCheck: HandleDirtyWordFilterChange(); break;
		case ID_RefreshFriends: RefreshFriendsList(); break;
		case ID_RefreshGameList: HandleRefreshGameList(); break;
	}
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_AcceptClient: HandleAcceptClient((AcceptClientEvent*)theEvent); return;
		case LobbyEvent_ClientAction: HandleClientAction((ClientActionEvent*)theEvent); return;
		case LobbyEvent_CreateRoom: HandleCreateRoom((CreateRoomEvent*)theEvent); return;
		case LobbyEvent_DoChatCommand: HandleDoChatCommand(theEvent->mComponent); return;
		case LobbyEvent_FriendAction: HandleFriendAction((FriendActionEvent*)theEvent); return;
		case LobbyEvent_JoinRoom: HandleJoinRoom((JoinRoomEvent*)theEvent); return;
		case LobbyEvent_JoinFriendsRoom: HandleJoinFriendsRoom((JoinFriendsRoomEvent*)theEvent); return;
		case LobbyEvent_JumpToRoom: HandleJumpToRoom((JumpToRoomEvent*)theEvent); return;
		case LobbyEvent_QueryServerGroups: HandleQueryServerGroups((QueryServerGroupsEvent*)theEvent); return;
		case LobbyEvent_QuickPlay: HandleQuickPlay((QuickPlayEvent*)theEvent); return;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "ClientOptionsPopup.h"
void RoutingLogic::ShowClientOptions(LobbyClient *theClient, int theRoomSpecFlags)
{
	if(mServer.get()==NULL || theClient==NULL)
		return;

	LobbyClient *myClient = mServer->GetMyClient();
	if(myClient==NULL)
		return;

	ClientOptionsPopupPtr aPopup = new ClientOptionsPopup; 
	aPopup->Init(theClient,myClient,theRoomSpecFlags);
	LobbyDialog::DoPopup(aPopup,PopupFlag_StandardPopup);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "FriendOptionsPopup.h"
void RoutingLogic::ShowFriendOptions(LobbyFriend *theFriend, int theRoomSpecFlags)
{
	FriendOptionsPopupPtr aPopup = new FriendOptionsPopup;
	aPopup->Init(theFriend, theRoomSpecFlags);
	LobbyDialog::DoPopup(aPopup,PopupFlag_StandardPopup);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleAcceptClient(AcceptClientEvent *theEvent)
{
	if(mServer.get()==NULL || mGameGroup.get()==NULL)
		return;

	RoutingConnection *aConnection = mServer->GetConnection();
	if(aConnection==NULL)
		return;

	if(theEvent->mClient.get()==NULL)
		return;

	LobbyClient *aClient = mServer->GetClient(theEvent->mClient->GetClientId());
	if(aClient!=theEvent->mClient.get())
		return;

	aConnection->AcceptClient(mGameGroup->GetGroupId(),aClient->GetClientId(),theEvent->mAccepted,L"");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleClientAction(ClientActionEvent *theEvent)
{
	if(mServer.get()==NULL)
		return;

	RoutingConnection *aConnection = mServer->GetConnection();
	if(aConnection==NULL)
		return;

	if(!theEvent->mRoomSpec.IsInternet())
		return;

	LobbyClient *aClient = theEvent->mClient;
	if(aClient==NULL)
		return;

	LobbyClientAction anAction = theEvent->mAction;

	switch(anAction)
	{
		case LobbyClientAction_ShowMenu: ShowClientOptions(aClient,theEvent->mRoomSpec); break;

		case LobbyClientAction_Ban: 
		case LobbyClientAction_Kick:
		case LobbyClientAction_Mute:
		case LobbyClientAction_Warn:
		case LobbyClientAction_Unmute: 
			DoAdminActionDialog(anAction,theEvent->mRoomSpec,aClient);
			break;

		case LobbyClientAction_AddToFriends: 
		{
			// Invalid wonuserid, will cause problems if clients do not have unique id's
			if (aClient->GetWONUserId() == 0)
				return;

			// User is already a friend
			if (aClient->IsFriend())
				return;

			if(!CheckFriendMapSize())
				return;

			aClient->SetIsFriend(true);
			LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,LobbyRoomSpecFlag_Internet,LobbyChangeType_Modify));

			// Store in persistent data
			LobbyFriend *aFriend = LobbyPersistentData::AddFriend(aClient->GetName(), aClient->GetWONUserId());
			if(aFriend!=NULL)
			{
				LobbyClient *anUpToDateClient = mServer->GetClientByWONId(aClient->GetWONUserId());
				if(anUpToDateClient!=NULL)
				{
					aFriend->NotifyOnServer(mServer->GetIPAddr(),mServer->GetName(),0);
					aFriend->UpdateFlagsFromClient(anUpToDateClient);
				}
				else
				{} // Find the client?

				LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend,LobbyChangeType_Add));
			}
		}
		break;
		
		case LobbyClientAction_RemoveFromFriends: 
		{
			if(!aClient->IsFriend())
				return;

			aClient->SetIsFriend(false);
			LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,LobbyRoomSpecFlag_Internet,LobbyChangeType_Modify));

			// Remove from persistent data (only need a valid WONId)
			LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
			if(aFriend!=NULL)
				LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend,LobbyChangeType_Delete));

			LobbyPersistentData::RemoveFriend(aClient->GetWONUserId());
		}	
		break;

		case LobbyClientAction_Block: LobbyMisc::SetBlocked(aClient,true,LobbyRoomSpecFlag_Internet); break;
		case LobbyClientAction_ClearAFK: mIsManualAFK = false; aConnection->SetClientFlags(RoutingClientFlag_IsAway,0,0,0); break;
		case LobbyClientAction_Ignore: LobbyMisc::SetIgnored(aClient,true,LobbyRoomSpecFlag_Internet); break;
		case LobbyClientAction_Invite: InviteClient(aClient,true,""); break;
		case LobbyClientAction_Uninvite: InviteClient(aClient,false,""); break;
		case LobbyClientAction_SetAFK: mIsManualAFK = true; aConnection->SetClientFlags(RoutingClientFlag_IsAway,RoutingClientFlag_IsAway,0,0);  break;
		case LobbyClientAction_Unblock: LobbyMisc::SetBlocked(aClient,false,LobbyRoomSpecFlag_Internet); break;
		case LobbyClientAction_Unignore: LobbyMisc::SetIgnored(aClient,false,LobbyRoomSpecFlag_Internet); break;	
		case LobbyClientAction_Whisper: LobbyEvent::BroadcastEvent(new SetChatInputEvent(LobbyChatCommand_Whisper, aClient->GetName(), theEvent->mRoomSpec)); break;
		
		case LobbyClientAction_Locate: 
		{
			if(aClient->IsFriend())
			{
				LobbyFriend *aFriend = LobbyPersistentData::GetFriend(aClient->GetWONUserId());
				if(aFriend!=NULL)
				{
					LocateFriend(aFriend, theEvent->mRoomSpec);
					return;
				}
			}
			ReportClientLocation(aClient,true, theEvent->mRoomSpec);
		}
		break;

	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleCreateRoom(CreateRoomEvent *theEvent)
{
	mCreateRoomDialog->Reset();
	int aResult = LobbyDialog::DoDialog(mCreateRoomDialog);
	if(aResult!=ControlId_Ok)
		return;		

	if(JoinPending()) // can't be joining two groups at the same time
		return;

	GUIString aRoomName = mCreateRoomDialog->GetRoomName();
	GUIString aRoomPassword = mCreateRoomDialog->GetPassword();
	if(aRoomName.empty()) // can't have blank room name
		return;

	LobbyServer *aServer = GetServer(theEvent->mServerAddr);
	if(aServer==NULL)
	{
		aServer = mServer;
		if(aServer==NULL)
			aServer = GetDefaultServer();
	}

	if(aServer==NULL) // no servers to create room on
		return;

	AutoKillPendingConnection aKill(this);
	if(!JoinServer(aServer))
		return;

	unsigned short aGroupId;
	if(!CreateGroup(mPendingServer,aRoomName,aRoomPassword,RoutingGroupFlag_IsChatRoom,aGroupId))
		return;

	SucceedJoinGroup(aGroupId,false);

	if(mRoomDialog->GetParent()!=NULL)
		LobbyScreen::GetWindowManager()->EndDialog(-1,NULL,mRoomDialog);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::NetCreateGame(LobbyGame *theGame)
{
	if(JoinPending() || mGameGroup.get()!=NULL)
		return false;

	LobbyServer *aServer = mServer;
	if(aServer==NULL)
		aServer = GetDefaultServer();

	if(aServer==NULL) // no servers to create room on
		return false;

	AutoKillPendingConnection aKill(this);
	if(!JoinServer(aServer))
		return false;

	unsigned short aGroupId;
	int aFlags = 0;
	if(theGame->IsAskToJoin())
		aFlags |= RoutingGroupFlag_AskCaptainToJoin;
	if(theGame->IsInviteOnly())
		aFlags |= RoutingGroupFlag_InviteOnly;

	if(!CreateGroup(mPendingServer,theGame->GetName(),theGame->GetPassword(),aFlags,aGroupId))
		return false;

	SucceedJoinGroup(aGroupId,true);

	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return false;

	theGame->SetGroupId(aGroupId);
	aServer->GetClientList()->ClearInvites();

	if(mGameGroup.get()!=NULL)
		theGame->SetClientList(mGameGroup->GetClientList());

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::NetJoinGame(LobbyGame *theGame)
{
	if(JoinPending() || mGameGroup.get()!=NULL || mServer.get()==NULL || theGame->GetGameType()!=LobbyGameType_Internet)
		return false;

	AutoKillPendingConnection aKill(this);
	if(!JoinServer(mServer))
		return false;

	if(!JoinGroup(mPendingServer,theGame->GetGroupId()))
		return false;

	SucceedJoinGroup(theGame->GetGroupId(),true);
	if(mGameGroup.get()!=NULL)
		theGame->SetClientList(mGameGroup->GetClientList());

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetKickClient(LobbyClient *theClient, bool isBan)
{
	if(mGameGroup.get()==NULL)
		return;

	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	aConnection->BanClientByClientId(mGameGroup->GetGroupId(),true,isBan?0:1,L"",theClient->GetClientId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetCloseGame(bool close)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL || mGameGroup.get()==NULL)
		return;

	aConnection->SetGroupFlags(mGameGroup->GetGroupId(),RoutingGroupFlag_Closed,close?RoutingGroupFlag_Closed:0,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetLeaveGame(LobbyGame *theGame)
{
	if(mGameGroup.get()==NULL)
		return;

	if(mChatGroup.get()==NULL)
	{
		mGameGroup = NULL; // prevent game disconnect message
		KillConnection();
	}
	else 
	{
		RoutingConnection *aConnection = GetConnection();
		if(aConnection!=NULL)
			aConnection->LeaveGroup(mGameGroup->GetGroupId());

		mGameGroup = NULL;
	}

	LobbyEvent::BroadcastEvent(new LeftRoomEvent(LobbyRoomSpecFlag_Internet | LobbyRoomSpecFlag_Game));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleAnonymousModeChange()
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	unsigned long clientFlags = 0;
	if (LobbyPersistentData::GetAnonymousToFriends())
		clientFlags |= RoutingClientFlag_IsAnonymous;

	aConnection->SetClientFlags(RoutingClientFlag_IsAnonymous,clientFlags,0,0);
}

		
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleDirtyWordFilterChange()
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	unsigned long clientFlags = 0;
	if (LobbyPersistentData::GetFilterDirtyWords()!=true)
		clientFlags |= RoutingClientFlag_DirtyWordFilteringOff;

	aConnection->SetClientFlags(RoutingClientFlag_DirtyWordFilteringOff,clientFlags,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleDoChatCommand(Component *theChatCtrl)
{
	if(mServer.get()==NULL)
		return;

	RoutingConnection *aConnection = mServer->GetConnection();
	if(aConnection==NULL)
		return;

	ChatCommandParser *aParser = LobbyMisc::GetChatCommandParser();
	if(aParser==NULL)
		return;

	if(!(aParser->GetRoomSpecFlags() & LobbyRoomSpecFlag_Internet))
		return;

	LobbyGroup *aGroup = GetGroupBySpec(aParser->GetRoomSpecFlags());
	if(aGroup==NULL)
		return;

	int aGroupId = aGroup->GetGroupId();
	LobbyClient *aTargetClient = aParser->GetLastTargetClient();
	LobbyChatCommand aCommand = aParser->GetLastChatCommand();
		
	switch(aCommand)
	{
		case LobbyChatCommand_Broadcast:
		case LobbyChatCommand_Emote:
		{
			unsigned short aFlags = aCommand==LobbyChatCommand_Emote?RoutingChatFlag_IsEmote:0;
			if(aParser->GetRoomSpecFlags() & LobbyRoomSpecFlag_Team)
			{
				LobbyClient *myClient = LobbyStaging::GetMyClient();
				if(myClient==NULL)
					return; // not on a team						

				RoutingRecipientList aDestList;
				LobbyClientListItr aClientList(aParser->GetClientList());
				while(aClientList.HasMoreClients())
				{
					LobbyClient *aClient = aClientList.GetNextClient();
					if(myClient->SameTeam(aClient))
						aDestList.push_back(aClient->GetClientId());
				}
				aConnection->SendChat(aParser->GetLastChat(),aFlags | LobbyChatFlag_TeamChat,aDestList);
			}
			else
				aConnection->SendChat(aParser->GetLastChat(),aFlags,aGroupId);

			break;
		}

		case LobbyChatCommand_Whisper: 
			if(aTargetClient!=NULL)
				aConnection->SendChat(aParser->GetLastChat(), RoutingChatFlag_IsWhisper,aTargetClient->GetClientId());
			break;
		

		case LobbyChatCommand_Away:
		{
			LobbyClient *myClient = mServer->GetMyClient();
			if(myClient!=NULL)
			{
				mIsManualAFK = !myClient->IsAway();
				aConnection->SetClientFlags(RoutingClientFlag_IsAway,mIsManualAFK?RoutingClientFlag_IsAway:0,0,0);
			}
			break;
		}

		case LobbyChatCommand_Invite:
		case LobbyChatCommand_Uninvite:
		{
			LobbyClient *aClient = aParser->GetLastTargetClient();
			if(aClient!=NULL)
			{
				if(!InviteClient(aClient,aCommand==LobbyChatCommand_Invite,aParser->GetLastChat()))
					LobbyEvent::DeliverEventTo(new ChatCommandErrorEvent(RoutingLogic_NotCaptain_String),theChatCtrl);
			}
			break;
		}

		case LobbyChatCommand_BecomeModerator:
		{
			LobbyClient *myClient = mServer->GetMyClient();
			if(myClient!=NULL)
			{
				mWantToBeModerator = !myClient->IsModerator();
				aConnection->BecomeModerator(mWantToBeModerator);
			}

			break;
		}

		case LobbyChatCommand_Mute:
			if(aTargetClient!=NULL)
				aConnection->MuteClientByClientId(aGroupId,true,aParser->GetLastInputDuration(),aParser->GetLastChat(),aTargetClient->GetClientId());
			break;

		case LobbyChatCommand_ServerMute:
			if(aTargetClient!=NULL)
				aConnection->MuteClientByClientId(RoutingId_Global,true,aParser->GetLastInputDuration(),aParser->GetLastChat(),aTargetClient->GetClientId());
			break;

		case LobbyChatCommand_Ban:
			if(aTargetClient!=NULL)
				aConnection->BanClientByClientId(aGroupId,true,aParser->GetLastInputDuration(),aParser->GetLastChat(),aTargetClient->GetClientId());
			break;

		case LobbyChatCommand_ServerBan:
			if(aTargetClient!=NULL)
				aConnection->BanClientByClientId(RoutingId_Global,true,aParser->GetLastInputDuration(),aParser->GetLastChat(),aTargetClient->GetClientId());
			break;

		case LobbyChatCommand_Unmute:
			if(aTargetClient!=NULL)
				aConnection->MuteClientByClientId(aGroupId,false,aParser->GetLastInputDuration(),aParser->GetLastChat(),aTargetClient->GetClientId());
			else
				GetBadUsers(true,false,aParser->GetRoomSpecFlags());
			break;

		case LobbyChatCommand_ServerUnmute:
			if(aTargetClient!=NULL)
				aConnection->MuteClientByClientId(RoutingId_Global,false,aParser->GetLastInputDuration(),aParser->GetLastChat(),aTargetClient->GetClientId());
			else
				GetBadUsers(true,true,aParser->GetRoomSpecFlags());
			break;

		case LobbyChatCommand_Ignore:
		{
			if(aTargetClient!=NULL)
				LobbyMisc::SetIgnored(aTargetClient,!aTargetClient->IsIgnored(),LobbyRoomSpecFlag_Internet);
		}
		break;

		case LobbyChatCommand_Block:
		{
			if(aTargetClient!=NULL)
				LobbyMisc::SetBlocked(aTargetClient,!aTargetClient->IsBlocked(),LobbyRoomSpecFlag_Internet);
		}
		break;

		case LobbyChatCommand_Unban:
			GetBadUsers(false,false,aParser->GetRoomSpecFlags());
			break;

		case LobbyChatCommand_ServerUnban:
			GetBadUsers(false,true,aParser->GetRoomSpecFlags());
			break;

		case LobbyChatCommand_Alert:
		{
			LobbyClient *myClient = mServer->GetMyClient();
			if(myClient!=NULL && myClient->IsAdmin())
				aConnection->SendServerAlert(aParser->GetLastChat());
		}
		break;

		case LobbyChatCommand_StartShutdown:
		{
			LobbyClient *myClient = mServer->GetMyClient();
			if(myClient!=NULL && myClient->IsAdmin())
				aConnection->StartServerShutdown(aParser->GetLastChat(),aParser->GetLastInputDuration());
		}
		break;

		case LobbyChatCommand_AbortShutdown:
		{
			LobbyClient *myClient = mServer->GetMyClient();
			if(myClient!=NULL && myClient->IsAdmin())
				aConnection->AbortServerShutdown();
		}
		break;

		case LobbyChatCommand_Warn: 
			if(aTargetClient!=NULL)
				aConnection->SendChat(aParser->GetLastChat(), LobbyChatFlag_IsWarning,aTargetClient->GetClientId());
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::InviteClient(LobbyClient *theClient, bool invite, const GUIString &theText)
{
	if(theClient==NULL)
		return false;

	if(mGameGroup.get()==NULL)
		return false;


	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL || !aGame->IAmCaptain())
		return false;

	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return false;

	theClient->SetIsInvited(invite);
	aConnection->InviteClient(mGameGroup->GetGroupId(),theClient->GetClientId(),invite,theText);
	LobbyEvent::BroadcastEvent(new GroupInvitationEvent(LobbyRoomSpecFlag_Internet,mGameGroup->GetName(),
		mGameGroup->GetGroupId(),theClient,invite,theText,true));

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::NeedLocateFriend(LobbyFriend *theFriend, time_t theTime)
{
	if(theFriend->IsSearching()) // already trying to find him
		return false;

	const LobbyServerMap &aMap = mServerList->GetServerMap();
	if(mServer.get()!=NULL && theFriend->GetServerIP()==mServer->GetIPAddr()) // he's on this server
		return false;
	else if(aMap.empty()) // no servers
		return false; 
	else if(aMap.size()==1 && mServer.get()!=NULL) // only 1 server and we're on it
		return false;
	else if(theTime-theFriend->GetLastLocateTime() < 30) // use cached result
		return false;
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::ReportClientLocation(LobbyClient *theClient, bool reportIfNotFound, int theRoomSpec, bool getUpToDateClient)
{
	if(mServer.get()==NULL || theClient==NULL)
		return false;

	GUIString aChatName;
	GUIString aGameName;

	// Find current client data sructure
	bool stillOnServer = true;
	if(getUpToDateClient && theClient->GetWONUserId()!=0)
	{
		LobbyClient *anUpToDateClient = mServer->GetClientByWONId(theClient->GetWONUserId());
		if(anUpToDateClient!=NULL)
			theClient = anUpToDateClient;
		else
			stillOnServer = false;
	}

	// See if he's in chat or in game
	if(stillOnServer)
	{
		if(theClient->InChat())
		{
			LobbyGroup *aGroup = mServer->GetGroup(theClient->GetChatGroupId());
			if(aGroup!=NULL)
				aChatName = aGroup->GetName();
		}

		if(theClient->InGame())
		{
			LobbyGroup *aGroup = mServer->GetGroup(theClient->GetGameGroupId());
			if(aGroup!=NULL)
				aGameName = aGroup->GetName();
		}
	}

	bool found = !aChatName.empty() || !aGameName.empty();
	if(!found && !reportIfNotFound)
		return false;

	theRoomSpec |= LobbyRoomSpecFlag_Internet;

	LobbyEvent::BroadcastEvent(new AnnounceClientLocationEvent(theClient, aChatName, aGameName, theRoomSpec));
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::ReportFriendLocation(LobbyFriend *theFriend, int theRoomSpec)
{
	if(theFriend==NULL)
		return;

	if(mServer.get()!=NULL && theFriend->GetServerIP()==mServer->GetIPAddr()) // on my server
	{
		LobbyClient *aClient = mServer->GetClientByWONId(theFriend->GetWONUserId());
		if(aClient!=NULL)
		{
			if(ReportClientLocation(aClient,false,theRoomSpec,false))
				return;
		}
	}
	
	LobbyEvent::BroadcastEvent(new AnnounceFriendEvent(theFriend, theRoomSpec, LobbyChangeType_Modify));	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::LocateFriend(LobbyFriend *theFriend, int theRoomSpec)
{
	if(theFriend==NULL)
		return;

	time_t aTime = time(NULL);
	if(!NeedLocateFriend(theFriend,aTime)) // already know where he is
	{
		if(!theFriend->IsSearching())
			ReportFriendLocation(theFriend,theRoomSpec);
	
		return;
	}

	UDPManager *aUDPManager = LobbyMisc::GetUDPManager();
	if(aUDPManager==NULL)
		return;


	// Send a message to all routing servers asking if the user is on the server
	// Get routing server list
	const LobbyServerMap &aMap = mServerList->GetServerMap();
	int aNumOps = 0;
	for(LobbyServerMap::const_iterator anItr = aMap.begin(); anItr != aMap.end(); ++anItr)
	{
		if(mServer.get()!=NULL && mServer->GetIPAddr()==anItr->first) // skip our server
			continue;
			
		// Add the UserId's
		RoutingGetUserStatusOpPtr anOp = new RoutingGetUserStatusOp(anItr->first,aUDPManager);
		anOp->AddWONUserId(theFriend->GetWONUserId());
		RunOp(anOp,new RoutingCompletion(LocateFriendCompletion, this), 10000);

		aNumOps++;
	}
	

	if(aNumOps>0)
	{
		// let friends list know that we're searching
		theFriend->StartSearch(theRoomSpec);
		theFriend->SetNumOps(aNumOps);
		LobbyEvent::BroadcastEvent(new FriendChangedEvent(theFriend,LobbyChangeType_Modify));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::CheckFriendMapSize()
{
	// Can't have more than 255 friends
	if(LobbyPersistentData::GetFriendMap().size()>=255)
	{
		LobbyDialog::DoErrorDialog(RoutingLogic_TooManyFriendsTitle_String,RoutingLogic_TooManyFriendsDetails_String);
		return false;
	}
	else
		return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleAddFriend()
{
	if(!CheckFriendMapSize())
		return;

	mAddFriendDialog->Reset();
	int aResult = LobbyDialog::DoDialog(mAddFriendDialog);
	if(aResult!=ControlId_Ok)
		return;

	ServerContext *anAccountServers = LobbyMisc::GetAccountServers();
	if(anAccountServers==NULL)
		return;

	GetUserIdFromNameOpPtr anOp = new GetUserIdFromNameOp(anAccountServers);
	GUIString aName = mAddFriendDialog->GetName();
	anOp->SetUserName(aName);
//	anOp->SetAuthType(AUTH_TYPE_SESSION,mAuthContext);
	LobbyDialog::DoStatusOp(anOp,AddFriendDialog_StatusTitle_String,AddFriendDialog_StatusDetails_String, CloseStatusDialogType_Success);
	if(!anOp->Succeeded() || anOp->GetUserId()==0)
		return;

	// Store in persistent data
	LobbyFriend *aFriend = LobbyPersistentData::AddFriend(aName, anOp->GetUserId());
	if(aFriend!=NULL)
	{
		if(mServer.get()!=NULL)
		{
			LobbyClient *aClient = mServer->GetClientByWONId(aFriend->GetWONUserId());
			if(aClient!=NULL)
			{
				aClient->SetIsFriend(true);

				aFriend->NotifyOnServer(mServer->GetIPAddr(),mServer->GetName(),0);
				aFriend->UpdateFlagsFromClient(aClient);

				LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,LobbyRoomSpecFlag_Internet,LobbyChangeType_Modify));
			}
			else
			{}// Find the friend?
		}

		LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend,LobbyChangeType_Add));
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::RefreshFriendsList()
{
	UDPManager *aUDPManager = LobbyMisc::GetUDPManager();
	if(aUDPManager==NULL)
		return;

	time_t aTime = time(NULL);

	const LobbyFriendMap &aFriendMap = LobbyPersistentData::GetFriendMap();
	const LobbyServerMap &aServerMap = mServerList->GetServerMap();
	LobbyServerMap::const_iterator aServerItr;

	// Cound the num servers to search
	int aNumServersToSearch = 0;
	for(aServerItr = aServerMap.begin(); aServerItr != aServerMap.end(); ++aServerItr )
	{
		if(mServer.get()!=NULL && mServer->GetIPAddr()==aServerItr->first) // skip our server
			continue;

		aNumServersToSearch++;
	}

	if(aNumServersToSearch==0)
		return;


	// Make the map of user ids to search for
	RoutingGetUserStatusOp::FriendStatusMap aSearchMap;
	for(LobbyFriendMap::const_iterator aFriendItr = aFriendMap.begin(); aFriendItr != aFriendMap.end(); ++aFriendItr)
	{
		LobbyFriend *aFriend = aFriendItr->second;
		if(NeedLocateFriend(aFriend,aTime))
		{
			aFriend->StartSearch();
			aFriend->SetNumOps(aNumServersToSearch);
			aSearchMap[aFriend->GetWONUserId()] = 0;
		}
	}

	if(aSearchMap.empty()) // no searching needed
		return;

	// Now send out the ops
	for(aServerItr = aServerMap.begin(); aServerItr != aServerMap.end(); ++aServerItr )
	{
		if(mServer.get()!=NULL && mServer->GetIPAddr()==aServerItr->first) // skip our server
			continue;
	
		RoutingGetUserStatusOpPtr anOp = new RoutingGetUserStatusOp(aServerItr->first,aUDPManager); 
		anOp->SetFriendStatusMap(aSearchMap);

		RunOp(anOp,new RoutingCompletion(LocateFriendCompletion, this), 10000);	
	}

	// Show querying status in friends list
	LobbyEvent::BroadcastEvent(LobbyEvent_SyncFriendsList);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::RemoveFriend(LobbyFriend *theFriend)
{
	if(theFriend==NULL)
		return;

	if(mServer.get()!=NULL)
	{
		LobbyClient *aClient = mServer->GetClientByWONId(theFriend->GetWONUserId());
		if(aClient!=NULL)
		{
			aClient->SetIsFriend(false);
			LobbyEvent::BroadcastEvent(new ClientChangedEvent(aClient,LobbyRoomSpecFlag_Internet,LobbyChangeType_Modify));
		}
	}

	// Remove from persistent data (only need a valid WONId)
	LobbyEvent::BroadcastEvent(new FriendChangedEvent(theFriend,LobbyChangeType_Delete));
	LobbyPersistentData::RemoveFriend(theFriend->GetWONUserId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void RoutingLogic::HandleFriendAction(FriendActionEvent *theEvent)
{
	LobbyFriend *aFriend = theEvent->mFriend;
	if(aFriend==NULL)
		return;

	LobbyFriendAction anAction = theEvent->mAction;
	int aRoomSpec =  theEvent->mRoomSpecFlags;
	if(aRoomSpec==0)
		aRoomSpec = LobbyRoomSpecFlag_Internet | LobbyRoomSpecFlag_Chat;

	switch(anAction)
	{

		case LobbyFriendAction_ShowMenuTryClient:
			if(mServer.get()!=NULL)
			{
				LobbyClient *aClient = mServer->GetClientByWONId(aFriend->GetWONUserId());
				if(aClient!=NULL)
				{
					ShowClientOptions(aClient,aRoomSpec);
					return;
				}
			}
			// fall through and show the friend menu

		case LobbyFriendAction_ShowMenu: ShowFriendOptions(aFriend, aRoomSpec); break;
		case LobbyFriendAction_Locate: LocateFriend(aFriend, aRoomSpec); break;
		case LobbyFriendAction_RefreshList: RefreshFriendsList(); break;
		case LobbyFriendAction_Remove: RemoveFriend(aFriend); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::TryJoinChatRoom(LobbyServer *theServer, unsigned short theGroupId)
{
	if(theServer==NULL)
		return false;

	AutoKillPendingConnection aKill(this);
	if(!JoinServer(theServer))
		return false;

	if(mChatGroup.get()!=NULL && theServer==mServer && theGroupId==mChatGroup->GetGroupId()) // already in the group
	{
		LobbyDialog::DoErrorDialog(RoutingLogic_AlreadyJoinedTitle_String,RoutingLogic_AlreadyInRoom_String);
		return false;
	}

	if(!JoinGroup(mPendingServer,theGroupId))
		return false;

	SucceedJoinGroup(theGroupId,false);
	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleJoinRoom(JoinRoomEvent *theEvent)
{
	if(JoinPending()) // can't be joining two groups at the same time
		return;

	LobbyServer *aServer = GetServer(theEvent->mServerAddr);
	if(aServer==NULL)
		return;

	int aGroupId = theEvent->mGroupId;
	if(aGroupId==LobbyServerId_Invalid)
		aGroupId = 0;

	if(!TryJoinChatRoom(aServer,aGroupId))
		return;

	if(mRoomDialog->GetParent()!=NULL)
		LobbyScreen::GetWindowManager()->EndDialog(-1,NULL,mRoomDialog);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleJoinFriendsRoom(JoinFriendsRoomEvent *theEvent)
{
	if(JoinPending()) // can't be joining two groups at the same time
		return;

	LobbyFriend *aFriend = theEvent->mFriend;
	if(aFriend==NULL)
		return;

	IPAddr aServerIP = aFriend->GetServerIP();
	if(!aServerIP.IsValid())
		return;

	LobbyServer *aServer = GetServer(aServerIP);
	if(aServer==NULL)
		return;

	AutoKillPendingConnection aKill(this);
	if(!JoinServer(aServer))
		return;

	LobbyClient *aClient = aServer->GetClientByWONId(aFriend->GetWONUserId());
	if(aClient==NULL) // not here anymore
	{
		// FIXME: Show Error Dialog?
		return;
	}
	
	unsigned short aGroupId = 0;
	if(aClient->InChat())
		aGroupId = aClient->GetChatGroupId();

	if(mChatGroup.get()!=NULL && aServer==mServer && aGroupId==mChatGroup->GetGroupId()) // already in the group
	{
		LobbyDialog::DoErrorDialog(RoutingLogic_AlreadyJoinedTitle_String,RoutingLogic_AlreadyInRoom_String);
		return;
	}
		
	if(!JoinGroup(aServer,aGroupId))
		return;

	SucceedJoinGroup(aGroupId,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleJumpToRoom(JumpToRoomEvent *theEvent)
{
	if(JoinPending()) // can't be joining two groups at the same time
		return;

	if(mServer.get()==NULL || mServer->GetIPAddr()!=theEvent->mIPAddr)
		return;


	LobbyGroup *aGroup = mServer->GetGroup(theEvent->mGroupId);
	if(aGroup==NULL)
		return;

	if(aGroup->IsGameRoom())
	{
		LobbyGame *aGame = mGameList->GetGameById(theEvent->mGroupId);
		if(aGame==NULL || aGame->GetName()!=theEvent->mRoomName)
			return;

		LobbyStaging::TryJoinGame(aGame);
	}
	else
		TryJoinChatRoom(mServer,theEvent->mGroupId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* RoutingLogic::GetQuickplayGame(LobbySkillLevel theSkillLevel, unsigned short theMaxPing)
{
	// look for game which are not restricted (invite only, password protected, etc),
	// and have room for at least one more player
	// select a game which is 1) the same skill level as that requested, or 2) open, or
	// 3) -1 level from that requested, or 4) +1 level from that requested
	// If fail to join a game, try another
	// Use an 'attempting to connect' dialog with a cancel button
	// If no games can be found that fit the criteria, display a failure message
	unsigned short aPingCeiling = 10000;
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig!=NULL)
		aPingCeiling = aConfig->mPingCeiling;

	if(aPingCeiling==0 || aPingCeiling>LobbyPing_Max)
		aPingCeiling = LobbyPing_Max;

	typedef std::multimap<unsigned short,LobbyGame*> SortMap;
	SortMap aSortMap;

	LobbyGame* aGame = NULL;
	int aValue = 0;
	mGameList->Rewind();
	while(mGameList->HasMoreGames())
	{
		aGame = mGameList->NextGame();
		if(!aGame->HasPassword() && !aGame->IsAskToJoin() && aGame->IsOkToJoin())
		{
			if(aGame->GetPing()<aPingCeiling)
			{
				aValue = 40000;
				LobbySkillLevel aSkillLevel = aGame->GetSkillLevel();	// four skill levels + 1 for 'open'
				if(aSkillLevel==theSkillLevel)
					aValue = 0;
				else if(aSkillLevel==LobbySkillLevel_None)
					aValue = 10000;
				else if(aSkillLevel+1==theSkillLevel)
					aValue = 20000;
				else if(aSkillLevel-1==theSkillLevel)
					aValue = 30000;

				if(aValue<40000)
				{
					if(aGame->GetPing()<theMaxPing)
					{
						aValue += aGame->GetPing();
						aSortMap.insert(SortMap::value_type(aValue, aGame));
					}
				}
			}
			else if(aGame->GetPing()==LobbyPing_None)
				LobbyMisc::QueuePing(aGame);
		}
	}

	SortMap::iterator aSortItr = aSortMap.begin();
	if(aSortItr!=aSortMap.end())
		return aSortItr->second;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleQuickPlay(QuickPlayEvent*theEvent)
{
	if(!LobbyStaging::LeaveGame(true))
		return;

	if(mServer.get()==NULL)
		JoinDefaultRoom(); 

	if(mServer.get()==NULL)
		return;

	// Wait for game list
	if(!mGotGameList)
	{
		mGetGameListOp = new AsyncOp;
		LobbyDialog::DoStatusOp(mGetGameListOp,RoutingLogic_GettingGameListTitle_String,RoutingLogic_GettingGameListDetails_String,CloseStatusDialogType_Success);
		mGetGameListOp = NULL;
		if(!mGotGameList)
			return;
	}

	// Keep looking for games while there are outstanding pings.  Try to join the first appropriate game
	// that we find
	while(true)
	{
		if(mServer.get()==NULL) // if we got disconnected while in the game list or ping dialog
			return;

		LobbyGame *aGame = GetQuickplayGame(theEvent->mSkillLevel,1000);
		if(aGame!=NULL)
		{
			LobbyStaging::TryJoinGame(aGame);
			return;
		}

		if(LobbyMisc::GetNumPingsToDo()>0)
		{
			AsyncOpPtr anOp = new AsyncOp;
			LobbyDialog::DoStatusOp(anOp,RoutingLogic_CollectingPingsTitle_String,RoutingLogic_CollectingPingsDetails_String,CloseStatusDialogType_Any,1000);
			if(anOp->Killed())
				return;
		}
		else
			break;
	}

	int aResult = LobbyDialog::DoYesNoDialog(GameStagingLogic_NoQuickPlayGameTitle_String, GameStagingLogic_NoQuickPlayGameDetails_String);
	if(aResult==ControlId_Ok)
		LobbyStaging::TryHostGame();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleRefreshGameList()
{
	if(LobbyScreen::OnInternetScreen())
	{
		mGameList->Rewind();
		while(mGameList->HasMoreGames())
		{
			LobbyGame *aGame = mGameList->NextGame();
			if(!aGame->IsFiltered())
				LobbyMisc::QueuePing(aGame);
		}

		LobbyEvent::BroadcastEvent(LobbyEvent_SyncFilteredGameList);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyServer* RoutingLogic::GetDefaultServer()
{
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig==NULL)
		return NULL;

	LobbyServer* aSrvP = NULL;
	LobbyServer* aLangP = NULL;

	// Find Server (either the last one you were on, or the one with the same language as you)
	const LobbyServerMap &aServerMap = mServerList->GetServerMap();
	LobbyServerMap::const_iterator aServerItr = aServerMap.begin();
	while(aServerItr!=aServerMap.end())
	{
		LobbyServer *aServer = aServerItr->second;
		if (aServer->GetName() == LobbyPersistentData::GetDefLobbyServer())
			return aServer; // found it

		if (aSrvP==NULL || aServer->GetNumUsers() > aSrvP->GetNumUsers()) 
			aSrvP = aServer;

		if (aServer->SupportsLanguage(aConfig->mLanguage))
		{
			if(aLangP==NULL || aServer->GetNumUsers() > aLangP->GetNumUsers())
				aLangP = aServer;
		}

		++aServerItr;
	}

	if(aLangP!=NULL)
		return aLangP; // pick server with correct language over just any server
	else
		return aSrvP;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::JoinDefaultRoom(bool anonymousJoin)
{

	LobbyServer* aSrvP = GetDefaultServer();
	if(aSrvP==NULL)
		return false;

	AutoKillPendingConnection aKill(this);
	if(!JoinServer(aSrvP, anonymousJoin))
		return false;

	// Find group on the server
	LobbyGroup*  aGrpP = NULL;
	const LobbyGroupMap &aGroupMap = aSrvP->GetGroupMap();
	LobbyGroupMap::const_iterator aGroupItr = aGroupMap.begin();
	while(aGroupItr!=aGroupMap.end())
	{
		LobbyGroup *aGroup = aGroupItr->second;
		if(aGroup->IsOpenChatRoom())
		{
			if (aGroup->GetName() == LobbyPersistentData::GetDefLobbyGroup())
			{
				aGrpP = aGroup;
				break;
			}
			else if(aGrpP==NULL)
				aGrpP = aGroup;
			else if(aGroup->GetNumUsers()>aGrpP->GetNumUsers() && aGroup->GetNumUsers()<50) // fixme: need configurable threshold instead of 50
				aGrpP = aGroup;
		}
		++aGroupItr;
	}

	// Join server
	int aGroupId = 0; 
	if(aGrpP!=NULL)
		aGroupId = aGrpP->GetGroupId();

	if(!JoinGroup(mPendingServer,aGroupId))
		return false;

	SucceedJoinGroup(aGroupId,false);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::EnterInternetScreen(bool joinDefaultRoom)
{
	GetRooms();
	if(mServerList->GetServerMap().empty())
	{
		LobbyDialog::DoErrorDialog(RoutingLogic_NoServersTitle_String,RoutingLogic_NoServersDetails_String);
		return;
	}

	if(!joinDefaultRoom)
		DoRoomDialog();
	else if(!JoinDefaultRoom())
	{
		if(LobbyDialog::UserCanceledLastStatusOp())
		{
			LobbyStaging::Logout(false);
			return;
		}
		else
			DoRoomDialog();
	}

	LobbyScreen::ShowInternetScreen();
	if(LobbyPersistentData::GetLobbySoundEffects() && !LobbyPersistentData::GetLobbyMusic())
		LobbyMisc::PlaySound(LobbyGlobal_SierraLogon_Sound);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::HandleQueryServerGroups(QueryServerGroupsEvent* theEvent)
{
	LobbyServer *aServer = GetServer(theEvent->mServerAddr);
	if(aServer==NULL)
		return;

	time_t aTime = time(NULL);
	if(aServer->IsConnected() || aServer->IsQueryingGroups() || aTime-aServer->GetLastGroupQueryTime()<30)
		return;

	aServer->ClearGroups();
	LobbyEvent::BroadcastEvent(new ServerChangedEvent(aServer,LobbyChangeType_Modify));

	aServer->SetQueryingGroups(true);
	aServer->SetLastGroupQueryTime(aTime);
	
	RoutingGetGroupListReqOpPtr anOp = new RoutingGetGroupListReqOp(aServer->GetIPAddr());
	anOp->SetFlags(RoutingGetGroupList_IncludeGroupName | RoutingGetGroupList_IncludeGroupFlags | RoutingGetGroupList_IncludeMemberCount | RoutingGetGroupList_IncludeClientCount);
	RunOpTrack(anOp, new RoutingCompletion(GetGroupListReqCompletion,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::JoinPending()
{
	return mPendingServer.get()!=NULL;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::CreateGroup(LobbyServer *theServer, const GUIString &theName, const GUIString &thePassword, int theRoomFlags, unsigned short &theGroupId)
{
	if(theServer==NULL)
		return false;

	RoutingConnectionPtr aConnection = theServer->GetConnection();
	if(aConnection.get()==NULL)
		return false;

	RoutingCreateGroupOpPtr anOp = new RoutingCreateGroupOp(aConnection);
	anOp->SetGroupPassword(thePassword);
	anOp->SetGroupName(theName);
	anOp->SetGroupFlags(theRoomFlags);
	anOp->SetJoinFlags(RoutingCreateGroupJoinFlag_JoinAsPlayer);
	anOp->SetLeaveGroupOnCancel(true);
//	anOp->SetMaxPlayers(theMaxPlayers);

	LobbyDialog::DoStatusOp(anOp,RoutingLogic_CreateGroupTitle_String,RoutingLogic_CreateGroupDetails_String,CloseStatusDialogType_Success);
	if(anOp->Succeeded())
	{
		theGroupId = anOp->GetGroupId();
		return true;
	}

	if(anOp->Killed())
	{
		// FIXME: need to leave group if the user hit cancel but the routing server finished creating it.
		// but I don't know the id of the group :(
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::JoinGroup(LobbyServer *theServer, int theGroupId)
{
	if(theServer==NULL)
		return false;

	RoutingConnectionPtr aConnection = theServer->GetConnection();
	if(aConnection.get()==NULL)
		return false;

	LobbyGroupPtr aGroup = theServer->GetGroup(theGroupId);
	if(aGroup.get()==NULL)
	{
		LobbyDialog::ShowStatus(RoutingLogic_JoinGroupTitle_String,WS_RoutingServG2_GroupDoesNotExist);
		return false;
	}

	GUIString aPassword;
	if(aGroup->IsPasswordProtected())
	{
		mPasswordDialog->Reset();
		int aResult = LobbyDialog::DoDialog(mPasswordDialog);
		if(aResult!=ControlId_Ok)
			return false;

		aPassword = mPasswordDialog->GetPassword();
	}

	if(!aConnection->IsConnected()) // not connected anymore
		return false;

	RoutingJoinGroupOpPtr anOp = new RoutingJoinGroupOp(aConnection);
	anOp->SetGroupId(theGroupId);
	anOp->SetGroupPassword(aPassword);

	CloseStatusDialogType aCloseType = CloseStatusDialogType_Success; //showDialogError?CloseStatusDialogType_Success:CloseStatusDialogType_Any;

	while(true)
	{
		if(aGroup.get()!=NULL && aGroup->IsAskToJoin())
			LobbyDialog::DoStatusOp(anOp,RoutingLogic_AskingCaptainTitle_String,RoutingLogic_AskingCaptainDetails_String,aCloseType); 
		else
			LobbyDialog::DoStatusOp(anOp,RoutingLogic_JoinGroupTitle_String,RoutingLogic_JoinGroupDetails_String,aCloseType);

		if(anOp->Succeeded())
			break;

		if(!aConnection->IsConnected()) // not connected anymore
			return false;

		if(anOp->Killed()) // user canceled
		{
			aConnection->CancelJoinGroup(theGroupId,true);
			return false;
		}
		else if(anOp->GetStatus()==WS_RoutingServG2_InvalidPassword)
		{
			mPasswordDialog->Reset();
			int aResult = LobbyDialog::DoDialog(mPasswordDialog);
			if(aResult!=ControlId_Ok)
				return false;

			anOp->SetGroupPassword(mPasswordDialog->GetPassword());
		}
		else if(anOp->GetStatus()==WS_RoutingServG2_ClientAlreadyInGroup) // Fixme: Is this bad?
			return true;
		else
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RoutingLogic::JoinServer(LobbyServer *theServer, bool anonymousJoin)
{
	if(theServer==NULL)
		return false;

	mPendingServer = theServer;
	if(theServer==mServer.get() && theServer->IsConnected()) // already on the server
		return true;

	if(mGameGroup.get()!=NULL && !LobbyStaging::LeaveGame(true))
		return false;

	RoutingConnectionPtr aConnection = new RoutingConnection;
	theServer->SetConnection(aConnection);

	AuthContext *anAuth = LobbyMisc::GetAuthContext();
	if(anAuth==NULL)
		return false;

	CloseStatusDialogType aCloseType = CloseStatusDialogType_Success; //showDialogError?CloseStatusDialogType_Success:CloseStatusDialogType_Any;

	aConnection->SetAuth(anAuth);

	aConnection->SetRoutingCompletion(RoutingOp_ClientJoinedServer, new RoutingOpCompletion(ClientJoinedServerCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ClientLeftServer, new RoutingOpCompletion(ClientLeftServerCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ClientJoinedGroup, new RoutingOpCompletion(ClientJoinedGroupCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ClientLeftGroup, new RoutingOpCompletion(ClientLeftGroupCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_PeerChat, new RoutingOpCompletion(ChatCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_PeerData, new RoutingOpCompletion(PeerDataCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ClientFlagsChanged, new RoutingOpCompletion(ClientFlagsChangedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_MemberFlagsChanged, new RoutingOpCompletion(MemberFlagsChangedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_GroupCreated, new RoutingOpCompletion(GroupCreatedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_GroupDeleted, new RoutingOpCompletion(GroupDeletedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_GroupCaptainChanged, new RoutingOpCompletion(CaptainChangedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_DataObjectCreated, new RoutingOpCompletion(DataObjectCreatedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_DataObjectDeleted, new RoutingOpCompletion(DataObjectDeletedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_GroupInvitation, new RoutingOpCompletion(GroupInvitationCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_GroupJoinAttempt, new RoutingOpCompletion(GroupJoinAttemptCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_YouWereBanned, new RoutingOpCompletion(ClientBannedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_YouWereMuted, new RoutingOpCompletion(ClientMutedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ServerAlert, new RoutingOpCompletion(ServerAlertCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ServerShutdownStarted, new RoutingOpCompletion(ServerShutdownStartedCompletion,this));
	aConnection->SetRoutingCompletion(RoutingOp_ServerShutdownAborted, new RoutingOpCompletion(ServerShutdownAbortedCompletion,this));

/*
	aConnection->SetRoutingCompletion(RoutingOp_GroupMemberCount, new RoutingOpCompletion(GroupMemberCountCompletion,this));
*/
	ServerConnectionConnectOpPtr aConnectOp = new ServerConnectionConnectOp(aConnection,theServer->GetIPAddr());
	LobbyDialog::DoStatusOp(aConnectOp,RoutingLogic_ConnectTitle_String,RoutingLogic_ConnectDetails_String,aCloseType);
	if(!aConnectOp->Succeeded())
		return false;

	unsigned long aRegisterClientFlags = RoutingRegisterClientFlag_LoginTypeCertificate | RoutingRegisterClientFlag_GetClientListEx;
	if(mWantToBeModerator)
		aRegisterClientFlags |= RoutingRegisterClientFlag_InitialModeratorStatus;

	RoutingRegisterClientOpPtr aRegisterOp = new RoutingRegisterClientOp(aConnection);
	aRegisterOp->SetRegisterFlags(aRegisterClientFlags);
	aRegisterOp->SetReconnectId(LobbyPersistentData::GetReconnectId(theServer->GetIPAddr()));

	// Set client flags, only one right now
	unsigned long clientFlags=0;
	if (!LobbyPersistentData::GetFilterDirtyWords())
		clientFlags |= RoutingClientFlag_DirtyWordFilteringOff;
	if (LobbyPersistentData::GetAnonymousToFriends() || anonymousJoin)
		clientFlags |= RoutingClientFlag_IsAnonymous;

	aRegisterOp->SetClientFlags(clientFlags);

	LobbyDialog::DoStatusOp(aRegisterOp,RoutingLogic_RegisterTitle_String,RoutingLogic_RegisterDetails_String,aCloseType);
	if(!aRegisterOp->Succeeded())
		return false;

	LobbyPersistentData::AddReconnectId(theServer->GetIPAddr(), aRegisterOp->GetReconnectId());
	theServer->UpdateClients(aRegisterOp->GetClientMap());
	theServer->SetMyClientId(aRegisterOp->GetClientId());

	aConnection->SetCloseCompletion(new RoutingCompletion(CloseCompletion,this));

	// Handle turning temp anonymous off
	if(anonymousJoin && !LobbyPersistentData::GetAnonymousToFriends())
		aConnection->SetClientFlags(RoutingClientFlag_IsAnonymous,0,0,0);

	RoutingGetGroupListOpPtr aGetGroupListOp = new RoutingGetGroupListOp(aConnection,RoutingGetGroupList_IncludeAll);
	LobbyDialog::DoStatusOp(aGetGroupListOp,RoutingLogic_QueryGroupsTitle_String,RoutingLogic_QueryGroupsDetails_String,aCloseType);
	if(!aGetGroupListOp->Succeeded())
		return false;

	theServer->UpdateGroups(aGetGroupListOp->GetGroupMap(),true);
	LobbyEvent::BroadcastEvent(new ServerChangedEvent(theServer,LobbyChangeType_Modify));

	theServer->UpdateFriends();
	LobbyEvent::BroadcastEvent(LobbyEvent_SyncFriendsList);

//	mIsManualAFK = false;
//	BroadcastEvent(new RefreshFriendsEvent(this));
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::KillConnection()
{
	if(mServer.get()!=NULL)
	{
		mServer->KillConnection();
		LobbyMisc::KillPings(); // kill any pings to the old server

		LobbyEvent::BroadcastEvent(new LeftRoomEvent(LobbyRoomSpecFlag_Internet,false,false));
	}

	if(mGetGameListOp.get()!=NULL)
		mGetGameListOp->Kill();

	mGotGameList = false;
	mServer = NULL;
	mChatGroup = NULL;

	mGameList->Clear();
	LobbyEvent::BroadcastEvent(new SyncGameListEvent(mGameList));

	if(mGameGroup.get()!=NULL)
	{
		mGameGroup = NULL;
		LobbyStagingPrv::NotifyGameDisconnect();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::KillPendingConnection()
{
	if(mPendingServer.get()!=NULL && mPendingServer!=mServer)
		mPendingServer->KillConnection();
	
	mPendingServer = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* RoutingLogic::NetGetMyClient()
{
	if(mServer.get()==NULL)
		return NULL;
	else
		return mServer->GetMyClient();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetSendGameMessageToCaptain(const ByteBuffer *theMsg)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL || mGameGroup.get()==NULL)
		return;

	aConnection->SendData(theMsg,0,mGameGroup->GetCaptainId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetBroadcastGameMessage(const ByteBuffer *theMsg)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL || mGameGroup.get()==NULL)
		return;

	aConnection->SendData(theMsg,0,mGameGroup->GetGroupId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetSendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL || mGameGroup.get()==NULL)
		return;

	aConnection->SendData(theMsg,0,theClient->GetClientId());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetUpdateGameSummary(LobbyGame *theGame)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL || mGameGroup.get()==NULL)
		return;

	WriteBuffer aBuf;
	theGame->WriteSummary(aBuf);

	aConnection->CreateDataObject(RoutingCreateDataObjectFlag_ReplaceIfExists,mGameGroup->GetGroupId(),
		mGameObjectName,L"",aBuf.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetDeleteGameSummary(LobbyGame *theGame)
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL || mGameGroup.get()==NULL)
		return;

	aConnection->DeleteDataObject(mGameGroup->GetGroupId(),mGameObjectName,L"");
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetSuspendLobby()
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection==NULL)
		return;

	// Leave ChatGroup
	if(mChatGroup.get()!=NULL)
	{
		aConnection->LeaveGroup(mChatGroup->GetGroupId());
		mChatGroup = NULL;
		LobbyEvent::BroadcastEvent(new LeftRoomEvent(LobbyRoomSpecFlag_Internet | LobbyRoomSpecFlag_Chat));
	}
	
	// Turn off all messages except peer data which lets you respond to late requests to join the server 
	// or pings if ping in progress is on
	int anAsyncMessageFlags = 0xffffffff & ~RoutingAsyncMessageFlag_PeerData;
	aConnection->SetClientFlags(0,0,0xffffffff,anAsyncMessageFlags); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetResumeLobby()
{
	RoutingConnection *aConnection = GetConnection();
	if(aConnection!=NULL)
		aConnection->SetClientFlags(RoutingClientFlag_IsAnonymous,RoutingClientFlag_IsAnonymous,0,0);
	
	LobbyStaging::LeaveGame(false);
	KillConnection();
	JoinDefaultRoom(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::NetLogout()
{
	AuthContext *aContext = LobbyMisc::GetAuthContext();
	if(aContext!=NULL)
		aContext->SetDoAutoRefresh(false);

	mWantToBeModerator = false;
	mCreateRoomDialog->Reset(true); // get rid of name and password if user had typed one in

	KillPendingConnection();
	KillConnection();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::SucceedJoinGroup(unsigned short theGroupId, bool isGame)
{
	LobbyGroup *anOldGroup = isGame?mGameGroup:mChatGroup;
	if(mPendingServer.get()!=NULL && mPendingServer!=mServer)
	{
		KillConnection();
		mIsManualAFK = mIsAutoAFK = false;
		mGotGameList = false;
		mServer = mPendingServer;
		RoutingConnection *aConnection = GetConnection();
		if(aConnection!=NULL) // subscribe to games after we've successfully joined the server
			aConnection->SubscribeDataObject(RoutingId_Global, mGameObjectName, 0, new RoutingOpCompletion(ReadDataObjectCompletion,this));
	}
	else if(anOldGroup!=NULL && mServer.get()!=NULL && mServer->GetConnection()!=NULL) // need to leave the old group
		mServer->GetConnection()->LeaveGroup(anOldGroup->GetGroupId());

	if(mServer.get()==NULL)
		return;

	LobbyGroup *aNewGroup = mServer->GetGroup(theGroupId);
	if(isGame)
		mGameGroup = aNewGroup;
	else
		mChatGroup = aNewGroup;

	if(aNewGroup!=NULL)
	{	
		if(aNewGroup->IsOpenChatRoom() && mServer.get()!=NULL)
			LobbyPersistentData::SetDefLobbyRoom(mServer->GetName(), aNewGroup->GetName());

		int aSpecFlags = LobbyRoomSpecFlag_Internet;
		aSpecFlags |= isGame?LobbyRoomSpecFlag_Game:LobbyRoomSpecFlag_Chat;

		LobbyEvent::BroadcastEvent(new JoinedRoomEvent(mServer->GetName(), aNewGroup->GetName(), 
			aSpecFlags, mServer->GetMyClientId(), 
			aNewGroup->GetClientList()));
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingLogic::SyncClientWithFriends(LobbyClient *theClient)
{
	if(!theClient->IsFriend())
		return;

	LobbyFriend *aFriend = LobbyPersistentData::GetFriend(theClient->GetWONUserId());
	if(aFriend==NULL)
		return;

	aFriend->UpdateFlagsFromClient(theClient);

	LobbyEvent::BroadcastEvent(new FriendChangedEvent(aFriend, LobbyChangeType_Modify));
}

