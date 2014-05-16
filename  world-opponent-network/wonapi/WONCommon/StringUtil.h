#ifndef __WON_STRINGUTIL_H__
#define __WON_STRINGUTIL_H__
#include "WONShared.h"

#include "Platform.h"
#include <string>

namespace WONAPI
{

std::wstring StringToWString(const std::string& theStr);
std::string WStringToString(const std::wstring& theStr);
std::string StringToUpperCase(const std::string& theStr);
std::string StringToLowerCase(const std::string& theStr);
std::wstring WStringToUpperCase(const std::wstring& theStr);
std::wstring WStringToLowerCase(const std::wstring& theStr);

enum MultiByteCodePage
{
	MultiByteCodePage_CLocale = -1,
	MultiByteCodePage_WindowsLocale = -2
};

std::string WStringToMultiByte(const std::wstring &theWStr, int theCodePage = MultiByteCodePage_CLocale);
std::wstring MultiByteToWString(const std::string &theStr, int theCodePage = MultiByteCodePage_CLocale);
void RecalcMultiByteCodePage(enum MultiByteCodePage theType);
void StringToUpperCaseInPlace(std::string &theStr);
void StringToLowerCaseInPlace(std::string &theStr);
void WStringToUpperCaseInPlace(std::wstring &theStr);
void WStringToLowerCaseInPlace(std::wstring &theStr);

int StringCompareNoCase(const std::string &s1, const std::string &s2);
int WStringCompareNoCase(const std::wstring &s1, const std::wstring &s2);

const std::string& GetEmptyStr();
const std::wstring& GetEmptyWStr();

inline const wchar_t* WStringGetCStr(const std::wstring &theStr)
{
#ifdef _LINUX // this is a work around for a bug in some versions of basic_string
	if(theStr.length()==0)
		return L"";
	else 
	{
		if(theStr.capacity() < theStr.length() + 1)
			((std::wstring&)theStr).reserve(theStr.length() + 1);

		((std::wstring&)theStr)[theStr.length()] = L'\0';
		return theStr.data();
	}
#else
	return theStr.c_str();
#endif
}

class StringLessNoCase
{
public:
	bool operator()(const std::string &s1, const std::string &s2) const { return StringCompareNoCase(s1,s2)<0; }
};

class WStringLessNoCase
{
public:
	bool operator()(const std::wstring &s1, const std::wstring &s2) const { return WStringCompareNoCase(s1,s2)<0; }
};


class MultiString
{
private:
	std::string mAsciiStr;
	std::wstring mUnicodeStr;

public:
	MultiString(const std::string &theStr) : mAsciiStr(theStr) {}
	MultiString(const std::wstring &theStr) : mUnicodeStr(theStr) {}

	const std::string& GetAscii();
	const std::wstring& GetUnicode();

	operator const std::string&() { return mAsciiStr; }
	operator const std::wstring&() { return mUnicodeStr; }

};

// These is functions have been known to crash if c is bigger than 256
// Test case:  Try isspace(7774)
inline int safe_isspace(unsigned long c) { if(c<256) return isspace(c); else return 0; }
inline int safe_isalpha(unsigned long c) { if(c<256) return isalpha(c); else return 0; }
inline int safe_isalnum(unsigned long c) { if(c<256) return isalnum(c); else return 0; }
inline int safe_isdigit(unsigned long c) { if(c<256) return isdigit(c); else return 0; }
inline int safe_isprint(unsigned long c) { if(c<256) return isprint(c); else return 0; }
inline int safe_ispunct(unsigned long c) { if(c<256) return ispunct(c); else return 0; }
inline int safe_isxdigit(unsigned long c) { if(c<256) return isxdigit(c); else return 0; }

}; // namespace WONAPI

#endif
