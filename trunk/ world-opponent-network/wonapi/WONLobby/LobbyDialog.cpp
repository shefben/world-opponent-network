#include "LobbyDialog.h"

#include "DialogLogic.h"
#include "LobbyContainer.h"
#include "LobbyEvent.h"
#include "LobbyMisc.h"
#include "LobbyScreen.h"
#include "WONLobbyPrv.h"

#include "WONGUI/Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyDialog::DoPopup(Component *theComponent, int thePopupFlags, int x, int y)
{
	if(gLobbyData.get()==NULL)
		return;

	Window *aWindow = LobbyScreen::GetWindow();
	if(aWindow!=NULL)
	{
		LobbyEvent::ListenToComponent(theComponent);
		aWindow->DoPopup(PopupParams(theComponent,thePopupFlags,gLobbyData->mLobbyContainer,x,y));
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyDialog::DoDialog(Component *theComponent)
{
	if(gLobbyData.get())
		return gLobbyData->mDialogLogic->DoDialog(theComponent);
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyDialog::DoStatusOp(AsyncOp *theOp, const GUIString &theTitle, const GUIString &theMessage, CloseStatusDialogType closeOnFinish, DWORD theTime)
{
	if(gLobbyData.get())
		gLobbyData->mDialogLogic->DoStatusOp(theOp,LobbyMisc::GetDefaultTimeout(),theTitle,theMessage,closeOnFinish,theTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyDialog::DoStatusDialog(const GUIString &theTitle, const GUIString &theStatus, bool isCancel)
{
	if(gLobbyData.get())
		gLobbyData->mDialogLogic->DoStatusDialog(theTitle,theStatus,isCancel);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyDialog::DoErrorDialog(const GUIString &theTitle, const GUIString &theStatus)
{
	if(gLobbyData.get())
		gLobbyData->mDialogLogic->DoErrorDialog(theTitle,theStatus);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyDialog::ShowStatus(const GUIString &theTitle, WONStatus theStatus)
{
	if(gLobbyData.get())
		gLobbyData->mDialogLogic->ShowStatus(theTitle,theStatus);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyDialog::FinishStatusDialog(WONStatus theStatus)
{
	if(gLobbyData.get())
		gLobbyData->mDialogLogic->FinishStatusDialog(theStatus);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyDialog::DoOkCancelDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound)
{
	if(gLobbyData.get())
		return gLobbyData->mDialogLogic->DoOkCancelDialog(theTitle,theDetails,doWarningSound);
	else
		return ControlId_Cancel;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LobbyDialog::DoYesNoDialog(const GUIString &theTitle, const GUIString &theDetails, bool doWarningSound)
{
	if(gLobbyData.get())
		return gLobbyData->mDialogLogic->DoYesNoDialog(theTitle,theDetails,doWarningSound);
	else
		return ControlId_Cancel;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LobbyDialog::UserCanceledLastStatusOp()
{
	if(gLobbyData.get())
		return gLobbyData->mDialogLogic->GetUserCanceledLastStatusOp();
	else
		return false;
}
