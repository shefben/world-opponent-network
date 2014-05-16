#ifndef __WON_GAMEBROWSERCTRL_H__
#define __WON_GAMEBROWSERCTRL_H__

#include "WONGUI/Container.h"
#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{

WON_PTR_FORWARD(BrowserComponent);
WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(InputBox);
WON_PTR_FORWARD(Label);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameBrowserCtrl : public Container
{
protected:
	enum 
	{
		ControlId_Refresh = 1,
		ControlId_Back,
		ControlId_Forward,
		ControlId_Home,
		ControlId_Stop,
		ControlId_Find,
		ControlId_FindNext,
		ControlId_FindPrev
	};

	BrowserComponentPtr mBrowser;
	ButtonPtr mBackButton;
	ButtonPtr mForwardButton;
	ButtonPtr mStopButton;
	ButtonPtr mHomeButton;
	ButtonPtr mRefreshButton;
	
	InputBoxPtr mFindInput;
	ButtonPtr mFindNextButton;
	ButtonPtr mFindPrevButton;
	ButtonPtr mCaseSensitiveCheck;
	LabelPtr mStatusLabel;

	bool mIsReset;
	GUIString mHomeLocation;

	void DoFind(bool next);
	void EnableButtons();

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);
	virtual void Paint(Graphics &g);

public:
	GameBrowserCtrl(const GUIString &theHomeLocation = "");
	void Reset();
	void GoHomeIfReset();
	void GoHome();

	BrowserComponent* GetBrowser() { return mBrowser; }

	void SetHomeLocation(const GUIString &theLocation) { mHomeLocation = theLocation; }

};
typedef SmartPtr<GameBrowserCtrl> GameBrowserCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameBrowserCtrlConfig : public ContainerConfig
{
protected:
	GameBrowserCtrl* mBrowser;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { ContainerConfig::SetComponent(theComponent); mBrowser = (GameBrowserCtrl*)theComponent; }

	static Component* CompFactory() { return new GameBrowserCtrl; }
	static ComponentConfig* CfgFactory() { return new GameBrowserCtrlConfig; }
};


}; // namespace WONAPI

#endif