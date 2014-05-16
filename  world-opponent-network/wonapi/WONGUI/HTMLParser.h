#ifndef __WON_HTMLParser_H__
#define __WON_HTMLParser_H__

#include "WONCommon/FileReader.h"
#include "WONCommon/StringUtil.h"
#include "WONGUI/GUIString.h"
#include "HTMLDocument.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTMLParser
{
protected:
	FileReader mFileReader;
	GUIString mBackStr;
	int mLineNum;

	HTMLTagAttributes mTagAttributes;
	HTMLDocumentPtr mDocument;
	HTMLTagList mTagList;

	typedef HTMLTag*(*TagFactoryFunc)(HTMLTagType theTagType);
	struct TagLookupInfo
	{
		HTMLTagType mType;
		TagFactoryFunc mFactoryFunc;

		TagLookupInfo() : mFactoryFunc(NULL), mType(HTMLTagType_None) { }
		TagLookupInfo(TagFactoryFunc theFunc, HTMLTagType theType) : mFactoryFunc(theFunc), mType(theType) { }
	};
	typedef std::map<GUIString,TagLookupInfo,GUIStringLessNoCase> TagNameMap;
	static TagNameMap& GetTagNameMap();
	TagNameMap &mTagNameMap;

	void CloseFile();
	inline bool EndOfFile();
	inline int GetChar();
	inline int PeekChar();
	inline void UngetChar(int theChar);	
	inline void UngetString(const GUIString &theStr);
	inline void UngetString(const GUIString &theStr, int theStartPos);
	inline bool SkipWhitespace();
	int ReadEntity();
	bool ReadWord(GUIString &theWord, const char *theStopChars = NULL, bool stopAtSpace = true, bool skip = false, bool erase = true);
	bool GetActualTag();
	bool GetNextTag();
	void SkipTag(const GUIString &theName);

	bool Parse();

public:
	HTMLParser();

	bool ParseFromFile(const GUIString &theFileName);
	bool ParseFromText(const GUIString &theText);

	void SetDocument(HTMLDocument *theDocument) { mDocument = theDocument; }
	HTMLDocument* GetDocument() { return mDocument; }
};

}; // namespace WONAPI

#endif