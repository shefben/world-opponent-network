#ifndef __WON_HTTPENGINE_H__
#define __WON_HTTPENGINE_H__

#include "WONCommon/SmartPtr.h"
#include "WONCommon/StringUtil.h"
#include "HTTPGetOp.h"

namespace WONAPI
{

class HTTPEngine;
class HTTPSession;
class HTTPConnection;

typedef SmartPtr<HTTPEngine> HTTPEnginePtr;
typedef SmartPtr<HTTPSession> HTTPSessionPtr;
typedef SmartPtr<HTTPConnection> HTTPConnectionPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTTPEngine : public RefCount
{
public:
	HTTPEngine();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTTPSession : public RefCount
{
protected:
	CriticalSection mDataCrit;

	typedef std::map<std::string,HTTPConnectionPtr,StringLessNoCase> ConnectionMap;
	ConnectionMap mConnectionMap;


public:
	HTTPSession();

	void AddOp(HTTPGetOp *theOp);
	int GetNumOps();

	void Kill();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTTPConnection : public RefCount
{
protected:
	CriticalSection mDataCrit;
	HTTPSession *mSession;
	BlockingSocketPtr mSocket;
	int mNumPipelineFailures;
	int mNumPipelineSuccesses;
	bool mDoPipeline;


	time_t mLastUseTime;

	typedef std::list<HTTPGetOpPtr> OpQueue;
	OpQueue mOpQueue[2];

	bool mRetryMode;
	HTTPGetOp* mCurOp[2];
	OpCompletionBasePtr mCurOpCompletion[2];
	
	static void StaticOpCallback(AsyncOpPtr theOp, RefCountPtr that);
	void OpCallback(HTTPGetOp *theOp);
	void QueueOp(HTTPGetOp *theOp, int theId);

public:
	HTTPConnection(HTTPSession *theSession = NULL);

	void QueueOp(HTTPGetOp *theOp);
	void RunOp(int theId);

	void Kill();
	int GetNumOps();
};

};

#endif