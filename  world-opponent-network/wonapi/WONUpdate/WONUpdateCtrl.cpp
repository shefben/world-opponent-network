//----------------------------------------------------------------------------------
// WONUpdateCtrl.cpp
//----------------------------------------------------------------------------------
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/WONGUIConfig/WONGUIConfig.h"
#include "WONGUI/WONGUIConfig/ResourceConfig.h"
#include "WONGUI/WONGUIConfig/BrowserComponentConfig.h"
#include "WONUpdateCtrl.h"
#include "MessageDlg.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Local Variables.
//----------------------------------------------------------------------------------
WONUpdateCtrl* g_pUpdateCtrl = NULL;


//----------------------------------------------------------------------------------
// GetMainControl: Return a pointer to the main control.
//----------------------------------------------------------------------------------
extern "C" WONAPI::WONUpdateCtrl* GetMainControl(void)
{
	return g_pUpdateCtrl;
}

//----------------------------------------------------------------------------------
// WONUpdateCtrl Constructor.
//----------------------------------------------------------------------------------
WONUpdateCtrl::WONUpdateCtrl(void)
{
	g_pUpdateCtrl = this;
	m_CurrentScreen = WS_None;
	m_PreviousScreen = WS_None;
	m_pFinishedProc = NULL;
	m_pTitleChangedProc = NULL;
	m_nReturnResult = SUR_UNKNOWN;
}

//----------------------------------------------------------------------------------
// WONUpdateCtrl Destructor.
//----------------------------------------------------------------------------------
WONUpdateCtrl::~WONUpdateCtrl(void)
{
	g_pUpdateCtrl = NULL;
	m_pFinishedProc = NULL;
}

//----------------------------------------------------------------------------------
// Finish: SierraUpdate is finished.  Either inform our creator (if they set the 
// callback) or close down.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::Finish(int nReturnCode)
{
	if (m_pFinishedProc)
		m_pFinishedProc(nReturnCode);
	else
		GetWindow()->Close();
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void WONUpdateCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
/*
	if((theEvent->mType==ComponentEvent_InputReturn && theEvent->mComponent==mChatInput)
		|| (theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent==mSendButton))
	{
		if(mChatInput->GetText().empty())
			return;

		mChatOutput->AddSegment("Brian: ",0x800000);
		mChatOutput->AddSegment(mChatInput->GetText(),true);
		mChatInput->AddInputHistory(mChatInput->GetText());
		mChatInput->SetText("");
	}
	else
*/
		Container::HandleComponentEvent(pEvent);
}

//----------------------------------------------------------------------------------
// SizeChanged: Handle a Size Changed message.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::SizeChanged(void)
{
	Container::SizeChanged();
}

//----------------------------------------------------------------------------------
// SetTitleChangedCallback: Set the callback to invoke when the title changes.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::SetTitleChangedCallback(SierraUpTitleChangedCallbackProc pProc)
{
	m_pTitleChangedProc = pProc;
	UpdateTitle(m_CurrentScreen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceConfigTablePtr WONUpdateCtrl::InitScreens(void)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	ConfigParser aParser;
	ResourceConfigTablePtr aTable = GetResourceTable();
	UpdateResource_Init(aTable);
	ComponentConfigPtr aConfig = new ContainerConfig;

	if(!(GetCustomInfo()->GetResourceDirectory().empty()))
		ComponentConfig::SetResourceFolder(GetCustomInfo()->GetResourceDirectory());

	aConfig->ReadConfig(this,"_UpdateMaster.cfg",aTable,&aParser);

	WONComponentConfig_Get(aConfig,m_pWelcomeCtrl,"WelcomeCtrl");
	AddChildLayout(m_pWelcomeCtrl, CLI_SameSize, this);
	AddChild(m_pWelcomeCtrl);

	WONComponentConfig_Get(aConfig,m_pConfigProxyCtrl,"ConfigProxyCtrl");
	AddChildLayout(m_pConfigProxyCtrl, CLI_SameSize, this);
	AddChild(m_pConfigProxyCtrl);

	WONComponentConfig_Get(aConfig,m_pMotdCtrl,"MotdCtrl");
	AddChildLayout(m_pMotdCtrl, CLI_SameSize, this);
	AddChild(m_pMotdCtrl);

	WONComponentConfig_Get(aConfig,m_pVersionCheckCtrl,"VersionCheckCtrl");
	AddChildLayout(m_pVersionCheckCtrl, CLI_SameSize, this);
	AddChild(m_pVersionCheckCtrl);

	WONComponentConfig_Get(aConfig,m_pOptionalPatchCtrl,"OptionalPatchCtrl");
	AddChildLayout(m_pOptionalPatchCtrl, CLI_SameSize, this);
	AddChild(m_pOptionalPatchCtrl);

	WONComponentConfig_Get(aConfig,m_pPatchDetailsCtrl,"PatchDetailsCtrl");
	AddChildLayout(m_pPatchDetailsCtrl, CLI_SameSize, this);
	AddChild(m_pPatchDetailsCtrl);

	WONComponentConfig_Get(aConfig,m_pSelectHostCtrl,"SelectHostCtrl");
	AddChildLayout(m_pSelectHostCtrl, CLI_SameSize, this);
	AddChild(m_pSelectHostCtrl);

	WONComponentConfig_Get(aConfig,m_pDownloadCtrl,"DownloadCtrl");
	AddChildLayout(m_pDownloadCtrl, CLI_SameSize, this);
	AddChild(m_pDownloadCtrl);

//@@@	WONComponentConfig_Get(aConfig,m_pTukatiDownloadCtrl,"TukatiDownloadCtrl");
//	AddChildLayout(m_pTukatiDownloadCtrl, CLI_SameSize, this);
//	AddChild(m_pTukatiDownloadCtrl);

//@@@	WONComponentConfig_Get(aConfig,m_pTukatiSpeedPassInstallCtrl,"TukatiSpeedPassInstallCtrl");

	WONComponentConfig_Get(aConfig,m_pWebLaunchCtrl,"WebLaunchCtrl");
	AddChildLayout(m_pWebLaunchCtrl, CLI_SameSize, this);
	AddChild(m_pWebLaunchCtrl);

	WONComponentConfig_Get(aConfig,m_pAboutCtrl,"AboutCtrl");
	WONComponentConfig_Get(aConfig,m_pLoginCtrl,"LoginCtrl");
	m_pDownloadCtrl->SetLoginCtrl(m_pLoginCtrl);

	ShowScreen(WS_Welcome);

	return aTable;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace 
{
	Component* WelcomeCtrlFactory() { return new WelcomeCtrl; }
	Component* ConfigProxyCtrlFactory() { return new ConfigProxyCtrl; }
	Component* MotdCtrlFactory() { return new MotdCtrl; }
	Component* VersionCheckCtrlFactory() { return new VersionCheckCtrl; }
	Component* OptionalPatchCtrlFactory() { return new OptionalPatchCtrl; }
	Component* PatchDetailsCtrlFactory() { return new PatchDetailsCtrl; }
	Component* SelectHostCtrlFactory() { return new SelectHostCtrl; }
	Component* DownloadCtrlFactory() { return new DownloadCtrl; }
//@@@	Component* TukatiDownloadCtrlFactory() { return new TukatiDownloadCtrl; }
//	Component* TukatiSpeedPassInstallCtrlFactory() { return new TukatiSpeedPassInstallCtrl; }
	Component* WebLaunchCtrlFactory() { return new WebLaunchCtrl; }
	Component* AboutCtrlFactory() { return new AboutCtrl; }
	Component* LoginCtrlFactory() { return new LoginCtrl; }
	Component* ProgressBarComponentFactory() { return new ProgressBarComponent; }
}

ResourceConfigTablePtr WONUpdateCtrl::GetResourceTable()
{
	ResourceConfigTablePtr aConfigTableP = new ResourceConfigTable;
	WONGUIConfig_Init(aConfigTableP);	
	BrowserComponentConfig::Init(aConfigTableP);

	aConfigTableP->AddAllocator("WelcomeCtrl",ContainerConfig::CfgFactory,WelcomeCtrlFactory);
	aConfigTableP->AddAllocator("ConfigProxyCtrl",ContainerConfig::CfgFactory,ConfigProxyCtrlFactory);
	aConfigTableP->AddAllocator("MotdCtrl",ContainerConfig::CfgFactory,MotdCtrlFactory);
	aConfigTableP->AddAllocator("VersionCheckCtrl",ContainerConfig::CfgFactory,VersionCheckCtrlFactory);
	aConfigTableP->AddAllocator("OptionalPatchCtrl",ContainerConfig::CfgFactory,OptionalPatchCtrlFactory);
	aConfigTableP->AddAllocator("PatchDetailsCtrl",ContainerConfig::CfgFactory,PatchDetailsCtrlFactory);
	aConfigTableP->AddAllocator("SelectHostCtrl",ContainerConfig::CfgFactory,SelectHostCtrlFactory);
	aConfigTableP->AddAllocator("DownloadCtrl",ContainerConfig::CfgFactory,DownloadCtrlFactory);
//@@@	aConfigTableP->AddAllocator("TukatiDownloadCtrl",ContainerConfig::CfgFactory,TukatiDownloadCtrlFactory);
//	aConfigTableP->AddAllocator("TukatiSpeedPassInstallCtrl",DialogConfig::CfgFactory,TukatiSpeedPassInstallCtrlFactory);
	aConfigTableP->AddAllocator("WebLaunchCtrl",ContainerConfig::CfgFactory,WebLaunchCtrlFactory);
	aConfigTableP->AddAllocator("AboutCtrl",DialogConfig::CfgFactory,AboutCtrlFactory);
	aConfigTableP->AddAllocator("LoginCtrl",DialogConfig::CfgFactory,LoginCtrlFactory);
	aConfigTableP->AddAllocator("ProgressBar",ProgressComponentConfig::CfgFactory,ProgressBarComponentFactory);
	return aConfigTableP;
}

//----------------------------------------------------------------------------------
// UpdateTitle: Sends the appropriarte title to the owning application.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::UpdateTitle(WIZARD_SCREEN Screen)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	GUIString sTitle;
	switch (Screen)
	{
		case WS_Welcome:       sTitle = pResMgr->GetFinishedString(Welcome_Title_String);       break;
		case WS_ConfigProxy:   sTitle = pResMgr->GetFinishedString(ConfigProxy_Title_String);   break;
		case WS_Motd:          sTitle = pResMgr->GetFinishedString(Motd_Title_String);          break;
		case WS_VersionCheck:  sTitle = pResMgr->GetFinishedString(VersionCheck_Title_String);  break;
		case WS_OptionalPatch: sTitle = pResMgr->GetFinishedString(OptionalPatch_Title_String); break;
		case WS_PatchDetails:  sTitle = pResMgr->GetFinishedString(PatchDetails_Title_String);  break;
		case WS_SelectHost:    sTitle = pResMgr->GetFinishedString(SelectHost_Title_String);    break;
		case WS_Download:      sTitle = pResMgr->GetFinishedString(Download_Title_String);      break;
//@@@		case WS_TukatiDownload:sTitle = pResMgr->GetFinishedString(Tukati_Title_String);		break;
		case WS_WebLaunch:     sTitle = pResMgr->GetFinishedString(VisitHost_Title_String);     break;
	}

	if (m_pTitleChangedProc)
		m_pTitleChangedProc(sTitle);
}

//----------------------------------------------------------------------------------
// GetScreenCtrl: Return the screen control that is visible.
//----------------------------------------------------------------------------------
WizardCtrl* WONUpdateCtrl::GetScreenCtrl(void)
{
	switch (m_CurrentScreen)
	{
		case WS_Welcome:       return m_pWelcomeCtrl;
		case WS_ConfigProxy:   return m_pConfigProxyCtrl;
		case WS_Motd:          return m_pMotdCtrl;
		case WS_VersionCheck:  return m_pVersionCheckCtrl;
		case WS_OptionalPatch: return m_pOptionalPatchCtrl;
		case WS_PatchDetails:  return m_pPatchDetailsCtrl;
		case WS_SelectHost:    return m_pSelectHostCtrl;
		case WS_Download:      return m_pDownloadCtrl;
//@@@		case WS_TukatiDownload:return m_pTukatiDownloadCtrl;
		case WS_WebLaunch:     return m_pWebLaunchCtrl;
		default:               return NULL;
	}
}

//----------------------------------------------------------------------------------
// ShowScreen: Display the specified dialog.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::ShowScreen(WIZARD_SCREEN NewScreen)
{
	m_pWelcomeCtrl->Show(NewScreen == WS_Welcome);
	m_pConfigProxyCtrl->Show(NewScreen == WS_ConfigProxy);
	m_pMotdCtrl->Show(NewScreen == WS_Motd);
	m_pVersionCheckCtrl->Show(NewScreen == WS_VersionCheck);
	m_pOptionalPatchCtrl->Show(NewScreen == WS_OptionalPatch);
	m_pPatchDetailsCtrl->Show(NewScreen == WS_PatchDetails);
	m_pSelectHostCtrl->Show(NewScreen == WS_SelectHost);
	m_pWebLaunchCtrl->Show(NewScreen == WS_WebLaunch);
//@@@	m_pTukatiDownloadCtrl->Show(NewScreen == WS_TukatiDownload);
	m_pDownloadCtrl->Show(NewScreen == WS_Download);

	m_PreviousScreen = m_CurrentScreen;
	m_CurrentScreen = NewScreen;

	UpdateTitle(NewScreen);

	Invalidate();
}

//----------------------------------------------------------------------------------
// ShowNextScreen: Handle a click on the 'Next' button.  The dialog will enable it 
// when it is a valid option.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::ShowNextScreen(void)
{
	CustomInfo* pCI = GetCustomInfo();

	switch (m_CurrentScreen)
	{
		case WS_Welcome:        ShowScreen(WS_Motd);          break;
		case WS_ConfigProxy:    ShowScreen(m_PreviousScreen); break;
		case WS_Motd:           ShowScreen(WS_VersionCheck);  break;
		case WS_VersionCheck:
			if (m_PatchCore.GetGameVersionCheckResult() == SUR_UP_TO_DATE)
				Finish(SUR_UP_TO_DATE);
			else if (pCI->GetOptionalUpgrade())
				ShowScreen(WS_OptionalPatch);
			else
				ShowScreen(WS_SelectHost);
			break;
		case WS_OptionalPatch: ShowScreen(WS_SelectHost);     break;
		case WS_PatchDetails:  ShowScreen(WS_SelectHost);     break;
		case WS_SelectHost:
			if (pCI->GetSelectedPatch() && pCI->GetSelectedPatch()->GetMustVisitHost())
				ShowScreen(WS_WebLaunch);
//@@@			else if (pCI->GetSelectedPatch() && pCI->GetSelectedPatch()->IsTukati())
//				ShowScreen(WS_TukatiDownload);
			else
				ShowScreen(WS_Download); 
			break;
		case WS_Download:
			if (m_nReturnResult == SUR_START_OVER)
			{
				// Reload everything ??? - Next Version
//				ShowScreen(WS_Welcome);
				ShowScreen(WS_Motd);
			}
			else
				Finish(m_nReturnResult);       
			break;
		case WS_WebLaunch:     Finish(SUR_MANUAL_DOWNLOAD);   break;
	}
}

//----------------------------------------------------------------------------------
// ShowPreviousScreen: Handle a click on the 'Back' button (the dialog will handle
// any shutdown/cleanup issues).
//----------------------------------------------------------------------------------
void WONUpdateCtrl::ShowPreviousScreen(void)
{
	switch (m_CurrentScreen)
	{
		case WS_Welcome:       Finish(SUR_USER_CANCEL);      break;
		case WS_ConfigProxy:   ShowScreen(m_PreviousScreen); break;
		case WS_Motd:          ShowScreen(WS_Welcome);       break;
		case WS_VersionCheck:  ShowScreen(WS_Motd);          break;
		case WS_SelectHost:
			if (GetCustomInfo()->GetOptionalUpgrade())
				ShowScreen(WS_OptionalPatch);
			else
				ShowScreen(WS_VersionCheck);
			break;
		case WS_OptionalPatch: ShowScreen(WS_Motd);          break;
		case WS_PatchDetails:  ShowScreen(WS_OptionalPatch); break;
		case WS_Download:      ShowScreen(WS_SelectHost);    break;
//@@@		case WS_TukatiDownload:ShowScreen(WS_SelectHost);    break;
		case WS_WebLaunch:     ShowScreen(WS_SelectHost);    break;
	}
}


//----------------------------------------------------------------------------------
// ShowHelp: Invoke the Help callback for the current window.
//----------------------------------------------------------------------------------
void WONUpdateCtrl::ShowHelp(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	SierraUpHelpCallbackProc pProc = NULL;

	switch (m_CurrentScreen)
	{
		case WS_Welcome:       pProc = pCustInfo->GetWelcomeHelpCallback();       break;
		case WS_ConfigProxy:   pProc = pCustInfo->GetConfigProxyHelpCallback();   break;
		case WS_Motd:          pProc = pCustInfo->GetMotdHelpCallback();          break;
		case WS_SelectHost:    pProc = pCustInfo->GetSelectHostHelpCallback();    break;
		case WS_VersionCheck:  pProc = pCustInfo->GetVersionCheckHelpCallback();  break;
		case WS_OptionalPatch: pProc = pCustInfo->GetOptionalPatchHelpCallback(); break;
		case WS_PatchDetails:  pProc = pCustInfo->GetPatchDetailsHelpCallback();  break;
		case WS_Download:      pProc = pCustInfo->GetDownloadHelpCallback();      break;
		case WS_WebLaunch:     pProc = pCustInfo->GetWebLaunchHelpCallback();     break;
	}

	if (pProc)
		pProc();
}

//----------------------------------------------------------------------------------
// KeyDown: Proces a keystroke.
//----------------------------------------------------------------------------------
bool WONUpdateCtrl::KeyDown(int nKey)
{
	// Make the escape key work like the back button.
	if (nKey == KEYCODE_ESCAPE)
	{
		switch (m_CurrentScreen)
		{
			case WS_Welcome:       m_pWelcomeCtrl->FireBackButton();       return true;
			case WS_ConfigProxy:   m_pConfigProxyCtrl->FireBackButton();   return true;
			case WS_Motd:          m_pMotdCtrl->FireBackButton();          return true;
			case WS_VersionCheck:  m_pVersionCheckCtrl->FireBackButton();  return true;
			case WS_OptionalPatch: m_pOptionalPatchCtrl->FireBackButton(); return true;
			case WS_PatchDetails:  m_pPatchDetailsCtrl->FireBackButton();  return true;
			case WS_SelectHost:    m_pSelectHostCtrl->FireBackButton();    return true;
			case WS_Download:      m_pDownloadCtrl->FireBackButton();      return true;
//@@@			case WS_TukatiDownload:m_pTukatiDownloadCtrl->FireBackButton();return true;
			case WS_WebLaunch:     m_pWebLaunchCtrl->FireBackButton();     return true;
		}
	}

	return Container::KeyDown(nKey);
}
