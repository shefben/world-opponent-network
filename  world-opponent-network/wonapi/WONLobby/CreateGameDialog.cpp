#include "CreateGameDialog.h"
#include "LobbyGame.h"
#include "LobbyTypes.h"

#include "WONGUI/ComboBox.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CreateGameDialog::CreateGameDialog()
{
	mGameType = LobbyGameType_None;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CreateGameDialog::~CreateGameDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateGameDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mGameNameInput,"GameNameInput");
		WONComponentConfig_Get(aConfig,mSkillCombo,"SkillCombo");
		WONComponentConfig_Get(aConfig,mPasswordInput,"PasswordInput");
		WONComponentConfig_Get(aConfig,mPasswordLabel,"PasswordLabel");
		WONComponentConfig_Get(aConfig,mOpenToAllRadio,"OpenToAllRadio");
		WONComponentConfig_Get(aConfig,mPasswordRadio,"PasswordRadio");
		WONComponentConfig_Get(aConfig,mInviteOnlyRadio,"InviteOnlyRadio");
		WONComponentConfig_Get(aConfig,mAskToJoinRadio,"AskToJoinRadio");
		WONComponentConfig_Get(aConfig,mCreateButton,"CreateButton");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateGameDialog::Reset(bool clearAll)
{
	mGameNameInput->RequestFocus();
	if(clearAll)
	{
		mOpenToAllRadio->SetCheck(true);
		mSkillCombo->SetSelItem(0,false);
		mGameNameInput->Clear(false);
		mPasswordInput->Clear(false);
		UpdatePasswordInput();
	}

	UpdateCreateButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateGameDialog::SetGameType(LobbyGameType theType)
{
	mGameType = theType;
	bool isInternet = theType==LobbyGameType_Internet;


	if(mInviteOnlyRadio->IsChecked() || mAskToJoinRadio->IsChecked())
		mOpenToAllRadio->SetCheck(true,false);

	mInviteOnlyRadio->SetVisible(isInternet);
	mAskToJoinRadio->SetVisible(isInternet);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyGamePtr CreateGameDialog::GetGame()
{
	LobbyGamePtr aGame = LobbyGame::CreateGame(mGameType);
	aGame->SetName(mGameNameInput->GetText());

	if(mPasswordRadio->IsChecked())
		aGame->SetPassword(mPasswordInput->GetText());
	
	aGame->SetAskToJoin(mAskToJoinRadio->IsChecked());
	aGame->SetInviteOnly(mInviteOnlyRadio->IsChecked());

	int aSelItemPos = mSkillCombo->GetSelItemPos();
	if(aSelItemPos<LobbySkillLevel_None || aSelItemPos>=LobbySkillLevel_Max)
		aSelItemPos = LobbySkillLevel_None;

	aGame->SetSkillLevel((LobbySkillLevel)aSelItemPos);
	return aGame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateGameDialog::UpdateCreateButton()
{
	bool enable = true;

	if(mGameNameInput->GetText().empty())
		enable = false;
	else if(mPasswordInput->IsVisible() && mPasswordInput->IsEmpty())
		enable = false;

	mCreateButton->Enable(enable);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateGameDialog::UpdatePasswordInput()
{
	mPasswordInput->SetVisible(mPasswordRadio->IsChecked());
	mPasswordLabel->SetVisible(mPasswordRadio->IsChecked());
	UpdateCreateButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CreateGameDialog::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		UpdatePasswordInput();
		Dialog::HandleComponentEvent(theEvent);
	}
	else if (theEvent->mType==ComponentEvent_InputTextChanged)
		UpdateCreateButton();
	else if (theEvent->mType==ComponentEvent_ComboSelChanged && theEvent->mComponent == mSkillCombo)
		UpdateCreateButton();
	else
		Dialog::HandleComponentEvent(theEvent);
}

