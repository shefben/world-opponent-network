#include "DialogLogic.h"
#include "LobbyCompletion.h"
#include "LobbyEvent.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyScreen.h"
#include "LobbyTypes.h"
#include "OkCancelDialog.h"
#include "StatusDialog.h"
#include "TOUDialog.h"
#include "WONLobby.h"
#include "WONStatusString.h"

#include "WONCommon/AsyncOp.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/GUICompat.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Window.h"

using namespace WONAPI;

typedef LogicCompletion<DialogLogic> DialogCompletion;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DialogLogic::DialogLogic()
{
	mPopupInOwnWindow = false;
	mUserCanceledLastStatusOp = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DialogLogic::~DialogLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::Init(ComponentConfig *theConfig)
{
	mDoingStatusDialog = false;
	mStatusCloseOnFinish = CloseStatusDialogType_None;

	mStatusOp = NULL;

	WONComponentConfig_Get(theConfig,mOkCancelDialog,"OkCancelDialog");
	WONComponentConfig_Get(theConfig,mStatusDialog,"StatusDialog");

	WindowManager *wm = WindowManager::GetDefaultWindowManager();
	if(wm!=NULL)
		mPopupInOwnWindow = wm->GetPopupInOwnWindowDef();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::KillHook()
{
	if(mStatusOp.get()!=NULL)
	{
		mStatusOp->Kill();
		mStatusOp = NULL;
	}

	CloseInactiveDialogs();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::StatusCompletion(AsyncOp *theOp)
{
	if(theOp==mStatusOp)
	{
		bool removeDialog = false;
		if(mStatusCloseOnFinish==CloseStatusDialogType_Any)
			removeDialog = true;
		else if(mStatusCloseOnFinish==CloseStatusDialogType_Success && theOp->Succeeded())
			removeDialog = true;

		if(removeDialog)
			LobbyScreen::GetWindowManager()->EndDialog(-1,NULL,mStatusDialog);
		else
			FinishStatusDialog(theOp->GetStatus());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::CloseInactiveDialogs()
{
	if(mDialogStack.empty())
		return;

	Container *aLobby = LobbyScreen::GetLobbyContainer();
	if(aLobby==NULL)
		return;

	bool removed = false;

	DialogStack::iterator anItr = mDialogStack.end();
	while(anItr!=mDialogStack.begin())
	{
		--anItr;

		DialogInfo &anInfo = *anItr;
		if(!anInfo.mActive)
		{
			Component *aComponent = anInfo.mDialog;
			Container *aParent = aComponent->GetParent();
			if(aParent!=NULL)
				aParent->RemoveChild(aComponent);

			if(mPopupInOwnWindow)
				anInfo.mWindow->Close();

			removed = true;
		}
		else
			break;
	}

	if(!removed)
		return;

	// This is simpler then trying to do the erase in the previous loop
	anItr=mDialogStack.begin(); 
	while(anItr!=mDialogStack.end())
	{
		if(!anItr->mActive)
			mDialogStack.erase(anItr++);
		else
			++anItr;
	}
	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DialogLogic::CheckIsCurrentDialog(Component *theDialog, bool &needCenter)
{
	needCenter = true;
	if(mDialogStack.empty())
		return false;

	DialogInfo &anInfo = mDialogStack.back();
	if(anInfo.mDialog.get()!=theDialog)
		return false;

	if(theDialog->Width()==anInfo.mBounds.Width() && theDialog->Height()==anInfo.mBounds.Height())
		needCenter = false;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WindowPtr DialogLogic::GetDialogWindow(bool isCurrentDialog)
{
	if(!mPopupInOwnWindow)
		return LobbyScreen::GetWindow();
	
	if(isCurrentDialog && !mDialogStack.empty())
		return mDialogStack.back().mWindow;
	

	WindowPtr aWindow = new PlatformWindow;
	aWindow->SetIsOneComponentWindow(true); 
	if(!mDialogStack.empty())
		aWindow->SetParent(mDialogStack.back().mWindow);
	else
		aWindow->SetParent(LobbyScreen::GetWindow());

	aWindow->SetCreateFlags(CreateWindow_HideAtFirst | CreateWindow_Popup);
	aWindow->Create(WONRectangle(0,0,100,100));
	LobbyEvent::ListenToComponent(aWindow->GetRoot());
	aWindow->GetRoot()->SetColorScheme(LobbyScreen::GetColorScheme());
	return aWindow;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int DialogLogic::DoDialog(Component *theComponent)
{
	bool needCenter = true;
	bool isCurrentDialog = CheckIsCurrentDialog(theComponent, needCenter);
	if(!isCurrentDialog)
		CloseInactiveDialogs();

	WindowPtr aWindow = GetDialogWindow(isCurrentDialog);
	if(aWindow.get()==NULL)
		return -1;

	ContainerPtr aContainer;
	if(mPopupInOwnWindow)
	{
		aWindow->Resize(theComponent->Width(),theComponent->Height());
		aContainer = aWindow->GetRoot();
	}
	else
		aContainer = LobbyScreen::GetLobbyContainer();

	if(aContainer.get()==NULL)
		return -1;

	if(needCenter)
	{
		if(mPopupInOwnWindow)
			aWindow->CenterWindow();
		else
			CenterLayout::Center(aContainer,theComponent,true,true);
	}

	if(!isCurrentDialog)
	{
		aContainer->AddChild(theComponent);
		mDialogStack.push_back(DialogInfo());
	}

	DialogInfo &anInfo = mDialogStack.back();
	anInfo.mWindow = aWindow;
	anInfo.mDialog = theComponent;
	anInfo.mActive = true;
	anInfo.mBounds = theComponent->GetBounds();

	if(mPopupInOwnWindow)
		aWindow->Show(true);

	int aResult = aWindow->DoDialog(theComponent);
	anInfo.mActive = false;
	return aResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::DoStatusOp(AsyncOp *theOp, DWORD theTimeout, const GUIString &theTitle, const GUIString &theMessage, CloseStatusDialogType closeOnFinish, DWORD theTime)
{
	if(mDoingStatusDialog)
	{
		theOp->ForceFinish(WS_Killed);
		return;
	}

	AutoWMPauseEvents aPauseEvents; // make sure op doesn't run before dialog box comes up

	mStatusOp = theOp;
	mUserCanceledLastStatusOp = false;
	
	RunOp(mStatusOp, new DialogCompletion(StatusCompletion,this), theTime);
	
	mStatusCloseOnFinish = closeOnFinish;

	DoStatusDialog(theTitle,theMessage,true);
	if(mStatusOp->Pending())
	{
		mUserCanceledLastStatusOp = true;
		mStatusOp->Kill();
	}

	mStatusOp = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::DoStatusDialog(const GUIString &theTitle, const GUIString &theStatus, bool isCancel)
{
	if(mDoingStatusDialog || WONLobby::IsLobbySuspended())
		return;

	mDoingStatusDialog = true;
	mStatusDialog->SetText(theTitle, theStatus, isCancel);
	int aResult = DoDialog(mStatusDialog);
	mStatusDialog->SetTick(GetTickCount());
	mDoingStatusDialog = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::DoErrorDialog(const GUIString &theTitle, const GUIString &theStatus)
{
	if(mDoingStatusDialog) // already doing a dialog
		return;

	if(LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	DoStatusDialog(theTitle,theStatus,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::ShowStatus(const GUIString &theTitle, WONStatus theStatus)
{
	DoStatusDialog(theTitle,FriendlyWONStatusToString(theStatus),false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::FinishStatusDialog(WONStatus theStatus)
{
	WONRectangle anOldSize = mStatusDialog->GetBounds();

	std::wstring aStatusStr;
	mStatusDialog->SetText(FriendlyWONStatusToString(theStatus),false);
	if(theStatus!=WS_Success && LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	if(anOldSize.Width()==mStatusDialog->Width() && anOldSize.Height()==mStatusDialog->Height())
		return;

	if(mPopupInOwnWindow)
	{
		Window *aWindow = mStatusDialog->GetMyWindow();
		if(aWindow==NULL)
			return;

		aWindow->Resize(mStatusDialog->Width(),mStatusDialog->Height());
		aWindow->CenterWindow();
	}
	else
	{
		Container *aContainer = LobbyScreen::GetLobbyContainer();
		if(aContainer!=NULL)
		{
			CenterLayout::Center(aContainer,mStatusDialog,true,true);
			WONRectangle aRect = mStatusDialog->GetBounds();
			aContainer->InvalidateRect(aRect);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int DialogLogic::DoOkCancelDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound)
{
	mOkCancelDialog->SetText(theTitle, theDetails, LobbyGlobal_OK_String, LobbyGlobal_Cancel_String);
	if(doWarningSound && LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	return DoDialog(mOkCancelDialog);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Uses mOkCancelDialog
int DialogLogic::DoYesNoDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound)
{
	mOkCancelDialog->SetText(theTitle, theDetails, LobbyGlobal_Yes_String, LobbyGlobal_No_String);
	if(doWarningSound && LobbyPersistentData::GetLobbySoundEffects())
		LobbyMisc::PlaySound(LobbyGlobal_Error_Sound);

	return DoDialog(mOkCancelDialog);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DialogLogic::TimerEvent()
{
	// The following code prevents the lobby window's title bar from flashing blue and grey
	// when we bring up a bunch of status dialogs one after the other.
//	if(mCurDialogWindow.get()==NULL && mDialogWindowCache.get()!=NULL && mDialogWindowCache->IsShowing())
//		mDialogWindowCache->Show(false);

	CloseInactiveDialogs();
}

