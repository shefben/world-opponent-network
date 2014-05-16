#ifndef __WON_ROUTINGLOGIC_H__
#define __WON_ROUTINGLOGIC_H__

#include "NetLogic.h"
#include <time.h>

namespace WONAPI
{

WON_PTR_FORWARD(AddFriendDialog);
WON_PTR_FORWARD(AdminActionDialog);
WON_PTR_FORWARD(BadUserDialog);
WON_PTR_FORWARD(CreateRoomDialog);
WON_PTR_FORWARD(LobbyGame);
WON_PTR_FORWARD(LobbyGameList);
WON_PTR_FORWARD(LobbyGroup);
WON_PTR_FORWARD(LobbyServer);
WON_PTR_FORWARD(LobbyServerList);
WON_PTR_FORWARD(PasswordDialog);
WON_PTR_FORWARD(RoomDialog);
WON_PTR_FORWARD(RoutingConnection);

class AcceptClientEvent;
class AsyncOp;
class ChatCommandParser;
class ClientActionEvent;
class CreateRoomEvent;
class FriendActionEvent;
class IPAddr;
class JoinRoomEvent;
class JoinFriendsRoomEvent;
class JumpToRoomEvent;
class LobbyClient;
class LobbyClientList;
class LobbyFriend;
class QueryServerGroupsEvent;
class QuickPlayEvent;
class RoutingConnection;
class RoutingOp;

enum LobbyClientAction;
enum LobbySkillLevel;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingLogic : public NetLogic
{
protected:
	time_t mLastRoomFetchTime;
	std::string mGameObjectName;
	LobbyServerListPtr mServerList;
	LobbyGameListPtr mGameList;
	bool mIsManualAFK;
	bool mIsAutoAFK;
	bool mGotGameList;
	bool mWantToBeModerator;
	AsyncOpPtr mGetGameListOp;

	LobbyServerPtr mServer;
	LobbyServerPtr mPendingServer;

	LobbyGroupPtr mChatGroup;
	LobbyGroupPtr mGameGroup;

	AddFriendDialogPtr mAddFriendDialog;
	AdminActionDialogPtr mAdminActionDialog;
	BadUserDialogPtr mBadUserDialog;
	CreateRoomDialogPtr mCreateRoomDialog;
	RoomDialogPtr mRoomDialog;
	PasswordDialogPtr mPasswordDialog;

	friend class AutoKillPendingConnection;

	void CloseCompletion(AsyncOp *theOp);
	void LocateFriendCompletion(AsyncOp *theOp);

	void CaptainChangedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientFlagsChangedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void MemberFlagsChangedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientJoinedServerCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientLeftServerCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientJoinedGroupCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientLeftGroupCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void DataObjectCreatedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void DataObjectDeletedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ReadDataObjectCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ChatCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void GroupCreatedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void GroupDeletedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void GroupInvitationCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void GroupJoinAttemptCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void PeerDataCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientBannedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ClientMutedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ServerAlertCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ServerShutdownStartedCompletion(RoutingOp *theOp, LobbyServer *theServer);
	void ServerShutdownAbortedCompletion(RoutingOp *theOp, LobbyServer *theServer);

	bool CheckFriendMapSize();
	bool CreateGroup(LobbyServer *theServer, const GUIString &theName, const GUIString &thePassword, int theRoomFlags, unsigned short &theGroupId);
	void DoAdminActionDialog(LobbyClientAction theAction, int theRoomSpecFlags, LobbyClient *theClient);
	void DoRoomDialog();
	RoutingConnection* GetConnection();
	bool IsConnected();
	LobbyServer* GetServer(const IPAddr &theAddr);
	LobbyServer* GetDefaultServer();
	LobbyGroup* GetGroup(int theId);
	LobbyGroup* GetGroupBySpec(int theRoomSpecFlags);
	void GetBadUsers(bool mutes, bool global, int theRoomSpecFlags);
	void GetGroupListReqCompletion(AsyncOp *theOp);
	LobbyGame* GetQuickplayGame(LobbySkillLevel theSkillLevel, unsigned short theMaxPing);
	int GetSpecFlags(unsigned short theId);
	int GetSpecFlags(LobbyGroup *theGroup, LobbyServer *theServer);
	void GetRooms();
	void HandleAnonymousModeChange(); 
	void HandleAcceptClient(AcceptClientEvent *theEvent);
	void HandleAddFriend();
	void HandleClientAction(ClientActionEvent *theEvent);
	void HandleCreateRoom(CreateRoomEvent *theEvent);
	void HandleDirtyWordFilterChange(); 
	void HandleDoChatCommand(Component *theChatCtrl);
	void HandleFriendAction(FriendActionEvent *theEvent);
	void HandleJoinRoom(JoinRoomEvent *theEvent); 
	void HandleJoinFriendsRoom(JoinFriendsRoomEvent *theEvent); 
	void HandleJumpToRoom(JumpToRoomEvent *theEvent);
	void HandleQueryServerGroups(QueryServerGroupsEvent *theEvent);
	void HandleQuickPlay(QuickPlayEvent*theEvent);
	void HandleRefreshGameList(); 
	bool InviteClient(LobbyClient *theClient, bool invite, const GUIString &theText);
	bool JoinDefaultRoom(bool anonymousJoin = false);
	bool JoinGroup(LobbyServer *theServer, int theGroupId);
	bool JoinPending();
	bool JoinServer(LobbyServer *theServer, bool anonymousJoin = false);
	void KillPendingConnection();
	void KillConnection();
	void LocateFriend(LobbyFriend *theFriend, int theRoomSpec);
	bool NeedLocateFriend(LobbyFriend *theFriend, time_t theTime);
	void RefreshFriendsList();
	void RemoveFriend(LobbyFriend *theFriend);
	bool ReportClientLocation(LobbyClient *theClient, bool reportIfNotFound, int theRoomSpec, bool getUpToDateClient = true);
	void ReportFriendLocation(LobbyFriend *theFriend, int theRoomSpec);
	void ShowClientOptions(LobbyClient *theClient, int theRoomSpecFlags);
	void ShowFriendOptions(LobbyFriend *theFriend, int theRoomSpecFlags);
	void SucceedJoinGroup(unsigned short theGroupId, bool isGame);
	void SyncClientWithFriends(LobbyClient *theClient);
	bool TryJoinChatRoom(LobbyServer *theServer, unsigned short theGroupId);

	virtual ~RoutingLogic();

public: 
	// Net Logic Interface
	virtual bool NetCreateGame(LobbyGame *theGame);
	virtual bool NetJoinGame(LobbyGame *theGame);
	virtual LobbyClient* NetGetMyClient();

	virtual void NetSendGameMessageToCaptain(const ByteBuffer *theMsg);
	virtual void NetBroadcastGameMessage(const ByteBuffer *theMsg);
	virtual void NetSendGameMessageToClient(LobbyClient *theClient, const ByteBuffer *theMsg);
	virtual void NetUpdateGameSummary(LobbyGame *theGame);
	virtual void NetDeleteGameSummary(LobbyGame *theGame);

	virtual void NetCloseGame(bool close);
	virtual void NetKickClient(LobbyClient *theClient, bool isBan);
	virtual void NetLeaveGame(LobbyGame *theGame);
	virtual void NetSuspendLobby();
	virtual void NetResumeLobby();
	virtual void NetLogout();

public:
	RoutingLogic();

	virtual void Init(ComponentConfig *theConfig);
	virtual void HandleControlEvent(int theControlId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void TimerEvent();

	LobbyGroup* GetChatGroup() { return mChatGroup; }
	LobbyServer* GetServer() { return mServer; }
	LobbyServerList* GetServerList() { return mServerList; }

	LobbyServer* GetServer(RoutingOp *theOp);
	LobbyClientList* GetClientList();

	void EnterInternetScreen(bool joinDefaultRoom);


};
typedef SmartPtr<RoutingLogic> RoutingLogicPtr;

} // namespace WONAPI

#endif