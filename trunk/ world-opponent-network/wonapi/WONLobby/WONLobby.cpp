#include "WONLobby.h"
#include "WONLobbyPrv.h"

#include "InitLogic.h"
#include "LobbyContainer.h"
#include "LobbyEvent.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyStagingPrv.h"

#include "WONGUI/ChildLayouts.h"
#include "WONGUI/MSControls.h"

using namespace WONAPI;

namespace WONAPI
{
	WONLobbyPrvPtr gLobbyData;
}

static GUIString gLobbyInitError;
static bool gLobbyHadInitError;
static ContainerPtr gLobbyErrorContainer;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONLobby::Init(ResourceConfigTable *theTable, ComponentInitFunc theInitFunc)
{
	assert(gLobbyData.get()==NULL);

	gLobbyInitError.erase();
	gLobbyHadInitError = false;
	gLobbyErrorContainer = NULL;

	gLobbyData = new WONLobbyPrv;

	if(!gLobbyData->Init(theTable,theInitFunc))
	{
		// Fixme: Clear any subscriptions that happened in InitComponent
		WindowManager *aWindowMgr = WindowManager::GetDefaultWindowManager();
		if(aWindowMgr!=NULL)
			aWindowMgr->ClearSubscriptions(); // FIXME, just clear lobby subscriptions

		gLobbyInitError = gLobbyData->mInitError;
		gLobbyHadInitError = true;
		gLobbyData = NULL;
		return false;
	}

	LobbyEvent::Init();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobby::InitAuth(const wstring& theLoginCommunity, const char* theHashFileListStr /* = NULL */)
{
	if(gLobbyData.get()==NULL)
		return;

	gLobbyData->InitAuth(theLoginCommunity, theHashFileListStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobby::Destroy()
{
	gLobbyErrorContainer = NULL;
	gLobbyHadInitError = false;
	gLobbyInitError.erase();

	if(gLobbyData.get()==NULL)
		return;

	LobbyStaging::Logout(false);
	gLobbyData->Destroy();
	LobbyEvent::Destroy();

	LobbyGlobal_LobbyMusic_Sound->Stop();

	LobbyResource_UnloadResources();
	gLobbyData = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& WONLobby::GetInitError()
{
	return gLobbyInitError;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void LobbyErrorButtonListener(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
		WONLobby::EndLobby(LobbyStatus_None);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Container* WONLobby::GetLobbyContainer()
{
	if(gLobbyData.get()!=NULL)
		return gLobbyData->mLobbyContainer;
	else
	{
		if(gLobbyHadInitError) // return a screen which shows the error
		{
			if(gLobbyErrorContainer.get()==NULL)
			{
				DialogPtr aContainer = new Dialog;
				aContainer->SetEndOnEscape(0);

				// Background
				MSDlgBackPtr aBack = new MSDlgBack;
				

				// Error Label
				LabelPtr aLabel = new Label;
				aLabel->SetWrap(true);
				aLabel->SetHorzAlign(HorzAlign_Center);
				aLabel->SetAdjustHeightToFitText(true);
				GUIString aText = "Lobby Error\n\n";
				aText.append(gLobbyInitError);
				aLabel->SetText(aText);

				// Ok Button
				MSButtonPtr aButton = new MSButton("Ok");
				aButton->SetButtonFlags(ButtonFlag_EndDialog,true);
				aButton->SetSizeAtLeast(100,0);
				aButton->RequestFocus();
	//			aButton->SetListener(new ComponentListenerFunc(LobbyErrorButtonListener));

				// Layout
				aContainer->AddChildLayout(aBack,CLI_SameSize,aContainer);
				aContainer->AddChildLayout(aLabel,CLI_SameWidth | CLI_SameLeft,aContainer,10,0,-20);
				aContainer->AddChildLayout(new CenterLayout(aContainer,aButton,true,false));
				aContainer->AddChildLayout(aButton,CLI_Below,aLabel,0,20);
				aContainer->AddChildLayout((new CenterLayout(aContainer,aLabel,false,true))->Add(aButton));

				// Add Components
				aContainer->AddChild(aBack);
				aContainer->AddChild(aLabel);
				aContainer->AddChild(aButton);

				gLobbyErrorContainer = aContainer;
			}

			return gLobbyErrorContainer;
		}
		else
			return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobby::StartLobby()
{
	if(gLobbyData.get()==NULL)
		return;

	gLobbyData->mSuspended = false;
	gLobbyData->mInitLogic->DoInit();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyStatus WONLobby::RunLobby()
{
	LobbyRunMode theRunMode = LobbyRunMode_Block;

	// If config error then just show the error container until the user hits ok
	if(gLobbyHadInitError && gLobbyErrorContainer.get()!=NULL)
	{
		Window *aWindow = gLobbyErrorContainer->GetMyWindow();
		if(aWindow!=NULL && aWindow->IsShowing())
			aWindow->DoDialog(gLobbyErrorContainer);

		return LobbyStatus_None;
	}


	if(gLobbyData.get()==NULL)
		return LobbyStatus_None;

	gLobbyData->mLobbyStatus = LobbyStatus_None;
	gLobbyData->mRunMode = theRunMode;
	gLobbyData->mSuspended = false;

	WindowManager *aWindowMgr = LobbyScreen::GetWindowManager();
	if(aWindowMgr==NULL)
		return LobbyStatus_None;

	if(theRunMode==LobbyRunMode_Block)
		aWindowMgr->MessageLoop();
	
	if(gLobbyData.get()==NULL)
		return LobbyStatus_None;
	else
		return gLobbyData->mLobbyStatus;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobby::EndLobby(LobbyStatus theStatus)
{
	if(gLobbyData.get()==NULL)
		return;

	gLobbyData->mLobbyStatus = theStatus;
	LobbyRunMode aRunMode = gLobbyData->mRunMode;

	// Tell any listeners.
	LobbyEvent::BroadcastEvent(LobbyEvent_EndLobby);
	
	if(aRunMode!=LobbyRunMode_Async)
	{
		WindowManager *aWindowMgr = LobbyScreen::GetWindowManager();
		if(aWindowMgr!=NULL)
			aWindowMgr->ExitMessageLoop();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobby::SuspendLobby()
{
	if(gLobbyData.get()==NULL)
		return;

	gLobbyData->mSuspended = true;
	LobbyGlobal_LobbyMusic_Sound->Stop();
	LobbyGlobal_SierraLogon_Sound->Stop();

	if(LobbyMisc::GetNumPingsToDo()>0)
	{
		LobbyMisc::KillPings(true);
		LobbyEvent::BroadcastEvent(LobbyEvent_ResyncGameList);
	}

	LobbyStagingPrv::NetSuspendLobby();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONLobby::ResumeLobby()
{	
	if(gLobbyData.get()==NULL)
		return;

	gLobbyData->mSuspended = false;
	if(LobbyPersistentData::GetLobbyMusic())
		LobbyGlobal_LobbyMusic_Sound->Resume();

	LobbyStagingPrv::NetResumeLobby();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONLobby::IsLobbySuspended()
{
	if(gLobbyData.get()==NULL)
		return false;
	else
		return gLobbyData->mSuspended;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyStatus WONLobby::GetStatus()
{
	if(gLobbyData.get()!=NULL)
		return gLobbyData->mLobbyStatus;
	else
		return LobbyStatus_None;
		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGame* WONLobby::GetGame()
{
	return LobbyStaging::GetGame();
}
