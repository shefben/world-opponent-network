#include "UserListCtrl.h"

#include "LobbyClient.h"
#include "LobbyEvent.h"
#include "LobbyResource.h"

#include "WONGUI/Button.h"
#include "WONGUI/ListBox.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{

class UserListItem : public TextListItem
{
public:
	LobbyClientPtr mClient;
	char mSortValue;
	bool mIsVisible;

public:
	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const; 
	virtual void CalcDimensions(ListArea *theListArea);

	UserListItem();
};
typedef SmartPtr<UserListItem> UserListItemPtr;

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UserListItem::UserListItem()
{
	mSortValue = 0;
	mIsVisible = true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int UserListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	int aDiff = ((UserListItem*)theItem)->mSortValue - mSortValue;
	if(aDiff==0)
		return TextListItem::Compare(theItem,theListArea);
	else
		return aDiff;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListItem::CalcDimensions(ListArea *theListArea)
{
	if(mClient.get()==NULL)
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
void UserListItem::Paint(Graphics &g, ListArea *theListArea)
{
	if (mClient.get()==NULL)  // seperator
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
UserListCtrl::UserListCtrl()
{
	mRoomSpecFlags = LobbyRoomSpecFlag_Chat | LobbyRoomSpecFlag_Internet;
	mActive = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UserListCtrl::~UserListCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static Image* GetUserIcon(LobbyClient *theClient, bool checkGame)
{
	if(theClient==NULL)
		return NULL;

	if(theClient->IsIgnored())
		return UserListCtrl_Ignored_Image;
	else if (theClient->IsBlocked())
		return UserListCtrl_Blocked_Image;
	else if(theClient->IsMuted(checkGame))
		return UserListCtrl_Muted_Image;
	else if(theClient->IsAway())
		return UserListCtrl_Away_Image;
	else if(theClient->IsAdmin())
		return UserListCtrl_Admin_Image;
	else if(theClient->IsModerator())
		return UserListCtrl_Moderator_Image;
	else if(theClient->IsCaptain(checkGame))
		return UserListCtrl_Captain_Image;
	else
		return UserListCtrl_Normal_Image;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::UpdateItem(LobbyClient *theClient, UserListItem *theItem)
{
	theItem->mClient = theClient;

	char aSortValue = 0;
	if(theClient->IsAdmin())
		aSortValue = 5;
	else if(theClient->IsModerator())
		aSortValue = 4;
	else if(theClient->IsFriend() && !theClient->IsAnonymous())
		aSortValue = 2;
	else 
		aSortValue = 0;	
	theItem->mSortValue = aSortValue;

	theItem->mText = theClient->GetName();
	theItem->mIcon = GetUserIcon(theClient,mRoomSpecFlags&LobbyRoomSpecFlag_Game?true:false);

	if (theClient->IsFriend() && !theClient->IsAnonymous())
		theItem->mFont = GetDefaultFontMod(FontStyle_Bold);
	else
		theItem->mFont = GetDefaultFont();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::UpdateSeperators()
{
	mUserList->UpdateItem(mSep1);
	mUserList->UpdateItem(mSep2);
	int aSep1Pos = mUserList->GetItemPos(mSep1);
	int aSep2Pos = mUserList->GetItemPos(mSep2);

	bool haveAdmins = aSep1Pos > 0;
	bool haveFriends = aSep2Pos > aSep1Pos+1;
	bool haveClients = aSep2Pos < mUserList->GetNumItems()-1;

	bool needSep1 = haveAdmins && (haveFriends || haveClients);
	bool needSep2 = haveFriends && haveClients;

	if(mSep1->mIsVisible!=needSep1)
	{
		mSep1->mIsVisible = needSep1;
		mUserList->UpdateItem(mSep1);
	}
	if(mSep2->mIsVisible!=needSep2)
	{
		mSep2->mIsVisible = needSep2;
		mUserList->UpdateItem(mSep2);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mUserList,"UserList");
		WONComponentConfig_Get(aConfig,mWhisperButton,"WhisperButton");
		WONComponentConfig_Get(aConfig,mIgnoreButton,"IgnoreButton");
	
		SubscribeToBroadcast(true);

		mSep1 = new UserListItem;
		mSep1->mSelectable = false;
		mSep1->mSortValue = 3;
		mSep2 = new UserListItem;
		mSep2->mSelectable = false;
		mSep2->mSortValue = 1;

		UpdateIgnoreButton();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UserListItem* UserListCtrl::GetListItem(LobbyClient *theClient)
{
	for(int i=0; i<mUserList->GetNumItems(); i++)
	{
		UserListItem *anItem = (UserListItem*)mUserList->GetItem(i);
		if(anItem->mClient.get()!=NULL && anItem->mClient->GetClientId()==theClient->GetClientId())
			return anItem;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UserListItem* UserListCtrl::GetSelectedItem()
{
	return (UserListItem*)mUserList->GetSelItem();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* UserListCtrl::GetSelectedClient()
{
	UserListItem *anItem = (UserListItem*)mUserList->GetSelItem();
	if(anItem==NULL)
		return NULL;
	else
		return anItem->mClient;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::UpdateIgnoreButton()
{
	LobbyClient* aClient = GetSelectedClient();
	if (aClient == NULL)
	{
		mIgnoreButton->SetText(UserListCtrl_Ignore_String);
		mIgnoreButton->Enable(false);
		mWhisperButton->Enable(false);
	}
	else
	{
		mWhisperButton->Enable(true);
		mIgnoreButton->Enable(true);
		if (aClient->IsIgnored())
			mIgnoreButton->SetText(UserListCtrl_Unignore_String);
		else 
			mIgnoreButton->SetText(UserListCtrl_Ignore_String);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::HandleJoinedRoom(JoinedRoomEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	mActive = true;
	mUserList->BeginMultiChange();
	mUserList->Clear();
	mUserList->InsertItem(mSep1);
	mUserList->InsertItem(mSep2);

	const LobbyClientMap &aMap = theEvent->mClientList->GetClientMap();
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
	{
		LobbyClient *aClient = anItr->second;
		UserListItemPtr anItem = new UserListItem;
		UpdateItem(aClient,anItem);
		mUserList->InsertItem(anItem);
	}

	mUserList->EndMultiChange();
	UpdateIgnoreButton();
	UpdateSeperators();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::HandleLeftRoom(LeftRoomEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	mActive = false;
	mUserList->Clear();
	UpdateIgnoreButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::HandleClientChanged(ClientChangedEvent *theEvent)
{
	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;

	if(theEvent->IsAdd())
	{
		UserListItemPtr anItem = new UserListItem;
		UpdateItem(theEvent->mClient,anItem);
		mUserList->InsertItem(anItem);
	}
	else if(theEvent->IsDelete())
	{
		UserListItem *anItem = GetListItem(theEvent->mClient);
		if(anItem==NULL)
			return;

		mUserList->DeleteItem(anItem);
	}
	else // modify
	{
		UserListItem *anItem = GetListItem(theEvent->mClient);
		if(anItem==NULL)
			return;

		UpdateItem(theEvent->mClient,anItem);
		mUserList->UpdateItem(anItem);
	}

	UpdateIgnoreButton();
	UpdateSeperators();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UserListCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ButtonPressed:	
			if (theEvent->mComponent==mIgnoreButton)
			{
				LobbyClient *aClient = GetSelectedClient();
				if(aClient!=NULL)
					FireEvent(new ClientActionEvent(aClient->IsIgnored()?LobbyClientAction_Unignore:LobbyClientAction_Ignore,mRoomSpecFlags,aClient));
			}
			else if (theEvent->mComponent==mWhisperButton)
			{
				LobbyClient *aClient = GetSelectedClient();
				if(aClient!=NULL)
					LobbyEvent::BroadcastEvent(new SetChatInputEvent(LobbyChatCommand_Whisper,aClient->GetName(),mRoomSpecFlags));
			}
			return;
		case ComponentEvent_ListSelChanged:
				UpdateIgnoreButton();
			return;
		case ComponentEvent_ListItemClicked:
			{
				LobbyClient *aClient = GetSelectedClient();
				if(aClient!=NULL)
				{
					ListItemClickedEvent* aListItemClickedEvent = ((ListItemClickedEvent*)theEvent);
					if(GetSelectedItem()==aListItemClickedEvent->mItem)
					{
						if (aListItemClickedEvent->mButton == MouseButton_Right)
							FireEvent(new ClientActionEvent(LobbyClientAction_ShowMenu,mRoomSpecFlags,aClient));
						else if (aListItemClickedEvent->mButton == MouseButton_Left && aListItemClickedEvent->mWasDoubleClick)
							LobbyEvent::BroadcastEvent(new SetChatInputEvent(LobbyChatCommand_Whisper,aClient->GetName(),mRoomSpecFlags));
					}
				}
			}
			return;


		case LobbyEvent_JoinedRoom:			HandleJoinedRoom((JoinedRoomEvent*)theEvent); return;
		case LobbyEvent_LeftRoom:			HandleLeftRoom((LeftRoomEvent*)theEvent); return;
		case LobbyEvent_ClientChanged:		HandleClientChanged((ClientChangedEvent*)theEvent); return;
		
	}

	Container::HandleComponentEvent(theEvent);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool UserListCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="GROUPTYPE")
	{
/*		GUIString aStr = ReadString();
		if(aStr.compareNoCase("CHAT")==0)
			mUserList->SetGroupType(GroupType_Chat);
		else if(aStr.compareNoCase("GAME")==0)
			mUserList->SetGroupType(GroupType_Game);
		else
			throw ConfigObjectException("Invalid Group Type: " + (std::string)aStr);*/
	}
	else if(theInstruction=="CTRLLINKID")
	{} //mUserList->SetCtrlLinkId((CtrlLinkId)ReadInt());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}
