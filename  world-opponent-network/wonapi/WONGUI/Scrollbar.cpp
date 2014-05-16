#include "Scrollbar.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scrollbar::Scrollbar(bool vertical)
{
	mPosition = 0;
	mPageSize = 0;
	mTotalSize = 0;
	mLineUpScrollSize =  mLineDownScrollSize = 10;
	mPageScrollSize = 50;
	mResizeThumb = true;
	mVertical = vertical;
	mMinThumbSize = 10;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scrollbar::~Scrollbar()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::CopyAttributes(Scrollbar *theCopyFrom)
{
	mLineUpScrollSize =  theCopyFrom->mLineUpScrollSize;
	mLineDownScrollSize =  theCopyFrom->mLineDownScrollSize;
	mResizeThumb = theCopyFrom->mResizeThumb;
	mMinThumbSize = theCopyFrom->mMinThumbSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::AddedToParent()
{
	if(mUpArrow->GetParent()!=this)
		AddControls();

	Container::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::AddControls()
{
	if(mScrollRect.get()==NULL)
		mScrollRect = new Button;

	mUpArrow->SetButtonFlags(ButtonFlag_RepeatFire,true);
	mDownArrow->SetButtonFlags(ButtonFlag_RepeatFire,true);	
	mScrollRect->SetButtonFlags(ButtonFlag_RepeatFire,true);

	AddChild(mScrollRect);
	AddChild(mThumb);
	AddChild(mUpArrow);
	AddChild(mDownArrow);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::S_SetPos(Component *theComponent, int x, int y)
{
	if(mVertical)
		theComponent->SetPos(x,y);
	else
		theComponent->SetPos(y,x);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::S_SetSize(Component *theComponent, int width, int height)
{
	if(mVertical)
		theComponent->SetSize(width,height);
	else
		theComponent->SetSize(height,width);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::S_SetPosSize(Component *theComponent, int x, int y, int width, int height)
{
	if(mVertical)
		theComponent->SetPosSize(x,y,width,height);
	else
		theComponent->SetPosSize(y,x,height,width);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::LayoutThumb(bool changePos)
{
	if(mPageSize >= mTotalSize)
	{
		mThumb->SetVisible(false);
		return;
	}

	mThumb->SetVisible(!Disabled());

	int aScrollHeight = S_Height(mScrollRect);
	int aThumbSize = S_Height(mThumb);
	if(mResizeThumb)
	{
		aThumbSize = aScrollHeight*mPageSize/mTotalSize;
		if(aThumbSize<mMinThumbSize)
			aThumbSize = mMinThumbSize;
	}

	int aThumbPos = S_Top(mThumb);
	if(changePos)
		aThumbPos = S_Top(mScrollRect)+(aScrollHeight-aThumbSize)*mPosition/(mTotalSize-mPageSize);

	if(aThumbPos<S_Top(mScrollRect))
	{
		aThumbPos = S_Top(mScrollRect);
		changePos = true;
	}
	if(aThumbPos+aThumbSize >= S_Bottom(mScrollRect))
	{
		aThumbPos = S_Bottom(mScrollRect)-aThumbSize;
		changePos = true;
	}

	int aThumbWidth = S_Width(mThumb);
	if(aThumbWidth==0)
		aThumbWidth = S_Width(this);

	if(changePos)
		S_SetPosSize(mThumb,S_Left(mThumb),aThumbPos,aThumbWidth,aThumbSize);
	else
		S_SetSize(mThumb,aThumbWidth,aThumbSize);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::SizeChanged()
{
	Container::SizeChanged();

	if(S_Height(mUpArrow)==0)
		S_SetSize(mUpArrow, S_Width(this),S_Width(this));
	if(S_Height(mDownArrow)==0)
		S_SetSize(mDownArrow,S_Width(this),S_Width(this));

//	S_SetPosSize(mUpArrow,0,0,S_Width(this),S_Width(this));
	S_SetPos(mDownArrow,0,S_Height(this)-S_Height(mDownArrow));
	S_SetPosSize(mScrollRect,0,S_Bottom(mUpArrow),S_Width(this),S_Height(this)-S_Height(mUpArrow)-S_Height(mDownArrow));
	LayoutThumb(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	if(theButton==MouseButton_Left && mMouseChild==mThumb)
	{
		mDragX = x-mThumb->Left();
		mDragY = y-mThumb->Top();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::MouseDrag(int x, int y)
{
	Container::MouseDrag(x,y);
	if(mMouseChild==mThumb)
	{
		int dx = (x-mThumb->Left())-mDragX;
		int dy = (y-mThumb->Top())-mDragY;

		int aNewY = mVertical? (mThumb->Top() + dy) : (mThumb->Left() + dx);

		if(aNewY<S_Top(mScrollRect))
			aNewY = S_Top(mScrollRect);
		if(aNewY+S_Height(mThumb) > S_Bottom(mScrollRect))
			aNewY = S_Bottom(mScrollRect)-S_Height(mThumb);

		int oldPosition = mPosition;
		int aHeightDiff = S_Height(mScrollRect) - S_Height(mThumb);
		if(aHeightDiff<=0)
			mPosition = 0;
		else
			mPosition = (mTotalSize-mPageSize)*(aNewY-S_Top(mScrollRect))/aHeightDiff;

		if(aNewY!=S_Top(mThumb))
		{
			S_SetPos(mThumb,S_Left(mThumb),aNewY);
			mScrollRect->Invalidate();
		}
		if(mPosition!=oldPosition)
		{
			FireEvent(ComponentEvent_Scroll);
		}

	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType!=ComponentEvent_ButtonPressed)
	{
		Container::HandleComponentEvent(theEvent);
		return;
	}

	int anOldPosition = mPosition;
	if(theEvent->mComponent==mUpArrow.get())
		mPosition-=mLineUpScrollSize;
	else if(theEvent->mComponent==mDownArrow.get())
		mPosition+=mLineDownScrollSize;
	else if(theEvent->mComponent==mScrollRect.get())
	{
		int aMouseX, aMouseY;
		GetMousePos(aMouseX, aMouseY);
		if(!mVertical)
			aMouseY = aMouseX;

		if(aMouseY < S_Top(mThumb))
			mPosition-=mPageScrollSize;
		else if(aMouseY >= S_Bottom(mThumb))
			mPosition+=mPageScrollSize;
	}
	else
	{
		Container::HandleComponentEvent(theEvent);
		return;
	}


	int aMaxPos = mTotalSize - mPageSize;
	if(mPosition>aMaxPos)
		mPosition = aMaxPos;
	if(mPosition<0)
		mPosition = 0;

	if(mPosition!=anOldPosition)
	{
		LayoutThumb(true);
		mScrollRect->Invalidate();
		FireEvent(ComponentEvent_Scroll);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scrollbar::Enable(bool isEnabled)
{
	if(isEnabled==!Disabled())
		return;

	Component::Enable(isEnabled);
	mUpArrow->Enable(isEnabled);
	mDownArrow->Enable(isEnabled);
	if(mTotalSize > mPageSize)
		mThumb->SetVisible(isEnabled);
}
