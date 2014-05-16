#include "LoginStatusCtrl.h"

#include "LobbyEvent.h"

#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoginStatusCtrl::LoginStatusCtrl()
{
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginStatusCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mStatusLabel,"StatusLabel");
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginStatusCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==LobbyEvent_LoginInitStatus)
	{
		mStatusLabel->SetText(((LoginInitStatusEvent*)theEvent)->mStatus);
		return;
	}
	else
		Container::HandleComponentEvent(theEvent);
}

