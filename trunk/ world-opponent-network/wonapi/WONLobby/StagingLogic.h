#ifndef __WON_StagingLogic_H__
#define __WON_StagingLogic_H__
#include "LobbyLogic.h"

namespace WONAPI
{

class ByteBuffer;
class GameActionEvent;
class JoinGameEvent;
class ReadBuffer;

WON_PTR_FORWARD(CreateGameDialog);
WON_PTR_FORWARD(LobbyClient);
WON_PTR_FORWARD(LobbyGame);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StagingLogic : public LobbyLogic
{
protected:
	LobbyGamePtr mGame;
	LobbyClientPtr mMyClient;
	AsyncOpPtr mJoinGameOp;
	short mJoinGameReply;
	bool mJoinedGame;
	bool mIAmReadySoundState;
	bool mEveryoneReadySoundState;
	time_t mLastReadySoundTime;

	CreateGameDialogPtr mCreateGameDialog;

	void ShowGameActionMenu(LobbyGame *theGame);
	bool VerifyPing(LobbyGame *theGame);

	void HandleJoinGameRequest(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandleJoinGameReply(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandlePlayerJoined(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandleReadyRequest(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandlePlayerReady(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandleStartGameMsg(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandleDissolveGame(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandlePingChangedRequest(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandlePingChanged(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandleClientKicked(ReadBuffer &theMsg, LobbyClient *theSender);

	void HandleGameAction(GameActionEvent *theEvent);
	void HandleStartGameButton();
	void HandleJoinGame(JoinGameEvent *theEvent);

	void CheckPlayReadySound();

	virtual ~StagingLogic();

public:
	StagingLogic();

	virtual void Init(ComponentConfig *theConfig);
	virtual void HandleControlEvent(int theControlId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	bool Logout(bool confirm);
	bool LeaveGame(bool confirm);
	bool IAmCaptain();
	bool IAmReady();
	LobbyGame* GetGame() { return mGame; }
	LobbyClient* GetMyClient() { return mMyClient; }

	void NotifyGameDisconnect(bool showErrorDlg);
	void NotifyClientKicked(LobbyClient *theClient, bool isBan);
	void NotifyClientLeftGame(LobbyClient *theClient);
	void NotifyPingChange(LobbyGame *theGame);

	void TryJoinGame(LobbyGame *theGame);
	void TryHostGame();

	void SendReadyRequest(bool isReady);
	void SendStartGame();
	void SendDissolveGame();
	void SetGameInProgress(bool inProgress);
	void SetEveryoneUnready();

	void HandleGameMessage(ReadBuffer &theMsg, LobbyClient *theSender);
	void HandleGameMessage(const ByteBuffer *theMsg, LobbyClient *theSender);
	void SendGameMessageToCaptain(const ByteBuffer *theMsg);
	void BroadcastGameMessage(const ByteBuffer *theMsg);
	void SendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg);
	void KickClient(LobbyClient *theClient, bool isBan);
	void UpdateStartButton();
};
typedef SmartPtr<StagingLogic> StagingLogicPtr;


} // namespace WONAPI

#endif