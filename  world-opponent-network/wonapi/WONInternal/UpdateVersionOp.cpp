
#include "UpdateVersionOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateVersionOp::UpdateVersionOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(7),
	  mProductName("")
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UpdateVersionOp::UpdateVersionOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(7),
	  mProductName("")
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UpdateVersionOp::CheckResponse()
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
void UpdateVersionOp::RunHook()
{
	SetMessageType(DBProxyMsgType::DBProxyPatchServer);
	SetSubMessageType(mMsgType);

	// Pack the message data
	WriteBuffer requestData;
	requestData.AppendString(mProductName);

	mVersionData->WriteToBuffer(requestData, VersionData::WriteFlag_AppendDescripURL | VersionData::WriteFlag_AppendState);
	
	// Pack and call base class implementation
	SetProxyRequestData(requestData.ToByteBuffer());
	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UpdateVersionOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;
	mVersionData = new VersionData;
}
