#include "LoginCtrl.h"

#include "LobbyConfig.h"
#include "LobbyEvent.h"
#include "LobbyPersistentData.h"

#include "WONGUI/ComboBox.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/InputBox.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoginCtrl::LoginCtrl()
{
	mInitComplete = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,mUserNameCombo,"UserNameCombo");
		WONComponentConfig_Get(aConfig,mPasswordInput,"PasswordInput");
		WONComponentConfig_Get(aConfig,mRememberPasswordCheck,"RememberPasswordCheck");
		WONComponentConfig_Get(aConfig,mLoginButton,"LoginButton");

		CheckEnableLoginButton();
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::CheckEnableLoginButton()
{
	mLoginButton->Enable(mInitComplete && !mUserNameCombo->GetText().empty() && !mPasswordInput->GetText().empty());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::SetLoginNames()
{
	mUserNameCombo->Clear();
	const LobbyPersistentData::UserInfoMap &aMap = LobbyPersistentData::GetUserInfoMap();
	LobbyPersistentData::UserInfoMap::const_iterator anItr = aMap.begin();
	time_t aLastTime = 0;
	ListItem *aLastItem = NULL;
	while(anItr!=aMap.end())
	{
		ListItem *anItem = mUserNameCombo->InsertString(anItr->first);
		if(anItr->second.mLastUseTime > aLastTime)
		{
			aLastTime = anItr->second.mLastUseTime;
			aLastItem = anItem;
		}

		++anItr;
	}

	if(aLastItem!=NULL)
	{
		mUserNameCombo->SetSelItem(mUserNameCombo->GetItemPos(aLastItem),false);
		CheckFillPassword();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::CheckFillPassword()
{
	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	if(aConfig!=NULL && aConfig->mIsPublic) // public computer -> (don't remember password)
	{	
		mPasswordInput->Clear();
		mRememberPasswordCheck->SetVisible(false);
		mRememberPasswordCheck->SetCheck(false);
	}
	else
	{
		GUIString aPassword = LobbyPersistentData::GetPassword(mUserNameCombo->GetText());
		mPasswordInput->SetText(aPassword);
		mPasswordInput->SetSel();

		mRememberPasswordCheck->SetCheck(!aPassword.empty());
		mRememberPasswordCheck->SetVisible(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::DoRememberPassword()
{
	GUIString aUserName = mUserNameCombo->GetText();
	GUIString aPassword;
	if(mRememberPasswordCheck->IsChecked())
		aPassword = mPasswordInput->GetText();

	if(aUserName.empty())
		return;

	LobbyPersistentData::AddUserInfo(aUserName, aPassword, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::HandleSetLoginFields(SetLoginFieldsEvent *theEvent)
{
	mUserNameCombo->SetText(theEvent->mUserName,false);
	mPasswordInput->SetText(theEvent->mPassword,false);
	mRememberPasswordCheck->SetCheck(false);
	CheckEnableLoginButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::HandleLoginInitStatus(LoginInitStatusEvent *theEvent)
{
	LoginInitStatusEvent *anEvent = (LoginInitStatusEvent*)theEvent;
	if(anEvent->mStatusType==LoginInitStatus_InitStart)
	{
		mInitComplete = false;
		CheckEnableLoginButton();
		SetVisible(true);
	}
	else if(anEvent->mStatusType==LoginInitStatus_InitSuccess)
	{
		mInitComplete = true;
		CheckEnableLoginButton();
	}
	else if(anEvent->mStatusType==LoginInitStatus_InitFailed)
		SetVisible(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoginCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_InputTextChanged: 
			if(theEvent->mComponent==mUserNameCombo.get())
			{
				if(mUserNameCombo->GetText().at(0)=='@')
				{
					if(!mPasswordInput->GetText().empty())
						mPasswordInput->Clear(false);
				}
			}
			CheckEnableLoginButton(); 
			return;

		case ComponentEvent_ComboSelChanged: 
		{
			if(theEvent->mComponent==mUserNameCombo) 
				CheckFillPassword();
			return;
		}

		case ComponentEvent_InputReturn:
		{ 
			if(mLoginButton->Disabled())
			{
				if(theEvent->mComponent==mUserNameCombo)
					mPasswordInput->RequestFocus();
				else if(theEvent->mComponent==mPasswordInput)
					mUserNameCombo->RequestFocus();

				return;
			}
		}

		case ComponentEvent_ButtonPressed:
		{
			if(theEvent->mComponent==mLoginButton)
				FireEvent(new LoginEvent(mUserNameCombo->GetText(),mPasswordInput->GetText(),mRememberPasswordCheck->IsChecked()));
			else if(theEvent->mComponent==mRememberPasswordCheck)
				DoRememberPassword();
			else
				break;

			return;
		}

		case LobbyEvent_SetLoginNames: SetLoginNames(); return;
		
		case LobbyEvent_SetLoginFields: HandleSetLoginFields((SetLoginFieldsEvent*)theEvent); return;
		case LobbyEvent_LoginInitStatus: HandleLoginInitStatus((LoginInitStatusEvent*)theEvent); return;
	}
		
	Dialog::HandleComponentEvent(theEvent);
}
