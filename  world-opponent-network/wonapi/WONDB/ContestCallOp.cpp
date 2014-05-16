#include "ContestCallOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ContestCallOp::Init()
{
	mProcNum = 0;
	mLengthFieldSize = 2;
	SetUseAuth2(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContestCallOp::ContestCallOp(ServerContext *theContestContext) : ServerRequestOp(theContestContext)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ContestCallOp::ContestCallOp(const IPAddr &theAddr) : ServerRequestOp(theAddr)
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ContestCallOp::RunHook()
{
	if(mAuthType==AUTH_TYPE_NONE)
	{
		Finish(WS_ServerReq_NeedAuthContext);
		return;
	}

	ServerRequestOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ContestCallOp::GetNextRequest()
{
	WriteBuffer aBuf(mLengthFieldSize);

	aBuf.AppendByte(3);			// mini message
	aBuf.AppendByte(4);		// Contest Service
	aBuf.AppendByte(1);		// ContestDBCall

	aBuf.AppendLong(mAuthContext->GetPeerData()->GetCertificate()->GetUserId()); // WONUserSeq
	aBuf.AppendShort(mProcNum); // Contest DB Procedure number

	for(int i=0; i<16; i++) // 16 Byte GUID (not needed)
		aBuf.AppendByte(0);

	aBuf.AppendBuffer(mRequestData);
	mRequest = aBuf.ToByteBuffer();
	return WS_ServerReq_Recv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus ContestCallOp::CheckResponse()
{
	ReadBuffer aMsg(mResponse->data(),mResponse->length());
	unsigned char aHeaderType = aMsg.ReadByte();
	unsigned short aServiceType = aMsg.ReadByte();
	unsigned short aMessageType = aMsg.ReadByte();
	if(aHeaderType!=3 || aServiceType!=4 || aMessageType!=2)
		return InvalidReplyHeader();

	short aStatus = aMsg.ReadShort();
	mReplyData = new ByteBuffer(aMsg.data() + aMsg.pos(), aMsg.Available());

	return (WONStatus)aStatus;
}

