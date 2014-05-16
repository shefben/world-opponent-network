#ifndef __USERLISTCTRL_H__
#define __USERLISTCTRL_H__

#include "WONGUI/WONGUIConfig/ContainerConfig.h"

namespace WONAPI
{

enum LobbyClientAction;

class ClientChangedEvent;
class JoinedRoomEvent;
class ListItem;
class LobbyClient;
class LeftRoomEvent;

WON_PTR_FORWARD(Button);
WON_PTR_FORWARD(ChatCtrl);
WON_PTR_FORWARD(ListArea);
WON_PTR_FORWARD(UserListItem);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UserListCtrl : public Container
{
protected:
	bool			mActive;
	int				mRoomSpecFlags;

	ListAreaPtr		mUserList;
	UserListItemPtr	mSep1, mSep2;
	
	ButtonPtr		mWhisperButton;
	ButtonPtr		mIgnoreButton;

	void UpdateItem(LobbyClient *theClient, UserListItem *theItem);
	UserListItem* GetListItem(LobbyClient *theClient);
	UserListItem* GetSelectedItem();
	LobbyClient* GetSelectedClient();

	void HandleJoinedRoom(JoinedRoomEvent *theEvent);
	void HandleLeftRoom(LeftRoomEvent *theEvent);
	void HandleClientChanged(ClientChangedEvent *theEvent);

	void UpdateSeperators();
	void UpdateIgnoreButton();
	
	virtual ~UserListCtrl();

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

public:
	UserListCtrl();

	void SetRoomSpecFlags(int theFlags) { mRoomSpecFlags = theFlags; }
};
typedef SmartPtr<UserListCtrl> UserListCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UserListCtrlConfig : public ContainerConfig
{
protected:
	UserListCtrl* mUserList;
	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent) { ContainerConfig::SetComponent(theComponent); mUserList = (UserListCtrl*)theComponent; }

	static Component* CompFactory() { return new UserListCtrl; }
	static ComponentConfig* CfgFactory() { return new UserListCtrlConfig; }
};


};

#endif