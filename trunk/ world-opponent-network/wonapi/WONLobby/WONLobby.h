#ifndef __WON_WONLOBBY_H__
#define __WON_WONLOBBY_H__

#include "WONCommon/Platform.h"

#include "LobbyStatus.h"

namespace WONAPI
{
class Container;
class LobbyGame;
class ResourceConfigTable;

WON_PTR_FORWARD(ComponentConfig);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONLobby
{
public:
	typedef void(*ComponentInitFunc)(ComponentConfig *theConfig);

	static bool Init(ResourceConfigTable *theTable = NULL, ComponentInitFunc theInitFunc = NULL);
	static void Destroy();

	static const GUIString& GetInitError();

	static Container* GetLobbyContainer();
	static void StartLobby();

	static LobbyStatus RunLobby(); 
	static void EndLobby(LobbyStatus theStatus = LobbyStatus_None);
	static LobbyStatus GetStatus();
	static void SuspendLobby();
	static void ResumeLobby();
	static LobbyGame* GetGame();
	static bool IsLobbySuspended();

	static void InitAuth(const std::wstring& theLoginCommunity, const char* theHashFileListStr = NULL);

};

} // namespace WONAPI


#endif