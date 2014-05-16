#ifndef __WON_LOBBYEVENT_H__
#define __WON_LOBBYEVENT_H__

#include "LobbyTypes.h"

#include "WONSocket/IPAddr.h"
#include "WONGUI/Component.h"

#include <string>

namespace WONAPI
{
WON_PTR_FORWARD(GetHTTPDocumentOp);
WON_PTR_FORWARD(LobbyClient);
WON_PTR_FORWARD(LobbyClientList);
WON_PTR_FORWARD(LobbyFriend);
WON_PTR_FORWARD(LobbyGame);
WON_PTR_FORWARD(LobbyGameList);
WON_PTR_FORWARD(LobbyGroup);
WON_PTR_FORWARD(LobbyServer);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyEventType
{
	LobbyEvent_Min							= Event_User_Start,

	LobbyEvent_Alert,
	LobbyEvent_AcceptClient,
	LobbyEvent_AnnounceFriend,
	LobbyEvent_AnnounceClientLocation,
	LobbyEvent_ClientAction,
	LobbyEvent_ClientChanged,
	LobbyEvent_ClientModerated,
	LobbyEvent_ChangeDirectConnect,
	LobbyEvent_ChatCommandError,
	LobbyEvent_CreateAccount,
	LobbyEvent_CreateRoom,
	LobbyEvent_DoChatCommand,
	LobbyEvent_EndLobby,
	LobbyEvent_EnteredScreen,
	LobbyEvent_FriendAction,
	LobbyEvent_FriendChanged,
	LobbyEvent_GameAction,
	LobbyEvent_GetLostPassword,
	LobbyEvent_GetLostUserName,
	LobbyEvent_GameChanged,
	LobbyEvent_GroupChanged,
	LobbyEvent_GroupInvitation,
	LobbyEvent_GroupJoinAttempt,
	LobbyEvent_GotHTTPDocument, // MOTD or TOU
	LobbyEvent_IgnoreInitError,
	LobbyEvent_JoinFriendsRoom,
	LobbyEvent_JoinGame,
	LobbyEvent_JoinRoom,
	LobbyEvent_JoinedGame,
	LobbyEvent_JoinedRoom,
	LobbyEvent_JumpToRoom,
	LobbyEvent_LeftGame,
	LobbyEvent_LeftRoom,
	LobbyEvent_LoadFilterCombos, // tells server list to reload filter comboboxes from persistent data
	LobbyEvent_Login,
	LobbyEvent_LoginInitStatus,
	LobbyEvent_PingGame,
	LobbyEvent_PlayerChanged,
	LobbyEvent_PlayerTeamChanged,
	LobbyEvent_QueryAccount,
	LobbyEvent_QueryAccountResult,
	LobbyEvent_QueryServerGroups,
	LobbyEvent_QuickPlay,
	LobbyEvent_RecvChat,
	LobbyEvent_RetryInitLogin,
	LobbyEvent_IgnoreInitLoginError,
	LobbyEvent_ServerChanged,
	LobbyEvent_SetChatInput,
	LobbyEvent_SetCrossPromotion,
	LobbyEvent_SetLanOptions,
	LobbyEvent_SetLoginFields,
	LobbyEvent_SetLoginNames,
	LobbyEvent_SetRoomList,
	LobbyEvent_SetRoomSpecFlags,
	LobbyEvent_SyncFirewallStatus,
	LobbyEvent_SyncFriendsList,
	LobbyEvent_SyncGameList,
	LobbyEvent_SyncFilteredGameList, // just consider the games that are currently in the listctrl
	LobbyEvent_ResyncGameList, // just re-add the current gamelist
	LobbyEvent_UpdateAccount,
	LobbyEvent_UpdateStartButton,

	LobbyEvent_Max
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyEvent : public ComponentEvent
{
private:
	static void Init();
	static void Destroy();
	friend class WONLobby;

public:
	LobbyEvent(int theType) : ComponentEvent(NULL, theType) { }

	static void ListenToComponent(Component *theComponent);
	static void DeliverLogicEvent(int theEventType);
	static void DeliverLogicEvent(LobbyEvent *theEvent);
	static void DeliverEventTo(LobbyEvent *theEvent, Component *theComponent);
	static void BroadcastEvent(int theEventType, bool sendNow = true);
	static void BroadcastEvent(LobbyEvent *theEvent, bool sendNow = true);
};
typedef SmartPtr<LobbyEvent> LobbyEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyChangeEvent : public LobbyEvent
{
protected:
	LobbyChangeType mChangeType;

public:
	LobbyChangeEvent(int theEventType, LobbyChangeType theChangeType);

	bool IsAdd();
	bool IsDelete();
	bool IsModify();
};
typedef SmartPtr<LobbyEvent> LobbyEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AcceptClientEvent : public LobbyEvent
{
protected:
	virtual ~AcceptClientEvent();

public:
	LobbyClientPtr mClient;
	bool mAccepted;

	AcceptClientEvent(LobbyClient *theClient, bool accepted);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AlertEvent : public LobbyEvent
{
protected:
	virtual ~AlertEvent();

public:
	enum AlertAction
	{
		Action_Alert,
		Action_ShutdownStarted,
		Action_ShutdownAborted
	};

	LobbyRoomSpec mRoomSpec;
	AlertAction   mAction;
	unsigned long mTimeLeft;
	GUIString     mComment;

	AlertEvent(int theRoomSpec, AlertAction theAction, unsigned long theTimeLeft, const GUIString& theComment);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnnounceClientLocationEvent : public LobbyEvent
{
protected:
	virtual ~AnnounceClientLocationEvent();

public:
	LobbyClientPtr mClient;
	GUIString mChatName;
	GUIString mGameName;
	LobbyRoomSpec mRoomSpec;
	bool mHandled;

	AnnounceClientLocationEvent(LobbyClient *theClient, const GUIString &theChatName, const GUIString &theGameName, int theSpecFlags);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnnounceFriendEvent : public LobbyChangeEvent
{
protected:
	virtual ~AnnounceFriendEvent();

public:
	LobbyFriendPtr mFriend;
	LobbyRoomSpec mRoomSpec;
	bool mHandled;

	AnnounceFriendEvent(LobbyFriend *theFriend, int theSpecFlags, LobbyChangeType theChangeType);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChatCommandErrorEvent : public LobbyEvent
{
public:
	GUIString mError;

	ChatCommandErrorEvent(const GUIString &theError) : LobbyEvent(LobbyEvent_ChatCommandError), mError(theError) {}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChangeDirectConnectEvent : public LobbyChangeEvent
{
protected:
	virtual ~ChangeDirectConnectEvent();

public:
	LobbyGamePtr mGame;

	ChangeDirectConnectEvent(LobbyChangeType theType, LobbyGame *theGame = NULL);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ClientActionEvent : public LobbyEvent
{
protected:
	virtual ~ClientActionEvent();

public:
	LobbyClientAction mAction;
	LobbyRoomSpec mRoomSpec;
	LobbyClientPtr mClient;

	ClientActionEvent(LobbyClientAction theAction, const LobbyRoomSpec& theRoomSpec, LobbyClient *theClient);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ClientModeratedEvent : public LobbyEvent
{
protected:
	virtual ~ClientModeratedEvent();

public:
	enum ModerationAction
	{
		Action_Ban,
		Action_Kick,
		Action_Mute,
		Action_Unmute
	};

	LobbyClientPtr mClient;
	LobbyRoomSpec mRoomSpec;
	GUIString mRoomName;
	ModerationAction mAction;
	unsigned long mDuration;
	bool mIsMe;
	GUIString mComment;

	ClientModeratedEvent(LobbyClient *theClient, int theRoomSpec, const GUIString& theRoomName, ModerationAction theAction, unsigned long theDuration, bool isMe, const GUIString& theComment);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ClientChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~ClientChangedEvent();

public:
	LobbyClientPtr mClient;
	LobbyRoomSpec mRoomSpec;

	ClientChangedEvent(LobbyClient *theClient, const LobbyRoomSpec& theRoomSpec, LobbyChangeType theChangeType);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreateAccountEvent : public LobbyEvent
{
public:
	std::wstring mUserName;
	std::wstring mPassword;
	std::string mEmail;
	unsigned char mBirthMonth;
	unsigned char mBirthDay;
	unsigned short mBirthYear;

	bool mSubscribeSierraNewsletter;

public:
	CreateAccountEvent() : LobbyEvent(LobbyEvent_CreateAccount) { }
};
typedef SmartPtr<CreateAccountEvent> CreateAccountEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CreateRoomEvent : public LobbyEvent
{
public:
	IPAddr mServerAddr;

	CreateRoomEvent(const IPAddr &theAddr) : LobbyEvent(LobbyEvent_CreateRoom), mServerAddr(theAddr) { }
};
typedef SmartPtr<CreateRoomEvent> CreateRoomEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FriendActionEvent : public LobbyEvent
{
protected:
	virtual ~FriendActionEvent();

public:
	LobbyFriendAction mAction;
	LobbyFriendPtr mFriend;
	int mRoomSpecFlags;

	FriendActionEvent(LobbyFriendAction theAction, LobbyFriend *theFriend, int theRoomSpecFlags = 0);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FriendChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~FriendChangedEvent();

public:
	LobbyFriendPtr mFriend;

	FriendChangedEvent(LobbyFriend *theFriend, LobbyChangeType theChangeType);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetLostPasswordEvent : public LobbyEvent
{
public:
	std::wstring mUserName;

public:
	GetLostPasswordEvent(const GUIString &theUserName) : 
	  LobbyEvent(LobbyEvent_GetLostPassword), mUserName(theUserName) { }
};
typedef SmartPtr<GetLostPasswordEvent> GetLostPasswordEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetLostUserNameEvent : public LobbyEvent
{
public:
	std::string mEmail;

public:
	GetLostUserNameEvent(const GUIString &theEmail) : 
	  LobbyEvent(LobbyEvent_GetLostUserName), mEmail(theEmail) { }
};
typedef SmartPtr<GetLostUserNameEvent> GetLostUserNameEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameActionEvent : public LobbyEvent
{
protected:
	virtual ~GameActionEvent();

public:
	LobbyGameAction mAction;
	LobbyGamePtr mGame;

	GameActionEvent(LobbyGameAction theAction, LobbyGame *theGame);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GameChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~GameChangedEvent();

public:
	GameChangedEvent(LobbyGame *theGame, LobbyChangeType theType);

	LobbyGamePtr mGame;
};
typedef SmartPtr<GameChangedEvent> GameChangedEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GroupChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~GroupChangedEvent();

public:
	GroupChangedEvent(LobbyGroup *theGroup, LobbyServer *theServer, LobbyChangeType theType);

	LobbyServerPtr mServer;
	LobbyGroupPtr mGroup;
};
typedef SmartPtr<GroupChangedEvent> GroupChangedEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GroupInvitationEvent : public LobbyEvent
{
protected:
	virtual ~GroupInvitationEvent();

public:
	LobbyRoomSpec mRoomSpec;
	GUIString mGroupName;
	unsigned short mGroupId;
	LobbyClientPtr mClient;
	bool mInvited;
	bool mFromMe;
	GUIString mComment;

	GroupInvitationEvent(int theRoomSpecFlags, const GUIString &theGroupName, unsigned short theGroupId, 
		LobbyClient *theClient, bool invited, const GUIString &theComment, bool fromMe);
};
typedef SmartPtr<GroupInvitationEvent> GroupInvitationEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GroupJoinAttemptEvent : public LobbyEvent
{
protected:
	virtual ~GroupJoinAttemptEvent();

public:
	LobbyRoomSpec mRoomSpec;
	LobbyClientPtr mClient;
	GUIString mComment;

	GroupJoinAttemptEvent(int theRoomSpecFlags, LobbyClient *theClient, const GUIString &theComment);
};
typedef SmartPtr<GroupJoinAttemptEvent> GroupJoinAttemptEventPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GotHTTPDocumentEvent : public LobbyEvent
{
protected:
	virtual ~GotHTTPDocumentEvent();

public:
	GotHTTPDocumentEvent(GetHTTPDocumentOp *theOp);

	GetHTTPDocumentOpPtr mOp;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JoinFriendsRoomEvent : public LobbyEvent
{
protected:
	virtual ~JoinFriendsRoomEvent();

public:
	LobbyFriendPtr mFriend;

	JoinFriendsRoomEvent(LobbyFriend *theFriend);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JoinGameEvent : public LobbyEvent
{
protected:
	virtual ~JoinGameEvent();

public:
	LobbyGamePtr mGame;

	JoinGameEvent(LobbyGame *theGame);
};
typedef SmartPtr<JoinGameEvent> JoinGameEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JoinRoomEvent : public LobbyEvent
{
public:
	IPAddr mServerAddr;
	int mGroupId;

	JoinRoomEvent(const IPAddr &theAddr, int theGroupId) : 
		LobbyEvent(LobbyEvent_JoinRoom), mServerAddr(theAddr), mGroupId(theGroupId) { }
};
typedef SmartPtr<JoinRoomEvent> JoinRoomEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JoinedRoomEvent : public LobbyEvent
{
protected:
	virtual ~JoinedRoomEvent();

public:
	LobbyClientListPtr mClientList;
	GUIString mServerName;
	GUIString mRoomName;
	unsigned short mMyClientId;
	LobbyRoomSpec mRoomSpec;

	JoinedRoomEvent(const GUIString &theServerName, const GUIString &theRoomName, 
		const LobbyRoomSpec& theRoomSpec, unsigned short myClientId, LobbyClientList *theClientList);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class JumpToRoomEvent : public LobbyEvent
{
public:
	GUIString mRoomName;
	unsigned short mGroupId;
	IPAddr mIPAddr;

	JumpToRoomEvent(const GUIString &theName, unsigned short theId, const IPAddr &theAddr) : 
		LobbyEvent(LobbyEvent_JumpToRoom), mRoomName(theName), mGroupId(theId), mIPAddr(theAddr) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LeftRoomEvent : public LobbyEvent
{
public:
	LobbyRoomSpec mRoomSpec;
	bool mDisconnect;
	bool mDisplayMessage;

	LeftRoomEvent(const LobbyRoomSpec& theRoomSpec, bool disconnect = false, bool displayMessage = true) : LobbyEvent(LobbyEvent_LeftRoom), 
		mRoomSpec(theRoomSpec), mDisconnect(disconnect), mDisplayMessage(displayMessage) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoginEvent : public LobbyEvent
{
public:
	std::wstring mUserName;
	std::wstring mPassword;
	bool mRememberPassword;

public:
	LoginEvent(const GUIString &theUserName, const GUIString &thePassword, bool rememberPassword) : 
	  LobbyEvent(LobbyEvent_Login), mUserName(theUserName), mPassword(thePassword),
	  mRememberPassword(rememberPassword) { }
};
typedef SmartPtr<LoginEvent> LoginEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LoginInitStatusType
{
	LoginInitStatus_None = 0,
	LoginInitStatus_InitStart,
	LoginInitStatus_InitSuccess,
	LoginInitStatus_InitFailed
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LoginInitStatusEvent : public LobbyEvent
{
public:
	GUIString mStatus;
	GUIString mDetailError;
	bool mAllowIgnore;
	LoginInitStatusType mStatusType;

	LoginInitStatusEvent(const GUIString &theStatus, LoginInitStatusType theStatusType = LoginInitStatus_None, const GUIString &theDetailError = GUIString::EMPTY_STR, bool allowIgnore = false) :
		LobbyEvent(LobbyEvent_LoginInitStatus), mStatus(theStatus), mStatusType(theStatusType), mDetailError(theDetailError), mAllowIgnore(allowIgnore) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PingGameEvent : public LobbyEvent
{
protected:
	virtual ~PingGameEvent();

public:
	LobbyGamePtr mGame;
	bool mGetDetails;

	PingGameEvent(LobbyGame *theGame, bool getDetails);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PlayerChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~PlayerChangedEvent();

public:
	LobbyClientPtr mClient;

	PlayerChangedEvent(LobbyClient *theClient, LobbyChangeType theChangeType);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class QueryAccountResultEvent : public LobbyEvent
{
public:
	GUIString mEmail;
	bool mSubscribeSierraNewsletter;

public:
	QueryAccountResultEvent(const GUIString &theEmail, bool subscribe) :
	  LobbyEvent(LobbyEvent_QueryAccountResult), mEmail(theEmail), 
		  mSubscribeSierraNewsletter(subscribe) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class QueryServerGroupsEvent : public LobbyEvent
{
public:
	IPAddr mServerAddr;

	QueryServerGroupsEvent(const IPAddr &theAddr) : 
		LobbyEvent(LobbyEvent_QueryServerGroups), mServerAddr(theAddr) {  }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class QuickPlayEvent : public LobbyEvent
{
public:
	LobbySkillLevel mSkillLevel;
	
	QuickPlayEvent(LobbySkillLevel theSkillLevel) : LobbyEvent(LobbyEvent_QuickPlay), mSkillLevel(theSkillLevel) { }
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RecvChatEvent : public LobbyEvent
{
protected:
	virtual ~RecvChatEvent();

public:
	LobbyClientPtr mSender;
	LobbyClientPtr mDestClient; // is null unless chat is a whisper

	LobbyRoomSpec mRoomSpec;
	int mChatFlags;
	GUIString mText;

	RecvChatEvent(const LobbyRoomSpec& theRoomSpec, int theChatFlags, LobbyClient *theSender, const GUIString &theText, LobbyClient *theDestClient);
	bool IsWhisper();
	bool IsEmote();
	bool IsWarning();
	bool IsTeam();
};
typedef SmartPtr<RecvChatEvent> RecvChatEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServerChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~ServerChangedEvent();

public:
	ServerChangedEvent(LobbyServer *theServer, LobbyChangeType theType);

	LobbyServerPtr mServer;
};
typedef SmartPtr<ServerChangedEvent> ServerChangedEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetChatInputEvent : public LobbyEvent
{
public:
	LobbyChatCommand mCommand;
	GUIString mName;
	LobbyRoomSpec mRoomSpec;
	bool mHandled;

	SetChatInputEvent(LobbyChatCommand theCommand, const GUIString &theName, int theSpecFlags) :
		LobbyEvent(LobbyEvent_SetChatInput), mCommand(theCommand), mName(theName), mRoomSpec(theSpecFlags),
		mHandled(false) {}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetCrossPromotionEvent : public LobbyEvent
{
public:
	Image* mImage;
	std::string mLink;

public:
	SetCrossPromotionEvent(Image* theImage, const std::string& theLink) : 
	  LobbyEvent(LobbyEvent_SetCrossPromotion), mImage(theImage), mLink(theLink) { }
};
typedef SmartPtr<SetCrossPromotionEvent> SetCrossPromotionEventPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetLoginFieldsEvent : public LobbyEvent
{
public:
	GUIString mUserName;
	GUIString mPassword;
	bool mRememberPassword;

	SetLoginFieldsEvent(const GUIString &theUserName, const GUIString &thePassword, bool rememberPassword) :
		LobbyEvent(LobbyEvent_SetLoginFields), 
		mUserName(theUserName), mPassword(thePassword), mRememberPassword(rememberPassword) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SetRoomSpecFlagsEvent : public LobbyEvent
{
public:
	int mSpecFlags;
	
	SetRoomSpecFlagsEvent(int theFlags) : LobbyEvent(LobbyEvent_SetRoomSpecFlags), mSpecFlags(theFlags) { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SyncGameListEvent : public LobbyEvent
{
protected:
	virtual ~SyncGameListEvent();

public:
	LobbyGameListPtr mGameList;

	SyncGameListEvent(LobbyGameList *theGameList);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PlayerTeamChangedEvent : public LobbyChangeEvent
{
protected:
	virtual ~PlayerTeamChangedEvent();

public:
	LobbyClientPtr mClient;
	PlayerTeamChangedEvent(LobbyClient *theClient, LobbyChangeType theChangeType);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class UpdateAccountEvent : public LobbyEvent
{
public:
	GUIString mEmail;
	GUIString mPassword;
	bool mSubscribeSierraNewsletter;


public:
	UpdateAccountEvent(const GUIString &theEmail, const GUIString &thePassword, bool subscribe) :
	  LobbyEvent(LobbyEvent_UpdateAccount), mEmail(theEmail), mPassword(thePassword),
		  mSubscribeSierraNewsletter(subscribe) { }
};


}; // namespace WONAPI

#endif