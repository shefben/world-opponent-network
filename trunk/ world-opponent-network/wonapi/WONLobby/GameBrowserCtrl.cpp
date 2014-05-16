#include "GameBrowserCtrl.h"

#include "WONGUI/BrowserComponent.h"


using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameBrowserCtrl::GameBrowserCtrl(const GUIString &theHomeLocation) 
{
	mHomeLocation = theHomeLocation;
	mIsReset = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mBrowser,"Browser");
		WONComponentConfig_Get(aConfig,mBackButton,"BackButton");
		WONComponentConfig_Get(aConfig,mForwardButton,"ForwardButton");
		WONComponentConfig_Get(aConfig,mStopButton,"StopButton");
		WONComponentConfig_Get(aConfig,mHomeButton,"HomeButton");
		WONComponentConfig_Get(aConfig,mRefreshButton,"RefreshButton");
		WONComponentConfig_Get(aConfig,mFindInput,"FindInput");
		WONComponentConfig_Get(aConfig,mFindNextButton,"FindNextButton");
		WONComponentConfig_Get(aConfig,mFindPrevButton,"FindPrevButton");
		WONComponentConfig_Get(aConfig,mCaseSensitiveCheck,"CaseSensitiveCheck");
		WONComponentConfig_Get(aConfig,mStatusLabel,"StatusLabel");

		mBackButton->SetControlId(ControlId_Back);
		mForwardButton->SetControlId(ControlId_Forward);
		mStopButton->SetControlId(ControlId_Stop);
		mRefreshButton->SetControlId(ControlId_Refresh);
		mHomeButton->SetControlId(ControlId_Home);
		mFindNextButton->SetControlId(ControlId_FindNext);
		mFindPrevButton->SetControlId(ControlId_FindPrev);

		EnableButtons();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::EnableButtons()
{
	mBackButton->Enable(mBrowser->BackButtonEnabled());
	mForwardButton->Enable(mBrowser->ForwardButtonEnabled());
	mStopButton->Enable(mBrowser->StopButtonEnabled());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::DoFind(bool next)
{
	mBrowser->FindStr(mFindInput->GetText(),next,mCaseSensitiveCheck->IsChecked());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::Reset()
{
	mBrowser->Reset();
	mIsReset = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::GoHomeIfReset()
{
	if(mIsReset)
	{
		mIsReset = false;
		GoHome();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::GoHome()
{
	mBrowser->Go(mHomeLocation);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ButtonPressed:
			switch(theEvent->GetControlId())
			{
				case ControlId_Back:			mBrowser->Back(); break;
				case ControlId_Forward:			mBrowser->Forward(); break;
				case ControlId_Stop:			mBrowser->Stop(); break;
				case ControlId_Refresh:			mBrowser->Refresh(); break;
				case ControlId_Home:			GoHome(); break;
				case ControlId_FindNext:		DoFind(true); break;
				case ControlId_FindPrev:		DoFind(false); break;			
			}
			return;

		case ComponentEvent_BrowserStatusChange: mStatusLabel->SetText(mBrowser->GetCurStatus()); return;
//		case ComponentEvent_BrowserLocationChange: mLocationInput->SetText(mBrowser->GetCurLocation(),false); return;
		case ComponentEvent_BrowserButtonChange: EnableButtons(); return;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameBrowserCtrl::Paint(Graphics &g)
{
	GoHomeIfReset();
	Container::Paint(g);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameBrowserCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(ContainerConfig::HandleInstruction(theInstruction))
		return true;

	if(theInstruction=="HOMELOCATION")
	{
		std::string aPath = ReadString();
		URL aURL = aPath;
		if(aURL.GetType()==URLType_File)
			aPath = ComponentConfig::GetResourceFile(aPath);

		mBrowser->SetHomeLocation(aPath);
	}
	else
		return false;

	return true;
}
