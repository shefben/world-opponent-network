#include "ConfigParser.h"
#include "WONCommon/LittleEndian.h"

using namespace std;
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ConfigParser::mLineCount = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ConfigParser::ConfigParser()
{
//	mFile = NULL;
	mLineNum = 0;
	mParent = NULL;
	mStopped = false;
	mWarningsAsErrors = false;
	mIgnorePathOnIncludeFile = false;
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ConfigParser::~ConfigParser()
{
	CloseFile();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::SetParent(ConfigParser *theParent)
{
	mParent = theParent;
	if(mParent!=NULL)
	{
		mIgnorePathOnIncludeFile = theParent->GetIgnorePathOnIncludeFile();
		mWarningsAsErrors = theParent->mWarningsAsErrors;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::AbortRead(const string &theReason, bool append)
{
	if(mAbortReason.empty() || append)
	{
		char aBuf[1024];
		sprintf(aBuf," (%s:%d)",mFileNameAndPath.c_str(),mLineNum);
		mAbortReason +=  theReason + aBuf;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::AddWarning(const std::string &theWarning)
{
	if(IsAborted()) // no point adding warnings if alread aborted
		return;

	if(mWarningsAsErrors)
		AbortRead(theWarning);

	char aBuf[1024];
	sprintf(aBuf,"Warning (%s: %d) - %s\n",mFileNameAndPath.c_str(),mLineNum,theWarning.c_str());
	mWarnings += aBuf;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::CopyWarnings(ConfigParser &theCopyFrom)
{
	mWarnings += theCopyFrom.GetWarnings();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& ConfigParser::GetAbortReason()
{
	if(mAbortReason.empty())
		AbortRead("Parse aborted.");

	return mAbortReason;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::Reset(void)
{
//	mDefineMap.clear();
	mDefineStack.clear();
	mAbortReason.erase();
	mWarnings.erase();
	mBackStr.erase();

	mLineNum = 1;
	mStopped = false;
//	if(mFile)
//		rewind(mFile);
	mFileReader.Rewind();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::PushDefineStack()
{
	if(mParent!=NULL)
		mParent->PushDefineStack();
	else
		mDefineStack.push_back(NULL);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::PopDefineStack()
{
	if(mParent!=NULL)
		mParent->PopDefineStack();
	else if(!mDefineStack.empty())
		mDefineStack.pop_back();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::AddDefine(const std::string &theName, const std::string &theVal, bool redefine)
{
	if(mParent!=NULL)
		return mParent->AddDefine(theName,theVal,redefine);

	if(theName.find_first_of(' ')!=string::npos)
	{
		AddWarning("Not allowed to have spaces in define names.");
		return true;
	}

	if(mDefineStack.empty())
		mDefineStack.push_back(new DefineMapContainer);

	if(mDefineStack.back().get()==NULL)
		mDefineStack.back() = new DefineMapContainer;

	DefineMap &aMap = mDefineStack.back()->mDefineMap;
	if(redefine)
		aMap[theName] = theVal;
	else if(aMap.insert(DefineMap::value_type(theName,theVal)).second==false)
	{
		AddWarning("Redefintion of " + theName);
		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::AddDefine(const std::string &theName, const std::wstring &theVal, bool redefine)
{
	if(mParent!=NULL)
		return mParent->AddDefine(theName,theVal,redefine);

	if(theName.find_first_of(' ')!=string::npos)
	{
		AddWarning("Not allowed to have spaces in define names.");
		return true;
	}

	if(mDefineStack.empty())
		mDefineStack.push_back(new DefineMapContainer);

	if(mDefineStack.back().get()==NULL)
		mDefineStack.back() = new DefineMapContainer;

	DefineMap &aMap = mDefineStack.back()->mDefineMap;
	if(redefine)
		aMap[theName] = theVal;
	else if(aMap.insert(DefineMap::value_type(theName,theVal)).second==false)
	{
		AddWarning("Redefintion of " + theName);
		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::GetDefine(const std::string &theName, std::string &theValR)
{
	if(mParent!=NULL)
		return mParent->GetDefine(theName,theValR);

	DefineStack::reverse_iterator aStackItr = mDefineStack.rbegin();
	while(aStackItr!=mDefineStack.rend())
	{
		if(aStackItr->get()!=NULL)
		{
			DefineMap &aMap = (*aStackItr)->mDefineMap;
			DefineMap::iterator anItr = aMap.find(theName);
			if(anItr!=aMap.end())
			{
				DefineStruct &aStruct = anItr->second;
				if(!aStruct.mIsWide)
					theValR = aStruct.mStr;
				else
					theValR = WStringToString(aStruct.mWStr);

				return true;
			}
		}
		++aStackItr;
	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::GetDefine(const std::string &theName, std::wstring &theValR)
{
	if(mParent!=NULL)
		return mParent->GetDefine(theName,theValR);

	DefineStack::reverse_iterator aStackItr = mDefineStack.rbegin();
	while(aStackItr!=mDefineStack.rend())
	{
		if(aStackItr->get()!=NULL)
		{
			DefineMap &aMap = (*aStackItr)->mDefineMap;
			DefineMap::iterator anItr = aMap.find(theName);
			if(anItr!=aMap.end())
			{
				DefineStruct &aStruct = anItr->second;
				if(aStruct.mIsWide)
					theValR = aStruct.mWStr;
				else
					theValR = StringToWString(aStruct.mStr);

				return true;
			}
		}
		++aStackItr;
	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::AddGroupDefine(const std::string &theName, ConfigParserState *theState)
{
	if(theName.find_first_of(' ')!=string::npos)
	{
		AddWarning("Not allowed to have spaces in define names.");
		return true;
	}

	ConfigParserState aState;
	if(theState==NULL)
	{
		GetState(aState);
		theState = &aState;
	}

	if(mParent!=NULL)
		return mParent->AddGroupDefine(theName,theState);

	if(mDefineStack.empty())
		mDefineStack.push_back(new DefineMapContainer);

	if(mDefineStack.back().get()==NULL)
		mDefineStack.back() = new DefineMapContainer;

	GroupDefineMap &aMap = mDefineStack.back()->mGroupDefineMap;
	if(aMap.insert(GroupDefineMap::value_type(theName,*theState)).second==false)
	{
		AddWarning("Redefintion of " + theName);
		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::GetGroupDefine(const std::string &theName, ConfigParserState &theState)
{
	if(mParent!=NULL)
		return mParent->GetGroupDefine(theName,theState);

	DefineStack::reverse_iterator aStackItr = mDefineStack.rbegin();
	while(aStackItr!=mDefineStack.rend())
	{
		if(aStackItr->get()!=NULL)
		{
			GroupDefineMap &aMap = (*aStackItr)->mGroupDefineMap;
			GroupDefineMap::iterator anItr = aMap.find(theName);
			if(anItr!=aMap.end())
			{
				theState = anItr->second;
				return true;
			}
		}
		++aStackItr;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::EndOfFile()
{	
//	if(!feof(mFile)) 
//		return false; 
	if(!mFileReader.EndOfFile())
		return false;
	else return 
		mBackStr.empty(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ConfigParser::GetChar()
{
	int aChar = 0;
	if(mBackStr.empty())
	{
		if(mIsUnicode)
		{
			if(IsLittleEndian())
			{
				aChar = mFileReader.TryReadByte();
				aChar |= mFileReader.TryReadByte()<<8;
			}
			else
			{
				aChar = mFileReader.TryReadByte()<<8;
				aChar |= mFileReader.TryReadByte();
			}
		}
		else
			aChar = mFileReader.TryReadByte();
	}
	else
	{
		aChar = mBackStr[mBackStr.length()-1];
		mBackStr.resize(mBackStr.length()-1);
	}

	if(aChar=='\r') // check for '\r' which can happen in unicode mode (or on Mactintosh maybe?)
	{
		int anotherChar = GetChar();
		if(anotherChar!='\n') // just got a '\r' --> this is also considered a carriage return
		{
			UngetChar(anotherChar);
			mLineCount++;
			mLineNum++;
		}
			
		return '\n';
	}

	if(aChar=='\n')
	{
		mLineCount++;
		mLineNum++;
	}

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ConfigParser::PeekChar()
{
//	if(mFile && !EndOfFile())
	if(mFileReader.IsOpen() && !EndOfFile())
	{
		int aChar = GetChar();
		UngetChar(aChar);
		return aChar;
	}
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::UngetChar(int theChar)
{ 
	if(theChar==EOF)
		return;

	if(theChar=='\n')
	{
		mLineNum--;
		mLineCount--;
	}

	mBackStr+=(wchar_t)theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::UngetStr(const std::string &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
		UngetChar(theStr[i]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::UngetStr(const std::wstring &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
		UngetChar(theStr[i]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::OpenFile(const std::string& theFilePath)
{
	CloseFile();
	mLineNum = 1;
	mIsUnicode = false;

	mBackStr.erase();
	mFileNameAndPath = theFilePath;

	int aPos = mFileNameAndPath.find_last_of('/');
	if(aPos == string::npos)
		aPos = mFileNameAndPath.find_last_of('\\');

	if(aPos == string::npos)
		mFilePath = "";
	else 
		mFilePath = mFileNameAndPath.substr(0, aPos+1);

	//mFile = fopen(theFilePath.c_str(), "r");
	mFileReader.Open(theFilePath.c_str());
//	if(mFile!=NULL)
	if(mFileReader.IsOpen())
	{
		int h1, h2, h3, h4;
//		h1 = fgetc(mFile);
//		h2 = fgetc(mFile);
//		h3 = fgetc(mFile);
//		h4 = fgetc(mFile);
		h1 = mFileReader.TryReadByte();
		h2 = mFileReader.TryReadByte();
		h3 = mFileReader.TryReadByte();
		h4 = mFileReader.TryReadByte();
		if(h1==0xff && h2==0xfe && h4==0)
		{
//			fclose(mFile);
//			mFile = fopen(theFilePath.c_str(),"rb");
//			fseek(mFile,2,SEEK_SET);
			mFileReader.SetPos(2);
			mIsUnicode = true;
		}
		else
			//fseek(mFile,0,SEEK_SET);
			mFileReader.Rewind();
	}
	mFileReader.ReadIntoMemory();
	//return mFile!=NULL;
	return mFileReader.IsOpen();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::CloseFile()
{
/*	if(mFile!=NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}*/
	mFileReader.Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::GetState(ConfigParserState &theState)
{
	theState.mBackStr = mBackStr;
	theState.mFileNameAndPath = mFileNameAndPath;
//	theState.mFilePos = ftell(mFile);
	theState.mFilePos = mFileReader.pos();
	theState.mLineNum = mLineNum;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::SetState(const ConfigParserState &theState)
{
	if(/*mFile==NULL*/ !mFileReader.IsOpen() || theState.mFileNameAndPath!=mFileNameAndPath)
	{
		if(!OpenFile(theState.mFileNameAndPath))
			return false;
	}

//	fseek(mFile,theState.mFilePos,SEEK_SET);
	mFileReader.SetPos(theState.mFilePos);
	mLineNum = theState.mLineNum;
	mBackStr = theState.mBackStr;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::SkipWhitespace()
{
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(!safe_isspace(aChar))
		{
			if(aChar=='#')
			{
				SkipLine();
				continue;
			}
			else if(aChar=='/')
			{
				int anotherChar = GetChar();
				if(anotherChar=='/')
				{
					SkipLine();
					continue;
				}
				else if(anotherChar=='*')
				{
					SkipBlockComment();
					continue;
				}
				else
					UngetChar(anotherChar);
			}

			UngetChar(aChar);
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::SkipLine()
{
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(aChar=='\n')
			return;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ConfigParser::SkipBlockComment()
{
	bool gotStar = false;
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(gotStar && aChar=='/')
			break;
		
		gotStar = aChar=='*';
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONCommon/PerfTimer.h"
template<class StrType, class CharType>
static bool ReadToken(ConfigParser *theParser, StrType &theToken, bool toEndLine, bool skip)
{
	if(theParser->EndOfFile())
		return false;

	theToken.erase();

	int aCurLine = theParser->GetLineNum();
	theParser->SkipWhitespace();
	if(toEndLine && aCurLine!=theParser->GetLineNum())
		return true;

	bool inQuote = false, lastWasBackslash = false, lastWasSpace = true;
	int aChar = 0;
	while(!theParser->EndOfFile())
	{
		lastWasBackslash = aChar=='\\' && !lastWasBackslash;
		aChar = theParser->GetChar();
		if(aChar==EOF)
			break;

		if(safe_isspace(aChar))
		{
			if(!inQuote)
			{
				if(!toEndLine)
				{
					theParser->UngetChar(aChar);
					break;
				}
				else if(aChar=='\n' && !lastWasBackslash)
				{
					theParser->UngetChar(aChar); // unget last '\n' in order to have correct line number on errors and warnings
					break;
				}
				else if(!skip)
					theToken += (CharType)' ';

				lastWasSpace = true;

				int aStartLine = theParser->GetLineNum();
				theParser->SkipWhitespace();
				if(aStartLine!=theParser->GetLineNum())
					break;
				else
					continue;
			}
			else if(aChar=='\n') // can't have quote go to next line.
			{
				theParser->AbortRead("Expecting end quote");
				return false;
			}
		}
		else if(aChar=='}' && !inQuote)
		{
			if(!theToken.empty() || skip)
				theParser->UngetChar(aChar);
			else if(!skip)
				theToken+=(CharType)'}';
				
			break;
		}
		else if(aChar==';' && !inQuote) // stop at semicolon
			break;
		else if(aChar=='/' && !inQuote)
		{
			int aNextChar = theParser->PeekChar();
			if(aNextChar=='*' || aNextChar=='/')
			{
				theParser->UngetChar(aChar);
				break;
			}
		}
		else if(aChar=='#' && !inQuote)
		{
			theParser->UngetChar(aChar);
			break;
		}
		else if(aChar=='"')
		{
			if(inQuote && !lastWasBackslash)
				inQuote = false;
			else if(!inQuote && lastWasSpace)
				inQuote = true;
		}
		else if(aChar=='\\')
		{
			int anotherChar = theParser->GetChar();
			if(anotherChar=='\n')
				continue;
			else
				theParser->UngetChar(anotherChar);
		}
		else if(aChar=='$')
		{
			if(!skip && (!inQuote || !lastWasBackslash))
			{
				// do substitution
				string aDefine;
				StrType aVal;
				while(!theParser->EndOfFile())
				{
					int aChar = theParser->GetChar();
					if(safe_isspace(aChar))
					{
						theParser->AbortRead("Expecting closing '$'");
						return false;
					}
					else if(aChar=='$')
					{
//						UngetChar(aChar);
						break;
					}
					aDefine += aChar;
				}
				
				if(!theParser->GetDefine(aDefine,aVal))
				{
					theParser->AbortRead("Define not found: " + aDefine);
					return false;
				}

				theToken += aVal;
				continue;
			}
		}
		
		if(!skip)
			theToken += aChar;
	}

	return skip || !theToken.empty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::ReadToken(std::wstring &theToken, bool toEndLine, bool skip)
{
	return ::ReadToken<std::wstring,wchar_t>(this,theToken,toEndLine,skip);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ConfigParser::ReadToken(std::string &theToken, bool toEndLine, bool skip)
{
	return ::ReadToken<std::string,char>(this,theToken,toEndLine,skip);

	/*
	if(EndOfFile())
		return false;

	theToken.erase();

	int aCurLine = mLineNum;
	SkipWhitespace();
	if(toEndLine && aCurLine!=mLineNum)
		return true;

	bool inQuote = false, lastWasBackslash = false, lastWasSpace = true;
	int aChar = 0;
	while(!EndOfFile())
	{
		lastWasBackslash = aChar=='\\';
		aChar = GetChar();
		if(aChar==EOF)
			break;

		if(isspace(aChar) && !inQuote)
		{
			if(!toEndLine)
			{
				UngetChar(aChar);
				break;
			}
			else if(aChar=='\n' && !lastWasBackslash)
			{
				UngetChar(aChar); // unget last '\n' in order to have correct line number on errors and warnings
				break;
			}
			else if(!skip)
				theToken += ' ';

			lastWasSpace = true;

			int aStartLine = mLineNum;
			SkipWhitespace();
			if(aStartLine!=mLineNum)
				break;
			else
				continue;
		}
		else if(aChar=='}' && !inQuote)
		{
			if(!theToken.empty())
				UngetChar(aChar);
			else if(!skip)
				theToken+='}';
				
			break;
		}
		else if(aChar=='/' && !inQuote)
		{
			int aNextChar = PeekChar();
			if(aNextChar=='*' || aNextChar=='/')
			{
				UngetChar(aChar);
				break;
			}
		}
		else if(aChar=='#' && !inQuote)
		{
			UngetChar(aChar);
			break;
		}
		else if(aChar=='"')
		{
			if(inQuote && !lastWasBackslash)
				inQuote = false;
			else if(!inQuote && lastWasSpace)
				inQuote = true;
		}
		else if(aChar=='\\')
		{
			int anotherChar = GetChar();
			if(anotherChar=='\n')
				continue;
			else
				UngetChar(anotherChar);
		}
		else if(aChar=='$')
		{
			if(!skip && (!inQuote || !lastWasBackslash))
			{
				// do substitution
				string aDefine, aVal;
				while(!EndOfFile())
				{
					int aChar = GetChar();
					if(aChar=='$')
					{
//						UngetChar(aChar);
						break;
					}
					aDefine += aChar;
				}
				
				if(!GetDefine(aDefine,aVal))
				{
					AbortRead("Define not found: " + aDefine);
					return false;
				}

				theToken += aVal;
				continue;
			}
		}
		
		if(!skip)
			theToken += aChar;
	}

	return skip || !theToken.empty();*/
}

