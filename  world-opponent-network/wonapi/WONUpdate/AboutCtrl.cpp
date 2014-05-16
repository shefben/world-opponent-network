//----------------------------------------------------------------------------------
// AboutCtrl.cpp
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
// AboutCtrl Constructor.
//----------------------------------------------------------------------------------
AboutCtrl::AboutCtrl(void)
{
	m_IsVersionSet = false;
}

//----------------------------------------------------------------------------------
// AboutCtrl Destructor.
//----------------------------------------------------------------------------------
AboutCtrl::~AboutCtrl(void)
{
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void AboutCtrl::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,m_pVersionLabel,"VersionLabel");
		WONComponentConfig_Get(aConfig,m_pCopyrightLabel,"CopyrightLabel");
		WONComponentConfig_Get(aConfig,m_pCloseButton,"CloseButton");
	}
}

//----------------------------------------------------------------------------------
// DoDialog: Display this dialog as a modal dialog.
//----------------------------------------------------------------------------------
int AboutCtrl::DoDialog(Window *pParent)
{
	if(!m_IsVersionSet)
	{
		GUIString sVersion = m_pVersionLabel->GetText();
		sVersion.append(m_sVersion);
		m_pVersionLabel->SetText(sVersion);
		m_IsVersionSet = true;
	}

	PlatformWindowPtr pWindow = new PlatformWindow;
	pWindow->SetParent(pParent);
	pWindow->SetTitle(GetCustomInfo()->GetResourceManager()->GetFinishedString(About_Title_String));
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
bool AboutCtrl::HandleCloseButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return false;

	GetWindow()->EndDialog();

	return true;
}

//----------------------------------------------------------------------------------
// HandleComponentEvent: Handle Component Events (Button pushes and the like).
//----------------------------------------------------------------------------------
void AboutCtrl::HandleComponentEvent(ComponentEvent* pEvent)
{
	if (pEvent->mComponent == m_pCloseButton && HandleCloseButton(pEvent))
		return;

	MSDialog::HandleComponentEvent(pEvent);
}
