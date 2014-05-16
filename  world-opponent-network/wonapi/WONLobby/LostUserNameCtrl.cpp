#include "LostUserNameCtrl.h"

#include "CreateAccountCtrl.h"
#include "LobbyEvent.h"

#include "WONGUI/ComboBox.h"
#include "WONGUI/GUIConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LostUserNameCtrl::LostUserNameCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostUserNameCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mEmailInput,"EmailInput");
		WONComponentConfig_Get(aConfig,mRetrieveButton,"RetrieveButton");

		CheckInput();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostUserNameCtrl::CheckInput()
{
	mRetrieveButton->Enable(CreateAccountCtrl::CheckEmail(mEmailInput->GetText()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostUserNameCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_InputTextChanged)
		CheckInput();
	else if(theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent==mRetrieveButton)
		FireEvent(new GetLostUserNameEvent(mEmailInput->GetText()));
	else
		Container::HandleComponentEvent(theEvent);
}

