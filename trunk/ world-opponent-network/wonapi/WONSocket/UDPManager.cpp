#define __WON_MASTER_CPP__
#include "UDPManager.h"
#include "SendBytesToOp.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UDPManagerRecvOp::UDPManagerRecvOp()
{
	mMessageVerifier = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UDPManagerRecvOp::UDPManagerRecvOp(const IPAddr &theRecvFrom)
{
	mFromAddr = theRecvFrom;
	mMessageVerifier = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerRecvOp::SetMessageVerifier(MessageVerifier theVerifier, void *theParam)
{
	mMessageVerifier = theVerifier;
	mMessageVerifierParam = theParam;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UDPManagerRecvOp::CheckMsg(ReadBuffer &theMsg)
{
	if(mMessageVerifier!=NULL)
		return mMessageVerifier(theMsg,mMessageVerifierParam);
	else
		return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerRecvOp::QueueRecvOp()
{
	if(mSocket.get()!=NULL)
	{
		RecvBytesFromOpPtr anOp = new RecvBytesFromOp(1024);
		anOp->SetCompletion(new OpRefCompletion(StaticRecvCompletion,this));
		mSocket->QueueOp(anOp);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerRecvOp::StaticRecvCompletion(AsyncOpPtr theOp, RefCountPtr theManager)
{
	RecvBytesFromOp *anOp = (RecvBytesFromOp*)theOp.get();
	if(!anOp->SucceededAlive())
		return;

	UDPManagerRecvOp *aManager = (UDPManagerRecvOp*)theManager.get();
	aManager->RecvCompletion(anOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerRecvOp::RecvCompletion(RecvBytesFromOp *theOp)
{
	if(!Pending())
		return;

	if(mFromAddr.IsValid() && theOp->GetAddr()!=mFromAddr)
	{
		QueueRecvOp();
		return;
	}

	const ByteBuffer *aBuf = theOp->GetBytes();

	try
	{
		ReadBuffer aMsg(aBuf->data(),aBuf->length());
		WONStatus aStatus = CheckMsg(aMsg);
		if(aStatus!=WS_None)
		{
			Finish(aStatus);
			return;
		}
	}
	catch(ReadBufferException&)
	{
	}
			
	QueueRecvOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerRecvOp::CleanupHook()
{
	if(mSocket.get()!=NULL && mOwnSocket)
	{
		mSocket->Kill();
		mSocket = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerRecvOp::RunHook()
{
	if(GetMode()==OP_MODE_BLOCK)
	{
		Finish(WS_BlockMode_NotSupported);
		return;
	}

	if(mUDPManager.get()!=NULL)
	{
		mUDPManager->QueueOp(this,GetTimeout());
		return;
	}

	if(mSocket.get()==NULL)
	{
		mSocket = new QueueSocket(AsyncSocket::UDP);
		mSocket->Bind(0);
		mOwnSocket = true;
	}
	QueueRecvOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UDPManagerSendRecvOp::UDPManagerSendRecvOp()
{
	mNumRetransmits = 0;
	mMsgTimeout = 10000;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UDPManagerSendRecvOp::StaticUnpackMsg(ReadBuffer &theMsg, void *theParam)
{
	UDPManagerSendRecvOp *anOp = (UDPManagerSendRecvOp*)theParam;
	return anOp->UnpackMsg(theMsg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ByteBuffer* UDPManagerSendRecvOp::GetRecvMsg()
{
	if(mRecvOp.get()==NULL)
		return NULL;
	else
		return mRecvOp->GetMsg();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerSendRecvOp::TrySendRecv()
{
	mRecvOp = new UDPManagerRecvOp(mDestAddr);
	mRecvOp->SetCompletion(new OpRefCompletion(StaticRecvCompletion,this));
	mRecvOp->SetMessageVerifier(StaticUnpackMsg,this);

	if(mUDPManager.get()!=NULL)
	{
		mUDPManager->SendBytesTo(mSendMsg,mDestAddr);
		mRecvOp->SetUDPManager(mUDPManager);
	}
	else
	{
		mSocket->QueueOp(new SendBytesToOp(mSendMsg,mDestAddr));
		mRecvOp->SetSocket(mSocket);
	}

	mRecvOp->RunAsync(mMsgTimeout);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerSendRecvOp::RetransmitHook()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerSendRecvOp::RunHook()
{
	if(GetMode()==OP_MODE_BLOCK)
	{
		Finish(WS_BlockMode_NotSupported);
		return;
	}

	mRetransmitCount = 0;

	if(mUDPManager.get()==NULL)
	{
		mSocket = new QueueSocket(AsyncSocket::UDP);
		mSocket->Bind(0);
	}

	TrySendRecv();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerSendRecvOp::StaticRecvCompletion(AsyncOpPtr theOp, RefCountPtr theManager)
{
	UDPManagerRecvOp *anOp = (UDPManagerRecvOp*)theOp.get();

	UDPManagerSendRecvOp *aManager = (UDPManagerSendRecvOp*)theManager.get();
	aManager->RecvCompletion(anOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerSendRecvOp::RecvCompletion(UDPManagerRecvOp *theOp)
{
	if(!Pending())
		return;

	if(theOp!=mRecvOp.get())
		return;

	if(theOp->GetStatus()!=WS_TimedOut)
	{
		Finish(theOp->GetStatus());
		return;
	}

	if(mRetransmitCount >= mNumRetransmits)
		Finish(WS_NoReply);
	else
	{
		mRetransmitCount++;
		RetransmitHook();
		TrySendRecv();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManagerSendRecvOp::CleanupHook()
{
	if(mRecvOp.get()!=NULL)
	{
		mRecvOp->Kill();
		mRecvOp = NULL;
	}

	if(mSocket.get()!=NULL)
	{
		mSocket->Kill();
		mSocket = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
UDPManager::UDPManager()
{
	mCleanListItr = mOpList.begin();
	mCleanMapItr = mOpMap.begin();
	mMaxRecvBytes = 1024;
	mAutoRecv = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::SetMaxRecvBytes(int theMaxBytes)
{
	mMaxRecvBytes = theMaxBytes;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::Close()
{
	if(mSocket.get()==NULL)
		return;

	mSocket->Kill();
	mSocket = NULL;


	if(mAutoRecvOp.get()!=NULL)
	{
		mAutoRecvOp->Kill();
		mAutoRecvOp = NULL;
	}

	OpList::iterator aListItr = mOpList.begin();
	for(; aListItr!=mOpList.end(); ++aListItr)
		(*aListItr)->Kill();

	OpMap::iterator aMapItr = mOpMap.begin();
	for(; aMapItr!=mOpMap.end(); ++aMapItr)
		aMapItr->second->Kill();

	mOpList.clear();
	mOpMap.clear();

	mCleanListItr = mOpList.end();
	mCleanMapItr = mOpMap.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus UDPManager::Bind(int thePort, bool allowBroadcast)
{
	Close();

	mSocket = new QueueSocket(AsyncSocket::UDP);
	if(allowBroadcast)
		mSocket->SetAllowBroadcast(true);

	WONStatus aStatus = mSocket->Bind(thePort);
	if(aStatus==WS_Success)
	{
		mSocket->SetRepeatCompletion(new OpRefCompletion(StaticRecvCompletion,this));
		mSocket->SetRepeatOp(new RecvBytesFromOp(mMaxRecvBytes));
	}

	return aStatus;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short UDPManager::GetLocalPort()
{
	if(mSocket.get()==NULL)
		return 0;
	else
		return mSocket->GetLocalPort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::SetAutoRecv(bool autoRecv)
{
	if(autoRecv)
	{
		if(mAutoRecvOp.get()==NULL)
			mAutoRecvOp = new UDPManagerRecvOp;
	}
	else
		mAutoRecvOp = NULL;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::QueueOp(UDPManagerRecvOp *theOp, DWORD theTimeout)
{
	if(theOp->GetCompletion()==NULL)
		theOp->SetCompletion(mDefaultCompletion);

	if(mSocket.get()==NULL)
	{
		// Just kill the op
		if(!theOp->Pending())
			theOp->RunAsync(theTimeout);

		theOp->Kill();
		return;
	}


	CleanOps();
	if(theOp->mFromAddr.IsValid())
		mOpMap.insert(OpMap::value_type(theOp->mFromAddr,theOp));
	else
		mOpList.push_back(theOp);

	if(!theOp->Pending())
		theOp->RunAsync(theTimeout);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::SendBytesTo(const ByteBuffer *theBytes, const IPAddr &theAddr)
{
	mSocket->QueueOp(new SendBytesToOp(theBytes,theAddr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::CleanOps()
{
	int i;

	// Clean list
	for(i=0; i<10; i++)
	{
		if(mCleanListItr==mOpList.end())
		{
			mCleanListItr = mOpList.begin();
			break;
		}

		UDPManagerRecvOp *anOp = *mCleanListItr;
		if(!anOp->Pending())
			mOpList.erase(mCleanListItr++);
		else
			++mCleanListItr;
	}

	// Clean map
	for(i=0; i<10; i++)
	{
		if(mCleanMapItr==mOpMap.end())
		{
			mCleanMapItr = mOpMap.begin();
			break;
		}

		UDPManagerRecvOp *anOp = mCleanMapItr->second;
		if(!anOp->Pending())
			mOpMap.erase(mCleanMapItr++);
		else
			++mCleanMapItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::Erase(const OpList::iterator &theItr)
{
	if(theItr==mCleanListItr)
		++mCleanListItr;

	mOpList.erase(theItr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::Erase(const OpMap::iterator &theItr)
{
	if(theItr==mCleanMapItr)
		++mCleanMapItr;

	mOpMap.erase(theItr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::StaticRecvCompletion(AsyncOpPtr theOp, RefCountPtr theManager)
{
	RecvBytesFromOp *anOp = (RecvBytesFromOp*)theOp.get();
	if(!anOp->SucceededAlive())
		return;

	UDPManager *aManager = (UDPManager*)theManager.get();
	aManager->RecvCompletion(anOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void UDPManager::RecvCompletion(RecvBytesFromOp *theOp)
{
	const ByteBuffer *aBuf = theOp->GetBytes();
	ReadBuffer aMsg(aBuf->data(),aBuf->length());
	
	// Check specified address map
	if(!mOpMap.empty())
	{
		const IPAddr &anAddr = theOp->GetAddr();
		OpMap::iterator anItr = mOpMap.lower_bound(anAddr);
		while(anItr!=mOpMap.end())
		{
			if(anItr->first!=anAddr)
				break;

			UDPManagerRecvOp *anOp = anItr->second;
			if(!anOp->Pending())
			{
				Erase(anItr++);
				continue;
			}

			try
			{
				aMsg.Rewind();
				WONStatus aStatus = anOp->CheckMsg(aMsg);
				if (aStatus == WS_ServerReq_Recv)
				{
					// Keep receiving
					UDPManagerRecvOp* aRecvOp = new UDPManagerRecvOp(anOp->GetFromAddr());
					QueueOp(aRecvOp,anOp->TimeLeft());
				}
				else if(aStatus!=WS_None)
				{
					anOp->SetMsg(aBuf);
					anOp->Finish(aStatus);
					Erase(anItr);
					return;
				}
			}
			catch(ReadBufferException&)
			{
			}

			++anItr;
		}	
	}

	// Check unspecified address list
	if(!mOpList.empty())
	{
		OpList::iterator anItr = mOpList.begin(); 
		while(anItr!=mOpList.end())
		{
			UDPManagerRecvOp *anOp = *anItr;
			if(!anOp->Pending())
			{
				Erase(anItr++);
				continue;
			}

			try
			{
				aMsg.Rewind();
				WONStatus aStatus = anOp->CheckMsg(aMsg);
				if(aStatus!=WS_None)
				{
					anOp->SetFromAddr(theOp->GetAddr());
					anOp->SetMsg(aBuf);
					anOp->Finish(aStatus);
					Erase(anItr);
					return;
				}
			}
			catch(ReadBufferException&)
			{
			}

			++anItr;
		}
	}

	if(mAutoRecvOp.get()!=NULL)
	{
		mAutoRecvOp->SetMsg(aBuf);
		mAutoRecvOp->SetFromAddr(theOp->GetAddr());
		mAutoRecvOp->SetCompletion(mDefaultCompletion);
		mAutoRecvOp->ForceFinish(WS_Success);
		mAutoRecvOp = new UDPManagerRecvOp;
	}

	
	int x = 0;
	x++;
}

