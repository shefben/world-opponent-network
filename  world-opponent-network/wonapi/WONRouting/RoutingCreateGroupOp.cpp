#include "RoutingCreateGroupOp.h"
#include "RoutingLeaveGroupOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoutingCreateGroupOp::RoutingCreateGroupOp(RoutingConnection *theConnection) : RoutingOp(theConnection)
{
	mGroupId = 0xFFFF;
	mParentGroupId = 0xFFFF;
	mMaxPlayers  = 0;
	mJoinFlags = 0;
	mGroupFlags = 0;
	mAsyncFlags = 0;
	mLeaveGroupOnCancel = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingCreateGroupOp::SendRequest()
{
	InitSendMsg(RoutingCreateGroupRequest);
	mSendMsg.AppendShort(mGroupId);
	mSendMsg.AppendWString(mGroupName);
	mSendMsg.AppendWString(mGroupPassword);
	mSendMsg.AppendShort(mMaxPlayers);
	mSendMsg.AppendByte(mJoinFlags);
	mSendMsg.AppendLong(mGroupFlags);
	mSendMsg.AppendLong(mAsyncFlags);
	mSendMsg.AppendShort(mParentGroupId);
	SendMsg();
	AddOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingCreateGroupOp::CheckLeaveGroup(short theStatus)
{
	if(theStatus==WS_Success && !Pending() && mLeaveGroupOnCancel) // op was canceled
	{
		mLeaveGroupOnCancel = false;
		RoutingLeaveGroupOpPtr anOp = new RoutingLeaveGroupOp(GetConnection(), mGroupId);
		anOp->RunAsync(OP_TIMEOUT_INFINITE);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingCreateGroupOp::HandleReply(unsigned char theMsgType, ReadBuffer &theMsg)
{
	if(theMsgType==RoutingJoinGroupReply) // Handle JoinGroup Reply 
	{
		if(mJoinFlags==0)
			return WS_RoutingOp_DontWantReply;

		short aStatus = theMsg.ReadShort();
		CheckLeaveGroup(aStatus);

		return (WONStatus)aStatus;
	}


	if(theMsgType!=RoutingCreateGroupReply) // Handle CreateGroup Reply
		return WS_RoutingOp_DontWantReply;

	WONStatus aStatus = (WONStatus)theMsg.ReadShort();
	if(aStatus==WS_Success)
	{
		mGroupId = theMsg.ReadShort();
		theMsg.ReadWString(mGroupName);

		CheckLeaveGroup(aStatus);

		if(mJoinFlags!=0)
			return WS_RoutingOp_NeedMoreReplies;
	}

	if(theMsg.HasMoreBytes())
		mParentGroupId = theMsg.ReadShort();

	return aStatus;
}
