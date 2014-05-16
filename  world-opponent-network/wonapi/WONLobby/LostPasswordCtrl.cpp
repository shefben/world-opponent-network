#include "LostPasswordCtrl.h"

#include "LobbyEvent.h"
#include "LobbyPersistentData.h"

#include "WONGUI/ComboBox.h"
#include "WONGUI/GUIConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LostPasswordCtrl::LostPasswordCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostPasswordCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mUserNameCombo,"UserNameCombo");
		WONComponentConfig_Get(aConfig,mRetrieveButton,"RetrieveButton");

		CheckInput();
		
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostPasswordCtrl::CheckInput()
{
	mRetrieveButton->Enable(mUserNameCombo->GetText().length()>=2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostPasswordCtrl::SetLoginNames()
{
	mUserNameCombo->Clear();
	const LobbyPersistentData::UserInfoMap &aMap = LobbyPersistentData::GetUserInfoMap();
	LobbyPersistentData::UserInfoMap::const_iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		mUserNameCombo->InsertString(anItr->first);
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LostPasswordCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_InputTextChanged)
		CheckInput();
	else if(theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent==mRetrieveButton)
		FireEvent(new GetLostPasswordEvent(mUserNameCombo->GetText()));
	else if(theEvent->mType==LobbyEvent_SetLoginNames)
		SetLoginNames();
	else
		Container::HandleComponentEvent(theEvent);
}

