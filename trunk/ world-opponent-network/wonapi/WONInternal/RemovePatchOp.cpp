#include "RemovePatchOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RemovePatchOp::RemovePatchOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(13)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RemovePatchOp::RemovePatchOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(13)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RemovePatchOp::CheckResponse()
{
	// Call the base class implementation
	WONStatus result = DBProxyOp::CheckResponse();
	if (result != WS_Success)
		return result;
	
	if (mSubMessageReplyType != mMsgType+1)
		return InvalidReplyHeader();
	
	// Do extended unpack

	// Finished
	return WS_Success;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemovePatchOp::RunHook()
{
	SetMessageType(DBProxyPatchServer);
	SetSubMessageType(mMsgType);

	// Pack the message data
	WriteBuffer requestData;
	requestData.AppendString(mProductName);
	requestData.AppendString(mConfigName);
	requestData.AppendString(mFromVersion);
	requestData.AppendString(mToVersion);
	requestData.AppendString(mNetAddress);		// patch url

	// Pack and call base class implementation
	SetProxyRequestData(requestData.ToByteBuffer());
	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemovePatchOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;

	mProductName	= "";
	mConfigName		= "";
	mFromVersion	= "";
	mToVersion		= "";
	mNetAddress		= "";		// patch url
}

