#include "GameOptionsPopup.h"

#include "LobbyConfig.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyResource.h"


#include "WONGUI/MenuBar.h"
#include "WONGUI/Window.h"

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameOptionsPopup::GameOptionsPopup()
{
	mPopupMenu = new PopupMenu;
	AddChild(mPopupMenu);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GameOptionsPopup::~GameOptionsPopup()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameOptionsPopup::AddItem(const GUIString &theStr, LobbyGameAction theAction)
{
	mPopupMenu->AddItem(theStr, NULL, NULL, theAction);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GameOptionsPopup::Init(LobbyGame *theGame)
{
	mPopupMenu->Clear(false);

	mGame = theGame;

	FontPtr aNormalFont = mPopupMenu->GetFont();
	FontPtr aBoldFont = mPopupMenu->GetFontMod(aNormalFont,FontStyle_Bold);

	mPopupMenu->SetFont(aBoldFont);
	MenuItem *anItem = mPopupMenu->AddItem(theGame->GetName());
	anItem->mSelectable = false;

	mPopupMenu->SetFont(aNormalFont);
	mPopupMenu->AddSeperator();

	if (theGame->IsOkToJoin())
		AddItem(GameOptionsPopup_Join_String, LobbyGameAction_Join);


	bool canPing = true;
	if(canPing && theGame->InProgress())
	{
		LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
		if(aConfig!=NULL)
			canPing = !aConfig->mNoPingInProgress;
	}

	if(canPing && theGame->GetGameType()!=LobbyGameType_Lan)
		AddItem(GameOptionsPopup_Ping_String, LobbyGameAction_Ping);

	if(canPing)
		AddItem(GameOptionsPopup_QueryDetails_String, LobbyGameAction_QueryDetails);

	if (theGame->GetDetailsVisible())
		AddItem(GameOptionsPopup_HideDetails_String, LobbyGameAction_HideDetails);
	else
		AddItem(GameOptionsPopup_ShowDetails_String, LobbyGameAction_ShowDetails);

	// FIX: Add to favorites for Client/Server games!

	if(theGame->GetGameType()==LobbyGameType_DirectConnect)
	{
		mPopupMenu->AddSeperator();
		AddItem(GameOptionsPopup_Remove_String, LobbyGameAction_Remove);
	}

	SetSize(mPopupMenu->Width(), mPopupMenu->Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Most events are handled by LobbyContainer
void GameOptionsPopup::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_ListItemUpClicked:
			{
				ListItemClickedEvent* theMenuEvent = (ListItemClickedEvent*)theEvent;
				if (theMenuEvent->mItem == NULL || theMenuEvent->mButton != MouseButton_Left)
					return;


				LobbyGameAction anAction = (LobbyGameAction)((MenuItem*)(theMenuEvent->mItem.get()))->mMenuId;
				if(anAction!=LobbyClientAction_None)
					FireEvent(new GameActionEvent(anAction,mGame));

				GetWindow()->EndPopup(this);
				return;
			}
	}

	Container::HandleComponentEvent(theEvent);
}
