#include "SimpleComponent.h"
#include "Window.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RectangleComponent::RectangleComponent()
{
	mSolidColor = -1;
	mOutlineColor = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RectangleComponent::RectangleComponent(int theSolidColor, int theOutlineColor)
{
	mSolidColor = theSolidColor;
	mOutlineColor = theOutlineColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponent::SetColor(int theSolidColor, int theOutlineColor)
{
	mSolidColor = theSolidColor;
	mOutlineColor = theOutlineColor;
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponent::SetSolidColor(int theSolidColor)
{
	mSolidColor = theSolidColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponent::SetOutlineColor(int theOutlineColor)
{
	mOutlineColor = theOutlineColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponent::Paint(Graphics &g)
{
	Component::Paint(g);
	if(mSolidColor!=-1)
	{
		g.SetColor(mSolidColor);
		g.FillRect(0,0,Width(),Height());
	}

	if(mOutlineColor!=-1)
	{
		g.SetColor(mOutlineColor);
		g.DrawRect(0,0,Width(),Height());
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RectangleComponent3D::RectangleComponent3D(bool up, int theThickness)
{
	mUp = up;
	mThickness = theThickness;

	mLightColor = ColorScheme::GetColorRef(StandardColor_3DHilight);
	mDarkColor = ColorScheme::GetColorRef(StandardColor_3DShadow);
	mSolidColor = -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RectangleComponent3D::RectangleComponent3D(bool up, int theThickness, int theLightColor, int theDarkColor, int theSolidColor)
{
	mUp = up;
	mThickness = theThickness;

	mLightColor = theLightColor;
	mDarkColor = theDarkColor;
	mSolidColor = theSolidColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponent3D::Paint(Graphics &g)
{
	int width = Width()-1;
	int height = Height()-1;
	bool up = mUp;
	if(mSolidColor!=COLOR_INVALID)
	{
		g.SetColor(mSolidColor);
		g.FillRect(0,0,Width(),Height());
	}

	for(int i=0; i<mThickness; i++)
	{
		if(up)
			g.SetColor(mLightColor);
		else
			g.SetColor(mDarkColor);

		g.DrawLine(i,i,width-i,i);
		g.DrawLine(i,i,i,height-i);
		if(up)
			g.SetColor(mDarkColor);
		else
			g.SetColor(mLightColor);

		g.DrawLine(width-i,i,width-i,height);
		g.DrawLine(i,height-i,width,height-i);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScreenContainer::ScreenContainer()
{
	mCurScreen = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScreenContainer::AddScreen(Component *theScreen)
{
	theScreen->SetVisible(false);
	AddChildLayout(theScreen,CLI_SameSize,this);
	AddChild(theScreen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScreenContainer::ShowScreen(Component *theScreen, bool takeFocus)
{
	if(theScreen==mCurScreen)
		return;

	if(mCurScreen!=NULL)
		mCurScreen->SetVisible(false);

	theScreen->SetVisible(true);
	mCurScreen = theScreen;
	if(mCurScreen!=NULL && takeFocus)
		mCurScreen->RequestFocus();

	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BorderComponent::BorderComponent()
{
//	SetComponentFlags(ComponentFlag_NoInput,true);
	mOnlyChildInput = true;
	mControlsAdded = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BorderComponent::SetComponent(BorderPos thePos, Component *theComponent, int theWidth, int theHeight)
{
	mComponents[thePos] = theComponent;
	if(theWidth!=-1 && theHeight!=-1)
		theComponent->SetSize(theWidth,theHeight);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BorderComponent::AddControls()
{
	for(int i=0; i<8; i++)
	{
		if(mComponents[i].get()==NULL)
			mComponents[i] = new Component;
			
		AddChild(mComponents[i]);
	}

	mLeftTop = mComponents[Pos_LeftTop];
	mRightTop = mComponents[Pos_RightTop];
	mLeftBottom = mComponents[Pos_LeftBottom];
	mRightBottom = mComponents[Pos_RightBottom];
	
	mLeft = mComponents[Pos_Left];
	mTop = mComponents[Pos_Top];
	mRight = mComponents[Pos_Right];
	mBottom = mComponents[Pos_Bottom];

	mControlsAdded = true;
	SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BorderComponent::SizeChanged()
{
	Container::SizeChanged();

	if(!mControlsAdded)
		return;

	mLeftTop->SetPos(0,0); 
	mRightTop->SetPos(Width()-mRightTop->Width(),0);
	mLeftBottom->SetPos(0,Height()-mLeftBottom->Height()); 
	mRightBottom->SetPos(Width()-mRightBottom->Width(),Height()-mRightBottom->Height());
	
	mLeft->SetPosSize(0,mLeftTop->Bottom(), mLeft->Width(), mLeftBottom->Top() - mLeftTop->Bottom());
	mRight->SetPosSize(Width()-mRight->Width(), mRightTop->Bottom(), mRight->Width(), mRightBottom->Top() - mRightTop->Bottom());
	mTop->SetPosSize(mLeftTop->Right(), 0, mRightTop->Left() - mLeftTop->Right(), mTop->Height());
	mBottom->SetPosSize(mLeftBottom->Right(), Height()-mBottom->Height(), mRightBottom->Left() - mLeftBottom->Right(), mBottom->Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageComponent::ImageComponent(Image *theImage, bool tile)
{
	SetImage(theImage, tile);
	mStretch = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageComponent::SetImage(Image *theImage)
{
	mImage = theImage;
	if(theImage!=NULL)
		SetSize(theImage->GetWidth(), theImage->GetHeight());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageComponent::SetImage(Image *theImage, bool tile)
{
	mImage = theImage;
	mTile = tile;
	if(theImage!=NULL)
		SetSize(theImage->GetWidth(), theImage->GetHeight());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageComponent::GetDesiredSize(int &width, int &height)
{
	if(mImage.get()!=NULL)
	{
		width = mImage->GetWidth();
		height = mImage->GetHeight();
	}
	else
		width = height = 10;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageComponent::Paint(Graphics &g)
{
	Component::Paint(g);
	if(mImage.get()==NULL)
		return;

	if(mStretch)
	{
		if(mImage->GetWidth()!=Width() || mImage->GetHeight()!=Height())
			mImage->SetSize(Width(),Height());

		g.DrawImage(mImage,0,0);
		return;
	}

	if(!mTile)
	{
		g.DrawImage(mImage,0,0);
		return;
	}

	g.PushClipRect(0,0,Width(),Height());
	for(int x=0; x<Width(); x+=mImage->GetWidth())
	{
		for(int y=0; y<Height(); y+=mImage->GetHeight())
		{
			g.DrawImage(mImage,x,y);
		}
	}
	g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimationComponent::AnimationComponent(Animation* theAnimation, bool startNow) : mAnimation(theAnimation)
{
	mDelayAcc = 0;
	mStarted = false;
	SetAnimation(theAnimation);
	if(theAnimation!=NULL)
	{
		SetSize(theAnimation->GetWidth(),theAnimation->GetHeight());
		if(startNow)
			Start();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponent::SetAnimation(Animation *theAnimation)
{
	mAnimation=theAnimation;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponent::Start(bool restart)
{
	if(restart)
	{
		mAnimation->Rewind();
		mDelayAcc = 0;
	}

	mStarted = true;
	RequestTimer(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponent::Stop()
{
	mStarted = false;
	RequestTimer(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponent::AddedToParent()
{
	Component::AddedToParent();
	if(mStarted)
		RequestTimer(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponent::SizeChanged()
{
	Component::SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponent::Paint(Graphics &g)
{
	Component::Paint(g);
	mAnimation->Draw(g,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AnimationComponent::TimerEvent(int theDelta)
{
	Component::TimerEvent(theDelta);

	mDelayAcc+=theDelta;
	if(mDelayAcc > mAnimation->GetFrameDelayTime())
	{
		if(!mAnimation->NextFrame())
			return false;

		mDelayAcc = 0;
		Invalidate();
	}

	return true;
}

