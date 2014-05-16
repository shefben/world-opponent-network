#include "CriticalSection_Windows.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace
{
	typedef BOOL(WINAPI *TryEnterFunc)(LPCRITICAL_SECTION lpCriticalSection);
	TryEnterFunc gTryEnterFunc = NULL;
	bool gTryEnterCheckedFunc = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool CriticalSection::TryEnter()
{
#ifdef WIN32_NOT_XBOX
	if(!gTryEnterCheckedFunc)
	{
		HMODULE aModule = GetModuleHandle("kernel32.dll");
		if(aModule!=NULL)
			gTryEnterFunc = (TryEnterFunc)GetProcAddress(aModule,"TryEnterCriticalSection");

		gTryEnterCheckedFunc = true;
	}
	if(gTryEnterFunc!=NULL)
		return gTryEnterFunc(&mCrit)?true:false;
	else
		return false;
#else
	return false;
#endif
}