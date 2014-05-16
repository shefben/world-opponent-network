#ifndef __WON_ROOTSCREEN_H__
#define __WON_ROOTSCREEN_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

class SetGameEvent;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(Component);
WON_PTR_FORWARD(GameBrowserCtrl);
WON_PTR_FORWARD(MSSeperator);
WON_PTR_FORWARD(RectangleComponent3D);
WON_PTR_FORWARD(TabCtrl);

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class RootScreen : public Container
{
protected:
	bool mIsLan;

	TabCtrlPtr              mTabCtrl;            // Main tab control.
	GUIString               mDefaultTabName;     // Name of the default tab to display when the tab control is first displayed.
	GameBrowserCtrlPtr      mHelpTab;            // Help tab.
	GameBrowserCtrlPtr		mNewsTab;			 // Community Tab (only on internet)

	ComponentPtr			mLowerLayoutPoint;   // Where to draw the main screen down to.  This was added for SwatUJ which can turn off the lower controls.
	MSSeperatorPtr			mLowerDivider;       // Divider to draw above the banner.
	ComponentPtr			mSierraLogo;         // Sierra logo (next to the banner).
	ComponentPtr			mLowerBanner;        // Its an Ad!
	RectangleComponent3DPtr	mLowerBannerFrame;   // Frame for the ad.
	ComponentPtr			mBackButton;         // Back button (return to login screen).

	bool                    mHideLowerControls;  // Hack for SwatUJ.
	bool                    mIsSwat;             // Hack for SwatUJ.

	int                     mLastJoinTab;        // 
	int                     mGameTab;            // Which tab [index] is the game tab?
	int						mDirectConnectTab;
	int						mLanTab;
	int						mLobbyTab;
	int						mRoomSpecFlags;

protected:
	void HandleTabCtrlTabSelected(ComponentEvent *theEvent);
	void HandleJoinedGame();
	void HandleLeftGame();
	void HandleEnteredScreen();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	virtual ~RootScreen();

public:
	RootScreen(bool isLan);

	virtual void InitComponent(ComponentInit &theInit);
	virtual void SizeChanged();
	void EnterGameTab(bool enter);

	int  GetCurTab();
	bool OnDirectConnectTab();
	bool OnLanTab();

	void ShowLobbyTab();
};
typedef SmartPtr<RootScreen> RootScreenPtr;


}; // namespace WONAPI

#endif