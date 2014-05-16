#ifndef __WON_LANLOGIC_H__
#define __WON_LANLOGIC_H__

#include "NetLogic.h"

#include "WONSocket/IPAddr.h"

namespace WONAPI
{

class ByteBuffer;
class ChangeDirectConnectEvent;
class ClientActionEvent;
class LobbyClientList;
class ReadBuffer;

enum LanRegisterStatus;

WON_PTR_FORWARD(AsyncOp);
WON_PTR_FORWARD(DirectConnectDialog);
WON_PTR_FORWARD(LanNameDialog);
WON_PTR_FORWARD(LanPortDialog);
WON_PTR_FORWARD(LobbyClient);
WON_PTR_FORWARD(LobbyClientList);
WON_PTR_FORWARD(LobbyGame);
WON_PTR_FORWARD(LobbyGameList);
WON_PTR_FORWARD(LobbyLanClient);
WON_PTR_FORWARD(LobbyLanSocket);
WON_PTR_FORWARD(QueueSocket);
WON_PTR_FORWARD(PasswordDialog);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LanLogic : public NetLogic
{
protected:
	DirectConnectDialogPtr mDirectConnectDialog;
	LanNameDialogPtr mLanNameDialog;
	LanPortDialogPtr mLanPortDialog;
	PasswordDialogPtr mPasswordDialog;

	LobbyGameListPtr mLanGameList;
	LobbyGameListPtr mDirectConnectGameList;

	typedef std::set<LobbyLanSocketPtr> SocketSet;
	SocketSet mSocketSet;
	LobbyLanSocketPtr mListenSocket;
	LobbyLanSocketPtr mClientSocket;
	time_t mLastClientSocketSendTime;
	AsyncOpPtr mClientKeepAliveTimer;
	bool mNeedAuth;

	QueueSocketPtr mBroadcastSocket;
	AsyncOpPtr mBroadcastTimer;
	IPAddr mBroadcastAddr;
	time_t mLastHeartBeatTime;

	LobbyClientListPtr mClientList;
	LobbyLanClientPtr mMyClient;
	unsigned short mNextClientId;
	bool mIAmCaptain;

	unsigned short GetFreeClientId();
	void KillNet();
	bool InGame();
	bool IAmCaptain();

	void AddSocket(LobbyLanSocket *theSocket);
	void AcceptCompletion(AsyncOp *theOp);
	void AcceptAuthCompletion(AsyncOp *theOp);
	void RecvCompletion(AsyncOp *theOp);
	void CloseCompletion(AsyncOp *theOp);
	void ClientKeepAliveCompletion(AsyncOp *theOp);
	void StartClientKeepAliveTimer();


	LanRegisterStatus HandleRegisterReply(const ByteBuffer *theMsg, LobbyGame *theGame);
	void HandleRegisterRequest(ReadBuffer &theMsg, LobbyLanSocket *theSocket);
	void HandleClientJoined(ReadBuffer &theMsg);
	void HandleClientLeft(ReadBuffer &theMsg);
	void HandleChatRequest(ReadBuffer &theMsg, LobbyLanClient *theSender);
	void HandleChat(ReadBuffer &theMsg);
	void HandleGameMessage(ReadBuffer &theMsg, LobbyLanClient *theSender);

	void HandleNonClientRequest(const ByteBuffer *theReq, LobbyLanSocket *theSocket);
	void HandleRecvMessage(const ByteBuffer *theMsg, LobbyLanClient *theSender);
	void SendMessageToCaptain(const ByteBuffer *theMsg);
	void SendMessageToClient(LobbyLanClient *theClient, const ByteBuffer *theMsg);
	void BroadcastMessage(const ByteBuffer *theMsg, bool sendToMe = true);

	void SendChatRequest(const GUIString &theText, unsigned short theFlags, unsigned short theDestId = 0);
	bool DoDirectConnectAuth();


	void HandleAddLanName(); 
	void HandleChangeLanBroadcastPort(); 
	void HandleChangeDirectConnect(ChangeDirectConnectEvent *theEvent);
	void HandleClientAction(ClientActionEvent *theEvent);
	void HandleDoChatCommand(Component *theChatCtrl);
	void HandleRefreshGameList();
	void HandleShowLanScreen();
	void ShowClientOptions(LobbyClient *theClient, int theRoomSpecFlags);

	// Lan ServerList Functions
	bool NeedBroadcast(LobbyGame *theGame);
	void HandleGameInfoRequest(ReadBuffer &theMsg, const IPAddr &theAddr);
	void HandleGameInfo(ReadBuffer &theMsg, const IPAddr &theAddr);
	void HandleGameHeartbeat(ReadBuffer &theMsg, const IPAddr &theAddr);
	void HandleGameDeleted(ReadBuffer &theMsg, const IPAddr &theAddr);
	void SendGameInfo(LobbyGame *theGame, const IPAddr& theAddr);
	void BroadcastGameHeartbeat(LobbyGame *theGame);
	void BroadcastGameDeleted(LobbyGame *theGame);
	void BroacastTimerCompletion(AsyncOp *theOp);
	void RecvBroadcastCompletion(AsyncOp *theOp);
	void StartBroadcastTimer();
	void StartLanListen();
	void StopLanListen();

	virtual ~LanLogic();
public:
	LanLogic();

	LobbyClientList* GetClientList();

	virtual void Init(ComponentConfig *theConfig);
	virtual void HandleControlEvent(int theControlId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	// NetLogic Interface
	virtual bool NetCreateGame(LobbyGame *theGame);
	virtual bool NetJoinGame(LobbyGame *theGame);
	virtual LobbyClient* NetGetMyClient();

	virtual void NetSendGameMessageToCaptain(const ByteBuffer *theMsg);
	virtual void NetBroadcastGameMessage(const ByteBuffer *theMsg);
	virtual void NetSendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg);
	virtual void NetUpdateGameSummary(LobbyGame *theGame);
	virtual void NetDeleteGameSummary(LobbyGame *theGame);
	virtual void NetKickClient(LobbyClient *theClient, bool isBan);	
	virtual void NetCloseGame(bool close);
	virtual void NetLeaveGame(LobbyGame *theGame);
	virtual void NetSuspendLobby();
	virtual void NetResumeLobby();
	virtual void NetLogout();

};
typedef SmartPtr<LanLogic> LanLogicPtr;


} // namespace WONAPI

#endif