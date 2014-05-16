#ifndef __WON_LobbyStagingPrv_H__
#define __WON_LobbyStagingPrv_H__

#include "LobbyStaging.h"

namespace WONAPI
{

class ByteBuffer;
class LobbyClient;
class LobbyGame;
class NetLogic;
class ReadBuffer;

enum LobbyGameType;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyStagingPrv
{
public:
	// Getting the NetLogic
	static NetLogic* GetNetLogic(LobbyGameType theType);
	static NetLogic* GetNetLogic(); // uses current game type

	// Messages from NetLogic
	static void NotifyGameDisconnect(bool showErrorDlg = true);
	static void NotifyClientKicked(LobbyClient *theClient, bool isBan);
	static void NotifyClientLeftGame(LobbyClient *theClient);
	static void NotifyPingChange(LobbyGame *theGame);
	static void HandleGameMessage(const ByteBuffer *theMsg, LobbyClient *theClient);
	static void HandleGameMessage(ReadBuffer &theMsg, LobbyClient *theClient);

	// Messages to NetLogic
	static bool NetCreateGame(LobbyGame *theGame);
	static bool NetJoinGame(LobbyGame *theGame);
	static LobbyClient* NetGetMyClient();
	static void NetSendGameMessageToCaptain(const ByteBuffer *theMsg);
	static void NetBroadcastGameMessage(const ByteBuffer *theMsg);
	static void NetSendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg);	
	static void NetKickClient(LobbyClient *theClient, bool isBan);
	static void NetDeleteGameSummary(LobbyGame *theGame);
	static void NetCloseGame(bool close);
	static void NetLeaveGame(LobbyGame *theGame);
	static void NetSuspendLobby();
	static void NetResumeLobby();
	static void NetLogout();

	// Not sure if this should go here, but this is what account logic calls after login succeeds
	static void EnterInternetScreen(bool joinDefaultRoom);
};


} // namespace WONAPI


#endif
