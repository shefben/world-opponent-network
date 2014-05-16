#ifndef __WON_THREAD_WINDOWS_H__
#define __WON_THREAD_WINDOWS_H__
#include "WONShared.h"

#include <process.h>
#include "Event.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Thread : public ThreadBase
{
private:
	static unsigned __stdcall StaticThreadFunc(void *pThread); 

protected:
	HANDLE mThreadHandle;
	DWORD mThreadId;

	virtual void StartHook();  
	virtual void WaitForStopHook(); 
	virtual bool InThisThreadHook();
	virtual void SetPriorityHook();

public:
	static DWORD GetCurThreadId() { return ::GetCurrentThreadId(); }
};

}; // namespace WONAPI

#endif
