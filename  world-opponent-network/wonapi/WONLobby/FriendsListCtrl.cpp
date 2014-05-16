#include "FriendsListCtrl.h"

#include "LobbyEvent.h"
#include "LobbyFriend.h"
#include "LobbyPersistentData.h"
#include "LobbyResource.h"

#include "WONGUI/ListBox.h"


using namespace WONAPI;

namespace WONAPI
{
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FriendsListItem : public TextListItem
{
public:
	LobbyFriendPtr	mFriend;
	bool mIsVisible;
	char mSortValue;

public:
	FriendsListItem();

	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
	virtual void Paint(Graphics &g, ListArea *theListArea);
};
typedef SmartPtr<FriendsListItem> FriendsListItemPtr;

} // namespace WONAPI

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FriendsListItem::FriendsListItem()
{
	mIsVisible = true;
	mSortValue = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListItem::CalcDimensions(ListArea *theListArea)
{
	if(mFriend.get()==NULL)
	{
		mWidth = 1;
		if(mIsVisible)
			mHeight = 8;
		else
			mHeight = 1;
	}
	else
		TextListItem::CalcDimensions(theListArea);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FriendsListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	int aDiff = ((FriendsListItem*)theItem)->mSortValue - mSortValue;
	if(aDiff==0)
		return TextListItem::Compare(theItem,theListArea);
	else
		return aDiff;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListItem::Paint(Graphics &g, ListArea *theListArea)
{
	if (mFriend.get()==NULL)
	{
		if(mIsVisible)
		{
			int aWidth = theListArea->GetTotalWidth()+10;	// 10 padding at the end ?
			if (theListArea->GetPaintColumnWidth() > aWidth)
				aWidth = theListArea->GetPaintColumnWidth();

			int aHeight = mHeight>>2;
			g.ApplyColorSchemeColor(StandardColor_Text);
			g.DrawLine(0,aHeight,aWidth,aHeight);
		}
	}
	else
		TextListItem::Paint(g,theListArea);

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FriendsListCtrl::FriendsListCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mFriendsList,"FriendsList");
		WONComponentConfig_Get(aConfig,mRemoveButton,"RemoveButton");
		WONComponentConfig_Get(aConfig,mLocateButton,"LocateButton");

		mSeperator = new FriendsListItem;
		mSeperator->mSortValue = 1;
		mSeperator->mSelectable = false;

		SubscribeToBroadcast(true);
		UpdateButtons();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::UpdateSeperator()
{
	mFriendsList->UpdateItem(mSeperator);
	int aSepPos = mFriendsList->GetItemPos(mSeperator);
	
	bool needSep = aSepPos>0 && aSepPos<mFriendsList->GetNumItems()-1;

	if(mSeperator->mIsVisible!=needSep)
	{
		mSeperator->mIsVisible = needSep;
		mFriendsList->UpdateItem(mSeperator);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::UpdateItem(LobbyFriend* theFriend, FriendsListItem *theItem)
{
	theItem->mFriend = theFriend;

	if(theFriend->IsOnline() && !theFriend->IsAnonymous())
		theItem->mSortValue = 2;
	else 
		theItem->mSortValue = 0;

	Image *anIcon = NULL;
	if (theFriend->IsSearching())
		anIcon = LobbyGlobal_Searching_Image;
	else if (theFriend->IsAnonymous())
		anIcon = FriendsListCtrl_Unknown_Image;
	else if (theFriend->InGame())
		anIcon = FriendsListCtrl_InGame_Image;
	else if (theFriend->InChat())
		anIcon = FriendsListCtrl_InChat_Image;
	else
		anIcon = FriendsListCtrl_Unknown_Image;

	theItem->mIcon = anIcon;
	theItem->mText = theFriend->GetName();

	if (!theFriend->IsOnline() || theFriend->IsAnonymous())
	{
		theItem->mColor = ColorScheme::GetColorRef(StandardColor_GrayText);
		theItem->mFont = GetDefaultFontMod(FontStyle_Italic);
	}
	else
	{
		theItem->mColor = ColorScheme::GetColorRef(StandardColor_Text);
		theItem->mFont = GetDefaultFont();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FriendsListItem* FriendsListCtrl::GetListItem(LobbyFriend *theFriend)
{
	for(int i=0; i<mFriendsList->GetNumItems(); i++)
	{
		FriendsListItem *anItem = (FriendsListItem*)mFriendsList->GetItem(i);
		if(anItem->mFriend.get()!=NULL && anItem->mFriend->GetWONUserId() == theFriend->GetWONUserId())
			return anItem;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyFriend* FriendsListCtrl::GetSelectedFriend()
{
	FriendsListItem *anItem = (FriendsListItem*)mFriendsList->GetSelItem();
	if(anItem!=NULL)
		return anItem->mFriend;
	else
		return NULL;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::HandleFriendChanged(FriendChangedEvent *theEvent)
{
	if(theEvent->IsAdd())
	{
		FriendsListItem *anOldItem = GetListItem(theEvent->mFriend);
		if(anOldItem!=NULL) // already in the list
			return;

		FriendsListItemPtr anItem = new FriendsListItem;
		UpdateItem(theEvent->mFriend,anItem);
		mFriendsList->InsertItem(anItem);
	}
	else if(theEvent->IsDelete())
	{
		FriendsListItem *anItem = GetListItem(theEvent->mFriend);
		if(anItem==NULL)
			return;

		mFriendsList->DeleteItem(anItem);
	}
	else // modify
	{
		FriendsListItem *anItem = GetListItem(theEvent->mFriend);
		if(anItem==NULL)
			return;

		UpdateItem(theEvent->mFriend,anItem);
		mFriendsList->UpdateItem(anItem);
	}

	UpdateButtons();
	UpdateSeperator();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::HandleSyncFriendsList()
{
	mFriendsList->BeginMultiChange();

	// Iterate through each of your friends to see if they are in the update list
	for (int row = 0; row < mFriendsList->GetNumItems(); row++)
	{
		// Is this a valid friend list item?
		FriendsListItem* anItem = (FriendsListItem*)mFriendsList->GetItem(row);
		if (anItem == NULL)
			continue;

		if(anItem->mFriend.get()!=NULL)
			UpdateItem(anItem->mFriend, anItem);
	}

	mFriendsList->EndMultiChange();

	UpdateButtons();
	UpdateSeperator();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::LoadFriendsList()
{
	mFriendsList->BeginMultiChange();
	mFriendsList->Clear();

	mFriendsList->InsertItem(mSeperator);

	const LobbyFriendMap &aMap = LobbyPersistentData::GetFriendMap();
	LobbyFriendMap::const_iterator anItr = aMap.begin();
	for(; anItr != aMap.end(); ++anItr)
	{
		FriendsListItemPtr anItem = new FriendsListItem;
		UpdateItem(anItr->second,anItem);
		mFriendsList->InsertItem(anItem);
	}

	mFriendsList->EndMultiChange();
	UpdateButtons();
	UpdateSeperator();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::UpdateButtons()
{
	ListItem* anItem = mFriendsList->GetSelItem();
	if (anItem == NULL)
	{
		mLocateButton->Enable(false);
		mRemoveButton->Enable(false);
	}
	else
	{
		mLocateButton->Enable(true);
		mRemoveButton->Enable(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FriendsListCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ButtonPressed:	
			if (theEvent->mComponent==mRemoveButton)
			{
				LobbyFriend *aFriend = GetSelectedFriend();
				if(aFriend!=NULL)
					FireEvent(new FriendActionEvent(LobbyFriendAction_Remove,aFriend));
			}
			else if (theEvent->mComponent==mLocateButton)
			{
				LobbyFriend *aFriend = GetSelectedFriend();
				if(aFriend!=NULL)				
					FireEvent(new FriendActionEvent(LobbyFriendAction_Locate,aFriend));
			}
			else
				break;

			return;
		case ComponentEvent_ListItemClicked:
			{
				ListItemClickedEvent* aListItemClickedEvent = ((ListItemClickedEvent*)theEvent);
				FriendsListItem *anItem = (FriendsListItem*)mFriendsList->GetSelItem();
				if(anItem==NULL || anItem!=aListItemClickedEvent->mItem || anItem->mFriend.get()==NULL)
					return;

				if (aListItemClickedEvent->mButton == MouseButton_Right)
					FireEvent(new FriendActionEvent(LobbyFriendAction_ShowMenuTryClient,anItem->mFriend));
				else if (aListItemClickedEvent->mButton == MouseButton_Left && aListItemClickedEvent->mWasDoubleClick)
					FireEvent(new FriendActionEvent(LobbyFriendAction_Locate,anItem->mFriend)); 
			}
			return;

		case ComponentEvent_ListSelChanged: UpdateButtons(); return;
		case LobbyEvent_EnteredScreen:		LoadFriendsList(); return;
		case LobbyEvent_SyncFriendsList:	HandleSyncFriendsList(); return;
		case LobbyEvent_FriendChanged:		HandleFriendChanged((FriendChangedEvent*)theEvent); return;		
	}

	Container::HandleComponentEvent(theEvent);
}