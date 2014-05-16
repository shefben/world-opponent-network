#define __WON_MASTER_CPP__

#include "Thread.h"
#include "WONDebug.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned __stdcall Thread::StaticThreadFunc(void *pThread) 
{ 
	__try
	{
		Thread *aThread = (Thread*)pThread;
		aThread->mThreadId = GetCurrentThreadId();
		aThread->ThreadFunc();
		aThread->mRunning = false;
		if(aThread->mAutoDelete)
			delete aThread;
	}
	__except(WONDebug::DumpStackToFile(GetExceptionInformation()))
	{
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Thread::StartHook()  
{ 
	unsigned int aThreadAddr;
	int anInitFlag = mPriority==ThreadPriority_Normal?0:CREATE_SUSPENDED;
	
	mThreadHandle = (HANDLE)_beginthreadex(NULL,0,StaticThreadFunc,this,anInitFlag,&aThreadAddr); 
	if(anInitFlag)
	{
		SetPriorityHook();
		ResumeThread(mThreadHandle);
	}
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Thread::WaitForStopHook() 
{ 
	WaitForSingleObject(mThreadHandle,INFINITE); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Thread::InThisThreadHook() 
{ 
	return GetCurrentThreadId()==mThreadId; 
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Thread::SetPriorityHook() 
{ 
	int aPriority = THREAD_PRIORITY_NORMAL;
	switch(mPriority)
	{
	case ThreadPriority_Highest: aPriority= THREAD_PRIORITY_HIGHEST; break;
	case ThreadPriority_High: aPriority= THREAD_PRIORITY_ABOVE_NORMAL; break;
	case ThreadPriority_Normal: aPriority= THREAD_PRIORITY_NORMAL; break;
	case ThreadPriority_Low: aPriority= THREAD_PRIORITY_BELOW_NORMAL; break;
	case ThreadPriority_Lowest: aPriority = THREAD_PRIORITY_LOWEST; break;
	}
	SetThreadPriority(mThreadHandle, aPriority);
}
