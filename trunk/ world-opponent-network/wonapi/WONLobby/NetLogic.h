#ifndef __WON_NETLOGIC_H__
#define __WON_NETLOGIC_H__

#include "LobbyLogic.h"

namespace WONAPI
{

class ByteBuffer;
class LobbyGame;
class LobbyClient;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NetLogic : public LobbyLogic
{
public:
	virtual bool NetCreateGame(LobbyGame *theGame) = 0;
	virtual bool NetJoinGame(LobbyGame *theGame) = 0;
	virtual LobbyClient* NetGetMyClient() = 0;

	virtual void NetSendGameMessageToCaptain(const ByteBuffer *theMsg) = 0;
	virtual void NetBroadcastGameMessage(const ByteBuffer *theMsg) = 0;
	virtual void NetSendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg) = 0;
	virtual void NetUpdateGameSummary(LobbyGame *theGame) = 0;
	virtual void NetDeleteGameSummary(LobbyGame *theGame) = 0;
	virtual void NetKickClient(LobbyClient *theClient, bool isBan) = 0;
	virtual void NetCloseGame(bool close) = 0;

	virtual void NetLeaveGame(LobbyGame *theGame) = 0;
	virtual void NetSuspendLobby() = 0;
	virtual void NetResumeLobby() = 0;
	virtual void NetLogout() = 0;
};

} // namespace WONAPI

#endif