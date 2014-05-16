//----------------------------------------------------------------------------------
// PatchUtils.h : definition file.
//----------------------------------------------------------------------------------
#ifndef __PatchUtils_H__
#define __PatchUtils_H__

#ifndef _STRING_
#include <string>
#endif

#include "WONGUI/GUIString.h"


//----------------------------------------------------------------------------------
// Constants.
//----------------------------------------------------------------------------------
const std::string sNoVersion = "0.0.0.0";


//----------------------------------------------------------------------------------
// Prototypes.
//----------------------------------------------------------------------------------
void ReleaseControl(DWORD tSpin = 0);

std::string ChangeFileExtension(const std::string& sFile, const std::string& sNewExt);
std::string GenerateTempFileName(std::string sPrefix = TEXT("FS"), std::string sExt = TEXT("tmp"));
DWORD GetFileSize(const std::string& sFileName);
FILETIME GetFileCreateTime(const std::string& sFileName);
void SetFileCreateTime(const std::string& sFileName, FILETIME ftModified);
FILETIME GetFileModifiedTime(const std::string& sFileName);
void SetFileModifiedTime(const std::string& sFileName, FILETIME ftModified);
bool FileExists(const std::string& sDir);
bool DirectoryExists(const std::string& sDir);
bool CreateDirectoryRecursive(const std::string& sDir);
std::string CorrectPath(const std::string& sFile);

bool IsFtpAddress(const WONAPI::GUIString& sURL);
int FindNextLine(const WONAPI::GUIString& sFileContents, int nStartPos);
int FindNextComment(const WONAPI::GUIString& sFileContents, int nStartPos);
WONAPI::GUIString CopyLine(const WONAPI::GUIString& sFileContents, int nStart);

WONAPI::GUIString NumToStrWithCommas(int nVal);
bool ReplaceSubString(WONAPI::GUIString& sString, const WONAPI::GUIString& sKey, const WONAPI::GUIString& sValue);
bool ReplaceSubInt(WONAPI::GUIString& sString, const WONAPI::GUIString& sKey, int nValue);

WONAPI::GUIString ExtractParam(WONAPI::GUIString& sCmdLine, const WONAPI::GUIString& sKey, const WONAPI::GUIString& sDef);
bool ExtractValuelessParam(WONAPI::GUIString& sCmdLine, const WONAPI::GUIString& sKey);
bool ReplaceParameter(WONAPI::GUIString& sCmdLine, const WONAPI::GUIString& sKey, const WONAPI::GUIString& sVal);

bool IsColorString(const WONAPI::GUIString& sColor);
DWORD StringToRGB(const WONAPI::GUIString& sColor);

bool IsVersion(const std::string& sVer);
int CompareVersions(const std::string& sVer1, const std::string& sVer2);
std::string GetExeVersion(const std::string& sFile);
std::string GetExeVersion(void);

#endif
