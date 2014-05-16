//----------------------------------------------------------------------------------
// VersionCheckCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "VersionCheckCtrl.h"
#include "WONUpdateCtrl.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
VersionCheckCtrl* VersionCheckCtrl::m_pInstance = NULL;


//----------------------------------------------------------------------------------
// VersionCheckCtrl Constructor.
//----------------------------------------------------------------------------------
VersionCheckCtrl::VersionCheckCtrl(void)
	: WizardCtrl()
{
	m_bShowBackMsg = true;
	m_pInstance = this;
	m_tLastBlink = 0;

	PatchCore* pCore = GetMainControl()->GetPatchCore();
	pCore->SetDnsTestCallback(new VersionCheckCtrlCallback(DnsTestCallback));
	pCore->SetMotdCallback(new VersionCheckCtrlCallback(MotdCallback));
	pCore->SetServerLookupCallback(new VersionCheckCtrlCallback(ServerLookupCallback));
	pCore->SetSelfVersionCheckCallback(new VersionCheckCtrlCallback(SelfVerCheckCallback));
	pCore->SetGameVersionCheckCallback(new VersionCheckCtrlCallback(GameVerCheckCallback));

	RequestTimer(true);
}

//----------------------------------------------------------------------------------
// VersionCheckCtrl Destructor.
//----------------------------------------------------------------------------------
VersionCheckCtrl::~VersionCheckCtrl(void)
{
	m_pInstance = NULL;
}

//----------------------------------------------------------------------------------
// DnsTestCallback: The DNS Test has completed (for better or worse).
//----------------------------------------------------------------------------------
void VersionCheckCtrl::DnsTestCallback(OperationInfo* /*pOpInfo*/)
{
	UpdateControls();
}

//----------------------------------------------------------------------------------
// MotdCallback: The Motd operation has completed.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::MotdCallback(OperationInfo* /*pOpInfo*/)
{
	UpdateControls();
}

//----------------------------------------------------------------------------------
// ServerLookupCallback: The Server Lookup has completed.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::ServerLookupCallback(OperationInfo* /*pOpInfo*/)
{
	UpdateControls();
}

//----------------------------------------------------------------------------------
// SelfVerCheckCallback: The version check operation has completed.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::SelfVerCheckCallback(OperationInfo* pOpInfo)
{
	UpdateControls();
}

//----------------------------------------------------------------------------------
// GameVerCheckCallback: The version check operation has completed.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::GameVerCheckCallback(OperationInfo* pOpInfo)
{
	UpdateControls();
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pGeneralStatusText,"GeneralStatusText");
		WONComponentConfig_Get(aConfig,m_pBackText,"BackText");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,m_pNextButton,"NextButton");

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetConfigProxyHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// DetermineControlStates: Determine which controls should be visible, etc.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::DetermineControlStates(void)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
	PatchCore* pCore = GetMainControl()->GetPatchCore();

	m_bEnableNextButton = false;
	m_bShowBackMsg = true;

	if (! pCore->GetDnsTestOp()->Succeeded())
		return;

	if (! pCore->GetServerLookupOp()->Succeeded())
		return;

	// Has SierraUp's version check completed?
	if (! pCore->GetSelfVersionCheckOp()->Completed())
		return;

	// Does SierraUp need to be updated?
	if (pCore->GetSelfVersionCheckResult() == SUR_PATCH_FOUND)
	{
		m_pGeneralStatusText->SetText(pResMgr->GetFinishedString(VersionCheck_StatusFinished_String));
		m_pGeneralStatusText->Invalidate();

		m_bEnableNextButton = true;
		m_bShowBackMsg = false;
		return;
	}

	// Is SierraUp hosed?
	if (pCore->GetSelfVersionCheckResult() != SUR_UP_TO_DATE)
	{
		m_pGeneralStatusText->SetText(pResMgr->GetFinishedString(VersionCheck_StatusErrorFound_String));
		m_pGeneralStatusText->Invalidate();

		m_bShowBackMsg = false;
		return;
	}

	// Has the game's version check returned?
	if (! pCore->GetGameVersionCheckOp()->Completed())
		return;

	// Is the game ready for update?
	if (pCore->GetGameVersionCheckResult() == SUR_PATCH_FOUND ||
		pCore->GetGameVersionCheckResult() == SUR_OPTIONAL_PATCH)
	{
		m_pGeneralStatusText->SetText(pResMgr->GetFinishedString(VersionCheck_StatusFinished_String));
		m_pGeneralStatusText->Invalidate();

		m_bEnableNextButton = true;
		m_bShowBackMsg = false;
		return;
	}

	// Is the game up to date?
	if (pCore->GetGameVersionCheckResult() == SUR_UP_TO_DATE)
	{
		// Tell the user that they are up to date.
		m_pGeneralStatusText->SetText(pResMgr->GetFinishedString(VersionCheck_UpToDate_String));
		m_pGeneralStatusText->Invalidate();

		m_pNextButton->SetText(pResMgr->GetFinishedString(VersionCheck_Finish_String));
		m_pNextButton->Invalidate();

		m_bEnableNextButton = true;
		m_bShowBackMsg = false;
		return;
	}

	// The game is hosed.
	m_pGeneralStatusText->SetText(pResMgr->GetFinishedString(VersionCheck_StatusErrorFound_String));
	m_pGeneralStatusText->Invalidate();
	m_bShowBackMsg = false;
}

//----------------------------------------------------------------------------------
// EnableControls: Enable or disable the controls as aproproiate.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::EnableControls(void)
{
	DetermineControlStates();

	// Back text.
	m_pBackText->SetVisible(m_bShowBackMsg);
	if (! m_bShowBackMsg)
	{
		RequestTimer(false);
		m_pGeneralStatusText->SetVisible(true);
	}

	// Next Button.
	m_pNextButton->Enable(m_bEnableNextButton);
	if (m_bEnableNextButton)
	{
		// Make sure the Next button is the default button.
		m_pNextButton->RequestFocus();

		// If we are in automatic mode, continue.
		if (GetCustomInfo()->GetAutomaticMode() && IsVisible())
			FireNextButton();
	}
}

//----------------------------------------------------------------------------------
// UpdateInfoText: Update the main information text.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::UpdateInfoText(void)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();
	PatchCore* pCore = GetMainControl()->GetPatchCore();
	GUIString sHorizSeparator = "  ";
	GUIString sVertSeparator = "\n\n";

	// Start with the standard information text.
	GUIString sInfo = pResMgr->BuildInfoString(VersionCheck_Info1_String_Id);

	// DNS test.
	if (pCore->GetDnsTestOp()->Started())
	{
		sInfo.append(sVertSeparator);
		sInfo.append(pResMgr->GetFinishedString(VersionCheck_DNSTestStarted_String));
		sInfo.append(sHorizSeparator);
		sInfo.append(pCore->GetDnsTestOp()->m_sResult);
	}

	// MotD download
	if (pCore->GetMotdDownloadOp()->Started())
	{
		// Add the MotD.
		sInfo.append(sVertSeparator);
		sInfo.append(pResMgr->GetFinishedString(VersionCheck_MotdLookupStarted_String));
		sInfo.append(sHorizSeparator);
		sInfo.append(pCore->GetMotdDownloadOp()->m_sResult);
	}

	// Server lookup.
	if (pCore->GetServerLookupOp()->Started())
	{
		sInfo.append(sVertSeparator);
		sInfo.append(pResMgr->GetFinishedString(VersionCheck_ServerLookupStarted_String));
		sInfo.append(sHorizSeparator);
		sInfo.append(pCore->GetServerLookupOp()->m_sResult);
	}

	// Self (SierraUp) version check.
	if (pCore->GetSelfVersionCheckOp()->Started())
	{
		sInfo.append(sVertSeparator);
		sInfo.append(pResMgr->GetFinishedString(VersionCheck_SelfVersionCheckStarted_String));
		sInfo.append(sHorizSeparator);
		sInfo.append(pCore->GetSelfVersionCheckOp()->m_sResult);
	}

	// Game version check.
	if (pCore->GetGameVersionCheckOp()->Started())
	{
		sInfo.append(sVertSeparator);
		sInfo.append(pResMgr->GetFinishedString(VersionCheck_GameVersionCheckStarted_String));
		sInfo.append(sHorizSeparator);
		sInfo.append(pCore->GetGameVersionCheckOp()->m_sResult);
	}

	m_pInfoText->Clear();
	m_pInfoText->AddFormatedText(sInfo);
}

//----------------------------------------------------------------------------------
// UpdateControls: Update the contents of the general status text to reflect what
// we have found out.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::UpdateControls(void)
{
	// Update the state of where we are.
	UpdateInfoText();

	// Determine what controls should be set how.
	EnableControls();
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void VersionCheckCtrl::Show(bool bShow)
{
	if (bShow)
	{
		// Back button gets default unless Next is enabled (handled in EnableControls)
		m_pBackButton->RequestFocus();

		// Some of the contents change dynamically, update them.
		UpdateControls();
		EnableControls();
	}

	SetVisible(bShow);
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help Button.
//----------------------------------------------------------------------------------
bool VersionCheckCtrl::HandleHelpButton(ComponentEvent* pEvent)
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
// HandleBackButton: Process a click on the Back Button.
//----------------------------------------------------------------------------------
bool VersionCheckCtrl::HandleBackButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowPreviousScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleNextButton: Process a click on the Next Button.
//----------------------------------------------------------------------------------
bool VersionCheckCtrl::HandleNextButton(ComponentEvent* pEvent)
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
void VersionCheckCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}

//----------------------------------------------------------------------------------
// TimerEvent: Handle a timer event.
//----------------------------------------------------------------------------------
bool VersionCheckCtrl::TimerEvent(int tDelta)
{
	bool wantTimer = Container::TimerEvent(tDelta);
	if(mTimerFlags==TimerFlag_IWant)
	{
		wantTimer = true;
		int tNow = GetTickCount();

		if (IsVisible() && (tNow - m_tLastBlink > 350))
		{
			m_pGeneralStatusText->SetVisible(! m_pGeneralStatusText->IsVisible());
			m_pGeneralStatusText->Invalidate();
			m_tLastBlink = tNow;
		}
	}

	return wantTimer;
}
