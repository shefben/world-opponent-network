#include "FriendOptionsPopup.h"

#include "LobbyFriend.h"
#include "LobbyEvent.h"
#include "LobbyGroup.h"
#include "LobbyResource.h"

#include "WONGUI/MenuBar.h"
#include "WONGUI/Window.h"


using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FriendOptionsPopup::FriendOptionsPopup()
{
	mPopupMenu = new PopupMenu;
	AddChild(mPopupMenu);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FriendOptionsPopup::~FriendOptionsPopup()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendOptionsPopup::AddItem(const GUIString &theStr, LobbyFriendAction theAction)
{
	mPopupMenu->AddItem(theStr,NULL,NULL,theAction);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendOptionsPopup::Init(LobbyFriend *theFriend, int theRoomSpecFlags)
{
	if(theFriend==NULL)
		return;

	mFriend = theFriend;
	mRoomSpecFlags = theRoomSpecFlags;

	mPopupMenu->Clear(false);
	
	FontPtr aNormalFont = mPopupMenu->GetFont();
	FontPtr aBoldFont = GetFontMod(aNormalFont,FontStyle_Bold);

	mPopupMenu->SetFont(aBoldFont);
	MenuItem *anItem = mPopupMenu->AddItem(theFriend->GetName());
	anItem->mSelectable = false;

	mPopupMenu->SetFont(aNormalFont);

	mPopupMenu->AddSeperator();
	AddItem(FriendOptionsCtrl_Locate_String, LobbyFriendAction_Locate);
	AddItem(FriendOptionsCtrl_RefreshList_String, LobbyFriendAction_RefreshList);
	mPopupMenu->AddSeperator();
	AddItem(FriendOptionsCtrl_RemoveFromFriends_String, LobbyFriendAction_Remove);

	SetSize(mPopupMenu->Width(), mPopupMenu->Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendOptionsPopup::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ListItemUpClicked:
			{
				ListItemClickedEvent* theMenuEvent = (ListItemClickedEvent*)theEvent;
				if (theMenuEvent->mItem == NULL || theMenuEvent->mButton != MouseButton_Left)
					return;


				LobbyFriendAction anAction = (LobbyFriendAction)((MenuItem*)(theMenuEvent->mItem.get()))->mMenuId;
				if(anAction!=LobbyFriendAction_None)
					FireEvent(new FriendActionEvent(anAction,mFriend,mRoomSpecFlags));

				GetWindow()->EndPopup(this);
				return;
			}
	}

	Container::HandleComponentEvent(theEvent);
}

