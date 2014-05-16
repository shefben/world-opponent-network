//----------------------------------------------------------------------------------
// LoginCtrl.cpp
//----------------------------------------------------------------------------------
#include <assert.h>
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/TabCtrl.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"
#include "LoginCtrl.h"
#include "WONUpdateCtrl.h"
#include "MessageDlg.h"
#include "CustomInfo.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// LoginCtrl Constructor.
//----------------------------------------------------------------------------------
LoginCtrl::LoginCtrl(void)
{
}

//----------------------------------------------------------------------------------
// LoginCtrl Destructor.
//----------------------------------------------------------------------------------
LoginCtrl::~LoginCtrl(void)
{
}

//----------------------------------------------------------------------------------
// InitComponent: Initialize all of the sub-controls.
//----------------------------------------------------------------------------------
void LoginCtrl::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,m_pLoginNameInputBox,"LoginNameInputBox");
		WONComponentConfig_Get(aConfig,m_pPasswordInputBox,"PasswordInputBox");
		WONComponentConfig_Get(aConfig,m_pLoginButton,"LoginButton");
		WONComponentConfig_Get(aConfig,m_pCancelButton,"CancelButton");
		WONComponentConfig_Get(aConfig,m_pText,"NeedLoginLabel");
	}
}

//----------------------------------------------------------------------------------
// DoDialog: Display this dialog as a modal dialog.
//----------------------------------------------------------------------------------
int LoginCtrl::DoDialog(const GUIString& theTitle, const GUIString& theText, Window *pParent)
{
	PlatformWindowPtr pWindow = new PlatformWindow;
	pWindow->SetParent(pParent);
	pWindow->SetTitle(theTitle); 
	m_pText->SetText(theText);
	m_pText->SetDesiredSize();
	pWindow->SetCreateFlags(CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable);
	RecalcLayout();

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
	m_pLoginNameInputBox->RequestFocus();

	int nResult = pWindow->DoDialog(this);
	pWindow->Close();

	return nResult;
}

