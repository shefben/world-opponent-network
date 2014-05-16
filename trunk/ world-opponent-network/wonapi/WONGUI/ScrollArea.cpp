#include "ScrollArea.h"

using namespace WONAPI;
		

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScrollArea::ScrollArea()
{
	mViewport = new ScrollAreaViewport(this);
	AddChild(mViewport);

	mVertOffset = mHorzOffset = mTotalHeight = mTotalWidth = 0;
	mAVertOffset = mAHorzOffset = mATotalHeight = mATotalWidth = 0;
	mExtraHeight = 0;

	mUpScrollAmount =  mDownScrollAmount = 10;
	mLeftScrollAmount = mRightScrollAmount = 10;

	mOldScrollAreaWidth = 	mOldScrollAreaHeight = 0;
	mLeftPad = mRightPad = mTopPad = mBottomPad = 0;

	mScroller = NULL;
	mUserHasScrolled = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::InvalidateRect(WONRectangle &theRect)
{
/*	if(!IsInvalid())
	{
		mInvalidState = InvalidState_PartiallyInvalid;
		if(mParent!=NULL)
			mParent->InvalidateUp(this);
	}
	if(!IsFullyInvalid())
		mInvalidRect.DoUnion(theRect);*/

	Container::InvalidateRect(theRect);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::Invalidate()
{
	Container::Invalidate();
//	mInvalidRect.width = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void  ScrollArea::AddedToParent()
{
	Container::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetDelegatePos()
{
	mViewport->SetPosSize(mScrollArea.Left()-mHorzOffset, mScrollArea.Top()-mVertOffset, mTotalWidth, mTotalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetScrollDimension(int theWidth, int theHeight)
{
	mTotalWidth = theWidth;
	mTotalHeight = theHeight;
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetScroller(Scroller *theScroller)
{
	mScroller = theScroller;	
	if(theScroller!=NULL)
		theScroller->SetScrollArea(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetScrollbar(Scrollbar *theScrollbar)
{
	ScrollbarScroller *aScroller = dynamic_cast<ScrollbarScroller*>(mScroller);
	if(aScroller==NULL)
	{
		aScroller = new ScrollbarScroller;
		SetScroller(aScroller);
	}

	aScroller->SetScrollbar(theScrollbar);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetScrollButton(Button *theButton, Direction theDirection)
{
	ButtonScroller *aScroller = dynamic_cast<ButtonScroller*>(mScroller);
	if(aScroller==NULL)
	{
		aScroller = new ButtonScroller;
		SetScroller(aScroller);
	}

	aScroller->SetButton(theButton, theDirection);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::AdjustPosition(bool becauseOfScrollbar)
{
	if(becauseOfScrollbar)
		mUserHasScrolled = true;

//	if(mAVertOffset==mVertOffset && mAHorzOffset==mHorzOffset && mATotalHeight==mTotalHeight && mATotalWidth==mTotalWidth)
//		return;
	if(mVertOffset+mScrollArea.height > mTotalHeight+mExtraHeight)
		mVertOffset = mTotalHeight+mExtraHeight-mScrollArea.height;
	if(mVertOffset<0)
		mVertOffset = 0;

	if(mHorzOffset+mScrollArea.width > mTotalWidth)
		mHorzOffset = mTotalWidth-mScrollArea.width;
	if(mHorzOffset<0)
		mHorzOffset = 0;

	if(mScroller!=NULL)
		mScroller->NotifyScrollChange(becauseOfScrollbar);

/*	bool needEvent = false;
	if(mATotalHeight!=mTotalHeight || mATotalWidth!=mTotalWidth)
		needEvent = true;
	else if(mOldScrollAreaWidth!=mScrollArea.Width() || mOldScrollAreaHeight!=mScrollArea.Height())
		needEvent = true;*/

	mAVertOffset = mVertOffset;
	mAHorzOffset = mHorzOffset;
	mATotalHeight = mTotalHeight;
	mATotalWidth = mTotalWidth;
	mOldScrollAreaWidth = mScrollArea.width;
	mOldScrollAreaHeight = mScrollArea.height;

	SetDelegatePos();
	Invalidate();

//	if(needEvent)
//		FireEvent(ComponentEvent_ScrollAreaChange,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetVertOffset(int theOffset)
{
	mVertOffset = theOffset;
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetHorzOffset(int theOffset)
{
	mHorzOffset = theOffset;
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetOffsets(int theHorzOffset, int theVertOffset)
{
	mHorzOffset = theHorzOffset;
	mVertOffset = theVertOffset;
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::HandleComponentEvent(ComponentEvent *theEvent)
{
	Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollArea::OnBottom()
{
	return mVertOffset>0 && mVertOffset+mScrollArea.height >= mTotalHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SizeChanged()
{
	Container::SizeChanged();
	bool onBottom = OnBottom();

	mScrollArea.x = mLeftPad;
	mScrollArea.y = mTopPad;
	mScrollArea.width = Width()-mLeftPad-mRightPad;
	mScrollArea.height = Height()-mTopPad-mBottomPad;
	if(mScrollArea.width<0)
		mScrollArea.width=0;
	if(mScrollArea.height<0)
		mScrollArea.height=0;

	if(onBottom)
		mVertOffset = mTotalHeight+mExtraHeight-mScrollArea.height;

	ScrollSizeChanged();

	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::PosChanged()
{
//	SetDelegatePos();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetBorderPadding(int theLeftPad, int theTopPad, int theRightPad, int theBottomPad)
{
	if(mLeftPad==theLeftPad && mRightPad==theRightPad && mTopPad==theTopPad && mBottomPad==theBottomPad)
		return;

	mLeftPad = theLeftPad;
	mTopPad = theTopPad;
	mRightPad = theRightPad;
	mBottomPad = theBottomPad;
	SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::Paint(Graphics &g)
{
	mBackground.Paint(g,0,0,Width(),Height(),mHorzOffset,mVertOffset);
	g.PushClipRect(mScrollArea.x,mScrollArea.y,mScrollArea.width,mScrollArea.height);
//	if(mInvalidRect.Width()>0)
//		g.PushClipRect(mInvalidRect.x,mInvalidRect.y,mInvalidRect.width,mInvalidRect.height);

	Container::Paint(g);

	int dx = 0, dy = 0;
	ScrollTranslate(dx,dy);
	g.Translate(-dx,-dy);

	ScrollPaint(g);

	g.Translate(dx,dy);
/*	if(mInvalidRect.Width()>0)
	{
		g.PopClipRect();
		mInvalidRect.width = 0;
	}*/
	g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::ScrollPaint(Graphics &g)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::ScrollSizeChanged()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::ScrollTranslate(int &x, int &y)
{	
	x += mHorzOffset - mScrollArea.x;
	y += mVertOffset - mScrollArea.y;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::ScrollUntranslate(int &x, int &y)
{
	x += mScrollArea.x - mHorzOffset;
	y += mScrollArea.y - mVertOffset;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollArea::MouseWheel(int theAmount)
{
	if(Container::MouseWheel(theAmount))
		return true;

	mUserHasScrolled = true;
	mVertOffset -= theAmount;
	AdjustPosition();
//	GetWindow()->ReMouseMove();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::SetSizeEqualToTotal(int theMinWidth, int theMinHeight)
{
	int aNewWidth = mTotalWidth;
	int aNewHeight = mTotalHeight;

	if(theMinWidth > aNewWidth)
		aNewWidth = theMinWidth;

	if(theMinHeight > aNewHeight)
		aNewHeight = theMinHeight;
		
	SetSize(aNewWidth + mLeftPad + mRightPad, aNewHeight + mTopPad + mBottomPad);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollArea::GetDesiredSize(int &width, int &height)
{
	if(mTotalWidth < 300)
		width = mTotalWidth;
	else
		width = 300;

	if(mTotalHeight < 300)
		height = mTotalHeight;
	else
		height = 300;
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollArea::IsRectVisible(const WONRectangle &theRect)
{
	WONRectangle aRect = mScrollArea;
	aRect.x = mHorzOffset;
	aRect.y = mVertOffset;

	return theRect.Intersects(aRect);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Scroller::Scroller() : mScrollArea(NULL) 
{ 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::SetScrollArea(ScrollArea *theScrollArea)
{
	mScrollArea = theScrollArea;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::SetOffsets(int theHorzOffset, int theVertOffset)
{
	if(GetHorzOffset()==theHorzOffset && GetVertOffset()==theVertOffset)
		return;

	mScrollArea->mHorzOffset = theHorzOffset;
	mScrollArea->mVertOffset = theVertOffset;
	mScrollArea->AdjustPosition(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::SetVertOffset(int theVertOffset)
{
	SetOffsets(GetHorzOffset(),theVertOffset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::SetHorzOffset(int theHorzOffset)
{
	SetOffsets(theHorzOffset,GetVertOffset());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::IncrementOffsets(int theHorzInc, int theVertInc)
{
	SetOffsets(GetHorzOffset()+theHorzInc, GetVertOffset()+theVertInc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::IncrementHorzOffset(int theInc)
{
	IncrementOffsets(theInc,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::IncrementVertOffset(int theInc)
{
	IncrementOffsets(0,theInc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Scroller::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	if(theButton==MouseButton_Left)
	{
		if(mFocusChild==NULL)
			mScrollArea->RequestFocus();
		else
			RequestFocus();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Scroller::MouseWheel(int theAmount)
{
	if(Container::MouseWheel(theAmount))
		return true;
	else if(mScrollArea.get()!=NULL)
		return mScrollArea->MouseWheel(theAmount);
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScrollbarScroller::ScrollbarScroller()
{
	mVertCondition = mHorzCondition = ScrollbarCondition_Enable;
	mSettingScrollbarConditions = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollbarScroller::SetScrollbar(Scrollbar *theScrollbar)
{
	if(theScrollbar->IsVertical())
		mVertScrollbar = theScrollbar;
	else
		mHorzScrollbar = theScrollbar;

	theScrollbar->SetListener(new ComponentListenerComp(this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollbarScroller::AddedToParent()
{
	if(mVertScrollbar->GetParent()!=this)
		AddControls();

	Scroller::AddedToParent();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollbarScroller::AddControls()
{
	mScrollArea->SetScroller(this);
	AddChild(mScrollArea);
	AddChild(mVertScrollbar);
	AddChild(mHorzScrollbar);
	if(mScrollArea->GetTotalWidth()>0 || mScrollArea->GetTotalHeight()>0)
		NotifyScrollChange(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollbarScroller::EnableScrollbar(Scrollbar *theScrollbar, ScrollbarCondition theCondition)
{
	if(theScrollbar==NULL || mScrollArea.get()==NULL)
		return false;

	bool visible = theScrollbar->IsVisible();
	
	// Prevent situations where conditional scrollbars are enabled and disabled recursively since
	// it turns out that it's possible for the appearance of a scrollbar to change the wordwrapping
	// such that the scrollbar is no longer necessary.
	if(mSettingScrollbarConditions && theCondition==ScrollbarCondition_Conditional && visible)  
		return false;

	switch(theCondition)
	{
		case ScrollbarCondition_Disable: theScrollbar->SetVisible(false); break;
		case ScrollbarCondition_Enable: theScrollbar->SetVisible(true); break;
		case ScrollbarCondition_Conditional: theScrollbar->SetVisible(mScrollArea->NeedScrollbar(theScrollbar->IsVertical()));
	}

	return visible!=theScrollbar->IsVisible(); // did anything change?
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollbarScroller::SetScrollbarConditions(ScrollbarCondition theHorzCondition, ScrollbarCondition theVertCondition)
{
	bool needChange = false;
	mHorzCondition = theHorzCondition;
	mVertCondition = theVertCondition;

	needChange = EnableScrollbar(mHorzScrollbar, mHorzCondition) || needChange;
	needChange = EnableScrollbar(mVertScrollbar, mVertCondition) || needChange;

	mSettingScrollbarConditions = true;
	if(needChange)
	{
		Invalidate();
		SizeChanged();
	}
	mSettingScrollbarConditions = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollbarScroller::ScrollbarEnabled(bool vertical)
{
	if(vertical)
	{
		if(mVertScrollbar.get()!=NULL)
			return mVertScrollbar->IsVisible();
	}
	else
	{
		if(mHorzScrollbar.get()!=NULL)
			return mHorzScrollbar->IsVisible();
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////	
void ScrollbarScroller::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_Scroll)
	{
		Scrollbar *aScrollbar = (Scrollbar*)theEvent->mComponent;
		int aPosition = aScrollbar->GetPosition();
		if(aScrollbar->IsVertical())
		{
			if(aScrollbar==mVertScrollbar)
			{
				SetVertOffset(aPosition);
				return;
			}
		}
		else 
		{
			if(aScrollbar==mHorzScrollbar)
			{
				SetHorzOffset(aPosition);
				return;
			}
		}
	}

	Scroller::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////	
void ScrollbarScroller::NotifyScrollChange(bool becauseOfScroller)
{
	if(mVertScrollbar.get()!=NULL)
	{
		mVertScrollbar->SetTotalSize(GetTotalHeight()); //mTotalHeight + mExtraHeight
		mVertScrollbar->SetPageSize (GetScrollAreaHeight());
		mVertScrollbar->SetPageScrollSize(GetScrollAreaHeight());
		mVertScrollbar->SetLineUpScrollSize(GetUpScrollAmount());
		mVertScrollbar->SetLineDownScrollSize(GetDownScrollAmount());

		if(!becauseOfScroller)
			mVertScrollbar->SetPosition(GetVertOffset());

		mVertScrollbar->LayoutThumb(!becauseOfScroller);
		mVertScrollbar->Enable(GetTotalHeight() > GetScrollAreaHeight());
		mVertScrollbar->Invalidate();
	}

	if(mHorzScrollbar.get()!=NULL)
	{
		mHorzScrollbar->SetTotalSize(GetTotalWidth());
		mHorzScrollbar->SetPageSize(GetScrollAreaWidth());
		mHorzScrollbar->SetPageScrollSize(GetScrollAreaWidth());

		if(!becauseOfScroller)
			mHorzScrollbar->SetPosition(GetHorzOffset());

		mHorzScrollbar->LayoutThumb(!becauseOfScroller);
		mHorzScrollbar->Enable(GetTotalWidth() > GetScrollAreaWidth());
		mHorzScrollbar->Invalidate();
	}

	SetScrollbarConditions(mHorzCondition,mVertCondition);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ButtonScroller::ButtonScroller()
{
	mSettingScrollButtonConditions = false;
	mVertCondition = mHorzCondition = ScrollButtonCondition_Enable;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::AddedToParent()
{

	if(mScrollArea.get()!=NULL && mScrollArea->GetParent()!=this)
		AddControls();

	Scroller::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::AddControls()
{
	mScrollArea->SetScroller(this);
	ButtonPtr* buttons[] = {&mLeftButton, &mRightButton, &mUpButton, &mDownButton};
	for(int i=0; i<4; i++)
	{
		ButtonPtr &aButton = *buttons[i];
		if(aButton.get()==NULL)
		{
			aButton = new Button;
			aButton->SetComponentFlags(ComponentFlag_NoInput,true);
		}
	}

	AddChild(mScrollArea);
	AddChild(mLeftButton);
	AddChild(mRightButton);
	AddChild(mUpButton);
	AddChild(mDownButton);

	if(mScrollArea->GetTotalWidth()>0 || mScrollArea->GetTotalHeight()>0)
		NotifyScrollChange(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ButtonScroller::EnableScrollButtons(Button *theLeft, Button *theRight, ScrollButtonCondition theCondition)
{	
	if(theLeft==NULL || theRight==NULL)
		return false;

	bool showLeft = theLeft->IsVisible(), showRight = theRight->IsVisible();
	switch(theCondition)
	{
		case ScrollButtonCondition_Disable: showLeft = false; showRight = false; break;
		case ScrollButtonCondition_Enable: showLeft = true; showRight = true; break;
		case ScrollButtonCondition_Conditional: showLeft = showRight = !(theLeft->Disabled() && theRight->Disabled()); break;
		case ScrollButtonCondition_PartialConditional: showLeft = !theLeft->Disabled(); showRight = !theRight->Disabled(); break;
	}

	// Prevent situations where conditional scrollbars are enabled and disabled recursively since
	// it turns out that it's possible for the appearance of a scrollbar to change the wordwrapping
	// such that the scrollbar is no longer necessary.
	if(mSettingScrollButtonConditions)
	{
		if(!showLeft && theLeft->IsVisible())
			return false;
		if(!showRight && theRight->IsVisible())
			return false;
	}

	// Did anything change?
	bool changed = showLeft!=theLeft->IsVisible() || showRight!=theRight->IsVisible();
	theLeft->SetVisible(showLeft);
	theRight->SetVisible(showRight);
	return changed;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::SetScrollButtonConditions(ScrollButtonCondition theHorzCondition, ScrollButtonCondition theVertCondition)
{
	bool needChange = false;
	mHorzCondition = theHorzCondition;
	mVertCondition = theVertCondition;

	needChange = EnableScrollButtons(mLeftButton,mRightButton, mHorzCondition) || needChange;
	needChange = EnableScrollButtons(mUpButton, mDownButton, mVertCondition) || needChange;

	mSettingScrollButtonConditions = true;
	if(needChange)
	{
		Invalidate();
		SizeChanged();
	}
	mSettingScrollButtonConditions = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::SizeChanged()
{
	Scroller::SizeChanged();
/*	if(mLeftButton.get()==NULL || mRightButton.get()==NULL || mUpButton.get()==NULL || mDownButton.get()==NULL)
		return;

	int l = 0, r = 0, u = 0, d = 0;
	if(mLeftButton->IsVisible())
	{
		mLeftButton->SetPos(0,(Height()-mLeftButton->Height())/2);
		l = mLeftButton->Width();
	}

	if(mRightButton->IsVisible())
	{
		mRightButton->SetPos(Width()-mRightButton->Width(),(Height()-mRightButton->Height())/2);
		r = mRightButton->Width();
	}

	if(mUpButton->IsVisible())
	{
		mUpButton->SetPos((Width()-mUpButton->Width())/2,0);
		u = mUpButton->Height();
	}

	if(mDownButton->IsVisible())
	{
		mDownButton->SetPos((Width()-mDownButton->Width())/2,(Height()-mDownButton->Height()));
		d = mDownButton->Height();
	}

	mScrollArea->SetPosSize(l,u,Width()-l-r,Height()-u-d);*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::SetButton(Button *theButton, Direction theDirection)
{
	switch(theDirection)
	{
		case Direction_Left: mLeftButton = theButton; break;
		case Direction_Right: mRightButton = theButton; break;
		case Direction_Up:	mUpButton = theButton; break;
		case Direction_Down: mDownButton = theButton; break;
		default:
			return;
	}

	theButton->SetListener(new ComponentListenerComp(this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::NotifyScrollChange(bool becauseOfScroller)
{
	if(mLeftButton.get()!=NULL)
		mLeftButton->Enable(GetHorzOffset()>0);

	if(mUpButton.get()!=NULL)
		mUpButton->Enable(GetVertOffset()>0);

	if(mRightButton.get()!=NULL)
		mRightButton->Enable(GetHorzOffset() + GetScrollAreaWidth() < GetTotalWidth());

	if(mDownButton.get()!=NULL)
		mDownButton->Enable(GetVertOffset() + GetScrollAreaHeight() < GetTotalHeight());

	SetScrollButtonConditions(mHorzCondition, mVertCondition);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ButtonScroller::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		if(theEvent->mComponent==mLeftButton)
			IncrementHorzOffset(-GetLeftScrollAmount());
		else if(theEvent->mComponent==mRightButton)
			IncrementHorzOffset(GetRightScrollAmount());
		else if(theEvent->mComponent==mUpButton)
			IncrementVertOffset(-GetUpScrollAmount());
		else if(theEvent->mComponent==mDownButton)
			IncrementVertOffset(GetDownScrollAmount());
		else
			Scroller::HandleComponentEvent(theEvent);
	}
	else
		Scroller::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::InvalidateIfRectVisible(WONRectangle &theRect)
{
	if(GetParent()==mScrollArea)
	{
		if(mScrollArea->IsRectVisible(theRect))
//			mScrollArea->InvalidateRect(theRect);
			mScrollArea->Invalidate();
	}
	else
	{
		Container *aParent = GetParent();
		if(aParent!=NULL)
		{
			theRect.x+=Left();
			theRect.y+=Top();
			((ScrollAreaViewport*)aParent)->InvalidateIfRectVisible(theRect);
		}
	}


}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::Reset()
{
	mMouseChild = mFocusChild = NULL;
	mChildTimerSet.clear();
	mChildList.clear();

	WindowManager *wm = WindowManager::GetDefaultWindowManager();
	if(wm!=NULL)
		wm->CheckEndPopup(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollAreaViewport::InvalidateUp(Component *theChild)
{
	WONRectangle aRect = theChild->GetBounds();
	InvalidateIfRectVisible(aRect);

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::ChildRequestFocus(Component *theChild)
{
	Container::ChildRequestFocus(theChild);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::ChildChanged(Component *theChild)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::ChildRequestTimer(Component *theChild)
{
	Container::ChildRequestTimer(theChild);
//	if(theChild->WantTimer())
//		mChildTimerSet.insert(theChild);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollAreaViewport::TimerEvent(int theDelta)
{
	return Container::TimerEvent(theDelta);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::SetCursor(Cursor *theCursor)
{
	mScrollArea->SetCursor(theCursor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaViewport::Paint(Graphics &g)
{
	Container::Paint(g);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ComponentScrollArea::ComponentScrollArea(Component *theComponent)
{
	mComponent = theComponent;
	GetViewport()->AddChild(mComponent);
	mVertCenter = true;
	mHorzCenter = true;
	UpdateComponentSize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentScrollArea::SizeChanged()
{
	ScrollArea::SizeChanged();
	PositionComponent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentScrollArea::PositionComponent()
{
	int x = 0, y = 0;
	if(mHorzCenter)
	{
		if(mComponent->Width() < mScrollArea.Width())
			x = (mScrollArea.Width() - mComponent->Width())/2;
	}

	if(mVertCenter)
	{
		if(mComponent->Height() < mScrollArea.Height())
			y = (mScrollArea.Height() - mComponent->Height())/2;
	}

	mComponent->SetPos(x,y);
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentScrollArea::SetComponentSize(int theWidth, int theHeight)
{
	SetScrollDimension(theWidth,theHeight);
	mComponent->SetSize(theWidth,theHeight);
	PositionComponent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentScrollArea::UpdateComponentSize()
{
	SetComponentSize(mComponent->Width(), mComponent->Height());
}
