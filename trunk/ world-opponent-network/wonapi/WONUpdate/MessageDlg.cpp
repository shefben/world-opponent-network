//----------------------------------------------------------------------------------
// MessageDlg.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "MessageDlg.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/SimpleComponent.h"
#include "WizardCtrl.h"
#include "CustomInfo.h"


using namespace WONAPI;

//----------------------------------------------------------------------------------
// MessageDialog: Creates, displays and returns the modal result of a Message 
// Dialog.
//----------------------------------------------------------------------------------
int WONAPI::MessageBox(Window *pParent, const GUIString& sMsg, const GUIString& sTitle, DWORD nFlags)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	GUIString sButton1 = GUIString::EMPTY_STR;
	GUIString sButton2 = GUIString::EMPTY_STR;
	GUIString sButton3 = GUIString::EMPTY_STR;

	DWORD nStyle = nFlags & MD_STYLEBITS;

	switch (nStyle)
	{
		case MD_OKCANCEL:
			sButton2 = pResMgr->GetFinishedString(Global_Cancel_String);
			// No break
		case MD_OK:
			sButton1 = pResMgr->GetFinishedString(Global_Ok_String);
			break;

		case MD_YESNOCANCEL:
			sButton3 = pResMgr->GetFinishedString(Global_Cancel_String);
			// No break
		case MD_YESNO:
			sButton1 = pResMgr->GetFinishedString(Global_Yes_String);
			sButton2 = pResMgr->GetFinishedString(Global_No_String);
			break;

		case MD_ABORT:
			sButton1 = pResMgr->GetFinishedString(Dialog_AbortDlgAbort_String);
			break;
	}

	// Constuct the Message Box.
	MSMessageBoxPtr pMsgDlg = new MSMessageBox(sTitle, sMsg, sButton1, sButton2, sButton3);

	// Set the background image.
	LabelPtr pBackground = new Label;
	pBackground->SetBackground(Dialog_BaseBackground_Background);
	pMsgDlg->GetLabel()->SetTransparent(true);
	pMsgDlg->AddChildLayout(pBackground, CLI_SameSize, pMsgDlg);
	pMsgDlg->AddChildEx(pBackground, 1);
	
	// Display the Message Box.
	int nMsgVal = pMsgDlg->DoDialog(pParent);

	// Translate it back to a standard MS-style dialog return.
	switch (nStyle)
	{
		case MD_OK:
		case MD_OKCANCEL:
			switch (nMsgVal)
			{
				case 1: return MDR_OK;
				case 2: return MDR_CANCEL;
			}
			break;

		case MD_YESNO:
		case MD_YESNOCANCEL:
			switch (nMsgVal)
			{
				case 1: return MDR_YES;
				case 2: return MDR_NO;
				case 3: return MDR_CANCEL;
			}
			break;

		case MD_ABORT:
			switch (nMsgVal)
			{
				case 1: return MDR_ABORT;
				default: return nMsgVal; // Set by the guy that closes us.
			}
			break;
	}

	assert(false);
	return MDR_OK;
}
