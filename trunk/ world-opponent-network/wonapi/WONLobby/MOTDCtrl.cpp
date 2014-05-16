#include "MOTDCtrl.h"

#include "LobbyEvent.h"

#include "WONGUI/BrowserComponent.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/HTMLParser.h"
#include "WONGUI/HTMLDocumentGen.h"
#include "WONMisc/GetHTTPDocumentOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MOTDCtrl::MOTDCtrl()
{
	mNeedGenHTML = false;
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MOTDCtrl::~MOTDCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MOTDCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mBrowser,"Browser");
		mBrowser->SetLaunchBrowserOnLink(true);

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MOTDCtrl::Paint(Graphics &g)
{
	if(mNeedGenHTML)
	{
		mNeedGenHTML = false;

		if(!mSysMOTDPath.empty() && !mGameMOTDPath.empty())
		{
			// HTMLDocGenFlag_IsOptionalHTML -> if first character is a 
			HTMLDocumentGen aGen;
			aGen.AddDocParam(mSysMOTDPath,HTMLDocGenFlag_IsOptionalHTML | HTMLDocGenFlag_SkipFirstCharIfNotHTML | HTMLDocGenFlag_IsFile); 
			aGen.AddDocParam(mGameMOTDPath,HTMLDocGenFlag_IsOptionalHTML | HTMLDocGenFlag_SkipFirstCharIfNotHTML | HTMLDocGenFlag_IsFile);

			mSysMOTDPath.erase();
			mGameMOTDPath.erase();
		
			aGen.printf("<body>%1%<br><br>%2%</body>");
			mBrowser->Go(aGen.GetDocument());
		}
	}
	Container::Paint(g);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MOTDCtrl::ShowMessage(GetHTTPDocumentOp *theOp)
{
	if(theOp->GetDocType()!=HTTPDocType_MOTD)
		return;

	mNeedGenHTML = true;
	mSysMOTDPath = theOp->GetDocumentPath(HTTPDocOwner_Sys);
	mGameMOTDPath = theOp->GetDocumentPath(HTTPDocOwner_Game);
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MOTDCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==LobbyEvent_GotHTTPDocument)
	{
		ShowMessage(((GotHTTPDocumentEvent*)theEvent)->mOp);
		return;
	}

	Container::HandleComponentEvent(theEvent);
}
