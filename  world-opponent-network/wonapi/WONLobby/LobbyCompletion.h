#ifndef __WON_LOBBYCOMPLETION_H__
#define __WON_LOBBYCOMPLETION_H__

#include "WONGUI/WindowManager.h"
#include "WONCommon/AsyncOp.h"
#include "WONCommon/Thread.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyCompletionBase : public OpCompletionBase
{
private:
	virtual void Complete(AsyncOpPtr theResult);
	
public:
	virtual void DoComplete(AsyncOp *theOp) { }
};
typedef SmartPtr<LobbyCompletionBase> LobbyCompletionBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyCompletionEvent : public WindowEventBase
{
protected:
	LobbyCompletionBasePtr mCompletion;
	AsyncOpPtr mOp;

public:
	LobbyCompletionEvent(AsyncOp *theOp, LobbyCompletionBase *theCompletion) :
	  mCompletion(theCompletion), mOp(theOp) { }

	virtual void Deliver()
	{
		mCompletion->DoComplete(mOp);
	}		
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class ClassType>
class LobbyCompletion : public LobbyCompletionBase
{
private:
	typedef void(ClassType::*Callback)(AsyncOp*);
	Callback mCallback;
	SmartPtr<ClassType> mClass;
	
public:
	LobbyCompletion(Callback theCallback, ClassType* theClass) : mCallback(theCallback), mClass(theClass) { }

	virtual void DoComplete(AsyncOp *theOp) { ((mClass.get())->*mCallback)(theOp); }
};


} // namespace WONAPI

#endif