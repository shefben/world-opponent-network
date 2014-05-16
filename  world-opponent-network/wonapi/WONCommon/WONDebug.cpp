#define __WON_MASTER_CPP__

#include "WONDebug.h"
using namespace WONAPI;

static std::string gStackFileName;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONDebug::SetStackFileName(const std::string &theName)
{
	gStackFileName = theName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONDebug::GetStackFileName()
{
	return gStackFileName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32_NOT_XBOX
#include "WONCommon/Platform.h"
#include <imagehlp.h>
#include <vector>
#include <time.h>

static LPVOID __stdcall FunctionTableAccess(HANDLE hProcess, DWORD dwPCAddress);
static DWORD __stdcall GetModuleBase(HANDLE hProcess, DWORD dwReturnAddress);
#define _countof(array) (sizeof(array)/sizeof(array[0]))

#define MODULE_NAME_LEN 4096
#define SYMBOL_NAME_LEN 4096

typedef BOOL (WINAPI *StackWalkFunc)(
    DWORD                             MachineType,
    HANDLE                            hProcess,
    HANDLE                            hThread,
    LPSTACKFRAME                      StackFrame,
    LPVOID                            ContextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE      ReadMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE    FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE          GetModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE        TranslateAddress
	);

typedef BOOL(WINAPI *SymLoadModuleFunc)(
    IN  HANDLE          hProcess,
    IN  HANDLE          hFile,
    IN  PSTR            ImageName,
    IN  PSTR            ModuleName,
    IN  DWORD           BaseOfDll,
    IN  DWORD           SizeOfDll
    );

typedef DWORD(WINAPI *SymSetOptionsFunc)(IN DWORD SymOptions);
typedef DWORD(WINAPI *SymGetOptionsFunc)(VOID);
typedef BOOL(WINAPI *SymInitializeFunc)(IN HANDLE hProcess, IN LPSTR UserSearchPath, IN BOOL fInvadeProcess);
typedef LPVOID(WINAPI *SymFunctionTableAccessFunc)(HANDLE hProcess, DWORD AddrBase);
typedef BOOL(WINAPI *SymGetModuleInfoFunc)(IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo);
typedef BOOL(WINAPI *SymUnDNameFunc)(IN PIMAGEHLP_SYMBOL sym, OUT LPSTR UnDecName, IN  DWORD UnDecNameLength);
typedef DWORD(WINAPI *UnDecorateSymbolNameFunc)(LPCSTR DecoratedName, LPSTR UnDecoratedName, DWORD UndecoratedLength, DWORD Flags);
typedef BOOL(WINAPI *SymGetSymFromAddrFunc)(IN HANDLE hProcess, IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol);

static StackWalkFunc gStackWalkFunc;
static SymLoadModuleFunc gSymLoadModuleFunc;
static SymSetOptionsFunc gSymSetOptionsFunc;
static SymGetOptionsFunc gSymGetOptionsFunc;
static SymInitializeFunc gSymInitializeFunc;
static SymFunctionTableAccessFunc gSymFunctionTableAccessFunc;
static SymGetModuleInfoFunc gSymGetModuleInfoFunc;
static SymUnDNameFunc gSymUnDNameFunc;
static UnDecorateSymbolNameFunc gUnDecorateSymbolNameFunc;
static SymGetSymFromAddrFunc gSymGetSymFromAddrFunc;

template<class T>
static bool GetWinDebugFunc(HMODULE theModule, const char *theName, T &theFunc)
{
	theFunc = (T)GetProcAddress(theModule,theName);
	return theFunc!=NULL;
}

static bool TryLoadWinDebugFunctions()
{
	struct AutoUnloadLibrary
	{
		HMODULE mModule;

		AutoUnloadLibrary() { mModule = NULL; }
		~AutoUnloadLibrary() { if(mModule!=NULL) FreeLibrary(mModule); }

	};
	static AutoUnloadLibrary gDebugDLL;
	HMODULE aModule = LoadLibrary("DbgHelp.dll");
	gDebugDLL.mModule = aModule;
	if(aModule==NULL)
		return false;

	if(!GetWinDebugFunc(aModule,"StackWalk",gStackWalkFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymLoadModule",gSymLoadModuleFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymSetOptions",gSymSetOptionsFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymGetOptions",gSymGetOptionsFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymInitialize",gSymInitializeFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymFunctionTableAccess",gSymFunctionTableAccessFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymGetModuleInfo",gSymGetModuleInfoFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymUnDName",gSymUnDNameFunc)) return false;
	if(!GetWinDebugFunc(aModule,"UnDecorateSymbolName",gUnDecorateSymbolNameFunc)) return false;
	if(!GetWinDebugFunc(aModule,"SymGetSymFromAddr",gSymGetSymFromAddrFunc)) return false;

	return true;
}


static bool LoadWinDebugFunctions()
{
	static bool gTriedLoadFunctions = false;
	static bool gLoadFunctionsResult = false;

	if(!gTriedLoadFunctions)
		gLoadFunctionsResult = TryLoadWinDebugFunctions();

	return gLoadFunctionsResult;
}


struct MY_SYMBOL_INFO
{
	DWORD dwAddress;
	DWORD dwOffset;
	CHAR    szModule[MODULE_NAME_LEN];
	CHAR    szSymbol[SYMBOL_NAME_LEN];
};

static LPVOID __stdcall FunctionTableAccess(HANDLE hProcess, DWORD dwPCAddress)
{
	return gSymFunctionTableAccessFunc(hProcess, dwPCAddress);
}

static DWORD __stdcall GetModuleBase(HANDLE hProcess, DWORD dwReturnAddress)
{
	IMAGEHLP_MODULE moduleInfo;

	if (gSymGetModuleInfoFunc(hProcess, dwReturnAddress, &moduleInfo))
		return moduleInfo.BaseOfImage;
	else
	{
		MEMORY_BASIC_INFORMATION memoryBasicInfo;

		if (::VirtualQueryEx(hProcess, (LPVOID) dwReturnAddress,
			&memoryBasicInfo, sizeof(memoryBasicInfo)))
		{
			DWORD cch = 0;
			char szFile[MAX_PATH] = { 0 };

		 cch = GetModuleFileNameA((HINSTANCE)memoryBasicInfo.AllocationBase,
										 szFile, MAX_PATH);

		 // Ignore the return code since we can't do anything with it.
		 if (!gSymLoadModuleFunc(hProcess,
			   NULL, ((cch) ? szFile : NULL),
			   NULL, (DWORD) memoryBasicInfo.AllocationBase, 0))
			{
				DWORD dwError = GetLastError();
//				TRACE1("Error: %d\n", dwError);
			}
		 return (DWORD) memoryBasicInfo.AllocationBase;
	  }
		else
		{}//TRACE1("Error is %d\n", GetLastError());
	}

	return 0;
}

static BOOL ResolveSymbol(HANDLE hProcess, DWORD dwAddress,
	MY_SYMBOL_INFO &siSymbol)
{
	BOOL fRetval = TRUE;

	siSymbol.dwAddress = dwAddress;

	union {
		CHAR rgchSymbol[sizeof(IMAGEHLP_SYMBOL) + 4095];
		IMAGEHLP_SYMBOL  sym;
	};

	CHAR szUndec[4096];
	CHAR szWithOffset[4096];
	LPSTR pszSymbol = NULL;
	IMAGEHLP_MODULE mi;

	memset(&siSymbol, 0, sizeof(MY_SYMBOL_INFO));
	mi.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

	if (!gSymGetModuleInfoFunc(hProcess, dwAddress, &mi))
		lstrcpyA(siSymbol.szModule, "<no module>");
	else
	{
		LPSTR pszModule = strrchr(mi.ImageName, '\\');
		if (pszModule == NULL)
			pszModule = mi.ImageName;
		else
			pszModule++;

		lstrcpynA(siSymbol.szModule, pszModule, _countof(siSymbol.szModule));
	   lstrcatA(siSymbol.szModule, "! ");
	}

	__try
	{
		sym.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		sym.Address = dwAddress;
		sym.MaxNameLength = 4095;

		if (gSymGetSymFromAddrFunc(hProcess, dwAddress, &(siSymbol.dwOffset), &sym))
		{
			pszSymbol = sym.Name;

			if (gUnDecorateSymbolNameFunc(sym.Name, szUndec, _countof(szUndec),
				UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_ACCESS_SPECIFIERS))
			{
				pszSymbol = szUndec;
			}
			else if (gSymUnDNameFunc(&sym, szUndec, _countof(szUndec)))
			{
				pszSymbol = szUndec;
			}

			if (siSymbol.dwOffset != 0)
			{
				wsprintfA(szWithOffset, "%s + %d bytes", pszSymbol, siSymbol.dwOffset);
				pszSymbol = szWithOffset;
			}
	  }
	  else
		  pszSymbol = "<no symbol>";
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		pszSymbol = "<EX: no symbol>";
		siSymbol.dwOffset = dwAddress - mi.BaseOfImage;
	}

	lstrcpynA(siSymbol.szSymbol, pszSymbol, _countof(siSymbol.szSymbol));
	return fRetval;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CTraceClipboardData 
{
protected:
	FILE *mFile;

public:
	CTraceClipboardData() { mFile = NULL; }
	virtual ~CTraceClipboardData() { Close(); }

	bool Open(const std::string &theFileName) 
	{ 
		Close();
		if(!theFileName.empty())
			mFile = fopen(theFileName.c_str(),"a+");

		return mFile!=NULL;
	}

	void Close() 
	{ 
		if(mFile!=NULL) 
			fclose(mFile);

		mFile = NULL;
	}

	void SendOut(const char *theStr) 
	{ 
		if(mFile!=NULL) 
			fprintf(mFile,theStr); 
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void MyDumpStack(LPEXCEPTION_POINTERS pInfo)
{
	CTraceClipboardData clipboardData;
	if(!clipboardData.Open(WONDebug::GetStackFileName()))
		return;
	
	char aBuf[1024];
	time_t ltime = time(NULL);
	struct tm *today = localtime(&ltime);
	strftime(aBuf,1000,"\n\n*** Begin WONDebug StackTrace %c ***\n",today); 
	clipboardData.SendOut(aBuf);

	if(!LoadWinDebugFunctions())
	{
		clipboardData.SendOut("Unable to load DbgHelp.dll functions.\n");
		strftime(aBuf,1000,"*** End WONDebug StackTrace %c ***\n",today); 
		clipboardData.SendOut(aBuf);
	}

	typedef std::vector<DWORD> DWORDArray;
	DWORDArray adwAddress;
	HANDLE hProcess = ::GetCurrentProcess();
	if (gSymInitializeFunc(hProcess, NULL, FALSE))
	{
		// force undecorated names to get params
		DWORD dw = gSymGetOptionsFunc();
		dw &= ~SYMOPT_UNDNAME;
		gSymSetOptionsFunc(dw);

		HANDLE hThread = ::GetCurrentThread();
		CONTEXT curThreadContext;
		if(pInfo==NULL)
		{
			curThreadContext.ContextFlags = CONTEXT_FULL;
			if (!::GetThreadContext(hThread, &curThreadContext))
				return;
		}

		CONTEXT &threadContext = pInfo?*pInfo->ContextRecord:curThreadContext;


//		if (::GetThreadContext(hThread, &threadContext))
		{
			STACKFRAME stackFrame;
			memset(&stackFrame, 0, sizeof(stackFrame));
			stackFrame.AddrPC.Mode = AddrModeFlat;

			DWORD dwMachType;

#if defined(_M_IX86)
			dwMachType                  = IMAGE_FILE_MACHINE_I386;

			// program counter, stack pointer, and frame pointer
			stackFrame.AddrPC.Offset    = threadContext.Eip;
			stackFrame.AddrStack.Offset = threadContext.Esp;
			stackFrame.AddrStack.Mode   = AddrModeFlat;
			stackFrame.AddrFrame.Offset = threadContext.Ebp;
			stackFrame.AddrFrame.Mode   = AddrModeFlat;
#elif defined(_M_MRX000)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_R4000;
			stackFrame.AddrPC. Offset    = treadContext.Fir;
#elif defined(_M_ALPHA)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_ALPHA;
			stackFrame.AddrPC.Offset    = (unsigned long) threadContext.Fir;
#elif defined(_M_PPC)
			// only program counter
			dwMachType                  = IMAGE_FILE_MACHINE_POWERPC;
			stackFrame.AddrPC.Offset    = threadContext.Iar;
#elif
#error("Unknown Target Machine");
#endif

			adwAddress.resize(0);
		
			int nFrame;
			for (nFrame = 0; nFrame < 1024; nFrame++)
			{
				if (!gStackWalkFunc(dwMachType, hProcess, hProcess,
					&stackFrame, &threadContext, NULL,
					FunctionTableAccess, GetModuleBase, NULL))
				{
					break;
				}

				if(adwAddress.size()<=nFrame)
					adwAddress.resize(nFrame+1);

				adwAddress[nFrame] = stackFrame.AddrPC.Offset;
			}
		}
	}
	else
	{
		DWORD dw = GetLastError();
		char sz[100];
		wsprintfA(sz,
			"AfxDumpStack Error: IMAGEHLP.DLL wasn't found. "
			"GetLastError() returned 0x%8.8X\n", dw);
		clipboardData.SendOut(sz);
	}

	// dump it out now
	int nAddress;
	int cAddresses = adwAddress.size();
	for (nAddress = 0; nAddress < cAddresses; nAddress++)
	{
		MY_SYMBOL_INFO info;
		DWORD dwAddress = adwAddress[nAddress];

		char sz[20];
		wsprintfA(sz, "%8.8X: ", dwAddress);
		clipboardData.SendOut(sz);

		if (ResolveSymbol(hProcess, dwAddress, info))
		{
			clipboardData.SendOut(info.szModule);
			clipboardData.SendOut(info.szSymbol);
		}
		else
			clipboardData.SendOut("symbol not found");
		clipboardData.SendOut("\n");
	}

	strftime(aBuf,1000,"*** End WONDebug StackTrace %c ***\n",today); 
	clipboardData.SendOut(aBuf);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONDebug::DumpStackToFile(void *theInfo)
{
	MyDumpStack((LPEXCEPTION_POINTERS)theInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

#else

int WONDebug::DumpStackToFile(void *theInfo)
{
	return 0;
}
#endif
