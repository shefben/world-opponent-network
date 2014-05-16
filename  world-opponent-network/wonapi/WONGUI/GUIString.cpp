#include "GUIString.h"
#include "WONCommon/StringUtil.h"

using namespace WONAPI;
using namespace std;

const GUIString GUIString::EMPTY_STR;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::Init()
{
	mLength = mCapacity = 0;
	mData = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(unsigned short theCapacity)
{
	Init();
	resize(theCapacity);
	resize(0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(GUIStringData *theCopy)
{
	Init();
	resize(theCopy->mLength);
	memcpy(mData,theCopy->mData,sizeof(CharData)*(mLength+1));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(GUIStringData *theCopy, unsigned short theOffset, unsigned short theLength)
{
	Init();
	resize(theLength);
	memcpy(mData,theCopy->mData + theOffset, sizeof(CharData)*theLength);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(const char *theStr)
{
	Init();
	AssignStr(theStr,strlen(theStr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(const wchar_t *theStr)
{
	Init();
	AssignWStr(theStr,wcslen(theStr));
}

//////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(const char *theStr, unsigned short theLen)
{
	Init();
	AssignStr(theStr,theLen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(const wchar_t *theStr, unsigned short theLen)
{
	Init();
	AssignWStr(theStr,theLen);
}
/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(const std::string &theStr)
{
	Init();
	AssignStr(theStr.c_str(),theStr.length());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::GUIStringData(const std::wstring &theStr)
{
	Init();
	AssignWStr(WStringGetCStr(theStr),theStr.length());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData::~GUIStringData()
{
	delete [] mData;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::toUpperCase() 
{
	int aLength = mLength;
	for(int i=0; i<aLength; i++)
	{
		if(mData[i]<256)
			mData[i] = toupper(mData[i]);
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::toLowerCase()
{
	int aLength = mLength;
	for(int i=0; i<aLength; i++)
	{
		if(mData[i]<256)
			mData[i] = tolower(mData[i]);
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::AssignStr(const char *theStr, int theLen)
{
	resize(theLen);
	for(int i=0; i<theLen; i++)
		mData[i] = (unsigned char)theStr[i];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::AssignWStr(const wchar_t *theStr, int theLen)
{
	resize(theLen);
	mLength = theLen;
	for(int i=0; i<theLen; i++)
		mData[i] = theStr[i];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::Fill(int theChar, unsigned short theOffset, unsigned short theLength)
{
	int aLength = theOffset+theLength;
	resize(aLength);
	for(int i=theOffset; i<aLength; i++)
		mData[i] = theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::erase(int theStartChar, int theLength)
{
//	wstring aStr = GetAsWString();
//	aStr.erase(theStartChar,theLength);
//	AssignWStr(aStr.c_str(),aStr.length());
//	return;
	
	if (theStartChar>mLength || theStartChar < 0)
		return;

	int aEndChar = theStartChar+theLength;
	if(theLength==0 || aEndChar>mLength)
		aEndChar = mLength;

	if(aEndChar!=mLength)
		memmove(mData+theStartChar,mData+aEndChar,sizeof(CharData)*(mLength-aEndChar));
	resize(mLength-(aEndChar-theStartChar));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::insert(int theStartChar, const GUIStringData *theStr)
{
//	wstring aStr1 = GetAsWString();
//	wstring aStr2 = theStr->GetAsWString();
//	aStr1.insert(theStartChar,aStr2);
//	AssignWStr(aStr1.c_str(),aStr1.length());
//	return;

	if(theStartChar>mLength)
		return;

	int aOldLen = mLength;

	resize(mLength+theStr->mLength);
	if(theStartChar!=aOldLen)
		memmove(mData+theStartChar+theStr->mLength,mData+theStartChar,sizeof(CharData)*(aOldLen-theStartChar));
	memcpy(mData+theStartChar,theStr->mData,sizeof(CharData)*theStr->mLength);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GUIStringData::assign(const GUIStringData *theStr, unsigned short theOffset, unsigned short theLength)
{
	reserve(theLength);
	resize(0);
	append(theStr,theOffset,theLength);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string GUIStringData::GetAsString() const
{
#ifdef GUISTRING_ASCII
	return string((char*)mData,mLength);
#else
	string aStr;
	aStr.reserve(mLength);
	for(int i=0; i<mLength; i++)
	{
		unsigned short aWide = mData[i];
		if(aWide<256)
			aStr += (char)aWide;
		else
			aStr += '?';
	}

	return aStr;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::wstring GUIStringData::GetAsWString() const
{
#if defined(GUISTRING_ASCII) || defined(_LINUX) 
	wstring aStr;
	aStr.reserve(mLength);
	for(int i=0; i<mLength; i++)
		aStr += (wchar_t)mData[i];

	return aStr;
#else 
	return wstring(mData,mLength);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::setAt(int thePos, int theChar)
{
	if (thePos < mLength)
		mData[thePos] = (unsigned short)theChar;
	return theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline int MyMin(int a, int b)
{
	return a<b?a:b;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::compare(const GUIStringData *theStr) const
{
	int aMinLen = MyMin(length(),theStr->length());
	int i=0;
	while(i<aMinLen)
	{
		if(mData[i]!=theStr->mData[i])
			return mData[i]-theStr->mData[i];

		i++;
	}

	return (length()==theStr->length()) ? 0 : (length() < theStr->length()) ? -1 : 1;		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::compareNoCase(const GUIStringData *theStr) const
{
	int aMinLen = length();
	if(theStr->length()<aMinLen)
		aMinLen = theStr->length();

	const CharData *p1 = mData;
	const CharData *p2 = theStr->mData;
	for(int i=0; i<aMinLen; i++)
	{
		int aChar1 = towupper(*p1);
		int aChar2 = towupper(*p2);
		if(aChar1!=aChar2)
			return aChar1-aChar2;

		p1++; p2++;
	}

	return (length()==theStr->length()) ? 0 : (length() < theStr->length()) ? -1 : 1;		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::compare(const char *theStr, int theOffset) const
{
	const CharData *p1 = mData + theOffset;
	const unsigned char *p2 = (const unsigned char*)theStr;
	int aLength = mLength - theOffset;
	for(int i=0; i<aLength; i++)
	{
		int c1 = *p1;
		int c2 = *p2;
		if(c2==0)
			return 1; // this is longer than theStr
		if(c1!=c2)
			return c1-c2;

		p1++; p2++;
	}
	return *p2=='0'?0:-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::compareNoCase(const char *theStr, int theOffset) const
{
	const CharData *p1 = mData + theOffset;
	const unsigned char *p2 = (const unsigned char*)theStr;
	int aLength = mLength - theOffset;
	for(int i=0; i<aLength; i++)
	{
		int c1 = towupper(*p1);
		int c2 = towupper(*p2);
		if(c2==0)
			return 1; // this is longer than theStr
		if(c1!=c2)
			return c1-c2;

		p1++; p2++;
	}
	return (*p2=='\0')?0:-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::atoi(unsigned short theOffset) const
{
	int aLength = length();
	int aNum = 0;
	while(isspace(mData[theOffset])) // skip whitespace
		theOffset++;

	bool negative;
	if(mData[theOffset]=='-')
	{
		negative = true;
		theOffset++;
	}
	else 
		negative = false;

	while(true)
	{
		int aDigit = mData[theOffset++] - '0';
		if(aDigit<0 || aDigit>9)
			break;

		aNum=aNum*10 + aDigit;
	}

	return negative?(aNum*-1):aNum;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::find(int theChar, int theStartPos) const
{
	int aLength = length();
	for(int i=theStartPos; i<aLength; i++)
	{
		if(mData[i]==theChar)
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::find(const GUIStringData *theStr, int theStartPos) const
{
	int p1,p2;
	int cp = theStartPos;
	const int len1 = length();
	const int len2 = theStr->length();
	const CharData *data2 = theStr->mData;
	while(cp < len1)
	{
		p1 = cp;
		p2 = 0;
		while(p1<len1 && p2<len2)
		{
			if(mData[p1]!=data2[p2])
				break;

			p1++; p2++;
		}
		if(p2==len2)
			return p1-len2;

		cp++;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::findNoCase(const GUIStringData *theStr, int theStartPos) const
{
	int p1,p2;
	int cp = theStartPos;
	const int len1 = length();
	const int len2 = theStr->length();
	const CharData *data2 = theStr->mData;
	while(cp < len1)
	{
		p1 = cp;
		p2 = 0;
		while(p1<len1 && p2<len2)
		{
			if(towupper(mData[p1])!=towupper(data2[p2]))
				break;

			p1++; p2++;
		}
		if(p2==len2)
			return p1-len2;

		cp++;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::findOneOf(const GUIStringData *theStr, int theStartPos) const
{
	int aFirstFound = -1;

	for (int i=0; i<theStr->mLength; i++)
	{
		int aFound = find(theStr->mData[i], theStartPos);
		if (aFound!=-1)
			if (aFirstFound==-1 || aFound<aFirstFound)
				aFirstFound = aFound;
	}

	return aFirstFound;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::rFind(int theChar, int theStartPos) const
{
	if (theStartPos == -1)
		theStartPos = length() - 1;

	for(int i=theStartPos; i>=0; i--)
	{
		if(mData[i]==theChar)
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int GUIStringData::rFind(const GUIStringData* theStr, int theStartPos) const
{
	std::wstring sSrc = GetAsWString();
	std::wstring sFind = theStr->GetAsWString();

	return sSrc.rfind(sFind, theStartPos);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIStringData* GUIString::GetNullStr()
{
	static GUIStringDataPtr aNullStr = new GUIStringData(L"");
	return aNullStr.get();
}



