//----------------------------------------------------------------------------------
// ConfigProxyCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/ImageDecoder.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"

#include "ConfigProxyCtrl.h"
#include "WONUpdateCtrl.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// ConfigProxyCtrl Constructor.
//----------------------------------------------------------------------------------
ConfigProxyCtrl::ConfigProxyCtrl(void)
	: WizardCtrl()
{
}

//----------------------------------------------------------------------------------
// ConfigProxyCtrl Destructor.
//----------------------------------------------------------------------------------
ConfigProxyCtrl::~ConfigProxyCtrl(void)
{
}

//----------------------------------------------------------------------------------
// LoadProxySettings: Fetch the proxy settings from the API (its buried deep in 
// there).
//----------------------------------------------------------------------------------
void ConfigProxyCtrl::LoadProxySettings(void)
{
	HTTPGetOp::ReadProxyHostFile();
	std::string sProxy = HTTPGetOp::GetStaticProxyHost();
	
	if (sProxy == "")
	{
		m_pUseProxyCheckBox->SetCheck(false);
		m_pHostInputBox->SetText("");
		m_pPortInputBox->SetText("");
	}
	else
	{
		std::string sHost = sProxy;
		std::string sPort = sProxy;

		char nPos = sHost.rfind(':');
		if (nPos != -1)
		{
			sHost.erase(nPos, sHost.length() - nPos);
			sPort.erase(0, nPos + 1);
		}
		else
			sPort = "80";

		m_pUseProxyCheckBox->SetCheck(true);
		m_pHostInputBox->SetText(sHost);
		m_pPortInputBox->SetText(sPort);
	}
}

//----------------------------------------------------------------------------------
// SaveProxySettings: Stuff the proxy setting back into the API (which will save 
// them for us).
//----------------------------------------------------------------------------------
void ConfigProxyCtrl::SaveProxySettings(void)
{
	std::string sHost;
	std::string sPort;
	
	if (m_pUseProxyCheckBox->IsChecked())
	{
		sHost = m_pHostInputBox->GetText();
		sPort = m_pPortInputBox->GetText();
	}

	std::string sProxy = sHost;
	if (sPort != "")
		sProxy += ":" + sPort;

	HTTPGetOp::WriteProxyHostFile(sProxy);
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void ConfigProxyCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,m_pInfoText,"InformationText");
		WONComponentConfig_Get(aConfig,m_pUseProxyCheckBox,"UseProxyCheckBox");
		WONComponentConfig_Get(aConfig,m_pHostLabel,"HostLabel");
		WONComponentConfig_Get(aConfig,m_pHostInputBox,"HostInputBox");
		WONComponentConfig_Get(aConfig,m_pPortLabel,"PortLabel");
		WONComponentConfig_Get(aConfig,m_pPortInputBox,"PortInputBox");
		WONComponentConfig_Get(aConfig,m_pHelpButton,"HelpButton");
		WONComponentConfig_Get(aConfig,m_pCancelButton,"CancelButton");
		WONComponentConfig_Get(aConfig,m_pOkButton,"OkButton");

		LoadProxySettings();

		// Show or hide the Help button as appropriate.
		m_pHelpButton->SetVisible(GetCustomInfo()->GetConfigProxyHelpCallback() != NULL);

		EnableControls();
	}
}

//----------------------------------------------------------------------------------
// EnableControls: Enable or disable the controls as aproproiate.
//----------------------------------------------------------------------------------
void ConfigProxyCtrl::EnableControls(void)
{
	bool bUseProxy = m_pUseProxyCheckBox->IsChecked();

	m_pHostLabel->Enable(bUseProxy);
	m_pHostInputBox->Enable(bUseProxy);
	m_pPortLabel->Enable(bUseProxy);
	m_pPortInputBox->Enable(bUseProxy);
}

//----------------------------------------------------------------------------------
// Show: Make this dialog visible (or invisible), and start (or stop) any threads 
// that are needed.
//----------------------------------------------------------------------------------
void ConfigProxyCtrl::Show(bool bShow)
{
	SetVisible(bShow);

	if (bShow)
	{
		// Some of the text changes dynamically based on patch selection, update it.
		ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

		// Informational (Main) text.
		GUIString sInfo = pResMgr->BuildInfoString(ConfigProxy_Info1_String_Id);
		m_pInfoText->Clear();
		m_pInfoText->AddFormatedText(sInfo);
		m_pInfoText->SetVertOffset(0); // Scroll to the top.

		// Make sure the Next button is the default button.
		m_pOkButton->RequestFocus();
	}

	// No threads to worry about in this dialog.
}

//----------------------------------------------------------------------------------
// HandleUseProxyCheckBox: Process a click on the Use Proxt Check Box.
//----------------------------------------------------------------------------------
bool ConfigProxyCtrl::HandleUseProxyCheckBox(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	EnableControls();

	return true;
}

//----------------------------------------------------------------------------------
// HandleHelpButton: Process a click on the Help Button.
//----------------------------------------------------------------------------------
bool ConfigProxyCtrl::HandleHelpButton(ComponentEvent* pEvent)
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
// HandleCancelButton: Process a click on the Cancel Button.
//----------------------------------------------------------------------------------
bool ConfigProxyCtrl::HandleCancelButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowPreviousScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleOkButton: Process a click on the Ok Button.
//----------------------------------------------------------------------------------
bool ConfigProxyCtrl::HandleOkButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	// Save the user's settings.
	SaveProxySettings();

	WONUpdateCtrl* pUpdateControl = reinterpret_cast<WONUpdateCtrl*>(GetParent());
	assert(pUpdateControl);
	pUpdateControl->ShowPreviousScreen();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void ConfigProxyCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if ((pEvent->mComponent == m_pUseProxyCheckBox && HandleUseProxyCheckBox(pEvent)) ||
		(pEvent->mComponent == m_pHelpButton && HandleHelpButton(pEvent)) ||
		(pEvent->mComponent == m_pCancelButton && HandleCancelButton(pEvent)) ||
		(pEvent->mComponent == m_pOkButton && HandleOkButton(pEvent)))
	{
		return;
	}

	Container::HandleComponentEvent(pEvent);
}
