#include "HTTPEngine.h"
#include <time.h>

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPEngine::HTTPEngine()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPSession::HTTPSession()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTTPSession::GetNumOps()
{
	AutoCrit aCrit(mDataCrit);
	int aNumOps = 0;
	ConnectionMap::iterator anItr = mConnectionMap.begin();
	while(anItr!=mConnectionMap.end())
	{
		HTTPConnection *aConnection = anItr->second;
		aNumOps += aConnection->GetNumOps();
		++anItr;
	}

	return aNumOps;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPSession::AddOp(HTTPGetOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	ConnectionMap::iterator anItr = mConnectionMap.insert(ConnectionMap::value_type(theOp->GetHost(),NULL)).first;
	if(anItr->second.get()==NULL)
		anItr->second = new HTTPConnection;

	HTTPConnectionPtr aConnection = anItr->second;
	aConnection->QueueOp(theOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPSession::Kill()
{
	AutoCrit aCrit(mDataCrit);
	ConnectionMap::iterator anItr = mConnectionMap.begin();
	while(anItr!=mConnectionMap.end())
	{
		HTTPConnection *aConnection = anItr->second;
		aConnection->Kill();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTTPConnection::HTTPConnection(HTTPSession *theSession) 
{
	mCurOp[0] = mCurOp[1] = NULL;
	mDoPipeline = true;
//	mDoPipeline = false;
	mSession = theSession;
	mRetryMode = false;
	
	mNumPipelineFailures = 0;
	mNumPipelineSuccesses = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTTPConnection::GetNumOps()
{
	int aNumOps = 0;
	for(int i=0; i<2; i++)
	{
		aNumOps += mOpQueue[i].size();
		if(mCurOp[i]!=NULL)
			aNumOps++;
	}

	return aNumOps;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPConnection::StaticOpCallback(AsyncOpPtr theOp, RefCountPtr that)
{
	HTTPConnection *aConnection = (HTTPConnection*)that.get();
	aConnection->OpCallback((HTTPGetOp*)theOp.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPConnection::OpCallback(HTTPGetOp *theOp)
{
	int i;
	for(i=0; i<2; i++)
	{
		if(theOp==mCurOp[i])
			break;
	}

	if(i>=2)
		return;

	mCurOp[i] = NULL;

	OpCompletionBasePtr aCompletion = mCurOpCompletion[i];
	mCurOpCompletion[i] = NULL;
	theOp->SetCompletion(aCompletion);

	if(!mRetryMode)
	{
		mSocket = theOp->GetSocket();
		if(i==1 && mSocket!=NULL)
		{
			mNumPipelineSuccesses++;
			if(mNumPipelineSuccesses>2)
				mNumPipelineFailures = 0;
		}

		if(mSocket==NULL && mDoPipeline && (!mOpQueue[1].empty() || mCurOp[1]!=NULL || mCurOp[0]!=NULL))
		{
			AutoCrit aCrit(mDataCrit);
			mRetryMode = true;		
			if(mCurOp[0]!=NULL)
				mCurOp[0]->Kill();
			if(mCurOp[1]!=NULL)
				mCurOp[1]->Kill();
		}
	}

	if(mRetryMode)
	{
		if(i==0 || !theOp->Succeeded())
			QueueOp(theOp);
		else
		{
			if(aCompletion.get()!=NULL)
				aCompletion->Complete(theOp);
		}

		if(mCurOp[0]==NULL && mCurOp[1]==NULL)
		{
			// need to insert the recvops back in the send queue
			AutoCrit aCrit(mDataCrit);

			int anEnd=1;
			if(mDoPipeline && ++mNumPipelineFailures>2)
			{
				mDoPipeline = false; // pipelining isn't working on this connection
				anEnd = 0; // throw in both the send and the recv ops
			}

			for(int j=1; j>=anEnd; j--)
			{
				OpQueue aQueue = mOpQueue[j];
				mOpQueue[j].clear();
				OpQueue::iterator anItr = aQueue.begin();
				while(anItr!=aQueue.end())
				{
					HTTPGetOp *anOp = (HTTPGetOp*)*anItr;
					QueueOp(anOp);
					++anItr;
				}
			}
			mRetryMode = false;
			if(mDoPipeline)
				RunOp(0);
			else
				RunOp(1);
		}
		return;
	}


	if(i==0 && mSocket!=NULL && theOp->GetStatus()==WS_HTTP_SendSuccess) // just finished sending
	{
		theOp->SetOnlyRecv();
		QueueOp(theOp,1);
	}
	else
	{
//		if(theOp->GetRedirectHost().empty())
//			mSocket = theOp->GetSocket();

		if(aCompletion.get()!=NULL)
			aCompletion->Complete(theOp);

		mLastUseTime = time(NULL);
	}

	RunOp(i);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPConnection::QueueOp(HTTPGetOp *theOp, int theId)
{
	AutoCrit aCrit(mDataCrit);
	mOpQueue[theId].push_back(theOp);
	if(mOpQueue[theId].size()==1)
		RunOp(theId);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPConnection::QueueOp(HTTPGetOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	if(mDoPipeline)
	{
		theOp->SetOnlySend();
		QueueOp(theOp,0);
	}
	else
	{
		theOp->SetSendAndRecv();
		QueueOp(theOp,1);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPConnection::RunOp(int theId)
{
	AutoCrit aCrit(mDataCrit);
	if(mRetryMode)
		return;

	if(mOpQueue[theId].empty())
		return;

	if(mCurOp[theId]!=NULL)
		return;

	HTTPGetOpPtr anOp = mOpQueue[theId].front();
	mOpQueue[theId].pop_front();

	mCurOp[theId] = anOp;
	mCurOpCompletion[theId] = mCurOp[theId]->GetCompletion();
	mCurOp[theId]->SetCompletion(new OpRefCompletion(StaticOpCallback,this));
	mCurOp[theId]->SetCloseConnection(false);
	mCurOp[theId]->SetDoRangeResume(false);
	mCurOp[theId]->SetSocket(mSocket);
	mCurOp[theId]->RunAsync(mCurOp[theId]->GetTimeout());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTTPConnection::Kill()
{
	AutoCrit aCrit(mDataCrit);

	mRetryMode = false;
	mNumPipelineFailures = 0;
	mNumPipelineSuccesses = 0;
	mDoPipeline = true;

	for(int i=0; i<2; i++)
	{
		if(mCurOp[i]!=NULL)
		{
			mCurOp[i]->Kill();
			mCurOp[i] = NULL;
			mCurOpCompletion[i] = NULL;
		}

		OpQueue::iterator anItr = mOpQueue[i].begin();
		while(anItr!=mOpQueue[i].end())
		{
			HTTPGetOp *anOp = *anItr;
			bool needForceKill = !anOp->Pending();
			anOp->Kill();
			if(needForceKill)
				anOp->ForceFinish(WS_Killed);
			
			++anItr;
		}
		mOpQueue[i].clear();
	}

	if(mSocket.get()!=NULL)
	{
		mSocket->Kill();
		mSocket = NULL;
	}
}
