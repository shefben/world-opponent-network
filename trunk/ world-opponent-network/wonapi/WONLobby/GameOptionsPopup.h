#ifndef __GameOptionsPopup_H__
#define __GameOptionsPopup_H__

#include "WONGUI/Container.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{

enum LobbyGameAction;

WON_PTR_FORWARD(LobbyGame);
WON_PTR_FORWARD(PopupMenu);

class GameOptionsPopup : public Container
{
protected:
	LobbyGamePtr	mGame;
	PopupMenuPtr	mPopupMenu;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void AddItem(const GUIString &theStr, LobbyGameAction theAction);

	virtual ~GameOptionsPopup();

public:
	GameOptionsPopup();
	void Init(LobbyGame *theGame);

};
typedef SmartPtr<GameOptionsPopup> GameOptionsPopupPtr;


} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __GameOptionsPopup_H__