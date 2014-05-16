#include "AdminActionDialog.h"

#include "LobbyClient.h"
#include "LobbyResource.h"
#include "LobbyTypes.h"

#include "WONGUI/ComboBox.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AdminActionDialog::AdminActionDialog()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AdminActionDialog::~AdminActionDialog()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AdminActionDialog::UpdateDoActionButton()
{
	if (mExplanation->GetText().empty() && !mIsReasonOptional)
		mDoActionButton->Enable(false);
	else
		mDoActionButton->Enable(true);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AdminActionDialog::HandleComponentEvent(ComponentEvent* theEvent)
{
	if (theEvent->mType == ComponentEvent_InputTextChanged)
	{
		if (theEvent->mComponent == mExplanation)
		{
			UpdateDoActionButton();
			return;
		}
	}
	Dialog::HandleComponentEvent(theEvent);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AdminActionDialog::GetApplyToServer()
{
	return mApplyToServer->IsChecked();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& AdminActionDialog::GetExplanation()
{
	return mExplanation->GetText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned long AdminActionDialog::GetNumSeconds()
{
	unsigned long aSeconds = 3600;
	
	int selDuration = mDurationCombo->GetSelItemPos();
	switch(selDuration)
	{
	case 0: aSeconds=1;		break;
	case 1: aSeconds=5;		break;
	case 2: aSeconds=10;	break;
	case 3: aSeconds=30;	break;
	case 4: aSeconds=0;		break;	// permanent
	}

	int selTimeUnit = mTimeSegmentCombo->GetSelItemPos();
	switch(selTimeUnit)
	{
	case 2: aSeconds*=24;	// fall through
	case 1: aSeconds*=60;    // fall through
	case 0: aSeconds*=60;	break;
	}

	return aSeconds;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AdminActionDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mExplanationHeader,"ExplanationHeader");
		WONComponentConfig_Get(aConfig,mExplanation,"Explanation");
		WONComponentConfig_Get(aConfig,mDurationHeader,"DurationHeader");
		WONComponentConfig_Get(aConfig,mDurationCombo,"DurationCombo");
		WONComponentConfig_Get(aConfig,mTimeSegmentCombo,"TimeSegmentCombo");
		WONComponentConfig_Get(aConfig,mDoActionButton,"DoActionButton");
		WONComponentConfig_Get(aConfig,mCancelButton,"CancelButton");
		WONComponentConfig_Get(aConfig,mApplyToRoom,"ApplyToRoomRadio");
		WONComponentConfig_Get(aConfig,mApplyToServer,"ApplyToServerRadio");
		WONComponentConfig_Get(aConfig,mTitleLabel,"TitleLabel");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AdminActionDialog::Reset(LobbyClientAction theType, LobbyClient *theTargetClient)
{
	mActionType = theType;

	// Set dialog control states based on type of action
	GUIString aActionString;
	bool bShowDuration;
	bool bShowExplanation;
	bool bShowScopeSwitch;

	// Reset the controls to default settings
	mApplyToServer->SetCheck(false,false);
	mApplyToRoom->SetCheck(true,false);
	mExplanation->SetText("",false);
	mDurationCombo->SetSelItem(1,false);
	mTimeSegmentCombo->SetSelItem(0,false);

	switch(mActionType)
	{
	case LobbyClientAction_Warn:	
		aActionString		= AdminActionDialog_Warn_String;
		bShowDuration		= false;
		bShowExplanation	= true;
		mIsReasonOptional	= false;
		bShowScopeSwitch	= false;
		break;

	case LobbyClientAction_Mute:	
		aActionString		= AdminActionDialog_Mute_String;
		bShowDuration		= true;
		bShowExplanation	= true;
		mIsReasonOptional	= true;
		bShowScopeSwitch	= true;
		break;

	case LobbyClientAction_Unmute:
		aActionString		= AdminActionDialog_UnMute_String;
		bShowDuration		= false;
		bShowExplanation	= false;
		mIsReasonOptional	= true;
		bShowScopeSwitch	= true;
		break;

	case LobbyClientAction_Kick:
		aActionString		= AdminActionDialog_Kick_String; 
		bShowDuration		= false;
		bShowExplanation	= true;
		mIsReasonOptional	= true;
		bShowScopeSwitch	= false;
		break;

	case LobbyClientAction_Ban:	
		aActionString		= AdminActionDialog_Ban_String;
		bShowDuration		= true;
		bShowExplanation	= true;
		mIsReasonOptional	= false;
		bShowScopeSwitch	= true;
		break;
	}


	// Set visible controls based on above settings
	mDurationHeader		->SetVisible(bShowDuration);
	mDurationCombo		->SetVisible(bShowDuration);
	mTimeSegmentCombo	->SetVisible(bShowDuration);
	mApplyToServer		->SetVisible(bShowScopeSwitch);
	mApplyToRoom		->SetVisible(bShowScopeSwitch);
	mExplanation		->Enable(bShowExplanation);

	// Set the focus to the explanation (if it's visible)
	if (bShowExplanation)
		mExplanation->RequestFocus();
		
	// Update the DoAction button
	UpdateDoActionButton();

	// Add the client's name to the title
	GUIString aTitle = aActionString;
	aTitle.append(" ");
	aTitle.append(theTargetClient->GetName());
	mTitleLabel->SetText(aTitle);

	// Tell the user if the explanation is required
	if (mIsReasonOptional)
		mExplanationHeader->SetText(AdminActionDialog_ExplanationOpt_String);
	else
		mExplanationHeader->SetText(AdminActionDialog_ExplanationReq_String);

	// Set the "OK" button text to a more appropriate string
	mDoActionButton->SetText(aActionString);

	RecalcLayout();
}