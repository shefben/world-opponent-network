#ifndef __TabCtrl_H__
#define __TabCtrl_H__
#include "WONGUI/Container.h"
#include "WONGUI/Button.h"

#include <vector>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TabCtrl;
class TabBar : public Container
{
protected:
	typedef std::vector<ButtonPtr> TabVector;
	TabVector mTabVector;
	Button *mCurTab;
	TabCtrl *mTabCtrl;
	int mOverlap;

	void PositionTab(Button *theTab);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

public:
	TabBar(TabCtrl *theCtrl);
	void ArrangeTabs();
	void AddTab(Button *theTab);
	void RemoveTab(int theId);
	void SetCurTab(int theId, bool containerRequestFocus = true);
	void SetCurTabByName(const GUIString &theName, bool containerRequestFocus = true); // uses Button::GetText 
	void EnableTab(int theId, bool theFlag = true);
	void EnableTabByName(const GUIString &theName, bool theFlag = true);
	bool Disabled(int theId);

	int GetCurTabId();
	Button* GetCurTab() { return mCurTab; }
	int GetTabByName(const GUIString &theName);
	int GetOverlap() { return mOverlap; }
	void SetOverlap(int theOverlap) { mOverlap = theOverlap; }
};
typedef SmartPtr<TabBar> TabBarPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TabCtrl : public Container
{
protected:
	friend class TabBar;

	TabBarPtr mBottomTabBar;
	ContainerPtr mTopTabBar;
	ContainerPtr mBottomTabBarContainer, mTopTabBarContainer;
	LayoutPointPtr mContainerLayout;

	int mLeftPad, mRightPad, mTopPad, mBottomPad;
	int mBottomTabLeftOffset, mBottomTabTopOffset;
	int mTabBarHeight, mTabBarOverlap;
	ButtonPtr mTabBarLeft, mTabBarRight;
	bool mArrowsOnLeft, mTabBarOnRight, mTabBarOnBottom;
	bool mUserHasInteracted;


	typedef std::vector<Container*> TabVector;
	TabVector mTabVector;
	Container* mCurTab;

	
	void AdjustTabBarScroll(int theAmount = 0);
	void KeepTabButtonInView();
	void NotifyTabChange(bool containerRequestFocus);
	virtual void AddedToParent();
	virtual void SizeChanged();
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual bool KeyDown(int theKey);
	virtual bool TakeTabFocus(bool forward, bool init);

	void PositionArrows();
	void HorzPositionTabBar();
	void VertPositionTabBar();
	void CalcContainerLayout();
	void PositionTabContainer(Container *theContainer);

	bool NeedScrollButtons();

public:
	TabCtrl();
	void AddTab(Button *theTab, Container *theContainer);
	void SetCurTab(int theId, bool containerRequestFocus = true);
	void SetCurTabByName(const GUIString &theName, bool containerRequestFocus = true); // uses Button::GetText 
	void EnableTab(int theId, bool theFlag = true);
	void EnableTabByName(const GUIString &theName, bool theFlag = true);
	bool Disabled(int theId);
	void RemoveTab(int theId);

	virtual void GetDesiredSize(int &width, int &height);


	int GetTabIdForContainer(Container *theContainer);
	int GetCurTabId() { return mBottomTabBar->GetCurTabId(); }
	int GetTabByName(const GUIString &theName) { return mBottomTabBar->GetTabByName(theName); }
	Container* GetCurTabContainer() { return mCurTab; }
	LayoutPoint* GetContainerLayout() { return mContainerLayout; }

	void SetTabArrows(Button *leftArrow, Button *rightArrow);
	void SetTabBarOnRight(bool onRight);
	void SetArrowsOnLeft(bool onLeft);
	void SetTabBarOnBottom(bool onBottom);

	void SetTabOverlap(int theOverlap) { mBottomTabBar->SetOverlap(theOverlap); }
	void SetTabBarOverlap(int theOverlap) { mTabBarOverlap = theOverlap; }
	void SetTabVertOffset(int theOffset) { mBottomTabTopOffset = theOffset; }
	void SetTabHorzOffset(int theOffset) { mBottomTabLeftOffset = theOffset; }
	void SetTabBarHeight(int theHeight) { mTabBarHeight = theHeight; }
	void SetUserHasInteracted(bool hasInteracted) { mUserHasInteracted = hasInteracted; }

	void CopyAttributes(TabCtrl *from);
};
typedef SmartPtr<TabCtrl> TabCtrlPtr;


}; // namespace WONAPI

#endif