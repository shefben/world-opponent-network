#include "RoomCtrl.h"

#include "LobbyEvent.h"
#include "LobbyGroup.h"
#include "LobbyMisc.h"
#include "LobbyResource.h"
#include "LobbyServer.h"

#include "WONGUI/Button.h"
#include "WONGUI/GUIConfig.h"
#include "WONGUI/TreeCtrl.h"

using namespace WONAPI;
		
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoomListItem : public ListItem
{
public:
	int mColor;
	bool mIsQuerying;
	ImagePtr mIcon;
	FontPtr mFont;
	GUIString mText;
	IPAddr mServerAddr;
	int mGroupId;

	virtual void Paint(Graphics &g, ListArea *theListArea);
	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;


public:
	void SetState(LobbyServer *theServer, LobbyGroup *theGroup);

	RoomListItem(LobbyServer *theServer, bool isQuerying = false);
	RoomListItem(LobbyGroup *theGroup, LobbyServer *theServer);
};
typedef SmartPtr<RoomListItem> RoomListItemPtr;



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoomListItem::RoomListItem(LobbyServer *theServer, bool isQuerying) 
: mServerAddr(theServer->GetIPAddr()), mGroupId(LobbyServerId_Invalid), mColor(-1), mIsQuerying(isQuerying) 
{ 
	SetState(theServer,NULL); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoomListItem::RoomListItem(LobbyGroup *theGroup, LobbyServer *theServer) 
	: mServerAddr(theServer->GetIPAddr()), mGroupId(theGroup->GetGroupId()), mColor(-1), mIsQuerying(false) 
{ 
	SetState(theServer,theGroup); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomListItem::SetState(LobbyServer *theServer, LobbyGroup *theGroup)
{
	char aBuf[50];
	if(mIsQuerying)
		mText = RoomCtrl_QueryingServer_String;
	else if(theGroup!=NULL)
	{
		sprintf(aBuf," (%d)",theGroup->GetNumUsers());
		mText = theGroup->GetName();
	
		if(theGroup->GetNumUsers()>0)
			mText.append(aBuf);
	}	
	else if(theServer!=NULL)
	{
		sprintf(aBuf," (%d)",theServer->GetNumUsers());
		mText = theServer->GetName();
		if(theServer->GetNumUsers())
			mText.append(aBuf);
	}
	else
		mText.erase();

	if(theGroup!=NULL && theGroup->IsPasswordProtected())
		mIcon = RoomCtrl_Password_Image;
	else
		mIcon = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomListItem::CalcDimensions(ListArea *theListArea)
{
	if(mFont.get()==NULL)
	{
		if(mGroupId!=LobbyServerId_Invalid || mIsQuerying)
			mFont = theListArea->GetFont();
		else
			mFont = theListArea->Component::GetFontMod(theListArea->GetFont(),FontStyle_Bold);
	}

	if(mColor<0)
		mColor = theListArea->GetTextColor();

	mWidth = mFont->GetStringWidth(mText) + 2;
	mHeight = mFont->GetHeight();
	if(mIcon.get()!=NULL)
	{
		mWidth += mIcon->GetWidth() + 2;
		if(mIcon->GetHeight() > mHeight)
			mHeight = mIcon->GetHeight();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int RoomListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	return mText.compareNoCase(((RoomListItem*)theItem)->mText);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomListItem::Paint(Graphics &g, ListArea *theListArea)
{
	int aColor = mColor;

	if(mSelected)
	{
		g.SetColor(theListArea->GetSelColor().GetBackgroundColor(g));
		int aWidth = theListArea->GetPaintColumnWidth();
		if(!theListArea->ListFlagSet(ListFlag_FullRowSelect))
			aWidth = mWidth;

		int x = 0;
		g.FillRect(x,0,aWidth,mHeight);
		aColor = theListArea->GetSelColor().GetForegroundColor(g,mColor);
	}

	int x = 2;
	if(mIcon.get()!=NULL)
	{
		g.DrawImage(mIcon,x,(mHeight - mIcon->GetHeight())/2);
		x+=mIcon->GetWidth()+2;		
	}

	g.SetColor(aColor);
	g.SetFont(mFont);
	g.DrawString(mText, x, (mHeight - mFont->GetHeight())/2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoomCtrl::RoomCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mRoomList,"RoomList");
		WONComponentConfig_Get(aConfig,mJoinButton,"JoinButton");
		WONComponentConfig_Get(aConfig,mCreateButton,"CreateButton");

		SubscribeToBroadcast(true);

		EnableControls();
	}
}

///////////////////////////////////////////////////////////////////////////////
// EnableControls: Enable or disable controls that cannot function until 
// something else happens.
//
// Join Room Button: You cannot join a room until a server or specific room 
// has been selected.
//
// Create Room Button: There is no need to check this button.  If the user 
// clicks it, they will get a room on a server, somewhere.
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::EnableControls()
{
	TreeItem *aTreeItem = mRoomList->GetSelItem();
	mJoinButton->Enable(aTreeItem!=NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::AddGroup(TreeItem *theServerItem, LobbyServer *theServer, LobbyGroup *theGroup)
{
	mRoomList->InsertItem(theServerItem,new RoomListItem(theGroup,theServer),-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::AddServer(LobbyServer *theServer, bool isOpen, int theInsertPos)
{
	mRoomList->BeginMultiChange();

	TreeItem *anItem = mRoomList->InsertItem(NULL, new RoomListItem(theServer), theInsertPos);
	const LobbyGroupMap &aGroupMap = theServer->GetGroupMap();
	LobbyGroupMap::const_iterator aGroupItr = aGroupMap.begin();
	while(aGroupItr!=aGroupMap.end())
	{
		LobbyGroup *aGroup = aGroupItr->second;
		if(aGroup->IsChatRoom())
			AddGroup(anItem, theServer, aGroup);

		++aGroupItr;
	}	

	if(!theServer->IsConnected() && aGroupMap.empty())
	{
		mRoomList->DeleteAllChildren(anItem);
		TreeItem *aChild = mRoomList->InsertItem(anItem,new RoomListItem(theServer,true), 0);
		aChild->mSelectable = false;
	}

	if(isOpen)
		mRoomList->OpenItem(anItem);

	mRoomList->EndMultiChange();

	EnableControls();
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::HandleSetRoomList()
{
	LobbyServerList *aList = LobbyMisc::GetLobbyServerList();
	if(aList==NULL)
		return;

	mRoomList->BeginMultiChange();
	mRoomList->Clear();

	const LobbyServerMap &aServerMap = aList->GetServerMap();
	LobbyServerMap::const_iterator aServerItr = aServerMap.begin();
	while(aServerItr!=aServerMap.end())
	{
		LobbyServer *aServer = aServerItr->second;
		AddServer(aServer);
		++aServerItr;
	}

	mRoomList->EndMultiChange();

	LobbyGroup *aChatGroup = LobbyMisc::GetChatGroup();
	LobbyServer *aChatServer = LobbyMisc::GetChatServer();
	if(aChatServer!=NULL) // Hilight current chat group
	{
		TreeItem* aServerItem = GetServerItem(aChatServer);
		if(aServerItem!=NULL)
		{
			mRoomList->OpenItem(aServerItem,false);
			if(aChatGroup!=NULL)
			{
				TreeItem* aGroupItem = GetGroupItem(aChatServer,aChatGroup);
				if(aGroupItem!=NULL)
				{
					mRoomList->SetSelItem(mRoomList->GetItemPos(aGroupItem));
					mRoomList->EnsureVisible(aGroupItem);
				}
			}
			else // just select the server
			{
				mRoomList->SetSelItem(mRoomList->GetItemPos(aServerItem));
				mRoomList->EnsureVisible(aServerItem);
			}

		}
	}

	EnableControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::DoJoinRoom()
{
	TreeItem *aTreeItem = mRoomList->GetSelItem();
	if(aTreeItem==NULL)
		return;

	RoomListItem *aRoomItem = (RoomListItem*)aTreeItem->GetItem();
	FireEvent(new JoinRoomEvent(aRoomItem->mServerAddr,aRoomItem->mGroupId));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::DoCreateRoom()
{
	IPAddr aServerAddr;

	TreeItem *aTreeItem = mRoomList->GetSelItem();
	if(aTreeItem!=NULL)
	{
		RoomListItem *aRoomItem = (RoomListItem*)aTreeItem->GetItem();
		aServerAddr = aRoomItem->mServerAddr;
	}

	FireEvent(new CreateRoomEvent(aServerAddr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* RoomCtrl::GetServerItem(const IPAddr &theAddr)
{
	for(int i=0; i<mRoomList->GetNumItems(); i++)
	{
		TreeItem *anItem = mRoomList->GetItem(i);
		RoomListItem *aRoomItem = (RoomListItem*)anItem->GetItem();
		if(aRoomItem->mServerAddr==theAddr)
			return anItem;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* RoomCtrl::GetGroupItem(const IPAddr &theAddr, int theId)
{
	TreeItem *aServerItem = GetServerItem(theAddr);
	if(aServerItem==NULL)
		return NULL;

	for(int i=0; i<aServerItem->GetNumChildren(); i++)
	{
		TreeItem *anItem = aServerItem->GetChild(i);
		RoomListItem *aRoomItem = (RoomListItem*)anItem->GetItem();
		if(aRoomItem->mGroupId==theId)
			return anItem;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* RoomCtrl::GetServerItem(LobbyServer *theServer)
{
	if(theServer!=NULL)
		return GetServerItem(theServer->GetIPAddr());
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TreeItem* RoomCtrl::GetGroupItem(LobbyServer *theServer, LobbyGroup *theGroup)
{
	if(theServer!=NULL && theGroup!=NULL)
		return GetGroupItem(theServer->GetIPAddr(), theGroup->GetGroupId());
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::HandleServerChanged(ServerChangedEvent *theEvent)
{
	if(theEvent->IsAdd())
	{
		AddServer(theEvent->mServer);
		return;
	}
	else if(theEvent->IsDelete())
	{
		TreeItem *anItem = GetServerItem(theEvent->mServer);
		if(anItem!=NULL)
			mRoomList->DeleteItem(anItem);

		EnableControls();
		return;
	}
	
	// modification
	TreeItem *aServerItem = GetServerItem(theEvent->mServer);
	if(aServerItem==NULL)
		return;

	bool isOpen = aServerItem->IsOpen();
	TreeItem *aSelItem = mRoomList->GetSelItem();
	int aSelGroupId = LobbyServerId_Invalid;
	IPAddr aSelServerAddr;
	if(aSelItem!=NULL && aSelItem->GetParent()==aServerItem)
	{
		aSelGroupId = ((RoomListItem*)aSelItem->GetItem())->mGroupId;
		aSelServerAddr = ((RoomListItem*)aSelItem->GetItem())->mServerAddr;
	}

	int aVertOffset = mRoomList->GetVertOffset();
	int anItemPos = aServerItem->GetPos();
	mRoomList->BeginMultiChange();
	mRoomList->DeleteItem(aServerItem);
	AddServer(theEvent->mServer, isOpen, anItemPos);

	if(aSelGroupId!=LobbyServerId_Invalid && aSelServerAddr.IsValid())
	{
		aSelItem = GetGroupItem(aSelServerAddr,aSelGroupId);
		if(aSelItem!=NULL)
			mRoomList->SetSelItem(mRoomList->GetItemPos(aSelItem));
	}
	
	mRoomList->EndMultiChange();
	mRoomList->SetVertOffset(aVertOffset);
	EnableControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::HandleGroupChanged(GroupChangedEvent *theEvent)
{
	// Only interested in chat rooms
	LobbyGroup *aGroup = theEvent->mGroup;
	LobbyServer *aServer = theEvent->mServer;
	if(aGroup==NULL || aServer==NULL)
		return;

	if (!aGroup->IsChatRoom())
		return;

	if(theEvent->IsAdd())
	{
		TreeItem *anItem = GetServerItem(aServer);
		if(anItem!=NULL)
			AddGroup(anItem,aServer,aGroup);
	}
	else if(theEvent->IsDelete())
	{
		TreeItem *anItem = GetGroupItem(aServer,aGroup);
		mRoomList->DeleteItem(anItem);
	}
	else // modification
	{
		TreeItem *anItem = GetGroupItem(aServer,aGroup);
		if(anItem!=NULL)
		{
			((RoomListItem*)(anItem->GetItem()))->SetState(aServer,aGroup);
			((RoomListItem*)(anItem->GetParent()->GetItem()))->SetState(aServer,NULL);

			mRoomList->UpdateItem(anItem->GetParent());
			mRoomList->UpdateItem(anItem);
		}
	}

	EnableControls();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::HandleItemOpened(TreeItemOpenedEvent *theEvent)
{
	TreeItem *anItem = theEvent->mItem;
	if(!anItem->IsOpen())
		return;

	RoomListItem *aRoomItem = (RoomListItem*)anItem->GetItem();
	FireEvent(new QueryServerGroupsEvent(aRoomItem->mServerAddr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoomCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ButtonPressed:
			if(theEvent->mComponent==mJoinButton) 
				DoJoinRoom(); 
			else if(theEvent->mComponent==mCreateButton) 
				DoCreateRoom(); 
			else
				break;

			return;


		case ComponentEvent_TreeItemOpened: HandleItemOpened((TreeItemOpenedEvent*)theEvent); return;
		case ComponentEvent_ListItemClicked:
		{
			EnableControls();
			ListItemClickedEvent *anEvent = (ListItemClickedEvent*)theEvent;
			if(anEvent->mWasDoubleClick && anEvent->mComponent==mRoomList && anEvent->IsSelectedItem())
			{
				TreeItem *anItem = (TreeItem*)anEvent->mItem.get();
				if(anItem->IsInItem(anEvent->mItemX,anEvent->mItemY))
					DoJoinRoom();
			}
			return;
		}

		case LobbyEvent_SetRoomList: HandleSetRoomList(); return;
		case LobbyEvent_ServerChanged: HandleServerChanged((ServerChangedEvent*)theEvent); return;
		case LobbyEvent_GroupChanged: HandleGroupChanged((GroupChangedEvent*)theEvent); return;
	}

	Container::HandleComponentEvent(theEvent);
}
