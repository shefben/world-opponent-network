#include "QuickPlayCtrl.h"

#include "LobbyEvent.h"
#include "LobbyResource.h"
#include "LobbyPersistentData.h"

#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
QuickPlayCtrl::QuickPlayCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QuickPlayCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mWelcomeLabel,"WelcomeLabel");
		WONComponentConfig_Get(aConfig,mNoviceButton,"NoviceRadio");		
		WONComponentConfig_Get(aConfig,mIntermediateButton,"IntermediateRadio"); 
		WONComponentConfig_Get(aConfig,mAdvancedButton,"AdvancedRadio");		
		WONComponentConfig_Get(aConfig,mExpertButton,"ExpertRadio");		
		WONComponentConfig_Get(aConfig,mPlayButton,"PlayButton");

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QuickPlayCtrl::HandleEnteredScreen()
{
	LobbyPersistentData::UserInfo* aUserInfo = LobbyPersistentData::GetCurUserInfo();

	if (aUserInfo != NULL)
		mWelcomeLabel->SetText(StringLocalize::GetStr(InternetQuickPlay_UserWelcome_String,aUserInfo->mUserName));
	else
		mWelcomeLabel->SetText(InternetQuickPlay_Welcome_String);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QuickPlayCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_EnteredScreen:	HandleEnteredScreen(); return;

		case ComponentEvent_ButtonPressed:
		{
			if(theEvent->mComponent==mPlayButton.get())
			{
				LobbySkillLevel aSkill = LobbySkillLevel_None;
				
				if(mNoviceButton->IsChecked()) aSkill = LobbySkillLevel_Novice;
				else if(mIntermediateButton->IsChecked()) aSkill = LobbySkillLevel_Intermediate;
				else if(mAdvancedButton->IsChecked()) aSkill = LobbySkillLevel_Advanced;
				else if(mExpertButton->IsChecked()) aSkill = LobbySkillLevel_Expert;

				FireEvent(new QuickPlayEvent(aSkill));

				break;
			}
		}
	}


	Dialog::HandleComponentEvent(theEvent);
}

