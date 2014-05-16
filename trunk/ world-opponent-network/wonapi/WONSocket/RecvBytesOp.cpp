#include "RecvBytesOp.h"
using namespace WONAPI;

static const int MAX_SINGLE_RECV = 64000;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
RecvBytesOp::RecvBytesOp(DWORD theNumBytes, AsyncSocket *theSocket) : SocketOp(theSocket)
{
	mSocketEvent[SocketEvent_Read] = true;

	mNumBytes = theNumBytes;
	mFile = NULL;
	mAppendFile = false;
	mBinaryFile = true;
	mSkipBytes = false;
	mFileModifyTime = 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void RecvBytesOp::SetRecvChunkCompletion(OpCompletionBase *theCompletion, DWORD theChunkSize)
{
	mRecvChunkCompletion = theCompletion;
	mRecvChunkSize = theChunkSize;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
WONStatus RecvBytesOp::StartRecvBytes()
{
	mCurRecv = 0;
	mCurRecvChunk = 0;
	mCloseFile = false;
	if(mFile==NULL && !mFilePath.empty())
	{
		if(mBinaryFile)
			mFile = fopen(mFilePath.c_str(),mAppendFile?"a+b":"wb");
		else
			mFile = fopen(mFilePath.c_str(),mAppendFile?"a+":"w");

		if(mFile==NULL)
			return WS_FailedToOpenFile;
		
		mCloseFile = true;
	}

	if(mNumBytes>0 && mFile==NULL && !mSkipBytes)
	{
		if(mNumBytes<MAX_SINGLE_RECV)
			mCurBytes.Reserve(mNumBytes); 
		else
			mCurBytes.Reserve(MAX_SINGLE_RECV); 
	}

	return ContinueRecvBytes();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
WONStatus RecvBytesOp::ContinueRecvBytes()
{
	while(true)
	{
		mSocket->WaitForRead(TimeLeft());

		int aRecvLen = 0;
		WONStatus aStatus;
		
		if(mNumBytes>0 && mFile==NULL && !mSkipBytes) // receive specific amount to memory
		{
			int aNumBytesToReceive = mNumBytes - mCurBytes.length();
			if(aNumBytesToReceive > MAX_SINGLE_RECV)
				aNumBytesToReceive = MAX_SINGLE_RECV;

			mCurBytes.ReserveChunk(mCurBytes.length() + aNumBytesToReceive, mNumBytes);
			aStatus = mSocket->RecvBytes(mCurBytes.data() + mCurBytes.length(), aNumBytesToReceive, &aRecvLen);		
			if(aRecvLen>0)
				mCurBytes.SkipBytes(aRecvLen);
		}
		else // receive non-specific amount and/or to file
		{
			char aBuf[1024];
			int aMaxRecv = 1024;
			
			if(mNumBytes>0)
			{
				aMaxRecv = mNumBytes - mCurRecv;
				if(aMaxRecv>1024)
					aMaxRecv = 1024;
				if(aMaxRecv<0)
					aMaxRecv = 0;
			}

			aStatus = mSocket->RecvBytes(aBuf,aMaxRecv,&aRecvLen);
			if(aRecvLen>0 && !mSkipBytes)
			{
				if(mFile!=NULL)
				{
					size_t aNumWritten = fwrite(aBuf,1,aRecvLen,mFile);
					if(aNumWritten!=aRecvLen)
						return FinishRecvBytes(WS_FailedToWriteToFile);
				}
				else
					mCurBytes.AppendBytes(aBuf,aRecvLen);
			}
		}

		mCurRecv+=aRecvLen;

		// Handle receive chunk notification
		if(aRecvLen>0 && mRecvChunkCompletion.get()!=NULL)
		{
			mCurRecvChunk+=aRecvLen;
			if(mCurRecvChunk > mRecvChunkSize)
			{
				RecvChunkOpPtr aRecvChunk = new RecvChunkOp(this);
				aRecvChunk->SetCompletion(mRecvChunkCompletion);
				aRecvChunk->Run(GetMode(),0);

				mCurRecvChunk%=mRecvChunkSize;
			}
		}

		if(mNumBytes==0) // Unknown size --> Check for graceful shutdown for finish
		{
			if(aStatus==WS_AsyncSocket_Shutdown)
				return FinishRecvBytes(WS_Success);
		}
		else if(mCurRecv==mNumBytes)
			return FinishRecvBytes(WS_Success);
	
		if(aStatus==WS_TimedOut)
		{
			if(TimeLeft()==0)
				return WS_TimedOut;
		}
		else if(aStatus!=WS_Success)
			return aStatus;	
	}
	
	return WS_Success;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
WONStatus RecvBytesOp::CleanupFile(WONStatus theStatus)
{
	if(mFile!=NULL)
	{
		if(mCloseFile)
		{
			if(fclose(mFile)!=0)
				theStatus = WS_FailedToWriteToFile;
		}

		if(mFileModifyTime!=0)
		{
			utimbuf times = { mFileModifyTime, mFileModifyTime };
			utime(mFilePath.c_str(), &times );
		}

		mFile = NULL;
	}

	return theStatus;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
WONStatus RecvBytesOp::FinishRecvBytes(WONStatus theStatus)
{
	if(mFile==NULL)
	{
		mRecvBytes = mCurBytes.ToByteBuffer();
		return theStatus;
	}
	else
		return CleanupFile(theStatus);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void RecvBytesOp::CleanupHook()
{
	SocketOp::CleanupHook();
	CleanupFile(GetStatus());
}
