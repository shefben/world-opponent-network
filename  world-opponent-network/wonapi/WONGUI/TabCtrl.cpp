#include "TabCtrl.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TabBar::TabBar(TabCtrl *theCtrl)
{
	mCurTab = NULL;
	mOverlap = 4;
	mTabCtrl = theCtrl;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TabBar::GetCurTabId()
{
	if(mCurTab==NULL)
		return -1;
	else
		return mCurTab->GetControlId();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TabBar::GetTabByName(const GUIString &theName)
{
	for(int i=0; i<mTabVector.size(); i++)
	{
		if(mTabVector[i]->GetText()==theName)
		{
			return i;
		}
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::ArrangeTabs()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::PositionTab(Button *theTab)
{
	int anId = theTab->GetControlId();
	int left = 0, top = 0;

	if(anId>0 && anId<=mTabVector.size())
	{
		Button *aPrevTab = mTabVector[anId-1];
		left = aPrevTab->Right()-mOverlap;
	}
	theTab->SetPos(left,top);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::AddTab(Button *theTab)
{
	theTab->SetButtonFlags(ButtonFlag_ActivateDown,true);
	theTab->SetControlId(mTabVector.size());
	mTabVector.push_back(theTab);
	PositionTab(theTab);
	SetWidth(theTab->Right());
	AddChildEx(theTab,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::RemoveTab(int theId)
{
	if(theId<0 || theId>=mTabVector.size())
		return;

	int aCurTab = GetCurTabId();

	RemoveChild(mTabVector[theId]);
	mTabVector.erase(mTabVector.begin() + theId);
	int i;
	for(i=0; i<mTabVector.size(); i++)
		mTabVector[i]->SetControlId(i);

	mCurTab = NULL;
	for(i=0; i<mTabVector.size(); i++)
	{
		Button *aButton = mTabVector[i];
		if(aButton->GetParent()!=this)
			AddChildEx(aButton,mTabVector.size()-i-1);
			
		PositionTab(aButton);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::SetCurTab(int theId, bool containerRequestFocus)
{
	if(theId<0 || theId>=mTabVector.size())
		return;

	int aCurId = GetCurTabId();
	if(aCurId==theId)
	{
		if(containerRequestFocus && mCurTab!=NULL)
			mCurTab->RequestFocus();

		return;
	}

	Button* aNewTab = mTabVector[theId];
	if(aNewTab->Disabled())
		return;

	if(mCurTab!=NULL)
	{
		mCurTab->SetCheck(false);
		if(mCurTab->GetParent()!=this)
		{
			AddChildEx(mCurTab,mTabVector.size()-aCurId-1);
			PositionTab(mCurTab);
		}
	}

	mCurTab = aNewTab;
	mCurTab->SetCheck(true);
	mTabCtrl->SetUserHasInteracted(true);
	mTabCtrl->NotifyTabChange(containerRequestFocus);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::SetCurTabByName(const GUIString &theName, bool containerRequestFocus)
{
	for(int i=0; i<mTabVector.size(); i++)
	{
		if(mTabVector[i]->GetText()==theName)
		{
			SetCurTab(i,containerRequestFocus);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::EnableTab(int theId, bool theFlag)
{
	if(theId<0 || theId>=mTabVector.size())
		return;

	int aCurId = GetCurTabId();
	if((aCurId==theId) && (false==theFlag))
	{
		aCurId = aCurId>0 ? aCurId-1 : aCurId+1;
		SetCurTab(aCurId);
	}

	Button* aTab = mTabVector[theId];
	if(aTab!=NULL)
	{
		aTab->Enable(theFlag);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::EnableTabByName(const GUIString &theName, bool theFlag)
{
	for(int i=0; i<mTabVector.size(); i++)
	{
		if(mTabVector[i]->GetText()==theName)
		{
			EnableTab(i,theFlag);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabBar::Disabled(int theId)
{
	if(theId<0 || theId>=mTabVector.size())
		return true;

	Button* aTab = mTabVector[theId];
	if(NULL==aTab)
		return true;

	return aTab->Disabled();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabBar::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		int anId = theEvent->GetControlId();
		if(anId>=0 && anId<mTabVector.size() && theEvent->mComponent==mTabVector[anId])
		{
			SetCurTab(anId,true);
			return;
		}
	}
	Container::HandleComponentEvent(theEvent);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TabCtrl::TabCtrl()
{
	mLeftPad = mRightPad = mTopPad = mBottomPad = 5;
	mBottomTabLeftOffset = 2;
	mBottomTabTopOffset = 3;
	mCurTab = NULL;
	mTabBarHeight = 0;
	mTabBarOverlap = 1;

	mArrowsOnLeft = false;
	mTabBarOnRight = false;
	mTabBarOnBottom = false;


	mBottomTabBarContainer = new Container;
	mTopTabBarContainer = new Container;

	mBottomTabBar = new TabBar(this);
	mTopTabBar = new Container;
	mTopTabBar->SetOnlyChildInput(true); 
	mTopTabBarContainer->SetOnlyChildInput(true);
	mTopTabBarContainer->SetComponentFlags(ComponentFlag_GrabBG,true);
	mBottomTabBarContainer->SetComponentFlags(ComponentFlag_GrabBG,true);
	mBottomTabBarContainer->SetComponentFlags(ComponentFlag_WantFocus,false);


	mBottomTabBarContainer->SetComponentFlags(ComponentFlag_Clip,true);
	mTopTabBarContainer->SetComponentFlags(ComponentFlag_Clip | ComponentFlag_AlwaysOnTop,true);
	mBottomTabBarContainer->AddChild(mBottomTabBar);
	mTopTabBarContainer->AddChild(mTopTabBar);
	AddChild(mBottomTabBarContainer);

	mContainerLayout = new LayoutPoint;
	AddLayoutPoint(mContainerLayout);

	AddChild(mTopTabBarContainer);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::CopyAttributes(TabCtrl *from)
{
	mBottomTabLeftOffset = from->mBottomTabLeftOffset;
	mBottomTabTopOffset = from->mBottomTabTopOffset;
	mTabBarHeight = from->mTabBarHeight;
	mArrowsOnLeft = from->mArrowsOnLeft;
	mTabBarOnRight = from->mTabBarOnRight;
	mTabBarOnBottom = from->mTabBarOnBottom;
	mTabBarOverlap = from->mTabBarOverlap;
	
	mBottomTabBar->SetOverlap(from->mBottomTabBar->GetOverlap());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::GetDesiredSize(int &width, int &height)
{
	Container::GetDesiredSize(width,height);
	width += mLeftPad + mRightPad;
	height += mTopPad + mBottomPad + mTabBarHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SetTabBarOnRight(bool onRight)
{
	mTabBarOnRight = onRight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SetArrowsOnLeft(bool onLeft)
{
	mArrowsOnLeft = onLeft;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SetTabBarOnBottom(bool onBottom)
{
	mTabBarOnBottom = onBottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SetTabArrows(Button *leftArrow, Button *rightArrow)
{
	mTabBarLeft = leftArrow;
	mTabBarRight = rightArrow;

	mTabBarLeft->SetComponentFlags(ComponentFlag_AlwaysOnTop,true);
	mTabBarRight->SetComponentFlags(ComponentFlag_AlwaysOnTop,true);

	mTabBarLeft->SetButtonFlags(ButtonFlag_RepeatFire,true);
	mTabBarRight->SetButtonFlags(ButtonFlag_RepeatFire,true);

	AddChild(mTabBarLeft);
	AddChild(mTabBarRight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::NotifyTabChange(bool containerRequestFocus)
{
	Button *aButton = mBottomTabBar->GetCurTab();
	if(aButton==NULL)
		return;

	mTopTabBar->AddChild(aButton);
	int anId = mBottomTabBar->GetCurTabId();

	if(anId<0 || anId>=mTabVector.size())
		return;

	Container *aNewTab = mTabVector[anId];
	if(aNewTab==mCurTab)
		return;

	if(mCurTab!=NULL)
		mCurTab->SetVisible(false);

	mCurTab = aNewTab;	
	PositionTabContainer(mCurTab);
	mCurTab->SetVisible(true);
	BringToTop(mCurTab);

	if(containerRequestFocus)
		mCurTab->RequestFocus();

	KeepTabButtonInView();

	Invalidate();

	FireEvent(ComponentEvent_TabCtrlTabSelected);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::KeepTabButtonInView()
{
	Button *aButton = mBottomTabBar->GetCurTab();
	if(aButton==NULL)
		return;

	int aButtonLeft = aButton->Left() + mBottomTabBar->Left();
	int aButtonRight = aButtonLeft + aButton->Width();
	int anOffset = 0;
	if(aButtonLeft<0 || aButton->Width()>mTopTabBarContainer->Width())
		AdjustTabBarScroll(-aButtonLeft);
	else if(aButtonRight > mTopTabBarContainer->Width())
		AdjustTabBarScroll(mTopTabBarContainer->Width() - aButtonRight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::PositionTabContainer(Container *theContainer)
{
	if(theContainer==NULL)
		return;
	
	theContainer->SetPos(mLeftPad,mContainerLayout->Top()+mTopPad);
	theContainer->SetSize(Width()-mLeftPad-mRightPad,mContainerLayout->Height()-mTopPad-mBottomPad);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::AddedToParent()
{
	Container::AddedToParent();
	if(mCurTab==NULL) 
		SetCurTab(0,false);

	mUserHasInteracted = false;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::CalcContainerLayout()
{
	int top = 0, bottom = Height();
	if(mTabBarOnBottom)
		bottom = mTopTabBarContainer->Top()+mTabBarOverlap; //mBottomTabBarContainer->Top();
	else
		top = mTopTabBarContainer->Bottom()-mTabBarOverlap; //mBottomTabBarContainer->Bottom();

	mContainerLayout->SetPosSize(0,top,Width(),bottom-top);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::PositionArrows()
{
	if(mTabBarLeft.get()==NULL || mTabBarRight.get()==NULL)
		return;

	int left = 0;
	if(!mArrowsOnLeft)
		left = Width()-mTabBarLeft->Width()-mTabBarRight->Width();

	int arrowOffset = 0;
	int top;
	if(mTabBarOnBottom)
		top = mContainerLayout->Bottom()+arrowOffset;
	else
		top = mContainerLayout->Top()-mTabBarLeft->Height()-arrowOffset;


	mTabBarLeft->SetPos(left,top);
	mTabBarRight->SetPos(mTabBarLeft->Right(),top);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::VertPositionTabBar()
{
	mBottomTabBar->SetHeight(mTabBarHeight-mBottomTabTopOffset-mTabBarOverlap);
	mBottomTabBarContainer->SetHeight(mBottomTabBar->Height());
	mTopTabBar->SetHeight(mTabBarHeight);
	mTopTabBarContainer->SetHeight(mTabBarHeight);

	int top = 0;

	int aTopOffset = mBottomTabTopOffset;
	if(mTabBarOnBottom)
	{
		top = Height()-mBottomTabBarContainer->Height();
		aTopOffset *= -1;
	}

	mTopTabBarContainer->SetTop(top);
	mBottomTabBarContainer->SetTop(top+aTopOffset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::HorzPositionTabBar()
{
	int left = 0, right = Width();
	bool needScrollButtons = NeedScrollButtons();
	if(needScrollButtons)
	{
		if(mArrowsOnLeft)
		{
			if(mTabBarRight.get()!=NULL && mTabBarRight->IsVisible())
				left = mTabBarRight->Right();
		}
		else
		{
			if(mTabBarLeft.get()!=NULL && mTabBarLeft->IsVisible())
				right = mTabBarLeft->Left()-1;
		}
	}
	else if(mTabBarOnRight)
		left = Width()-mBottomTabBar->Width()-mBottomTabLeftOffset;


	mTopTabBarContainer->SetLeft(left);
	mTopTabBarContainer->SetWidth(right-left);
	mBottomTabBarContainer->SetLeft(left+mBottomTabLeftOffset);
	mBottomTabBarContainer->SetWidth(right-left-mBottomTabLeftOffset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SizeChanged()
{
	if(mTabBarLeft.get()!=NULL && mTabBarRight.get()!=NULL)
	{
		bool needScrollButtons = NeedScrollButtons();
		mTabBarLeft->SetVisible(needScrollButtons);
		mTabBarRight->SetVisible(needScrollButtons);
	}

	VertPositionTabBar();
	CalcContainerLayout();
	PositionArrows();	
	HorzPositionTabBar();

	PositionTabContainer(mCurTab);

	AdjustTabBarScroll();
	if(!mUserHasInteracted)
		KeepTabButtonInView(); 

	Container::SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabCtrl::NeedScrollButtons()
{
	return mBottomTabBar->Width() > Width();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::AdjustTabBarScroll(int theAmount)
{
	int anOldLeft = mBottomTabBar->Left();
	int aLeft = anOldLeft + theAmount;

	int aScrollWidth = mBottomTabBar->Width();
	int aViewWidth = mBottomTabBarContainer->Width();
	if(aViewWidth>=aScrollWidth)
		aLeft = 0;
	else 
	{
		bool enableLeft = true, enableRight = true;
		if(aLeft+aScrollWidth<aViewWidth-5) // too far right
		{
			enableRight= false;
			aLeft = aViewWidth-5-aScrollWidth;
		}
		if(aLeft>=0)
		{
			enableLeft = false;
			aLeft = 0;
		}

		if(mTabBarLeft.get()!=NULL && mTabBarRight.get()!=NULL)
		{
			mTabBarLeft->Enable(enableLeft);
			mTabBarRight->Enable(enableRight);
		}

	}

	if(aLeft!=anOldLeft)
	{
		mBottomTabBar->SetLeft(aLeft);
		mTopTabBar->SetLeft(aLeft);
		mBottomTabBarContainer->Invalidate();
		mTopTabBarContainer->Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType==ComponentEvent_ButtonPressed)
	{
		int aTabOffset = 0;
		if(theEvent->mComponent==mTabBarLeft)
			aTabOffset = 10;
		else if(theEvent->mComponent==mTabBarRight)
			aTabOffset = -10;
		else
		{
			Container::HandleComponentEvent(theEvent);
			return;
		}

		mUserHasInteracted = true;
		AdjustTabBarScroll(aTabOffset);
	}
	else			
		Container::HandleComponentEvent(theEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabCtrl::TakeTabFocus(bool forward, bool init)
{
	// The tab button is last in the container but we want it to tab like
	// it's first.
	Button *aButton = mBottomTabBar->GetCurTab();
	if(aButton==NULL || mCurTab==NULL)
		return Container::TakeTabFocus(forward,init);
	
	bool setFocus = false;
	if(init)
	{
		if(!forward)
			setFocus = mCurTab->TakeTabFocus(forward,true);

		if(!setFocus)
		{
			setFocus = aButton->TakeTabFocus(forward,true);
			if(setFocus)
				mTopTabBar->SetHasUsedDialogKeys(true);
		}

		return setFocus;
	}

	if(aButton->HasFocus())
	{
		if(forward)
			return mCurTab->TakeTabFocus(true,true);
		else
			return false;
	}
	else 
	{
		if(mCurTab->TakeTabFocus(forward,false))
			return true;

		if(forward)
			return false;
		else if(aButton->TakeTabFocus(forward,true))
		{
			mTopTabBar->SetHasUsedDialogKeys(true);
			return true;
		}
		else
			return false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabCtrl::KeyDown(int theKey)
{
	Button *aButton = mBottomTabBar->GetCurTab();
	if(aButton==NULL || !aButton->HasFocus())
		return Container::KeyDown(theKey);

	int dx = 0;
	if(theKey==KEYCODE_RIGHT)
		dx = 1;
	else if(theKey==KEYCODE_LEFT)
		dx = -1;
	else
		return Container::KeyDown(theKey);

	int anId = mBottomTabBar->GetCurTabId();
	while(mBottomTabBar->Disabled(anId+dx) && (anId+dx > 0) && (anId+dx < mTabVector.size()-1))
	{
		if(theKey==KEYCODE_RIGHT)
			dx += 1;
		else if(theKey==KEYCODE_LEFT)
			dx -= 1;
	}

	mBottomTabBar->SetHasUsedDialogKeys(true);
	mTopTabBar->SetHasUsedDialogKeys(true);
	mUserHasInteracted = true;
	SetCurTab(anId+dx,false);		

	aButton = mBottomTabBar->GetCurTab();
	if(aButton!=NULL)	
		aButton->RequestFocus();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SetCurTab(int theId, bool containerRequestFocus)
{
	mBottomTabBar->SetCurTab(theId,containerRequestFocus);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::SetCurTabByName(const GUIString &theName, bool containerRequestFocus)
{
	mBottomTabBar->SetCurTabByName(theName,containerRequestFocus);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TabCtrl::GetTabIdForContainer(Container *theContainer)
{
	// will search to find which tab contain this container
	for(int i=0; i<mTabVector.size(); i++)
		if(mTabVector[i]==theContainer)
			return i;

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::EnableTab(int theId, bool theFlag)
{
	mBottomTabBar->EnableTab(theId,theFlag);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TabCtrl::Disabled(int theId)
{
	return mBottomTabBar->Disabled(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::EnableTabByName(const GUIString &theName, bool theFlag)
{
	mBottomTabBar->EnableTabByName(theName,theFlag);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::AddTab(Button *theTab, Container *theContainer)
{
	if(mTabBarHeight==0)
	{
		if(theTab->Height()>0)
			mTabBarHeight = theTab->Height();
		else
			mTabBarHeight = 20;
	}

	theTab->SetHeight(mTabBarHeight);
	theTab->SetComponentFlags(ComponentFlag_GrabBG,false);

	mTabVector.push_back(theContainer);
	bool oldNeedScrollButtons = NeedScrollButtons();

	mBottomTabBar->AddTab(theTab);
	if(theContainer!=NULL)
	{
		theContainer->SetVisible(false);
		theContainer->SetComponentFlags(ComponentFlag_Clip,true);
		AddChild(theContainer);
	}

//  moved this to AddedToParent
//	if(mCurTab==NULL) 
//		SetCurTab(0,true);

	if(NeedScrollButtons()!=oldNeedScrollButtons)
		SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TabCtrl::RemoveTab(int theId)
{
	if(theId<0 || theId>=mTabVector.size())
		return;

	int aCurTabId = GetCurTabId();

	mTopTabBar->RemoveChild(mBottomTabBar->GetCurTab());
	mCurTab = NULL;

	RemoveChild(mTabVector[theId]);
	mTabVector.erase(mTabVector.begin() + theId);
	mBottomTabBar->RemoveTab(theId);
	

	if(theId<aCurTabId)
		SetCurTab(aCurTabId-1);
	else if(theId>aCurTabId)
		SetCurTab(aCurTabId);
	else 
		SetCurTab(0);
}