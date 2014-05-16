#include "CreateRoomDialog.h"

#include "WONGUI/InputBox.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CreateRoomDialog::CreateRoomDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateRoomDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mRoomNameInput,"RoomNameInput");
		WONComponentConfig_Get(aConfig,mPasswordInput,"PasswordInput");
		WONComponentConfig_Get(aConfig,mPasswordLabel,"PasswordLabel");
		WONComponentConfig_Get(aConfig,mPasswordCheck,"PasswordCheck");
		WONComponentConfig_Get(aConfig,mCreateButton,"CreateButton");
		CheckInput();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateRoomDialog::Reset(bool clearAll)
{
	mRoomNameInput->RequestFocus();
	if(clearAll)
	{
		mRoomNameInput->Clear(false);
		mPasswordInput->Clear(false);
		mPasswordCheck->SetCheck(false);
		UpdatePasswordInput();
	}

	CheckInput();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateRoomDialog::UpdatePasswordInput()
{
	mPasswordInput->SetVisible(mPasswordCheck->IsChecked());
	mPasswordLabel->SetVisible(mPasswordCheck->IsChecked());
	CheckInput();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateRoomDialog::CheckInput()
{
	bool enable = true;
	if(mRoomNameInput->IsEmpty())
		enable = false;
	else if(mPasswordInput->IsVisible() && mPasswordInput->IsEmpty())
		enable = false;

	mCreateButton->Enable(enable);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& CreateRoomDialog::GetRoomName()
{
	return mRoomNameInput->GetText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& CreateRoomDialog::GetPassword()
{
	if(mPasswordCheck->IsChecked())
		return mPasswordInput->GetText();
	else
		return GUIString::EMPTY_STR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateRoomDialog::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed && theEvent->mComponent==mPasswordCheck)
		UpdatePasswordInput();
	else if(theEvent->mType==ComponentEvent_InputTextChanged)
		CheckInput();
	else
		Dialog::HandleComponentEvent(theEvent);
}

