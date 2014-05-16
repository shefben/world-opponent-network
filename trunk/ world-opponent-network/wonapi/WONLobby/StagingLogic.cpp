#include "StagingLogic.h"

#include "CreateGameDialog.h"
#include "LobbyClient.h"
#include "LobbyDialog.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyGameMsg.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyPlayer.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyStagingPrv.h"
#include "WONLobby.h"

#include "WONCommon/ReadBuffer.h"
#include "WONCommon/WriteBuffer.h"
#include "WONGUI/GUIConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StagingLogic::StagingLogic()
{
	mJoinedGame = false;
	mLastReadySoundTime = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StagingLogic::~StagingLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::Init(ComponentConfig *theConfig)
{
	WONComponentConfig_Get(theConfig,mCreateGameDialog,"CreateGameDialog");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleJoinGameRequest(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(theSender->IsPlayer())
		return;

	ByteBufferPtr aReply = mGame->HandleJoinGameRequest(theMsg,theSender);
	if(aReply.get()==NULL)
		return;

	SendGameMessageToClient(theSender,aReply);
	
	LobbyPlayer *aPlayer = theSender->GetPlayer();
	if(aPlayer!=NULL) // client successfully joined the game, tell everyone else
	{
		WriteBuffer aPlayerJoined;
		aPlayerJoined.AppendByte(LobbyGameMsg_PlayerJoined);
		aPlayerJoined.AppendShort(theSender->GetClientId());
		aPlayer->WriteData(aPlayerJoined);
		BroadcastGameMessage(aPlayerJoined.ToByteBuffer());
		LobbyStaging::UpdateGameSummary();
		return;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleJoinGameReply(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true))
		return;

	mJoinGameReply = mGame->HandleJoinGameReply(theMsg);
	if(mJoinGameOp.get()!=NULL)
		mJoinGameOp->Finish(WS_Success);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandlePlayerJoined(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true))
		return;

	unsigned short aClientId = theMsg.ReadShort();
	LobbyClient *aClient = mGame->GetClient(aClientId);
	if(aClient==NULL)
		return;

	if(!aClient->IsPlayer()) 
	{
		LobbyPlayerPtr aPlayer = LobbyPlayer::CreatePlayer();
		if(!aPlayer->ReadData(theMsg))
			return;

		aClient->SetPlayer(aPlayer);
	}
	else
	{
		// already know about him (can happen if we're the captain or it's our client right after we joined)
		if(!IAmCaptain()) // already got ourselves in the join game reply
			return;
	}

	LobbyEvent::BroadcastEvent(new PlayerChangedEvent(aClient,LobbyChangeType_Add));
	if(aClient->SameTeam(mMyClient))
		LobbyEvent::BroadcastEvent(new PlayerTeamChangedEvent(aClient,LobbyChangeType_Add));

	mGame->HandlePlayerJoined(aClient);
	if(LobbyPersistentData::GetStagingSoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_PlayerJoined_Sound);

	if(mEveryoneReadySoundState && !aClient->IsPlayerReady())
		mEveryoneReadySoundState = false;

	UpdateStartButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleReadyRequest(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!IAmCaptain() || !theSender->IsPlayer())
		return;

	bool isReady = theMsg.ReadBool();
	if(theSender->IsPlayerReady()==isReady) // no change needed
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_PlayerReady);
	aMsg.AppendShort(theSender->GetClientId());
	aMsg.AppendBool(isReady);
	BroadcastGameMessage(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandlePlayerReady(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true))
		return;


	unsigned short aClientId = theMsg.ReadShort();
	bool isReady = theMsg.ReadBool();
	
	LobbyClient *aClient = mGame->GetClient(aClientId);
	if(aClient==NULL)
		return;

	LobbyPlayer *aPlayer = aClient->GetPlayer();
	if(aPlayer==NULL)
		return;

	aPlayer->SetReady(isReady);

	CheckPlayReadySound();
	
	LobbyEvent::BroadcastEvent(new PlayerChangedEvent(aClient,LobbyChangeType_Modify));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandlePingChangedRequest(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!IAmCaptain())
		return;

	unsigned short aPing = theMsg.ReadShort();

	LobbyPlayer *aPlayer = theSender->GetPlayer();
	if(aPlayer==NULL)
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_PingChanged);
	aMsg.AppendShort(theSender->GetClientId());
	aMsg.AppendShort(aPing);
	BroadcastGameMessage(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandlePingChanged(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true))
		return;

	unsigned short aClientId = theMsg.ReadShort();
	unsigned short aPing = theMsg.ReadShort();

	LobbyClient *aClient = mGame->GetClient(aClientId);
	if(aClient==NULL)
		return;

	LobbyPlayer *aPlayer = aClient->GetPlayer();
	if(aPlayer==NULL)
		return;

	aPlayer->SetPing(aPing);
	LobbyEvent::BroadcastEvent(new PlayerChangedEvent(aClient,LobbyChangeType_Modify));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleDissolveGame(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true) || IAmCaptain())
		return;

	LeaveGame(false);
	LobbyDialog::DoErrorDialog(RoutingLogic_GameDissolvedTitle_String, RoutingLogic_GameDissolvedDetails_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SetEveryoneUnready()
{
	if(mGame.get()==NULL)
		return;

	LobbyPlayerList aList = mGame->GetClientList();
	while(aList.HasMorePlayers())
	{
		LobbyClient *aClient = NULL;
		LobbyPlayer *aPlayer = aList.GetNextPlayer(&aClient);
		if(aPlayer!=NULL && aClient!=NULL)
		{
			aPlayer->SetReady(false);
	
			// Fixme, Make SyncPlayerList event
			LobbyEvent::BroadcastEvent(new PlayerChangedEvent(aClient,LobbyChangeType_Modify));
		}
	}

	CheckPlayReadySound();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SetGameInProgress(bool inProgress)
{
	if(mGame.get()==NULL)
		return;

	if(mGame->InProgress()==inProgress)
		return;

	if(!IAmCaptain())
		return;

	mGame->SetInProgress(inProgress);
	LobbyStagingPrv::NetCloseGame(inProgress);
	LobbyStaging::UpdateGameSummary();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleStartGameMsg(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true))
		return;

	if(IAmCaptain())
	{
		SetGameInProgress(true);
		WONLobby::EndLobby(LobbyStatus_HostGame);
	}
	else
	{
//		if(LobbyPersistentData::GetStagingSoundEffects())
//			LobbyGlobal_AllReady_Sound->Play();

		WONLobby::EndLobby(LobbyStatus_JoinGame);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleClientKicked(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(!theSender->IsCaptain(true))
		return;

	unsigned short aClientId = theMsg.ReadShort();
	bool isBan = theMsg.ReadBool();

	LobbyClient *aClient = mGame->GetClient(aClientId);
	if(aClient==NULL)
		return;

	NotifyClientKicked(aClient,isBan);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleGameMessage(ReadBuffer &theMsg, LobbyClient *theSender)
{
	if(mGame.get()==NULL || theSender==NULL)
		return;		
	try
	{
		unsigned char aMsgType = theMsg.ReadByte();
		if(mJoinGameReply==LobbyGameStatus_None && aMsgType!=LobbyGameMsg_JoinReply) // not in game yet
			return;

		switch(aMsgType)
		{
			case LobbyGameMsg_JoinRequest: HandleJoinGameRequest(theMsg,theSender); break;
			case LobbyGameMsg_JoinReply: HandleJoinGameReply(theMsg,theSender); break;
			case LobbyGameMsg_PlayerJoined: HandlePlayerJoined(theMsg,theSender); break;
			case LobbyGameMsg_ReadyRequest: HandleReadyRequest(theMsg,theSender); break;
			case LobbyGameMsg_PlayerReady: HandlePlayerReady(theMsg,theSender); break;
			case LobbyGameMsg_DissolveGame: HandleDissolveGame(theMsg,theSender); break;
			case LobbyGameMsg_StartGame: HandleStartGameMsg(theMsg,theSender); break;
			case LobbyGameMsg_PingChangedRequest: HandlePingChangedRequest(theMsg,theSender); break;
			case LobbyGameMsg_PingChanged: HandlePingChanged(theMsg,theSender); break;
			case LobbyGameMsg_ClientKicked: HandleClientKicked(theMsg,theSender); break;

			default:
				mGame->HandleGameMessage(aMsgType,theMsg,theSender);
		}
	}
	catch(ReadBufferException&)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleGameMessage(const ByteBuffer *theMsg, LobbyClient *theSender)
{
	if(theMsg==NULL)
		return;

	ReadBuffer aMsg(theMsg->data(),theMsg->length());
	HandleGameMessage(aMsg, theSender);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SendGameMessageToCaptain(const ByteBuffer *theMsg)
{
	if(mGame.get()==NULL)
		return;

	if(IAmCaptain())
		HandleGameMessage(theMsg,mMyClient);
	else
		LobbyStagingPrv::NetSendGameMessageToCaptain(theMsg);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::BroadcastGameMessage(const ByteBuffer *theMsg)
{
	if(mGame.get()==NULL || !IAmCaptain())
		return;

	HandleGameMessage(theMsg,mMyClient);
	LobbyStagingPrv::NetBroadcastGameMessage(theMsg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg)
{
	if(mGame.get()==NULL || !IAmCaptain())
		return;

	LobbyStagingPrv::NetSendGameMessageToClient(theClient,theMsg);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StagingLogic::IAmCaptain()
{
	return mGame.get()!=NULL && mGame->IAmCaptain();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StagingLogic::IAmReady()
{
	return mMyClient.get()!=NULL && mMyClient->IsPlayerReady();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleControlEvent(int theControlId)
{
	switch(theControlId)
	{
		case ID_HostGame: TryHostGame(); break;
		case ID_LeaveGame: LeaveGame(true); break;
		case ID_Logout: Logout(true); break;
		case ID_StartGame: HandleStartGameButton(); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_GameAction: HandleGameAction((GameActionEvent*)theEvent); break;
		case LobbyEvent_JoinGame: HandleJoinGame((JoinGameEvent*)theEvent); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "GameOptionsPopup.h"
void StagingLogic::ShowGameActionMenu(LobbyGame *theGame)
{
	if(theGame==NULL)
		return;

	GameOptionsPopupPtr aPopup = new GameOptionsPopup;
	aPopup->Init(theGame);
	LobbyDialog::DoPopup(aPopup,PopupFlag_StandardPopup);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleGameAction(GameActionEvent *theEvent)
{
	LobbyGame *aGame = theEvent->mGame;
	if(aGame==NULL)
		return;

	switch(theEvent->mAction)
	{
		case LobbyGameAction_ShowMenu: ShowGameActionMenu(aGame); break;
		case LobbyGameAction_Join: TryJoinGame(aGame); break;

		case LobbyGameAction_Ping: 
			LobbyMisc::QueuePing(aGame,true);
			LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Modify));						
			break;
		
		case LobbyGameAction_QueryDetails: 
		case LobbyGameAction_ShowDetails: 
			LobbyMisc::QueueDetailQuery(aGame);
			aGame->SetDetailsVisible(true);
			LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Modify));			
			break;
				
		case LobbyGameAction_HideDetails: 
			aGame->SetDetailsVisible(false);
			LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Modify));
			break;

		case LobbyGameAction_Remove:
			if(aGame->GetGameType()==LobbyGameType_DirectConnect)
				LobbyEvent::DeliverLogicEvent(new ChangeDirectConnectEvent(LobbyChangeType_Delete,aGame));
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StagingLogic::Logout(bool confirm)
{
	if(!LeaveGame(confirm))
		return false;

	mCreateGameDialog->Reset(true);
	LobbyStagingPrv::NetLogout();
	LobbyMisc::KillPings();
	LobbyScreen::ShowLoginScreen();
	LobbyPersistentData::WriteGlobal(false); // in order to remember users settings for the login screen (e.g. whether or not to play button clicks)
	LobbyPersistentData::WriteUser(false);

	LobbyGlobal_SierraLogon_Sound->Stop();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SendReadyRequest(bool isReady)
{
	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_ReadyRequest);
	aMsg.AppendBool(isReady);
	SendGameMessageToCaptain(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SendDissolveGame()
{
	if(!IAmCaptain())
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_DissolveGame);
	BroadcastGameMessage(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::SendStartGame()
{
	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_StartGame);
	BroadcastGameMessage(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleStartGameButton()
{
	if(mMyClient.get()==NULL || mGame.get()==NULL)
		return;

	if(IAmCaptain())
	{
		if(mGame->CanStartGame())
		{
			if(mGame->TryStartGame())
				SendStartGame();
		}
	}
	else
	{
		// Send ready request
		bool ready = !mMyClient->IsPlayerReady();
		if(!ready || mGame->CanReady())
		{
			mIAmReadySoundState = ready; // so sound doesn't play because of me
			SendReadyRequest(ready);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AutoLeaveGame
{
public:
	bool mDoLeave;

	AutoLeaveGame() : mDoLeave(true) { }
	~AutoLeaveGame() 
	{ 
		if(mDoLeave)
			LobbyStaging::LeaveGame(false); 
	}

	void Release() { mDoLeave = false; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::TryHostGame()
{
	if(!LeaveGame(true))
		return;

	LobbyGameType aGameType = LobbyScreen::GetGameScreenType();
	if(aGameType==LobbyGameType_None)
		return;

	if(aGameType!=LobbyGameType_Lan)
	{
		if(LobbyPersistentData::GetShowConfirmations())
		{
			int aResult = ControlId_Ok;
			if (!LobbyStaging::IsFirewallTestDone())
				aResult = LobbyDialog::DoOkCancelDialog(GameStagingLogic_ConnectWarningTitle1_String,GameStagingLogic_ConnectWarningDetails1_String);
			else if (LobbyStaging::IsBehindFirewall())
				aResult = LobbyDialog::DoOkCancelDialog(GameStagingLogic_ConnectWarningTitle2_String, GameStagingLogic_ConnectWarningDetails2_String);

			if(aResult!=ControlId_Ok)
				return;
		}
	}

	mCreateGameDialog->SetGameType(aGameType);
	mCreateGameDialog->Reset();
	int aResult = LobbyDialog::DoDialog(mCreateGameDialog);
	if(aResult!=ControlId_Ok)
		return;		

	LobbyGamePtr aGame = mCreateGameDialog->GetGame();
	aGame->SetIPAddr(LobbyStaging::GetIPAddr(aGameType));
	mGame = aGame;
	mJoinGameReply = LobbyGameStatus_Success; // so we don't ignore messages (which happens when the status is LobbyGameStatus_None)

	AutoLeaveGame aLeaveGame;
	if(!aGame->PreNetCreateGame())
		return;

	if(!LobbyStagingPrv::NetCreateGame(aGame))
		return;

	mMyClient = LobbyStagingPrv::NetGetMyClient();
	if(mMyClient.get()==NULL)
		return;

	// Fixme, these should probably all happen with one call
	aGame->SetIAmCaptain(true);
	aGame->SetCaptainId(mMyClient->GetClientId());
	mMyClient->SetIsCaptain(true,true);

	if(!aGame->PostNetCreateGame())
		return;

	aGame->GetClientList()->AddClient(mMyClient); // make sure my client is in the client list
	aGame->HandleHostGame(mMyClient);

	aLeaveGame.Release();
	mJoinedGame = true;
	mLastReadySoundTime = 0;
	mEveryoneReadySoundState = aGame->IsEveryoneReady();
	mIAmReadySoundState = LobbyStaging::IAmReady(); 
	LobbyStaging::UpdateGameSummary();

	LobbyEvent::BroadcastEvent(LobbyEvent_JoinedGame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StagingLogic::VerifyPing(LobbyGame *theGame)
{
	if(theGame->GetGameType()!=LobbyGameType_Internet) // don't need to verify pings for non-internet games
		return true;

	if(theGame->GetHadSuccessfulPing())
		return true;

	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig==NULL || aConfig->mAllowJoinGameWithoutPing)
	{
		// Warn that we haven't been able to ping the game yet
		if(LobbyPersistentData::GetShowConfirmations())
			return LobbyDialog::DoOkCancelDialog(GameStagingLogic_ConnectWarningTitle3_String,GameStagingLogic_ConnectWarningDetails3_String)==ControlId_Ok;
		else
			return true;
	}

	LobbyMisc::QueuePing(theGame,true);
	LobbyEvent::BroadcastEvent(new GameChangedEvent(theGame,LobbyChangeType_Modify));						
	while(theGame->IsPinging())
	{
		AsyncOpPtr anOp = new AsyncOp;
		LobbyDialog::DoStatusOp(anOp,GameStagingLogic_VerifyingPingTitle_String,GameStagingLogic_VerifyingPingDetails_String,CloseStatusDialogType_Any,500);
		if(anOp->Killed())
			return false;
	}
	
	if(!theGame->GetHadSuccessfulPing())
	{
		LobbyDialog::DoErrorDialog(GameStagingLogic_JoinGameErrorTitle_String,GameStagingLogic_NoPingReplyError_String);
		return false;
	}

	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::HandleJoinGame(JoinGameEvent *theEvent)
{
	TryJoinGame(theEvent->mGame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::TryJoinGame(LobbyGame *theGame)
{
	if(theGame==NULL)
		return;

	if(mGame.get()!=NULL && theGame->IsSameGame(mGame))
	{
		LobbyDialog::DoErrorDialog(RoutingLogic_AlreadyJoinedTitle_String,RoutingLogic_AlreadyInGame_String);
		return;
	}

	if(!VerifyPing(theGame))
		return;

	short aJoinStatus = theGame->GetOkToJoinStatus();
	if(aJoinStatus!=LobbyGameStatus_Success)
	{		
		if(aJoinStatus!=LobbyGameStatus_None)
			LobbyDialog::DoErrorDialog(GameStagingLogic_JoinGameErrorTitle_String,theGame->GetStatusString(aJoinStatus));
	
		return;
	}


	if(!LeaveGame(true))
		return;

	AutoLeaveGame aLeaveGame;
	LobbyGamePtr aGame = LobbyGame::CreateGame(theGame->GetGameType()); // copy game so that it isn't modified from the server list

 	aGame->CopyFrom(theGame);
	mGame = aGame;
	mJoinGameReply = LobbyGameStatus_None;

	if(!aGame->PreNetJoinGame()) // Let game know we're about to enter
		return; // don't need NetLeave because haven't called NetJoin yet

	if(!LobbyStagingPrv::NetJoinGame(aGame))
		return;

	mMyClient = LobbyStagingPrv::NetGetMyClient();
	if(mMyClient.get()==NULL)
		return;

	if(!aGame->PostNetJoinGame()) // Let game know that we have entered
		return;

	mJoinGameOp = new AsyncOp;
	ByteBufferPtr aJoinReq = mGame->GetJoinGameRequest();
	SendGameMessageToCaptain(aJoinReq);
	LobbyDialog::DoStatusOp(mJoinGameOp,GameStagingLogic_GettingGameInfoTitle_String,GameStagingLogic_GettingGameInfoDetails_String,CloseStatusDialogType_Success);
	
	mJoinGameOp = NULL;
	if(mJoinGameReply!=LobbyGameStatus_Success)
	{
		if(mJoinGameReply!=LobbyGameStatus_None)
			LobbyDialog::DoErrorDialog(GameStagingLogic_JoinGameErrorTitle_String,theGame->GetStatusString(mJoinGameReply));

		return;
	}


	aLeaveGame.Release();
	mJoinedGame = true;
	mLastReadySoundTime = 0;
	mEveryoneReadySoundState = aGame->IsEveryoneReady();
	mIAmReadySoundState = LobbyStaging::IAmReady(); 
	LobbyEvent::BroadcastEvent(LobbyEvent_JoinedGame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool StagingLogic::LeaveGame(bool confirm)
{
	if(mGame.get()==NULL)
		return true;

	if(confirm)
	{
		if(!LobbyPersistentData::GetShowConfirmations()) // user doesn't want to have to confirm
			confirm = false;
	}
	
	if(confirm)
	{
		int aResult;
		if(IAmCaptain())
			aResult = LobbyDialog::DoYesNoDialog(GameStagingLogic_DissolveGame_String,GameStagingLogic_DissolveGameConfirm_String);
		else
			aResult = LobbyDialog::DoYesNoDialog(GameStagingLogic_LeaveGame_String,GameStagingLogic_LeaveGameConfirm_String);

		if(aResult!=ControlId_Ok)
			return false;
	}

	if(mGame.get()==NULL)
		return true;

	if(IAmCaptain())
	{
		SendDissolveGame(); // Does dialog (actually doesn't do dialog anymore... got rid of it)
		LobbyStagingPrv::NetDeleteGameSummary(mGame);
	}

	if(mGame.get()==NULL) // need to check variables after a dialog (LeaveGame could have been called while in the dialog)
		return true;


	if(mJoinGameOp.get()!=NULL)
	{
		mJoinGameOp->Kill();
		mJoinGameOp = NULL;
	}

	LobbyGamePtr aGame = mGame;
	mGame = NULL;

	aGame->DeletePlayers(); // set all clients player data to NULL

	mMyClient = NULL;
	aGame->PreNetLeaveGame();
	LobbyStagingPrv::NetLeaveGame(aGame);
	aGame->PostNetLeaveGame();

	mLastReadySoundTime = 0;
	if(mJoinedGame)
	{
		LobbyEvent::BroadcastEvent(LobbyEvent_LeftGame);
		mJoinedGame = false;
	}
		
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::NotifyGameDisconnect(bool showErrorDlg)
{
	if(mGame.get()==NULL)
		return;

	LeaveGame(false);

	if(showErrorDlg)
		LobbyDialog::DoErrorDialog(StagingLogic_DisconnectedTitle_String,StagingLogic_DisconnectedDetails_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::NotifyClientKicked(LobbyClient *theClient, bool isBan)
{
	LobbyGame *aGame = GetGame();
	if(theClient==NULL || aGame==NULL)
		return;

	ClientModeratedEvent::ModerationAction anAction;
	if(isBan)
		anAction = ClientModeratedEvent::Action_Ban;
	else
		anAction = ClientModeratedEvent::Action_Kick;

	bool isMe = theClient==mMyClient.get();
	int aRoomSpec = LobbyRoomSpecFlag_Game;
	if(mGame->GetGameType()==LobbyGameType_Internet)
		aRoomSpec |= LobbyRoomSpecFlag_Internet;
	else
		aRoomSpec |= LobbyRoomSpecFlag_Lan;

	LobbyEvent::BroadcastEvent(new ClientModeratedEvent(theClient, aRoomSpec, mGame->GetName(), anAction, 0, isMe, GUIString::EMPTY_STR));
	if(isMe)
	{
		LeaveGame(false);
		if(isBan)
			LobbyDialog::DoErrorDialog(StagingLogic_BannedTitle_String,StagingLogic_BannedDetails_String); 
		else
			LobbyDialog::DoErrorDialog(StagingLogic_KickedTitle_String,StagingLogic_KickedDetails_String);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::NotifyClientLeftGame(LobbyClient *theClient)
{
	if(!theClient->IsPlayer() || mGame.get()==NULL)
		return;

	LobbyEvent::BroadcastEvent(new PlayerChangedEvent(theClient,LobbyChangeType_Delete));
	if(theClient->SameTeam(mMyClient))
		LobbyEvent::BroadcastEvent(new PlayerTeamChangedEvent(theClient,LobbyChangeType_Delete));

	mGame->HandlePlayerLeft(theClient);

	if(IAmCaptain())
		LobbyStaging::UpdateGameSummary();

	UpdateStartButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::NotifyPingChange(LobbyGame *theGame)
{
	if(mGame.get()==NULL || IAmCaptain())
		return;
	
	if(!theGame->IsSameGame(mGame))
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_PingChangedRequest);
	aMsg.AppendShort(theGame->GetPing());
	SendGameMessageToCaptain(aMsg.ToByteBuffer());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::KickClient(LobbyClient *theClient, bool isBan)
{
	if(theClient==NULL || mGame.get()==NULL || !IAmCaptain())
		return;

	WriteBuffer aMsg;
	aMsg.AppendByte(LobbyGameMsg_ClientKicked);
	aMsg.AppendShort(theClient->GetClientId());
	aMsg.AppendBool(isBan);
	BroadcastGameMessage(aMsg.ToByteBuffer());

	LobbyStagingPrv::NetKickClient(theClient,isBan);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::UpdateStartButton()
{
	LobbyEvent::BroadcastEvent(LobbyEvent_UpdateStartButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StagingLogic::CheckPlayReadySound()
{
	UpdateStartButton();

	LobbyGame *aGame = GetGame();
	if(aGame==NULL)
		return;

	bool everyoneReady = aGame->IsEveryoneReady();
	if(everyoneReady)
	{
		if(mEveryoneReadySoundState!=everyoneReady)
		{
			time_t aTime = time(NULL);
			if(aTime - mLastReadySoundTime >= 15) // don't play everyone ready sound too often
			{
				mLastReadySoundTime = aTime;
				if(LobbyPersistentData::GetStagingSoundEffects())
					LobbyMisc::PlaySound(LobbyGlobal_AllReady_Sound);
			}
		}
	}
	else if(!aGame->IAmCaptain())
	{
		if(mIAmReadySoundState!=LobbyStaging::IAmReady())
		{
			mIAmReadySoundState = LobbyStaging::IAmReady();
			if(!mIAmReadySoundState && LobbyPersistentData::GetStagingSoundEffects()) // became unready
				LobbyMisc::PlaySound(LobbyGlobal_AllNotReady_Sound);
		}
	}
	
	mEveryoneReadySoundState = everyoneReady;
}
