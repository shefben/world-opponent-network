#include "CreateAccountCtrl.h"

#include "LobbyEvent.h"
#include "LobbyResource.h"

#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"
#include "WONGUI/InputBox.h"


using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CreateAccountCtrl::CreateAccountCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateAccountCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
		WONComponentConfig_Get(aConfig,mUserNameInput,"UserNameInput");
		WONComponentConfig_Get(aConfig,mUserNameDesc,"UserNameDesc");
		WONComponentConfig_Get(aConfig,mPasswordInput,"PasswordInput");
		WONComponentConfig_Get(aConfig,mPasswordDesc,"PasswordDesc");
		WONComponentConfig_Get(aConfig,mConfirmPasswordInput,"ConfirmPasswordInput");
		WONComponentConfig_Get(aConfig,mConfirmPasswordDesc,"ConfirmPasswordDesc");
		WONComponentConfig_Get(aConfig,mEmailInput,"EmailInput");
		WONComponentConfig_Get(aConfig,mEmailDesc,"EmailDesc");
		WONComponentConfig_Get(aConfig,mMonthInput,"MonthInput");
		WONComponentConfig_Get(aConfig,mDayInput,"DayInput");
		WONComponentConfig_Get(aConfig,mYearInput,"YearInput");
		WONComponentConfig_Get(aConfig,mBirthdateDesc,"BirthdateDesc");
		WONComponentConfig_Get(aConfig,mNewsletterCheck,"NewsletterCheck");
		WONComponentConfig_Get(aConfig,mCreateButton,"CreateButton");

		CheckInput();
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CreateAccountCtrl::SetDesc(bool theCondition, Label *theLabel)
{
	theLabel->SetVisible(theCondition);
	return theCondition;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CreateAccountCtrl::CheckEmail(const std::string &theEmail)
{
	if(theEmail.empty())
		return false;

	int anAtPos = theEmail.find('@',1);
	if(anAtPos==string::npos)
		return false;

	int aDotPos = theEmail.find('.',anAtPos);
	if(aDotPos==string::npos)
		return false;

	if(theEmail.length()<=aDotPos+1)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CreateAccountCtrl::CheckBirthdate(InputBox *theMonth, InputBox *theDay, InputBox *theYear)
{
	int aMonth = theMonth->GetText().atoi();
	int aDay = theDay->GetText().atoi();
	int aYear = theYear->GetText().atoi();

	if(aMonth<=0 || aDay<=0 || aYear<=1889)
		return false;

	if(aMonth>12 || aDay>31 || aYear>1999)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateAccountCtrl::CheckInput()
{
	bool hadError = false;
	// Check UserName
	if(SetDesc(mUserNameInput->GetText().length()<2, mUserNameDesc))
		hadError = true;

	// Check Password
	if(SetDesc(mPasswordInput->GetText().length()<4, mPasswordDesc))
		hadError = true;

	// Check Password Confirm
	if(SetDesc(mPasswordInput->GetText()!=mConfirmPasswordInput->GetText(), mConfirmPasswordDesc))
		hadError = true;

	// Check email
	if(SetDesc(!CheckEmail(mEmailInput->GetText()), mEmailDesc))
		hadError = true;

	// Check birthdate
	if(SetDesc(!CheckBirthdate(mMonthInput, mDayInput, mYearInput), mBirthdateDesc))
		hadError = true;

	mCreateButton->Enable(!hadError);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateAccountCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_InputTextChanged)
		CheckInput();
	else if(theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent==mCreateButton)
	{
		CreateAccountEventPtr anEvent = new CreateAccountEvent();

		anEvent->mUserName = mUserNameInput->GetText();
		anEvent->mPassword = mPasswordInput->GetText();
		anEvent->mEmail = mEmailInput->GetText();
		anEvent->mBirthDay = mDayInput->GetText().atoi();
		anEvent->mBirthMonth = mMonthInput->GetText().atoi();
		anEvent->mBirthYear = mYearInput->GetText().atoi();
		anEvent->mSubscribeSierraNewsletter = mNewsletterCheck->IsChecked();
		FireEvent(anEvent);
	}
	else
		Container::HandleComponentEvent(theEvent);
}
