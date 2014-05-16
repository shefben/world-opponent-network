#ifndef __ClientOptionsPopup_H__
#define __ClientOptionsPopup_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

class LobbyGroup;
class LobbyGame;

enum LobbyClientAction;

WON_PTR_FORWARD(LobbyClient);
WON_PTR_FORWARD(PopupMenu);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ClientOptionsPopup : public Container
{
protected:
	PopupMenuPtr	mPopupMenu;
	LobbyClientPtr mClient;
	int mRoomSpecFlags;

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	void AddItem(const GUIString &theStr, LobbyClientAction theAction);

	virtual ~ClientOptionsPopup();

public:
	ClientOptionsPopup();

	void Init(LobbyClient *theClient, LobbyClient *myClient, int theRoomSpecFlags);
};
typedef SmartPtr<ClientOptionsPopup> ClientOptionsPopupPtr;


} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __ClientOptionsPopup_H__