//----------------------------------------------------------------------------------
// ParameterParser.h
//----------------------------------------------------------------------------------
#ifndef __ParameterParser_H__
#define __ParameterParser_H__

#include "WONGUI/GUIString.h"
#include "WONGUI/GUICompat.h"
#include "WONConfig/ConfigObject.h"

namespace WONAPI
{

//----------------------------------------------------------------------------------
// ParameterParser.
//----------------------------------------------------------------------------------
class ParameterParser
{
protected:
	GUIString m_sLastError;
	GUIString m_sOrigCmdLine;

	void RemoveComments(GUIString& sFileContents);
	void RemoveBlankLines(GUIString& sFileContents);
	bool SetLastError(const GUIString& sError);
	bool SetLastErrorWithFile(const GUIString& sError, const GUIString& sFile);
	bool LoadConfigFile(const std::string& sFile);
	bool UpdateConfigFileCurrentVersion(const std::string& sFile, const GUIString& sVersion);
	bool ExtractConfigFile(GUIString& sParams);
	bool ExtractWorkingFolder(GUIString& sParams);
	void ExtractProductName(GUIString& sParams);
	void ExtractDisplayName(GUIString& sParams);
	void ExtractPatchFolder(GUIString& sParams);
	void ExtractUpdateVersion(GUIString& sParams);
	void ExtractPatchTypes(GUIString& sParams);
	void ExtractCurrentVersion(GUIString& sParams);
	void ExtractDirectoryServers(GUIString& sParams);
	void ExtractHelpInfo(GUIString& sParams);
	void ExtractExtraConfig(GUIString& sParams);
	void ExtractAutoStart(GUIString& sParams);
	void ExtractMonitorPatch(GUIString& sParams);
	void ExtractPatchSucceededValue(GUIString& sParams);
	void ExtractVersionExe(GUIString& sParams);
	void ExtractLaunchExe(GUIString& sParams);
	void ExtractResourceDirectory(GUIString& sParams);
	void ExtractResizable(GUIString& sParams);
	void ExtractStartWidth(GUIString& sParams);
	void ExtractStartHeight(GUIString& sParams);
	void ExtractValidateVersionFile(GUIString& sParams);
	void ExtractNoPatchURL(GUIString& sParams);
	void ExtractTimeouts(GUIString& sParams);
	void ExtractDebug(GUIString& sParams);
	void ExtractParameters(GUIString& sParams);
	bool ParseCommandLine(const GUIString& sCmdLine);

public:
	ParameterParser(void);

	bool InitConfigurationSettings(const GUIString& sCmdLine);
	bool ValidateAppInfo(void);
	GUIString GetLastError(void) { return m_sLastError; }
};

} // namespace WONAPI

#endif