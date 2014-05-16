#include "StatusDialog.h"

#include "LobbyResource.h"

#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StatusDialog::StatusDialog()
{
	mLastTick = GetTickCount();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StatusDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;
	
		WONComponentConfig_Get(aConfig,mTitleLabel,"TitleLabel");
		WONComponentConfig_Get(aConfig,mCloseButton,"CloseButton");
		WONComponentConfig_Get(aConfig,mStatusLabel,"StatusLabel");
		WONComponentConfig_Get(aConfig,mCancelButton,"CancelButton");

		WONComponentConfig_Get(aConfig,mBack,"Back");
		WONComponentConfig_Get(aConfig,mDialogComponent,"DialogComponent");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StatusDialog::SetText(const GUIString &theStatus, bool isCancel)
{
	SetText(mTitleLabel->GetText(), theStatus, isCancel);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StatusDialog::SetText(const GUIString &theTitle, const GUIString &theStatus, bool isCancel)
{
	bool isDragging = GetIsDragging();

	int aLastDialogWidth = Width();
	int aLastDialogHeight = Height();
	int aLastLabelWidth = mStatusLabel->Width();
	int aLastLabelHeight = mStatusLabel->Height();
	int aLastBodyWidth = mDialogComponent->Width();
	int aLastBodyHeight = mDialogComponent->Height();
	int aLastCancelButtonYPos = mCancelButton->Top();

	mTitleLabel->SetText(theTitle);
	mTitleLabel->SetDesiredWidth();

	mCancelButton->SetText(isCancel?LobbyGlobal_Cancel_String:LobbyGlobal_OK_String);
	mCancelButton->SetDesiredSize();
	mCancelButton->SetSizeAtLeast(100,0);

	mStatusLabel->SetVertPad(0,0);
	mStatusLabel->SetText(GUIString::EMPTY_STR);
	mStatusLabel->SetSize(0,0);

	SetSize(0,0);
	SetSizeAtLeast(mTitleLabel->Width() + mCloseButton->Width() + mBack->Left() + mBack->Right() + 5, 0);

	mDialogComponent->SetSize(0,0);
	mDialogComponent->SetDesiredSize();

	SetDesiredSize(20,20);

	int anOriginalWidth = Width();
	int anOriginalHeight = Height();
	int anOriginalBodyWidth = mStatusLabel->Width();
	int anOriginalBodyHeight = mStatusLabel->Height();
	int aScreenWidth = LobbyScreen::GetLobbyContainer()->Width();
	int aScreenHeight = LobbyScreen::GetLobbyContainer()->Height();

	int aStringWidth = mStatusLabel->GetTextFont()->GetStringWidth(theStatus);
	int aMinStringWidth = mStatusLabel->GetTextFont()->GetCharWidth('M') * 30;
	int aNewBodyWidth = aStringWidth;
	if(aStringWidth < aMinStringWidth)
		aNewBodyWidth = aMinStringWidth;
	if(aNewBodyWidth > (aScreenWidth / 2))
		aNewBodyWidth = aScreenWidth / 2;

	int aGrowWidth = aNewBodyWidth - mStatusLabel->Width();
	if(aGrowWidth < 0) aGrowWidth = 0;

	mStatusLabel->SetWidth(aNewBodyWidth);
	mStatusLabel->SetText(theStatus);
	int aNumLines = mStatusLabel->Height() / mStatusLabel->GetTextFont()->GetHeight();
	if(aNumLines<5) 
	{
		int aPad = ((5-aNumLines)*mStatusLabel->GetTextFont()->GetHeight())/2;
		mStatusLabel->SetVertPad(aPad,aPad);
		mStatusLabel->SetDesiredSize();
	}

	int aNewBodyHeight = mStatusLabel->Height();
	int aGrowHeight = aNewBodyHeight - anOriginalBodyHeight;
	if(aGrowHeight < 0) aGrowHeight = 0;

	if((anOriginalHeight + aGrowHeight) > (anOriginalWidth + aGrowWidth) || (anOriginalHeight + aGrowHeight) >= aScreenHeight)
	{
		int anExtraGrowWidth = ((((anOriginalWidth + aGrowWidth) * 5) / 4) - (anOriginalWidth + aGrowWidth));
		aGrowWidth += anExtraGrowWidth;
		aNewBodyWidth += anExtraGrowWidth;
		mStatusLabel->SetWidth(aNewBodyWidth);
		aNewBodyHeight = mStatusLabel->Height();
		aGrowHeight = aNewBodyHeight - anOriginalBodyHeight;
	}

	SetSize(anOriginalWidth + aGrowWidth, anOriginalHeight + aGrowHeight);

	if(GetTickCount() - mLastTick < 50)
	{
		mStatusLabel->SetSizeAtLeast(aLastLabelWidth, aLastLabelHeight);
		mDialogComponent->SetSizeAtLeast(aLastBodyWidth,aLastBodyHeight);
		SetSizeAtLeast(aLastDialogWidth, aLastDialogHeight);
		if(aLastCancelButtonYPos > mCancelButton->Top())
			mCancelButton->SetTop(aLastCancelButtonYPos);
	}

	Invalidate();
	SetIsDragging(isDragging); // so dragging isn't canceled by resizing
}

