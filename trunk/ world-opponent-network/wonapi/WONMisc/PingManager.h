#ifndef __PingManager_H__
#define __PingManager_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONCommon/Completion.h"
#include "WONSocket/UDPManager.h"
#include "WONCommon/SmartPtr.h"
#include "WONCommon/AsyncOpTracker.h"

#include <list>
#include <map>

namespace WONAPI 
{

class IPAddr;
class ReadBuffer;
class WriteBuffer;

WON_PTR_FORWARD(PingManager);

///////////////////////////////////////////////////////////////////////////////
// PingManager
//
// Class to ping multiple servers and return the results of each ping when finished.
//
// Note:  This does not use an ICMP ping in any way.  It uses a UDP message that must
// be responded to with a UDP message of the proper format.  Servers pinged must support
// this ping either through use of this class or sending a message back themselves.
//
////////////////////////////////////////////////////////////////////////////////

//typedef ParamCompletion<AsyncOpPtr, void*> PingFinishedCompletion;
//typedef OpCompletionBase PingFinishedCompletion
//typedef SmartPtr<PingFinishedCompletion> PingFinishedCompletionPtr;

typedef ParamCompletion<AsyncOpPtr, OpCompletionBasePtr> PingFinishedCompletion;

class PingGameOp : public UDPManagerSendRecvOp
{
public:
	enum PingStatus
	{
		PingStatus_Failed = 0,
		PingStatus_Success = 1
	};

public:
	PingGameOp(PingManager* thePingManager, IPAddr& theAddr, ByteBufferPtr& theExtraData);
	virtual ~PingGameOp();

	// Public Inlines
public:
	PingManager* GetManager() { return mPingManager;}

protected:
	virtual void RunHook();
	virtual void RetransmitHook();
	virtual WONStatus UnpackMsg(ReadBuffer &theMsg);
	
public:

	// Send Info
	ByteBufferPtr	mExtraData;
	unsigned long	mSeqNum;

	// Status Info
	PingStatus		mPingStatus;
	unsigned long	mPingTime;

private:
	PingManagerPtr	mPingManager;
};
typedef SmartPtr<PingGameOp> PingGameOpPtr;

typedef std::list<PingGameOpPtr> PingTargetList;
typedef std::map<unsigned short, PingGameOpPtr> SeqPingTargetMap;

// Signifies that this is a WON message sent in game channels
const unsigned char WON_MessageIdent = 0xEE;  

class PingManager : public RefCount
{
public:
	
	// Determines what type of WON message
	enum WONMsgType
	{
		WONMsgType_PingRequest = 0,
		WONMsgType_PingResponse = 1
	};
	
public:
	PingManager();

	// Don't allow construction on the stack
protected:
	virtual ~PingManager();
	
public:
	static void PingManagerCompletionStatic(AsyncOpPtr theOp, OpCompletionBasePtr theOpCompletion);

public:
	// Functions used to do pings and batch pings
	bool StartPings();														// Start pinging - No options can be changed until all pings are finished
	void KillPings();														// Stop all pings
	bool AddPingTarget(IPAddr& theTargetAddr, ByteBufferPtr	&theExtraData); // Add ping destination to list
	bool QueuePingOp(PingGameOp* thePingGameOp);							// Queue a ping op to the list.  This is the more versatile version of AddPingTarget
	
	// Public utility
	unsigned short GetNextSequenceNum();
	void PingManagerCompletion(PingGameOpPtr theOp, OpCompletionBasePtr theOpCompletion);

	// Functions used to respond to pings
	bool static IsPingMsg(ReadBuffer& theMsg);
	bool static GetPingExtendedData(ReadBuffer& theMsg, ByteBufferPtr& theExtendedData);
	bool static CreatePingReponse(ReadBuffer& theMsg, ByteBufferPtr& theExtraSendData, WriteBuffer& theResponse);

	// Accessors	
public:
	bool SetNumRetrys(unsigned short theNumRetrys);
	bool SetMaxSimPings(unsigned long theMaxSimPings);
	bool SetTimeout(DWORD thePingTimeout);

	bool SetPingFinishedCompletion(OpCompletionBase* thePingFinishedCompletion); // Set callback to be called when ping finishes

	// Inlines
public:
	int GetNumPingsLeft() {AutoCrit aCrit(mCriticalSection); return mOpTracker.GetNumTrack() + mPingTargetList.size();}
	OpCompletionBase* GetDefaultCompletion() {AutoCrit aCrit(mCriticalSection); return mDefaultPingCompletion.get();}

	// Private methods
private:
	void RunOp(PingGameOp* theOp);
	void PingManager::RunPings();

	// Inlines
	void Track(AsyncOp* theOp) { AutoCrit aCrit(mCriticalSection); mOpTracker.Track(theOp);}
	void Untrack(AsyncOp* theOp) { AutoCrit aCrit(mCriticalSection); mOpTracker.Untrack(theOp);}
	
private:

	// Options
	unsigned short				mNumRetrys;		// Number of times to try pinging until we say ping failed
	unsigned long				mMaxSimPings;	// How many pings to perform in parallel
	DWORD						mPingTimeout;	// Time to wait for a ping before trying again

	OpCompletionBasePtr			mDefaultPingCompletion;	// Callback to call when finished or failed

	bool						mPingInProgress;  // Are we in the process of pinging
	unsigned short				mNextSequenceNum;

	PingTargetList				mPingTargetList;
	AsyncOpTracker				mOpTracker;

	UDPManagerPtr				mUDPManager;

	CriticalSection				mCriticalSection;
};

typedef SmartPtr<PingManager> PingManagerPtr;

} // namespace WONAPI

#endif // #ifndef __PingManager_H__
