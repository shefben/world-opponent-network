//----------------------------------------------------------------------------------
// ParameterParser.cpp
//----------------------------------------------------------------------------------
#include "ParameterParser.h"
#include "CustomInfo.h"
#include "PatchUtils.h"
#include "DebugLog.h"
#include "MessageDlg.h"


using namespace WONAPI;


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
const int MAX_PARAM_LEN  = 80000; // Should be massive overkill, but someone will push it.


//----------------------------------------------------------------------------------
// Globals.
//----------------------------------------------------------------------------------
static GUIString g_sDefaultConfigFile       = "SierraUp.cfg";
static GUIString g_sConfigFileKey           = "ConfigFile";
static GUIString g_sWorkingFolderKey        = "WorkingFolder";
static GUIString g_sProductNameKey          = "ProductName";
static GUIString g_sDisplayNameKey          = "DisplayName";
static GUIString g_sPatchFolderKey          = "PatchFolder";
static GUIString g_sCurrentVerKey           = "Version";
static GUIString g_sUpdateVersionKey        = "UpdateVersion";
static GUIString g_sPatchTypesKey           = "PatchTypes";
static GUIString g_sDirServerKey            = "DirectoryServer";
static GUIString g_sWelcomeDlgHlpFileKey    = "WelcomeDlgHelpFile";
static GUIString g_sVerCheckDlgHlpFileKey   = "VersionCheckDlgHelpFile";
static GUIString g_sOptPatchDlgHlpFileKey   = "OptionalPatchDlgHelpFile";
static GUIString g_sSelectDlgHlpFileKey     = "SelectDlgHelpFile";
static GUIString g_sDownloadDlgHlpFileKey   = "DownloadDlgHelpFile";
static GUIString g_sProxyDlgHlpFileKey      = "ProxyDlgHelpFile";
static GUIString g_sVisitHostDlgHlpFileKey  = "VisitHostDlgHelpFile";
static GUIString g_sHelpExeKey              = "HelpExe";
static GUIString g_sExtraConfigKey          = "ExtraConfig";
static GUIString g_sAutoStartKey            = "AutoStart";
static GUIString g_sMonitorPatchKey         = "MonitorPatch";
static GUIString g_sPatchSucceededValKey    = "PatchSucceededValue";
static GUIString g_sVersionExeKey           = "VersionExe";
static GUIString g_sLaunchExeKey            = "LaunchExe";
static GUIString g_sLaunchParamsKey         = "LaunchParams";
static GUIString g_sMotdTimeoutKey          = "MotdTimeout";
static GUIString g_sVersionTimeoutKey       = "VersionTimeout";
static GUIString g_sPatchTimeoutKey         = "PatchTimeout";
static GUIString g_sPatchDescTimeoutKey     = "PatchDescriptionTimeout";
static GUIString g_sDebugKey                = "Debug";
static GUIString g_sValidateVerFileKey      = "ValidateVersionFile";
static GUIString g_sNoPatchUrlKey           = "NoPatchURL";
static GUIString g_sResourceDirectory       = "ResourceDirectory";
static GUIString g_sResizable				= "Resizable";
static GUIString g_sStartWidth				= "StartWidth";
static GUIString g_sStartHeight				= "StartHeight";

ParameterParser::ParameterParser(void)
{
/*
	RegisterStringValue(g_sDefaultConfigFile,
	RegisterStringValue(g_sDefaultConfigFile,
	RegisterStringValue(g_sConfigFileKey,
	RegisterStringValue(g_sWorkingFolderKey,
	RegisterStringValue(g_sProductNameKey,
	RegisterStringValue(g_sDisplayNameKey,
	RegisterStringValue(g_sPatchFolderKey,
	RegisterStringValue(g_sCurrentVerKey,
	RegisterStringValue(g_sUpdateVersionKey,
	RegisterStringValue(g_sPatchTypesKey,
	RegisterStringValue(g_sDirServerKey,
	RegisterStringValue(g_sWelcomeDlgHlpFileKey,
	RegisterStringValue(g_sVerCheckDlgHlpFileKey,
	RegisterStringValue(g_sOptPatchDlgHlpFileKey,
	RegisterStringValue(g_sSelectDlgHlpFileKey,
	RegisterStringValue(g_sDownloadDlgHlpFileKey,
	RegisterStringValue(g_sProxyDlgHlpFileKey,
	RegisterStringValue(g_sVisitHostDlgHlpFileKey,
	RegisterStringValue(g_sHelpExeKey,
	RegisterStringValue(g_sExtraConfigKey,
	RegisterStringValue(g_sAutoStartKey,
	RegisterStringValue(g_sMonitorPatchKey,
	RegisterStringValue(g_sPatchSucceededValKey,
	RegisterStringValue(g_sVersionExeKey,
	RegisterStringValue(g_sLaunchExeKey,
	RegisterStringValue(g_sLaunchParamsKey,
	RegisterULongValue(g_sMotdTimeoutKey,
	RegisterULongValue(g_sVersionTimeoutKey,
	RegisterULongValue(g_sPatchTimeoutKey,
	RegisterULongValue(g_sPatchDescTimeoutKey,
	RegisterStringValue(g_sPatchDescExeKey,
	RegisterBoolValue(g_sDebugKey,
	RegisterStringValue(g_sValidateVerFileKey,
	RegisterStringValue(g_sNoPatchUrlKey,
	RegisterStringValue(g_sResourceDirectory,
	RegisterBoolValue(g_sResizable,
	RegisterIntValue(g_sStartWidth,
	RegisterIntValue(g_sStartHeight,
*/
}

//----------------------------------------------------------------------------------
// RemoveComments: Strip comments from the config file contents.
//----------------------------------------------------------------------------------
void ParameterParser::RemoveComments(GUIString& sFileContents)
{
	int nNextComment = FindNextComment(sFileContents, 0);

	while (nNextComment != -1)
	{
		int nEOL = nNextComment;
		while (sFileContents.at(nEOL) && (sFileContents.at(nEOL) != '\r' && sFileContents.at(nEOL) != '\n'))
			++nEOL;
		if (sFileContents.at(nEOL)) // skip the cr/lf.
			++nEOL;

		sFileContents.erase(nNextComment, nEOL - nNextComment);
		nNextComment = FindNextComment(sFileContents, nNextComment);
	}
}

//----------------------------------------------------------------------------------
// RemoveBlankLines: Strip blank lines from the config file contents.
//----------------------------------------------------------------------------------
void ParameterParser::RemoveBlankLines(GUIString& sFileContents)
{
	int nOffset = 0;
	int nReturn = FindNextLine(sFileContents, 0);

	while (nReturn != -1)
	{
		if (nReturn == nOffset)
			sFileContents.erase(nReturn, 1);
		else
			nOffset = nReturn;

		nReturn = FindNextLine(sFileContents, nReturn);
	}
}

//----------------------------------------------------------------------------------
// SetLastError: Set the last error (so we can display it later).
//----------------------------------------------------------------------------------
bool ParameterParser::SetLastError(const GUIString& sError)
{
	m_sLastError = sError;

	return m_sLastError.length() == 0;
}

//----------------------------------------------------------------------------------
// SetLastErrorWithFile: Set the last error (so we can display it later).
//----------------------------------------------------------------------------------
bool ParameterParser::SetLastErrorWithFile(const GUIString& sError, const GUIString& sFile)
{
	m_sLastError = sError;
	ReplaceSubString(m_sLastError, "%FILE%", sFile);

	return m_sLastError.length() == 0;
}

//----------------------------------------------------------------------------------
// LoadConfigFile: Load the configuration file, and read all parameters from it.
//----------------------------------------------------------------------------------
bool ParameterParser::LoadConfigFile(const std::string& sFile)
{
	ResourceManager* pResMgr = GetCustomInfo()->GetResourceManager();

	FILE* pFile = fopen(sFile.c_str(), "rt");
	if (pFile)
	{
		TCHAR sRawContents[MAX_PARAM_LEN] = "";
		UINT nRead = static_cast<UINT>(fread(sRawContents, 1, sizeof(sRawContents) - 1, pFile));
		if (nRead == sizeof(sRawContents) - 1)
		{
			TCHAR sRead[32];
			wsprintf(sRead, "%d", nRead);
			GUIString sFormat = pResMgr->GetFinishedString(Global_CfgFileTooBig_String);

			ReplaceSubString(sFormat, "%FILE%", sFile);
			ReplaceSubString(sFormat, "%SIZE%", sRead);

			SetLastError(sFormat);
			fclose(pFile);
			return false;
		}
		if (nRead > 0)
		{
			sRawContents[nRead] = 0;
			GUIString sFileContents = sRawContents;
			RemoveComments(sFileContents);
			ExtractParameters(sFileContents);
			fclose(pFile);
			return m_sLastError.length() == 0;
		}

		SetLastErrorWithFile(pResMgr->GetFinishedString(Global_NoReadCfgFile_String), sFile);
		fclose(pFile);
		return false;
	}
	else
	{
		SetLastErrorWithFile(pResMgr->GetFinishedString(Global_NoOpenCfgFile_String), sFile);
		return false;
	}
}

//----------------------------------------------------------------------------------
// UpdateConfigFileCurrentVersion: Update the current version number in the 
// supplied config file.
//
// Note: If the current version is not set in the file, it will NOT be set by the
// call to ReplaceParameter.
//----------------------------------------------------------------------------------
bool ParameterParser::UpdateConfigFileCurrentVersion(const std::string& sFile, const GUIString& sVersion)
{
	return false;
}

//----------------------------------------------------------------------------------
// ExtractConfigFile: Extract the configuration file, and read all parameters from 
// it.
//----------------------------------------------------------------------------------
bool ParameterParser::ExtractConfigFile(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sConfigFileKey, "");
	if (sVal.length())
		return LoadConfigFile(sVal);

	return true; // No file specified, we are fine.
}

//----------------------------------------------------------------------------------
// ExtractWorkingFolder: Extract the Working Folder from the command line/config 
// file.
//----------------------------------------------------------------------------------
bool ParameterParser::ExtractWorkingFolder(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sWorkingFolderKey, "");
	if (sVal.length())
		return SetCurrentDirectory(std::string(sVal).c_str()) == TRUE;
	return true;
}

//----------------------------------------------------------------------------------
// ExtractProductName: Extract the Product Name (ID) from the command line/config 
// file.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractProductName(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sProductNameKey, "");
	if (sVal.length())
		GetCustomInfo()->SetProductName(sVal);
}

//----------------------------------------------------------------------------------
// ExtractDisplayName: Extract the Display Name (for GUI) from the command line/
// config file.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractDisplayName(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sDisplayNameKey, "");
	if (sVal.length())
		GetCustomInfo()->SetDisplayName(sVal);
}

//----------------------------------------------------------------------------------
// ExtractPatchFolder: Extract the Patch Folder from the command line/config file.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractPatchFolder(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sPatchFolderKey, "");
	if (sVal.length())
		GetCustomInfo()->SetPatchFolder(sVal);
}

//----------------------------------------------------------------------------------
// ExtractUpdateVersion: Extract the update version from the command line (n/a to 
// config file).
//----------------------------------------------------------------------------------
void ParameterParser::ExtractUpdateVersion(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sUpdateVersionKey, "");
	if (sVal.length())
		GetCustomInfo()->SetUpdateVersion(sVal);
}

//----------------------------------------------------------------------------------
// ExtractPatchTypes: Extract the patch types string.  This string is passed 
// diorectly to the GetVerOp, so we don't parse it ourself.  If this string 
// contains all of the types for a given patch, that ppatch will be considered.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractPatchTypes(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sPatchTypesKey, "");
	if (sVal.length())
		GetCustomInfo()->SetPatchTypes(sVal);
}

//----------------------------------------------------------------------------------
// ExtractCurrentVersion: Extract the current version from the command line/config 
// file.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractCurrentVersion(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sCurrentVerKey, "");
	if (sVal.length())
		GetCustomInfo()->SetCurVersion(sVal);
}

//----------------------------------------------------------------------------------
// ExtractDirectoryServers: Extract the Directory Servers from the command line/
// config file.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractDirectoryServers(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sDirServerKey, "");
	while (sVal.length())
	{
//		m_DirectoryServers.push_back(sVal);
		GetCustomInfo()->AddDirectoryServer(sVal);
		sVal = ExtractParam(sParams, g_sDirServerKey, "");
	}
}

//----------------------------------------------------------------------------------
// ExtractHelpInfo: Extract the Select Dialog Help File from the command line/
// config file.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractHelpInfo(GUIString& sParams)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	GUIString sVal = ExtractParam(sParams, g_sWelcomeDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetWelcomeHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sVerCheckDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetVersionCheckHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sOptPatchDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetOptionalPatchHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sSelectDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetSelectHostHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sProxyDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetConfigProxyHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sDownloadDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetDownloadHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sVisitHostDlgHlpFileKey, "");
	if (sVal.length())
	{
		// Do range checking.
		if (! FileExists(sVal))
			SetLastErrorWithFile(pResMgr->GetFinishedString(Global_BadHelpFile_String), sVal);
		else
			pCustInfo->SetWebLaunchHelpFile(sVal);
	}

	sVal = ExtractParam(sParams, g_sHelpExeKey, "");
	if (sVal.length())
		pCustInfo->SetShowHelpFileExe(sVal);
}

//----------------------------------------------------------------------------------
// ExtractExtraConfig: Extract the language that the application has been 
// localaized to.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractExtraConfig(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sExtraConfigKey, "");
	if (sVal.length())
	{
		GetCustomInfo()->SetExtraConfig(sVal);
		return;
	}

	// Historic - for original versions.
	sVal = ExtractParam(sParams, "Language", "");
	if (sVal.length())
	{
		DebugLog("Old parameter 'Language' specified, should be changed to 'ExtraConfig'\n");
		GetCustomInfo()->SetExtraConfig(sVal);
	}
}

//----------------------------------------------------------------------------------
// ExtractAutoStart: Extract the switch telling us to auto start the application.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractAutoStart(GUIString& sParams)
{
	if (ExtractValuelessParam(sParams, g_sAutoStartKey))
		GetCustomInfo()->SetAutoStart(true);
}

//----------------------------------------------------------------------------------
// ExtractMonitorPatch: Extract the switch telling us to moinitor the patch (don't 
// shut down).
//----------------------------------------------------------------------------------
void ParameterParser::ExtractMonitorPatch(GUIString& sParams)
{
	if (ExtractValuelessParam(sParams, g_sMonitorPatchKey))
		GetCustomInfo()->SetMonitorPatch(true);
}

//----------------------------------------------------------------------------------
// ExtractPatchSucceededValue: Extract the switch telling us to moinitor the patch 
// (don't shut down).
//----------------------------------------------------------------------------------
void ParameterParser::ExtractPatchSucceededValue(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sPatchSucceededValKey, "");
	if (sVal.length())
		GetCustomInfo()->SetPatchSuccessValue(atoi(std::string(sVal).c_str()));
}

//----------------------------------------------------------------------------------
// ExtractVersionExe: Extract the executble to extract the version from if a patch 
// is applied, and we are monitoring the patch.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractVersionExe(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sVersionExeKey, "");
	if (sVal.length())
		GetCustomInfo()->SetVersionExe(sVal);
}

//----------------------------------------------------------------------------------
// ExtractResourceDirectory: Extract the GUI resource directory.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractResourceDirectory(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sResourceDirectory, "");
	if (sVal.length())
		GetCustomInfo()->SetResourceDirectory(sVal);
}

//----------------------------------------------------------------------------------
// ExtractResizable: Extract Resizable parameter.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractResizable(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sResizable, "");
	if (sVal.length())
	{
		sVal.toLowerCase();
		if (sVal == "true" || sVal == "yes")
			GetCustomInfo()->SetResizable(true);
		else
			GetCustomInfo()->SetResizable(false);
	}
}

//----------------------------------------------------------------------------------
// ExtractStartWidth: Extract StartWidth parameter.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractStartWidth(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sStartWidth, "");
	if (sVal.length())
		GetCustomInfo()->SetStartWidth(sVal.atoi());
}

//----------------------------------------------------------------------------------
// ExtractStartHeight: Extract StartHeight parameter.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractStartHeight(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sStartHeight, "");
	if (sVal.length())
		GetCustomInfo()->SetStartHeight(sVal.atoi());
}

//----------------------------------------------------------------------------------
// ExtractLaunchExe: Extract the executble to run if the patch is up to date.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractLaunchExe(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sLaunchExeKey, "");
	if (sVal.length())
		GetCustomInfo()->SetLaunchExe(sVal);

	// These are cumulative: from the cmd line and cfg file (so keep adding them).
	sVal = ExtractParam(sParams, g_sLaunchParamsKey, "");
	if (sVal.length())
		GetCustomInfo()->AppendLaunchParameters(sVal);
}

//----------------------------------------------------------------------------------
// ExtractValidateVersionFile: Do we need to do an extra version check in a text 
// file?
//
// Tribes2 uses this mechanism to make sure their patches completed.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractValidateVersionFile(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sValidateVerFileKey, "");
	if (sVal.length())
		GetCustomInfo()->SetValidateVersionFile(sVal);
}

//----------------------------------------------------------------------------------
// ExtractNoPatchURL: Extract the URL to go to if we are unable to locate a patch.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractNoPatchURL(GUIString& sParams)
{
	GUIString sVal = ExtractParam(sParams, g_sNoPatchUrlKey, "");
	if (sVal.length())
		GetCustomInfo()->SetNoPatchURL(sVal);
}

//----------------------------------------------------------------------------------
// ExtractTimeouts: Extract the operation timeout values.
//----------------------------------------------------------------------------------
void ParameterParser::ExtractTimeouts(GUIString& sParams)
{
	CustomInfo* pCustInfo = GetCustomInfo();

	GUIString sVal = ExtractParam(sParams, g_sMotdTimeoutKey, "");
	if (sVal.length())
		pCustInfo->SetMotdTimeout(static_cast<DWORD>(atoi(std::string(sVal).c_str())));

	sVal = ExtractParam(sParams, g_sVersionTimeoutKey, "");
	if (sVal.length())
		pCustInfo->SetVersionTimeout(static_cast<DWORD>(atoi(std::string(sVal).c_str())));

	sVal = ExtractParam(sParams, g_sPatchTimeoutKey, "");
	if (sVal.length())
		pCustInfo->SetPatchTimeout(static_cast<DWORD>(atoi(std::string(sVal).c_str())));

	sVal = ExtractParam(sParams, g_sPatchDescTimeoutKey, "");
	if (sVal.length())
		pCustInfo->SetPatchDescTimeout(static_cast<DWORD>(atoi(std::string(sVal).c_str())));
}

//----------------------------------------------------------------------------------
// ExtractDebug: Extract the debug flag
//----------------------------------------------------------------------------------
void ParameterParser::ExtractDebug(GUIString& sParams)
{
	// This can be turned on in a couple of places, so only dump the log stuff once.
	if (! GetCustomInfo()->GetDebug())
	{
		if (ExtractValuelessParam(sParams, g_sDebugKey))
		{
			GetCustomInfo()->SetDebug(true);

			std::string sVer = GetExeVersion();
			DebugLog("======================================================================\n");
			DebugLog("Starting SierraUpdate, Version %s\n", sVer.c_str());
			DebugLog("Command Line: %s\n", std::string(m_sOrigCmdLine).c_str());
		}
	}
}

//----------------------------------------------------------------------------------
// ExtractParameters: Extract all parameters (except config file).
//----------------------------------------------------------------------------------
void ParameterParser::ExtractParameters(GUIString& sParams)
{
	// Extract the debug flag first.
	ExtractDebug(sParams);

	// If specified, extract parameters from a config file (this can cause recursion).
	ExtractConfigFile(sParams);

	// Extract the various parameters (override the config file if appropriate).
	ExtractProductName(sParams);
	ExtractDisplayName(sParams);
	ExtractPatchFolder(sParams);
	ExtractPatchTypes(sParams);
	ExtractCurrentVersion(sParams);
	ExtractDirectoryServers(sParams);
	ExtractHelpInfo(sParams);
	ExtractExtraConfig(sParams);
	ExtractAutoStart(sParams);
	ExtractMonitorPatch(sParams);
	ExtractPatchSucceededValue(sParams);
	ExtractVersionExe(sParams);
	ExtractLaunchExe(sParams);
	ExtractValidateVersionFile(sParams);
	ExtractNoPatchURL(sParams);
	ExtractTimeouts(sParams);
	ExtractResourceDirectory(sParams);
	ExtractResizable(sParams);
	ExtractStartWidth(sParams);
	ExtractStartHeight(sParams);

	// Strip end-of-line comments.
	RemoveComments(sParams);
	RemoveBlankLines(sParams);

	// Look for leftovers.
	if (m_sLastError.length() == 0)
	{
		GUIString sStrippedParams = sParams;
		int nPos = sStrippedParams.findOneOf(TEXT(" \t\r\n"));
		while (nPos != -1)
		{
			sStrippedParams.erase(nPos, 1);
			nPos = sStrippedParams.findOneOf(TEXT(" \t\r\n"));
		}

		if (sStrippedParams.length())
		{
			std::string sFormat = GetCustomInfo()->GetResourceManager()->GetFinishedString(Global_UnknownParam_String);
			DebugLog(sFormat.c_str(), std::string(sParams).c_str());
			DebugLog("\n");
		}
	}
}

//----------------------------------------------------------------------------------
// ParseCommandLine: Parse the command line.
//----------------------------------------------------------------------------------
bool ParameterParser::ParseCommandLine(const GUIString& sCmdLine)
{
	GUIString sParams = sCmdLine;

	SetLastError("");

	// Remove the first parameter - it is our exe name and should not be considered.
	TCHAR cBreak = ' ';
	if (sParams.at(0) == '\"')
	{
		sParams.erase(0, 1);
		cBreak = '\"';
	}
	int nStop = sParams.find(cBreak);
	if (nStop != -1)
		nStop = sParams.length();
	sParams.erase(0, nStop);

	// Now parse what is left.
	ExtractParameters(sParams);

	return m_sLastError.length() == 0;
}

//----------------------------------------------------------------------------------
// InitConfigurationSettings: Parse and extract the command line and default config 
// file.
//----------------------------------------------------------------------------------
bool ParameterParser::InitConfigurationSettings(const GUIString& sRawCmdLine)
{
	m_sOrigCmdLine = sRawCmdLine;

	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	// Start by adjusting the working folder (if needed).
	GUIString sCmdLine = sRawCmdLine;
	ExtractWorkingFolder(sCmdLine);

	// Are we preforming one or more silent commands (should we execute them and shut down)?
	ExtractUpdateVersion(sCmdLine);
	if (pCustInfo->GetUpdateVersion().length())
	{
		UpdateConfigFileCurrentVersion(g_sDefaultConfigFile, pCustInfo->GetUpdateVersion());
		return false;
	}

	// Continue by reading the default config file.
	if (FileExists(g_sDefaultConfigFile))
		if (! LoadConfigFile(g_sDefaultConfigFile))
			return false;

	// Override the defaults.
	if (! ParseCommandLine(sCmdLine))
		return false;
	
	// Additional parameter validation.
	if (! ValidateAppInfo())
	{
		DebugLog("%s\n", std::string(m_sLastError).c_str());
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------
// ValidateAppInfo: Make sure we received all required fields.
//----------------------------------------------------------------------------------
bool ParameterParser::ValidateAppInfo(void)
{
	CustomInfo* pCustInfo = GetCustomInfo();
	ResourceManager* pResMgr = pCustInfo->GetResourceManager();

	if (pCustInfo->GetProductName().length() == 0)
		return SetLastError("Invalid Parameter: No Product Name supplied");

	if (pCustInfo->GetDisplayName().length() == 0)
		return SetLastError("Invalid Parameter: No Display Name supplied");

	if (pCustInfo->GetPatchFolder().length() == 0)
		return SetLastError("Invalid Parameter: No Update Folder supplied");

	if (pCustInfo->GetCurVersion().length() == 0)
		return SetLastError("Invalid Parameter: No Version supplied");

	if (pCustInfo->GetDirectoryServerNames()->size() == 0)
		return SetLastError("Invalid Parameter: No Directory Servers supplied");

	// The other fields are optional.

	return true;
}
