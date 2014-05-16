#define __WON_MASTER_CPP__

#include "WONCommon/WriteBuffer.h"
#include "RoutingGetUserStatusOp.h"
#include "WONRouting/RoutingTypes.h"
#include "WONSocket/SendBytesToOp.h"
#include "WONSocket/RecvBytesFromOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoutingGetUserStatusOp::RoutingGetUserStatusOp(const IPAddr& theAddress, UDPManager *theManager)
{
	SetDestAddr(theAddress);
	SetUDPManager(theManager);
	SetNumRetransmits(3);
	SetMsgTimeout(1000);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingGetUserStatusOp::AddWONUserId(unsigned long theUserId)
{
	mFriendStatusMap.insert(FriendStatusMap::value_type(theUserId, 0));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingGetUserStatusOp::RunHook()
{
	// Send the request
	WriteBuffer aBuf;		// no length field size
	aBuf.AppendByte(3);		// mini message
	aBuf.AppendByte(7);		// RoutingServerG2
	aBuf.AppendByte(RoutingIsUserPresentRequest);	
	
	aBuf.AppendByte(mFriendStatusMap.size());
	FriendStatusMap::const_iterator anItr = mFriendStatusMap.begin();
	for (; anItr != mFriendStatusMap.end(); ++anItr)
		aBuf.AppendLong(anItr->first);

	mSendMsg = aBuf.ToByteBuffer();
	UDPManagerSendRecvOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingGetUserStatusOp::UnpackMsg(ReadBuffer &theMsg)
{
	try
	{	
		// Read the header
		if(theMsg.ReadByte()!=3) return WS_None;	// mini message = 3
		if(theMsg.ReadByte()!=7) return WS_None;	// routingg2 = 7
		if(theMsg.ReadByte()!=RoutingIsUserPresentReply) return WS_None;
	
		// Should be one response for each userid sent up
		unsigned char numUsers	= theMsg.ReadByte();
		if (numUsers != mFriendStatusMap.size())
			return WS_InvalidMessage;

		FriendStatusMap::iterator anItr = mFriendStatusMap.begin();
		for (int i=0; i < numUsers; ++i)
		{
			unsigned char aFlag		= theMsg.ReadByte();

			anItr->second = aFlag;
			++anItr;
		}
	}
	catch(ReadBufferException&)
	{
		return WS_MessageUnpackFailure;
	}

	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
