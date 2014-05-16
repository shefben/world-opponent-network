#ifndef __WON_LobbyStaging_H__
#define __WON_LobbyStaging_H__

#include "WONSocket/IPAddr.h"

namespace WONAPI
{

class ByteBuffer;
class LobbyClient;
class LobbyGame;
class LobbyPlayer;

enum LobbyGameType;
enum WONStatus;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyStaging
{
public:
	// Firewall/LocalIP methods
	static unsigned short GetLobbyPort();
	static void SetLobbyPort(unsigned short thePort);
	static void StartFirewallDetect();
	static void SetPublicAddr(const IPAddr &theAddr);
	static void SetBehindFirewall(bool behindFirewall);
	static IPAddr GetIPAddr(LobbyGameType theGameType);
	static IPAddr GetPublicAddr();
	static bool IsBehindFirewall();
	static bool IsFirewallTestDone();

	// Game functions
	static void TryHostGame();
	static void TryJoinGame(LobbyGame *theGame);
	static bool LeaveGame(bool confirm);
	static bool Logout(bool confirm);
	static void UpdateGameSummary();
	static void SetGameInProgress(bool inProgress);
	static LobbyGame* GetGame();
	static LobbyClient* GetMyClient();
	static LobbyPlayer* GetMyPlayer();
	static bool IAmReady();
	static bool IAmCaptain();
	static void SetEveryoneUnready();
	static void UpdateStartButton();
	static void SendReadyRequest(bool isReady);
	static void SendStartGame();
	static void SendDissolveGame();
	static void KickClient(LobbyClient *theClient, bool isBan);

	static void SendGameMessageToCaptain(const ByteBuffer *theMsg);
	static void BroadcastGameMessage(const ByteBuffer *theMsg);
	static void SendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg);
};


} // namespace WONAPI


#endif
