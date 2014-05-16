//----------------------------------------------------------------------------------
// CustomInfo.cpp
//----------------------------------------------------------------------------------
#include "CustomInfo.h"
#include "WonUpdateCtrl.h"
#include "MessageDlg.h"
#include "PatchUtils.h"

using namespace WONAPI;


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
const std::string sPatchSettingsFile = "PatchSettings.cfg";

const char sPatchListSec[]           = "PatchList";
const char sDefHostKey[]             = "DefaultHost";

const char sMonitoredSec[]           = "Monitored";
const char sLastVersionKey[]         = "LastVersion";

const char sFtpPatchInfoSec[]        = "FtpPatchInto";

///const int         DEF_TIMEOUT        = 120000; // 2 minutes is plenty long enough.
///const std::string DEF_PATCH_DESC_EXE = TEXT("notepad.exe");


//----------------------------------------------------------------------------------
// Global Variables;
//----------------------------------------------------------------------------------
CustomInfo g_CustomInfo;


//----------------------------------------------------------------------------------
// Static member variables.
//----------------------------------------------------------------------------------
SierraUpHelpCallbackProc CustomInfo::m_pWelcomeHelpProc       = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pConfigProxyHelpProc   = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pMotdHelpProc          = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pVersionCheckHelpProc  = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pOptionalPatchHelpProc = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pPatchDetailsHelpProc  = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pSelectHostHelpProc    = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pDownloadHelpProc      = NULL;
SierraUpHelpCallbackProc CustomInfo::m_pWebLaunchHelpProc     = NULL;
std::string              CustomInfo::m_sWelcomeHelpFile;
std::string              CustomInfo::m_sConfigProxyHelpFile;
std::string              CustomInfo::m_sMotdHelpFile;
std::string              CustomInfo::m_sVersionCheckHelpFile;
std::string              CustomInfo::m_sOptionalPatchHelpFile;
std::string              CustomInfo::m_sPatchDetailsHelpFile;
std::string              CustomInfo::m_sSelectHostHelpFile;
std::string              CustomInfo::m_sDownloadHelpFile;
std::string              CustomInfo::m_sWebLaunchHelpFile;
std::string              CustomInfo::m_sShowHelpFileExe;


//----------------------------------------------------------------------------------
// CustomInfo Constructor.
//----------------------------------------------------------------------------------
CustomInfo::CustomInfo(void)
{
	m_sDisplayName           = "";
	m_sProductName           = "";
	m_sExtraConfig           = "";

	m_sPatchFolder           = "";
	m_sPatchTypes            = "";
///	m_sPatchDescExe          = "";
	m_sNoPatchURL            = "";
	m_sCurVersion            = "";
	m_sNewVersion            = "";
	m_sUpdateVersion         = "";

	m_pDirSrvrNames          = new std::list<std::string>();
	m_pDirSrvrs              = new ServerContext();

	m_pSelectedPatch         = NULL;
	m_bMixedPatches          = false;
	m_bOptionalUpgrade       = false;

	m_sPatchFile             = "";

	m_tMotdTimeout           =  60000; // 1 minute.
	m_tVersionTimeout        = 120000; // 2 minutes.
	m_tPatchTimeout          =  60000; // 1 minute.
	m_tPatchDescTimeout      =  60000; // 1 minute.

	m_bAutoStart             = false;
	m_bMonitorPatch          = false;

	m_bAutomaticMode         = false;

	m_bDebug                 = false;

	m_bResizable             = false;
	m_iStartWidth			 = 640;
	m_iStartHeight			 = 480;
}

//----------------------------------------------------------------------------------
// CustomInfo Destructor.
//----------------------------------------------------------------------------------
CustomInfo::~CustomInfo(void)
{
	if (m_pDirSrvrNames)
	{
		m_pDirSrvrNames->clear();
		delete m_pDirSrvrNames;
		m_pDirSrvrNames = NULL;
	}
}

//----------------------------------------------------------------------------------
// ClearPatchList: Empty the patch data list.
//----------------------------------------------------------------------------------
void CustomInfo::ClearPatchList(void)
{
	SetSelectedPatch(NULL);

	CPatchDataList::iterator Itr = m_PatchDataList.begin();

	while (Itr != m_PatchDataList.end())
	{
		delete *Itr;
		m_PatchDataList.erase(Itr);
		Itr = m_PatchDataList.begin();
	}
}

//----------------------------------------------------------------------------------
// ShowHelpFile: Invoke the application that shold display the supplied help file.
//----------------------------------------------------------------------------------
void ShowHelpFile(const std::string& sFile)
{
	std::string sExe = CustomInfo::GetShowHelpFileExe();
	if (! sExe.length())
		sExe = "notepad.exe";

	TCHAR sCmdLine[MAX_PATH];
	wsprintf(sCmdLine, "%s %s", sExe.c_str(), sFile.c_str());
	UINT nRes = WinExec(sCmdLine, SW_SHOW);
	if (nRes < 32)
	{
		wsprintf(sCmdLine, "notepad.exe %s", sFile);
		nRes = WinExec(sCmdLine, SW_SHOW);
		if (nRes < 32)
		{
//			if (GetMainControl() && GetMainControl()->GetScreenCtrl())
			{
				ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

				GUIString sInfo = pResMgr->GetFinishedString(Global_BadHelpExe_String);
				ReplaceSubString(sInfo, "%FILE%", sFile);
				MessageBox(GetMainControl()->GetScreenCtrl()->GetWindow(), sInfo, pResMgr->GetFinishedString(Global_Error_String), MD_OK);
			}
		}
	}
}

//----------------------------------------------------------------------------------
// ShowWelcomeHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowWelcomeHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetWelcomeHelpFile());
}

//----------------------------------------------------------------------------------
// SetWelcomeHelpFile: Set the file to display when the dialog's help button is 
// pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetWelcomeHelpFile(const std::string& sHelpFile)
{
	m_sWelcomeHelpFile = sHelpFile;
	SetWelcomeHelpCallback(ShowWelcomeHelpFile);
}

//----------------------------------------------------------------------------------
// ShowConfigProxyHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowConfigProxyHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetConfigProxyHelpFile());
}

//----------------------------------------------------------------------------------
// SetConfigProxyHelpFile: Set the file to display when the dialog's help button is 
// pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetConfigProxyHelpFile(const std::string& sHelpFile)
{
	m_sConfigProxyHelpFile = sHelpFile;
	SetConfigProxyHelpCallback(ShowConfigProxyHelpFile);
}

//----------------------------------------------------------------------------------
// ShowMotdHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowMotdHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetMotdHelpFile());
}

//----------------------------------------------------------------------------------
// SetMotdHelpFile: Set the file to display when the dialog's help button is 
// pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetMotdHelpFile(const std::string& sHelpFile)
{
	m_sMotdHelpFile = sHelpFile;
	SetMotdHelpCallback(ShowMotdHelpFile);
}

//----------------------------------------------------------------------------------
// ShowVersionCheckHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowVersionCheckHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetVersionCheckHelpFile());
}

//----------------------------------------------------------------------------------
// SetVersionCheckHelpFile: Set the file to display when the dialog's help button 
// is pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetVersionCheckHelpFile(const std::string& sHelpFile)
{
	m_sVersionCheckHelpFile = sHelpFile;
	SetVersionCheckHelpCallback(ShowVersionCheckHelpFile);
}

//----------------------------------------------------------------------------------
// ShowOptionalPatchHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowOptionalPatchHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetOptionalPatchHelpFile());
}

//----------------------------------------------------------------------------------
// SetOptionalPatchHelpFile: Set the file to display when the dialog's help button 
// is pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetOptionalPatchHelpFile(const std::string& sHelpFile)
{
	m_sOptionalPatchHelpFile = sHelpFile;
	SetOptionalPatchHelpCallback(ShowOptionalPatchHelpFile);
}

//----------------------------------------------------------------------------------
// ShowPatchDetailsHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowPatchDetailsHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetPatchDetailsHelpFile());
}

//----------------------------------------------------------------------------------
// SetPatchDetailsHelpFile: Set the file to display when the dialog's help button 
// is pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetPatchDetailsHelpFile(const std::string& sHelpFile)
{
	m_sPatchDetailsHelpFile = sHelpFile;
	SetPatchDetailsHelpCallback(ShowPatchDetailsHelpFile);
}

//----------------------------------------------------------------------------------
// ShowSelectHostHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowSelectHostHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetSelectHostHelpFile());
}

//----------------------------------------------------------------------------------
// SetSelectHostHelpFile: Set the file to display when the dialog's help button is 
// pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetSelectHostHelpFile(const std::string& sHelpFile)
{
	m_sSelectHostHelpFile = sHelpFile;
	SetSelectHostHelpCallback(ShowSelectHostHelpFile);
}

//----------------------------------------------------------------------------------
// ShowDownloadHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowDownloadHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetDownloadHelpFile());
}

//----------------------------------------------------------------------------------
// SetDownloadHelpFile: Set the file to display when the dialog's help button is 
// pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetDownloadHelpFile(const std::string& sHelpFile)
{
	m_sDownloadHelpFile = sHelpFile;
	SetDownloadHelpCallback(ShowDownloadHelpFile);
}

//----------------------------------------------------------------------------------
// ShowWebLaunchHelpFile: Display the help file associated with this dialog.
//----------------------------------------------------------------------------------
void ShowWebLaunchHelpFile(void)
{
	ShowHelpFile(CustomInfo::GetWebLaunchHelpFile());
}

//----------------------------------------------------------------------------------
// SetWebLaunchHelpFile: Set the file to display when the dialog's help button is 
// pressed.
//----------------------------------------------------------------------------------
void CustomInfo::SetWebLaunchHelpFile(const std::string& sHelpFile)
{
	m_sWebLaunchHelpFile = sHelpFile;
	SetWebLaunchHelpCallback(ShowWebLaunchHelpFile);
}

//----------------------------------------------------------------------------------
// AppendLaunchParameters: Added the supplied paraemters to the launch parameters.
//----------------------------------------------------------------------------------
void CustomInfo::AppendLaunchParameters(const std::string& sParams)
{
	if (m_sLaunchParams.length())
		m_sLaunchParams += " ";
	m_sLaunchParams += sParams;
}

//----------------------------------------------------------------------------------
// SetPatchFolder: Set the folder that the patch will be saved into.
//----------------------------------------------------------------------------------
void CustomInfo::SetPatchFolder(const std::string& sFolder)
{
	TCHAR cBackSlash = '\\';

	// Replace '.' and '..' with the correct info.
	m_sPatchFolder = CorrectPath(sFolder);

	// Make sure it ends with a backslash.
	int nLen = m_sPatchFolder.length();
	int nLastSlash = m_sPatchFolder.rfind(cBackSlash);
	
	if (nLen && nLastSlash != nLen - 1)
		m_sPatchFolder += cBackSlash;
}

//----------------------------------------------------------------------------------
// GetPatchFolder: Fetch the folder that the patch will be saved into.
//----------------------------------------------------------------------------------
std::string CustomInfo::GetPatchFolder(void)
{
	// If no path was specified, use the temp directory.
	if (! m_sPatchFolder.length())
	{
		char sTempDir[MAX_PATH];
		GetTempPath(MAX_PATH, sTempDir);
		SetPatchFolder(sTempDir);
	}

	return m_sPatchFolder;
}

//----------------------------------------------------------------------------------
// GetDirectoryServers: Assemble and return the list of directory servers.
// Note: On some versions of Win95, this will cause a three minute timeout (if the 
// user is not online), so we package this here so we can put in in a thread.
//----------------------------------------------------------------------------------
WONAPI::ServerContextPtr CustomInfo::GetDirectoryServers(void)
{
	m_pDirSrvrs->Clear();

	std::list<std::string>::iterator Itr = m_pDirSrvrNames->begin();

	while (Itr != m_pDirSrvrNames->end())
	{
		std::string sDirSrvr = *Itr;
		m_pDirSrvrs->AddAddress(WONAPI::IPAddr(sDirSrvr));
		++Itr;
	}

	return m_pDirSrvrs;
}

//----------------------------------------------------------------------------------
// SaveDefaultSelection: Save the specified host (so we can select it next time)
//----------------------------------------------------------------------------------
void CustomInfo::SaveDefaultSelection(const GUIString& sName)
{
	std::string sFile = m_sPatchFolder + sPatchSettingsFile;
	std::string sPatch = sName;
	WritePrivateProfileString(sPatchListSec, sDefHostKey, sPatch.c_str(), sFile.c_str());
}

//----------------------------------------------------------------------------------
// LoadDefaultSelection: Load the default (if present) host.
//----------------------------------------------------------------------------------
GUIString CustomInfo::LoadDefaultSelection(void)
{
	char sName[MAX_PATH];
	std::string sFile = m_sPatchFolder + sPatchSettingsFile;
	GetPrivateProfileString(sPatchListSec, sDefHostKey, TEXT(""), sName, MAX_PATH, sFile.c_str());
	return sName;
}

//----------------------------------------------------------------------------------
// TagFtpDownload: Write the Settings associated with a particular FTP file 
// download.  We use this info to determine if we should attempt a resumed download.
//----------------------------------------------------------------------------------
void CustomInfo::TagFtpDownload(const std::string& sPatchFile, 
								const std::string& sVersion, 
								long nSize, long nCheckSum)
{
	std::string sFile = m_sPatchFolder + sPatchSettingsFile;

	char sInfo[MAX_PATH];
	wsprintf(sInfo, "%s - %d - %d", sVersion.c_str(), nSize, nCheckSum);

	WritePrivateProfileString(sFtpPatchInfoSec, sPatchFile.c_str(), sInfo, sFile.c_str());
}

//----------------------------------------------------------------------------------
// MatchFtpDownloadTag: Check to see if the patch info for the specified file 
// matches the existing patch info.  If so, we will resume the download.
//----------------------------------------------------------------------------------
bool CustomInfo::MatchFtpDownloadTag(const std::string& sPatchFile, 
									 const std::string& sVersion, 
									 long nSize, long nCheckSum)
{
	std::string sFile = m_sPatchFolder + sPatchSettingsFile;

	char sInfo[MAX_PATH];
	char sCheck[MAX_PATH];
	wsprintf(sCheck, "%s - %d - %d", sVersion.c_str(), nSize, nCheckSum);

	GetPrivateProfileString(sFtpPatchInfoSec, sPatchFile.c_str(), "", sInfo, sizeof(sInfo), sFile.c_str());

	return lstrcmp(sInfo, sCheck) == 0;
}

//----------------------------------------------------------------------------------
// SaveLastMonitoredVersion: Write the last monitored patch version number.
//----------------------------------------------------------------------------------
void CustomInfo::SaveLastMonitoredVersion(const std::string& sVersion)
{
	std::string sFile = m_sPatchFolder + sPatchSettingsFile;
	WritePrivateProfileString(sMonitoredSec, sLastVersionKey, sVersion.c_str(), sFile.c_str());
}

//----------------------------------------------------------------------------------
// GetLastMonitoredVersion: Fetch the last monitored patch version number.
//----------------------------------------------------------------------------------
std::string CustomInfo::GetLastMonitoredVersion(void)
{
	char sVersion[MAX_PATH];
	std::string sFile = m_sPatchFolder + sPatchSettingsFile;
	GetPrivateProfileString(sMonitoredSec, sLastVersionKey, "0.0.0.0", sVersion, MAX_PATH, sFile.c_str());
	return sVersion;
}


//----------------------------------------------------------------------------------
// GetCustomInfo: Accessor function that returns a pointer the the global object.
//----------------------------------------------------------------------------------
extern "C" WONAPI::CustomInfo* GetCustomInfo(void)
//WONAPI::CustomInfo* GetCustomInfo(void)
{
	return &g_CustomInfo;
}
