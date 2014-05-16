#include "ReportPatchStatusOp.h"
#include "WONSocket/SendBytesToOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReportPatchStatusOp::ReportPatchStatusOp(const IPAddr& theAddr)
	: DBProxyOp(theAddr),
	  mMsgType(19)
{
	Init();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ReportPatchStatusOp::CheckResponse()
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
void ReportPatchStatusOp::RunHook()
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
	requestData.AppendByte(mPatchStatus);

	// Pack and call base class implementation
	SetProxyRequestData(requestData.ToByteBuffer());

	if(mUDPSocket.get()==NULL) // just do TCP ServerRequest
	{
		DBProxyOp::RunHook();
		return;
	}

	// Do UDP server request

	IPAddr anAddr = GetAddr();
	if(!anAddr.IsValid())
	{
		Finish(WS_ServerReq_NoServersSpecified);
		return;
	}

	Reset();
	unsigned char aLengthFieldSize = GetLengthFieldSize();
	SetLengthFieldSize(0);
	GetNextRequest();
	SetLengthFieldSize(aLengthFieldSize);
	SendBytesToOpPtr anOp = new SendBytesToOp(mRequest, anAddr, mUDPSocket);
	anOp->Run(GetMode(),GetTimeout());
	Finish(WS_Success);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReportPatchStatusOp::Init()
{
	Reset();
	ServerRequestOp::mLengthFieldSize = 4;

	mProductName	= "";
	mConfigName		= "";
	mFromVersion	= "";
	mToVersion		= "";
	mNetAddress		= "";		// patch url
	mPatchStatus	= 0;
}

