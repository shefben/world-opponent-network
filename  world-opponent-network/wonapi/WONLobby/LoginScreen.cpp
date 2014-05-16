#include "LoginScreen.h"

#include "LobbyResource.h"

#include "WONGUI/GUIConfig.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoginScreen::LoginScreen()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginScreen::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,mMOTDCtrl,"MOTDCtrl");
		WONComponentConfig_Get(aConfig,mCreateAccountCtrl,"CreateAccountCtrl");
		WONComponentConfig_Get(aConfig,mLostUserNameCtrl,"LostUserNameCtrl");
		WONComponentConfig_Get(aConfig,mLostPasswordCtrl,"LostPasswordCtrl");
		WONComponentConfig_Get(aConfig,mLoginHelpCtrl,"LoginHelpCtrl");
		WONComponentConfig_Get(aConfig,mRightPanel,"RightPanel");

		mRightPanel->ShowScreen(mMOTDCtrl,false);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginScreen::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		switch(theEvent->mComponent->GetControlId())
		{
			case ID_ShowMOTD: ShowMOTD(); return;
			case ID_ShowLoginHelp: mRightPanel->ShowScreen(mLoginHelpCtrl); return;
		}
	}

	Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginScreen::ShowMOTD()
{
	mRightPanel->ShowScreen(mMOTDCtrl); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginScreen::ShowCreateAccountCtrl()
{
	mRightPanel->ShowScreen(mCreateAccountCtrl); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginScreen::ShowLostPasswordCtrl()
{
	mRightPanel->ShowScreen(mLostPasswordCtrl); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginScreen::ShowLostUsernameCtrl()
{
	mRightPanel->ShowScreen(mLostUserNameCtrl); 
}

