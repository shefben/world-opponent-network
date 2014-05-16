#include "TOUDialog.h"

#include "LobbyEvent.h"
#include "LobbyMisc.h"

#include "WONGUI/BrowserComponent.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/HTMLDocumentGen.h"
#include "WONMisc/GetHTTPDocumentOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TOUDialog::TOUDialog()
{
	mNeedGenHTML = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TOUDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mBrowser,"Browser");
		WONComponentConfig_Get(aConfig,mAgreeCheck,"AgreeCheck");
		WONComponentConfig_Get(aConfig,mAcceptButton,"AcceptButton");
		WONComponentConfig_Get(aConfig,mRejectButton,"RejectButton");

		mAcceptButton->SetControlId(1);
		mRejectButton->SetControlId(-1);
	
		Reset();

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TOUDialog::Reset()
{
	if(mNeedGenHTML)
	{
		mNeedGenHTML = false;
		HTMLDocumentGen aGen;
		aGen.AddDocParam(LobbyMisc::GetSysTOUPath(),HTMLDocGenFlag_IsOptionalHTML | HTMLDocGenFlag_IsFile);
		aGen.AddDocParam(LobbyMisc::GetGameTOUPath(),HTMLDocGenFlag_IsOptionalHTML | HTMLDocGenFlag_IsFile);
		aGen.printf("<body>%1%<br><br>%2%</body>");

		mBrowser->Go(aGen.GetDocument());
	}

	mBrowser->GetHTMLView()->SetOffsets(0,0);
	mAgreeCheck->SetCheck(false);
	mAcceptButton->Enable(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TOUDialog::Paint(Graphics &g)
{
	Container::Paint(g);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TOUDialog::HandleHTTPDocEvent(GotHTTPDocumentEvent *theEvent)
{
	GotHTTPDocumentEvent *anEvent = (GotHTTPDocumentEvent*)theEvent;
	GetHTTPDocumentOp *anOp = anEvent->mOp;
	if(anOp->GetDocType()!=HTTPDocType_TOU)
		return;

	mNeedGenHTML = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TOUDialog::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==LobbyEvent_GotHTTPDocument)
	{
		HandleHTTPDocEvent((GotHTTPDocumentEvent*)theEvent);
		return;
	}
	else if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		if(theEvent->mComponent==mAgreeCheck)
		{
			mAcceptButton->Enable(mAgreeCheck->IsChecked());
			return;
		}
	}

	Dialog::HandleComponentEvent(theEvent);
}
