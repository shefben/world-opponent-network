#include "RootScreen.h"

#include "GameBrowserCtrl.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"
#include "LobbyStaging.h"


#include "WONGUI/GUIConfig.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/TabCtrl.h"


using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RootScreen::RootScreen(bool isLan)
{
	mIsLan = isLan;
	if(isLan)
		mRoomSpecFlags = LobbyRoomSpecFlag_Lan;
	else
		mRoomSpecFlags = LobbyRoomSpecFlag_Internet;

	mHideLowerControls = false;
	mIsSwat = false;
	mLastJoinTab = -1;
	mDirectConnectTab = -1;
	mLanTab = -1;
	mLobbyTab = -1;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RootScreen::~RootScreen()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int	RootScreen::GetCurTab()
{
	return mTabCtrl->GetCurTabId(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RootScreen::OnDirectConnectTab()
{
	return mIsLan && mTabCtrl->GetCurTabId()==mDirectConnectTab;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RootScreen::OnLanTab()
{
	return mIsLan && mTabCtrl->GetCurTabId()==mLanTab;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		// Hacks for SwatUJ
		mIsSwat = aConfig->GetChildComponent("SWATUJDEF")!=NULL;
		
		WONComponentConfig_Get(aConfig,mTabCtrl,"TabCtrl");
		WONComponentConfig_Get(aConfig,mLowerLayoutPoint,"LowerLayoutPoint");
		WONComponentConfig_Get(aConfig,mLowerDivider,"LowerDivider");
		WONComponentConfig_Get(aConfig,mSierraLogo,"SierraLogo");
		WONComponentConfig_Get(aConfig,mLowerBanner,"LowerBanner");
		WONComponentConfig_Get(aConfig,mLowerBannerFrame,"LowerBannerFrame");
		WONComponentConfig_Get(aConfig,mBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,mHelpTab,"HelpTab");

		ContainerPtr aTab;
		WONComponentConfig_Get(aConfig,aTab,"GameTab"); mGameTab = mTabCtrl->GetTabIdForContainer(aTab);
		mTabCtrl->EnableTab(mGameTab,false);


		if(!mIsLan)
		{
			WONComponentConfig_Get(aConfig,mNewsTab,"NewsTab");
			WONComponentConfig_Get(aConfig,aTab,"LobbyTab"); mLobbyTab = mTabCtrl->GetTabIdForContainer(aTab);
			BroadcastEventDown(new SetRoomSpecFlagsEvent(LobbyRoomSpecFlag_Internet));
		}
		else
		{
			WONComponentConfig_Get(aConfig,aTab,"LanTab"); mLanTab = mTabCtrl->GetTabIdForContainer(aTab);
			WONComponentConfig_Get(aConfig,aTab,"DirectConnectTab"); mDirectConnectTab = mTabCtrl->GetTabIdForContainer(aTab);

			BroadcastEventDown(new SetRoomSpecFlagsEvent(LobbyRoomSpecFlag_Lan));
		}

		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::SizeChanged()
{
	if (mLowerLayoutPoint.get())
	{
		if (mHideLowerControls)
		{
			mLowerDivider->SetVisible(false);
			mSierraLogo->SetVisible(false);
			mLowerBanner->SetVisible(false);
			mLowerBannerFrame->SetVisible(false);
			mBackButton->SetVisible(false);

			mLowerLayoutPoint->SetTop(Height());
		}
		else
		{
			static int aFullBannerHt = mLowerBanner->Height();

			mLowerDivider->SetVisible(true);
			mSierraLogo->SetVisible(true);
			mBackButton->SetVisible(true);

			int aGroupHt = mLowerDivider->Height();

			bool showCrossPromotion = LobbyPersistentData::GetShowCrossPromotion();
			if (showCrossPromotion)
			{
				mLowerBannerFrame->SetVisible(true);
				mLowerBanner->SetVisible(true);

				mLowerBanner->SetHeight(aFullBannerHt);
				aGroupHt += aFullBannerHt + mLowerBannerFrame->GetThickness() * 2;
			}
			else
			{
				mLowerBannerFrame->SetVisible(false);
				mLowerBanner->SetVisible(false);

				int aControlHt = max(mSierraLogo->Height(),mBackButton->Height());
				aGroupHt += aControlHt;
				mLowerBanner->SetHeight(aControlHt);
			}

			aGroupHt += (mLowerDivider->Height() - 2) / 2; // Same whitespace as above banner.

			mLowerLayoutPoint->SetTop(Height() - aGroupHt);
		}
	}

	Container::SizeChanged();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::EnterGameTab(bool enter)
{
	if(enter)
	{
		mTabCtrl->EnableTab(mGameTab,true);
		if(mTabCtrl->GetCurTabId()!=mGameTab)
		{
			mLastJoinTab = mTabCtrl->GetCurTabId();
			mTabCtrl->SetCurTab(mGameTab);
		}
	}
	else
	{
		if(mTabCtrl->GetCurTabId()==mGameTab)
			mTabCtrl->SetCurTab(mLastJoinTab);
			
		mTabCtrl->EnableTab(mGameTab,false);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::ShowLobbyTab()
{
	mTabCtrl->SetCurTab(mLobbyTab);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::HandleJoinedGame()
{
	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL || !aGame->CheckRoomSpecFlags(mRoomSpecFlags))
		return;

	EnterGameTab(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::HandleLeftGame()
{
	EnterGameTab(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::HandleEnteredScreen()
{

	mHelpTab->Reset();
	if(mNewsTab.get()!=NULL)
		mNewsTab->Reset();

	mTabCtrl->SetCurTabByName(LobbyPersistentData::GetDefaultTabName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::HandleTabCtrlTabSelected(ComponentEvent *theEvent)
{
	// Hack for SwatUJ - we get smaller on the game tab.
	if (theEvent->mComponent==mTabCtrl)
	{
		if(mIsSwat)
		{
			static bool onGameTab = false;
			static int aPrevHeight = 0;

			Window *aWindow = GetWindow();
			WONRectangle aWindowRect, aClientRect;
			aWindow->GetScreenPos(aWindowRect,false);
			aWindow->GetScreenPos(aClientRect,true);
			int aFrameHeight = aWindowRect.Height() - aClientRect.Height();

			if (mTabCtrl->GetCurTabId()==2)
			{
				mHideLowerControls = true;
				onGameTab = true;
				aPrevHeight = aWindowRect.height;
				aWindow->Move(aWindowRect.x, aWindowRect.y, aWindowRect.width, 150 + aFrameHeight);
			}
			else if(onGameTab)
			{
				mHideLowerControls = false;
				onGameTab = false;
				aWindow->Move(aWindowRect.x, aWindowRect.y, aWindowRect.width, aPrevHeight);
			}
			RecalcLayout();
		}

		LobbyPersistentData::WriteUser(false);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RootScreen::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_JoinedGame: HandleJoinedGame(); return;
		case LobbyEvent_LeftGame: HandleLeftGame(); return;
		case LobbyEvent_EnteredScreen: HandleEnteredScreen(); return;

		case ComponentEvent_ButtonPressed:
			if(theEvent->GetControlId()==ID_CrossPromotionCheck)
			{
				RecalcLayout(); // hide/show cross promotion
				return;
			}
			break;


		case ComponentEvent_TabCtrlTabSelected:
			HandleTabCtrlTabSelected(theEvent);
			break;
	}
	Container::HandleComponentEvent(theEvent);
}

