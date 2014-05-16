#include "PingManager.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
// PingGameOp
//

PingGameOp::PingGameOp(PingManager* thePingManager, IPAddr& theAddr, ByteBufferPtr& theExtraData) :
	UDPManagerSendRecvOp(),
	mPingManager(thePingManager),
	mExtraData(theExtraData)
{
	assert(mPingManager.get());

	mSeqNum = 0;
	mPingStatus = PingStatus_Failed;
	mPingTime = 0;
	//mNumRetrys = 0;

	SetDestAddr(theAddr);
}
	
PingGameOp::~PingGameOp()
{

}

void 
PingGameOp::RunHook()
{
	RetransmitHook();

	UDPManagerSendRecvOp::RunHook();
}

void 
PingGameOp::RetransmitHook()
{
	// Get a new seq number each time we retransmit
	mSeqNum = mPingManager->GetNextSequenceNum();
	mPingTime = GetTickCount();

	WriteBuffer aBuf;
	aBuf.AppendByte(WON_MessageIdent);
	aBuf.AppendByte(PingManager::WONMsgType_PingRequest);
	aBuf.AppendShort(mSeqNum);
	aBuf.AppendBuffer(mExtraData, 2);

	mSendMsg = aBuf.ToByteBuffer();
}

WONStatus
PingGameOp::UnpackMsg(ReadBuffer &theMsg)
{
	if((unsigned char)theMsg.ReadByte() != WON_MessageIdent)
		return WS_None;

	if(theMsg.ReadByte() != PingManager::WONMsgType_PingResponse)
		return WS_None;

	if(theMsg.ReadShort() != mSeqNum)
		return WS_None;

	mPingTime = GetTickCount() - mPingTime;

	if(theMsg.Available()>0)
	{
		unsigned short aNumBytes = (unsigned short)theMsg.ReadShort();
		mExtraData = new ByteBuffer(theMsg.ReadBytes(aNumBytes),aNumBytes);
	}

	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
// PingManager
//

const unsigned short Max_Short = 0xFFFF;

// Constructor/Destructor
PingManager::PingManager() 
{
	// Defaults
	mNumRetrys = 0;		
	mMaxSimPings = 1;	
	mPingTimeout = 3000;	

	//mPingFinishedCompletion;

	mPingInProgress = false;
	mNextSequenceNum = 0;

	mUDPManager = new UDPManager();

	// Set default PingFinishedCompletion
	mUDPManager->SetDefaultCompletion( new PingFinishedCompletion(PingManagerCompletionStatic, NULL));
	mUDPManager->Bind(0, false);
}
	
PingManager::~PingManager()
{
	//KillPings();
	mUDPManager->Close();
}

void
PingManager::PingManagerCompletionStatic(AsyncOpPtr theOp, OpCompletionBasePtr theOpCompletion)
{
	PingGameOp* aOp = dynamic_cast<PingGameOp*>(theOp.get());

	aOp->GetManager()->PingManagerCompletion(aOp, theOpCompletion);
}

void
PingManager::PingManagerCompletion(PingGameOpPtr theOp, OpCompletionBasePtr theOpCompletion)
{
	// Untrack the operation
	Untrack(theOp);
	
	// Run any new pings
	RunPings();

	// Set completion back to their completion
	theOp->SetCompletion(theOpCompletion);

	if (theOp->GetStatus() == WS_Success)
	{
		theOp->mPingStatus = PingGameOp::PingStatus_Success;
	}
	else
	{
		theOp->mPingStatus = PingGameOp::PingStatus_Failed;
	}
	
	// Call users compleation
	if(theOpCompletion.get()!=NULL)
		theOpCompletion->Complete(static_cast<AsyncOpPtr>(theOp));
	else if (mDefaultPingCompletion.get()!=NULL)
		mDefaultPingCompletion->Complete(static_cast<AsyncOpPtr>(theOp));
}

// Batch ping Interface
	
bool
PingManager::StartPings()
{
	AutoCrit aCrit(mCriticalSection);
	mPingInProgress = true;

	RunPings();

	return mPingInProgress;
}

void 
PingManager::KillPings()
{
	AutoCrit aCrit(mCriticalSection);
	if (mPingInProgress)
	{
		mPingTargetList.clear();
		mOpTracker.KillAll();
		mPingInProgress = false;
	}
}

bool 
PingManager::AddPingTarget(IPAddr& theTargetAddr, ByteBufferPtr& theExtraData)
{
	PingGameOpPtr anOp = new PingGameOp(this, theTargetAddr, theExtraData);
	QueuePingOp(anOp);
	return true;
}

bool 
PingManager::QueuePingOp(PingGameOp* thePingGameOp)
{
	AutoCrit aCrit(mCriticalSection); 

	// Use Ping manager completion but pass in the original completion for later use
	OpCompletionBase* aOpCompletionBase = thePingGameOp->GetCompletion();
	thePingGameOp->SetCompletion( new PingFinishedCompletion(PingManagerCompletionStatic, aOpCompletionBase));

	mPingTargetList.push_back(thePingGameOp);
	
	if (mPingInProgress)
	{
		RunPings();
	}
	
	return true;
}

// Handle ping Interface

bool 
PingManager::IsPingMsg(ReadBuffer& theMsg)
{
	theMsg.Rewind();

	if((unsigned char)theMsg.ReadByte() != WON_MessageIdent)
		return false;

	if(theMsg.ReadByte() != PingManager::WONMsgType_PingRequest)
		return false;

	return true;
}

bool 
PingManager::GetPingExtendedData(ReadBuffer& theMsg, ByteBufferPtr& theExtendedData)
{
	if (!IsPingMsg(theMsg))
		return false;

	theMsg.ReadShort(); // Sequence number

	theExtendedData = theMsg.ReadBuf(2);
	
	return true;
}

bool
PingManager::CreatePingReponse(ReadBuffer& theMsg, ByteBufferPtr& theExtraSendData, WriteBuffer& theResponse)
{
	if (!IsPingMsg(theMsg))
		return false;

	// Build ping response
	theResponse.AppendByte(WON_MessageIdent);
	theResponse.AppendByte(PingManager::WONMsgType_PingResponse);
	theResponse.AppendShort(theMsg.ReadShort());
	theResponse.AppendBuffer(theExtraSendData.get(), 2);

	return true;
}

// Accessors

bool
PingManager::SetNumRetrys(unsigned short theNumRetrys)
{
	AutoCrit aCrit(mCriticalSection); 
	if (!mPingInProgress)
	{
		mNumRetrys = theNumRetrys;
		return true;
	}

	return false;
}

bool 
PingManager::SetMaxSimPings(unsigned long theMaxSimPings)
{
	AutoCrit aCrit(mCriticalSection); 
	if (!mPingInProgress)
	{
		mMaxSimPings = theMaxSimPings;
		return true;
	}
	return false;
}

bool
PingManager::SetTimeout(DWORD thePingTimeout)
{
	AutoCrit aCrit(mCriticalSection); 
	if (!mPingInProgress)
	{
		mPingTimeout = thePingTimeout;
		return true;
	}
	return false;
}

bool
PingManager::SetPingFinishedCompletion(OpCompletionBase* thePingFinishedCompletion)
{
	AutoCrit aCrit(mCriticalSection); 
	if (!mPingInProgress)
	{
		mDefaultPingCompletion = thePingFinishedCompletion;
		//mUDPManager->SetDefaultCompletion( new PingFinishedCompletion(PingManagerCompletion, thePingFinishedCompletion));
		return true;
	}
	return false;
}

unsigned short 
PingManager::GetNextSequenceNum()
{
	AutoCrit aCrit(mCriticalSection);
	if (mNextSequenceNum < Max_Short)
	{
		return  mNextSequenceNum++;
	}
	else
	{
		mNextSequenceNum = 0;
		return Max_Short;
	}
}

// Private Methods
void
PingManager::RunOp(PingGameOp* theOp)
{
	theOp->SetMsgTimeout(mPingTimeout);
	theOp->SetNumRetransmits(mNumRetrys);
	theOp->SetUDPManager(mUDPManager);

	Track(theOp);

	theOp->RunAsync(OP_TIMEOUT_INFINITE);  // Time out after a mNumRetrys retrys
}

void
PingManager::RunPings()
{
	// Go through the list running pings until max sim pings is reached
	while(!mPingTargetList.empty())
	{
		if((mOpTracker.GetNumTrack() < mMaxSimPings) && (mPingTargetList.size() > 0))
		{
			PingGameOpPtr aOp = mPingTargetList.front();
			mPingTargetList.pop_front();

			RunOp(aOp.get());
		}
		else
		{
			break;
		}
	}
}
