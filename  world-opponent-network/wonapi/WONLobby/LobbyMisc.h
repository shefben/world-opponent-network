#ifndef __WON_LOBBYMISC_H__
#define __WON_LOBBYMISC_H__

#include "WONCommon/Platform.h"
#include <time.h>
#include <string>

namespace WONAPI
{

class AsyncOp;
class AuthContext;
class CDKey;
class ChatCommandParser;
class LobbyClient;
class LobbyGame;
class LobbyGroup;
class LobbyServer;
class LobbyServerList;
class ServerContext;
class Sound;
class UDPManager;

enum WONStatus;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyMisc
{
public:
	static ServerContext* GetAccountServers();
	static ServerContext* GetAuthServers();
	static ServerContext* GetDirServers();
	static ServerContext* GetRoomDirServers();
	static ServerContext* GetFirewallDetectServers();
	static ServerContext* GetPatchServers();
	static void SetRoomDirServersToDirServers();

	static AuthContext* GetAuthContext();
	static bool GetCDKey(CDKey &theKey);
	static bool DoCDKeyCheck();
	static bool IsCDKeyValid();

	static unsigned short GetProductId();
	static unsigned short GetLanProductId();

	static void SetVersionStatus(WONStatus theStatus);
	static WONStatus GetVersionStatus();

	static int GetDefaultTimeout();
	static void RunOp(AsyncOp *theOp, DWORD theTime = 0); // time = 0 -> use default

	static void SetTOUTimes(time_t theSysTime, time_t theGameTime);
	static void GetTOUTimes(time_t &theSysTime, time_t &theGameTime);
	static bool CheckDoTOU(time_t theSysTime, time_t theGameTime);
	static const std::string& GetSysTOUPath();
	static const std::string& GetGameTOUPath();

	static LobbyServerList* GetLobbyServerList();
	static LobbyGroup* GetChatGroup();
	static LobbyServer* GetChatServer();

	static ChatCommandParser* GetChatCommandParser(int theRoomSpecFlags);
	static ChatCommandParser* GetChatCommandParser();

	static void SetIgnored(LobbyClient *theClient, bool ignored, int theRoomSpecFlags);
	static void SetBlocked(LobbyClient *theClient, bool blocked, int theRoomSpecFlags);

	static void QueuePing(LobbyGame *theGame, bool force = false);
	static void QueueDetailQuery(LobbyGame *theGame);
	static int GetNumPingsToDo();
	static void KillPings(bool resetValues = false); // resetValues -> mark game as no longer pinging (or maybe put last known ping back)

	static UDPManager* GetUDPManager();
	static void PlaySound(Sound *theSound);

	static void KillFirewallDetectOps();
	static void RestartUDPManager(); 
	static void CheckSetNetworkAdapter(long theHost);
};

} // namespace WONAPI

#endif