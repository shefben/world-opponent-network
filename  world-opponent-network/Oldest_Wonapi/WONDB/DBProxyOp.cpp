
#include "DBProxyOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DBProxyOp::DBProxyOp(ServerContext* theContextP)
	: ServerRequestOp(theContextP),
	  mMessageType(0),
	  mSubMessageType(0),
	  mReplyErrorString("")
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DBProxyOp::DBProxyOp(const IPAddr& theAddr)
	: ServerRequestOp(theAddr),
	  mMessageType(0),
	  mSubMessageType(0),
	  mReplyErrorString("")
{

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus DBProxyOp::GetNextRequest()
{
	WriteBuffer aWriteBuf(mLengthFieldSize);
	aWriteBuf.AppendByte(5);
	aWriteBuf.AppendShort(14);
	aWriteBuf.AppendShort(mMessageType);
	aWriteBuf.AppendShort(mSubMessageType);
	aWriteBuf.AppendBuffer(mRequestData);
	mRequest = aWriteBuf.ToByteBuffer();

	return WS_ServerReq_Recv;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus DBProxyOp::CheckResponse()
{
	ReadBuffer aReadBuffer(mResponse->data(), mResponse->length());
	unsigned char	aHeaderType		= aReadBuffer.ReadByte();
	unsigned short	aServiceType	= aReadBuffer.ReadShort();
	unsigned short	aMessageType	= aReadBuffer.ReadShort();
	mSubMessageReplyType			= aReadBuffer.ReadShort();
	WONStatus		aStatus			= (WONStatus)aReadBuffer.ReadShort();
	aReadBuffer.ReadString(mReplyErrorString);
	unsigned char	aSequence		= aReadBuffer.ReadByte();

	// Any errors?
	if (aStatus != WS_Success)
		return aStatus;

	// Is this a valid response?
	if (aHeaderType != 5 || aServiceType != 14)
		return InvalidReplyHeader();

	// Copy over the data buffer
	mReceivedData.AppendBytes(aReadBuffer.data()+aReadBuffer.pos(), aReadBuffer.length() - aReadBuffer.pos());
	
	// Do we expect any more messages from the server?
	if (aSequence & 0x80)
	{
		mReplyData = mReceivedData.ToByteBuffer();
		return WS_Success;
	}
	else
		return WS_ServerReq_Recv;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DBProxyOp::Reset()
{
	// Clear all received data
	//mReplyData->Release();

	ServerRequestOp::Reset();
}


