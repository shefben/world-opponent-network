//----------------------------------------------------------------------------------
// MotdCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONAPI/WONCommon/StringUtil.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "WONGUI/BrowserComponent.h"
#include "WONGUI/HTMLDocumentGen.h"
#include "MotdCtrl.h"
#include "WONUpdateCtrl.h"
#include "CustomInfo.h"
#include "DebugLog.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
MotdCtrl* MotdCtrl::m_pInstance = NULL;


//----------------------------------------------------------------------------------
// MotdCtrl Constructor.
//----------------------------------------------------------------------------------
MotdCtrl::MotdCtrl(void)
	: WizardCtrl()
{
	m_pInstance = this;

	PatchCore* pCore = GetMainControl()->GetPatchCore();
	pCore->SetDnsTestCallback(new MotDCtrlCallback(DnsTestCallback));
	pCore->SetMotdCallback(new MotDCtrlCallback(MotdCallback));
}

//----------------------------------------------------------------------------------
// MotdCtrl Destructor.
//----------------------------------------------------------------------------------
MotdCtrl::~MotdCtrl(void)
{
	m_pInstance = NULL;
}

//----------------------------------------------------------------------------------
// DnsTestCallback: The DNS Test has completed (for better or worse).
//----------------------------------------------------------------------------------
void MotdCtrl::DnsTestCallback(OperationInfo* pOpInfo)
{
	DNSTestOp* pOp = dynamic_cast<DNSTestOp*>(pOpInfo->m_pOp.get());
	if (! pOp->GetStatus() == WS_Success)
	{
		// If this window is visible display the error.
		if (IsVisible())
		{
			// Tell the user that they are [probably] not connected to the internet.
			ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
			MessageBox(GetWindow(), pResMgr->GetFinishedString(Global_CheckConection_String), pResMgr->GetFinishedString(Global_CheckConectionTitle_String), MD_OK);
		}
	}
}

//----------------------------------------------------------------------------------
// MotdComplete: The MotD's have been fetched.  Extract them from the raw data.
//----------------------------------------------------------------------------------
void MotdCtrl::MotdCallback(OperationInfo* pOpInfo)
{
	GetMOTDOp* pGetMotdOp = dynamic_cast<GetMOTDOp*>(pOpInfo->m_pOp.get());

	DebugLog("Retrieved MotD.\n");

	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	std::string aSysMOTDPath = pGetMotdOp->GetSysMOTDPath();
	std::string aGameMOTDPath = pGetMotdOp->GetGameMOTDPath();

	HTMLDocumentGen aGen;
	aGen.AddDocParam(aSysMOTDPath,HTMLDocGenFlag_IsOptionalHTML | HTMLDocGenFlag_SkipFirstCharIfNotHTML | HTMLDocGenFlag_IsFile); 
	aGen.AddDocParam(aGameMOTDPath,HTMLDocGenFlag_IsOptionalHTML | HTMLDocGenFlag_SkipFirstCharIfNotHTML | HTMLDocGenFlag_IsFile);
	
	aGen.printf("<body>%1%<br><br>%2%</body>");
	m_pInfoText->Go(aGen.GetDocument());

	// Update the Next button to say 'next' instead of 'skip'.
	m_pNextButton->SetText(pResMgr->GetFinishedString(Motd_Next_String));
	m_pNextButton->Invalidate();
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void MotdCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		// Do *not* refresh the info string, or it will blow away the downloaded MotD.

		// Make sure the Next button is the default button.
		m_pNextButton->RequestFocus();

		// Start the operation chain (if needed).
		GetMainControl()->GetPatchCore()->DoNextOperation();
	}
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void MotdCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,m_pInfoText,"InfoBox");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,m_pNextButton,"NextButton");

		ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
		GUIString sInfo = pResMgr->BuildInfoString(Motd_Info1_String_Id);

		m_pInfoText->SetLaunchBrowserOnLink(true);

		// Insert default text
		HTMLDocumentGen aGen;
		aGen.AddDocParam(sInfo); 
		aGen.printf("%1%");
		m_pInfoText->Go(aGen.GetDocument());		

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetConfigProxyHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help button.
//----------------------------------------------------------------------------------
bool MotdCtrl::HandleHelpButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// Invoke the Help callback for this dialog.
	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowHelp();

	return true;
}

//----------------------------------------------------------------------------------
// HandleBackButton: Process a click on the Back button.
//
// Note:  If the Motd has not finished downloading, it will be shut down when the 
// 'Show' method is invoced to hide this window.
//----------------------------------------------------------------------------------
bool MotdCtrl::HandleBackButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowPreviousScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleNextButton: Process a click on the Next button.
//
// Note:  If the Motd has not finished downloading, it will be shut down when the 
// 'Show' method is invoced to hide this window.
//----------------------------------------------------------------------------------
bool MotdCtrl::HandleNextButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowNextScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void MotdCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}
