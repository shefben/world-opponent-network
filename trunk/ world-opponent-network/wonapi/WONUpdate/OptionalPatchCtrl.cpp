//----------------------------------------------------------------------------------
// OptionalPatchCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "OptionalPatchCtrl.h"
#include "WONUpdateCtrl.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// OptionalPatchCtrl Constructor.
//----------------------------------------------------------------------------------
OptionalPatchCtrl::OptionalPatchCtrl(void)
	: WizardCtrl()
{
}

//----------------------------------------------------------------------------------
// OptionalPatchCtrl Destructor.
//----------------------------------------------------------------------------------
OptionalPatchCtrl::~OptionalPatchCtrl(void)
{
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void OptionalPatchCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pPatchDetailsButton,"PatchDetailsButton");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,m_pNextButton,"NextButton");

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetWelcomeHelpCallback() != NULL);
	}
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void OptionalPatchCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		// Some of the text changes dynamically based on patch selection, update it.
		ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

		// Informational (Main) text.
		GUIString sInfo = pResMgr->BuildInfoString(OptionalPatch_Info1_String_Id);
		m_pInfoText->Clear();
		m_pInfoText->AddFormatedText(sInfo);
		m_pInfoText->SetVertOffset(0); // Scroll to the top.

		// Make sure the Next button is the default button.
		m_pNextButton->RequestFocus();

		CustomInfo* pCI = GetCustomInfo();
		m_pPatchDetailsButton->Enable(pCI->GetPatchDescriptionUrl() != "");
	}

	// No threads used in this dialog.
}

//----------------------------------------------------------------------------------
// HandlePatchDetailsButton: Process a click on the Patch Details Button.
//----------------------------------------------------------------------------------
bool OptionalPatchCtrl::HandlePatchDetailsButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowScreen(WONUpdateCtrl::WS_PatchDetails);

	return true;
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help Button.
//----------------------------------------------------------------------------------
bool OptionalPatchCtrl::HandleHelpButton(ComponentEvent* pEvent)
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
bool OptionalPatchCtrl::HandleBackButton(ComponentEvent* pEvent)
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
bool OptionalPatchCtrl::HandleNextButton(ComponentEvent* pEvent)
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
void OptionalPatchCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pBackButton && HandleBackButton(pEvent)) ||
		(pEvent->mComponent == m_pNextButton && HandleNextButton(pEvent)) ||
		(pEvent->mComponent == m_pPatchDetailsButton && HandlePatchDetailsButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}
