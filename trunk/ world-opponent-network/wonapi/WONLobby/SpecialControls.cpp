#include "SpecialControls.h"

#include "LobbyEvent.h"

#include "WONCommon/MiscUtil.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageTextButton::ImageTextButton(const GUIString &theText)
{
	SetComponentFlags(ComponentFlag_GrabBG,true);
	mFont = GetDefaultFont();
	mText = theText;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageTextButton::Paint(Graphics &g)
{
	ImageButton::Paint(g);

	int offset = (IsButtonDown() ? 1 :0);
	g.SetFont(mFont);
	g.SetColor(0x000000);
	g.DrawString(mText,30+offset,(Height()-mFont->GetHeight()-4)/2+offset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageTextButton::ActivateHook()
{
	if (mBrowseLocation.empty())
		return true;
		
	Browse(mBrowseLocation);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageTextButtonConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="FONT")
		mButton->mFont = ReadFont();
	else if(theInstruction=="TEXT")
		mButton->mText = ReadString();
	else if(theInstruction=="BROWSELOCATION")
		mButton->mBrowseLocation = ReadString();
	else
		return ImageButtonConfig::HandleInstruction(theInstruction);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MaxSplitterButton::MaxSplitterButton(Direction theDirection) : MSArrowButton(theDirection) 
{
	SetSize(15,15);
	SetButtonFlags(ButtonFlag_RepeatFire,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MaxSplitterButton::Paint(Graphics &g)
{
	MSArrowButton::Paint(g);
	if(IsChecked())
	{
		int x = Width()/2-3, y = Height()/2-2;
		if(IsButtonDown())
		{
			x++;
			y++;
		}
		g.ApplyColorSchemeColor(StandardColor_ButtonText);
		g.FillRect(x,y,7,4);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MaxSplitterButtonConfig::HandleInstruction(const std::string &theInstruction)
{
	if(MSArrowButtonConfig::HandleInstruction(theInstruction))
		return true;

	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CrossPromotionButton::CrossPromotionButton()
{
	SubscribeToBroadcast(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CrossPromotionButton::HandleSetCrossPromotionEvent(SetCrossPromotionEvent *theEvent)
{
	if(theEvent->mImage)
	{
		SetImage(ImageButtonType_Normal,theEvent->mImage);
		mBrowseLocation = theEvent->mLink;
	}
	else
	{
		SetImage(ImageButtonType_Normal,mDefaultImage);
		mBrowseLocation = mDefaultBrowseLocation;
	}
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CrossPromotionButton::SetDefaultImage(Image* theImage)
{
	mDefaultImage = theImage;
	if(mImages[ImageButtonType_Normal]==NULL)
		SetImage(ImageButtonType_Normal,mDefaultImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CrossPromotionButton::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==LobbyEvent_SetCrossPromotion)
	{
		HandleSetCrossPromotionEvent((SetCrossPromotionEvent*)theEvent);
		return;
	}
	ImageTextButton::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CrossPromotionButtonConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="DEFAULTIMAGE")
		mButton->SetDefaultImage(ReadImage());
	else if(theInstruction=="DEFAULTBROWSE")
		mButton->SetDefaultLink(ReadString());
	else
		return ImageButtonConfig::HandleInstruction(theInstruction);

	return true;
}
