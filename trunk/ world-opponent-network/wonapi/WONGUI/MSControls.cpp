#include "MSControls.h"
#include "SimpleComponent.h"
#include "ChildLayouts.h"
#include "WONCommon/WONFile.h"

#ifdef WIN32_NOT_XBOX
#include <commdlg.h>
#endif

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ButtonPtr WONAPI::DefaultMSArrowButtonFactory(Direction theDirection)
{
	return new MSArrowButton(theDirection);
}
MSArrowButtonFactory MSScrollbar::ArrowButtonFactory = DefaultMSArrowButtonFactory;
MSScrollbar::ThumbFactoryFunc MSScrollbar::ThumbFactory = MSScrollbar::DefaultThumbFactory;

MSArrowButtonFactory MSComboBox::ArrowButtonFactory = DefaultMSArrowButtonFactory;

int MSCheckbox::mCheckboxWidth = 13;
int MSCheckbox::mCheckboxHeight = 13;
MSCheckbox::CheckboxPainterFunc MSCheckbox::CheckboxPainter = MSCheckbox::DefaultCheckboxPainterFunc;

int MSRadioButton::mRadioWidth = 13;
int MSRadioButton::mRadioHeight = 13;
MSRadioButton::RadioPainterFunc MSRadioButton::RadioPainter = MSRadioButton::DefaultRadioPainterFunc;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawFrame1(Graphics &g, int x, int y, int width, int height, bool down)
{
	g.Translate(x,y);

	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawRect(0,0,width,height);
	g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);

	if(down)
	{
		g.DrawLine(0,0,width-1,0);
		g.DrawLine(0,0,0,height-1);

		g.ApplyColorSchemeColor(StandardColor_3DFace);
		g.DrawLine(width-2,1,width-2,height-2);
		g.DrawLine(width-2,height-2,2,height-2);

		g.ApplyColorSchemeColor(StandardColor_3DShadow);
		g.DrawLine(1,1,width-2,1);
		g.DrawLine(1,1,1,height-2);		
	}
	else
	{
		g.DrawLine(width-1,0,width-1,height-1);
		g.DrawLine(0,height-1,width-1,height-1);

		g.ApplyColorSchemeColor(StandardColor_3DShadow);
		g.DrawLine(1,height-2,width-2,height-2);
		g.DrawLine(width-2,height-2,width-2,1);
	}	

	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawFrame2(Graphics &g, int x, int y, int width, int height, bool down)
{
	g.Translate(x,y);

	if(!down)
	{
		g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
		g.DrawLine(width-1,0,width-1,height-1);
		g.DrawLine(0,height-1,width-1,height-1);

		g.ApplyColorSchemeColor(StandardColor_3DHilight);
		g.DrawLine(1,1,1,height-2);
		g.DrawLine(1,1,width-2,1);

		g.ApplyColorSchemeColor(StandardColor_3DShadow);
		g.DrawLine(1,height-2,width-2,height-2);
		g.DrawLine(width-2,height-2,width-2,1);
	}
	else
	{
		g.ApplyColorSchemeColor(StandardColor_3DShadow);
		g.DrawRect(0,0,width,height);
	}

	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawFrame3(Graphics &g, int x, int y, int width, int height, bool down)
{
	g.Translate(x,y);

	g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
	g.DrawRect(0,0,width,height);
	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawLine(width-1,0,width-1,height-1);
	g.DrawLine(0,height-1,width-1,height-1);
	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawLine(1,1,width-2,1);
	g.DrawLine(1,1,1,height-2);		
	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.DrawLine(width-2,1,width-2,height-2);
	g.DrawLine(1,height-2,width-2,height-2);		

	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawArrow(Graphics &g, Direction theDirection, int x, int y, int width, int height, bool disabled)
{
	int i = 0;

	if(disabled)
		g.ApplyColorSchemeColor(StandardColor_GrayText);
	else
		g.ApplyColorSchemeColor(StandardColor_ButtonText);

	int arrow_size = 4;
	int minSize = width;
	if(height<minSize)
		minSize = height;

	if(minSize<12)
		arrow_size = minSize/3;

	if(arrow_size<2)
		arrow_size = 2;

	int arrow_size2 = arrow_size/2;


	if(theDirection==Direction_Left)
	{
		int x = width/2-arrow_size2, y = height/2;
		for(i=0; i<arrow_size; i++)
			g.DrawLine(x+i, y-i, x+i, y+i);	

		if(disabled)
		{
			g.ApplyColorSchemeColor(StandardColor_3DHilight);
			g.DrawLine(x+i,y-i+arrow_size2,x+i,y+i);
		}
	}
	else if(theDirection==Direction_Right)
	{
		int x = width/2-arrow_size2, y = height/2;
		for(i=0; i<arrow_size; i++)
		{
			g.DrawLine(x+(3-i), y-i, x+(3-i), y+i);	
			if(disabled && i<3)
			{
				g.ApplyColorSchemeColor(StandardColor_3DHilight);
				g.DrawLine(x+(arrow_size-i-1),y+i+1,x+(arrow_size-i-1),y+i+2);
				g.ApplyColorSchemeColor(StandardColor_GrayText);
			}
		}

		if(disabled)
			g.SetPixel(x+4,y+1,g.GetColorSchemeColor(StandardColor_3DHilight));
	}
	else if(theDirection==Direction_Up)
	{
		int x = width/2, y = height/2-arrow_size2;
		for(i=0; i<arrow_size; i++)
			g.DrawLine(x-i, y+i, x+i, y+i);	

		if(disabled)
		{
			g.ApplyColorSchemeColor(StandardColor_3DHilight);
			g.DrawLine(x-i+2, y+i, x+i, y+i);
		}
	}
	else if(theDirection==Direction_Down)
	{
		int x = width/2, y = height/2-arrow_size2;
		for(i=0; i<arrow_size; i++)
		{
			g.DrawLine(x-(arrow_size-i-1), y+i, x+(arrow_size-i-1), y+i);	
			if(disabled && i>0)
			{
				g.ApplyColorSchemeColor(StandardColor_3DHilight);
				g.DrawLine(x+(arrow_size-i-1)+1,y+i,x+(arrow_size-i-1)+2,y+i);
				g.ApplyColorSchemeColor(StandardColor_GrayText);
			}
		}

		if(disabled)
			g.SetPixel(x+1,y+arrow_size,g.GetColorSchemeColor(StandardColor_3DHilight));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawCheck(Graphics &g, int x, int y, int width, int height, bool down, bool checked)
{
	g.Translate(x,y);

	if(down)
		g.ApplyColorSchemeColor(StandardColor_3DFace);
	else
		g.ApplyColorSchemeColor(StandardColor_Back);

	g.FillRect(0,0,width,height);

	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawLine(0,0,width-1,0);
	g.DrawLine(0,0,0,height-1);

	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawLine(width-1,0,width-1,height-1);
	g.DrawLine(0,height-1,width-1,height-1);

	g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
	g.DrawLine(1,1,width-2,1);
	g.DrawLine(1,1,1,height-2);

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.DrawLine(width-2,1,width-2,height-2);
	g.DrawLine(1,height-2,width-2,height-2);

	if(checked)
	{
		g.ApplyColorSchemeColor(StandardColor_Text);
		int x = 3;
		int y = height/2 - 1;
		int i;
		for(i=0; i<3; i++, x++, y++)
			g.DrawLine(x,y,x,y+2);

		y-=2;
		for(i=0; i<4; i++, x++, y--)
			g.DrawLine(x,y,x,y+2);
	}
	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawRadio(Graphics &g, int x, int y, int width, int height, bool down, bool checked)
{
	g.Translate(x,y);

	if(down)
		g.ApplyColorSchemeColor(StandardColor_3DFace);
	else
		g.ApplyColorSchemeColor(StandardColor_Back);

	g.FillEllipse(0,0,width,height);

	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawArc(0,0,width,height,45,180);
	g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
	g.DrawArc(1,1,width-2,height-2,45,180);
	
	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawArc(0,0,width,height,235,180);
	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.DrawArc(1,1,width-2,height-2,235,180);

	if(checked)
	{
		g.ApplyColorSchemeColor(StandardColor_Text);
		g.FillEllipse(width/2-2,height/2-2,4,4);
	}

	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawTab(Graphics &g, int x, int y, int width, int height)
{
	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(x,y+2,width-x,height-y-2);
	g.DrawLine(x+2,y+1,width-1,y+1);
	g.DrawLine(x+1,y+2,width-1,y+2);

	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawLine(x,y+2,x,height-1);
	g.SetPixel(x+1,y+1,g.GetColorSchemeColor(StandardColor_3DHilight));
	g.DrawLine(x+2,y,width-2,y);

	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawLine(width-2,y+2,width-2,height-1);
	g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
	g.SetPixel(width-2,y+1,g.GetColorSchemeColor(StandardColor_3DDarkShadow));
	g.DrawLine(width-1,y+2,width-1,height-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSGraphicsUtil::DrawCloseX(Graphics &g, int x, int y, int width, int height)
{
	g.Translate(x,y);
	g.ApplyColorSchemeColor(StandardColor_ButtonText);
	int hmid = width/2;
	int vmid = height/2;
	int left = hmid-4;
	int right = hmid+3;
	int top = vmid-4;
	int bottom = vmid+3;
	for(int i=0; i<4; i++)
	{
		g.DrawLine(left+i,top+i,left+i+1,top+i);
		g.DrawLine(right-i,top+i,right-1-i,top+i);
		g.DrawLine(left+i,bottom-i,left+i+1,bottom-i);
		g.DrawLine(right-i,bottom-i,right-i-1,bottom-i);
	}
	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSButtonBase::MSButtonBase()
{
//	mFont = GetFont(FontDescriptor("Tahoma",FontStyle_Plain,8));
	mText = new WrappedText;
	mText->SetWrap(false);
	mText->SetFont(GetNamedFont("MSButton"));

	mStandardTextColor = StandardColor_ButtonText;
	mTextColor = -1;

	mHAlign = HorzAlign_Center;
	mVAlign = VertAlign_Center;
	mFocusStyle = FocusStyle_AroundWindow;
	mTopTextPad = mBottomTextPad = mLeftTextPad = mRightTextPad = 2;
	mIsTransparent = false;
	mAdjustHeightToFitText = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::SetStandardTextColor(StandardColor theColor)
{
	mStandardTextColor = theColor;
	mTextColor = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::SetTextHook(const GUIString &theText)
{
	mText->SetText(theText); 
	AdjustHeightToFitText(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::SetTransparent(bool isTransparent)
{
	mIsTransparent = isTransparent;
	if(isTransparent)
		SetComponentFlags(ComponentFlag_GrabBG,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::GetDesiredSize(int &width, int &height)
{
	width = mText->GetWidth() + mLeftTextPad + mRightTextPad;
	height = mText->GetHeight() + mTopTextPad + mBottomTextPad;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSButtonBase::~MSButtonBase()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::AdjustHeightToFitText()
{
	if(!mAdjustHeightToFitText)
		return;

	if(Height()-mTopTextPad-mBottomTextPad!=mText->GetHeight())
		SetSize(Width(),mText->GetHeight()+mTopTextPad+mBottomTextPad);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::SetAdjustHeightToFitText(bool adjust)
{
	if(adjust==mAdjustHeightToFitText)
		return;

	mAdjustHeightToFitText = adjust;
	AdjustHeightToFitText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::SizeChanged()
{
	Button::SizeChanged();
	mText->SetMaxWidth(Width()-mLeftTextPad-mRightTextPad);
	AdjustHeightToFitText();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButtonBase::DrawButtonText(Graphics &g, int xoff, int yoff)
{
/*	g.SetFont(mFont);
	Font *aFM = mFont;
	int aTextWidth = aFM->GetStringWidth(mText);
	int aTextHeight = aFM->GetHeight();*/

	int aTextWidth = mText->GetWidth();
	int aTextHeight = mText->GetHeight();
	int aWidth = Width()-mLeftTextPad-mRightTextPad;
	int aHeight = Height()-mTopTextPad-mBottomTextPad;

	int x,y;
	switch(mHAlign)
	{	
		case HorzAlign_Left: x = 0; break;
		case HorzAlign_Center: x = (aWidth - aTextWidth)/2; break;
		case HorzAlign_Right: x = aWidth - aTextWidth; break;
		default: x = 0; break;
	}

	switch(mVAlign)
	{
		case VertAlign_Top: y = 0; break;
		case VertAlign_Center: y = (aHeight - aTextHeight)/2; break;
		case VertAlign_Bottom: y = aHeight - aTextHeight; break;
		default: y = 0; break;
	}

	x+=xoff + mLeftTextPad;
	y+=yoff + mTopTextPad;

	if(Disabled())
	{
		g.ApplyColorSchemeColor(StandardColor_3DHilight);
//		g.DrawString(mText,x+1,y+1);
		mText->Paint(g,x+1,y+1);
		g.ApplyColorSchemeColor(StandardColor_GrayText);
//		g.DrawString(mText,x,y);
		mText->Paint(g,x,y);
	}
	else
	{
		if(mTextColor<0)
			g.ApplyColorSchemeColor(mStandardTextColor);
		else
			g.SetColor(mTextColor);
//		g.DrawString(mText, x, y);
		mText->Paint(g,x,y);
	}

	if(HasFocus())
	{
		Container *aParent = GetParent();
		if(aParent!=NULL && aParent->GetHasUsedDialogKeys())
		{
			if(mTextColor<0)
				g.ApplyColorSchemeColor(mStandardTextColor);
			else
				g.SetColor(mTextColor);
			switch(mFocusStyle)
			{
				case FocusStyle_AroundWindow: g.DrawDottedRect(4,3,Width()-8,Height()-7); break;
				case FocusStyle_AroundText: g.DrawDottedRect(x-1,y-1,aTextWidth+2,aTextHeight+2); break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButton::Init()
{
	mLeftTextPad = mRightTextPad = 8;
	mTopTextPad = mBottomTextPad = 4;
	SetComponentFlags(ComponentFlag_WantFocus,true);
	SetButtonFlags(ButtonFlag_WantDefault,true);
	mUseFrame2 = false;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSButton::MSButton()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSButton::MSButton(const GUIString &theText)
{
	Init();
	mText->SetText(theText);
	SetDesiredSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSButton::~MSButton()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSButton::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);

	bool down = IsButtonDown();
	int offset = down?1:0;

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	if(!mIsTransparent)
		g.FillRect(0,0,Width(),Height());

	int x = 0, y = 0, width = Width(), height = Height();
	bool useFrame2 = mUseFrame2;
	if(IsDefaultButton())
	{
		g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
		g.DrawRect(0,0,Width(),Height());
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


	DrawButtonText(g,offset,offset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSTabButton::MSTabButton(const GUIString &theText, bool fillWhole, int theLowerAmount)
{

	mLowerAmount = theLowerAmount;
	if(!theText.empty())
	{
//		mText = theText;
		mText->SetText(theText);
//		SetSize(mFont->GetStringWidth(mText)+16,mFont->GetHeight()+8);
//		SetDesiredSize(16,8);
		SetDesiredSize();
	}

	mIsTransparent = !fillWhole;
	SetButtonFlags(ButtonFlag_ActivateDown | ButtonFlag_Radio, true);
	SetVertAlign(VertAlign_Center);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSTabButton::GetDesiredSize(int &width, int &height)
{
	MSButtonBase::GetDesiredSize(width,height);
	width+=16;
	height += 5 + mLowerAmount;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSTabButton::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);
	MSGraphicsUtil::DrawTab(g,0,0,Width(),Height());
	DrawButtonText(g,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSHeaderButton::MSHeaderButton(const GUIString &theText)
{
	SetComponentFlags(ComponentFlag_WantFocus,false);
	SetUseFrame2(true);
	IncrementHorzTextPad(2,0);
	SetHorzAlign(HorzAlign_Left);

	if(!theText.empty())
	{
		mText->SetText(theText);
		SetDesiredSize();
	}	
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSHeaderButton::Paint(Graphics &g)
{

	MSButton::Paint(g);

	ListSortType aSortType = MultiListHeader::GetSortTypeForButton(this);
	if(aSortType==ListSortType_None)
		return;

	g.ApplyColorSchemeColor(StandardColor_ButtonText);
	int x;
	if(mText->GetWidth()==0)
		x = (Width()-4)/2;
	else
		x = mText->GetWidth() + 10 + mLeftTextPad;
	
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
MSArrowButton::MSArrowButton(Direction theDirection)
{
	mDirection = theDirection;
	SetButtonFlags(ButtonFlag_RepeatFire, true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSArrowButton::~MSArrowButton()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSArrowButton::Paint(Graphics &g)
{
	Button::Paint(g);

	int i = 0;
	bool down = IsButtonDown();
	int offset = down?1:0;

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(0,0,Width(),Height());
	MSGraphicsUtil::DrawFrame2(g,0,0,Width(),Height(),down);
	if(down)
		MSGraphicsUtil::DrawArrow(g,mDirection,1,1,Width(),Height(),Disabled());
	else
		MSGraphicsUtil::DrawArrow(g,mDirection,0,0,Width(),Height(),Disabled());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSCloseButton::MSCloseButton()
{
	SetSize(16,14);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSCloseButton::Paint(Graphics &g)
{
	Button::Paint(g);

	bool down = IsButtonDown();
	int offset = down?1:0;

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(0,0,Width(),Height());
	MSGraphicsUtil::DrawFrame1(g,0,0,Width(),Height(),down);
	MSGraphicsUtil::DrawCloseX(g,offset,offset,Width(),Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSScrollThumb::MSScrollThumb()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSScrollThumb::~MSScrollThumb()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollThumb::Paint(Graphics &g)
{
	Component::Paint(g);
	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(0,0,Width(),Height());
	MSGraphicsUtil::DrawFrame2(g,0,0,Width(),Height(),false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSCheckbox::Init()
{
	mStandardTextColor = StandardColor_Text;
	mTextColor = -1;

	mHAlign = HorzAlign_Left;
	mFocusStyle = FocusStyle_AroundText;
	mLeftTextPad = mCheckboxWidth + 4;
	SetComponentFlags(ComponentFlag_WantFocus,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSCheckbox::MSCheckbox()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSCheckbox::MSCheckbox(const GUIString &theText)
{
	Init();
	mText->SetText(theText);
	SetDesiredSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSCheckbox::~MSCheckbox()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSCheckbox::DefaultCheckboxPainterFunc(Graphics &g, MSCheckbox *theCheckbox)
{
	MSGraphicsUtil::DrawCheck(g,0,0,mCheckboxWidth,mCheckboxHeight,theCheckbox->IsButtonDown(),theCheckbox->IsChecked());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSCheckbox::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	if(!mIsTransparent)
		g.FillRect(0,0,Width(),Height());

	int aWidth = mCheckboxWidth, aHeight = mCheckboxHeight;
	int aYTrans = 0;
	if(mVAlign==HorzAlign_Center) aYTrans = (Height() - aHeight - mTopTextPad - mBottomTextPad)/2;
	else if(mVAlign==VertAlign_Bottom) aYTrans = Height() - aHeight - mTopTextPad - mBottomTextPad;
	g.Translate(0,mTopTextPad+aYTrans);

	CheckboxPainter(g,this);

	g.Translate(0,-aYTrans-mTopTextPad);

	DrawButtonText(g, 0/*aWidth + 4*/, 0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSRadioButton::Init()
{
	SetButtonFlags(ButtonFlag_Radio, true);
	mHAlign = HorzAlign_Left;
	mFocusStyle = FocusStyle_AroundText;
	mLeftTextPad = mRadioWidth + 4;

//	SetComponentFlags(ComponentFlag_WantFocus,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSRadioButton::MSRadioButton()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSRadioButton::MSRadioButton(const GUIString &theText)
{
	Init();
	mText->SetText(theText);
	SetDesiredSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSRadioButton::~MSRadioButton()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSRadioButton::DefaultRadioPainterFunc(Graphics &g, MSRadioButton *theRadio)
{
	MSGraphicsUtil::DrawRadio(g,0,0,mRadioWidth,mRadioHeight,theRadio->IsButtonDown(),theRadio->IsChecked());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSRadioButton::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	if(!mIsTransparent)
		g.FillRect(0,0,Width(),Height());

	int aWidth = mRadioWidth, aHeight = mRadioHeight;
	int aYTrans = 0;
	if(mVAlign==HorzAlign_Center) aYTrans = (Height() - aHeight - mTopTextPad - mBottomTextPad)/2;
	else if(mVAlign==VertAlign_Bottom) aYTrans = Height() - aHeight - mTopTextPad - mBottomTextPad;
	g.Translate(0,mTopTextPad+aYTrans);

	RadioPainter(g,this);
	
	g.Translate(0,-aYTrans-mTopTextPad);

	DrawButtonText(g, 0/*aWidth + 4*/, 0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSLabel::MSLabel()
{
	mStandardTextColor = StandardColor_Text;
	mTextColor = -1;

	mTopTextPad = mRightTextPad = mLeftTextPad = mBottomTextPad = 0;
	mHAlign = HorzAlign_Left;
	SetButtonFlags(ButtonFlag_NoActivate,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSLabel::MSLabel(const GUIString &theText)
{
	mStandardTextColor = StandardColor_Text;
	mTextColor = -1;

	mTopTextPad = mRightTextPad = mLeftTextPad = mBottomTextPad = 0;
	SetButtonFlags(ButtonFlag_NoActivate,true);
	SetText(theText);
	SetDesiredSize();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSLabel::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	if(!mIsTransparent)
		g.FillRect(0,0,Width(),Height());

	DrawButtonText(g,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSScrollbar::MSScrollbar(bool vertical) : Scrollbar(vertical)
{
	mIsTransparent = false;

	if(vertical)
	{
		mUpArrow = ArrowButtonFactory(Direction_Up);
		mDownArrow = ArrowButtonFactory(Direction_Down);
	}
	else
	{
		mUpArrow = ArrowButtonFactory(Direction_Left);
		mDownArrow = ArrowButtonFactory(Direction_Right);
	}

	if(mUpArrow->Width()==0 || mUpArrow->Height()==0)
		mUpArrow->SetSizeAtLeast(15,15);

	if(mDownArrow->Width()==0 || mDownArrow->Height()==0)
		mDownArrow->SetSizeAtLeast(15,15);
	
	mThumb = ThumbFactory(IsVertical());
	mScrollRect = new Button;

	SetSize(mUpArrow->Width(),mUpArrow->Height());
//	AddControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollbar::SetTransparent(bool isTransparent)
{
	mIsTransparent = isTransparent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollbar::Paint(Graphics &g)
{
	if(mScrollRect->IsInvalid() && !mIsTransparent)
	{
		g.ApplyColorSchemeColor(StandardColor_Scrollbar);
		g.FillRect(mScrollRect->Left(),mScrollRect->Top(),mScrollRect->Width(),mScrollRect->Height());
	}
	Container::Paint(g);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSInputBox::Paint(Graphics &g)
{
	InputBox::Paint(g);
	MSGraphicsUtil::DrawFrame3(g,0,0,Width(),Height(),true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSInputBox::GetDesiredSize(int &width, int &height)
{
	InputBox::GetDesiredSize(width,height);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSComboBox::MSComboBox()
{
	mDrawFrame = true;

	mButton = ArrowButtonFactory(Direction_Down);
	if(mButton->Width()==0)
		mButton->SetWidth(16);

	mInputBox = new InputBox;

	mListArea = new ListArea;
	mListArea->SetFont(GetNamedFont("MSButton"));
	mListArea->SetListFlags(ListFlag_SelOver, true);

	mInputBox->SetFont(GetNamedFont("MSButton"));

	MSScrollContainerPtr aListBox = new MSScrollContainer(mListArea);
	aListBox->SetScrollbarConditions(ScrollbarCondition_Disable,ScrollbarCondition_Conditional);
	aListBox->SetDraw3DFrame(false);
	mListBox = aListBox;

	mButton->SetButtonFlags(ButtonFlag_RepeatFire,false);
//	AddControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSComboBox::Paint(Graphics& g)
{
	ComboBox::Paint(g);

	if(mDrawFrame)
	{
		MSGraphicsUtil::DrawFrame3(g,0,0,Width(),Height(),true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSComboBox::GetDesiredSize(int &width, int &height)
{
	width = 100;
	height = 22;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSComboBox::SizeChanged()
{
	ComboBox::SizeChanged();
	int aTopPad = 0, aBottomPad = 0, aRightPad = 0, aLeftPad = 0;
	if(mDrawFrame)
		aTopPad = aBottomPad = aRightPad = 2;

	int anInputBoxHeight = Height(); //mInputBox->Height();
	int aButtonHeight = anInputBoxHeight-aBottomPad-aTopPad;
	int aButtonWidth = mButton->Width();  
	if(!mButton->IsVisible())
		aButtonWidth = 0;
//	if(aButtonWidth>aButtonHeight)
//		aButtonWidth = aButtonHeight;
 
	int anInputBoxWidth = Width()-aButtonWidth-aLeftPad-aRightPad;

	if(mButton->IsVisible())
		mButton->SetPosSize(anInputBoxWidth+aLeftPad,aTopPad,aButtonWidth,aButtonHeight);
	
	mInputBox->SetPosSize(aLeftPad,0,anInputBoxWidth,anInputBoxHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSScrollContainer::MSScrollContainer(ScrollArea *theScrollArea)
{
	mVertScrollbar = new MSScrollbar(true);
	mHorzScrollbar = new MSScrollbar(false);
	mScrollArea = theScrollArea;
	mHeaderHeight = 0;
	mDraw3DFrame = true;
	mDrawFrame = true;
	mTransparent = false;

//	AddControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollContainer::SetDraw3DFrame(bool on)
{
	if(mDraw3DFrame==on)
		return;

	mDraw3DFrame = on;
	SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollContainer::SetDrawFrame(bool on)
{
	if(mDrawFrame==on)
		return;

	mDrawFrame = on;
	SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollContainer::SizeChanged()
{
	Container::SizeChanged();
	if(mScrollArea.get()==NULL)
		return;

	bool horzOn = mHorzScrollbar->IsVisible();
	bool vertOn = mVertScrollbar->IsVisible();

	int xoff = 0, yoff = 0;
	if(mDrawFrame && mDraw3DFrame)
		xoff = yoff = 2;

	int aScrollbarWidth = mVertScrollbar->Width();
	int aScrollbarHeight = mHorzScrollbar->Height();

	int aScrollWidth = Width() - xoff*2;
	if(vertOn)
		aScrollWidth -= aScrollbarWidth;

	int aScrollHeight = Height() - yoff*2;
	if(horzOn)
		aScrollHeight -= aScrollbarHeight;

	if(vertOn)
		mVertScrollbar->SetPosSize(Width()-aScrollbarWidth-xoff,yoff,aScrollbarWidth,aScrollHeight);

	if(horzOn)
		mHorzScrollbar->SetPosSize(xoff,Height()-aScrollbarHeight-yoff,aScrollWidth,aScrollbarHeight);

	mScrollArea->SetPosSize(xoff,mHeaderHeight+yoff,aScrollWidth,aScrollHeight-mHeaderHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSScrollContainer::Paint(Graphics &g)
{
	Container::Paint(g);

	if(mVertScrollbar->IsVisible() && mHorzScrollbar->IsVisible() && !mTransparent) // grey square in corner
	{
		g.ApplyColorSchemeColor(StandardColor_Scrollbar);
		g.FillRect(mVertScrollbar->Left(),mVertScrollbar->Bottom(),mVertScrollbar->Width(),mHorzScrollbar->Height());
	}

	if(mDrawFrame)
	{
		if(mDraw3DFrame)
			MSGraphicsUtil::DrawFrame1(g,0,0,Width(),Height(),true);
		else
		{
			g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
			g.DrawRect(0,0,Width(),Height());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSScrollButtonContainer::MSScrollButtonContainer(ScrollArea *theScrollArea)
{
	RectangleComponentPtr aBack = new RectangleComponent();
	aBack->SetSolidColor(StandardColor_3DFace);
	AddChildLayout(aBack,CLI_SameSize,this);
	AddChild(aBack);
	mLeftButton = new MSArrowButton(Direction_Left);
	mLeftButton->SetSize(15,15);
	mRightButton = new MSArrowButton(Direction_Right);
	mRightButton->SetSize(15,15);
	mUpButton = new MSArrowButton(Direction_Up);
	mUpButton->SetSize(15,15);
	mDownButton = new MSArrowButton(Direction_Down);
	mDownButton->SetSize(15,15);

	mScrollArea = theScrollArea;
//	AddControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSPopup::Paint(Graphics &g)
{
	Component::Paint(g);

	g.ApplyColorSchemeColor(StandardColor_MenuBack);
	g.FillRect(0,0,Width(),Height());
	
	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawLine(1,1,Width()-2,1);
	g.DrawLine(1,1,1,Height()-2);		
	
	g.ApplyColorSchemeColor(StandardColor_3DDarkShadow);
	g.DrawLine(Width()-1,0,Width()-1,Height()-1);
	g.DrawLine(1,Height()-1,Width()-1,Height()-1);

	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawLine(Width()-2,0,Width()-2,Height()-2);
	g.DrawLine(2,Height()-2,Width()-2,Height()-2);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSDlgBack::Paint(Graphics& g)
{
	Component::Paint(g);

	g.ApplyColorSchemeColor(StandardColor_3DFace);
	g.FillRect(0,0,Width(),Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSDialog::MSDialog()
{
	MSDlgBackPtr aBack = new MSDlgBack;
	AddChildLayout(aBack,CLI_SameSize,this);
	AddChild(aBack);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSeperator::MSSeperator(int theHeight)
{
	SetSize(Width(), theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSeperator::Paint(Graphics &g)
{
	Component::Paint(g);

	int aY = (Height() / 2) - 1;
	if(aY < 0)
		aY = 0;

	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawLine(0, aY, Width() - 1, aY);
	aY++;
	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawLine(0, aY, Width() - 1, aY);
}

	bool mIsUp;
	int mType;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MS3DFrame::MS3DFrame(int theType, bool up)
{
	mType = theType;
	mIsUp = up;
	SetComponentFlags(ComponentFlag_NoInput,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MS3DFrame::Paint(Graphics& g)
{
	Component::Paint(g);
	switch(mType)
	{
		case 1: MSGraphicsUtil::DrawFrame1(g,0,0,Width(),Height(),!mIsUp); break;
		case 2: MSGraphicsUtil::DrawFrame2(g,0,0,Width(),Height(),!mIsUp); break;
		case 3: MSGraphicsUtil::DrawFrame3(g,0,0,Width(),Height(),!mIsUp); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSEtchedFrame::MSEtchedFrame()
{
	SetComponentFlags(ComponentFlag_NoInput,true);
}
 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrame::Paint(Graphics &g)
{
	Component::Paint(g);

	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawRect(0,0,Width(),Height());
	g.DrawRect(1,1,Width()-1,Height()-1);
	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawRect(0, 0,Width()-1,Height()-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSEtchedFrameGroup::MSEtchedFrameGroup()
{
	mLabel = new Label;
	mLabel->SetHorzAlign(HorzAlign_Center);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSEtchedFrameGroup::MSEtchedFrameGroup(const GUIString &theTitle)
{
	mLabel = new Label;
	mLabel->SetHorzAlign(HorzAlign_Center);

	SetLabelText(theTitle);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrameGroup::GetDesiredSize(int &width, int &height)
{
	if(mComponent.get()!=NULL)
	{
		width = mComponent->Width() + 20;
		height = mComponent->Height() + mLabel->Height() + 20;
	}
	else
		width = height = 20;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrameGroup::AddedToParent()
{
	if(mLabel->GetParent()!=this)
	{
		AddChild(mLabel);
		AddChildLayout(mLabel,CLI_SameLeft,this,5);

		if(mComponent.get()!=NULL)
		{
			AddChild(mComponent);

			AddChildLayout(mComponent,CLI_SameLeft | CLI_SameWidth,this,10,0,-20);
			AddChildLayout(mComponent,CLI_Below,mLabel,0,10);
			AddChildLayout(mComponent,CLI_GrowToBottom,this,0,0,0,-10);
		}
	}

	Container::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrameGroup::SetLabelText(const GUIString &theText) 
{ 
	mLabel->SetText(theText); 
	mLabel->SetDesiredSize(10,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrameGroup::AddContainedComponent(Component *theComponent)
{
	mComponent = theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSEtchedFrameGroup::Paint(Graphics &g)
{
	Container::Paint(g);


	int dy = mLabel->Height()/2;
	g.Translate(0,dy);

	int right = Width()-1;
	int bottom = Height()-1-dy;
	int labelLeft = mLabel->Left();
	int labelRight = mLabel->Right();

	g.ApplyColorSchemeColor(StandardColor_3DHilight);
	g.DrawLine(0,1,labelLeft,1);
	g.DrawLine(labelRight,1,right,1);
	g.DrawLine(right,0,right,bottom);
	g.DrawLine(0,bottom,right,bottom);
	g.DrawLine(1,1,1,bottom);

	g.ApplyColorSchemeColor(StandardColor_3DShadow);
	g.DrawLine(0,0,labelLeft,0);
	g.DrawLine(labelRight,0,right-1,0);
	g.DrawLine(right-1,0,right-1,bottom-1);
	g.DrawLine(0,bottom-1,right-1,bottom-1);
	g.DrawLine(0,0,0,bottom-1);

	g.Translate(0,-dy);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSHeaderControl::MSHeaderControl(MultiListArea *theListArea) : MultiListHeader(theListArea)
{
	mTextLeftPad = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSHeaderControl::SetColumn(int theCol, const GUIString &theTitle)
{
	MSButtonPtr aButton = new MSHeaderButton(theTitle);
//	aButton->SetComponentFlags(ComponentFlag_WantFocus,false);
//	aButton->SetFocusStyle(MSButtonBase::FocusStyle_None);
//	aButton->SetHorzAlign(HorzAlign_Left);
//	aButton->SetHorzTextPad(mTextLeftPad+2,0);
//	aButton->SetUseFrame2(true);
	SetColumnItem(theCol, aButton);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSListCtrl::MSListCtrl(MultiListArea *theListArea) : MSScrollContainer(theListArea?theListArea:(new MultiListArea))
{
	mHeader = new MSHeaderControl(GetListArea());
	AddChild(mHeader);
	mHeaderHeight = 15;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSListCtrl::SizeChanged()
{
	MSScrollContainer::SizeChanged();

	mHeader->SetPosSize(mScrollArea->Left(),mScrollArea->Top()-mHeaderHeight,mScrollArea->Width(),mHeaderHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSToolTip::MSToolTip(const GUIString &theText)
{
//	mFont = GetFont(FontDescriptor("Tahoma",FontStyle_Plain,8));
	SetFont(GetNamedFont("MSButton"));
	SetHorzAlign(HorzAlign_Center);
	SetVertAlign(VertAlign_Center);
	SetText(theText);
	SetBackground(Background(0xffffdd,0x000000));
	SetComponentFlags(ComponentFlag_GrabBG,true);
//	SetSize(mFont->GetStringWidth(theText)+6,mFont->GetHeight()+4);
	SetDesiredSize(6,4);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSToolButton::MSToolButton(const GUIString &theText)
{
	mFocusStyle = FocusStyle_None;
	mHAlign = HorzAlign_Left;
	mLeftTextPad = 3;

	SetText(theText);
//	SetSize(mFont->GetStringWidth(theText)+10,mFont->GetHeight()+4);
	SetDesiredSize(10,4);
	SetButtonFlags(ButtonFlag_InvalidateOnMouseOver,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSToolButton::Paint(Graphics &g)
{
	MSButtonBase::Paint(g);

	bool down = IsButtonDown();
	bool over = !down && IsMouseOver();
	int offset = down?1:0;

	g.SetColor(0xD0D0D0);
	g.FillRect(0,0,Width(),Height());

	if(down || over)
	{
		g.SetColor(down?0x848484:0xffffff);
		g.DrawLine(0,0,Width()-1,0);
		g.DrawLine(0,0,0,Height()-1);
	
		g.SetColor(over?0x848484:0xffffff);
		g.DrawLine(0,Height()-1,Width()-1,Height()-1);
		g.DrawLine(Width()-1,0,Width()-1,Height()-1);
	}

	DrawButtonText(g,offset,offset);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSTabCtrl::MSTabCtrl()
{
	mIsTransparent = false;

	MS3DFramePtr aFrame = new MS3DFrame(1,true);
	MSDlgBackPtr aBack = new MSDlgBack;
	AddChildLayout(aBack,CLI_SameLeft | CLI_SameTop | CLI_SameSize,mContainerLayout);
	AddChildLayout(aFrame,CLI_SameLeft | CLI_SameTop | CLI_SameSize,mContainerLayout);
	AddChild(aBack);
	AddChild(aFrame);

	ButtonPtr leftArrow = new MSArrowButton(Direction_Left);
	ButtonPtr rightArrow = new MSArrowButton(Direction_Right);
	leftArrow->SetSize(15,15);
	rightArrow->SetSize(15,15);
	SetTabArrows(leftArrow,rightArrow);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSTabCtrl::SetTransparent(bool isTransparent)
{
	mIsTransparent = isTransparent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSTabCtrl::AddedToParent()
{
	TabCtrl::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSTabCtrl::AddStandardTab(const GUIString &theText, Container *theContainer)
{
	AddTab(new MSTabButton(theText),theContainer);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSMessageBox::MSMessageBox(const GUIString &theTitle, const GUIString &theMessage, 
	const GUIString &theButton1, const GUIString &theButton2, const GUIString &theButton3)
{
	// Create Controls
	mTitle = theTitle;
	GUIString aStr = theMessage;
	if(aStr.empty())
		aStr = theTitle;

	mLabel = new MSLabel(aStr);
	mLabel->SetHorzAlign(HorzAlign_Center);
	mLabel->SetAdjustHeightToFitText(true);
	mLabel->SetWrap(true);

	aStr = theButton1;
	if(aStr.empty())
		aStr = "Ok";
	mButton1 = new MSButton(aStr);
	mButton2 = new MSButton(theButton2);
	mButton3 = new MSButton(theButton3);
	bool doButton2 = !theButton2.empty();
	bool doButton3 = doButton2 && !theButton3.empty();
	SameSizeLayoutPtr aSameSize = (new SameSizeLayout(mButton1))->Add(mButton2)->Add(mButton3);
	aSameSize->Execute(this);

	int aWidth1 = mLabel->GetTextWidth();
	if(aWidth1<140)
		aWidth1 = 140;
	if(aWidth1 > 400)
		aWidth1 = 400;

	int aButtonWidth = mButton1->Width();
	if(aButtonWidth<80)
	{
		aButtonWidth = 80;
		mButton1->SetWidth(aButtonWidth);
	}
	int aWidth2 = aButtonWidth;
	if(doButton2)
		aWidth2 += 20 + aButtonWidth;
	if(doButton3)
		aWidth2 += 20 + aButtonWidth;
	if(aWidth2>aWidth1)
		aWidth1 = aWidth2;

	// End Dialog / Control Id's
	SetDefaultButton(mButton1);
	mButton1->SetButtonFlags(ButtonFlag_EndDialog,true);
	mButton2->SetButtonFlags(ButtonFlag_EndDialog,true);
	mButton3->SetButtonFlags(ButtonFlag_EndDialog,true);
	mButton1->SetControlId(1);
	mButton2->SetControlId(2);
	mButton3->SetControlId(3);
	if(doButton3)
		SetEndOnEscape(3);
	else if(doButton2)
		SetEndOnEscape(2);
	else
		SetEndOnEscape(1);

	// Layout
	SetWidth(aWidth1+20);
	mLabel->SetPos(10,10);
	AddChildLayout(mLabel,CLI_SameWidth,this,0,0,-20);
//	mButton1->SetWidth(80);
	CenterLayoutPtr aCenterLayout = new CenterLayout(this,mButton1,true,false);
	AddChildLayout(mButton1,CLI_Below,mLabel,0,20);
	if(doButton2)
	{
		AddChildLayout(mButton2,CLI_SameTop | CLI_Right | CLI_SameSize,mButton1,10);
		aCenterLayout->Add(mButton2);
	}
	if(doButton3)
	{
		AddChildLayout(mButton3,CLI_SameTop | CLI_Right | CLI_SameSize,mButton2,10);
		aCenterLayout->Add(mButton3);
	}
	AddChildLayout(aCenterLayout);
	SetHeight(mButton1->Bottom()+10);

	// Add Children
	AddChild(mLabel);
	AddChild(mButton1);
	if(doButton2)
		AddChild(mButton2);
	if(doButton3)
		AddChild(mButton3);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSMessageBox::Msg(Window *theParent, const GUIString &theTitle, const GUIString &theMessage, 
			   const GUIString &theButton1, const GUIString &theButton2, 
			   const GUIString &theButton3)
{
	MSMessageBoxPtr aMsgBox = new MSMessageBox(theTitle,theMessage,theButton1,theButton2,theButton3);
	return aMsgBox->DoDialog(theParent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WindowPtr MSMessageBox::PrepareWindow(Window *theParent)
{
	PlatformWindowPtr aWindow = new PlatformWindow;
	aWindow->SetParent(theParent);
	aWindow->SetTitle(mTitle);

	aWindow->SetCreateFlags(CreateWindow_NotMinimizable | CreateWindow_SizeSpecClientArea | CreateWindow_NotSizeable | CreateWindow_Centered);

	aWindow->Create(WONRectangle(0,0,Width(),Height()));

	aWindow->AddComponent(this);
	mButton1->RequestFocus();
	return aWindow.get();

}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MSMessageBox::DoDialog(Window *theParent)
{
	WindowPtr aWindow = PrepareWindow(theParent);
	int aResult = aWindow->DoDialog(this);
	aWindow->Close();

	return aResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSFileDialog::MSFileDialog(bool isSave)
{
	mIsSave = isSave;
	mAllowMultiple = false;
	mWarnIfExists = false;
	mGetDir = false;
	mNoChangeDir = false;

	mFileMustExist = !isSave;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32_NOT_XBOX
static MSFileDialog *gFileDialog = NULL;
static string gLastPath;
static bool gGotDir;
static UINT CALLBACK MyOFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	if(gFileDialog==NULL)
		return 0;

	if (uiMsg == WM_NOTIFY && gFileDialog->IsGetDir())
    {
     	LPOFNOTIFY lpofn = (LPOFNOTIFY)lParam;
        if(lpofn->hdr.code==CDN_FOLDERCHANGE)
		{
			string aNewPath;
			char aBuf[MAX_PATH+1];
			SendMessage(::GetParent(hdlg),CDM_GETFOLDERPATH,MAX_PATH,(LPARAM)aBuf);
			aNewPath = aBuf;
			if(aNewPath==gLastPath) // user hit ok button
			{
				PostMessage(::GetParent(hdlg),WM_COMMAND,MAKELONG( IDCANCEL, 0 ) ,NULL);
				gGotDir = true;
	       		return 1;
			}
			else
				gLastPath = aNewPath;
		}
    } 
	else if(uiMsg==CDN_INCLUDEITEM && gFileDialog->IsGetDir())
	{
		OFNOTIFYEX *anInfo = (OFNOTIFYEX*)lParam;

	}
	else if(uiMsg==WM_INITDIALOG)
	{

		if(!gFileDialog->GetOkTitle().empty())
		{
			string aStr = gFileDialog->GetOkTitle();
			SendMessage(::GetParent(hdlg),CDM_SETCONTROLTEXT,IDOK,(LPARAM)aStr.c_str()); 
		}
	}
	

	return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetGetDir(bool getDir)
{
	mGetDir = getDir;
#ifdef WIN32
	mFilterList.clear();
	AddFilter("*.zzz","Directories");
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetTitle(const GUIString &theTitle)
{
	mTitle = theTitle;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetOkTitle(const GUIString &theOkTitle)
{
	mOkTitle = theOkTitle;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetInitialFileName(const GUIString &theFileName)
{
	mInitialFileName = theFileName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetInitialDir(const GUIString &theDir)
{
	mInitialDir = theDir;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetAllowMultiple(bool allowMultiple)
{
	if(!mIsSave)
		mAllowMultiple = allowMultiple;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetFileMustExist(bool mustExist)
{
	mFileMustExist = mustExist;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetWarnIfExists(bool warnIfExists)
{
	if(mIsSave)
		mWarnIfExists = warnIfExists;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::SetNoChangeDir(bool noChange)
{
	mNoChangeDir = noChange;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSFileDialog::AddFilter(const GUIString &theFilter, const GUIString &theDescription)
{
	mFilterList.push_back(FilterPair(theFilter,theDescription));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const MSFileDialog::FileList& MSFileDialog::GetFileList()
{
	return mFileList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString MSFileDialog::GetFile()
{
	if(mFileList.empty())
		return "";
	else
		return mFileList.front();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSFileDialog::DoDialog(Window *theParent)
{
	mFileList.clear();
#ifdef WIN32_NOT_XBOX
	const int MAX_FILE_NAME = 8192;
	char aFileName[MAX_FILE_NAME];
	char aInitialDir[MAX_FILE_NAME];
	OPENFILENAME aStruct;
	memset(&aStruct,0,sizeof(aStruct));
	aStruct.lStructSize = sizeof(aStruct);
	aStruct.hwndOwner = ((MSWindow*)theParent)->GetHWND();

	std::string aTitle = mTitle;
	aStruct.lpstrTitle = aTitle.c_str();
	aStruct.lpfnHook = MyOFNHookProc;
	gFileDialog = this;
	gGotDir = false;
	gLastPath.erase();

	std::string aFilterStr;
	for(FilterList::iterator anItr = mFilterList.begin(); anItr!=mFilterList.end(); ++anItr)
	{
		const FilterPair &aPair = *anItr;
		aFilterStr.append(aPair.mDesc);
		aFilterStr.append(1,'\0');
		aFilterStr.append(aPair.mFilter);
		aFilterStr.append(1,'\0');
	}
	aFilterStr.append(2,'\0');
	aStruct.lpstrFilter = aFilterStr.c_str();

	aFileName[0] = '\0';
	std::string strFileName = mInitialFileName;
	strcpy(aFileName, strFileName.c_str());
	aStruct.lpstrFile = aFileName;
	aStruct.nMaxFile = MAX_FILE_NAME-1;

	if(mInitialDir.length())
	{
		std::string strInitialDir = mInitialDir;
		strcpy(aInitialDir, strInitialDir.c_str());
		aStruct.lpstrInitialDir = aInitialDir;
	}

	aStruct.Flags = OFN_EXPLORER;
	if(mAllowMultiple)
		aStruct.Flags |= OFN_ALLOWMULTISELECT;
	if(mWarnIfExists)
		aStruct.Flags |= OFN_OVERWRITEPROMPT;
	if(mFileMustExist)
		aStruct.Flags |= OFN_FILEMUSTEXIST;
	if(mNoChangeDir)
		aStruct.Flags |= OFN_NOCHANGEDIR;
	if(mGetDir || !mOkTitle.empty())
		aStruct.Flags |= OFN_ENABLEHOOK;

	BOOL aResult = FALSE;
	if(mIsSave)
		aResult = GetSaveFileName(&aStruct);
	else
		aResult = GetOpenFileName(&aStruct);

	gFileDialog = NULL;
	if(gGotDir) // user just selected a directory
		strcpy(aFileName,gLastPath.c_str());
	else if(!aResult)
		return false;
	else if(mGetDir) // strip filename off the folder
	{
		WONFile aFile(aFileName);
		string aFileNameStr = aFile.GetFileDirectory();
		strcpy(aFileName,aFileNameStr.c_str());
	}

	if(mAllowMultiple)
	{
		const char *aPtr = aFileName;
		GUIString aDirectory = aPtr;
		aPtr+=aDirectory.length()+1;
		while(true)
		{
			GUIString aStr = aPtr;
			if(aStr.empty())
			{
				if(mFileList.empty()) // only one file
					mFileList.push_back(aDirectory);
			
				break;
			}
			else
			{
				GUIString aFullPath = aDirectory;
				aFullPath.append('/');
				aFullPath.append(aStr);
				mFileList.push_back(aFullPath);
			}

			aPtr += aStr.length() + 1;
		}
	}
	else
		mFileList.push_back(aFileName);

	return true;
#else
	return false;
#endif	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static const int MSSpinner_ButtonWidth = 13;
void MSSpinner::Init()
{
	mLabel = new Label;
	mLabel->SetVertAlign(VertAlign_Center);
	AddChild(mLabel);

	mInputBox = new MSInputBox();
	mInputBox->SetTransform(InputBox::NumericFilter);

	mUpArrow = new MSArrowButton(Direction_Up);
	mDownArrow = new MSArrowButton(Direction_Down);
	mUpArrow->SetComponentFlags(ComponentFlag_WantFocus,true);
	mDownArrow->SetComponentFlags(ComponentFlag_WantFocus,true);
	mUpArrow->SetComponentFlags(ComponentFlag_WantTabFocus,false);
	mDownArrow->SetComponentFlags(ComponentFlag_WantTabFocus,false);

	AddChildLayout(mLabel, CLI_SameHeight, this);
	AddChildLayout(mUpArrow, CLI_SameTop | CLI_SameRight, this);
	AddChildLayout(mDownArrow, CLI_SameBottom | CLI_SameRight, this);
	AddChildLayout(mInputBox, CLI_Right | CLI_SameHeight, mLabel,2);
	AddChildLayout(mInputBox, CLI_GrowToLeft, mUpArrow);

	AddControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSpinner::MSSpinner()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSpinner::MSSpinner(const GUIString &theText, int minValue, int maxValue)
{
	Init();
	SetLabelText(theText);
	SetRange(minValue,maxValue,false);
	SetDesiredSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSpinner::SetLabelText(const GUIString &theText)
{
	mLabel->SetText(theText);
	mLabel->SetDesiredWidth();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSpinner::GetDesiredSize(int &width, int &height)
{
	int maxDigits = 0;
	int aNum = mMaxValue;
	do
	{
		maxDigits++;
		aNum/=10;
	} while(aNum>0);

	if(mMinValue<0)
		maxDigits++;

	mInputBox->GetDesiredSize(width,height);

	Font *aFont = mInputBox->GetFont();
	int anInputWidth = aFont->GetCharWidth('0')*maxDigits + 10;
	width = mLabel->Width() + 2 + anInputWidth + MSSpinner_ButtonWidth;

	// want height/2 is odd -> height/2 = 2n+1 -> height = 4n+2
	int rem = (height+2)%4; // make sure height is divisible by 4
	if(rem!=0)
		height += (4-rem);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSpinner::SizeChanged()
{
	int nWidth = Width();
	int nHeight = Height();
	int nBtnHt = nHeight/2;
	int nBtnWd = MSSpinner_ButtonWidth;
	mUpArrow->SetSize(nBtnWd, nBtnHt);
	mDownArrow->SetSize(nBtnWd, nBtnHt);

	Container::SizeChanged();
}

