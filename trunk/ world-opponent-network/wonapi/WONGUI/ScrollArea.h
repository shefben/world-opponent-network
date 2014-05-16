#ifndef __WON_SCROLLAREA_H__
#define __WON_SCROLLAREA_H__

#include "Component.h"
#include "Scrollbar.h"
#include "Image.h"
#include "Background.h"

#include <set>

namespace WONAPI
{
class ScrollAreaViewport;
typedef SmartPtr<ScrollAreaViewport> ScrollAreaViewportPtr;

class Scroller;
typedef SmartPtr<Scroller> ScrollerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollArea : public Container
{
protected:
	ScrollAreaViewportPtr mViewport;
	WONRectangle mInvalidRect;

	int mVertOffset, mHorzOffset;
	int mTotalHeight, mTotalWidth;

	int mAVertOffset, mAHorzOffset;
	int mATotalHeight, mATotalWidth;
	int mOldScrollAreaWidth, mOldScrollAreaHeight;

	int mLeftPad, mRightPad, mTopPad, mBottomPad;
	bool mUserHasScrolled;

	friend class Scroller;
	Scroller* mScroller;
	WONRectangle mScrollArea;
	Background mBackground;

	int mExtraHeight; // needed for showing full lines
	int mUpScrollAmount, mDownScrollAmount, mLeftScrollAmount, mRightScrollAmount;

	void SetDelegatePos();
	virtual void AdjustPosition(bool becauseOfScrollbar = false);
	void ScrollTranslate(int &x, int &y);
	void ScrollUntranslate(int &x, int &y);

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void AddedToParent();
	virtual void PosChanged();
	virtual void SizeChanged();
	virtual void ScrollSizeChanged();
	virtual void Paint(Graphics &g);
	virtual void ScrollPaint(Graphics &g);
	virtual bool MouseWheel(int theAmount);
	virtual void GetDesiredSize(int &width, int &height);

	virtual void InvalidateRect(WONRectangle &theRect);
	virtual void Invalidate();

	ScrollAreaViewport* GetViewport() { return mViewport; }
	void SetScrollDimension(int theWidth, int theHeight);

public:
	ScrollArea();
	void SetScroller(Scroller *theScroller);
	void SetScrollbar(Scrollbar *theScrollbar);
	void SetScrollButton(Button *theButton, Direction theDirection);
	void SetBackground(const Background &theBackground) { mBackground = theBackground; ApplyBackground(mBackground); }
	Background& GetBackground() { return mBackground; }
	
	bool NeedScrollbar(bool vertical) const { return vertical? mTotalHeight>mScrollArea.Height() : mTotalWidth>mScrollArea.Width(); }
	const WONRectangle& GetScrollArea() const { return mScrollArea; }

	void SetBorderPadding(int theLeftPad, int theTopPad, int theRightPad, int theBottomPad);

	int GetTotalWidth() { return mTotalWidth; }
	int GetTotalHeight() { return mTotalHeight; }
	int GetLeftPad() { return mLeftPad; }
	int GetRightPad() { return mRightPad; }
	int GetTopPad() { return mTopPad; }
	int GetBottomPad() { return mBottomPad; }

	int GetHorzOffset() { return mHorzOffset; }
	int GetVertOffset() { return mVertOffset; }

	void SetVertOffset(int theOffset);
	void SetHorzOffset(int theOffset);
	void SetOffsets(int theHorzOffset, int theVertOffset);
	void SetSizeEqualToTotal(int theMinWidth = -1, int theMinHeight = -1);

	bool IsRectVisible(const WONRectangle &theRect);
	bool OnBottom();
};
typedef WONAPI::SmartPtr<ScrollArea> ScrollAreaPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Scroller : public Container
{
protected:
	ScrollAreaPtr mScrollArea;

	void SetOffsets(int theHorzOffset, int theVertOffset);
	void SetVertOffset(int theVertOffset);
	void SetHorzOffset(int theHorzOffset);

	void IncrementOffsets(int theHorzInc, int theVertInc);
	void IncrementHorzOffset(int theInc);
	void IncrementVertOffset(int theInc);

public:
	virtual void MouseDown(int x, int y, MouseButton theButton);

public:
	int GetHorzOffset() { return mScrollArea->GetHorzOffset(); }
	int GetVertOffset() { return mScrollArea->GetVertOffset(); }

	int GetTotalHeight() { return mScrollArea->mTotalHeight + mScrollArea->mExtraHeight; }
	int GetTotalWidth() { return mScrollArea->mTotalWidth; }
	int GetScrollAreaWidth() { return mScrollArea->mScrollArea.Width(); }
	int GetScrollAreaHeight() { return mScrollArea->mScrollArea.Height(); }

	int GetUpScrollAmount() { return mScrollArea->mUpScrollAmount; }
	int GetDownScrollAmount() { return mScrollArea->mDownScrollAmount; }
	int GetLeftScrollAmount() { return mScrollArea->mLeftScrollAmount; }
	int GetRightScrollAmount() { return mScrollArea->mRightScrollAmount; }
	
	ScrollArea* GetScrollArea() { return mScrollArea; }
	virtual bool MouseWheel(int theAmount);

public:
	Scroller();
	void SetScrollArea(ScrollArea *theScrollArea);
	virtual void NotifyScrollChange(bool /*becauseOfScroller*/) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ScrollbarCondition
{
	ScrollbarCondition_Disable		= 0,
	ScrollbarCondition_Enable		= 1,
	ScrollbarCondition_Conditional	= 2
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollbarScroller : public Scroller
{
protected:
	ScrollbarPtr mHorzScrollbar, mVertScrollbar;
	ScrollbarCondition mVertCondition, mHorzCondition;
	bool mSettingScrollbarConditions;

	bool EnableScrollbar(Scrollbar *theScrollbar, ScrollbarCondition theCondition);

public:
	virtual void NotifyScrollChange(bool becauseOfScroller);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void AddedToParent();

public:
	ScrollbarScroller();
	void SetScrollbar(Scrollbar *theScrollbar);
	void AddControls();

	void SetScrollbarConditions(ScrollbarCondition theHorzCondition, ScrollbarCondition theVertCondition);
	bool ScrollbarEnabled(bool vertical);

	Scrollbar* GetScrollbar(bool vertical) { return vertical?mVertScrollbar:mHorzScrollbar; }
};
typedef SmartPtr<ScrollbarScroller> ScrollbarScrollerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ScrollButtonCondition
{
	ScrollButtonCondition_Disable	= 0,
	ScrollButtonCondition_Enable	= 1,
	ScrollButtonCondition_Conditional = 2,
	ScrollButtonCondition_PartialConditional = 3
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ButtonScroller : public Scroller
{
protected:
	ButtonPtr mLeftButton, mRightButton, mUpButton, mDownButton;
	ScrollButtonCondition mVertCondition, mHorzCondition;
	bool mSettingScrollButtonConditions;

	bool EnableScrollButtons(Button *theLeft, Button *theRight, ScrollButtonCondition theCondition);

public:
	virtual void SizeChanged();
	virtual void NotifyScrollChange(bool becauseOfScroller);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void AddedToParent();


public:
	ButtonScroller();
	void SetButton(Button *theButton, Direction theDirection);
	void AddControls();

	void SetScrollButtonConditions(ScrollButtonCondition theHorzCondition, ScrollButtonCondition theVertCondition);

};
typedef SmartPtr<ButtonScroller> ButtonScrollerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScrollAreaViewport : public Container
{
protected:
	ScrollArea* mScrollArea;


public:
	ScrollAreaViewport(ScrollArea *theArea) : mScrollArea(theArea) { }
	void InvalidateIfRectVisible(WONRectangle &theRect);
	void Reset();

	virtual bool TimerEvent(int theDelta);

	virtual bool InvalidateUp(Component *theChild);
	virtual void ChildRequestFocus(Component *theChild);
	virtual void ChildRequestTimer(Component *theChild);
	virtual void ChildChanged(Component *theChild);
	virtual void SetCursor(Cursor *theCursor);
	virtual void Paint(Graphics &g);
};		

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentScrollArea : public ScrollArea
{
protected:
	ComponentPtr mComponent;
	bool mHorzCenter, mVertCenter;

	void PositionComponent();

public:
	virtual void SizeChanged();

public:
	ComponentScrollArea(Component *theComponent);
	void SetComponentSize(int theWidth, int theHeight);
	void UpdateComponentSize();

	Component* GetComponent() { return mComponent; }
};
typedef SmartPtr<ComponentScrollArea> ComponentScrollAreaPtr;

};

#endif