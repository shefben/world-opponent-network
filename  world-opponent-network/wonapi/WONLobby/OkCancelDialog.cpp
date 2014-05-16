#include "OkCancelDialog.h"

#include "WONGUI/GUIConfig.h"
#include "WONGUI/Label.h"
#include "LobbyScreen.h"
#include <math.h>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
OkCancelDialog::OkCancelDialog()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OkCancelDialog::InitComponent(ComponentInit &theInit)
{
	Dialog::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mTitleLabel,"TitleLabel");
		WONComponentConfig_Get(aConfig,mCloseButton,"CloseButton");
		WONComponentConfig_Get(aConfig,mBodyLabel,"BodyLabel");

		WONComponentConfig_Get(aConfig,mOkButton,"OKButton");
		WONComponentConfig_Get(aConfig,mCancelButton,"CancelButton");

		WONComponentConfig_Get(aConfig,mBack,"Back");
		WONComponentConfig_Get(aConfig,mDialogComponent,"DialogComponent");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OkCancelDialog::SetText(const GUIString &theTitle, const GUIString &theText, const GUIString &theOkText, const GUIString &theCancelText)
{
	mTitleLabel->SetText(theTitle);
	mTitleLabel->SetDesiredWidth();

	mOkButton->SetText(theOkText);
	mOkButton->SetDesiredSize();
	mOkButton->SetSizeAtLeast(100,0);

	mCancelButton->SetText(theCancelText);
	mCancelButton->SetDesiredSize();
	mCancelButton->SetSizeAtLeast(100,0);

	mBodyLabel->SetVertPad(0,0);
	mBodyLabel->SetText(GUIString::EMPTY_STR);
	mBodyLabel->SetSize(0,0);

	SetSize(0,0);
	SetSizeAtLeast(mTitleLabel->Width() + mCloseButton->Width() + mBack->Left() + mBack->Right() + 5, 0);

	mDialogComponent->SetSize(0,0);
	mDialogComponent->SetDesiredSize();

	SetDesiredSize(20,20);

	int anOriginalWidth = Width();
	int anOriginalHeight = Height();
	int anOriginalBodyWidth = mBodyLabel->Width();
	int anOriginalBodyHeight = mBodyLabel->Height();

	int aStringWidth = mBodyLabel->GetTextFont()->GetStringWidth(theText);
	int aNewBodyWidth;
	if(aStringWidth < 250)
		aNewBodyWidth = aStringWidth;
	else if(aStringWidth < 800)
		aNewBodyWidth = 250;
	else
		aNewBodyWidth = 400;

	int aGrowWidth = aNewBodyWidth - mBodyLabel->Width();
	if(aGrowWidth < 0) aGrowWidth = 0;

	mBodyLabel->SetWidth(aNewBodyWidth);
	mBodyLabel->SetText(theText);
	int aNumLines = mBodyLabel->Height() / mBodyLabel->GetTextFont()->GetHeight();
	if(aNumLines<5) 
	{
		int aPad = ((5-aNumLines)*mBodyLabel->GetTextFont()->GetHeight())/2;
		mBodyLabel->SetVertPad(aPad,aPad);
		mBodyLabel->SetDesiredSize();
	}

	int aNewBodyHeight = mBodyLabel->Height();
	int aGrowHeight = aNewBodyHeight - anOriginalBodyHeight;
	if(aGrowHeight < 0) aGrowHeight = 0;

	int aScreenWidth = LobbyScreen::GetLobbyContainer()->Width();
	int aScreenHeight = LobbyScreen::GetLobbyContainer()->Height();
	if((anOriginalHeight + aGrowHeight) > (anOriginalWidth + aGrowWidth) || (anOriginalHeight + aGrowHeight) >= aScreenHeight)
	{
		int anExtraGrowWidth = ((((anOriginalWidth + aGrowWidth) * 5) / 4) - (anOriginalWidth + aGrowWidth));
		aGrowWidth += anExtraGrowWidth;
		aNewBodyWidth += anExtraGrowWidth;
		mBodyLabel->SetWidth(aNewBodyWidth);
		aNewBodyHeight = mBodyLabel->Height();
		aGrowHeight = aNewBodyHeight - anOriginalBodyHeight;
	}

	SetSize(anOriginalWidth + aGrowWidth, anOriginalHeight + aGrowHeight);
}