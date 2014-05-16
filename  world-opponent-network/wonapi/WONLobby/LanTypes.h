#ifndef __WON_LANTYPES_H__
#define __WON_LANTYPES_H__

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LanBroadcastMsgType
{
	LanBroadcastMsgType_GameInfo			= 1,
	LanBroadcastMsgType_Heartbeat			= 2,
	LanBroadcastMsgType_GameDeleted			= 3,
	LanBroadcastMsgType_GameInfoRequest		= 4	
};	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LanMsgType
{
	LanMsgType_RegisterRequest	= 1,
	LanMsgType_RegisterReply	= 2,
	LanMsgType_ClientJoined		= 3,
	LanMsgType_ClientLeft		= 4,
	LanMsgType_ChatRequest		= 5,
	LanMsgType_Chat				= 6,
	LanMsgType_KeepAlive		= 7,
	LanMsgType_GameMessage		= 8
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum LanRegisterStatus
{
	LanRegisterStatus_Success		= 0,
	LanRegisterStatus_None			= 1,
	LanRegisterStatus_BadPassword	= 2,
	LanRegisterStatus_NotOnSubnet	= 3	
};


} // namespace WONAPI

#endif