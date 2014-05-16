#include "RoutingGetClientListOp.h"
#include "RoutingClientJoinedServerOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingGetClientListOp::SendRequest()
{
	mClientMap.clear();
	InitSendMsg(RoutingGetClientListRequest);
	if(mIsEx)
		mSendMsg.AppendBool(true);

	SendMsg();
	AddOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingGetClientListOp::HandleReply(unsigned char theMsgType, ReadBuffer &theMsg)
{
	if(theMsgType!=RoutingGetClientListReply && theMsgType!=RoutingGetClientListReplyEx)
		return WS_RoutingOp_DontWantReply;

	mIsEx = theMsgType==RoutingGetClientListReplyEx;
	WONStatus aStatus = (WONStatus)theMsg.ReadShort();
	if(aStatus==WS_Success)
	{
		unsigned short aNumClients = theMsg.ReadShort();
		for(int i=0; i<aNumClients; i++)
		{
			RoutingClientInfoPtr anInfo = GetNewClientInfo();
			anInfo->mId = theMsg.ReadShort();
			theMsg.ReadWString(anInfo->mName);
			anInfo->mFlags = theMsg.ReadLong();
			if(mIsEx)
				RoutingClientJoinedServerOp::ParseExClientData(theMsg,anInfo);

			mClientMap[anInfo->mId] = anInfo;
		}
	}

	return aStatus;
}
