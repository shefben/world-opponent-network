#ifndef __FRIENDSLISTCTRL_H__
#define __FRIENDSLISTCTRL_H__

#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{

class FriendChangedEvent;
class LobbyFriend;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(FriendsListItem);
WON_PTR_FORWARD(ListArea);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FriendsListCtrl : public Container
{
protected:
	ListAreaPtr		mFriendsList;

	ButtonPtr		mRemoveButton;
	ButtonPtr		mLocateButton;

	FriendsListItemPtr	mSeperator;

	void UpdateItem(LobbyFriend *theFriend, FriendsListItem *theItem);
	FriendsListItem* GetListItem(LobbyFriend *theFriend);
	LobbyFriend* GetSelectedFriend();

	void UpdateSeperator();
	void UpdateButtons();

	void HandleSyncFriendsList();
	void LoadFriendsList();
	void HandleFriendChanged(FriendChangedEvent *theEvent);

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

public:
	FriendsListCtrl();

	static Component* CompFactory() { return new FriendsListCtrl; }
	static ComponentConfig* CfgFactory() { return new ContainerConfig; }
};
typedef SmartPtr<FriendsListCtrl> FriendsListCtrlPtr;

};

#endif