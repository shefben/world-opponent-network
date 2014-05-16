#ifndef __FriendOptionsPopup_H__
#define __FriendOptionsPopup_H__

#include "WONGUI/Container.h"

enum LobbyResourceId;
namespace WONAPI
{

class LobbyGroup;
class LobbyGame;

enum LobbyFriendAction;

WON_PTR_FORWARD(LobbyFriend);
WON_PTR_FORWARD(PopupMenu);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FriendOptionsPopup : public Container
{
protected:
	PopupMenuPtr	mPopupMenu;
	LobbyFriendPtr mFriend;
	int mRoomSpecFlags;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void AddItem(const GUIString &theStr, LobbyFriendAction theAction);

	virtual ~FriendOptionsPopup();

public:
	FriendOptionsPopup();

	void Init(LobbyFriend *theFriend, int theRoomSpecFlags);
};
typedef SmartPtr<FriendOptionsPopup> FriendOptionsPopupPtr;


} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __FriendOptionsPopup_H__