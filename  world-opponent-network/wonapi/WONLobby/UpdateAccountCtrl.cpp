#include "UpdateAccountCtrl.h"

#include "CreateAccountCtrl.h"
#include "LobbyEvent.h"
#include "LobbyMisc.h"

#include "WONAuth/AuthContext.h"
#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/InputBox.h"
#include "WONGUI/Label.h"

using namespace WONAPI;
using namespace std;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateAccountCtrl::UpdateAccountCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateAccountCtrl::~UpdateAccountCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateAccountCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mUserNameInput,"UserNameInput");
		WONComponentConfig_Get(aConfig,mOldPasswordInput,"OldPasswordInput");
		WONComponentConfig_Get(aConfig,mPasswordInput,"PasswordInput");
		WONComponentConfig_Get(aConfig,mConfirmPasswordInput,"ConfirmPasswordInput");
		WONComponentConfig_Get(aConfig,mEmailInput,"EmailInput");
		WONComponentConfig_Get(aConfig,mNewsletterCheck,"NewsletterCheck");
		WONComponentConfig_Get(aConfig,mUserNameDesc,"UserNameDesc");
		WONComponentConfig_Get(aConfig,mOldPasswordDesc,"OldPasswordDesc");
		WONComponentConfig_Get(aConfig,mPasswordDesc,"PasswordDesc");
		WONComponentConfig_Get(aConfig,mConfirmPasswordDesc,"ConfirmPasswordDesc");
		WONComponentConfig_Get(aConfig,mEmailDesc,"EmailDesc");
		WONComponentConfig_Get(aConfig,mQueryButton,"QueryButton");
		WONComponentConfig_Get(aConfig,mUpdateButton,"UpdateButton");
		
		mUserNameInput->Enable(false);

		CheckInput();
		SubscribeToBroadcast(true);	
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool UpdateAccountCtrl::SetDesc(bool theCondition, Label *theLabel)
{
	theLabel->SetVisible(theCondition);
	return theCondition;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateAccountCtrl::CheckInput()
{
	bool hadError = false;
	bool hadOldPassword = true;

	if(SetDesc(mOldPasswordInput->GetText()!=mActualOldPassword, mOldPasswordDesc))
	{
		hadError = true;
		hadOldPassword = false;
	}

	// Check Password
	if(SetDesc(!mPasswordInput->GetText().empty() && mPasswordInput->GetText().length()<4, mPasswordDesc))
		hadError = true;

	// Check Password Confirm
	if(SetDesc(mPasswordInput->GetText()!=mConfirmPasswordInput->GetText(), mConfirmPasswordDesc))
		hadError = true;

	// Check email
	if(SetDesc(!mEmailInput->GetText().empty() && !CreateAccountCtrl::CheckEmail(mEmailInput->GetText()), mEmailDesc))
		hadError = true;

	mUpdateButton->Enable(!hadError);
	mQueryButton->Enable(hadOldPassword);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateAccountCtrl::HandleEnteredScreen()
{
	AuthContext *anAuth = LobbyMisc::GetAuthContext();
	if(anAuth==NULL)
		return;

	mUserNameInput->SetText(anAuth->GetUserName());
	mOldPasswordInput->Clear();
	mPasswordInput->Clear();
	mConfirmPasswordInput->Clear();
	mEmailInput->Clear();
	mNewsletterCheck->SetCheck(false);

	mActualOldPassword = anAuth->GetPassword();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateAccountCtrl::HandleQueryAccountResult(QueryAccountResultEvent *theEvent)
{
	mEmailInput->SetText(theEvent->mEmail);
	mNewsletterCheck->SetCheck(theEvent->mSubscribeSierraNewsletter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateAccountCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_InputTextChanged: CheckInput(); return;
		case ComponentEvent_ButtonPressed:
			if(theEvent->mComponent==mQueryButton)
				FireEvent(LobbyEvent_QueryAccount);
			else if(theEvent->mComponent==mUpdateButton)
				FireEvent(new UpdateAccountEvent(mEmailInput->GetText(),mPasswordInput->GetText(),mNewsletterCheck->IsChecked()));
			return;

		case LobbyEvent_EnteredScreen: HandleEnteredScreen(); return;
		case LobbyEvent_QueryAccountResult: HandleQueryAccountResult((QueryAccountResultEvent*)theEvent); return;
	}

	Container::HandleComponentEvent(theEvent);
}
