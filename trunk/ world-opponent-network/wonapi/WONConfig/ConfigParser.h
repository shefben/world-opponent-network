#ifndef __WON_CONFIGPARSER_H__
#define __WON_CONFIGPARSER_H__

#include "WONCommon/StringUtil.h"
#include "WONCommon/FileReader.h"
#include <map>
#include <vector>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct ConfigParserState
{
	long mFilePos;
	std::string mFileNameAndPath;
	int mLineNum;

	std::wstring mBackStr;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ConfigParser
{
private:
	ConfigParser(const ConfigParser& theCopy);
	ConfigParser& operator=(const ConfigParser& theAssign);

protected:
	struct DefineStruct
	{
		bool mIsWide;
		std::wstring mWStr;
		std::string mStr;

		DefineStruct() : mIsWide(false) { }
		DefineStruct(const std::string &theStr) : mStr(theStr), mIsWide(false) { }
		DefineStruct(const std::wstring &theStr) : mWStr(theStr), mIsWide(true) { }
	};


	typedef std::map<std::string,DefineStruct,WONAPI::StringLessNoCase> DefineMap;
	typedef std::map<std::string,ConfigParserState,StringLessNoCase> GroupDefineMap;
	struct DefineMapContainer : public RefCount
	{
		DefineMap mDefineMap;
		GroupDefineMap mGroupDefineMap;
	};
	typedef SmartPtr<DefineMapContainer> DefineMapContainerPtr; 

	typedef std::vector<DefineMapContainerPtr> DefineStack;
	DefineStack mDefineStack;

//	DefineMap mDefineMap;
//	GroupDefineMap mGroupDefineMap;
	
	ConfigParser *mParent;

//	FILE* mFile;
	FileReader mFileReader;
	std::wstring mBackStr;
	std::string mAbortReason;
	std::string mWarnings;
	bool mWarningsAsErrors;
	bool mIsUnicode;
	int mLineNum;
	static int mLineCount;

	std::string mFilePath;
	std::string mFileNameAndPath;

	bool mIgnorePathOnIncludeFile;
	bool mStopped;


public: 
	// needed by ReadToken template function
	int GetLineNum() { return mLineNum; }
	int GetChar();
	int PeekChar();
	void UngetChar(int theChar);
	void UngetStr(const std::string &theStr);
	void UngetStr(const std::wstring &theStr);
	bool EndOfFile();

public:
	ConfigParser();
	virtual ~ConfigParser();

	void SetParent(ConfigParser *theParent);

	const std::string& GetFilePath() const { return mFilePath; }
	
	bool GetIgnorePathOnIncludeFile() { return mIgnorePathOnIncludeFile; }
	void SetIgnorePathOnIncludeFile(bool ignore) { mIgnorePathOnIncludeFile = ignore; }

	static void ResetLineCount() { mLineCount = 0; }
	static int GetLineCount() { return mLineCount; }

	void Reset(); 
	bool OpenFile(const std::string& theFilePath);
	void CloseFile();
	bool IsUnicode() { return mIsUnicode; }

	void SkipWhitespace();
	void SkipLine();
	void SkipBlockComment();

	void AbortRead(const std::string &theReason, bool append = false);
	void AddWarning(const std::string &theWarning);
	void CopyWarnings(ConfigParser &theCopyFrom);
	void SetWarningsAsErrors(bool theVal) { mWarningsAsErrors = theVal; }

	void StopRead() { mStopped = true; }
	bool IsStopped() { return mStopped; }

	bool IsAborted() const { return !mAbortReason.empty(); }
	const std::string& GetAbortReason();

	bool HasWarnings() const { return !mWarnings.empty(); }
	const std::string& GetWarnings() const { return mWarnings; } 

	void PushDefineStack();
	void PopDefineStack();
	bool AddDefine(const std::string &theName, const std::string &theVal, bool redefine = false);
	bool AddDefine(const std::string &theName, const std::wstring &theVal, bool redefine = false);
	bool GetDefine(const std::string &theName, std::string &theValR);
	bool GetDefine(const std::string &theName, std::wstring &theValR);

	bool AddGroupDefine(const std::string &theName, ConfigParserState *theState = NULL);
	bool GetGroupDefine(const std::string &theName, ConfigParserState &theState);

	bool ReadToken(std::string &theToken, bool toEndLine = false, bool skip = false);
	bool ReadToken(std::wstring &theToken, bool toEndLine = false, bool skip = false);

	void GetState(ConfigParserState &theState);
	bool SetState(const ConfigParserState &theState);
};

}; // namespace WONAPI


#endif
