#include "WONControls.h"
#include "MSControls.h"
#include "ImageButton.h"
#include "SimpleComponent.h"
#include "ChildLayouts.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSkin::~WONSkin()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButton* WONSkin::GetPushButton()
{
	return mPushButton->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButton* WONSkin::GetCheckbox()
{
	return mCheckbox->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButton* WONSkin::GetRadioButton()
{
	return mRadioButton->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONHeaderButton* WONSkin::GetHeaderButton()
{
	return mHeaderButton->Duplicate();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButton* WONSkin::GetTabButton()
{
	return mTabButton->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSimpleButton* WONSkin::GetCloseButton()
{
	return mCloseButton->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSimpleButton* WONSkin::GetArrowButton(Direction theDirection)
{
	WONSimpleButton *aButton = mArrowButton[theDirection]->Duplicate();
	aButton->SetDesiredSize();
	return aButton;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrameComponent* WONSkin::GetPopupBack()
{
	WONFrameComponent *aComponent = new WONFrameComponent(mPopupBack->GetFrame());
	return aComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrameComponent* WONSkin::GetDialogBack()
{
	WONFrameComponent *aComponent = new WONFrameComponent(mDialogBack->GetFrame());
	return aComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSimpleButton* WONSkin::GetScrollThumb(bool isVertical)
{
	return mScrollThumb[isVertical?1:0]->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSimpleButton* WONSkin::GetScrollGutter(bool isVertical)
{
	return mScrollGutter[isVertical?1:0]->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONScrollbar* WONSkin::GetScrollbar(bool isVertical)
{
	WONScrollbar* aScrollbar = new WONScrollbar(isVertical);
	aScrollbar->CopyAttributes(mScrollbar[isVertical?1:0]);

	aScrollbar->SetThumb(GetScrollThumb(isVertical));
	aScrollbar->SetScrollRect(GetScrollGutter(isVertical));
	if(isVertical)
	{
		aScrollbar->SetUpArrow(GetArrowButton(Direction_Up));
		aScrollbar->SetDownArrow(GetArrowButton(Direction_Down));
		aScrollbar->SetWidth(aScrollbar->GetUpArrow()->Width());
	}
	else
	{
		aScrollbar->SetUpArrow(GetArrowButton(Direction_Left));
		aScrollbar->SetDownArrow(GetArrowButton(Direction_Right));
		aScrollbar->SetHeight(aScrollbar->GetUpArrow()->Height());
	}

	return aScrollbar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONInputBox* WONSkin::GetInputBox()
{
	return mInputBox->Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONComboBox* WONSkin::GetComboBox()
{
	WONComboBox *aCombo = mComboBox->Duplicate();

	Button *anArrow = mComboButton->Duplicate();
	anArrow->SetDesiredSize();
	anArrow->SetButtonFlags(ButtonFlag_RepeatFire,false);

	WONScrollContainer *aListBox = GetScrollContainer();
	aListBox->SetScrollbarConditions(ScrollbarCondition_Disable,ScrollbarCondition_Conditional);
	
	aListBox->SetFrame(WONFrame(new WONMSSkin::MSImage(WONMSSkin::MSImageType_ComboPopupFrame),0));
//	aListBox->SetDraw3DFrame(false);

	aCombo->SetButton(anArrow);
	aCombo->SetListBox(aListBox);

	return aCombo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONScrollContainer* WONSkin::GetScrollContainer()
{
	WONScrollContainer *aContainer = mScrollContainer->Duplicate();
	aContainer->SetScrollbar(GetScrollbar(true));
	aContainer->SetScrollbar(GetScrollbar(false));
	return aContainer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONListCtrl* WONSkin::GetListCtrl()
{
	WONListCtrl *aList = mListCtrl->Duplicate();
	aList->CopyAttributes(mScrollContainer);
	aList->SetHeader(new WONHeaderControl(this));

	MultiListArea *aListArea = new MultiListArea;
	aList->SetScrollArea(aListArea);
	aList->SetScrollbar(GetScrollbar(true));
	aList->SetScrollbar(GetScrollbar(false));

	aList->GetHeader()->SetMultiListArea(aListArea);


	return aList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONTabCtrl* WONSkin::GetTabCtrl()
{
	WONTabCtrl *aTabCtrl = mTabCtrl->Duplicate();
	aTabCtrl->SetSkin(this);
	aTabCtrl->SetTabArrows(GetArrowButton(Direction_Left),GetArrowButton(Direction_Right));
	return aTabCtrl;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSkin::CopyButtonType(const GUIString &theType, WONButton *theCopyTo)
{
	ButtonTypeMap::iterator anItr = mButtonTypeMap.find(theType);
	if(anItr==mButtonTypeMap.end())
		return false;
		
	theCopyTo->CopyAttributes(anItr->second);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSkin::CopySimpleButtonType(const GUIString &theType, WONSimpleButton *theCopyTo)
{
	SimpleButtonTypeMap::iterator anItr = mSimpleButtonTypeMap.find(theType);
	if(anItr==mSimpleButtonTypeMap.end())
		return false;
		
	theCopyTo->CopyAttributes(anItr->second);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONMSSkin::WONMSSkin()
{
	FontPtr aFont = WindowManager::GetDefaultWindowManager()->GetNamedFont("MSButton");

	// Push Button
	mPushButton = new WONButton;
	mPushButton->SetButtonFlags(ButtonFlag_InvalidateOnMouseOver,false);
	mPushButton->SetButtonFlags(ButtonFlag_WantDefault,true);
	mPushButton->SetDrawGrayText(true);
	mPushButton->SetButtonState(0,new MSImage(MSImageType_PushButton,0),-1,aFont);
	mPushButton->SetButtonState(1,new MSImage(MSImageType_PushButton,MSImageFlag_Down),-1,NULL);
	mPushButton->SetButtonState(4,new MSImage(MSImageType_PushButton,MSImageFlag_Default),-1,NULL);

	// Checkbox
	int cw = 13, ch = 13;
	mCheckbox = new WONButton;
	mCheckbox->SetPushOffsets(0,0);
	mCheckbox->SetHasCheck(true);
	mCheckbox->SetScaleImage(false);
	mCheckbox->SetTextPadding(cw+4,2,2,2);
	mCheckbox->SetButtonState(0,new MSImage(MSImageType_Checkbox,0,cw,ch),-1,aFont);
	mCheckbox->SetButtonState(1,new MSImage(MSImageType_Checkbox,MSImageFlag_Down,cw,ch),-1,NULL);
	mCheckbox->SetButtonState(0,new MSImage(MSImageType_Checkbox,MSImageFlag_Checked,cw,ch),-1,NULL,true);
	mCheckbox->SetButtonState(1,new MSImage(MSImageType_Checkbox,MSImageFlag_Checked | MSImageFlag_Down,cw,ch),-1,NULL,true);

	// RadioButton
	int rw = 13, rh = 13;
	mRadioButton = new WONButton;
	mRadioButton->SetButtonFlags(ButtonFlag_Radio,true);
	mRadioButton->SetPushOffsets(0,0);
	mRadioButton->SetHasCheck(true);
	mRadioButton->SetScaleImage(false);
	mRadioButton->SetTextPadding(rw+4,2,2,2);
	mRadioButton->SetButtonState(0,new MSImage(MSImageType_RadioButton,0,rw,rh),-1,aFont);
	mRadioButton->SetButtonState(1,new MSImage(MSImageType_RadioButton,MSImageFlag_Down,rw,rh),-1,NULL);
	mRadioButton->SetButtonState(0,new MSImage(MSImageType_RadioButton,MSImageFlag_Checked,rw,rh),-1,NULL,true);
	mRadioButton->SetButtonState(1,new MSImage(MSImageType_RadioButton,MSImageFlag_Checked | MSImageFlag_Down,rw,rh),-1,NULL,true);

	// HeaderButton
	mHeaderButton = new WONHeaderButton;
	mHeaderButton->SetVertTextPad(2,2);
	mHeaderButton->SetScaleImage(true);
	mHeaderButton->SetButtonState(0, new MSImage(MSImageType_ListHeader,0),-1,aFont);
	mHeaderButton->SetButtonState(1, new MSImage(MSImageType_ListHeader,MSImageFlag_Down),-1,aFont);

	// Tab Button
	mTabButton = new WONButton;
	mTabButton->SetScaleImage(true);
	mTabButton->SetHasCheck(true);
	mTabButton->SetPushOffsets(0,0);
	mTabButton->SetButtonState(0, new MSImage(MSImageType_TabButton,0),-1,aFont);
	mTabButton->SetButtonState(0, new MSImage(MSImageType_TabButton,MSImageFlag_Checked),-1,aFont);

	// Close Button
	mCloseButton = new WONSimpleButton;
	mCloseButton->SetImage(0, new MSImage(MSImageType_CloseButton,0,16,14));
	mCloseButton->SetImage(1, new MSImage(MSImageType_CloseButton,MSImageFlag_Down,16,14));

	// Arrow Buttons
	int i;
	for(i=0; i<4; i++)
	{
		mArrowButton[i] = new WONSimpleButton;
		mArrowButton[i]->SetImage(0,new MSImage(MSImageType_LeftArrow+i,0,15,15));
		mArrowButton[i]->SetImage(1,new MSImage(MSImageType_LeftArrow+i,MSImageFlag_Down,15,15));
		mArrowButton[i]->SetImage(2,new MSImage(MSImageType_LeftArrow+i,MSImageFlag_Disabled,15,15));
	}

	// Scrollbar, ScrollThumb, and ScrollGutter
	for(i=0; i<2; i++)
	{
		mScrollbar[i] = new WONScrollbar;

		mScrollThumb[i] = new WONScrollThumb;
		mScrollThumb[i]->SetImage(0,new MSImage(MSImageType_ScrollThumb,0));
		mScrollThumb[i]->SetScaleImage(true);

		mScrollGutter[i] = new WONSimpleButton;
		mScrollGutter[i]->SetScaleImage(true);
		mScrollGutter[i]->SetImage(0,new MSImage(MSImageType_ScrollGutter,0));
	}

	// ComboBox Button
	mComboButton = new WONSimpleButton;
	mComboButton->SetScaleImage(true);
	mComboButton->SetImage(0,new MSImage(MSImageType_DownArrow,0,16,18));
	mComboButton->SetImage(1,new MSImage(MSImageType_DownArrow,MSImageFlag_Down,16,18));
	mComboButton->SetImage(2,new MSImage(MSImageType_DownArrow,MSImageFlag_Disabled,16,18));

	mPopupBack = new WONFrameComponent;
	mPopupBack->SetImage(new MSImage(MSImageType_ScrollThumb,0));

	mDialogBack = new WONFrameComponent;
	mDialogBack->SetImage(new MSImage(MSImageType_ScrollThumb,0));

	mInputBox = new WONInputBox;
	mComboBox = new WONComboBox;
	mScrollContainer = new WONScrollContainer;
	mListCtrl = new WONListCtrl;
	mTabCtrl = new WONTabCtrl;
	mTabCtrl->SetFrame(WONFrame(new MSImage(MSImageType_TabCtrlFrame),2));

	mInputBox->SetFrame(WONFrame(new MSImage(MSImageType_Frame3,MSImageFlag_Down),2));
	mComboBox->SetFrame(WONFrame(new MSImage(MSImageType_Frame3,MSImageFlag_Down),2));
	mScrollContainer->SetFrame(WONFrame(new MSImage(MSImageType_Frame3,MSImageFlag_Down),2));
	mScrollContainer->SetCorner(new MSImage(MSImageType_ScrollCorner,0));
	mListCtrl->CopyAttributes(mScrollContainer);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONMSSkin::MSImage::DrawPushButton(Graphics &g, bool useFrame2, bool checkDefault)
{

	bool down = FlagSet(MSImageFlag_Down);

	int x = 0, y = 0, width = mWidth, height = mHeight;

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(0,0,width,height);

	if(checkDefault && (FlagSet(MSImageFlag_Default) || down))
	{
		g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
		g.DrawRect(0,0,width,height);
		x++;
		y++;
		width-=2;
		height-=2;
	
		if(down)
			useFrame2 = true;
	}

	if(useFrame2)	
		MSGraphicsUtil::DrawFrame2(g,x,y,width,height,down);
	else
		MSGraphicsUtil::DrawFrame1(g,x,y,width,height,down);
}

		
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONMSSkin::MSImage::DrawArrowButton(Graphics &g, Direction theDirection)
{
	DrawPushButton(g,true,false);
	if(FlagSet(MSImageFlag_Down))
		MSGraphicsUtil::DrawArrow(g,theDirection,1,1,mWidth,mHeight,FlagSet(MSImageFlag_Disabled));
	else
		MSGraphicsUtil::DrawArrow(g,theDirection,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Disabled));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONMSSkin::MSImage::DrawCloseButton(Graphics &g)
{
	DrawPushButton(g,false,false);
	if(FlagSet(MSImageFlag_Down))
		MSGraphicsUtil::DrawCloseX(g,1,1,mWidth,mHeight);
	else
		MSGraphicsUtil::DrawCloseX(g,0,0,mWidth,mHeight);
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONMSSkin::MSImage::Paint(Graphics &g)
{
	switch(mImageType)
	{
		case MSImageType_Frame1:		MSGraphicsUtil::DrawFrame1(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down)); return;
		case MSImageType_Frame2:		MSGraphicsUtil::DrawFrame2(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down)); return;
		case MSImageType_Frame3:		MSGraphicsUtil::DrawFrame3(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down)); return;

		case MSImageType_LeftArrow:		DrawArrowButton(g,Direction_Left);	return;
		case MSImageType_UpArrow:		DrawArrowButton(g,Direction_Up);	return;
		case MSImageType_RightArrow:	DrawArrowButton(g,Direction_Right);	return;
		case MSImageType_DownArrow:		DrawArrowButton(g,Direction_Down);	return;
		
		case MSImageType_PushButton:	DrawPushButton(g,false,true);	return;
		case MSImageType_TabCtrlFrame:	DrawPushButton(g,false,false); return;
		case MSImageType_ListHeader:	DrawPushButton(g,true,false);   return;
		case MSImageType_ScrollThumb:	DrawPushButton(g,true,false); return;
		case MSImageType_CloseButton:	DrawCloseButton(g); return;	

		case MSImageType_TabButton:		MSGraphicsUtil::DrawTab(g,0,0,mWidth,mHeight); return;
		case MSImageType_Checkbox:		MSGraphicsUtil::DrawCheck(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down),FlagSet(MSImageFlag_Checked)); return;
		case MSImageType_RadioButton:	MSGraphicsUtil::DrawRadio(g,0,0,mWidth,mHeight,FlagSet(MSImageFlag_Down),FlagSet(MSImageFlag_Checked)); return;

		case MSImageType_ScrollGutter:	g.ApplyColorSchemeColor(StandardColor_Scrollbar); g.FillRect(0,0,mWidth,mHeight); return;
		case MSImageType_ScrollCorner:  g.ApplyColorSchemeColor(StandardColor_3DFace); g.FillRect(0,0,mWidth,mHeight); return;
		
		case MSImageType_ComboPopupFrame: g.ApplyColorSchemeColor(StandardColor_3DDarkShadow); g.DrawRect(0,0,mWidth,mHeight); return;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrame::WONFrame(StretchImage *theImage)
{
	mImage = theImage;
	mLeft = theImage->GetLeft();
	mTop = theImage->GetTop();
	mRight = theImage->GetRight();
	mBottom = theImage->GetBottom();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrame::WONFrame(Image *theImage, int theSize)
{
	mImage = theImage;
	mLeft = mTop = mRight = mBottom = theSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrame::WONFrame(Image *theImage, int theLeft, int theTop, int theRight, int theBottom)
{
	mImage = theImage;
	mLeft = theLeft;
	mTop = theTop;
	mRight = theRight;
	mBottom = theBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrame::WONFrame(const WONFrame &theFrame)
{
	Copy(theFrame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrame::SetImage(Image *theImage)
{
	mImage = theImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrame::SetImage(Image *theImage, int theLeft, int theTop, int theRight, int theBottom)
{
	mImage = theImage;
	mLeft = theLeft;
	mTop = theTop;
	mRight = theRight;
	mBottom = theBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrame& WONFrame::operator=(const WONFrame &theFrame)
{
	Copy(theFrame);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrame::Copy(const WONFrame &from)
{
	if(from.mImage.get()==NULL)
		mImage = NULL;
	else
		mImage = from.mImage->Duplicate();

	mLeft = from.mLeft;
	mTop = from.mTop;
	mRight = from.mRight;
	mBottom = from.mBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrame::Paint(Graphics &g, Component *theComponent)
{
	if(mImage.get()==NULL)
		return;

	if(mImage->GetWidth()!=theComponent->Width() || mImage->GetHeight()!=theComponent->Height())
		mImage->SetSize(theComponent->Width(),theComponent->Height());

	g.DrawImage(mImage,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrame::Paint(Graphics &g, Component *theComponent, int lp, int tp, int rp, int bp)
{
	if(mImage.get()==NULL)
		return;

	if(mImage->GetWidth()!=theComponent->Width()-lp-rp || mImage->GetHeight()!=theComponent->Height()-tp-bp)
		mImage->SetSize(theComponent->Width()-lp-rp,theComponent->Height()-tp-bp);

	g.DrawImage(mImage,lp,tp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSimpleButton::WONSimpleButton()
{
	mScaleImage = false;
	mCheckMouseButtonDown = false;
	SetButtonFlags(ButtonFlag_InvalidateOnMouseOver,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSimpleButton::GetDesiredSize(int &width, int &height)
{
	width = height = 0;
	for(int i=0; i<4; i++)
	{
		Image *anImage = mImages[i];
		if(anImage!=NULL)
		{
			if(anImage->GetWidth()>width)
				width = anImage->GetWidth();

			if(anImage->GetHeight()>height)
				height = anImage->GetHeight();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSimpleButton* WONSimpleButton::Duplicate()
{
	WONSimpleButton *aButton = new WONSimpleButton;
	for(int i=0; i<4; i++)
		aButton->mImages[i] = mImages[i];

	aButton->mScaleImage = mScaleImage;
	aButton->mCheckMouseButtonDown = mCheckMouseButtonDown;
	aButton->CopyAttributes(this);

	aButton->SetSize(Width(),Height());

	return aButton;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSimpleButton::Paint(Graphics &g)
{
	Button::Paint(g);
	Image *anImage = NULL;
	if(Disabled())
		anImage = mImages[2];
	else if((mCheckMouseButtonDown && IsMouseButtonDown()) || (!mCheckMouseButtonDown && IsButtonDown()))
		anImage = mImages[1];
	else if(IsMouseOver())
		anImage = mImages[3];
	
	if(anImage==NULL)
		anImage = mImages[0];

	if(mScaleImage && (anImage->GetWidth()!=Width() || anImage->GetHeight()!=Height()))
		anImage->SetSize(Width(),Height());

	g.DrawImage(anImage,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButton::WONButton()
{
	mLeftTextPad = mRightTextPad = 8;
	mTopTextPad = mBottomTextPad = 4;
	mLeftFocusPad = mRightFocusPad = 4;
	mTopFocusPad = 3; mBottomFocusPad = 4;
	mPushDX = mPushDY = 1;
	mHasCheck = false;
	mScaleImage = true;
	mAdjustHeightToFitText = false;
	mWrapText = false;
	mDrawGrayText = true;
	mIsTransparent = false;

	mHAlign = HorzAlign_Center;
	mVAlign = VertAlign_Center;

	mFocusStyle = 1;

	SetButtonFlags(ButtonFlag_InvalidateOnMouseOver,true);
	SetComponentFlags(ComponentFlag_WantFocus,true);

	mDefaultTextColor = ColorScheme::GetColorRef(StandardColor_ButtonText);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetTextPaddingAtLeast(int left, int top, int right, int bottom)
{
	if(left>mLeftTextPad)
		mLeftTextPad = left;

	if(top>mTopTextPad)
		mTopTextPad = top;

	if(right>mRightTextPad)
		mRightTextPad = right;

	if(bottom>mBottomTextPad)
		mBottomTextPad = bottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetTextHook(const GUIString &theText)
{
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second;
		aText->SetText(theText);
		++anItr;
	}

	AdjustHeightToFitText(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const GUIString& WONButton::GetText()
{
	if(mFontTextMap.empty())
		return GUIString::EMPTY_STR;
	else
		return mFontTextMap.begin()->second->GetText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SizeChanged()
{
	Button::SizeChanged();
	if(!mWrapText)
		return;

	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second;
		aText->SetMaxWidth(Width()-mLeftTextPad-mRightTextPad);
		++anItr;
	}

	AdjustHeightToFitText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::AdjustHeightToFitText()
{
	if(!mAdjustHeightToFitText)
		return;

	int width,height;
	GetTextSize(width,height);

	if(Height()-mTopTextPad-mBottomTextPad!=height)
		SetSize(Width(),height+mTopTextPad+mBottomTextPad);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetAdjustHeightToFitText(bool adjust)
{
	if(adjust==mAdjustHeightToFitText)
		return;

	mAdjustHeightToFitText = adjust;
	AdjustHeightToFitText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetWrapText(bool wrap)
{
	if(mWrapText==wrap)
		return;

	mWrapText = wrap;
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second;
		aText->SetWrap(wrap);

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetTransparent(bool isTransparent)
{
	mIsTransparent = isTransparent;
	if(isTransparent)
		SetComponentFlags(ComponentFlag_GrabBG,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetButtonState(int theState, Image *theImage, int theColor, Font *theFont, bool checked)
{
	int checkedIndex = checked?1:0;
	ButtonState &aState = mStates[theState][checkedIndex];
	if(checkedIndex)
		mHasCheck = true;

	FontPtr aFont;
	if(mFontTextMap.empty() && theFont==NULL)
		theFont = aFont = WindowManager::GetDefaultWindowManager()->GetDefaultFont();

	if(theFont!=NULL)
	{
		FontTextMap::iterator anItr = mFontTextMap.insert(FontTextMap::value_type(theFont,NULL)).first;
		if(anItr->second.get()==NULL)
		{
			WrappedTextPtr aText = new WrappedText;
			aText->SetCentered(mHAlign==HorzAlign_Center);
			aText->SetWrap(false);
			aText->SetFont(theFont);
			anItr->second = aText;
		}

		aState.mText = anItr->second;
	}

	aState.mImage = theImage;
	aState.mColor = theColor;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::GetTextSize(int &width, int &height)
{
	width = 0; height = 0;
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second;
		if(aText->GetWidth() > width)
			width = aText->GetWidth();

		if(aText->GetHeight() > height)
			height = aText->GetHeight();

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::GetDesiredSize(int &width, int &height)
{
	GetTextSize(width,height);
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<2; j++)
		{
			Image *anImage = mStates[i][j].mImage;
			if(anImage!=NULL)
			{
				if(!mScaleImage && anImage->GetWidth()>width)
					width = anImage->GetWidth();
			
				if(!mScaleImage && anImage->GetHeight() > height)
					height = anImage->GetHeight();
			}
		}
	}


	width += mLeftTextPad + mRightTextPad;
	height += mTopTextPad + mBottomTextPad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetHorzAlign(HorzAlign theHorzAlign)
{
	if(theHorzAlign==mHAlign)
		return;

	if(mHAlign!=HorzAlign_Center && theHorzAlign!=HorzAlign_Center)
		return;

	mHAlign = theHorzAlign;

	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second;
		aText->SetCentered(mHAlign==HorzAlign_Center); 
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetVertAlign(VertAlign theVertAlign)
{ 
	mVAlign = theVertAlign; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetTextColor(int theColor)
{
	if(!mScaleImage)
	{
		for(int i=0; i<5; i++)
			for(int j=0; j<2; j++)
				mStates[i][j].mColor = theColor;
	}
	else
	{
		mStates[0][0].mColor = theColor;
		mStates[0][1].mColor = theColor;
		mStates[4][0].mColor = theColor;
		mStates[4][1].mColor = theColor;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::SetFont(Font *theFont)
{
	mFontTextMap.clear();
	WrappedTextPtr aText = mStates[0][0].mText;
	aText->SetFont(theFont);
	mFontTextMap[theFont] = aText;
	
	for(int i=0; i<5; i++)
		for(int j=0; j<2; j++)
			mStates[i][j].mText = aText;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::CopyAttributes(WONButton *from)
{
	Button::CopyAttributes(from);
	mLeftTextPad		= from->mLeftTextPad;
	mTopTextPad			= from->mTopTextPad;
	mRightTextPad		= from->mRightTextPad;
	mBottomTextPad		= from->mBottomTextPad;
	mLeftFocusPad		= from->mLeftFocusPad;
	mTopFocusPad		= from->mTopFocusPad;
	mRightFocusPad		= from->mRightFocusPad;
	mBottomFocusPad		= from->mBottomFocusPad;
	mPushDX				= from->mPushDX;
	mPushDY				= from->mPushDY;
	mHasCheck			= from->mHasCheck;
	mScaleImage			= from->mScaleImage;
	mHAlign				= from->mHAlign;
	mVAlign				= from->mVAlign;
	mDrawGrayText		= from->mDrawGrayText;
	mIsTransparent		= from->mIsTransparent;
	mFocusStyle			= from->mFocusStyle;
	mDefaultTextColor	= from->mDefaultTextColor;
	if(from->ComponentFlagSet(ComponentFlag_GrabBG))
		SetComponentFlags(ComponentFlag_GrabBG,true);
	if(from->ComponentFlagSet(ComponentFlag_Translucent))
		SetComponentFlags(ComponentFlag_Translucent,true);

	mFontTextMap = from->mFontTextMap;
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		anItr->second = new WrappedText;
		anItr->second->SetCentered(mHAlign==HorzAlign_Center);
		anItr->second->SetWrap(mWrapText);
		anItr->second->SetFont(anItr->first);
		++anItr;
	}


	for(int i=0; i<5; i++)
	{
		for(int j=0; j<2; j++)
		{
			ButtonState &src = from->mStates[i][j];
			ButtonState &dst = mStates[i][j];

			dst.mColor = src.mColor;

			if(src.mImage.get()!=NULL)
			{
				if(mScaleImage)
					dst.mImage = src.mImage->Duplicate();
				else
					dst.mImage = src.mImage;
			}

			if(src.mText.get()!=NULL)
				dst.mText = mFontTextMap[src.mText->GetFont()];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONButton* WONButton::Duplicate()
{
	WONButton* aCopy = new WONButton;
	aCopy->CopyAttributes(this);

	return aCopy;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::DrawButtonText(Graphics &g, WrappedText *theText, int theColor)
{
	if(theText==NULL)
		return;

	int x,y;
	if(mScaleImage)
	{
		switch(mHAlign)
		{
			case HorzAlign_Left: x = mLeftTextPad; break;
			case HorzAlign_Center: x = (Width() - theText->GetWidth() - mLeftTextPad - mRightTextPad)/2 + mLeftTextPad; break;
			case HorzAlign_Right: x = Width() - theText->GetWidth() - mRightTextPad; break;

		}
		y = (Height() - theText->GetHeight())/2;
		if(IsButtonDown())
		{
			x+=mPushDX;
			y+=mPushDY;
		}
	}
	else
	{
		x = mLeftTextPad;
		y = (Height() - theText->GetHeight())/2;
	}

	if(Disabled() && mDrawGrayText)
	{
		g.ApplyColorSchemeColor(StandardColor_3DHilight);
		theText->Paint(g,x+1,y+1);
		g.ApplyColorSchemeColor(StandardColor_GrayText);
		theText->Paint(g,x,y);
	}
	else
	{
		g.SetColor(theColor);
		theText->Paint(g,x,y);
		if(mFocusStyle && HasFocus() && GetParent()->GetHasUsedDialogKeys())
		{
			if(mScaleImage)
				g.DrawDottedRect(mLeftFocusPad,mTopFocusPad,Width()-mLeftFocusPad-mRightFocusPad,Height()-mTopFocusPad-mBottomFocusPad);
			else
				g.DrawDottedRect(mLeftTextPad-1,y-1,theText->GetWidth()+2,theText->GetHeight()+2);
		}
		
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::GetButtonState(Graphics &g, int *theColor, WrappedText **theText, Image **theImage)
{
	int aState = 0;
	if(Disabled())
		aState = 2;
	else if(IsButtonDown())
		aState = 1;
	else if(IsMouseOver() && ButtonFlagSet(ButtonFlag_InvalidateOnMouseOver))
		aState = 3;
	else if(IsDefaultButton())
		aState = 4;

	int aCheckedIndex = (mHasCheck && IsChecked())?1:0;

	ButtonState &aStateStruct = mStates[aState][aCheckedIndex];

	Image *anImage = aStateStruct.mImage;
	int aColor = aStateStruct.mColor;
	WrappedText *aText = aStateStruct.mText;

	if(anImage==NULL) anImage = mStates[0][aCheckedIndex].mImage;
	if(aColor==-1) aColor = mStates[0][aCheckedIndex].mColor;
	if(aText==NULL) aText = mStates[0][aCheckedIndex].mText;

	if(aCheckedIndex==1)
	{
		if(anImage==NULL) anImage = mStates[0][0].mImage;
		if(aColor==-1) aColor = mStates[0][0].mColor;
		if(aText==NULL) aText = mStates[0][0].mText;
	}

	if(aColor==-1)
		aColor = mDefaultTextColor;

	if(theText) *theText = aText;
	if(theColor) *theColor = aColor;
	if(theImage) *theImage = anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONButton::Paint(Graphics &g)
{
	Button::Paint(g);
	if(!mIsTransparent)
	{
		g.ApplyColorSchemeColor(StandardColor_3DFace);
		g.FillRect(0,0,Width(),Height());
	}

	Image *anImage; 
	int aColor; 
	WrappedText *aText; 
	GetButtonState(g,&aColor,&aText,&anImage);

	if(mScaleImage)
	{
		if(anImage!=NULL)
		{
			if(anImage->GetWidth()!=Width() || anImage->GetHeight()!=Height())
				anImage->SetSize(Width(),Height());

			g.DrawImage(anImage,0,0);
		}
		DrawButtonText(g,aText,aColor);
	}
	else
	{
		int y = 0;

		if(anImage!=NULL)
		{
			int aHeight = anImage->GetHeight();
			if(mVAlign==HorzAlign_Center) y = (Height() - aHeight - mTopTextPad - mBottomTextPad)/2;
			else if(mVAlign==VertAlign_Bottom) y = Height() - aHeight - mTopTextPad - mBottomTextPad;

			g.DrawImage(anImage,0,y + mTopTextPad);
		}
		DrawButtonText(g,aText,aColor);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONHeaderButton::WONHeaderButton()
{
	SetHorzAlign(HorzAlign_Left);

	mLeftTextPad = mRightTextPad = 4;
	mTopTextPad = mBottomTextPad = 4;

	SetComponentFlags(ComponentFlag_WantFocus,false);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONHeaderButton* WONHeaderButton::Duplicate()
{
	WONHeaderButton *aButton = new WONHeaderButton;
	aButton->CopyAttributes(this);
	return aButton;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONHeaderButton::Paint(Graphics &g)
{
	WONButton::Paint(g);

	ListSortType aSortType = MultiListHeader::GetSortTypeForButton(this);
	if(aSortType==ListSortType_None)
		return;

	int aColor;
	WrappedText *aText;

	GetButtonState(g,&aColor, &aText);
	g.SetColor(aColor);
	int x;
	if(aText==NULL || aText->GetWidth()==0)
		x = (Width()-4)/2;
	else
		x = aText->GetWidth() + 10 + mLeftTextPad;
	
	int y = Height()/2-2;
	int offset = IsButtonDown()?1:0;
	x+=offset;
	y+=offset;

	int i;
	if(aSortType==ListSortType_Ascending)
	{
		for(i=0; i<4; i++)
			g.DrawLine(x-i, y+i, x+i, y+i);	

	} 
	else if(aSortType==ListSortType_Descending)
	{
		for(i=0; i<4; i++)
			g.DrawLine(x-(3-i), y+i, x+(3-i), y+i);			
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONScrollThumb::WONScrollThumb()
{
	SetCheckMouseButtonDown(true);
	SetScaleImage(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONScrollbar::WONScrollbar(bool vertical) : Scrollbar(vertical) 
{
	mIsTransparent = false;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONInputBox::WONInputBox()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONInputBox* WONInputBox::Duplicate()
{
	WONInputBox *anInput = new WONInputBox;
	anInput->SetFrame(mFrame);
	
	return anInput;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONInputBox::Paint(Graphics &g)
{
	InputBox::Paint(g);
	mFrame.Paint(g,this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONComboBox::WONComboBox()
{
	mDrawFrame = true;
	mButtonInFrame = true;

	mInputBox = new InputBox;
	mListArea = new ListArea;
	mListArea->SetListFlags(ListFlag_SelOver, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONComboBox* WONComboBox::Duplicate()
{
	WONComboBox *aCombo = new WONComboBox;
	aCombo->SetFrame(mFrame);
	aCombo->SetDrawFrame(mDrawFrame);
	aCombo->SetButtonInFrame(mButtonInFrame);

	return aCombo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONComboBox::GetDesiredSize(int &width, int &height)
{
	if(mInputBox.get()!=NULL)
	{
		mInputBox->GetDesiredSize(width,height);
		height+=2;
//		if(mDrawFrame)
//			height+=mFrame.mTop + mFrame.mBottom;
	}
	else 
		height = 22;

	if(mListArea.get()!=NULL && mListArea->GetNumItems()>0)
	{
		width = 10 + mListArea->GetTotalWidth();
		if(mButton.get()!=NULL)
			width += mButton->Width();
	}
	else
		width = 100;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONComboBox::SizeChanged()
{
	ComboBox::SizeChanged();
	int aTopPad = 0, aBottomPad = 0, aRightPad = 0, aLeftPad = 0;
	if(mDrawFrame)
	{
		aLeftPad = mFrame.mLeft;
		aTopPad = mFrame.mTop;
		aBottomPad = mFrame.mBottom;
		aRightPad = mFrame.mRight;
	}

	int anInputBoxHeight = Height(); //mInputBox->Height();
	int aButtonHeight = anInputBoxHeight-aBottomPad-aTopPad;
	int aButtonWidth = mButton->Width();  
	if(!mButton->IsVisible())
		aButtonWidth = 0;
//	if(aButtonWidth>aButtonHeight)
//		aButtonWidth = aButtonHeight;
 
	int anInputBoxWidth = Width()-aButtonWidth-aRightPad;


	mInputBox->SetLeftPad(aLeftPad + 2);
	mInputBox->SetPosSize(0,0,anInputBoxWidth,anInputBoxHeight);
	
	int aButtonLeft = mInputBox->Right();
	if(mButton->IsVisible())
	{
		if(mButtonInFrame)
			mButton->SetPosSize(mInputBox->Right(),aTopPad,aButtonWidth,aButtonHeight);
		else
			mButton->SetPosSize(Width()-aButtonWidth,0,aButtonWidth,Height());
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONComboBox::Paint(Graphics& g)
{
	ComboBox::Paint(g);

	if(mDrawFrame)
	{
		if(mButtonInFrame || !mButton->IsVisible())
			mFrame.Paint(g,this);
		else
			mFrame.Paint(g,this,0,0,mButton->Width(),0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONScrollContainer::WONScrollContainer()
{
	mHeaderHeight = 0;
	mTransparent = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONScrollContainer::SizeChanged()
{
	Container::SizeChanged();
	if(mScrollArea.get()==NULL)
		return;

	bool horzOn = mHorzScrollbar->IsVisible();
	bool vertOn = mVertScrollbar->IsVisible();

	int left = mFrame.mLeft, right = mFrame.mRight, top = mFrame.mTop, bottom = mFrame.mBottom;

	int aScrollbarWidth = mVertScrollbar->Width();
	int aScrollbarHeight = mHorzScrollbar->Height();

	int aScrollWidth = Width() - left - right;
	if(vertOn)
		aScrollWidth -= aScrollbarWidth;

	int aScrollHeight = Height() - top - bottom;
	if(horzOn)
		aScrollHeight -= aScrollbarHeight;

	if(vertOn)
		mVertScrollbar->SetPosSize(Width()-aScrollbarWidth-right,top,aScrollbarWidth,aScrollHeight);

	if(horzOn)
		mHorzScrollbar->SetPosSize(left,Height()-aScrollbarHeight-bottom,aScrollWidth,aScrollbarHeight);

	mScrollArea->SetPosSize(left,mHeaderHeight+top,aScrollWidth,aScrollHeight-mHeaderHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONScrollContainer* WONScrollContainer::Duplicate()
{
	WONScrollContainer *aContainer = new WONScrollContainer;
	aContainer->CopyAttributes(this);
	return aContainer;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONScrollContainer::CopyAttributes(WONScrollContainer *from)
{
	mFrame = from->mFrame;

	SetCorner(from->mCorner);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONScrollContainer::Paint(Graphics &g)
{
	Container::Paint(g);

	if(mVertScrollbar->IsVisible() && mHorzScrollbar->IsVisible() && !mTransparent && mCorner.get()!=NULL) // grey square in corner
	{
		g.DrawImage(mCorner,mVertScrollbar->Left(),mVertScrollbar->Bottom());

//		g.ApplyColorSchemeColor(StandardColor_Scrollbar);
//		g.FillRect(mVertScrollbar->Left(),mVertScrollbar->Bottom(),mVertScrollbar->Width(),mHorzScrollbar->Height());
	}

	mFrame.Paint(g,this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONHeaderControl::WONHeaderControl(WONSkin *theSkin)
{
	mSkin = theSkin;
	mTextLeftPad = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONHeaderControl::SetColumn(int theCol, const GUIString &theTitle, int *theDesiredHeight)
{
	WONHeaderButtonPtr aButton = mSkin->GetHeaderButton();
	aButton->SetText(theTitle);
//	aButton->SetComponentFlags(ComponentFlag_WantFocus,false);
//	aButton->SetHorzAlign(HorzAlign_Left);
//	aButton->SetHorzTextPad(mTextLeftPad+2,0);
	if(theDesiredHeight!=NULL)
	{
		int width;
		aButton->GetDesiredSize(width,*theDesiredHeight);
	}

	SetColumnItem(theCol, aButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONHeaderControl::SetSkin(WONSkin *theSkin) 
{ 
	mSkin = theSkin; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONListCtrl::WONListCtrl()
{
	mHeaderHeight = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONListCtrl::SetHeader(WONHeaderControl *theHeader)
{
	mHeader = theHeader;
	AddChild(theHeader);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONListCtrl::SetStandarColumnHeader(int theCol, const GUIString &theTitle, int theWidth)
{
	int aDesiredHeight = 0;
	mHeader->SetColumn(theCol,theTitle, &aDesiredHeight);

	if(mHeaderHeight==0)
	{
		if(aDesiredHeight>0)
			mHeaderHeight = aDesiredHeight;
		else
			mHeaderHeight = 15;
	}
		
	GetListArea()->SetColumnWidth(theCol,theWidth);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONListCtrl::SetColumnHeader(int theCol, Component *theHeader, int theWidth)
{
	if(mHeaderHeight==0)
	{
		if(theHeader->Height()>0)
			mHeaderHeight = theHeader->Height();
		else
			mHeaderHeight = 15;
	}

	mHeader->SetColumnItem(theCol,theHeader);
	GetListArea()->SetColumnWidth(theCol,theWidth);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONListCtrl* WONListCtrl::Duplicate()
{
	WONListCtrl *aList = new WONListCtrl;
	aList->mHeaderHeight = mHeaderHeight;

	return aList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONListCtrl::SizeChanged()
{
	WONScrollContainer::SizeChanged();

	mHeader->SetPosSize(mScrollArea->Left(),mScrollArea->Top()-mHeaderHeight,mScrollArea->Width(),mHeaderHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrameComponent::WONFrameComponent(Image *theImage, int theLeft, int theTop, int theRight, int theBottom)
{
	SetImage(theImage, theLeft, theTop, theRight, theBottom);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFrameComponent::WONFrameComponent(const WONFrame& theFrame)
{
	mFrame.Copy(theFrame);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrameComponent::SetImage(Image *theImage)
{
	mFrame.SetImage(theImage);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrameComponent::SetImage(Image *theImage, int theLeft, int theTop, int theRight, int theBottom)
{
	mFrame.SetImage(theImage, theLeft, theTop, theRight, theBottom);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrameComponent::SetBorders(int theLeft, int theTop, int theRight, int theBottom)
{
	mFrame.mLeft = theLeft;
	mFrame.mTop = theTop;
	mFrame.mRight = theRight;
	mFrame.mBottom = theBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFrameComponent::Paint(Graphics &g)
{	
	Component::Paint(g);
	mFrame.Paint(g, this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONTabCtrl::WONTabCtrl(WONSkin *theSkin)
{
	mSkin = theSkin;
	mIsTransparent = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONTabCtrl::SetFrame(const WONFrame &theFrame) 
{ 
	mFrame = theFrame; 
	ImageComponentPtr aFrame = new ImageComponent;
	aFrame->SetStretch(true);
	aFrame->SetImage(mFrame.mImage);

	AddChildLayout(aFrame,CLI_SameLeft | CLI_SameTop | CLI_SameSize,mContainerLayout);
	AddChild(aFrame);

	if(!mIsTransparent)
	{
		mBack = new MSDlgBack;
		AddChildLayout(mBack,CLI_SameLeft | CLI_SameTop | CLI_SameSize,aFrame,mFrame.mLeft,mFrame.mTop,-mFrame.mRight-mFrame.mLeft,-mFrame.mTop-mFrame.mBottom);
		AddChild(mBack);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONTabCtrl::SetTransparent(bool isTransparent)
{
	if(mBack.get()!=NULL && isTransparent)
		RemoveChild(mBack);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONTabCtrl::AddStandardTab(const GUIString &theText, Container *theContainer)
{
	WONButtonPtr aButton = mSkin->GetTabButton();
	aButton->SetText(theText);
	aButton->SetDesiredWidth();
	AddTab(aButton,theContainer);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONTabCtrl* WONTabCtrl::Duplicate()
{
	WONTabCtrl *aTabCtrl = new WONTabCtrl(mSkin);
	aTabCtrl->CopyAttributes(this);
	aTabCtrl->SetTransparent(mIsTransparent);
	aTabCtrl->SetFrame(mFrame);
	return aTabCtrl;
}
