
#include "AddVersionOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AddVersionOp::AddVersionOp(ServerContext* theContext)
	: DBProxyOp(theContext),
	  mMsgType(3),
	  mProductName("")
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AddVersionOp::AddVersionOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(3),
	  mProductName("")
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus AddVersionOp::CheckResponse()
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
void AddVersionOp::RunHook()
{
	SetMessageType(DBProxyPatchServer);
	SetSubMessageType(mMsgType);

	// Pack the message data
	WriteBuffer requestData;
	requestData.AppendString(mProductName);

	// Append the version data (don't include state);
	mVersionData->WriteToBuffer(requestData, VersionData::WriteFlag_AppendDescripURL);
	
	// Pack and call base class implementation
	SetProxyRequestData(requestData.ToByteBuffer());
	DBProxyOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AddVersionOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;
	mVersionData = new VersionData;
}
