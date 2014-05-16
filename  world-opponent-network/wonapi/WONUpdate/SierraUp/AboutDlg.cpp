//----------------------------------------------------------------------------------
// AboutDlg.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/WONGUIConfig.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "AboutDlg.h"
#include "../CustomInfo.h"
#include "../WizardCtrl.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// AboutDlg Constructor
//----------------------------------------------------------------------------------
AboutDlg::AboutDlg(void)
{
}

//----------------------------------------------------------------------------------
// AboutDlg Destructor
//----------------------------------------------------------------------------------
AboutDlg::~AboutDlg(void)
{
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void AboutDlg::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		SafeGetRequiredComponent(aConfig,m_pVersionLabel,"VersionLabel");
		SafeGetRequiredComponent(aConfig,m_pCopyrightLabel,"CopyrightLabel");
		SafeGetRequiredComponent(aConfig,m_pCloseButton,"CloseButton");

		GUIString sVersion = m_pVersionLabel->GetText();
		sVersion.append(m_sVersion);
		m_pVersionLabel->SetText(sVersion);

		SetEndOnEscape(1);
	}
}

//----------------------------------------------------------------------------------
// DoDialog: Display this dialog as a modal dialog.
//----------------------------------------------------------------------------------
int AboutDlg::DoDialog(Window *pParent)
{
	PlatformWindowPtr pWindow = new PlatformWindow;
	pWindow->SetParent(pParent);
	pWindow->SetTitle(GetCustomInfo()->GetResourceManager()->GetString(IDS_ABOUT_DLG_TITLE));
	pWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable);

	WONRectangle aRect;
	if (pParent != NULL)
		pParent->GetScreenPos(aRect);
	else
		pWindow->GetWindowManager()->GetScreenRect(aRect);

	// Center it on top of its parent (or the desktop).
	pWindow->Create(WONRectangle(aRect.Left() + (aRect.Width() - Width()) / 2,
								 aRect.Top() + (aRect.Height() - Height()) / 2,
								 Width(), Height()));

	pWindow->AddComponent(this);
	m_pCloseButton->RequestFocus();

	int nResult = pWindow->DoDialog(this);
	pWindow->Close();

	return nResult;
}

//----------------------------------------------------------------------------------
// HandleCloseButton: Close the dialog box.
//----------------------------------------------------------------------------------
bool AboutDlg::HandleCloseButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	GetWindow()->EndDialog();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void AboutDlg::HandleComponentEvent(ComponentEvent* pEvent)
{
	if (pEvent->mComponent == m_pCloseButton && HandleCloseButton(pEvent))
		return;

	MSDialog::HandleComponentEvent(pEvent);
}
