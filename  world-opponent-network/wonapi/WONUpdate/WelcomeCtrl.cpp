//----------------------------------------------------------------------------------
// WelcomeCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "WelcomeCtrl.h"
#include "WONUpdateCtrl.h"
#include "MessageDlg.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// WelcomeCtrl Constructor.
//----------------------------------------------------------------------------------
WelcomeCtrl::WelcomeCtrl(void)
	: WizardCtrl()
{
}

//----------------------------------------------------------------------------------
// WelcomeCtrl Destructor.
//----------------------------------------------------------------------------------
WelcomeCtrl::~WelcomeCtrl(void)
{
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void WelcomeCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pConfigProxyButton,"ConfigureProxyButton");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pBackButton,"CancelButton");
		WONComponentConfig_Get(aConfig,m_pNextButton,"NextButton");

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetWelcomeHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void WelcomeCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		// Some of the text changes dynamically based on patch selection, update it.
		ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

		// Informational (Main) text.
		GUIString sInfo = pResMgr->BuildInfoString(Welcome_Info1_String_Id);
		m_pInfoText->Clear();
		m_pInfoText->AddFormatedText(sInfo);
		m_pInfoText->SetVertOffset(0); // Scroll to the top.

		// Make sure the Next button is the default button.
		m_pNextButton->RequestFocus();
	}

	// No threads used in this dialog.
}

//----------------------------------------------------------------------------------
// HandleConfigProxyButton: Process a click on the Confgure Proxy Button.
//----------------------------------------------------------------------------------
bool WelcomeCtrl::HandleConfigProxyButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	if (pUpdateControl)
		pUpdateControl->ShowScreen(WONUpdateCtrl::WS_ConfigProxy);

	return true;
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help Button.
//----------------------------------------------------------------------------------
bool WelcomeCtrl::HandleHelpButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

#ifdef _DEBUG
	if (GetKeyState(VK_SHIFT) & 0x8000)
	{
		int nRes = MessageBox(GetWindow(), "OK Message Box Test\n\nLine 3\nLine 4\n\nLine 6 (the most evil i i i i line)", "Ok Test", MD_OK);

		char sLine[333]; 
		wsprintf(sLine, "OK-Cancel Message Box Test\n\nPrev Result = %d\n\nLine 3\nLine 4\n\nLine 6 (the most evil i i i i line)", nRes);
		nRes = MessageBox(GetWindow(), sLine, "Ok/Cancel Test", MD_OKCANCEL);

		wsprintf(sLine, "Yes-No Message Box Test\n\nPrev Result = %d\n\nLine 3\nLine 4\n\nLine 6 (the most evil i i i i line)", nRes);
		nRes = MessageBox(GetWindow(), sLine, "Yes/No Test", MD_YESNO);

		wsprintf(sLine, "Yes-No-Cancel Message Box Test\n\nPrev Result = %d\n\nLine 3\nLine 4\n\nLine 6 (the most evil i i i i line)\n\nLine 10\nLine 11\nLine 12", nRes);
		nRes = MessageBox(GetWindow(), sLine, "Yes/No/Cancel Test", MD_YESNOCANCEL);

		wsprintf(sLine, "Abort Message Box Test\n\nPrev Result = %d\n\nLine 3\nLine 4\n\nLine 6 (the most evil, hideous and loathsome i i i i line)", nRes);
		nRes = MessageBox(GetWindow(), sLine, "Abort Test", MD_ABORT);

		wsprintf(sLine, "OK Message Box Test (number two)\n\nPrev Result = %d\n\nLine 3\nLine 4\n\nLine 6 (the most evil, hideous and loathsome i i i i line)", nRes);
		MessageBox(GetWindow(), sLine, "Ok Re-Test", MD_OK);
	}
#endif

	// Invoke the Help callback for this dialog.
	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowHelp();

	return true;
}

//----------------------------------------------------------------------------------
// HandleBackButton: Process a click on the Back (Cancel) Button.
//----------------------------------------------------------------------------------
bool WelcomeCtrl::HandleBackButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	GetWindow()->Close();

	return true;
}

//----------------------------------------------------------------------------------
// HandleNextButton: Process a click on the Next (Continue) Button.
//----------------------------------------------------------------------------------
bool WelcomeCtrl::HandleNextButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	if (pUpdateControl)
		pUpdateControl->ShowNextScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void WelcomeCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)) ||
		(pEvent->mComponent == m_pConfigProxyButton && HandleConfigProxyButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}
