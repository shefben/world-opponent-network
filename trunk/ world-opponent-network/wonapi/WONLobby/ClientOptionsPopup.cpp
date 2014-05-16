#include "ClientOptionsPopup.h"

#include "LobbyClient.h"
#include "LobbyEvent.h"
#include "LobbyGroup.h"
#include "LobbyResource.h"

#include "WONGUI/MenuBar.h"
#include "WONGUI/Window.h"


using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ClientOptionsPopup::ClientOptionsPopup()
{
	mPopupMenu = new PopupMenu;
	AddChild(mPopupMenu);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ClientOptionsPopup::~ClientOptionsPopup()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ClientOptionsPopup::AddItem(const GUIString &theStr, LobbyClientAction theAction)
{
	mPopupMenu->AddItem(theStr,NULL,NULL,theAction);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ClientOptionsPopup::Init(LobbyClient *theClient, LobbyClient *myClient, int theRoomSpecFlags)
{
	if(theClient==NULL || myClient==NULL)
		return;

	mClient = theClient;
	mRoomSpecFlags = theRoomSpecFlags;

	mPopupMenu->Clear(false);

	
	bool isGame = theRoomSpecFlags&LobbyRoomSpecFlag_Game?true:false;
	bool isInternet = theRoomSpecFlags&LobbyRoomSpecFlag_Internet?true:false;
	bool amModerator = myClient->IsModerator();
	bool amCaptain = myClient->IsCaptain(isGame);
	bool isMe = myClient->GetClientId() == theClient->GetClientId();

	FontPtr aNormalFont = mPopupMenu->GetFont();
	FontPtr aBoldFont = GetFontMod(aNormalFont,FontStyle_Bold);

	mPopupMenu->SetFont(aBoldFont);
	MenuItem *anItem = mPopupMenu->AddItem(theClient->GetName());
	anItem->mSelectable = false;

	if (amModerator)
	{
		char clientIdString[255];
		sprintf(clientIdString, "%d", theClient->GetWONUserId());

		MenuItem *anItem = mPopupMenu->AddItem(clientIdString);
		anItem->mSelectable = false;
	}

	mPopupMenu->SetFont(aNormalFont);

	AddItem(ClientOptionsCtrl_Whisper_String, LobbyClientAction_Whisper);
	if(!isMe && myClient->IsCaptain(true) && theClient->GetGameGroupId()!=myClient->GetGameGroupId()) // add invite command
	{
		if (!mClient->IsInvited())
			AddItem(ClientOptionsCtrl_Invite_String,LobbyClientAction_Invite);
		else
			AddItem(ClientOptionsCtrl_Uninvite_String,LobbyClientAction_Uninvite);
	}

	if(isInternet)
	{
		if(!isMe)
			AddItem(FriendOptionsCtrl_Locate_String,LobbyClientAction_Locate);

		if (!theClient->IsFriend())
			AddItem(ClientOptionsCtrl_AddToFriends_String,LobbyClientAction_AddToFriends);
		else
			AddItem(ClientOptionsCtrl_RemoveFromFriends_String,LobbyClientAction_RemoveFromFriends);
	}

	if(isInternet)
		mPopupMenu->AddSeperator();

	if(isMe && isInternet)
	{
		if (theClient->IsAway()) 
			AddItem(ClientOptionsCtrl_ClearAFK_String,LobbyClientAction_ClearAFK);
		else
			AddItem(ClientOptionsCtrl_SetAFK_String,LobbyClientAction_SetAFK);
	}
	else
	{
		// Ignore
		if (!theClient->IsIgnored())
			AddItem(ClientOptionsCtrl_Ignore_String,LobbyClientAction_Ignore);
		else
			AddItem(ClientOptionsCtrl_Unignore_String,LobbyClientAction_Unignore);

		// Block
		if(isInternet)
		{
			if (!theClient->IsBlocked())
				AddItem(ClientOptionsCtrl_Block_String,LobbyClientAction_Block);
			else
				AddItem(ClientOptionsCtrl_Unblock_String,LobbyClientAction_Unblock);
		}
	}
	


	if (isInternet && (amModerator || amCaptain))
	{
		mPopupMenu->AddSeperator();
		
		// Only moderators can warn (not captains)
		if (amModerator)
			AddItem(ClientOptionsCtrl_Warn_String,LobbyClientAction_Warn);

		if (theClient->IsMuted(isGame)) 
			AddItem(ClientOptionsCtrl_UnMute_String,LobbyClientAction_Unmute);
		else
			AddItem(ClientOptionsCtrl_Mute_String,LobbyClientAction_Mute);

		if(!isMe)
		{
			AddItem(ClientOptionsCtrl_Kick_String,LobbyClientAction_Kick);
			AddItem(ClientOptionsCtrl_Ban_String,LobbyClientAction_Ban);
		}
	}
	else if(!isInternet && amCaptain && !isMe)
	{
		mPopupMenu->AddSeperator();
		AddItem(ClientOptionsCtrl_Kick_String,LobbyClientAction_Kick);
	}

	SetSize(mPopupMenu->Width(), mPopupMenu->Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ClientOptionsPopup::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ListItemUpClicked:
			{
				ListItemClickedEvent* theMenuEvent = (ListItemClickedEvent*)theEvent;
				if (theMenuEvent->mItem == NULL || theMenuEvent->mButton != MouseButton_Left)
					return;


				LobbyClientAction anAction = (LobbyClientAction)((MenuItem*)(theMenuEvent->mItem.get()))->mMenuId;
				if(anAction!=LobbyClientAction_None)
					FireEvent(new ClientActionEvent(anAction,mRoomSpecFlags,mClient));

				GetWindow()->EndPopup(this);
				return;
			}
	}

	Container::HandleComponentEvent(theEvent);
}

