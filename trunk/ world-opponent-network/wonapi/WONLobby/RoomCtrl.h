#ifndef __ROOMCTRL_H__
#define __ROOMCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

class GroupChangedEvent;
class IPAddr;
class LobbyGroup;
class LobbyServer;
class ServerChangedEvent;
class TreeItem;
class TreeItemOpenedEvent;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(TreeArea);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoomCtrl : public Container
{
protected:
	TreeAreaPtr		mRoomList;
	ButtonPtr		mJoinButton;
	ButtonPtr		mCreateButton;

	void DoJoinRoom();
	void DoCreateRoom();

	TreeItem* GetServerItem(const IPAddr &theAddr);
	TreeItem* GetGroupItem(const IPAddr &theAddr, int theId);

	TreeItem* GetServerItem(LobbyServer *theServer);
	TreeItem* GetGroupItem(LobbyServer *theServer, LobbyGroup *theGroup);


	void AddServer(LobbyServer *theServer, bool isOpen = false, int theInsertPos = -1);
	void AddGroup(TreeItem *theServerItem, LobbyServer *theServer, LobbyGroup *theGroup);

	void HandleSetRoomList();
	void HandleServerChanged(ServerChangedEvent *theEvent);
	void HandleGroupChanged(GroupChangedEvent *theEvent);
	void HandleItemOpened(TreeItemOpenedEvent *theEvent);

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);
	void EnableControls();

public:
	RoomCtrl();
};
typedef SmartPtr<RoomCtrl> RoomCtrlPtr;

};

#endif