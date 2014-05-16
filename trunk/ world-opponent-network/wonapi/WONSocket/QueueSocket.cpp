#include "QueueSocket.h"
#include "SocketOp.h"
#include "WONCommon/TimerThread.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
QueueSocket::QueueSocket(SocketType theType) : AsyncSocket(theType) 
{
	mQueuedClose = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::SetQueueCompletion(OpCompletionBase *theCompletion)
{
	AutoCrit aCrit(mDataCrit);
	mDefaultQueueCompletion = theCompletion;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::SetRepeatCompletion(OpCompletionBase *theCompletion)
{
	AutoCrit aCrit(mDataCrit);
	mRepeatCompletion = theCompletion;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::SetCloseCompletion(OpCompletionBase *theCompletion)
{
	AutoCrit aCrit(mDataCrit);
	mCloseCompletion = theCompletion;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::StartCloseTimer(DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	CancelCloseTimer();

	if(theTimeout!=0)
	{
		mCloseTimer = new AsyncOp;
		mCloseTimer->SetCompletion(new OpRefCompletion(StaticCloseTimerCallback,this));
		mCloseTimer->RunAsync(theTimeout);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::CancelCloseTimer()
{
	if(mCloseTimer.get()!=NULL)
	{
		AsyncOpPtr aTimer = mCloseTimer;
		mCloseTimer = NULL;
		aTimer->Kill();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::StaticCloseTimerCallback(AsyncOpPtr theOp, RefCountPtr theParam)
{
	QueueSocket *aSocket = (QueueSocket*)theParam.get();
	aSocket->CloseTimerCallback(theOp.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::CloseTimerCallback(AsyncOp *theTimer)
{
	AutoCrit aCrit(mDataCrit);

	if(theTimer==mCloseTimer.get() && theTimer->TimedOut())
	{
		mCloseTimer = NULL;
		OpCompletionBasePtr aCloseCompletion = mCloseCompletion;

		aCrit.Leave();

		Close();
		if(aCloseCompletion.get()!=NULL)
		{
			CloseOpPtr aCloseOp = new CloseOp(this);
			aCloseOp->SetCompletion(aCloseCompletion);
			aCloseOp->ForceFinish(WS_Killed);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace
{

class QueueOpFastCompletion : public OpRefCompletion
{
protected:
	typedef void(*Callback)(AsyncOpPtr theResult, RefCountPtr theParam);

public:
	OpCompletionBasePtr mOriginalCompletion;

	QueueOpFastCompletion(Callback theCallback, RefCount *theParam, OpCompletionBase *theOriginal) : 
		OpRefCompletion(theCallback,theParam), mOriginalCompletion(theOriginal) { }
};

class QueueCloseOp : public SocketOp
{
public:
	QueueCloseOp() : SocketOp(NULL) { }
	WONStatus Continue() { return WS_Success; }
};
typedef SmartPtr<QueueCloseOp> QueueCloseOpPtr;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::StaticCallback(AsyncOpPtr theOp, RefCountPtr theParam)
{
	QueueSocket *aSocket = (QueueSocket*)theParam.get();
	aSocket->Callback((SocketOp*)theOp.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::Callback(SocketOp* theOp)
{
	AutoCrit aCrit(mDataCrit);

	bool close = (!theOp->Succeeded() && GetType()!=UDP) || !IsValid();

	OpCompletionBasePtr aCompletion;

	if(theOp==mQueueOp.get())
	{
		aCompletion = mCurQueueCompletion;
		mCurQueueCompletion = NULL;
		mQueueOp = NULL;

		if(!close)
		{
			if(mQueuedClose && mOpQueue.empty())
				close = true;
			else
				RunQueueOp();
		}
	}
	else if(theOp==mRepeatOp.get())
	{
		aCompletion = mRepeatCompletion;

		if(theOp->Killed()) // prevent repeating the op forever (for instance when the API is shutdown)
			close = true;

		if(!close)
		{
			mRepeatOp = mRepeatOp->Duplicate();
			RunRepeatOp();
		}
		else
		{
			if(mQueuedClose && theOp->Killed())
			{
				close = false;  // still waiting for CloseOp to get to the front of the Queue
				aCompletion = NULL;  // We killed the repeat op on purpose in QueueClose 
			}

			mRepeatOp = NULL;
		}
	}
	else 
	{
		QueueOpFastCompletion *aFastCompletion = dynamic_cast<QueueOpFastCompletion*>(theOp->GetCompletion());
		if(aFastCompletion!=NULL)
			aCompletion = aFastCompletion->mOriginalCompletion;
	}

	OpCompletionBasePtr aCloseCompletion;
	if(close)
		aCloseCompletion = mCloseCompletion;

	aCrit.Leave();

	if(aCompletion.get()!=NULL)
		aCompletion->Complete(theOp);

	if(close)
	{
		Close();
		if(aCloseCompletion.get()!=NULL)
		{
			CloseOpPtr aCloseOp = new CloseOp(this);
			aCloseOp->SetCompletion(aCloseCompletion);
			aCloseOp->ForceFinish(theOp->GetStatus());
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::SetRepeatOp(SocketOp *theOp)
{
	AutoCrit aCrit(mDataCrit);
	mRepeatOp = theOp;
	RunRepeatOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::RunRepeatOp()
{
	AutoCrit aCrit(mDataCrit);
	if(mRepeatOp.get()==NULL || mRepeatOp->Pending())
		return;
		
	mRepeatOp->SetCompletion(new OpRefCompletion(StaticCallback,this));
	mRepeatOp->SetSocket(this);
	mRepeatOp->RunAsync(OP_TIMEOUT_INFINITE);	
}

	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::QueueOp(SocketOp *theOp, DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	theOp->SetSocket(this);
	theOp->SetTimeout(theTimeout);
	if(theOp->GetCompletion()==NULL)
		theOp->SetCompletion(mDefaultQueueCompletion);

	mOpQueue.push_back(theOp);
	RunQueueOp();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::QueueOpFast(SocketOp *theOp, DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);
	if(mQueueOp.get()!=NULL)
	{
		QueueOp(theOp,theTimeout);
		return;
	}

	theOp->SetSocket(this);
	OpCompletionBasePtr anOriginalCompletion  = theOp->GetCompletion();
	if(anOriginalCompletion.get()==NULL)
		anOriginalCompletion =  mDefaultQueueCompletion;

	theOp->SetCompletion(new QueueOpFastCompletion(StaticCallback,this,anOriginalCompletion));
	theOp->RunAsync(theTimeout);
	if(theOp->Pending())
	{
		mQueueOp = theOp;
		mCurQueueCompletion = anOriginalCompletion;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::RunQueueOp()
{
	AutoCrit aCrit(mDataCrit);
	if(mQueueOp.get()!=NULL)
		return;

	if(mOpQueue.empty())
		return;

	mQueueOp = mOpQueue.front();
	mOpQueue.pop_front();

	mCurQueueCompletion = mQueueOp->GetCompletion();
	mQueueOp->SetCompletion(new OpRefCompletion(StaticCallback,this));
	mQueueOp->SetSocket(this);

	mQueueOp->RunAsync(mQueueOp->GetTimeout());	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::QueueClose(DWORD theTimeout)
{
	AutoCrit aCrit(mDataCrit);

	mQueuedClose = true;
	if(mRepeatOp.get()!=NULL)
	{
		mRepeatOp->Kill();
//		mRepeatOp = NULL;
	}

	QueueCloseOpPtr aCloseOp = new QueueCloseOp;
	aCloseOp->SetRunAsyncImmediately(false); // make sure it calls close from the API Pump Thread
	QueueOp(aCloseOp, OP_TIMEOUT_INFINITE);
	if(theTimeout!=OP_TIMEOUT_INFINITE)
		StartCloseTimer(theTimeout);

	return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::OpenHook()
{
	mQueuedClose = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::CloseHook()
{
	mQueuedClose = false;
	if(mRepeatOp.get()!=NULL)
	{
		mRepeatOp->SetCompletion(mRepeatCompletion);
		mRepeatOp->Kill();
		mRepeatOp = NULL;
	}

	if(mQueueOp.get()!=NULL)
	{
		mQueueOp->SetCompletion(mCurQueueCompletion);
		mQueueOp->Kill();
		mQueueOp = NULL;
		mCurQueueCompletion = NULL;
	}

	OpQueue::iterator anItr = mOpQueue.begin();
	while(anItr!=mOpQueue.end())
	{
		(*anItr)->Kill();
		++anItr;
	}

	if(mCloseTimer.get()!=NULL)
	{
		mCloseTimer->Kill();
		mCloseTimer = NULL;
	}

	mOpQueue.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void QueueSocket::KillHook()
{
	mDefaultQueueCompletion = NULL;
	mCurQueueCompletion = NULL;
	mRepeatCompletion = NULL;
	mCloseCompletion = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



