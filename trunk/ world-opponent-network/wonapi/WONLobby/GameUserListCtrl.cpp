#include "GameUserListCtrl.h"

#include "LobbyClient.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyResource.h"
#include "LobbyStaging.h"
#include "PingPainter.h"

#include "WONGUI/MultiListBox.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{

class GameUserListItem : public ListItem
{
public:
	enum GameUserListItem_Column
	{
		Column_CaptainIcon,
		Column_ReadyIcon,
		Column_Name,
		Column_Ping
	};

	LobbyClientPtr mClient;
	ImagePtr mIcon;
	FontPtr mFont;
	
public:
	GameUserListItem(LobbyClient *theClient);
	void UpdateIcon();

	virtual void CalcDimensions(ListArea *theListArea);
	virtual int Compare(const ListItem *theItem, ListArea *theListArea) const;
	virtual void Paint(Graphics &g, ListArea *theListArea);
};
typedef SmartPtr<GameUserListItem> GameUserListItemPtr;

} // namespace WONAPI

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameUserListItem::GameUserListItem(LobbyClient *theClient) : mClient(theClient)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListItem::UpdateIcon()
{
	if(mClient->IsIgnored())
		mIcon = UserListCtrl_Ignored_Image;
	else if (mClient->IsBlocked())
		mIcon = UserListCtrl_Blocked_Image;
	else if(mClient->IsMuted(true))
		mIcon = UserListCtrl_Muted_Image;
	else if(mClient->IsAway())
		mIcon = UserListCtrl_Away_Image;
	else if(mClient->IsAdmin())
		mIcon = UserListCtrl_Admin_Image;
	else if(mClient->IsModerator())
		mIcon = UserListCtrl_Moderator_Image;
	else if(mClient->IsCaptain(true))
		mIcon = UserListCtrl_Captain_Image;
	else
		mIcon = UserListCtrl_Normal_Image;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListItem::CalcDimensions(ListArea *theListArea)
{
	mFont = theListArea->GetFont();
	mHeight = mFont->GetHeight();
	if(mIcon.get()!=NULL && mIcon->GetHeight()>mHeight)
		mHeight = mIcon->GetHeight();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GameUserListItem::Compare(const ListItem *theItem, ListArea *theListArea) const
{
	GameUserListItem* anItem = (GameUserListItem*)theItem;
	int c1 = mClient->IsCaptain(true)?1:0;
	int c2 = anItem->mClient->IsCaptain(true)?1:0;
	if(c1!=c2)
		return c2-c1; // captain should sort above everyone else
	else
		return mClient->GetName().compareNoCase(anItem->mClient->GetName());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListItem::Paint(Graphics &g, ListArea *theListArea)
{
	MultiListArea *aListArea = (MultiListArea*)theListArea;
	int aPaintCol = aListArea->GetColumnPaintContext();
	int aPaintColWidth = aListArea->GetPaintColumnWidth();
	int aColor = g.GetColorSchemeColor(StandardColor_Text);
	if(mSelected)
		aColor = aListArea->GetSelColor().GetForegroundColor(g,aColor);

	g.SetColor(aColor);
	g.SetFont(mFont);

	switch(aPaintCol)
	{
		case -1:
			if(mSelected)
			{
				g.SetColor(aListArea->GetSelColor().GetBackgroundColor(g));
				g.FillRect(0,0,aListArea->GetPaintColumnWidth(),mHeight);
			}
			break;

		case Column_CaptainIcon:
			g.DrawImage(mIcon,0,(mHeight-mIcon->GetHeight())/2);
			break;

		case Column_ReadyIcon:
			if (mClient->IsPlayerReady())
				g.DrawImage(GameUserListCtrl_Ready_Image,0,(mHeight-mIcon->GetHeight())/2);
			break;

		case Column_Name:
			g.SetFont(mFont);
			g.DrawString(mClient->GetName(),0,(mHeight-mFont->GetHeight())/2);
			break;

		case Column_Ping:
			LobbyPaintPingBars(g,mClient->GetPlayerPing(),mHeight-2);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameUserListCtrl::GameUserListCtrl()
{
	mRoomSpecFlags = LobbyRoomSpecFlag_Game;
	mInGame = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::InitComponent(ComponentInit &theInit)
{
	Container::InitComponent(theInit);
	if(theInit.mType==ComponentInitType_ComponentConfig)
	{
		ComponentConfigInit &anInit = (ComponentConfigInit&)theInit;
		ComponentConfig *aConfig = anInit.mConfig;

		WONComponentConfig_Get(aConfig,mUserList,"UserList");

		mKickButton = dynamic_cast<Button*>(aConfig->GetChildComponent("KickButton"));
		if(mKickButton.get()!=NULL)
			mKickButton->Enable(false);
	
		SubscribeToBroadcast(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyClient* GameUserListCtrl::GetSelectedClient()
{
	GameUserListItem *anItem = (GameUserListItem*)mUserList->GetSelItem();
	if(anItem==NULL)
		return NULL;
	else
		return anItem->mClient;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameUserListItem* GameUserListCtrl::GetListItem(LobbyClient *theClient)
{
	for(int i=0; i<mUserList->GetNumItems(); i++)
	{
		GameUserListItem *anItem = (GameUserListItem*)mUserList->GetItem(i);
		if(anItem->mClient.get()!=NULL && anItem->mClient->GetClientId()==theClient->GetClientId())
			return anItem;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::HandleKickPlayer(bool isBan)
{
	LobbyClient *aClient = GetSelectedClient();
	if(aClient==NULL)
		return;

	LobbyStaging::KickClient(aClient,isBan);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::UpdateKickButton()
{
	if(!mInGame || mKickButton.get()==NULL)
		return;

	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL)
		return;

	if(aGame->IAmCaptain())
	{
		LobbyClient* aClient = GetSelectedClient();
		if(aClient==NULL || aClient->GetClientId()==aGame->GetCaptainId())
			mKickButton->Enable(false);
		else
			mKickButton->Enable(true);
	}
	else
		mKickButton->Enable(false);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::HandleClientChanged(ClientChangedEvent *theEvent)
{
	if(!mInGame)
		return;

	if(!theEvent->mRoomSpec.Allows(mRoomSpecFlags))
		return;


	if(theEvent->IsModify()) 
	{
		GameUserListItem *anItem = GetListItem(theEvent->mClient);
		if(anItem==NULL)
			return;

		anItem->UpdateIcon();
		mUserList->ListArea::UpdateItem(anItem);
	}
	else
		return;

	UpdateKickButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::HandlePlayerChanged(PlayerChangedEvent *theEvent)
{
	if(!mInGame)
		return;

	if(theEvent->IsAdd())
	{
		GameUserListItemPtr anItem = new GameUserListItem(theEvent->mClient);
		anItem->UpdateIcon();
		mUserList->ListArea::InsertItem(anItem);
	
	}
	else if(theEvent->IsDelete())
	{
		GameUserListItem *anItem = GetListItem(theEvent->mClient);
		if(anItem==NULL)
			return;

		mUserList->ListArea::DeleteItem(anItem);
	}	
	else // modify
	{
		GameUserListItem *anItem = GetListItem(theEvent->mClient);
		if(anItem==NULL)
			return;

		anItem->UpdateIcon();
		mUserList->ListArea::UpdateItem(anItem);
	}
		return;

	UpdateKickButton();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::HandleJoinedGame()
{
	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL)
		return;

	if(!aGame->CheckRoomSpecFlags(mRoomSpecFlags))
		return;


	LobbyClientList *aList = aGame->GetClientList();
	if(aList==NULL)
		return;

	if(mKickButton.get()!=NULL)
	{
		if(mRoomSpecFlags&LobbyRoomSpecFlag_Internet)
			mKickButton->SetText(GameUserListCtrl_Ban_String);
		else
			mKickButton->SetText(GameUserListCtrl_Kick_String);
	}

	mUserList->BeginMultiChange();
	mUserList->Clear();

	const LobbyClientMap &aMap = aList->GetClientMap();
	for(LobbyClientMap::const_iterator anItr = aMap.begin(); anItr!=aMap.end(); ++anItr)
	{
		LobbyClient *aClient = anItr->second;
		if(aClient->IsPlayer())
		{
			GameUserListItemPtr anItem = new GameUserListItem(aClient);
			anItem->UpdateIcon();
			mUserList->ListArea::InsertItem(anItem);
		}
	}
	
	mUserList->EndMultiChange();
	mInGame = true;
	UpdateKickButton();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::HandleLeftGame()
{
	if(!mInGame)
		return;

	mInGame = false;
	mUserList->Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameUserListCtrl::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_JoinedGame: HandleJoinedGame(); return;
		case LobbyEvent_LeftGame: HandleLeftGame(); return;
		case LobbyEvent_ClientChanged: HandleClientChanged((ClientChangedEvent*)theEvent); return;
		case LobbyEvent_PlayerChanged: HandlePlayerChanged((PlayerChangedEvent*)theEvent); return;
		case LobbyEvent_SetRoomSpecFlags:
			mRoomSpecFlags |= ((SetRoomSpecFlagsEvent*)theEvent)->mSpecFlags;
			return;

		case ComponentEvent_ButtonPressed:
			if (theEvent->mComponent==mKickButton)
				HandleKickPlayer(mRoomSpecFlags&LobbyRoomSpecFlag_Internet?true:false); 
			else
				break;

			return;

		case ComponentEvent_ListSelChanged: UpdateKickButton(); return;

		case ComponentEvent_ListItemClicked:
		{
			LobbyClient *aClient = GetSelectedClient();
			if(aClient!=NULL)
			{
				ListItemClickedEvent* aListItemClickedEvent = ((ListItemClickedEvent*)theEvent);
				if(aListItemClickedEvent->IsSelectedItem())
				{
					if (aListItemClickedEvent->mButton == MouseButton_Right)
						FireEvent(new ClientActionEvent(LobbyClientAction_ShowMenu,mRoomSpecFlags,aClient));
					else if (aListItemClickedEvent->mButton == MouseButton_Left && aListItemClickedEvent->mWasDoubleClick)
						LobbyEvent::BroadcastEvent(new SetChatInputEvent(LobbyChatCommand_Whisper,aClient->GetName(),mRoomSpecFlags));
				}
			}
		}
		return;
	}

	Container::HandleComponentEvent(theEvent);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GameUserListCtrlConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="CTRLLINKID")
	{} //mUserList->SetCtrlLinkId((CtrlLinkId)ReadInt());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}
