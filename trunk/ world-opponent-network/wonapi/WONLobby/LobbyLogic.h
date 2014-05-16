#ifndef __WON_LOBBYLOGIC_H__
#define __WON_LOBBYLOGIC_H__

#include "LobbyCompletion.h"
#include "WONCommon/AsyncOpTracker.h"
#include "WONGUI/WindowEvent.h"
#include <list>

namespace WONAPI
{
class ComponentConfig;
class LogicCompletionBase;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyLogic : public RefCount
{
protected:
	AsyncOpTracker mOpTracker;
	bool mKilled;

	virtual void KillHook();
	void RunOp(AsyncOp *theOp, LogicCompletionBase *theCompletion, DWORD theTime = 0); // time = 0 -> use default
	void RunOpTrack(AsyncOp *theOp, LogicCompletionBase *theCompletion, DWORD theTime = 0); // time = 0 -> use Default

	virtual ~LobbyLogic();

public:
	LobbyLogic();

	virtual void Init(ComponentConfig *theConfig);
	virtual void TimerEvent();
	virtual void HandleControlEvent(int theControlId);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	bool UntrackOp(AsyncOp *theOp);
	void Kill();
	bool IsKilled() { return mKilled; }
};
typedef SmartPtr<LobbyLogic> LobbyLogicPtr; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LobbyLogicMgr
{
protected:
	typedef std::list<LobbyLogicPtr> LogicList;
	LogicList mLogicList;

public:
	LobbyLogicMgr();
	void AddLogic(LobbyLogic *theLogic);

	void HandleComponentEvent(ComponentEvent *theEvent);
	void Init(ComponentConfig *theConfig);
	void Kill();
	
	void DispatchControlEvent(int theControlId);
	void DispatchComponentEvent(ComponentEvent *theEvent);
	void DispatchTimerEvent();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LogicCompletionBase : public LobbyCompletionBase
{
protected:
	bool mTrack;

public:
	LogicCompletionBase() : mTrack(false) { }
	void SetTrack(bool track) { mTrack = track; } 
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class ClassType>
class LogicCompletion : public LogicCompletionBase
{
private:
	typedef void(ClassType::*Callback)(AsyncOp*);
	Callback mCallback;
	SmartPtr<ClassType> mClass;
	
public:
	LogicCompletion(Callback theCallback, ClassType* theClass) : mCallback(theCallback), mClass(theClass) { }

	virtual void DoComplete(AsyncOp *theOp) 
	{ 
		if(mClass->IsKilled())
			return;

		if(mTrack && !mClass->UntrackOp(theOp))
			return;

		((mClass.get())->*mCallback)(theOp); 
	}
};


} // namespace WONAPI

#endif