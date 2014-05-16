#include "PingLogic.h"


#include "LobbyConfig.h"
#include "LobbyEvent.h"
#include "LobbyGame.h"
#include "LobbyMisc.h"
#include "LobbyPersistentData.h"
#include "LobbyStagingPrv.h"

#include "WONSocket/UDPManager.h"

using namespace WONAPI;

namespace {
	const unsigned int gDefaultPingCeiling = 5000;
	unsigned short gPingSequence = 0;
	enum { PING_RETRY_MAX = 3 };
};

typedef LogicCompletion<PingLogic> PingLogicCompletion;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum PingMsgType
{
	PingMsgType_PingRequest		=	7,
	PingMsgType_PingReply		=	8
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum PingDetailFlags
{
	PingDetailFlag_GetSummary	=	0x01,
	PingDetailFlag_GetDetails	=	0x02
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyPingGameOp : public UDPManagerSendRecvOp
{
protected:
	LobbyGamePtr mGame;
	DWORD mTransmitTick;
	unsigned short mPingSequence;
	unsigned short mPingTime;
	unsigned char mDetailFlags;

	ByteBufferPtr mDetails;

	virtual void RunHook();
	virtual void RetransmitHook();
	virtual WONStatus UnpackMsg(ReadBuffer &theMsg);

public:
	LobbyPingGameOp(LobbyGame *theGame, unsigned char theDetailFlags);

	unsigned short GetPingTime() { return mPingTime; }
	LobbyGame* GetGame() { return mGame; }

	const ByteBuffer* GetDetails() { return mDetails; }
	unsigned char GetDetailFlags() { return mDetailFlags; }
};
typedef SmartPtr<LobbyPingGameOp> LobbyPingGameOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LobbyPingGameOp::LobbyPingGameOp(LobbyGame *theGame, unsigned char theDetailFlags)
{
	mGame = theGame;
	mDetailFlags = theDetailFlags;

	SetDestAddr(mGame->GetIPAddr());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPingGameOp::RetransmitHook()
{
	mPingSequence = gPingSequence++;
	mTransmitTick = GetTickCount();

	WriteBuffer aBuf;
	aBuf.AppendByte(PingMsgType_PingRequest);
	aBuf.AppendShort(mPingSequence);
	aBuf.AppendByte(mGame->GetGameType());
	aBuf.AppendByte(mDetailFlags);
	mSendMsg = aBuf.ToByteBuffer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LobbyPingGameOp::RunHook()
{

	mPingTime = LobbyPing_Failed; // mark as failed until proven otherwise

	RetransmitHook();
	UDPManagerSendRecvOp::RunHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus LobbyPingGameOp::UnpackMsg(ReadBuffer &theMsg)
{
	if(theMsg.ReadByte()!=PingMsgType_PingReply)
		return WS_None;

	if(theMsg.ReadShort()!=mPingSequence)
		return WS_None;

	mPingTime = GetTickCount() - mTransmitTick;
	mDetailFlags = 0;

	if(theMsg.Available()>0)
	{
		mDetailFlags = theMsg.ReadByte();
		int aNumBytes = theMsg.Available();
		mDetails = new ByteBuffer(theMsg.ReadBytes(aNumBytes),aNumBytes);
	}

	return WS_Success;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PingLogic::PingLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PingLogic::~PingLogic()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::Init(ComponentConfig *theConfig)
{
	UDPManager *aMgr = LobbyMisc::GetUDPManager();
	if(aMgr!=NULL)
		aMgr->SetDefaultCompletion(new PingLogicCompletion(RecvUDPCompletion,this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::HandleControlEvent(int theControlId)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case LobbyEvent_PingGame: 
		{
			PingGameEvent *anEvent = (PingGameEvent*)theEvent;
			QueuePing(anEvent->mGame,anEvent->mGetDetails);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::HandlePingRequest(ReadBuffer &theMsg, const IPAddr &theReplyAddr)
{
	LobbyGame *aGame = LobbyStaging::GetGame();
	if(aGame==NULL || !aGame->IAmCaptain())
		return;

	unsigned short aPingSequence = theMsg.ReadShort();
	unsigned char aGameType = theMsg.ReadByte();
	if(aGame->GetGameType()!=aGameType)
		return;

	if(aGame->InProgress()) // reply to pings while game in progress?
	{
		LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
		if(aConfig!=NULL && aConfig->mNoPingInProgress)
			return;
	}

	WriteBuffer aReply(3);
	aReply.AppendByte(PingMsgType_PingReply);
	aReply.AppendShort(aPingSequence);

	// Append game details if needed
	unsigned char aDetailFlags = theMsg.ReadByte();
	aDetailFlags &= 0x03; // these are the only flags that are currently understood
	aReply.AppendByte(aDetailFlags);
	if (aDetailFlags & PingDetailFlag_GetSummary)
		aGame->WriteSummary(aReply);
	if (aDetailFlags & PingDetailFlag_GetDetails)
		aGame->WriteDetails(aReply);

	UDPManager *aMgr = LobbyMisc::GetUDPManager();
	if(aMgr==NULL)
		return;

	aMgr->SendBytesTo(aReply.ToByteBuffer(),theReplyAddr);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::RecvUDPCompletion(AsyncOp *theOp)
{
	UDPManagerRecvOp *anOp = (UDPManagerRecvOp*)theOp;
	if(!anOp->SucceededAlive())
		return;

	ReadBuffer aMsg(anOp->GetMsg()->data(), anOp->GetMsg()->length());
	const IPAddr &aReplyAddr = anOp->GetFromAddr();
	try
	{
		int aType = aMsg.ReadByte();
		switch(aType)
		{
			case PingMsgType_PingRequest:	HandlePingRequest(aMsg,aReplyAddr);		  
		}
	}
	catch(ReadBufferException&)
	{
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::PingCompletion(AsyncOp *theOp)
{
	LobbyPingGameOp *anOp = (LobbyPingGameOp*)theOp;
	if(anOp->Killed())
		return;

	LobbyGame *aGame = anOp->GetGame();
	unsigned short aPingTime = anOp->GetPingTime();
	if(aPingTime!=LobbyPing_Failed)
		aGame->SetHadSuccessfulPing(true);

	// take care of retrying with SetNumRetransmits(PING_RETRY_MAX-1) method
	/*	else if (anOp->CanRetry()) 
	{
		anOp->IncRetryCt();
		LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
		RunOpTrack(anOp, new PingLogicCompletion(PingCompletion,this), aConfig ? aConfig->mPingCeiling : gDefaultPingCeiling);
		return;
	}*/

	aGame->SetPing(aPingTime);
	if(anOp->GetDetails()!=NULL)
	{
		ReadBuffer aMsg(anOp->GetDetails()->data(),anOp->GetDetails()->length());

		bool success = true;
		unsigned char aDetailFlags = anOp->GetDetailFlags();
		
		if(aDetailFlags & PingDetailFlag_GetSummary)
			success = success && aGame->ReadSummary(aMsg);
		if(aDetailFlags & PingDetailFlag_GetDetails)
			success = success && aGame->ReadDetails(aMsg);

		// fixme: if(!success) then what?
	}

	LobbyEvent::BroadcastEvent(new GameChangedEvent(aGame,LobbyChangeType_Modify));
	LobbyStagingPrv::NotifyPingChange(aGame);

	int aMaxPings = LobbyPersistentData::GetMaxPings();

	while(!mPingList.empty())
	{
		if(mOpTracker.GetNumTrack()<aMaxPings || aMaxPings==0)
		{
			LobbyGamePtr aGame = mPingList.front();
			mPingList.pop_front();

			if(!aGame->IsPinging()) // ping was canceled
				continue;

			bool gettingDetails = aGame->IsPingingDetails();
			if(gettingDetails)
				continue; // only non-detail pings are delayed, so this means we're already running a ping through the detail query

			aGame->SetPing(LobbyPing_None);
			QueuePing(aGame,gettingDetails,true);
		}
		else
			return;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int PingLogic::GetNumPingsToDo()
{
	return mPingList.size() + mOpTracker.GetNumTrack();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::QueuePing(LobbyGame *theGame, bool getDetails, bool force)
{
	if(theGame==NULL)
		return;

	if(theGame->InProgress()) // send pings while game in progress?
	{
		LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
		if(aConfig!=NULL && aConfig->mNoPingInProgress)
			return;
	}


	if(getDetails)
	{
		if(theGame->IsPingingDetails())
			return;
	}
	else if(theGame->IsPinging()) // already pinging
		return;
	
	if(!force && !getDetails)
	{
		// Check if we're already pinging the maximum amount
		unsigned short aMaxPings = LobbyPersistentData::GetMaxPings();
		if(mOpTracker.GetNumTrack()>=aMaxPings && aMaxPings>0)
		{
			theGame->SetPing(getDetails?LobbyPing_PingingDetails:LobbyPing_Pinging);
			LobbyStagingPrv::NotifyPingChange(theGame);
			mPingList.push_back(theGame);
			return;
		}
	}

	UDPManager *aManager = LobbyMisc::GetUDPManager();
	if(aManager==NULL)
		return;

	theGame->SetPing(getDetails?LobbyPing_PingingDetails:LobbyPing_Pinging);
	LobbyStagingPrv::NotifyPingChange(theGame);


	unsigned char aDetailFlags = 0;
	if(theGame->GetGameType()==LobbyGameType_DirectConnect)
		aDetailFlags |= PingDetailFlag_GetSummary;

	if(getDetails)
		aDetailFlags |= PingDetailFlag_GetDetails;

	LobbyPingGameOpPtr anOp = new LobbyPingGameOp(theGame,aDetailFlags);
	anOp->SetUDPManager(aManager);

	LobbyConfig *aConfig = LobbyConfig::GetLobbyConfig();
	anOp->SetMsgTimeout(aConfig ? aConfig->mPingCeiling : gDefaultPingCeiling);
	
	if(PING_RETRY_MAX>0)
		anOp->SetNumRetransmits(PING_RETRY_MAX-1);

	RunOpTrack(anOp, new PingLogicCompletion(PingCompletion,this), OP_TIMEOUT_INFINITE); // let MsgTimeout handle timeout
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PingLogic::KillPings(bool resetValues)
{
	// Reset pings in the ping list
	for(PingList::const_iterator aListItr = mPingList.begin(); aListItr!=mPingList.end(); ++aListItr)
		(*aListItr)->SetPing(LobbyPing_None);

	// Reset pings which we're currently waiting on 
	const AsyncOpTracker::OpMap &aMap = mOpTracker.GetOpMap();
	AsyncOpTracker::OpMap::const_iterator aMapItr;
	for(aMapItr = aMap.begin(); aMapItr != aMap.end(); ++aMapItr)
	{
		LobbyPingGameOp *anOp = dynamic_cast<LobbyPingGameOp*>(aMapItr->second->val.get());
		if(anOp!=NULL)
			anOp->GetGame()->SetPing(LobbyPing_None);
	}


	mPingList.clear();
	mOpTracker.KillAll();
}
