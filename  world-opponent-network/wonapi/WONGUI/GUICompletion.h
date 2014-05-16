#ifndef __WON_GUICOMPLETION_H__
#define __WON_GUICOMPLETION_H__

#include "WindowManager.h"
#include "WONCommon/Thread.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class ResultType>
class GUICompletion : public CompletionBase<ResultType>
{
private:
	virtual void Complete(ResultType theResult);
	SmartPtr<CompletionBase<ResultType> > mCompletion;
	
public:
	GUICompletion(CompletionBase<ResultType>* theCompletion) : mCompletion(theCompletion) { }
	void DoGUIComplete(AsyncOp *theOp);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class ResultType>
class GUICompletionEvent : public WindowEventBase
{
protected:
	SmartPtr<CompletionBase<ResultType> > mCompletion;
	ResultType mResult;

public:
	GUICompletionEvent(AsyncOp *theOp, CompletionBase<ResultType> *theCompletion) :
	  mCompletion(theCompletion), mResult(theOp) { }

	virtual void Deliver()
	{
		mCompletion->Complete(mResult);
	}		
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class ResultType>
inline void GUICompletion<ResultType>::Complete(ResultType theResult)
{
	if(Thread::GetCurThreadId()==WindowManager::GetThreadId())
		mCompletion->Complete(theResult);
	else
	{
		WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
		if(aMgr!=NULL)
			aMgr->PostEvent(new GUICompletionEvent<ResultType>(theResult,mCompletion));
	}	
}

typedef GUICompletion<AsyncOpPtr> OpGUICompletion;

} // namespace WONAPI

#endif