//----------------------------------------------------------------------------------
// PatchUtils.cpp : implementaion file.
//----------------------------------------------------------------------------------
#include <direct.h>
#include "PatchUtils.h"


using namespace WONAPI;


//----------------------------------------------------------------------------------
// ReleaseControl: Let other process have a crack at the processer.
//----------------------------------------------------------------------------------
void ReleaseControl(DWORD tSpin)
{
	MSG Msg;

	DWORD tWaitTil = GetTickCount() + tSpin;

	do
	{
		while (PeekMessage(&Msg, NULL, 0, 0, TRUE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	while (GetTickCount() < tWaitTil);
}

//----------------------------------------------------------------------------------
// ChangeFileExtension: Change the extension on the spplied file to the new 
// extension.  If the file does not have an extension, add the new extension to it.
//----------------------------------------------------------------------------------
std::string ChangeFileExtension(const std::string& sFile, const std::string& sNewExt)
{
	std::string sNewFile = sFile;

	int nPos = sNewFile.rfind('.');
	if (nPos != std::string::npos)
		sNewFile.erase(nPos, sNewFile.length() - nPos);

	sNewFile += '.';
	sNewFile += sNewExt;

	return sNewFile;
}

//----------------------------------------------------------------------------------
// GenerateTempFileName: Generate a temporary file name to hold a recipe file.
//----------------------------------------------------------------------------------
std::string GenerateTempFileName(std::string sPrefix, std::string sExt)
{
	char sFileName[MAX_PATH];
	char sTempDir[MAX_PATH];
	GetTempPath(MAX_PATH, sTempDir);
	GetTempFileName(sTempDir, sPrefix.c_str(), 0, sFileName);
	DeleteFile(sFileName); // Sometimes this file is actually created.

	std::string sFile = ChangeFileExtension(sFileName, sExt);

	return sFile;
}

//----------------------------------------------------------------------------------
// GetFileSize: Return the size of the file (in bytes).
//----------------------------------------------------------------------------------
DWORD GetFileSize(const std::string& sFileName)
{
	if (sFileName.length() == 0)
		return 0;

	DWORD nFileSize = 0;
	HANDLE hFile = CreateFile(sFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (hFile != INVALID_HANDLE_VALUE) //lint !e1924
	{
		nFileSize = ::GetFileSize(hFile, 0);
		CloseHandle(hFile);
	}
	return nFileSize;
}

//----------------------------------------------------------------------------------
// GetFileCreateTime: Return the time-stamp of the file.
//----------------------------------------------------------------------------------
FILETIME GetFileCreateTime(const std::string& sFileName)
{
	FILETIME ftCreate = { 0, 0 };

	if (sFileName.length() == 0)
		return ftCreate;

	HANDLE hFile = CreateFile(sFileName.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (hFile != INVALID_HANDLE_VALUE) //lint !e1924
	{
		GetFileTime(hFile, &ftCreate, NULL, NULL);
		CloseHandle(hFile);
	}
	return ftCreate;
}

//----------------------------------------------------------------------------------
// SetFileCreateTime: Set the time-stamp of the file.
//----------------------------------------------------------------------------------
void SetFileCreateTime(const std::string& sFileName, FILETIME ftCreate)
{
	if (sFileName.length() == 0)
		return;

	HANDLE hFile = CreateFile(sFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (hFile != INVALID_HANDLE_VALUE) //lint !e1924
	{
		SetFileTime(hFile, &ftCreate, NULL, NULL);

//		LPVOID lpMsgBuf;
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
//		MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
//		LocalFree(lpMsgBuf);

		CloseHandle(hFile);
	}
}

//----------------------------------------------------------------------------------
// GetFileModifiedTime: Return the time-stamp of the file.
//----------------------------------------------------------------------------------
FILETIME GetFileModifiedTime(const std::string& sFileName)
{
	FILETIME ftModified = { 0, 0 };

	if (sFileName.length() == 0)
		return ftModified;

	HANDLE hFile = CreateFile(sFileName.c_str(), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (hFile != INVALID_HANDLE_VALUE) //lint !e1924
	{
		GetFileTime(hFile, NULL, NULL, &ftModified);
		CloseHandle(hFile);
	}
	return ftModified;
}

//----------------------------------------------------------------------------------
// SetFileModifiedTime: Set the time-stamp of the file.
//----------------------------------------------------------------------------------
void SetFileModifiedTime(const std::string& sFileName, FILETIME ftModified)
{
	if (sFileName.length() == 0)
		return;

	HANDLE hFile = CreateFile(sFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (hFile != INVALID_HANDLE_VALUE) //lint !e1924
	{
		SetFileTime(hFile, NULL, NULL, &ftModified);
		CloseHandle(hFile);
	}
}

//----------------------------------------------------------------------------------
// FileExists: Does the specified file exist?
//----------------------------------------------------------------------------------
bool FileExists(const std::string& sFileName)
{
	if (sFileName.length() == 0)
		return false;

	bool bFound = false;
	WIN32_FIND_DATA FD;
	HANDLE hFile = FindFirstFile(sFileName.c_str(), &FD);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		bFound = true;
		FindClose(hFile);
	}

	return bFound;
}

//----------------------------------------------------------------------------------
// DirectoryExists: Does the specified directory exist?
//----------------------------------------------------------------------------------
bool DirectoryExists(const std::string& sDir)
{
	UINT nLen = sDir.length();

	if (nLen == 0)
		return false;

	// Check for a disk.
	int nColen = sDir.find(':');
	if (nLen == 2 && nColen == 1)
	{
		UINT nType = GetDriveType(sDir.c_str());
		return nType != DRIVE_UNKNOWN;
	}

	DWORD nAttribs = GetFileAttributes(sDir.c_str());
	if (nAttribs == 0xFFFFFFFF)
		return false;

	return (nAttribs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

//----------------------------------------------------------------------------------
// CreateDirectoryRecursive: Recursive directory creation.
//----------------------------------------------------------------------------------
bool CreateDirectoryRecursive(const std::string& sDir)
{
	// If it already exists, we're done, bail out.
	if (DirectoryExists(sDir))
		return true;

	// Try to create the parent (or some ancestor) first.
	std::string sNew = sDir;
	UINT nPos = sNew.rfind('\\');

	if (nPos != 0xFFFFFFFF)
	{
		sNew.erase(nPos, sNew.length() - nPos);
		if (CreateDirectoryRecursive(sNew))
			return CreateDirectory(sDir.c_str(), 0) != FALSE;
	}

	return false;
}

//----------------------------------------------------------------------------------
// CorrectPath. Replace local path info with ridge path info.
//----------------------------------------------------------------------------------
std::string CorrectPath(const std::string& sFile)
{
	std::string sNewFile = sFile;

	if (sNewFile.length() && sNewFile[0] == '.')
	{
		char sLocalPath[MAX_PATH];
		_getcwd(sLocalPath, MAX_PATH);

		// Make sure it ends in a slash.
		int nLen = strlen(sLocalPath);
		if (nLen && sLocalPath[nLen -1] != '\\')
			strcat(sLocalPath, "\\");

		// Replace the 'funny directories'
		if (sFile == ".")
			sNewFile = sLocalPath;
		else if (sFile.find("..\\") == 0)
			sNewFile = sLocalPath + sFile;
		else if (sFile.find(".\\") == 0)
		{
			std::string sTempFile = sFile;
			sTempFile.erase(0, 2); // Remove the leading '.\'
			sNewFile = sLocalPath + sTempFile;
		}
	}

	return sNewFile;
}

//----------------------------------------------------------------------------------
// IsFtpAddress: Is the supplied address an FTP address?
//----------------------------------------------------------------------------------
bool IsFtpAddress(const GUIString& sURL)
{
	GUIString sLower = sURL;
	sLower.toLowerCase();
	
	// Truncate to match FTP tag.
	if (sLower.length() > 6)
		sLower.erase(6, sLower.length() - 6);

	return sLower == "ftp://" || sLower == "ftp:\\\\";
}

//----------------------------------------------------------------------------------
// NumToStrWithCommas: Convert a number to a string that includes commas.
// i.e. 1234567 -> '1,234,567'
//----------------------------------------------------------------------------------
GUIString NumToStrWithCommas(int nVal)
{
	TCHAR sOutput[32];

	if (nVal == 0)
		return "0";

	if (nVal < 0)
	{
		GUIString sVal = TEXT("-");
		sVal.append(NumToStrWithCommas(-nVal));
		return sVal;
	}

	int nOnes = nVal % 1000;
	int nTousands = (nVal / 1000) % 1000;
	int nMillions = (nVal / 1000000) % 1000;
	int nBillions = nVal / 1000000000;

	// Fetch the internationalized numeric separator.
	setlocale(LC_ALL, "");
	lconv* pLocalConv = localeconv();
	char* pSep = pLocalConv->mon_thousands_sep;
	TCHAR cSep = *pSep;

	if (nBillions)
		wsprintf(sOutput, "%d%c%03d%c%03d%c%03d", nBillions, cSep, nMillions, cSep, nTousands, cSep, nOnes);
	else if (nMillions)
		wsprintf(sOutput, "%d%c%03d%c%03d", nMillions, cSep, nTousands, cSep, nOnes);
	else if (nTousands)
		wsprintf(sOutput, "%d%c%03d", nTousands, cSep, nOnes);
	else
		wsprintf(sOutput, "%d", nOnes);

	return sOutput;
}

//----------------------------------------------------------------------------------
// IsStartOfLine: Is nOffset positioned at the start of a line?
//----------------------------------------------------------------------------------
bool IsStartOfLine(const GUIString& sFileContents, int nOffset)
{
	if (nOffset == 0)
		return true;

	return (sFileContents.at(nOffset - 1) == '\r' || sFileContents.at(nOffset - 1) == '\n');
}

//----------------------------------------------------------------------------------
// FindNextLine: Return the offset of the beginning of the next line.
//----------------------------------------------------------------------------------
int FindNextLine(const GUIString& sFileContents, int nStartPos)
{
	// Semi colens and double slashes start comments.
	int nComment = -1;
	int nCR = sFileContents.find('\r', nStartPos);
	int nNL = sFileContents.find('\n', nStartPos);

	if (nCR != -1 && nNL != -1)
		return min(nNL, nCR) + 1;
	else if (nCR != -1)
		return nCR + 1;
	else if (nNL != -1)
		return nNL + 1;
	else
		return -1;
}
//----------------------------------------------------------------------------------
// FindNextComment: Return the offset of the next comment.
//----------------------------------------------------------------------------------
int FindNextComment(const GUIString& sFileContents, int nStartPos)
{
	// Semi colens and double slashes start comments.
	int nComment = -1;
	int nSemiColen = sFileContents.find(';', nStartPos);
	int nDblSlash = sFileContents.find("//", nStartPos);

//	if (nSemiColen != -1)
//		while (! IsStartOfLine(sFileContents, nSemiColen))
//			nSemiColen = sFileContents.find(';', nSemiColen + 1);
	while (nSemiColen != -1 && ! IsStartOfLine(sFileContents, nSemiColen))
		nSemiColen = sFileContents.find(';', nSemiColen + 1);

//	if (nDblSlash != -1)
//		while (! IsStartOfLine(sFileContents, nDblSlash))
//			nDblSlash = sFileContents.find("//", nDblSlash + 1);
	while (nDblSlash != -1 && ! IsStartOfLine(sFileContents, nDblSlash))
		nDblSlash = sFileContents.find("//", nDblSlash + 1);

	if (nSemiColen != -1 && nDblSlash != -1)
		return min(nSemiColen, nDblSlash);
	else if (nSemiColen != -1)
		return nSemiColen;
	else 
		return nDblSlash;
}

//----------------------------------------------------------------------------------
// CopyLine: Duplicate the first line (after nStart) from sFileContents.  Do not
// include the new line or return characters.
//----------------------------------------------------------------------------------
GUIString CopyLine(const GUIString& sFileContents, int nStart)
{
	GUIString sResult;

	while(sFileContents.at(nStart) && sFileContents.at(nStart) != '\r' && sFileContents.at(nStart) != '\n')
		sResult.append(sFileContents.at(nStart++));

	return sResult;
}

//----------------------------------------------------------------------------------
// ReplaceSubString: Replace all instances of sKey with sValue within sString.  The 
// search is NOT case sensitive.
//----------------------------------------------------------------------------------
bool ReplaceSubString(GUIString& sString, const GUIString& sKey, const GUIString& sValue)
{
	bool bRet = false;

	GUIString sLowerString = sString;
	sLowerString.toLowerCase();

	GUIString sLowerKey = sKey;
	sLowerKey.toLowerCase();

	int nPos = sLowerString.find(sLowerKey);
	while (nPos != -1)
	{
		// Replace the 'key' with the 'value'.
		sString.erase(nPos, sKey.length());
		sString.insert(nPos, sValue);

		// Look for another copy of the key.
		sLowerString = sString;
		sLowerString.toLowerCase();
		nPos = sLowerString.find(sLowerKey);
	}

	return bRet;
}

//----------------------------------------------------------------------------------
// ReplaceSubString: Replace all instances of sKey with nValue within sString.  The 
// search is NOT case sensitive.
//----------------------------------------------------------------------------------
bool ReplaceSubInt(WONAPI::GUIString& sString, const WONAPI::GUIString& sKey, int nValue)
{
	char sValue[64];
	wsprintf(sValue, "%d", nValue);
	return ReplaceSubString(sString, sKey, sValue);
}

//----------------------------------------------------------------------------------
// ExtractParam: This routine finds the specified parameter and returns a pointer 
// to it's value.
//
// The returned parameter is sans quotation marks.  
//
// The original string (sCmdLine) is modified by removing the parameter and its 
// corresponding value.
//
// Parameters:
//   cmdLine ... The string to extract the parameter from.
//   sKey ...... The Key to search for.
//   sDef ...... The Default string to return if one was not found.
//----------------------------------------------------------------------------------
GUIString ExtractParam(GUIString& sCmdLine, const GUIString& sKey, const GUIString& sDef)
{
	bool bMatch = false;
	GUIString sValue = sDef;

	// Create lower case strings for searching.
	GUIString sLowerCmdLine = sCmdLine;
	sLowerCmdLine.toLowerCase();

	GUIString sLowerKey = sKey;
	sLowerKey.toLowerCase();

	// Do we have a match (if no key, assume the front)?
	int nMatch = 0;
	if (sLowerKey != "")
		nMatch = sLowerCmdLine.find(sLowerKey);

	if (nMatch != -1)
	{
		bMatch = true;

		// Make sure we are starting a parameter
		if (! (nMatch == 0 || 
			   sLowerCmdLine.at(nMatch - 1) == ' ' ||
			   sLowerCmdLine.at(nMatch - 1) == '\t' || 
			   sLowerCmdLine.at(nMatch - 1) == '\n' ||
			   sLowerCmdLine.at(nMatch - 1) == '\r'))
		{
			// Advance past this 'false hit' and try again.
			GUIString sLeader = sCmdLine;
			GUIString sTrailer = sCmdLine;
			nMatch += sKey.length();
			sLeader.erase(nMatch, sCmdLine.length() - nMatch);
			sTrailer.erase(0, nMatch);

			sValue = ExtractParam(sTrailer, sKey, sDef);
			sCmdLine = sLeader;
			sCmdLine.append(sTrailer);
			return sValue;
		}

		// Find the value in the original string.
		int nValue = nMatch + sKey.length();
		int nEnd = -1;

		// Skip an optional colon
		if(sCmdLine.at(nValue) == ':')
			++nValue;

		// Skip any leading whitespace.
		while (sCmdLine.at(nValue) == ' ' || sCmdLine.at(nValue) == '\t' ||
			   sCmdLine.at(nValue) == '\n' || sCmdLine.at(nValue) == '\r')
			++nValue;

		// Are we dealing with quotes?
		if (sCmdLine.at(nValue) == '\"')
		{
			++nValue;
			nEnd = sCmdLine.find('\"', nValue);
			if (nEnd != -1)
				sCmdLine.setAt(nEnd, ' ');
		}
		else
		{
			// Find the parater value end.
			nEnd = sCmdLine.length();                // End of the string (probably way too far).
			int nTest = sCmdLine.find(' ', nValue);  // Check for interveaning space.
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
			nTest = sCmdLine.find('\t', nValue);     // Check for interveaning tab. 
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
			nTest = sCmdLine.find('\n', nValue);     // Check for interveaning new line. 
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
			nTest = sCmdLine.find('\r', nValue);     // Check for interveaning return. 
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
		}

		// Copy the value from the commad line to the placeholder.
		sValue = sCmdLine;
		sValue.erase(nEnd, sCmdLine.length() - nEnd);
		sValue.erase(0, nValue);

		// Remove any trailing whitespace.
		while (nEnd < sCmdLine.length() && 
			   (sCmdLine.at(nEnd) == ' ' ||
			    sCmdLine.at(nEnd) == '\t' || 
			    sCmdLine.at(nEnd) == '\n' || 
				sCmdLine.at(nEnd) == '\r'))
			++nEnd;

		// Remove trailing comments.
		if (nEnd < sCmdLine.length() + 1 && ((sCmdLine.at(nEnd) == ';') || (sCmdLine.at(nEnd) == '/' && sCmdLine.at(nEnd+1) == '/')))
		{
			int nCR = sCmdLine.find('\r', nEnd);
			int nNL = sCmdLine.find('\n', nEnd);

			if (nCR == -1)
				nCR = nNL;
			if (nCR > nNL)
				nCR = nNL;
			if (nCR != -1)
				nEnd = nCR;

			// Remove leading whitespace.
			while (nEnd < sCmdLine.length() && 
				   (sCmdLine.at(nEnd) == ' ' ||
					sCmdLine.at(nEnd) == '\t' || 
					sCmdLine.at(nEnd) == '\n' || 
					sCmdLine.at(nEnd) == '\r'))
				++nEnd;
		}

		// Remove the key/value pair.
		sCmdLine.erase(nMatch, nEnd - nMatch);
	}

	return sValue;
}

//----------------------------------------------------------------------------------
// ExtractValuelessParam
//
// This routine rerturns true if it finds specified parameter and false 
// otherwise.
// The original string (cmdLine) is modified by removing the parameter.
//
// Parameters:
//   cmdLine ... The string to extract the parameter from.
//   sKey ...... The Key to search for.
//----------------------------------------------------------------------------------
bool ExtractValuelessParam(GUIString& sCmdLine, const GUIString& sKey)
{
	bool bMatch = false;

	// Create lower case strings for searching.
	GUIString sLowerCmdLine = sCmdLine;
	sLowerCmdLine.toLowerCase();

	GUIString sLowerKey = sKey;
	sLowerKey.toLowerCase();

	// Do we have a match (if no key, assume the front)?
	int nMatch = 0;
	if (sLowerKey != "")
		nMatch = sLowerCmdLine.find(sLowerKey);

	if (nMatch != -1)
	{
		bMatch = true;

		// Make sure we are starting a parameter
		if (! (nMatch == 0 || 
			   sLowerCmdLine.at(nMatch - 1) == ' ' ||
			   sLowerCmdLine.at(nMatch - 1) == '\t' || 
			   sLowerCmdLine.at(nMatch - 1) == '\n' ||
			   sLowerCmdLine.at(nMatch - 1) == '\r'))
		{
			// Advance past this 'false hit' and try again.
			GUIString sLeader = sCmdLine;
			GUIString sTrailer = sCmdLine;
			nMatch += sKey.length();
			sLeader.erase(nMatch, sCmdLine.length() - nMatch);
			sTrailer.erase(0, nMatch);

			bMatch = ExtractValuelessParam(sTrailer, sKey);
			sCmdLine = sLeader;
			sCmdLine.append(sTrailer);
			return bMatch;
		}

		// Find the value in the original string.
		int nEnd = nMatch + sKey.length();

		// Remove any trailing whitespace.
		while (nEnd < sCmdLine.length() && // Prevents an MFC assertion in debug more.
			   (sCmdLine.at(nEnd) == ' ' || sCmdLine.at(nEnd) == '\t' ||
				sCmdLine.at(nEnd) == '\n' || sCmdLine.at(nEnd) == '\r'))
			++nEnd;

		// Remove the key/value pair.
		sCmdLine.erase(nMatch, nEnd - nMatch);
	}

	return bMatch;
}

//----------------------------------------------------------------------------------
// If the Paramter exists, replace it.
//----------------------------------------------------------------------------------
bool ReplaceParameter(GUIString& sCmdLine, const GUIString& sKey, const GUIString& sVal)
{
	bool bMatch = false;

	// Create lower case strings for searching.
	GUIString sLowerCmdLine = sCmdLine;
	sLowerCmdLine.toLowerCase();

	GUIString sLowerKey = sKey;
	sLowerKey.toLowerCase();

	// Do we have a match (if no key, assume the front)?
	int nMatch = 0;
	if (sLowerKey != "")
		nMatch = sLowerCmdLine.find(sLowerKey);

	if (nMatch != -1)
	{
		bMatch = true;

		// Make sure we are starting a parameter
		if (! (nMatch == 0 || 
			   sLowerCmdLine.at(nMatch - 1) == ' ' ||
			   sLowerCmdLine.at(nMatch - 1) == '\t' || 
			   sLowerCmdLine.at(nMatch - 1) == '\n' ||
			   sLowerCmdLine.at(nMatch - 1) == '\r'))
		{
			// Advance past this 'false hit' and try again.
			GUIString sLeader = sCmdLine;
			GUIString sTrailer = sCmdLine;
			nMatch += sKey.length();
			sLeader.erase(nMatch, sCmdLine.length() - nMatch);
			sTrailer.erase(0, nMatch);

			bMatch = ReplaceParameter(sTrailer, sKey, sVal);
			sCmdLine = sLeader;
			sCmdLine.append(sTrailer);
			return bMatch;
		}

		// Find the value in the original string.
		int nValue = nMatch + sKey.length();
		int nEnd = -1;

		// Skip any leading whitespace.
		while (sCmdLine.at(nValue) == ' ' || sCmdLine.at(nValue) == '\t' ||
			   sCmdLine.at(nValue) == '\n' || sCmdLine.at(nValue) == '\r')
			++nValue;

		// Are we dealing with quotes?
		if (sCmdLine.at(nValue) == '\"')
		{
			++nValue;
			nEnd = sCmdLine.find('\"', nValue);
			if (nEnd != -1)
				sCmdLine.setAt(nEnd, ' ');
		}
		else
		{
			// Find the parater value end.
			nEnd = sCmdLine.length();                // End of the string (probably way too far).
			int nTest = sCmdLine.find(' ', nValue);  // Check for interveaning space.
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
			nTest = sCmdLine.find('\t', nValue);     // Check for interveaning tab. 
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
			nTest = sCmdLine.find('\n', nValue);     // Check for interveaning new line. 
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
			nTest = sCmdLine.find('\r', nValue);     // Check for interveaning return. 
			if (nTest != -1 && nTest < nEnd)
				nEnd = nTest;
		}

		// Remove any trailing whitespace.
		while (sCmdLine.at(nEnd) == ' ' || sCmdLine.at(nEnd) == '\t' ||
			   sCmdLine.at(nEnd) == '\n' || sCmdLine.at(nEnd) == '\r')
			++nEnd;

		// Remove the key/value pair.
		sCmdLine.erase(nValue, nEnd - nValue);

		GUIString sNew = " ";
		sNew.append(sKey);
		sNew.append(" \"");
		sNew.append(sVal);
		sNew.append("\" ");
		sCmdLine.insert(nValue, sNew);
	}

	return bMatch;
}

//----------------------------------------------------------------------------------
// IsColorString: Is the supplied string an RGB string?
//----------------------------------------------------------------------------------
bool IsColorString(const GUIString& sColor)
{
	GUIString sTemp = sColor;
	bool bIsColor = true;

	// Skip leading white space.
	while (sTemp.at(0) != 0 && (sTemp.at(0) < '0' || sTemp.at(0) > '9'))
		sTemp.erase(0, 1);

	if (! sTemp.length())
		bIsColor = false;

	// Consume red value and skip separating whitespace.
	while (sTemp.at(0) >= '0' && sTemp.at(0) <= '9')
		sTemp.erase(0, 1);
	while (sTemp.at(0) != 0 && (sTemp.at(0) < '0' || sTemp.at(0) > '9'))
		sTemp.erase(0, 1);

	if (! sTemp.length())
		bIsColor = false;

	// Consume green value and skip separating whitespace.
	while (sTemp.at(0) >= '0' && sTemp.at(0) <= '9')
		sTemp.erase(0, 1);
	while (sTemp.at(0) != 0 && (sTemp.at(0) < '0' || sTemp.at(0) > '9'))
		sTemp.erase(0, 1);

	if (! sTemp.length())
		bIsColor = false;

	return bIsColor;
}

//----------------------------------------------------------------------------------
// StringToRGB: Convert a string to a RGB color value.
//----------------------------------------------------------------------------------
COLORREF StringToRGB(const GUIString& sColor)
{
	GUIString sTemp = sColor;

	// Skip leading white space.
	while (sTemp.at(0) != 0 && (sTemp.at(0) < '0' || sTemp.at(0) > '9'))
		sTemp.erase(0, 1);

	// Extract the Red value.
	int nRed = atoi(std::string(sTemp).c_str());

	// Consume red value and skip separating whitespace.
	while (sTemp.at(0) >= '0' && sTemp.at(0) <= '9')
		sTemp.erase(0, 1);
	while (sTemp.at(0) != 0 && (sTemp.at(0) < '0' || sTemp.at(0) > '9'))
		sTemp.erase(0, 1);

	// Extract the Green value.
	int nGreen = atoi(std::string(sTemp).c_str());

	// Consume green value and skip separating whitespace.
	while (sTemp.at(0) >= '0' && sTemp.at(0) <= '9')
		sTemp.erase(0, 1);
	while (sTemp.at(0) != 0 && (sTemp.at(0) < '0' || sTemp.at(0) > '9'))
		sTemp.erase(0, 1);

	// Extract the Blue value.
	int nBlue = atoi(std::string(sTemp).c_str());

//	return RGB(nRed, nGreen, nBlue); // Not compatible w/ the GUI.
	return nRed << 16 | nGreen << 8 | nBlue;
}

//----------------------------------------------------------------------------------
// IsVersion: Is the supplied string a valid version?  Note: sNoVersion is 
// not considered to be valid.
//----------------------------------------------------------------------------------
bool IsVersion(const std::string& sVer)
{
	return sVer.length() && sVer != sNoVersion;
}

//----------------------------------------------------------------------------------
// Compares two version strings. The return value is negative if the first 
// version is less than the second, and posative if it is greater.  A return
// of zero denotes identical versions.  The value of the return indicates 
// where the difference occured.
//----------------------------------------------------------------------------------
int CompareVersions(const std::string& sVer1, const std::string& sVer2)
{
	std::string sVersion1 = sVer1;
	std::string sVersion2 = sVer2;

	int nVer1First = 0, nVer1Secondary = 0, nVer1Third = 0, nVer1Fourth = 0;
	int nVer2First = 0, nVer2Secondary = 0, nVer2Third = 0, nVer2Fourth = 0;

	// Extract the version numbers.
	nVer1First = atoi(sVersion1.c_str());
	int nDot = sVersion1.find('.');
	if (nDot != -1)
	{
		sVersion1.erase(0, nDot + 1);
		nVer1Secondary = atoi(sVersion1.c_str());
		nDot = sVersion1.find('.');
		if (nDot != -1)
		{
			sVersion1.erase(0, nDot + 1);
			nVer1Third = atoi(sVersion1.c_str());
			nDot = sVersion1.find('.');
			if (nDot != -1)
			{
				sVersion1.erase(0, nDot + 1);
				nVer1Fourth = atoi(sVersion1.c_str());
			}
		}
	}

	nVer2First = atoi(sVersion2.c_str());
	nDot = sVersion2.find('.');
	if (nDot != -1)
	{
		sVersion2.erase(0, nDot + 1);
		nVer2Secondary = atoi(sVersion2.c_str());
		nDot = sVersion2.find('.');
		if (nDot != -1)
		{
			sVersion2.erase(0, nDot + 1);
			nVer2Third = atoi(sVersion2.c_str());
			nDot = sVersion2.find('.');
			if (nDot != -1)
			{
				sVersion2.erase(0, nDot + 1);
				nVer2Fourth = atoi(sVersion2.c_str());
			}
		}
	}

	// Now compare the numbers to see which version is larger.
	if (nVer1First < nVer2First)
		return -1;
	else if (nVer1First > nVer2First)
		return 1;
	else if (nVer1Secondary < nVer2Secondary)
		return -2;
	else if (nVer1Secondary > nVer2Secondary)
		return 2;
	else if (nVer1Third < nVer2Third)
		return -3;
	else if (nVer1Third > nVer2Third)
		return 3;
	else if (nVer1Fourth < nVer2Fourth)
		return -4;
	else if (nVer1Fourth > nVer2Fourth)
		return 4;
	else
		return 0;
}

//----------------------------------------------------------------------------------
// This routine gets the version information from the file specified in 
// sFile and returns it in the passed parameter pVersion.
//
// Parameters:
//   sFile ...... Source file to retreive version information.
//
// Returns:
//   TRUE if version data was found, FALSE otherwise.
//----------------------------------------------------------------------------------
std::string GetExeVersion(const std::string& sFile)
{     
	DWORD       nVersionInfoSize, nParam;
	bool        bVersion = false;
	LPVOID      pVersionInfo = NULL;
	LPVOID      pVersionNum = NULL;
	std::string sVersion = sNoVersion;

	nVersionInfoSize = GetFileVersionInfoSize((LPTSTR)(sFile.c_str()), &nParam);

	if (nVersionInfoSize > 0)
	{
		UINT iParam;
		pVersionInfo = new BYTE[nVersionInfoSize];
		bVersion = GetFileVersionInfo((LPTSTR)(sFile.c_str()), nParam, nVersionInfoSize, pVersionInfo) != FALSE;

		if (bVersion)
		{
			bVersion = VerQueryValue(pVersionInfo, TEXT("\\"), &pVersionNum, &iParam) != FALSE;
			if (bVersion)
			{
				TCHAR sTemp[64];
				wsprintf(sTemp, TEXT("%d.%d.%d.%d"),
						 HIWORD(((VS_FIXEDFILEINFO *)pVersionNum)->dwFileVersionMS),  //lint !e1924
						 LOWORD(((VS_FIXEDFILEINFO *)pVersionNum)->dwFileVersionMS),  //lint !e1924
						 HIWORD(((VS_FIXEDFILEINFO *)pVersionNum)->dwFileVersionLS),  //lint !e1924
						 LOWORD(((VS_FIXEDFILEINFO *)pVersionNum)->dwFileVersionLS)); //lint !e1924
				sVersion = sTemp;
			}
		}
	}

	delete [] pVersionInfo;
	return sVersion;
}

//----------------------------------------------------------------------------------
// This routine gets the version information from this application and 
// returns it in the passed parameter pVersion.
//
// Parameters:
//   pVersion ... Version of file searched. (i.e. 1.2.3.4).
//
// Returns:
//   TRUE if version data was found, FALSE otherwise.
//----------------------------------------------------------------------------------
std::string GetExeVersion(void)
{
	char sPath[MAX_PATH];
	GetModuleFileName(NULL, sPath, MAX_PATH);
	std::string sFile = sPath;

	return GetExeVersion(sFile);
}

