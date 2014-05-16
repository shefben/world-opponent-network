//----------------------------------------------------------------------------------
// SierraUp.cpp : Defines the entry point for the application.
//----------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include "WONAPI/WONAPI.h"
#include "WONCommon/WONBigFile.h"
#include "WONGUI/GUICompat.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/ImageFilter.h"
#include "WONGUI/MSNativeImage.h"
#include "WONGUI/jpeglib/JPGDecoder.h"
#include "WONGUI/PNGlib/PNGDecoder.h"
#include "WONGUI/WONGUIConfig/ResourceConfig.h"
#include "WONGUI/WONGUIConfig/WONGUIConfig.h"
#include "Resource.h"
#include "../WONUpdateCtrl.h"
#include "../AbortDlg.h"
#include "../CustomInfo.h"
#include "../ParameterParser.h"
#include "../MessageDlg.h"
#include "../PatchUtils.h"
#include "../DebugLog.h"
#include "../SierraUpRestarterIni.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
const int IDM_ABOUTBOX = 0x0110;
LPCSTR    sRestartExe      = "SierraUpRestarter.exe";
LPCSTR    sPsapiDll        = "SierraUpPsapi.dll";
LPCSTR    sRestartIniFile  = ".\\SierraUpRestarter.ini";
//LPCSTR    sSelfPatchFolder = ".\\_SierraUpdatePatch";
LPCSTR    sRestartWaitTime = "1000";


//----------------------------------------------------------------------------------
// Forward Declarations.
//----------------------------------------------------------------------------------
class PumpThread;


//----------------------------------------------------------------------------------
// Global Variables.
//----------------------------------------------------------------------------------
PlatformWindowManagerPtr g_pWindowMgr;
PlatformWindowPtr        g_pWindow;
ContainerPtr             g_pMainScreen;
AboutCtrlPtr             g_pAboutDlg;
WNDPROC                  g_pGUIProc = NULL;
WONAPICore*              g_pCore = new WONAPICore(false);
PumpThread*              g_pPumpThread = NULL;

//----------------------------------------------------------------------------------
// PumpThread.
//----------------------------------------------------------------------------------
class PumpThread : public WONAPI::Thread
{
protected:
	virtual void ThreadFunc(void)
	{
		while (true)
		{
			if (g_pCore->WaitTillNeedPump(20))
			{
				g_pWindow->PostEvent(new UserEvent(PumpAPI));
				g_pWindow->PostEvent(new UserEvent(SignalAPI));
				mSignalEvent.WaitFor(INFINITE);
			}

			if (mStopped)
				return;
		}
	}

	static void SignalAPI(void*)
	{
		if (g_pPumpThread != NULL)
			g_pPumpThread->Signal();
	}

	static void PumpAPI(void*)
	{
		if (g_pCore != NULL)
			g_pCore->Pump(0);
	}
	
public:
	virtual ~PumpThread(void) { Stop(); }
};

//----------------------------------------------------------------------------------
// Create the specified file by extracting the specified resource into ito it.
//----------------------------------------------------------------------------------
bool ExtractResourceFile(int nResID, const std::string& sDest)
{
	HMODULE hModule = (HMODULE)g_pWindowMgr->GetHinstance();

	// Extract the embedded file to 'SierraUp.cfg'.
	HRSRC hFound = FindResource(hModule, MAKEINTRESOURCE(nResID), RT_RCDATA); //lint !e1924
	if (! hFound)
	{
		DebugLog("Error - Unable to locate embedded resource file %d -> %s ", nResID, sDest.c_str());
		return false;
	}

	DWORD nResSize = SizeofResource(hModule, hFound);
	HGLOBAL hRes = LoadResource(hModule, hFound);
	LPVOID pRes = LockResource(hRes);

	HANDLE hExe = CreateFile(sDest.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, 
							  CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hExe == INVALID_HANDLE_VALUE) //lint !e1924
	{
		DebugLog("Error - Unable to create file %s for resource extraction", sDest.c_str());
		return false;
	}

	// Write the file.
	DWORD nWrote = 0;
	if (! ::WriteFile(hExe, pRes, nResSize, &nWrote, NULL))
		nWrote = 0xFFFFFFFF;
	if (nWrote != nResSize)
	{
		DebugLog("Error - Unable to write file %s for resource extraction", sDest.c_str());
		CloseHandle(hExe);
		return false;
	}
	CloseHandle(hExe);

	// We are done with the resource (the file is written), so dump it.
	UnlockResource(hRes);
	FreeResource(hRes);

	return true;
}

//----------------------------------------------------------------------------------
// Create the Restartter appliaction by extracting it from the embedded resource.
//----------------------------------------------------------------------------------
bool ExtractRestarter(void)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	DebugLog("Preparing to extract restarter\n");

	if (! ExtractResourceFile(IDF_RESTARTER_EXE, sRestartExe))
	{
		MessageBox(g_pWindow, pResMgr->GetFinishedString(Global_NoWriteRestarter_String), pResMgr->GetFinishedString(Global_Error_String), MB_OK);
		return false;
	}

	if (! ExtractResourceFile(IDF_PSAPI_DLL, sPsapiDll))
	{
		MessageBox(g_pWindow, pResMgr->GetFinishedString(Global_NoWritePsapi_String), pResMgr->GetFinishedString(Global_Error_String), MB_OK);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------
// Start the restarter.  Or is that restart the starter?  No, that's not right.
//
// Start the application that waits for SierraUp to shutdown.  The restarter will 
// then apply the SierraUp patch, and restart Sierraup.
//----------------------------------------------------------------------------------
bool LaunchRestarter(const std::string& sPatchExe)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	DebugLog("Preparing to launch restarter (applying update file: %s)\n", sPatchExe.c_str());

	char sSelf[MAX_PATH];
	GetModuleFileName(0, sSelf, MAX_PATH);

	// Fetch the current working folder.
	TCHAR sWorkingDir[MAX_PATH];
	GetCurrentDirectory(sizeof(sWorkingDir), sWorkingDir);
	if (sWorkingDir[0] && sWorkingDir[strlen(sWorkingDir) - 1] != '\\')
		strcat(sWorkingDir, "\\");

	// Fetch the folder SierraUp is installed in.
	std::string sSelfDir = sSelf;
	int nPos = sSelfDir.rfind('\\');
	sSelfDir.erase(nPos + 1, sSelfDir.length() - (nPos + 1)); // Leave the last backslash.
	DebugLog("SierraUpdate folder: %s\n", sSelfDir.c_str());

	// What is our executable name (just in case we change it).
	std::string sOurExeName = sSelf;
	nPos = sOurExeName.rfind('\\'); // Redundant, but just in case I move this block of code.
	sOurExeName.erase(0, nPos + 1);
	std::string sLowerExeName = StringToLowerCase(sOurExeName);
	DebugLog("SierraUpdate executable: %s\n", sOurExeName.c_str());

	// Fetch the original Command Line (we are going to re-use it).
	std::string sOrigCmdLine = GetCommandLine();
	std::string sLowerCmdLine = StringToLowerCase(sOrigCmdLine);
	std::string sNewCmdLine = sOrigCmdLine; // We will remove 'ourself' in a bit.
	DebugLog("Original Command Line = '%s'\n", sOrigCmdLine.c_str());

	// Find and remove ourself from the command line.
	nPos = sLowerCmdLine.find(sLowerExeName);
	if (nPos != -1)
	{
		UINT nDelta = nPos + sLowerExeName.length();
		sNewCmdLine.erase(0, nDelta);

		if (sNewCmdLine.length() && sNewCmdLine[0] == '\"')
			sNewCmdLine.erase(0, 1); // Bypass the ending quote.

		if (sNewCmdLine.length())
			sNewCmdLine.erase(0, 1); // Bypass the space.
	}
	DebugLog("New Command Line (for restart) = '%s'\n", sNewCmdLine.c_str());

	// We need the full path to the patch exe (replace the '.\' with our folder).
	std::string sFullPatchPath = CorrectPath(sPatchExe);
	DebugLog("Full update path = '%s'\n", sFullPatchPath.c_str());

	// Write the configuration file the the restarter will use.
	WritePrivateProfileString(sRestartSec, sWaitTimeKey, sRestartWaitTime, sRestartIniFile);
	WritePrivateProfileString(sRestartSec, sWaitAppKey, sSelf, sRestartIniFile);
	WritePrivateProfileString(sRestartSec, sRestartParamsKey, sNewCmdLine.c_str(), sRestartIniFile);
	WritePrivateProfileString(sRestartSec, sUpdateAppKey, sFullPatchPath.c_str(), sRestartIniFile);
	WritePrivateProfileString(sRestartSec, sUpdateParamsKey, sSelfDir.c_str(), sRestartIniFile);
	WritePrivateProfileString(sRestartSec, sDebugKey, GetCustomInfo()->GetDebug() ? "1" : "0", sRestartIniFile);

	std::string sCmdLine = "\"";
	sCmdLine += sRestartExe;
	sCmdLine += TEXT("\" ");
	sCmdLine += sRestartIniFile;

	STARTUPINFO SI;
	ZeroMemory(&SI, sizeof(SI));
	SI.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION PI;

	DebugLog("Launching SierraUpRestarter\n");
	BOOL bRes = CreateProcess(NULL, (LPTSTR)(LPCTSTR)sCmdLine.c_str(), NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &SI, &PI);

	if (bRes)
	{
		// Clean up.
		CloseHandle(PI.hThread);
		CloseHandle(PI.hProcess);

		return true;
	}
	else
	{
		GUIString sMsg;

		if (GetLastError() == 2)
			sMsg = pResMgr->GetFinishedString(Global_NoFindRestarter_String);
		else
		{
			sMsg = pResMgr->GetFinishedString(Global_NoRunRestarter_String);
			ReplaceSubInt(sMsg, "%ERROR%", GetLastError());
		}

		DebugLog("Error Starting SierraUpRestarter: %s\n", std::string(sMsg).c_str());
		MessageBox(g_pWindow, sMsg, pResMgr->GetFinishedString(Global_RestarterFailed_String), MB_OK);
		return false;
	}
}

//----------------------------------------------------------------------------------
// RemoveRestarterFiles: Removes files created to allow SierraUp to restart itself.
//----------------------------------------------------------------------------------
void RemoveRestarterFiles(void)
{
	DeleteFile(sRestartExe);
	DeleteFile(sPsapiDll);

	if (! GetCustomInfo()->GetDebug())
		DeleteFile(sRestartIniFile);
}

//----------------------------------------------------------------------------------
// Finished: The core claims that we are finished, so shut down.
//----------------------------------------------------------------------------------
void Finished(int nReturnCode)
{
	if (GetMainControl()->GetReturnResult() == SUR_PATCHING_SELF)
	{
		if (ExtractRestarter())
			LaunchRestarter(GetCustomInfo()->GetPatchFile());
	}

	g_pWindow->Close();
}

//----------------------------------------------------------------------------------
// TitleChanged: Time to upadte the title.
//----------------------------------------------------------------------------------
void TitleChanged(const GUIString& sTitle)
{
	g_pWindow->SetTitle(sTitle);
}

/*
//----------------------------------------------------------------------------------
// HandleMinimizeButton: Process a click on the minimize button.
//----------------------------------------------------------------------------------
static void HandleMinimizeButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return;
}

//----------------------------------------------------------------------------------
// HandleCloseButton: Process a click on the close button.
//----------------------------------------------------------------------------------
static void HandleCloseButton(ComponentEvent* pEvent)
{
	if (pEvent->mType != ComponentEvent_ButtonPressed)
		return;
}
*/

//----------------------------------------------------------------------------------
// Display the specified help file.
//----------------------------------------------------------------------------------
void ShowDummyHelp(const std::string& sDialog)
{
	std::string sMsg;
	sMsg  = "This is Dummy Help for the " + sDialog + " Dialog.";
	sMsg += "\n\n";
	sMsg += "If this was a real help system, you might actually see something useful";
	sMsg += "\n\n";
	sMsg += "As it is, there is no real help availible at this time.  Sorry.";

	MessageBox(g_pWindow, sMsg, "Dummy Help System", MD_OK);
}

//----------------------------------------------------------------------------------
// WelcomeDlgHelpCallback: Callback to invoke if the user clicks on the help button 
// on the Welcome dialog.
//----------------------------------------------------------------------------------
void WelcomeDlgHelpCallback(void)
{
	ShowDummyHelp("Welcome");
}

//----------------------------------------------------------------------------------
// ConfigProxyDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Configure Proxy dialog.
//----------------------------------------------------------------------------------
void ConfigProxyDlgHelpCallback(void)
{
	ShowDummyHelp("Configure Proxy");
}

//----------------------------------------------------------------------------------
// MotdDlgHelpCallback: Callback to invoke if the user clicks on the help button on 
// the MotD dialog.
//----------------------------------------------------------------------------------
void MotdDlgHelpCallback(void)
{
	ShowDummyHelp("Message of the Day");
}

//----------------------------------------------------------------------------------
// VersionCheckDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Version Check dialog.
//----------------------------------------------------------------------------------
void VersionCheckDlgHelpCallback(void)
{
	ShowDummyHelp("Version Check");
}

//----------------------------------------------------------------------------------
// OptionalPatchDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Optional Patch dialog.
//----------------------------------------------------------------------------------
void OptionalPatchDlgHelpCallback(void)
{
	ShowDummyHelp("Optional Update");
}

//----------------------------------------------------------------------------------
// PatchDetailsDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Patch Details dialog.
//----------------------------------------------------------------------------------
void PatchDetailsDlgHelpCallback(void)
{
	ShowDummyHelp("Update Details");
}

//----------------------------------------------------------------------------------
// SelectHostDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Select Host dialog.
//----------------------------------------------------------------------------------
void SelectHostDlgHelpCallback(void)
{
	ShowDummyHelp("Select Host");
}

//----------------------------------------------------------------------------------
// DownloadDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Download dialog.
//----------------------------------------------------------------------------------
void DownloadDlgHelpCallback(void)
{
	ShowDummyHelp("Download");
}

//----------------------------------------------------------------------------------
// WebLaunchDlgHelpCallback: Callback to invoke if the user clicks on the help 
// button on the Visit Host dialog.
//----------------------------------------------------------------------------------
void WebLaunchDlgHelpCallback(void)
{
	ShowDummyHelp("Web Launch (Visit Host)");
}

//----------------------------------------------------------------------------------
// InitializeHelp: Dummy Help System - enabled only for debugging (and as an 
// example for developers who whish to directly include SierraUp in their 
// application).
//----------------------------------------------------------------------------------
void InitializeHelp(void)
{
/*
#ifdef _DEBUG
	GetCustomInfo()->SetWelcomeHelpCallback(WelcomeDlgHelpCallback);
	GetCustomInfo()->SetConfigProxyHelpCallback(ConfigProxyDlgHelpCallback);
	GetCustomInfo()->SetMotdHelpCallback(MotdDlgHelpCallback);
	GetCustomInfo()->SetVersionCheckHelpCallback(VersionCheckDlgHelpCallback);
	GetCustomInfo()->SetOptionalPatchHelpCallback(OptionalPatchDlgHelpCallback);
	GetCustomInfo()->SetPatchDetailsHelpCallback(PatchDetailsDlgHelpCallback);
	GetCustomInfo()->SetSelectHostHelpCallback(SelectHostDlgHelpCallback);
	GetCustomInfo()->SetDownloadHelpCallback(DownloadDlgHelpCallback);
	GetCustomInfo()->SetWebLaunchHelpCallback(WebLaunchDlgHelpCallback);
#endif
*/
}

//----------------------------------------------------------------------------------
// ShowScreen: Display the provided screen.
//----------------------------------------------------------------------------------
void ShowScreen(Container* pScreen)
{
	g_pWindow->EndPopup();
	g_pWindow->GetRoot()->RemoveAllChildren();
	pScreen->SetPosSize(0, 0, g_pWindow->GetRoot()->Width(), g_pWindow->GetRoot()->Height());
	g_pWindow->AddComponent(pScreen);
	g_pWindow->GetRoot()->AddChildLayout(pScreen, CLI_SameSize, g_pWindow->GetRoot());
}

//----------------------------------------------------------------------------------
// CreateMainScreen: Create the main screen.
//----------------------------------------------------------------------------------
void CreateMainScreen(void)
{
	int nWidth = g_pWindow->GetRoot()->Width();
	int nHeight = g_pWindow->GetRoot()->Height();
	g_pMainScreen = new Container;
	g_pMainScreen->SetSize(nWidth, nHeight);

	WONUpdateCtrlPtr pCtrl = new WONUpdateCtrl();
	ResourceConfigTablePtr aTable = pCtrl->InitScreens();
	g_pMainScreen->AddChildLayout(pCtrl, CLI_SameSize, g_pMainScreen);
	pCtrl->SetFinishedCallback(Finished);
	pCtrl->SetTitleChangedCallback(TitleChanged);

	g_pAboutDlg = pCtrl->GetAboutCtrl();
	CustomInfo* pCI = GetCustomInfo();
	g_pAboutDlg->SetVersion(pCI->GetSierraUpVersion());

	g_pMainScreen->AddChild(pCtrl);
}

//----------------------------------------------------------------------------------
// InitWindow: Initialize the main window.
//----------------------------------------------------------------------------------
void InitWindow(Window* pWindow)
{
	// Create Window
	WONRectangle aScreenRect;
	g_pWindowMgr->GetScreenRect(aScreenRect);
	int aCreateFlags = CreateWindow_Centered | CreateWindow_SizeSpecClientArea | CreateWindow_HideAtFirst;
	int aWidth = 640;
	int aHeight = 480;

//	// If the dialog is not resizeable, disable that ability now.
//	if (! GetCustomInfo()->GetResourceManager()->GetResizeable())
//		aCreateFlags |= CreateWindow_NotSizeable;

	pWindow->SetCreateFlags(aCreateFlags);
	pWindow->Create(WONRectangle(0, 0, aWidth, aHeight));

	// Make sure the user cannot make it smaller than the original starting size.
	WONRectangle rScreen;
	pWindow->GetScreenPos(rScreen);
	pWindow->SetMinWindowSize(rScreen.Width(), rScreen.Height());
}

//----------------------------------------------------------------------------------
// UpdateSystemMenu: Add the 'About' menu item to the system menu.
//----------------------------------------------------------------------------------
void UpdateSystemMenu(void)
{
	assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	assert(IDM_ABOUTBOX < 0xF000);

	// Add 'About...' menu item to system menu.
	HWND hWnd = g_pWindow->GetHWND();
	HMENU hMenu = GetSystemMenu(hWnd, FALSE);

	AppendMenu(hMenu, MF_SEPARATOR, 0, 0L);
	AppendMenu(hMenu, MF_STRING, IDM_ABOUTBOX, "About");
}

//----------------------------------------------------------------------------------
// UpdateIcon: Load the icon from the resource files, and attach it to the windows.
//----------------------------------------------------------------------------------
void UpdateIcon(void)
{
	DisplayContext *aContext = g_pWindow->GetDisplayContext();

	std::string anIconFile;
	if(!(GetCustomInfo()->GetResourceDirectory().empty()))
	{
		anIconFile = GetCustomInfo()->GetResourceDirectory();
		char anEndChar = anIconFile[anIconFile.size()-1];
		if(anEndChar != '/' && anEndChar != '\\')
			anIconFile += '/';
	}
	anIconFile += ("SierraUp.ico");

	HICON anHIcon = (HICON)LoadImage(NULL,anIconFile.c_str(),IMAGE_ICON,0,0,LR_LOADFROMFILE);
	if(anHIcon==NULL)
		anHIcon = LoadIcon(g_pWindowMgr->GetHinstance(),MAKEINTRESOURCE(IDI_SIERRAUP));

	if(anHIcon!=NULL)
	{
		MSNativeIconPtr anIcon = new MSNativeIcon(aContext, anHIcon);
		g_pWindow->SetIcon(anIcon);
		g_pWindowMgr->SetDefaultWindowIcon(anIcon);
	}
}

//----------------------------------------------------------------------------------
// WindowProc: Alternate window proc (so we can look for clicks on the 'about' 
// system menu item).
//----------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Process the user's selection of the About Menu item (on the System Menu).
	if (uMsg == WM_SYSCOMMAND)
	{
		if ((wParam & 0xFFF0) == IDM_ABOUTBOX)
		{
			g_pAboutDlg->DoDialog(g_pWindow);
			return false;
		}

		if ((wParam & 0xFFF0) == SC_CLOSE)
		{
			// If we are looking at the Download screen, have the user validate the operation.
			if (GetMainControl()->GetScreen() == WONUpdateCtrl::WS_Download)
			{
				GetMainControl()->GetScreenCtrl()->FireBackButton();
				return false;
			}
		}
	}

	if (g_pGUIProc)
		return CallWindowProc(g_pGUIProc, hWnd, uMsg, wParam, lParam);

	return false;
}		

//----------------------------------------------------------------------------------
// WONGUI_ENTRY_POINT: Cross platform executable entry point.  AKA WinMain on
// Windows systems.
//----------------------------------------------------------------------------------
WONGUI_ENTRY_POINT
{
	Container::SetMouseWheelGoesToFocusChildDef(false);

//	int anArgc;
//	LPWSTR *anArgv;
//	anArgv = CommandLineToArgvW(GetCommandLineW(),&anArgc);
//	if(anArgc>1)
//		gFullScreen = true;
//
//	GlobalFree(anArgv);

	g_pWindowMgr = new PlatformWindowManager;
	JPGDecoder::Add(g_pWindowMgr->GetImageDecoder());
	PNGDecoder::Add(g_pWindowMgr->GetImageDecoder());
	g_pWindow = new PlatformWindow;

	// Dummy Help System - enabled only for debugging (and as an example).
	InitializeHelp();

	GetCustomInfo()->SetSierraUpVersion(GetExeVersion());

	// InitWindow
	InitWindow(g_pWindow);

	// Safety background.
	g_pWindow->SetTitle("SierraUpdate");
//	g_pWindowMgr->InitSystemColorScheme();
	MSDlgBackPtr pBackground = new MSDlgBack;
	Container *aRoot = g_pWindow->GetRoot();
	aRoot->AddChildLayout(pBackground, CLI_SameSize, aRoot);
	aRoot->AddChild(pBackground);

	// Load the configuration settings:
	ParameterParser Parser;
	bool bParsed = Parser.InitConfigurationSettings(GetCommandLineW());

	FileReader::AddBigFile(new WONBigFile(MAKEINTRESOURCE(IDR_WBF1),"WBF"),GetCustomInfo()->GetResourceDirectory().c_str());

	// Remove files created when we restart ourself.
	RemoveRestarterFiles();

	// Check after InitWindow (so we can set the resizable flag correctly).
	if (! bParsed)
	{
		MessageBox(g_pWindow, Parser.GetLastError(), "SierraUpdate Parameter Error", MD_OK);
		g_pWindowMgr->CloseAllWindows();
		return 0; // Bail out - we are 'done'.
	}

	// If the dialog is not resizeable, disable that ability now.
	if (! GetCustomInfo()->GetResizable())
	{
		HWND hWnd = g_pWindow->GetHWND();
		DWORD nStyle = GetWindowLong(hWnd, GWL_STYLE);
		nStyle = nStyle & ~ (WS_SIZEBOX | WS_MAXIMIZEBOX);
		SetWindowLong(hWnd, GWL_STYLE, nStyle);
		SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE);
	}

	int nWd = GetCustomInfo()->GetStartWidth();
	int nHt = GetCustomInfo()->GetStartHeight();
	if (nWd && nHt)
	{
		HWND hWnd = g_pWindow->GetHWND();
		int nLeft = (GetSystemMetrics(SM_CXSCREEN) - nWd) / 2;
		int nTop = (GetSystemMetrics(SM_CYSCREEN) - nHt) / 2;
		SetWindowPos(hWnd, NULL, nLeft, nTop, nWd, nHt, SWP_NOZORDER);
	}

	// Initialize Sub-windows
	CreateMainScreen();
	GetCustomInfo()->GetResourceManager()->Init();

	ShowScreen(g_pMainScreen);
	g_pWindow->Show(true);

	// Customizations.
	UpdateSystemMenu();
	UpdateIcon();

	HWND hWnd = g_pWindow->GetHWND();
	g_pGUIProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);

	// Put everything into one thread.
	g_pPumpThread = new PumpThread();
	g_pPumpThread->Start();

	return g_pWindowMgr->MessageLoop();

	delete g_pPumpThread;
}