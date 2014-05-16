#ifndef __WON_LOBBYTYPES_H__
#define __WON_LOBBYTYPES_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum CloseStatusDialogType
{
	CloseStatusDialogType_None,
	CloseStatusDialogType_Success,
	CloseStatusDialogType_Any
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyChangeType
{
	LobbyChangeType_Add,
	LobbyChangeType_Delete,
	LobbyChangeType_Modify
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyGameType
{
	LobbyGameType_None,
	LobbyGameType_Lan,
	LobbyGameType_Internet,
	LobbyGameType_DirectConnect
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbySkillLevel
{
	LobbySkillLevel_None		= 0,
	LobbySkillLevel_Novice,
	LobbySkillLevel_Intermediate,
	LobbySkillLevel_Advanced,
	LobbySkillLevel_Expert,
	LobbySkillLevel_Max
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum 
{
	LobbyServerId_Invalid		= 0xffff,

	LobbyPing_Failed			= 0xffff,
	LobbyPing_None				= 0xfffe,
	LobbyPing_Pinging			= 0xfffd,
	LobbyPing_PingingDetails	= 0xfffc,
	LobbyPing_Max				= 0xf000
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyRoomSpecFlags
{
	LobbyRoomSpecFlag_Chat				=	0x0001,
	LobbyRoomSpecFlag_Game				=	0x0002,
	LobbyRoomSpecFlag_Internet			=	0x0004,
	LobbyRoomSpecFlag_Lan				=	0x0008,
	LobbyRoomSpecFlag_Team				=	0x0010
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Each room spec flag narrows down the scope.  So for instance if something needs to
// go to both a game room and a chat room, then don't specify the game flag or the chat flag.
class LobbyRoomSpec
{
protected:
	int mSpecFlags;

public:
	LobbyRoomSpec(int theFlags = 0) : mSpecFlags(theFlags) { }
	void SetSpecFlags(int theFlags) { mSpecFlags = theFlags; }

	operator int() const { return mSpecFlags; }

	bool IsChatRoom() const { return mSpecFlags&LobbyRoomSpecFlag_Chat?true:false; }
	bool IsGameRoom() const { return mSpecFlags&LobbyRoomSpecFlag_Game?true:false; }
	bool IsInternet() const { return mSpecFlags&LobbyRoomSpecFlag_Internet?true:false; }
	bool IsLan()      const { return mSpecFlags&LobbyRoomSpecFlag_Lan?true:false; }
	bool IsTeam()     const { return mSpecFlags&LobbyRoomSpecFlag_Team?true:false; }

	bool Allows(int theSpecFlags) const { return (theSpecFlags&mSpecFlags)==mSpecFlags; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyChatFlags // these match the corresponding RoutingChatFlags
{
	LobbyChatFlag_IsEmote				=	0x0002,
	LobbyChatFlag_IsWhisper				=	0x0004,
	LobbyChatFlag_IsWarning				=	0x0010,
	
	LobbyChatFlag_TeamChat				=	0x0100 // developer specific routing chat flags are in the upper byte

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyChatCommand
{
	LobbyChatCommand_None,
	LobbyChatCommand_Broadcast,
	LobbyChatCommand_Whisper,
	LobbyChatCommand_Emote,
	LobbyChatCommand_Block,
	LobbyChatCommand_Ignore,
	LobbyChatCommand_Away,
	LobbyChatCommand_BecomeModerator,
	LobbyChatCommand_ServerMute,
	LobbyChatCommand_ServerBan,
	LobbyChatCommand_Mute,
	LobbyChatCommand_Ban,
	LobbyChatCommand_Unmute,
	LobbyChatCommand_ServerUnmute,
	LobbyChatCommand_Unban,
	LobbyChatCommand_ServerUnban,
	LobbyChatCommand_Invite,
	LobbyChatCommand_Uninvite,
	LobbyChatCommand_Reply,
	LobbyChatCommand_Clear,
	LobbyChatCommand_Help,
	LobbyChatCommand_ShowTeam,
	LobbyChatCommand_Alert,
	LobbyChatCommand_StartShutdown,
	LobbyChatCommand_AbortShutdown,
	LobbyChatCommand_Warn
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyClientAction
{
	LobbyClientAction_None,
	LobbyClientAction_AddToFriends,
	LobbyClientAction_Ban,
	LobbyClientAction_Block,
	LobbyClientAction_ClearAFK,
	LobbyClientAction_Ignore,
	LobbyClientAction_Invite,
	LobbyClientAction_Kick,
	LobbyClientAction_Mute,
	LobbyClientAction_RemoveFromFriends,
	LobbyClientAction_SetAFK,
	LobbyClientAction_Unblock,
	LobbyClientAction_Unignore,	
	LobbyClientAction_Uninvite,
	LobbyClientAction_Unmute,
	LobbyClientAction_Warn,
	LobbyClientAction_Whisper,
	LobbyClientAction_Locate,

	LobbyClientAction_ShowMenu
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyFriendAction
{
	LobbyFriendAction_None,
	LobbyFriendAction_Locate,
	LobbyFriendAction_RefreshList,
	LobbyFriendAction_Remove,

	LobbyFriendAction_ShowMenu,
	LobbyFriendAction_ShowMenuTryClient // Shows client menu instead of friend menu if the client is on the server
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyGameAction
{
	LobbyGameAction_None,
	LobbyGameAction_Join,
	LobbyGameAction_Ping,
	LobbyGameAction_QueryDetails,
	LobbyGameAction_ShowDetails,
	LobbyGameAction_HideDetails,
	LobbyGameAction_Remove,

	LobbyGameAction_ShowMenu
};


} // namespace WONAPI


#endif