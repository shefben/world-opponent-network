#include "InitFailedCtrl.h"

#include "LobbyEvent.h"

#include "WONGUI/Button.h"
#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InitFailedCtrl::InitFailedCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitFailedCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mRetryButton,"RetryButton");
		WONComponentConfig_Get(aConfig,mIgnoreButton,"IgnoreButton");
		WONComponentConfig_Get(aConfig,mDescLabel,"DescLabel");

		SetVisible(false);
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitFailedCtrl::HandleLoginInitStatus(LoginInitStatusEvent *theEvent)
{
	LoginInitStatusEvent *anEvent = (LoginInitStatusEvent*)theEvent;
	if(anEvent->mStatusType==LoginInitStatus_InitStart)
		SetVisible(false);
	else if(anEvent->mStatusType==LoginInitStatus_InitFailed)
	{
		mDescLabel->SetText(theEvent->mDetailError);
		mIgnoreButton->SetVisible(theEvent->mAllowIgnore);
		EnableLayout(1,theEvent->mAllowIgnore?true:false);

		SetVisible(true);
		RecalcLayout();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InitFailedCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(LobbyEvent_LoginInitStatus==theEvent->mType)
		HandleLoginInitStatus((LoginInitStatusEvent*)theEvent);
	else if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		if(theEvent->mComponent==mRetryButton)
			FireEvent(LobbyEvent_RetryInitLogin);
		else if(theEvent->mComponent==mIgnoreButton)
			FireEvent(LobbyEvent_IgnoreInitError);
		else
			Container::HandleComponentEvent(theEvent);
	}
	else
		Container::HandleComponentEvent(theEvent);
}
