#ifndef __WON_LOBBYGAMEMSG_H__
#define __WON_LOBBYGAMEMSG_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyGameMsg
{
	LobbyGameMsg_JoinRequest				= 1,
	LobbyGameMsg_JoinReply					= 2,
	LobbyGameMsg_PlayerJoined				= 3,
	LobbyGameMsg_ReadyRequest				= 4,
	LobbyGameMsg_PlayerReady				= 5,
	LobbyGameMsg_DissolveGame				= 6,
	LobbyGameMsg_StartGame					= 7,
	LobbyGameMsg_PingChangedRequest			= 8,
	LobbyGameMsg_PingChanged				= 9,
	LobbyGameMsg_ClientKicked				= 10,

	LobbyGameMsg_Max						= 100 // reserve first 100 messages
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LobbyGameStatus
{
	LobbyGameStatus_Success					= 0,
	LobbyGameStatus_None					= 1,				
	LobbyGameStatus_GameFull				= 2,
	LobbyGameStatus_GameInProgress			= 3,
	LobbyGameStatus_UnpackFailure			= 4,
	LobbyGameStatus_NotInvited				= 5,
	LobbyGameStatus_CaptainRejectedYou		= 6,
	LobbyGameStatus_InvalidPassword			= 7,
	LobbyGameStatus_DuplicateName			= 8,

	LobbyGameStatus_Max						= 10000 // reserve the first 10000 codes for the lobby
};


} // namespace WONAPI

#endif