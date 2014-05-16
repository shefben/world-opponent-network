#include "GameStagingCtrl.h"

#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyMisc.h"
#include "LobbyResource.h"
#include "LobbyStaging.h"

#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameStagingCtrl::GameStagingCtrl()
{
	mRoomSpecFlags = LobbyRoomSpecFlag_Game;
	mInGame = false;
	mReady = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameStagingCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mLeaveGameButton,"LeaveGameButton");
		WONComponentConfig_Get(aConfig,mStartGameButton,"StartGameButton");

		mStartGameButton->Enable(false);

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameStagingCtrl::UpdateStartButton()
{
	if(!mInGame)
		return;

	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL)
		return;

	bool enable = false;
	if(aGame->IAmCaptain())
		enable = aGame->CanStartGame();
	else
	{
		if(mReady!=LobbyStaging::IAmReady())
		{
			mReady = LobbyStaging::IAmReady();
			mStartGameButton->SetText(mReady?GameStagingCtrl_NotReady_String:GameStagingCtrl_Ready_String);
		}

		if(!mReady)
			enable = aGame->CanReady();
		else
			enable = true; // can always unready
	}

	mStartGameButton->Enable(enable);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameStagingCtrl::HandleEnteredGame()
{
	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL || !aGame->CheckRoomSpecFlags(mRoomSpecFlags))
		return;

	mInGame = true;

	if(aGame->IAmCaptain())
	{
		mLeaveGameButton->SetText(GameStagingCtrl_DissolveGame_String);
		mStartGameButton->SetText(GameStagingCtrl_StartGame_String);
	}
	else 
	{
		mLeaveGameButton->SetText(GameStagingCtrl_LeaveGame_String);

		mReady = LobbyStaging::IAmReady();
		mStartGameButton->SetText(mReady?GameStagingCtrl_NotReady_String:GameStagingCtrl_Ready_String);
	}

	UpdateStartButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameStagingCtrl::HandleLeftGame()
{
	mInGame = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameStagingCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_JoinedGame: HandleEnteredGame(); break;
		case LobbyEvent_LeftGame: HandleLeftGame(); break;
		case LobbyEvent_SetRoomSpecFlags: mRoomSpecFlags |= ((SetRoomSpecFlagsEvent*)theEvent)->mSpecFlags; break;
		case LobbyEvent_UpdateStartButton: UpdateStartButton(); break;
	}

	Container::HandleComponentEvent(theEvent);
}
