#ifndef __WON_WONLOBBYPRV_H__
#define __WON_WONLOBBYPRV_H__

#include "LobbyLogic.h"
#include "WONAuth/CDKey.h"
#include "WONCommon/SmartPtr.h"
#include "WONSocket/IPAddr.h"
#include <string>

namespace WONAPI
{

class ResourceConfigTable;

enum LobbyGameType;
enum LobbyRunMode;
enum LobbyStatus;

WON_PTR_FORWARD(AccountLogic);
WON_PTR_FORWARD(AuthContext);
WON_PTR_FORWARD(ChatCommandParser);
WON_PTR_FORWARD(DialogLogic);
WON_PTR_FORWARD(StagingLogic);
WON_PTR_FORWARD(InitLogic);
WON_PTR_FORWARD(LanLogic);
WON_PTR_FORWARD(LobbyContainer);
WON_PTR_FORWARD(LobbyPersistentData);
WON_PTR_FORWARD(LogicMgr);
WON_PTR_FORWARD(PingLogic);
WON_PTR_FORWARD(RoutingLogic);
WON_PTR_FORWARD(ServerContext);
WON_PTR_FORWARD(UDPManager);
WON_PTR_FORWARD(WindowEventBase);
WON_PTR_FORWARD(LobbyConfig);
WON_PTR_FORWARD(WONBigFile);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONLobbyPrv : public RefCount
{
public:
	LobbyPersistentDataPtr mPersistentData;
	LobbyConfigPtr mLobbyConfig;
	LobbyContainerPtr mLobbyContainer;
	GUIString mInitError;

	typedef std::list<WONBigFilePtr> BigFileList;
	BigFileList mBigFileList;

	AccountLogicPtr mAccountLogic;
	DialogLogicPtr mDialogLogic;
	InitLogicPtr mInitLogic;
	RoutingLogicPtr mRoutingLogic;
	StagingLogicPtr mStagingLogic;
	LanLogicPtr mLanLogic;
	PingLogicPtr mPingLogic;
	LobbyLogicMgr mLogicMgr;
	UDPManagerPtr mUDPManager;


	WindowEventBasePtr mTimerEvent;
	DWORD mLastTimerTick;
	LobbyStatus mLobbyStatus;
	LobbyRunMode mRunMode;
	bool mSuspended;

	// Server Lists
	ServerContextPtr mAccountServers;
	ServerContextPtr mAuthServers;
	ServerContextPtr mDirServers;
	ServerContextPtr mRoomDirServers;
	ServerContextPtr mFirewallDetectServers;
	ServerContextPtr mPatchServers;

	WONStatus mVersionStatus;
	AuthContextPtr mAuthContext;
	CDKey mCDKey;
	std::wstring mLoginCommunity;
	bool mHasSetHashFileList;
	typedef std::list<std::string> HashFileList;
	HashFileList mHashFileList;
	unsigned short mProductId; // hash of version and product name
	unsigned short mLanProductId; // hash of lan version and product name

	unsigned short mLobbyPort;
	time_t mSysTOUTime, mGameTOUTime;
	bool mBehindFirewall;
	bool mFirewallTestDone;
	std::string mSysTOUPath;
	std::string mGameTOUPath;
	IPAddr mPublicAddr;
	ChatCommandParserPtr mChatCommandParser;

	LobbyGameType mCurGameType;

	typedef void(*ComponentInitFunc)(ComponentConfig *theConfig);

	void RemoveBigFiles();

public:
	WONLobbyPrv();
	virtual ~WONLobbyPrv();

	bool Init(ResourceConfigTable *theTable, ComponentInitFunc theInitFunc);
	void InitAuth(const std::wstring& theLoginCommunity, const char* theHashFileListStr = NULL);
	void Destroy();
};
typedef SmartPtr<WONLobbyPrv> WONLobbyPrvPtr; 
extern WONLobbyPrvPtr gLobbyData;


} // namespace WONAPI

#endif