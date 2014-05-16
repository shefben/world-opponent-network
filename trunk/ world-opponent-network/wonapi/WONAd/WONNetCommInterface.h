#ifndef __WON_WONNETCOMMINTERFACE_H__
#define __WON_WONNETCOMMINTERFACE_H__
#include "WONShared.h"

#include "WONSocket/BlockingSocket.h"
#include "WONCommon/AsyncOpTracker.h"
#include "WONAd/AdApi/NetCommInterface.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AdReqOp : public AsyncOpWithTracker
{
private:
	QueueSocketPtr mSocket;
	IPAddr mAddr;
	ByteBufferPtr mRequest;
	unsigned char **mResponseBuf;
	unsigned long *mResponseLen;
	unsigned char *mActualResponseBuf;

	bool CallbackHook(AsyncOp *theOp, int theParam);

protected:
	virtual void RunHook();
	virtual void CleanupHook();
	virtual ~AdReqOp() { delete mActualResponseBuf; }

public:
	AdReqOp(const IPAddr &theAddr, ByteBufferPtr theReq, unsigned char **theResponseBuf, unsigned long *theResponseLen) : 
	  mAddr(theAddr), mRequest(theReq), mResponseBuf(theResponseBuf), mResponseLen(theResponseLen), mActualResponseBuf(NULL) {}
};

typedef SmartPtr<AdReqOp> AdReqOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// This class provides the network communications for the Ad API
class WONNetCommInterface : public NetCommInterface
{	
	class AdCompletion : public OpCompletionBase
	{
	private:
		void *mData;
		NetCommCallback mCallback;

	public:
		virtual void Complete(AsyncOpPtr theOp) { mCallback(theOp->Succeeded(),mData); }
		AdCompletion(NetCommCallback theCallback, void *theData) : mCallback(theCallback), mData(theData) {}
	};
	
// Public Methods
public:
	WONNetCommInterface();
	virtual ~WONNetCommInterface();

	virtual bool SendHTTPRequest( const char* theHostName,  unsigned short thePort, const char* theRequest, long theTimeOut, unsigned char** theHTTPRequestResultP, unsigned long* theHTTPRequestResultSizeP, NetCommCallback theNetCommCallbackP, void* theCallbackDataP);
	virtual bool HTTPGet(const char* theServer, unsigned short thePort, const char* theServerPath, const char* theDestPath, long theRequestTimeout, NetCommCallback theNetCommCallbackP, void* theCallbackDataP);

};

} // namespace WONAPI

#endif 
