#ifndef __WON_GAMESTAGINGCTRL_H__
#define __WON_GAMESTAGINGCTRL_H__

#include "WONGUI/Container.h"

namespace WONAPI
{

class PlayerChangedEvent;

WON_PTR_FORWARD(Button);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameStagingCtrl : public Container
{
protected:
	ButtonPtr mLeaveGameButton;
	ButtonPtr mStartGameButton;
	int mRoomSpecFlags;
	bool mInGame;
	bool mReady;

	void UpdateStartButton();
	void HandleEnteredGame();
	void HandleLeftGame();

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void InitComponent(ComponentInit &theInit);

public:
	GameStagingCtrl();
};
typedef SmartPtr<GameStagingCtrl> GameStagingCtrlPtr;

}; // namespace WONAPI


#endif