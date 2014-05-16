
#include "RemoveVersionOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RemoveVersionOp::RemoveVersionOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(5),
	  mProductName("")
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RemoveVersionOp::RemoveVersionOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(5),
	  mProductName("")
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RemoveVersionOp::CheckResponse()
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
void RemoveVersionOp::RunHook()
{
	SetMessageType(DBProxyPatchServer);
	SetSubMessageType(mMsgType);

	// Pack the message data
	WriteBuffer requestData;
	requestData.AppendString(mProductName);
	requestData.AppendString(mConfigName);
	requestData.AppendString(mVersion);

	// Pack and call base class implementation
	SetProxyRequestData(requestData.ToByteBuffer());
	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RemoveVersionOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;

	mConfigName = "";
	mVersion = "";
}
