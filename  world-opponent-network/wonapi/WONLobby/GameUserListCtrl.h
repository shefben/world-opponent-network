#ifndef __GAMEUSERLISTCTRL_H__
#define __GAMEUSERLISTCTRL_H__

#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{

class ClientChangedEvent;
class GameUserListItem;
class LobbyClient;
class PlayerChangedEvent;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(MultiListArea);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameUserListCtrl : public Container
{
protected:
	MultiListAreaPtr mUserList;
	ButtonPtr mKickButton;
	int mRoomSpecFlags;
	bool mInGame;

public:
	void UpdateKickButton();
	void HandleKickPlayer(bool isBan); 

	LobbyClient* GetSelectedClient();
	GameUserListItem* GetListItem(LobbyClient *theClient);

	void HandlePlayerChanged(PlayerChangedEvent *theEvent);
	void HandleClientChanged(ClientChangedEvent *theEvent);
	void HandleJoinedGame();
	void HandleLeftGame();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

public:
	GameUserListCtrl();
	void SetRoomSpecFlags(int theFlags) { mRoomSpecFlags = theFlags; }
};
typedef SmartPtr<GameUserListCtrl> GameUserListCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameUserListCtrlConfig : public ContainerConfig
{
protected:
	GameUserListCtrl *mUserList;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { ContainerConfig::SetComponent(theComponent); mUserList = (GameUserListCtrl*)theComponent; }

	static ComponentConfig* CfgFactory() { return new GameUserListCtrlConfig; }
	static Component* CompFactory() { return new GameUserListCtrl; }
};

};

#endif