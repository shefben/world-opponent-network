#include "FilePushOp.h"
#include "WONCommon/WriteBuffer.h"
#include "msg/tmessage.h"
#include "msg/BadMsgException.h"

#include "WONAuth/PeerAuthOp.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FilePushOp::FilePushOp(const IPAddr &theAddr) :
	mConnectAddr(theAddr), mIsCompress(false), mBlockTransferSize(65535)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FilePushOp::~FilePushOp()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::CallbackHook(AsyncOp *theOp, int theId)
{
	if(!theOp->Succeeded())
	{
		Finish(theOp->GetStatus());
		return true;
	}

	switch(theId)
	{
		case TRACK_AUTH: 
			if(GetEncryptKey((PeerAuthOp*)theOp))
				SendStreamFileRequest(); 
			return true;
	
		case TRACK_STATUS_REPLY: 
		{
			RecvMsgOp *anOp = (RecvMsgOp*)theOp;
			HandleFactStatusReply(anOp->GetMsg());
			return true;
		}

		case TRACK_ACK:
		{
			RecvBytesOp *anOp = (RecvBytesOp*)theOp;
			HandleAck(anOp->GetBytes()->data()[0]);
			return true;
		}

	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::CheckError(WONStatus theStatus)
{
	if(theStatus!=WS_Success)
	{
		Finish(theStatus);
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::GetEncryptKey(PeerAuthOp *theOp)
{
	AuthSession *aSession = theOp->GetSession();
	if(aSession==NULL)
	{
		Finish(WS_AuthRequired);
		return false;
	}

	mEncryptKey = aSession->GetKey();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FilePushOp::DoProgressCompletion()
{
	if(mProgressCompletion.get()!=NULL)
		mProgressCompletion->Complete(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::HandleFactStatusReply(const ByteBuffer *theReply)
{
	ReadBuffer aReply(theReply->data(), theReply->length());
	WONStatus aStatus = WS_None;
	try
	{
		unsigned char aHeaderType = aReply.ReadByte();
		unsigned short aServiceType = aReply.ReadShort();
		unsigned short aMessageType = aReply.ReadShort();
		aStatus = (WONStatus)aReply.ReadShort();
		if(CheckError(aStatus))
			return false;
	}
	catch(ReadBufferException&)
	{
		Finish(WS_MessageUnpackFailure);
		return false;
	}

	if(mTotalBytesSent > 0) // this is the second status reply so we're done
	{
		DoProgressCompletion();
		Finish(WS_Success);
		return false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::RecvFactStatusReply()
{
	if(IsAsync())
	{
		RecvMsgAsync(TRACK_STATUS_REPLY);
		return false;
	}

	ByteBufferPtr aReply;
	WONStatus aStatus = mSocket->RecvMsg(aReply, RecvTime());
	if(CheckError(aStatus))
		return false;

	return HandleFactStatusReply(aReply);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::HandleAck(char theAck)
{
	if(theAck==1)
	{
		DoProgressCompletion();
		if(IsAsync())
			SendFileChunk();

		return true;
	}

	return RecvFactStatusReply();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::RecvAck()
{
	if(IsAsync())
	{
		RecvBytesOpPtr anOp = new RecvBytesOp(1,mSocket);
		Track(anOp, TRACK_ACK);
		mSocket->QueueOp(anOp);
		return false;
	}

	ByteBufferPtr aByte;
	WONStatus aStatus = mSocket->RecvBytes(aByte, 1);
	if(CheckError(aStatus))
		return false;

	return HandleAck(aByte->data()[0]);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::SendFileChunk()
{		
	WriteBuffer aBuf(4,false);
	aBuf.Reserve(mBlockTransferSize + 20);

	int aLen = fread(aBuf.data(),1,mBlockTransferSize,mFile.GetFile());
	WONStatus aStatus = WS_None;

	// Encrypt bytes and pack them in aBuf
	if(aLen > 0)
	{
		ByteBufferPtr anEncrypt = mEncryptKey.Encrypt(aBuf.data(),aLen);
		aBuf.AppendBytes(anEncrypt->data(), anEncrypt->length());
	}

	// Send bytes
	mLastBytesSent = aLen;
	mTotalBytesSent += aLen;
	if(IsAsync())
		mSocket->QueueOp((SocketOp*)Track(new SendBytesOp(aBuf.ToByteBuffer())));
	else
	{
		aStatus = mSocket->SendBytes(aBuf.ToByteBuffer(), SendTime());
		if(CheckError(aStatus))
			return false;
	}

	if(aLen>0)
		return RecvAck();
	else 
		return RecvFactStatusReply();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::SendStreamFileRequest()
{
	WriteBuffer aMsg(mLengthFieldSize);
	aMsg.AppendByte(5);					// Small Message
	aMsg.AppendShort(5);				// Small Factory Service
	aMsg.AppendShort(21);				// FactStreamFile Msg
	aMsg.AppendString(mRemoteFileName);
	aMsg.AppendBool(mIsCompress);
	aMsg.AppendLong(mModifyTime.dwHighDateTime);
	aMsg.AppendLong(mModifyTime.dwLowDateTime);
	aMsg.AppendString(mComment);

	if(IsAsync())
	{
		SendMsgAsync(aMsg.ToByteBuffer(),TRACK_STREAM_REQUEST);
		RecvFactStatusReply();
		RecvAck();
		return false;
	}

	WONStatus aStatus = mSocket->SendMsg(aMsg.ToByteBuffer(), SendTime());
	if(CheckError(aStatus))
		return false;

	if(!RecvFactStatusReply())
		return false;

	return RecvAck();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FilePushOp::RunHook(void)
{
	mTotalBytesSent = 0;
	mLastBytesSent = 0;
	FILE *aFile = fopen(mFileName.c_str(),"rb");
	mFile.Attach(aFile);
	if(aFile==NULL)
	{
		Finish(WS_FailedToOpenFile);
		return;
	}

	mSocket = new BlockingSocket;
	mSocket->SetLengthFieldSize(mLengthFieldSize);

	PeerAuthOpPtr aPeerAuthOp = new PeerAuthOp(mConnectAddr, mAuthContext, mAuthType, mSocket);
	aPeerAuthOp->CopyMaxTimes(this);
	if(IsAsync())
	{
		Track(aPeerAuthOp,TRACK_AUTH);
		aPeerAuthOp->RunAsync(OP_TIMEOUT_INFINITE);
		return;
	}
	
	if(!aPeerAuthOp->RunBlock(TimeLeft()))
	{
		Finish(aPeerAuthOp->GetStatus());
		return;
	}

	if(!GetEncryptKey(aPeerAuthOp))
		return;

	if(!SendStreamFileRequest())
		return;

	while(true)
	{
		if(!SendFileChunk())
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FilePushOp::CleanupHook(void)
{
	ServerOp::CleanupHook();
	mFile.Detach();
}
