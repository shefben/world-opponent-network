#define __WON_MASTER_CPP__ // compile seperately so WONInternal doesn't require WONMsg
#include "BaseMsgRequestOp.h"
#include "WONCommon/WriteBuffer.h"
#include "msg/tmessage.h"
#include "msg/BadMsgException.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseMsgRequestOp::BaseMsgRequestOp(ServerContext *theContext) : 
	ServerRequestOp(theContext)
{
	mRequestMsg = NULL;
	mResponseMsg = NULL;
	mCheckReply = NULL;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseMsgRequestOp::BaseMsgRequestOp(const IPAddr &theAddr) :
	ServerRequestOp(theAddr)
{
	mRequestMsg = NULL;
	mResponseMsg = NULL;
	mCheckReply = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BaseMsgRequestOp::~BaseMsgRequestOp()
{
	delete mRequestMsg;
	delete mResponseMsg;

	while (! mResponseList.empty())
	{
		WONMsg::BaseMessage* aMsgP = mResponseList.back();
		mResponseList.pop_back();
		delete aMsgP;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BaseMsgRequestOp::SetRequestMsg(const WONMsg::BaseMessage &theRequest)
{
	if(mRequestMsg!=NULL)
		delete mRequestMsg;

	mRequestMsg = (WONMsg::BaseMessage*)theRequest.Duplicate();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BaseMsgRequestOp::SetResponseMsg(const WONMsg::BaseMessage &theResponse)
{
	if(mResponseMsg!=NULL)
		delete mResponseMsg;

	mResponseMsg = (WONMsg::BaseMessage*)theResponse.Duplicate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus BaseMsgRequestOp::GetNextRequest()
{ 
	if(mRequestMsg==NULL)
		return WS_BaseMsgRequest_NoRequest;

	mRequestMsg->Pack();
	WriteBuffer aMsg(mLengthFieldSize);
	aMsg.AppendBytes(mRequestMsg->GetDataPtr(),mRequestMsg->GetDataLen());
	mRequest = aMsg.ToByteBuffer();

	return WS_ServerReq_Recv; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus BaseMsgRequestOp::CheckResponse()
{
	if(mResponseMsg==NULL)
	{
		if(mResponse->length()==0)
			return WS_BaseMsgRequest_BadHeaderType;

		unsigned char aHeaderType = *(unsigned char*)(mResponse->data());
		mResponseMsg = WONMsg::BaseMessage::GetMsgOfType(aHeaderType);
		if(mResponseMsg==NULL)
			return WS_BaseMsgRequest_BadHeaderType;
	}

	mResponseMsg->ResetWritePointer();
	mResponseMsg->AppendBytes(mResponse->length(),mResponse->data(),false,false);
	try
	{
		mResponseMsg->Unpack();
	}
	catch(WONMsg::BadMsgException&)
	{
		return WS_BaseMsgRequest_UnpackFailure;
	}

	if (mCheckReply)
	{
		mResponseList.push_back((WONMsg::BaseMessage*)mResponseMsg->Duplicate());
		return ((*mCheckReply)(*mRequestMsg, *mResponseMsg) ? WS_ServerReq_Recv : WS_Success);
	}
	else
		return WS_Success;
}

