#include "HTMLParser.h"
#include "MSControls.h"
#include "SimpleComponent.h"
#include "ImageButton.h"
#include "GIFDecoder.h"
#include "Window.h"
#include "ImageFilter.h"

using namespace WONAPI;
using namespace std;

typedef std::map<GUIString,ImageMapPtr,GUIStringLessNoCase> ImageMapMap;
static ImageMapMap mImageMapMap;

struct LoadedImageStruct
{
	RawImagePtr mRawImage; // for scaling
	NativeImagePtr mImage;
	AnimationPtr mAnimation;

	LoadedImageStruct(NativeImage *theImage = NULL, Animation *theAnimation = NULL) :
		mImage(theImage), mAnimation(theAnimation) { }
};
typedef std::map<GUIString,LoadedImageStruct,GUIStringLessNoCase> LoadedImageMap;
LoadedImageMap mLoadedImageMap;
LoadedImageMap mScaledImageMap;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLParser::TagNameMap& HTMLParser::GetTagNameMap()
{
	static TagNameMap mTagNameMap;
	if(mTagNameMap.empty())
	{
		mTagNameMap["A"] = TagType_Anchor;
		mTagNameMap["P"] = TagType_Paragraph;
		mTagNameMap["DIV"] = TagType_Div;
		mTagNameMap["BR"] = TagType_LineBreak;
		mTagNameMap["HR"] = TagType_HorizontalRule;
		mTagNameMap["B"] = TagType_Bold;
		mTagNameMap["STRONG"] = TagType_Bold;
		mTagNameMap["I"] = TagType_Italic;
		mTagNameMap["EM"] = TagType_Italic;
		mTagNameMap["U"] = TagType_Underline;
		mTagNameMap["H1"] = TagType_H1;
		mTagNameMap["H2"] = TagType_H2;
		mTagNameMap["H3"] = TagType_H3;
		mTagNameMap["H4"] = TagType_H4;
		mTagNameMap["H5"] = TagType_H5;
		mTagNameMap["H6"] = TagType_H6;
		mTagNameMap["CODE"] = TagType_Code;
		mTagNameMap["TT"] = TagType_Code;
		mTagNameMap["KBD"] = TagType_Code;
		mTagNameMap["SAMP"] = TagType_Code;
		mTagNameMap["LI"] = TagType_ListItem;
		mTagNameMap["UL"] = TagType_UnorderedList;
		mTagNameMap["OL"] = TagType_OrderedList;
		mTagNameMap["IMG"] = TagType_Image;
		mTagNameMap["PRE"] = TagType_Pre;
		mTagNameMap["BODY"] = TagType_Body;
		mTagNameMap["FONT"] = TagType_Font;
		mTagNameMap["BASEFONT"] = TagType_BaseFont;
		mTagNameMap["CENTER"] = TagType_Center;
		mTagNameMap["TABLE"] = TagType_Table;
		mTagNameMap["TR"] = TagType_TableRow;
		mTagNameMap["TD"] = TagType_TableData;
		mTagNameMap["TH"] = TagType_TableHeader;
		mTagNameMap["SMALL"] = TagType_Small;
		mTagNameMap["BIG"] = TagType_Big;
		mTagNameMap["NOBR"] = TagType_NoBreak;
		mTagNameMap["INPUT"] = TagType_Input;
		mTagNameMap["SELECT"] = TagType_Select;
		mTagNameMap["OPTION"] = TagType_Option;
		mTagNameMap["MAP"] = TagType_ImageMap;
		mTagNameMap["AREA"] = TagType_Area;
		mTagNameMap["SCRIPT"] = TagType_Script;
		mTagNameMap["STYLE"] = TagType_Style;
	}

	return mTagNameMap;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLParser::HTMLParser(MultiLineElement *theDisplay) : mTagNameMap(GetTagNameMap())
{
	mDisplay = theDisplay;
	mFile = NULL;
	mText = NULL;
	mLineNum = 1;
	mRichTextParse = false;
	mDocumentFetcher = DefaultDocumentFetcher;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::SetDocumentFetcher(DocumentFetcher theFetcher)
{
	if(theFetcher==NULL)
		mDocumentFetcher = DefaultDocumentFetcher;
	else
		mDocumentFetcher = theFetcher;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string HTMLParser::DefaultDocumentFetcher(const std::string &thePath, const std::string &theBasePath)
{
	int aPos = theBasePath.find_last_of("\\");
	if(aPos==string::npos)
		aPos = theBasePath.find_last_of("/");

	if(aPos!=string::npos)
		return theBasePath.substr(0,aPos+1) + thePath;
	else
		return thePath;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::AddDisplaySegment(LineSegment *theSegment, int theFlags, bool clearAccum)
{
	if(theFlags&(LineSegmentFlag_PreLineBreak | LineSegmentFlag_PostLineBreak))
		mLineType = 0;

	if(clearAccum)
		ClearAccumulatedSegment();

	mHadLineBreak = mHadParagraphBreak = false;
	mDisplay->AddSegment(theSegment, theFlags);
}
	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::AddParagraphBreak(bool unconditional, int extraFlags)
{
	ClearAccumulatedSegment();
	if(unconditional || !mHadParagraphBreak)
	{
		mHadParagraphBreak = true;
		mHadLineBreak = true;
		mHadSpaceOnEnd = true;
		mDisplay->AddSegment(new LineBreakSegment(20),extraFlags | LineSegmentFlag_PreLineBreak | LineSegmentFlag_PostLineBreak);	
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::AddLineBreak(int extraFlags)
{
	ClearAccumulatedSegment();
	if(mHadLineBreak)
	{
		AddParagraphBreak(true,extraFlags);
		return;
	}

	mHadLineBreak = true;
	mHadSpaceOnEnd = true;
	mDisplay->AddSegment(new LineBreakSegment(0),extraFlags | LineSegmentFlag_PreLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::CloseFile()
{
	if(mFile!=NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}
	mText = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::EndOfFile()
{	
	if(mText)
		return *mText==0;
	if(!feof(mFile)) 
		return false; 
	else return 
		mBackStr.empty(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetWesternChar(int theChar)
{
	switch(theChar)
	{
		case 130:   return 8218;  //     Single Low-9 Quotation Mark
		case 131:   return 402;  //      Latin Small Letter F With Hook
		case 132:   return 8222;  //     Double Low-9 Quotation Mark
		case 133:   return 8230;  //     Horizontal Ellipsis
		case 134:   return 8224;  //     Dagger
		case 135:   return 8225;  //     Double Dagger
		case 136:   return 710;  //      Modifier Letter Circumflex Accent
		case 137:   return 8240;  //     Per Mille Sign
		case 138:   return 352;  //      Latin Capital Letter S With Caron
		case 139:   return 8249;  //     Single Left-Pointing Angle Quotation Mark
		case 140:   return 338;  //      Latin Capital Ligature OE
		case 145:   return 8216;  //     Left Single Quotation Mark
		case 146:   return 8217;  //     Right Single Quotation Mark
		case 147:   return 8220;  //     Left Double Quotation Mark
		case 148:   return 8221;  //     Right Double Quotation Mark
		case 149:   return 8226;  //     Bullet
		case 150:   return 8211;  //     En Dash
		case 151:   return 8212;  //     Em Dash
		case 152:   return 732;  //      Small Tilde
		case 153:   return 8482;  //     Trade Mark Sign
		case 154:   return 353;  //      Latin Small Letter S With Caron
		case 155:   return 8250;  //     Single Right-Pointing Angle Quotation Mark
		case 156:   return 339;  //      Latin Small Ligature OE
		case 159:   return 376;  //      Latin Capital Letter Y With Diaeresis
	}

	return theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetChar()
{
	int aChar = 0;
	if(mBackStr.empty())
	{
		if(mText)
		{
			aChar = *mText;
			if(aChar!=0)
				mText++;
			else
				aChar = -1;
		}
		else
			aChar = fgetc(mFile);

		if(aChar>=130 && aChar<=159)
			aChar = GetWesternChar(aChar);
	}
	else
	{
		aChar = mBackStr.at(mBackStr.length()-1);
		mBackStr.resize(mBackStr.length()-1);
	}

	if(aChar=='\n')
		mLineNum++;

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::PeekChar()
{
	if(EndOfFile())
		return -1;


	int aChar = GetChar();
	UngetChar(aChar);
	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::UngetChar(int theChar)
{ 
	if(theChar==EOF)
		return;

	if(theChar=='\n')
		mLineNum--;

	mBackStr.append(theChar);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::UngetString(const GUIString &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
	{
		UngetChar(theStr.at(i));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::SkipWhitespace()
{
	bool hadSpace = false;
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(!safe_isspace(aChar))
		{
			UngetChar(aChar);
			break;
		}
		else
			hadSpace = true;
	}

	return hadSpace;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::ReadEntity()
{
	GUIString anEntity;
	int i;
	for(i=0; i<20 && !EndOfFile(); i++)
	{
		int aChar = GetChar();
		anEntity.append(aChar);
		if(aChar==';')
			break;
	}

	if(i==20)
	{
		UngetString(anEntity);
		return -1;
	}

	if(anEntity.at(0)=='#')
	{
		int anAsciiValue = anEntity.atoi(1);

		if(anAsciiValue>=130 && anAsciiValue<=159)
			anAsciiValue = GetWesternChar(anAsciiValue);

		if(anAsciiValue==0)
			anAsciiValue = -1;

		return anAsciiValue;
	}

	if(anEntity.compareNoCase("GT;")==0)
		return '>';
	if(anEntity.compareNoCase("LT;")==0)
		return '<';
	if(anEntity.compareNoCase("AMP;")==0)
		return '&';
	if(anEntity.compareNoCase("QUOT;")==0)
		return '"';
	if(anEntity.compareNoCase("NBSP;")==0)
		return 0xa0; 
		
	UngetString(anEntity);
	return -1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ReadWord(GUIString &theWord,  const char *theStopChars, bool stopAtSpace, bool skip, bool erase)
{
	if(erase)
	{
		theWord.resize(0);
		theWord.reserve(32);
	}

	if(stopAtSpace)
		SkipWhitespace();

	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(aChar<256 && ((stopAtSpace && safe_isspace(aChar))  || (theStopChars!=NULL && strchr(theStopChars,aChar))))
		{
			if(aChar!=0)
			{
				UngetChar(aChar);
				break;
			}
		}

		if(aChar=='&')
		{
			aChar = ReadEntity();
			if(!skip)
			{
				if(aChar!=-1)
					theWord.append(aChar);
				else
					theWord.append('&');
			}
		}
		else
		{
			if(!skip)
				theWord.append(aChar);
		}
	}

	return !theWord.empty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLineSegment* HTMLParser::GetTextLineSegment(const GUIString &theStr, int theColor, Font *theFont)
{
	if(mInAnchor)
		return new TextLinkLineSegment(mCurLinkParam,theStr,theColor,mDefaultALinkColor,theFont);
	else
		return new TextLineSegment(theStr,theColor,theFont);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::ClearAccumulatedSegment()
{
	if(mPrevSegment.get()==NULL)
		return;

	if(!mLastWord.empty())
	{
		TextLineSegment *aTextSeg = (TextLineSegment*)mPrevSegment.get();
		aTextSeg->mText.append(mLastWord);
		AddDisplaySegment(aTextSeg,0,false);
		mLastWord.erase();
	}
	else if(!mCurLine.IsStarted())
		AddDisplaySegment(mPrevSegment,0,false);
	else
	{
		mCurLine.AddSegment(mPrevSegment);
		mCurLine.FinishBuild();
		AddDisplaySegment(mCurLine.mLine,0,false);
	}

	mCurLine.Clear();
	mPrevSegment = NULL;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::AddAccumulatedSegment()
{
	if(mPrevSegment.get()==NULL)
		return;

	if(!mCurLine.IsStarted())
		mCurLine.StartBuild(new LineElement);

	if(!mLastWord.empty())
	{
		TextLineSegment *aTextSeg = (TextLineSegment*)mPrevSegment.get();
		AddDisplaySegment(aTextSeg,0,false);
		if(dynamic_cast<TextLinkLineSegment*>(aTextSeg))
			mCurLine.AddSegment(new TextLinkLineSegment(mCurLinkParam,mLastWord,aTextSeg->mColor,mDefaultALinkColor,aTextSeg->mFont));
		else
			mCurLine.AddSegment(new TextLineSegment(mLastWord,aTextSeg->mColor,aTextSeg->mFont));
		mLastWord.erase();
	}
	else
		mCurLine.AddSegment(mPrevSegment);

	mPrevSegment = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::GetTextTag()
{
	mCurTagName = "_TEXT";
	mCurTagType = TagType_Text;
	mIsEndTag = false;
	mCurText.erase();

	GUIString aWord;
	mHadSpaceOnEnd = false;
	if(!mInBody)
	{
		ReadWord(aWord,"<",false,true);
		return true;
	}
	
	if(safe_isspace(PeekChar()))	// breakable-point
	{
		ClearAccumulatedSegment();
		GetChar();
		mCurText.assign(" ");
	}
	else if(mPrevSegment.get()!=NULL) // accumulating non-breakable segments
	{
		AddAccumulatedSegment();
		ReadWord(aWord,"<");
		bool hadSpace = false;
		if(safe_isspace(PeekChar()))
		{
			hadSpace = true;
			aWord.append(' ');
			GetChar();
		}
		mPrevSegment = GetTextLineSegment(aWord,mCurColor,mCurFont);
		if(hadSpace)
			ClearAccumulatedSegment();
		else
			return true;
	}

	while(ReadWord(aWord,"<"))
	{		
		if(safe_isspace(PeekChar()))
		{
			mCurText.append(aWord);
			mCurText.append(' ');
		}
		else
		{
			if(!mCurText.empty())
			{
				mLastWord = aWord;
				mPrevSegment = GetTextLineSegment(mCurText,mCurColor,mCurFont);
			}
			else
				mPrevSegment = GetTextLineSegment(aWord,mCurColor,mCurFont);

			return true;
		}
	}

	mHadSpaceOnEnd = true;
	AddDisplaySegment(GetTextLineSegment(mCurText, mCurColor, mCurFont));
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::GetActualTag()
{
	mCurTagAttributes.clear();
	mIsEndTag = false;

	if(PeekChar()=='/')
	{
		GetChar(); // skip '/'
		mIsEndTag = true;
	}

	ReadWord(mCurTagName,">");
	if(mCurTagName.size()>=3 && mCurTagName.at(0)=='!' && mCurTagName.at(1)=='-' && mCurTagName.at(2)=='-') // comment
	{
		// Skip comment
		mCurTagType = TagType_None;
		if(mCurTagName.size()>2 && mCurTagName.at(mCurTagName.length()-2)=='-' && mCurTagName.at(mCurTagName.length()-1)=='-' && PeekChar()=='>')
		{
			GetChar(); // skip >
			return true;
		}

		while(!EndOfFile())
		{
			int char1 = GetChar();
			int char2 = GetChar();
			int char3 = GetChar();
			if(char1=='-' && char2=='-' && char3=='>')
				return true;

			UngetChar(char3);
			UngetChar(char2);
		}
	}

	TagNameMap::iterator anItr = mTagNameMap.find(mCurTagName);
	if(anItr==mTagNameMap.end()) // unrecognized tag... just skip it
	{
		mCurTagType = TagType_None;
		GUIString aSkip;
		ReadWord(aSkip,">",false,true);
		GetChar(); // skip '>'
		return true;
	}
	else
		mCurTagType = anItr->second;

	while(!EndOfFile())
	{
		if(PeekChar()=='>')
		{
			GetChar(); // skip '>'
			return true;
		}

		GUIString anAttribute;
		GUIString aValue;

		ReadWord(anAttribute,">=");
		SkipWhitespace();
		if(PeekChar()=='=') // read attribute value
		{
			GetChar(); // skip '=' sign
			SkipWhitespace();
			if(PeekChar()=='"')
			{
				GetChar(); // Skip '"' sign
				ReadWord(aValue,"\"",false);
				GetChar(); // Skip '"' sign
			}
			else if(PeekChar()=='\'')
			{
				GetChar(); // Skip ' sign
				ReadWord(aValue,"'",false);
				GetChar(); // Skip ' sign
			}
			else
				ReadWord(aValue,">");
		}
		mCurTagAttributes[anAttribute] = aValue;

	}

	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::GetNextTag()
{
	if(!mInBody || mHadSpaceOnEnd && !mInPre)
		SkipWhitespace();

	int aChar = GetChar();
	if(aChar=='<')
		return GetActualTag();
	else 
	{
		UngetChar(aChar);
		return GetTextTag();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleOrderedList()
{
	mListStack.push_back(ListInfo());
	ListInfo &anInfo = mListStack.back();
	anInfo.mOrdered = true;
	anInfo.mCount = 0;
	anInfo.mType = 0;

	GUIString *aParam;
	aParam = GetParam("TYPE");
	if(aParam!=NULL)
	{
		if(aParam->compare("1")==0)
			anInfo.mType = 0;
		else if(aParam->compare("a")==0)
			anInfo.mType = 1;
		else if(aParam->compare("A")==0)
			anInfo.mType = 2;
		else if(aParam->compare("i")==0)
			anInfo.mType = 3;
		else if(aParam->compare("I")==0)
			anInfo.mType = 4;
	}

	aParam = GetParam("START");
	if(aParam!=NULL)
		anInfo.mCount = aParam->atoi();

	if(anInfo.mCount<=0)
		anInfo.mCount = 1;

	if(mListStack.size()==1)
		AddParagraphBreak();

	AddDisplaySegment(new MarginLineSegment(mListStack.size()*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleUnorderedList()
{
	mListStack.push_back(ListInfo());
	ListInfo &anInfo = mListStack.back();
	anInfo.mOrdered = false;
	anInfo.mCount = 0;
	anInfo.mType = -1;	

	GUIString *aParam;
	aParam = GetParam("TYPE");
	if(aParam!=NULL)
	{
		if(aParam->compareNoCase("disc"))
			anInfo.mType = 1;
		else if(aParam->compareNoCase("circle"))
			anInfo.mType = 2;
		else if(aParam->compareNoCase("square"))
			anInfo.mType = 3;
	}

	if(anInfo.mType==-1)
	{
		int i = mListStack.size()-2;
		while(i>=0)
		{
			ListInfo &anotherInfo = mListStack[i];
			if(!anotherInfo.mOrdered)
			{
				anInfo.mType = anotherInfo.mType + 1;
				break;
			}
			i--;
		}
	}

	aParam = GetParam("START");
	if(aParam!=NULL)
		anInfo.mCount = aParam->atoi();

	if(anInfo.mCount<=0)
		anInfo.mCount = 1;

	if(anInfo.mType<0)
		anInfo.mType = 1;
	if(anInfo.mType>3)
		anInfo.mType = 3;

	if(mListStack.size()==1)
		AddParagraphBreak();

	AddDisplaySegment(new MarginLineSegment(mListStack.size()*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleEndList()
{
	if(mListStack.empty())
		return;

	mListStack.pop_back();
	AddDisplaySegment(new MarginLineSegment(mListStack.size()*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
	if(mListStack.empty())
		AddParagraphBreak();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetLetterItem(int theCount, char *theBuf, bool upper)
{
	int aPos = 0;
	while(theCount>0)
	{
		theCount--;
		theBuf[aPos++] = (upper?'A':'a') + theCount%26;
		theCount/=26;
	} 


	theBuf[aPos] = '.';
	theBuf[aPos+1] = 0;

	// Reverse String
	int i = 0;
	aPos--;
	while(i<aPos)
	{
		char aTemp = theBuf[aPos]; theBuf[aPos] = theBuf[i]; theBuf[i] = aTemp;
		i++; aPos--;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetRomanNumeralHelp(char *theBuf, int &thePos, int &theNumber, int hiVal, char hiChar, char midChar, char lowChar)
{
	int lowVal = hiVal/10;
	int midVal = hiVal>>1;

	while(theNumber>=hiVal)
	{
		theBuf[thePos++] = hiChar;
		theNumber-=hiVal;
	}

	if(theNumber>=hiVal-lowVal)
	{
		theBuf[thePos++] = lowChar;
		theBuf[thePos++] = hiChar;
		theNumber-=hiVal-lowVal;
	}

	if(theNumber>=midVal)
	{
		theBuf[thePos++] = midChar;
		theNumber-=midVal;
	}

	if(theNumber>=midVal-lowVal)
	{
		theBuf[thePos++] = lowChar;
		theBuf[thePos++] = midChar;
		theNumber-=midVal-lowVal;
	}

	while(theNumber>=lowVal)
	{
		theBuf[thePos++] = lowChar;
		theNumber-=lowVal;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetRomanNumeralItem(int theCount, char *theBuf, bool upper)
{
	static char lower_letters[] = { 'm', 'd', 'c', 'l', 'x', 'v', 'i' };
	static char upper_letters[] = { 'M', 'D', 'C', 'L', 'X', 'V', 'I' };
	const char* letters = upper?upper_letters:lower_letters;

	if(theCount>=4000)
	{
		sprintf(theBuf,"%d.",theCount);
		return;
	}

	int aPos = 0;
	int aHiVal = 1000;
	int aLetterPos = 0;
	for(int i=0; i<3; i++)
	{
		GetRomanNumeralHelp(theBuf,aPos,theCount,aHiVal,letters[aLetterPos],letters[aLetterPos+1],letters[aLetterPos+2]);
		aHiVal/=10;
		aLetterPos+=2;
	}
	theBuf[aPos++] = '.';
	theBuf[aPos] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleListItem()
{
	if(mListStack.empty())
	{
		AddLineBreak();
		mPrevSegment = new BulletElement(mCurColor);
//		mDisplay->AddSegment(new BulletElement(mCurColor,0),LineSegmentFlag_PreLineBreak);
		return;
	}

	ListInfo &anInfo = mListStack.back();
	AddLineBreak();

	if(anInfo.mOrdered)
	{
		char aBuf[100];
		aBuf[0] = 0;
		switch(anInfo.mType)
		{
		case 0: sprintf(aBuf,"%d.",anInfo.mCount); break;
		case 1: GetLetterItem(anInfo.mCount,aBuf,false); break;
		case 2: GetLetterItem(anInfo.mCount,aBuf,true); break;
		case 3: GetRomanNumeralItem(anInfo.mCount,aBuf,false); break;
		case 4: GetRomanNumeralItem(anInfo.mCount,aBuf,true); break;
		}

		mPrevSegment = new OrderedListElement(mCurColor,mCurFont,aBuf);
	}
	else
		mPrevSegment = new BulletElement(mCurColor,anInfo.mType);

	mLineType = 1;
	anInfo.mCount++;
//	AddDisplaySegment(new BulletElement(mCurColor,anInfo.mType));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HorzAlign HTMLParser::GetCurAlign()
{
	if(mAlignStack.empty())
		return mDefaultAlign;
	else
		return mAlignStack.back();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::PushAlign(HorzAlign theAlign, bool force)
{
	HorzAlign anOldAlign = GetCurAlign();
	mAlignStack.push_back(theAlign);

	if(anOldAlign!=theAlign || force)
		AddDisplaySegment(new AlignLineSegment(theAlign),LineSegmentFlag_PreLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::PopAlign()
{
	if(mAlignStack.empty())
		return;

	HorzAlign anOldAlign = GetCurAlign();
	mAlignStack.pop_back();
	HorzAlign aNewAlign = GetCurAlign();

	if(aNewAlign!=anOldAlign)
		AddDisplaySegment(new AlignLineSegment(aNewAlign),LineSegmentFlag_PreLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::CheckAlignment()
{
	GUIString *anAlignStr = GetParam("ALIGN");
	if(anAlignStr==NULL)
	{
		PushAlign(GetCurAlign());
		return;
	}

	if(anAlignStr->compareNoCase("CENTER")==0)
		PushAlign(HorzAlign_Center);
	else if(anAlignStr->compareNoCase("RIGHT")==0)
		PushAlign(HorzAlign_Right);
	else if(anAlignStr->compareNoCase("LEFT")==0)
		PushAlign(HorzAlign_Left);
	else if(anAlignStr->compareNoCase("JUSTIFY")==0)
		PushAlign(HorzAlign_Left);
	else
		PushAlign(GetCurAlign());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetFontStyle()
{
	int aStyle = 0;
	if(mBoldCount)
		aStyle |= FontStyle_Bold;
	if(mItalicCount)
		aStyle |= FontStyle_Italic;
	if(mUnderlineCount)
		aStyle |= FontStyle_Underline;

	return aStyle;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::CalcFont()
{
	static int FONT_SIZE_TABLE[7] = { 8, 10, 12, 14, 18, 24, 36 };
	
	if(mFontStack.empty())
		return;

	FontInfo anInfo = mFontStack.back();
	int aSize = anInfo.mSize-1;
	if(aSize<0)
		aSize = 0;
	if(aSize>6)
		aSize = 6;

	FontDescriptor aDescriptor(anInfo.mFace, GetFontStyle(), FONT_SIZE_TABLE[aSize]);
	mCurFont = mWindow->GetFont(aDescriptor);
	if(anInfo.mColor<0)
		mCurColor = mDefaultTextColor;
	else
		mCurColor = anInfo.mColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::PushFont(const char *theFace, int theSize, int theColor)
{
	FontInfo anInfo;
	if(!mFontStack.empty())
		anInfo = mFontStack.back();
	else
	{
		anInfo.mColor = -1;
		anInfo.mSize = 3;
		anInfo.mFace = mDefaultFontFace;
	}

	if(theFace!=NULL)
		anInfo.mFace = theFace;

	if(theSize>=1 && theSize<=7)
		anInfo.mSize = theSize;

	if(theColor>=0)
		anInfo.mColor = theColor;

	mFontStack.push_back(anInfo);
	CalcFont();
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::PopFont()
{
	if(!mFontStack.empty())
	{
		mFontStack.pop_back();
		CalcFont();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString* HTMLParser::GetParam(const GUIString &theName)
{
	TagAttributes::iterator anItr = mCurTagAttributes.find(theName);
	if(anItr==mCurTagAttributes.end())
		return NULL;

	return &anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetPercentValue(GUIString *theVal)
{
	if(theVal==NULL || theVal->empty())
		return 0;

	if(theVal->at(theVal->length()-1)=='%')
		return -theVal->atoi();
	else
		return theVal->atoi();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetHexDigit(char theChar)
{
	if(theChar>='0' && theChar<='9')
		return theChar - '0';
	
	theChar = toupper(theChar);
	if(theChar>='A' && theChar<='F')
		return theChar - 'A' + 10;

	return -1;
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::GetIntParam(const GUIString &theName, int &theInt)
{
	GUIString *aParam = GetParam(theName);
	if(aParam==NULL)
		return false; 

	string aStr = *aParam;
	int anInt = 0;
	if(sscanf(aStr.c_str(),"%d",&theInt)==1)
		return true;
	else
		return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetColor(GUIString *theStr)
{
	if(theStr==NULL)
		return -1;

	if(theStr->empty())
		return -1;

	int anOffset = 0;
	if(theStr->at(0)!='#')
	{
		struct ColorDefine { const char* mName; int mColor; };
		static ColorDefine aColorTable[] = {
			{"Black",			0x000000},
			{"Silver",			0xC0C0C0},
			{"Gray",			0x808080},
			{"White",			0xFFFFFF},
			{"Maroon",			0x800000},
			{"Red",				0xFF0000},
			{"Purple",			0x800080},
			{"Fuchsia",			0xFF00FF},
			{"Green",			0x008000},
			{"Lime",			0x00FF00},
			{"Olive",			0x808000},
			{"Yellow",			0xFFFF00},
			{"Navy",			0x000080},
			{"Blue",			0x0000FF},
			{"Teal",			0x008080},
			{"Aqua",			0x00FFFF},
			{"Snow",					0xFFFAFA},
			{"GhostWhite",				0xF8F8FF },
			{"WhiteSmoke",				0xF5F5F5 },
			{"Gainsboro",				0xDCDCDC },
			{"FloralWhite",				0xFFFAF0 },
			{"OldLace",					0xFDF5E6 },
			{"Linen",					0xFAF0E6 },
			{"AntiqueWhite",			0xFAEBD7 },
			{"PapayaWhip",				0xFFEFD5 },
			{"BlanchedAlmond",			0xFFEBCD },
			{"Bisque",					0xFFE4C4 },
			{"PeachPuff",				0xFFDAB9 },
			{"NavajoWhite",				0xFFDEAD },
			{"Moccasin",				0xFFE4B5 },
			{"Cornsilk",				0xFFF8DC },
			{"Ivory",					0xFFFFF0 },
			{"LemonChiffon",			0xFFFACD },
			{"Seashell",				0xFFF5EE },
			{"Honeydew",				0xF0FFF0 },
			{"MintCream",				0xF5FFFA },
			{"Azure",					0xF0FFFF },
			{"AliceBlue",				0xF0F8FF },
			{"lavender",				0xE6E6FA },
			{"LavenderBlush",			0xFFF0F5 },
			{"MistyRose",				0xFFE4E1 },
			{"White",					0xFFFFFF },
			{"Black",					0x000000 },
			{"DarkSlateGray",			0x2F4F4F },
			{"DimGrey",					0x696969 },
			{"SlateGrey",				0x708090 },
			{"LightSlateGray",			0x778899 },
			{"Grey",					0xBEBEBE },
			{"LightGrey",				0xD3D3D3 },
			{"MidnightBlue",			0x191970 },
			{"NavyBlue",				0x000080 },
			{"CornflowerBlue",			0x6495ED },
			{"DarkSlateBlue",			0x483D8B },
			{"SlateBlue",				0x6A5ACD },
			{"MediumSlateBlue",			0x7B68EE },
			{"LightSlateBlue",			0x8470FF },
			{"MediumBlue",				0x0000CD },
			{"RoyalBlue",				0x4169E1 },
			{"Blue",					0x0000FF },
			{"DodgerBlue",				0x1E90FF },
			{"DeepSkyBlue",				0x00BFFF },
			{"SkyBlue",					0x87CEEB },
			{"LightSkyBlue",			0x87CEFA },
			{"SteelBlue",				0x4682B4 },
			{"LightSteelBlue",			0xB0C4DE },
			{"LightBlue",				0xADD8E6 },
			{"PowderBlue",				0xB0E0E6 },
			{"PaleTurquoise",			0xAFEEEE },
			{"DarkTurquoise",			0x00CED1 },
			{"MediumTurquoise",			0x48D1CC },
			{"Turquoise",				0x40E0D0 },
			{"Cyan",					0x00FFFF },
			{"LightCyan",				0xE0FFFF },
			{"CadetBlue",				0x5F9EA0 },
			{"MediumAquamarine",		0x66CDAA },
			{"Aquamarine",				0x7FFFD4 },
			{"DarkGreen",				0x006400 },
			{"DarkOliveGreen",			0x556B2F },
			{"DarkSeaGreen",			0x8FBC8F },
			{"SeaGreen",				0x2E8B57 },
			{"MediumSeaGreen",			0x3CB371 },
			{"LightSeaGreen",			0x20B2AA },
			{"PaleGreen",				0x98FB98 },
			{"SpringGreen",				0x00FF7F },
			{"LawnGreen",				0x7CFC00 },
			{"Green",					0x00FF00 },
			{"Chartreuse",				0x7FFF00 },
			{"MedSpringGreen",			0x00FA9A },
			{"GreenYellow",				0xADFF2F },
			{"LimeGreen",				0x32CD32 },
			{"YellowGreen",				0x9ACD32 },
			{"ForestGreen",				0x228B22 },
			{"OliveDrab",				0x6B8E23 },
			{"DarkKhaki",				0xBDB76B },
			{"PaleGoldenrod",			0xEEE8AA },
			{"LtGoldenrodYello",		0xFAFAD2 },
			{"LightYellow",				0xFFFFE0 },
			{"Yellow",					0xFFFF00 },
			{"Gold",					0xFFD700 },
			{"LightGoldenrod",			0xEEDD82 },
			{"goldenrod",				0xDAA520 },
			{"DarkGoldenrod",			0xB8860B },
			{"RosyBrown",				0xBC8F8F },
			{"IndianRed",				0xCD5C5C },
			{"SaddleBrown",				0x8B4513 },
			{"Sienna",					0xA0522D },
			{"Peru",					0xCD853F },
			{"Burlywood",				0xDEB887 },
			{"Beige",					0xF5F5DC },
			{"Wheat",					0xF5DEB3 },
			{"SandyBrown",				0xF4A460 },
			{"Tan",						0xD2B48C },
			{"Chocolate",				0xD2691E },
			{"Firebrick",				0xB22222 },
			{"Brown",					0xA52A2A },
			{"DarkSalmon",				0xE9967A },
			{"Salmon",					0xFA8072 },
			{"LightSalmon",				0xFFA07A },
			{"Orange",					0xFFA500 },
			{"DarkOrange",				0xFF8C00 },
			{"Coral",					0xFF7F50 },
			{"LightCoral",				0xF08080 },
			{"Tomato",					0xFF6347 },
			{"OrangeRed",				0xFF4500 },
			{"Red",						0xFF0000 },
			{"HotPink",					0xFF69B4 },
			{"DeepPink",				0xFF1493 },
			{"Pink",					0xFFC0CB },
			{"LightPink",				0xFFB6C1 },
			{"PaleVioletRed",			0xDB7093 },
			{"Maroon",					0xB03060 },
			{"MediumVioletRed",			0xC71585 },
			{"VioletRed",				0xD02090 },
			{"Magenta",					0xFF00FF },
			{"Violet",					0xEE82EE },
			{"Plum",					0xDDA0DD },
			{"Orchid",					0xDA70D6 },
			{"MediumOrchid",			0xBA55D3 },
			{"DarkOrchid",				0x9932CC },
			{"DarkViolet",				0x9400D3 },
			{"BlueViolet",				0x8A2BE2 },
			{"Purple",					0xA020F0 },
			{"MediumPurple",			0x9370DB },
			{"Thistle",					0xD8BFD8 },
			{"DarkGrey",				0xA9A9A9 },
			{"DarkBlue",				0x00008B },
			{"DarkCyan",				0x008B8B },
			{"DarkMagenta",				0x8B008B },
			{"DarkRed",					0x8B0000 },
			{"LightGreen",				0x90EE90 }
		};
		typedef map<GUIString,int,StringLessNoCase> ColorMap; 
		static ColorMap aColorMap;
		if(aColorMap.empty())
		{
			int aNumColors = sizeof(aColorTable)/sizeof(ColorDefine);
			for(int i=0; i<aNumColors; i++)
				aColorMap[aColorTable[i].mName] = aColorTable[i].mColor;
		}
		ColorMap::iterator anItr = aColorMap.find(*theStr);
		if(anItr!=aColorMap.end())
			return anItr->second;	
//		else
//			return -1;
	}
	else
		anOffset = 1;

//	if(theStr->length()!=7)
//		return -1;

	int aColor = 0, i;
	for(i=0; i<6; i++)
	{
		aColor<<=4;
		int aDigit = GetHexDigit(theStr->at(i+anOffset));
		if(aDigit<0)
			return -1;

		aColor |= aDigit;
	
		if(theStr->at(i+2)=='\0')
			break;
	}

	if(i<6)
		aColor<<=(4*(5-i));
	
	return aColor;

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr HTMLParser::GetImage(GUIString *theImagePath, AnimationPtr *theAnimation, RawImagePtr *theRawImage)
{
	if(theImagePath==NULL)
		return NULL;

//	if(mImageFetcher!=NULL)
//		return 

//	string aPath = mPath + (string)(*theImagePath);
	string aPath = mDocumentFetcher((string)(*theImagePath),mPath);
	MultiImageDecoder *aDecoder = mWindow->GetWindowManager()->GetImageDecoder();
	RawImagePtr aRawImage = aDecoder->DecodeRaw(aPath.c_str());
	if(aRawImage.get()==NULL)
		return NULL;

	NativeImagePtr anImage = aRawImage->GetNative(mWindow->GetDisplayContext());
//	NativeImagePtr anImage = aDecoder->DecodeRaw(mWindow->GetDisplayContext(),aPath.c_str());
	if(theAnimation!=NULL)
	{
		AnimationPtr anAnimation = aDecoder->GetAnimation(false);
		if(anAnimation.get()!=NULL)
			anAnimation->RealizeNative(mWindow->GetDisplayContext());

		*theAnimation = anAnimation;
	}
	if(theRawImage!=NULL)
		*theRawImage = aRawImage;

	return anImage;

/*
	char *anExt = NULL;
	anExt = strrchr(aPath.c_str(),'.');
	if(anExt==NULL)
		return false;

	anExt++;
	NativeImagePtr anImage;
	DisplayContext *aContext = mWindow->GetDisplayContext();
	if(stricmp(anExt,"jpg")==0 || stricmp(anExt,"jpeg")==0)
		anImage = JPEGDecoder::Decode(aContext,aPath.c_str());
	else if(stricmp(anExt,"png")==0)
		anImage = PNGDecoder::Decode(aContext,aPath.c_str());
	else if(stricmp(anExt,"gif")==0)
	{
		GifDecoder aDecoder;
		anImage = aDecoder.Decode(aContext,aPath.c_str());
		if(theAnimation!=NULL)
		{
			Animation *anAnimation = aDecoder.GetAnimation();
			if(anAnimation!=NULL && anAnimation->GetNumFrames()>1)
				*theAnimation = anAnimation;
		}
	}

	return anImage;*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::HandleImage()
{
	GUIString *anImagePath = GetParam("SRC");
	if(anImagePath==NULL)
		return false;

	bool isNew = true;
	LoadedImageMap::iterator anImageItr = mLoadedImageMap.insert(LoadedImageMap::value_type(*anImagePath,LoadedImageStruct())).first;
	LoadedImageStruct &aStruct = anImageItr->second;
	if(aStruct.mImage.get()==NULL)
		aStruct.mImage = GetImage(anImagePath, &aStruct.mAnimation, &aStruct.mRawImage);
	else
		isNew = false;
	
	NativeImagePtr anImage = aStruct.mImage;
	if(anImage.get()==NULL)
		return false;

	RawImagePtr aRawImage = aStruct.mRawImage;

	if(aStruct.mAnimation.get()!=NULL)
	{
		AnimationPtr anAnim = aStruct.mAnimation;
		if(!isNew)
			anAnim = anAnim->Duplicate();

		AnimationComponentPtr anAnimComp = new AnimationComponent(anAnim);
		anAnimComp->Start();
		AddDisplaySegment(new ComponentLineSegment(anAnimComp));
		return true;
	}

	int hspace = 0, vspace = 0;
	GUIString *aParam = GetParam("ALIGN");
	int aFlags = 0;
	int anAlign = 0;
	if(aParam!=NULL)
	{
		if(aParam->compareNoCase("LEFT")==0)
		{
			aFlags |= LineSegmentFlag_FloatLeft;
			hspace = 3;
		}
		else if(aParam->compareNoCase("RIGHT")==0)
		{
			aFlags |= LineSegmentFlag_FloatRight;
			hspace = 3;
		}
		else if(aParam->compareNoCase("MIDDLE")==0)
			anAlign = 1;
		else if(aParam->compareNoCase("TOP")==0)
			anAlign = 2;
	}

	aParam = GetParam("HSPACE");
	if(aParam!=NULL)
		hspace = aParam->atoi();

	aParam = GetParam("VSPACE");
	if(aParam!=NULL)
		vspace = aParam->atoi();
	
	int aWidth = anImage->GetWidth();
	int aHeight = anImage->GetHeight();
	aParam = GetParam("WIDTH");
	if(aParam!=NULL)
		aWidth = aParam->atoi();
	aParam = GetParam("HEIGHT");
	if(aParam!=NULL)
		aHeight = aParam->atoi();

	ImageLineSegmentPtr anImageSegment;
	if((aWidth!=anImage->GetWidth() || aHeight!=anImage->GetHeight()))
	{
		char aBuf[50];
		sprintf(aBuf,"_%d_%d",aWidth,aHeight);
		GUIString aScaleName = *anImagePath;
		aScaleName.append(aBuf);
		anImageItr = mScaledImageMap.insert(LoadedImageMap::value_type(aScaleName,LoadedImageStruct())).first;
		LoadedImageStruct &aStruct = anImageItr->second;
		if(aStruct.mImage.get()==NULL)
		{
			if(aRawImage.get()==NULL)
				aRawImage = anImage->GetRaw();

			ScaleImageFilterPtr aFilter = new ScaleImageFilter(aWidth,aHeight);
			aStruct.mImage = aFilter->Filter(aRawImage)->GetNative(mWindow->GetDisplayContext());
//			aStruct.mImage = anImage->Scale(aWidth,aHeight);
//			aStruct.mImage = anImage;
		}

		anImage = aStruct.mImage;
	}

	ImageMap *anImageMap = NULL;
	aParam = GetParam("USEMAP");
	if(aParam!=NULL && aParam->length()>1 && aParam->at(0)=='#')
	{
		ImageMapMap::iterator anItr = mImageMapMap.insert(ImageMapMap::value_type(aParam->substr(1,aParam->length()-1),NULL)).first;
		if(anItr->second==NULL)
			anItr->second = new ImageMap;

		anImageMap = anItr->second;
	}

	int aBorder = mInAnchor?2:0;
	int aBorderColor = mCurColor;
	if(mInAnchor)
		anImageSegment = new ImageLinkLineSegment(mCurLinkParam,mCurColor,mDefaultALinkColor,anImage);
	else if(anImageMap!=NULL)
	{
		aBorder = 2;
		aBorderColor = mDefaultLinkColor;
		anImageSegment = new ImageLinkLineSegment(NULL,mCurColor,mDefaultALinkColor,anImage);
		((ImageLinkLineSegment*)anImageSegment.get())->mImageMap = anImageMap;
	}
	else
		anImageSegment = new ImageLineSegment(anImage);

	aParam = GetParam("BORDER");
	if(aParam!=NULL)
		aBorder = aParam->atoi();

	anImageSegment->mHSpace = hspace;
	anImageSegment->mVSpace = vspace;
	anImageSegment->mAlign = anAlign;
	anImageSegment->mBorder = aBorder;
	anImageSegment->mBorderColor = aBorderColor;

//	if(mPrevSegment.get()!=NULL)
	{
//		if(dynamic_cast<TextLineSegment*>(mPrevSegment.get())!=NULL)
			ClearAccumulatedSegment();
//		else
//			AddAccumulatedSegment();
	}

	if(aFlags!=0)
	{
		AddDisplaySegment(anImageSegment,aFlags);
	}
	else
	{
		mPrevSegment = anImageSegment;
		mHadSpaceOnEnd = false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleFont(bool isBase)
{
	int aSize = -1;
	int aColor = -1;
	std::string aFace;
	GUIString *aParam;

	aParam = GetParam("FACE");
	if(aParam!=NULL)
	{
		int aCommaPos = aParam->find(',');
		if(aCommaPos>=0)
			aFace = aParam->substr(0,aCommaPos);
		else
			aFace = *aParam;
	}

	aParam = GetParam("SIZE");
	if(aParam!=NULL && !aParam->empty())
	{
		char aStartChar = aParam->at(0);
		if(aStartChar=='+' || aStartChar=='-')
		{
			int aVal = aParam->atoi(1);
			if(aStartChar=='+')
				aSize = 3 + aVal;
			else
				aSize = 3 - aVal;
		}
		else
			aSize = aParam->atoi();
	}

	aParam = GetParam("COLOR");
	aColor = GetColor(aParam);

	if(isBase)
		mDefaultFontFace = aFace;

	string aFaceStr = aFace;
	PushFont(aFace.empty()?NULL:aFaceStr.c_str(),aSize,aColor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleTable()
{
	mInTable = true;
	mCurTable = new TableElement;

	GUIString *aParam;
	aParam = GetParam("CELLSPACING");
	if(aParam!=NULL)
		mCurTable->mCellSpacing = aParam->atoi();

	aParam = GetParam("CELLPADDING");
	if(aParam!=NULL)
		mCurTable->mCellPadding = aParam->atoi();

	aParam = GetParam("BORDER");
	if(aParam!=NULL)
	{
		mCurTable->mBorder = aParam->atoi();
		if(mCurTable->mBorder==0 && aParam->at(0)!='0')
			mCurTable->mBorder = 1;
	}

	aParam = GetParam("BGCOLOR");
	mCurTable->mBGColor = GetColor(aParam);

	aParam = GetParam("WIDTH");
	mCurTable->mWidthSpec = GetPercentValue(aParam);

	mCurTableAlign = 0;
	aParam = GetParam("ALIGN");
	if(aParam!=NULL)
	{
		if(aParam->compareNoCase("Left")==0)
			mCurTableAlign = 1;
		else if(aParam->compareNoCase("Center")==0)
			mCurTableAlign = 2;
		else if(aParam->compareNoCase("Right")==0)
			mCurTableAlign = 3;
	}

//	mCurTable->mCellSpacing = 5;
//	mCurTable->mCellPadding = 5;
//	mCurTable->mBorder = 1;

	mCurTable->StartTraversal();

//	ALIGN=[ left | center | right ] (table alignment) 
//	BGCOLOR=Color (table background color) 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetTableDataHAlign(GUIString *theAlign, int theDefault)
{
	if(theAlign==NULL)
		return theDefault;

	if(theAlign->compareNoCase("LEFT")==0)
		return 0;
	else if(theAlign->compareNoCase("CENTER")==0)
		return 1;
	else if(theAlign->compareNoCase("MIDDLE")==0)
		return 1;
	else if(theAlign->compareNoCase("RIGHT")==0)
		return 2;
	else
		return theDefault;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser::GetTableDataVAlign(GUIString *theAlign)
{
	if(theAlign==NULL)
		return -1;

	if(theAlign->compareNoCase("TOP")==0)
		return 0;
	else if(theAlign->compareNoCase("MIDDLE")==0)
		return 1;
	else if(theAlign->compareNoCase("CENTER")==0)
		return 1;
	else if(theAlign->compareNoCase("BOTTOM")==0)
		return 2;
	else
		return -1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleTableRow()
{
	if(mInTable)
	{
		mCurTable->NextRow();
		GUIString *aParam = GetParam("BGCOLOR");
		mCurTable->SetRowBGColor(GetColor(aParam));

		int anHAlign = GetTableDataHAlign(GetParam("ALIGN"));
		int aVAlign = GetTableDataVAlign(GetParam("VALIGN"));
		if(anHAlign==-1)
			anHAlign = 0;
		if(aVAlign==-1)
			aVAlign = 1;

		mCurTable->SetRowAlign(anHAlign, aVAlign);
	}
	else if(mParsingTableData)
		HandleEndTableData();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleTableData(bool isHeader)
{
	if(mInTable)
	{
		GUIString *aParam;
		TableElement::CellInfo *aCellInfo = mCurTable->NextColumn();
		MultiLineElement *anElement = aCellInfo->mContents;

		aCellInfo->mBGColor = GetColor(GetParam("BGCOLOR"));
		aCellInfo->mHAlign = GetTableDataHAlign(GetParam("ALIGN"),isHeader?1:0);
		aCellInfo->mVAlign = GetTableDataVAlign(GetParam("VALIGN"));

		if(GetParam("NOWRAP")!=NULL)
			aCellInfo->mNoWrap = true;
		
		int aRowSpan = 0, aColSpan = 0;
		aParam = GetParam("ROWSPAN");
		if(aParam!=NULL)
			aRowSpan = aParam->atoi();
		
		aParam = GetParam("COLSPAN");
		if(aParam!=NULL)
			aColSpan = aParam->atoi();

		if(aRowSpan<=0) aRowSpan = 1;
		if(aColSpan<=0) aColSpan = 1;

		int aWidth = GetPercentValue(GetParam("WIDTH"));
		int aHeight = GetPercentValue(GetParam("HEIGHT"));
		if(aWidth>=0)
			aCellInfo->mWidth = aWidth;
		else if(aColSpan<=1)
			mCurTable->SetColumnWidth(aWidth);
		if(aHeight>0)
			aCellInfo->mHeight = aHeight;

		HorzAlign aDefaultAlign = mCurTable->GetCurRowAlign();
		if(aCellInfo->mHAlign==1)
			aDefaultAlign = HorzAlign_Center;
		else if(aCellInfo->mHAlign==2)
			aDefaultAlign = HorzAlign_Right;

		// Important!  aCellInfo can be invalidated with SetCellSpan since this can grow the vector so call SetCellSpan after done using aCellInfo
		if(aRowSpan>1 || aColSpan>1)
			mCurTable->SetCellSpan(aRowSpan, aColSpan);

		HTMLParser aParser;

		aParser.ParseTableData(*this,anElement,isHeader,aDefaultAlign);
		CopyTagInfo(aParser);
		if(mIsEndTag)
			HandleEndTag();
		else
			HandleStartTag();
	}
	else if(mParsingTableData)
		HandleEndTableData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleEndTableData()
{
	mDone = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleSelect()
{
	mCurCombo = new MSComboBox;
	mCurCombo->SetSize(50,22);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleOption()
{
	GUIString aWord;
	GUIString anOption;
	bool done = false;
	while(!EndOfFile() && !done)
	{
		while(ReadWord(aWord,"<"))
		{		
			anOption.append(aWord);
			if(safe_isspace(PeekChar()))
				anOption.append(' ');
		}
		if(PeekChar()=='<')
		{
			GetChar();
			if(ReadWord(aWord))
			{
				if(aWord.compareNoCase("/OPTION") || aWord.compareNoCase("OPTION"))
				{
					UngetString(aWord);
					UngetChar('<');
					done = true;
				}
			}
		}
	}

	mCurCombo->InsertString(anOption);
	int aWidth = mCurCombo->GetListArea()->GetTotalWidth();
	mCurCombo->SetSize(aWidth+20,mCurCombo->Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleInput()
{
	GUIString *aParam;
	std::string aType = "Text";
	std::string aValue;

	aParam = GetParam("TYPE");
	if(aParam!=NULL)
		aType = *aParam;

	aParam = GetParam("VALUE");
	if(aParam!=NULL)
		aValue = *aParam;

	LineSegmentPtr aSegment;
//	FontPtr aFont = mWindow->GetFont(FontDescriptor("Times New Roman",FontStyle_Plain,12));
	FontPtr aFont = mWindow->GetFont(FontDescriptor("Tahoma",FontStyle_Plain,10));
	if(StringCompareNoCase(aType,"Button")==0 || StringCompareNoCase(aType,"Submit")==0 || StringCompareNoCase(aType,"Reset")==0)
	{
		if(aValue.empty())
			aValue = aType;

		MSButtonPtr aButton = new MSButton(aValue.c_str());
		aButton->SetFont(aFont);
		aButton->SetDesiredSize();
		aSegment = new ComponentLineSegment(aButton);
	}
	else if(StringCompareNoCase(aType,"Radio")==0)
	{
		MSRadioButtonPtr aRadio = new MSRadioButton;
		aRadio->SetTransparent(true);
		aRadio->SetSize(15,12);
		aRadio->SetButtonFlags(ButtonFlag_Radio,false);
		aSegment = new ComponentLineSegment(aRadio);
	}
	else if(StringCompareNoCase(aType,"Checkbox")==0)
	{
	}
	else if(StringCompareNoCase(aType,"File")==0)
	{
	}
	else if(StringCompareNoCase(aType,"Hidden")==0)
	{
	}
	else if(StringCompareNoCase(aType,"Image")==0)
	{
		NativeImagePtr anImage = GetImage(GetParam("SRC"));
		if(anImage.get()!=NULL)
		{
			ImageButtonPtr anImageButton = new ImageButton;
			anImageButton->SetSize(anImage->GetWidth(),anImage->GetHeight());
			anImageButton->SetImage(ImageButtonType_Normal,anImage);
			anImageButton->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
			aSegment = new ComponentLineSegment(anImageButton);
		}
	}
	else if(StringCompareNoCase(aType,"Select")==0)
	{
		HandleSelect();
		return;
	}
	else 
	{
		MSInputBoxPtr anInputBox = new MSInputBox;
		anInputBox->SetFont(aFont);
		if(StringCompareNoCase(aType,"Password")==0)
			anInputBox->SetPasswordChar('*');
		else if(StringCompareNoCase(aType,"Readonly")==0)
			anInputBox->SetComponentFlags(ComponentFlag_WantFocus,false);

		int aSize = 0;
		int aMaxChars = 0;
		aParam = GetParam("SIZE");
		if(aParam!=NULL)
			aSize = aParam->atoi();
		if(aSize==0)
			aSize = 20;

		aParam = GetParam("MAXLENGTH");
		if(aParam!=NULL)
			aMaxChars = aParam->atoi();

//		anInputBox->SetSize(anInputBox->GetFont()->GetCharWidth('M')*(aSize+1),24);
		anInputBox->SetSize(aSize*6+24,22);
		anInputBox->SetMaxChars(aMaxChars);
		anInputBox->SetText(aValue);
		aSegment = new ComponentLineSegment(anInputBox);
	}

//	aSegment = NULL;
	if(aSegment.get()!=NULL)
	{
		AddAccumulatedSegment();
		mPrevSegment = aSegment;
		mHadSpaceOnEnd = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleHorizontalRule()
{
	HorizontalRuleSegmentPtr aSeg = new HorizontalRuleSegment;
	int aSize = 0;
	GUIString *aParam = GetParam("SIZE");
	if(aParam!=NULL)
		aSize = aParam->atoi();

	if(aSize<=0)
		aSize = 2;

	aSeg->mHeight = aSize;
	aSeg->mWidthSpec = GetPercentValue(GetParam("WIDTH"));
	aSeg->mNoShade = GetParam("NOSHADE")!=NULL;

	HorzAlign anAlign = HorzAlign_Center;
	aParam = GetParam("ALIGN");
	if(aParam!=NULL)
	{
		if(StringCompareNoCase(*aParam,"RIGHT")==0)
			anAlign = HorzAlign_Right;
		else if(StringCompareNoCase(*aParam,"LEFT")==0)
			anAlign = HorzAlign_Left;
	}

	PushAlign(anAlign);
	AddDisplaySegment(aSeg,LineSegmentFlag_PreLineBreak | LineSegmentFlag_PostLineBreak);
	PopAlign();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleArea()
{
	if(mCurImageMap.get()==NULL)
		return;

	GUIString *aLink = GetParam("HREF");
	if(aLink==NULL)
		return;

	GUIString *aParamStr = GetParam("COORDS");
	if(aParamStr==NULL)
		return;

	string aParam = *aParamStr;

	int left=-1,top=-1,right=-1,bottom=-1;
	sscanf(aParam.c_str(),"%d,%d,%d,%d",&left,&top,&right,&bottom);
	if(left==-1 || top==-1 || right==-1 || bottom==-1)
		return;

	WONRectangle aRect(left,top,right-left,bottom-top);
	mCurImageMap->AddLink(new TextLinkParam(*aLink), aRect);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleBody()
{
	mInBody = true; 
	mHadSpaceOnEnd = true; // skip first space

	int aColor;
	if(mDisplayContainer.get()!=NULL)
	{
		aColor = GetColor(GetParam("BGCOLOR"));
		if(aColor>=0)
			mDisplayContainer->SetBackground(aColor);
	}
	
	aColor = GetColor(GetParam("TEXTCOLOR"));
	if(aColor>=0)
		mDefaultTextColor = aColor;

	aColor = GetColor(GetParam("LINK"));
	if(aColor>=0)
		mDefaultLinkColor = aColor;

	aColor = GetColor(GetParam("ALINK"));
	if(aColor>=0)
		mDefaultALinkColor = aColor;

//	mDefaultTextColor = 0xffffff;
	mFontStack.clear();
	PushFont(mDefaultFontFace.c_str(),3,-1);

	NativeImagePtr anImage = GetImage(GetParam("BACKGROUND"));
	if(anImage.get()!=NULL && mDisplayContainer.get()!=NULL)
	{
		// Pre-Tile the image a little if it's too small
		int aWidth = anImage->GetWidth();
		int aHeight = anImage->GetHeight();
		int anExtraWidth = 0;
		int anExtraHeight = 0;
		if(aWidth < 50)
		{
			if(50%aWidth==0)
				anExtraWidth = 50-aWidth;
			else
				anExtraWidth = (50/aWidth)*aWidth;
		}
		if(aHeight < 50)
		{
			if(50%aHeight==0)
				anExtraHeight = 50-aHeight;
			else
				anExtraHeight = (50/aHeight)*aHeight;
		}

		if(anExtraWidth>0 || anExtraHeight>0)
		{
			aWidth+=anExtraWidth;
			aHeight+=anExtraHeight;
			NativeImagePtr aBigImage = mWindow->CreateImage(aWidth,aHeight);
			Graphics &g = aBigImage->GetGraphics();
			for(int x=0; x<aWidth; x+=anImage->GetWidth())
				for(int y=0; y<aHeight; y+=anImage->GetHeight())
					g.DrawImage(anImage,x,y);

			anImage = aBigImage;
		}
		mDisplayContainer->SetBackground(anImage.get());
	}

	mLeftMargin = mRightMargin = mTopMargin = mBottomMargin = 5;
	if(GetIntParam("MARGINWIDTH",mLeftMargin))
		mRightMargin = mLeftMargin;
	if(GetIntParam("MARGINHEIGHT",mTopMargin))
		mBottomMargin = mTopMargin;

	GetIntParam("LEFTMARGIN",mLeftMargin);
	GetIntParam("RIGHTMARGIN",mRightMargin);
	GetIntParam("TOPMARGIN",mTopMargin);
	GetIntParam("BOTTOMMARGIN",mBottomMargin);

	if(mLeftMargin>0 || mRightMargin>0)
		AddDisplaySegment(new MarginLineSegment(mLeftMargin,mRightMargin));

	if(mTopMargin>0)
		AddDisplaySegment(new LineBreakSegment(mTopMargin),LineSegmentFlag_PostLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleStartTag()
{
//	mInBody = true;
	mInTagCount[mCurTagType]++;
	switch(mCurTagType)
	{
		case TagType_Body: 
			HandleBody(); 
			break;

		case TagType_Anchor: 
		{
			GUIString *aStr = GetParam("HREF");
			if(aStr!=NULL)
			{
				mInAnchor = true;
				mUnderlineCount++;
				PushFont(NULL,-1,mDefaultLinkColor);
				mCurLinkParam = new TextLinkParam(*aStr);
			}
			else
			{
				aStr = GetParam("NAME");
				if(aStr!=NULL)
					mDisplay->AddSegment(new AnchorLineSegment(*aStr));
			}
			break; 
		}

		case TagType_ImageMap:
		{
			GUIString *aName = GetParam("NAME");
			if(aName!=NULL)
			{
				ImageMapMap::iterator anItr = mImageMapMap.insert(ImageMapMap::value_type(*aName,NULL)).first;
				if(anItr->second.get()==NULL)
					anItr->second = new ImageMap;

				mCurImageMap = anItr->second;				
			}
			break;
		}

		case TagType_Area:
			HandleArea();
			break;

		case TagType_HorizontalRule:
			HandleHorizontalRule();
			break;

		case TagType_LineBreak: 
		{
			GUIString *aParam = GetParam("CLEAR");
			int anExtraFlags = 0;
			if(aParam!=NULL)
			{
				if(aParam->compareNoCase("LEFT")==0)
					anExtraFlags = LineSegmentFlag_BreakClearLeft;
				else if(aParam->compareNoCase("RIGHT")==0)
					anExtraFlags = LineSegmentFlag_BreakClearRight;
				else if(aParam->compareNoCase("ALL")==0)
					anExtraFlags = LineSegmentFlag_BreakClearLeft | LineSegmentFlag_BreakClearRight;
			}

			AddLineBreak(anExtraFlags);
			break;
		}

		case TagType_Paragraph: 
			if(mInParagraph)
				PopAlign();


			ClearAccumulatedSegment();
			AddParagraphBreak();

			mHadSpaceOnEnd = true;
			mInParagraph = true;
			CheckAlignment();
			break;

		case TagType_Div:
			if(dynamic_cast<BulletElement*>(mPrevSegment.get())==NULL)
			{
				ClearAccumulatedSegment();
				AddLineBreak();
				mHadSpaceOnEnd = true;
				CheckAlignment();
			}
			break;

		case TagType_Bold:		mBoldCount++; CalcFont(); break;
		case TagType_Italic:	mItalicCount++; CalcFont(); break;
		case TagType_Underline: mUnderlineCount++; CalcFont(); break;
		case TagType_H1:
		case TagType_H2:
		case TagType_H3:
		case TagType_H4:
		case TagType_H5:
		case TagType_H6:
		{
			AddParagraphBreak();
			CheckAlignment();
			mBoldCount++;
			PushFont("Times New Roman",TagType_H6 - mCurTagType + 1, -1);
			break;
		}
		case TagType_Small:
			PushFont(NULL,2, -1);
			break;

		case TagType_Big:
			PushFont(NULL,4, -1);
			break;

		case TagType_Code:
		{
			PushFont("Courier",-1,-1);
			break;
		}
		case TagType_OrderedList:
			HandleOrderedList();
			break;

		case TagType_UnorderedList:
			HandleUnorderedList();
			break;

		case TagType_ListItem:
			HandleListItem();
			break;

		case TagType_NoBreak:
			mInNoBreak = true;
			break;

		case TagType_Pre:
		{
			AddParagraphBreak();
			mInPre = true;
			PushFont("Courier",-1,-1);
			break;
		}

		case TagType_Image:
			HandleImage();
			break;

		case TagType_Font:
			HandleFont(false);
			break;

		case TagType_BaseFont:
			HandleFont(true);
			break;

		case TagType_Center:
			PushAlign(HorzAlign_Center);
			break;


		case TagType_Table:
			HandleTable();
			break;

		case TagType_TableRow:
			HandleTableRow();
			break;

		case TagType_TableData:
			HandleTableData(false);
			break;

		case TagType_TableHeader:
			HandleTableData(true);
			break;

		case TagType_Input:
			HandleInput();
			break;

		case TagType_Select:
			HandleSelect();
			break;

		case TagType_Option:
			HandleOption();
			break;

		case TagType_Style:
		{
			int aState = 0;
			bool done = false;
			while(!EndOfFile() && !done)
			{
				int aChar = toupper(GetChar());
				switch(aState)
				{
					case 0: if(aChar=='<') aState++; else aState = 0; break;
					case 1: if(aChar=='/') aState++; else aState = 0; break;
					case 2: if(aChar=='S') aState++; else aState = 0; break;
					case 3: if(aChar=='T') aState++; else aState = 0; break;
					case 4: if(aChar=='Y') aState++; else aState = 0; break;
					case 5: if(aChar=='L') aState++; else aState = 0; break;
					case 6: if(aChar=='E') done = true; else aState = 0; break;
				}
			}
			while(!EndOfFile() && GetChar()!='>')
			{
			}
		}
		break;

		case TagType_Script:
		{
			int aState = 0;
			bool done = false;
			while(!EndOfFile() && !done)
			{
				int aChar = toupper(GetChar());
				switch(aState)
				{
					case 0: if(aChar=='<') aState++; else aState = 0; break;
					case 1: if(aChar=='/') aState++; else aState = 0; break;
					case 2: if(aChar=='S') aState++; else aState = 0; break;
					case 3: if(aChar=='C') aState++; else aState = 0; break;
					case 4: if(aChar=='R') aState++; else aState = 0; break;
					case 5: if(aChar=='I') aState++; else aState = 0; break;
					case 6: if(aChar=='P') aState++; else aState = 0; break;
					case 7: if(aChar=='T') done = true; else aState = 0; break;
				}
			}
			while(!EndOfFile() && GetChar()!='>')
			{
			}
		}
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::HandleEndTag()
{
	if(--mInTagCount[mCurTagType] < 0)
	{		
		if(!mParsingTableData || 
			(mCurTagType!=TagType_TableRow && mCurTagType!=TagType_TableData && mCurTagType!=TagType_Table))		
		{
			mInTagCount[mCurTagType] = 0;
			return;
		}
	}

	switch(mCurTagType)
	{
		case TagType_Body: 
			if(mBottomMargin>0)
				AddDisplaySegment(new LineBreakSegment(mBottomMargin),LineSegmentFlag_PostLineBreak);
			mInBody = false; 
			break;
		
		case TagType_Anchor:	
			if(mInAnchor)
			{
				mInAnchor = false;
				mUnderlineCount--;
				PopFont();
			}
			break; 

		case TagType_ImageMap:
			mCurImageMap = NULL;
			break;

		case TagType_Bold:		mBoldCount--; CalcFont(); break;
		case TagType_Italic:	mItalicCount--; CalcFont(); break;
		case TagType_Underline: mUnderlineCount--; CalcFont(); break;
		case TagType_H1:
		case TagType_H2:
		case TagType_H3:
		case TagType_H4:
		case TagType_H5:
		case TagType_H6:
		{
			mBoldCount--;
			PopFont(); PopAlign();
			AddParagraphBreak();
			break;
		}
		case TagType_Big:
		case TagType_Small:
			PopFont();
			break;

		case TagType_Paragraph: 
			if(mInParagraph)
			{
				ClearAccumulatedSegment();
				PopAlign();
				AddParagraphBreak();

				mInParagraph = false;
				mHadSpaceOnEnd = true;
			}
			break;

		case TagType_Div:
			if(mLineType==0)
			{
				ClearAccumulatedSegment();
				PopAlign();
				AddLineBreak();
				mHadSpaceOnEnd = true;
			}
			break;
		
		case TagType_Code:
			PopFont();
			break;

		case TagType_NoBreak:
			mInNoBreak = false;
			break;

		case TagType_Pre:
			PopFont();
			mInPre = false;
			AddParagraphBreak();
			break;

		case TagType_OrderedList:
		case TagType_UnorderedList:
			HandleEndList();
			break;

		case TagType_Font:
			PopFont();
			break;

		case TagType_Center:
			PopAlign();
			break;

		case TagType_Table:
			if(mInTable)
			{
				mInTable = false;
				int aFlags = LineSegmentFlag_PreLineBreak;
				if(mCurTableAlign==1)
					aFlags |= LineSegmentFlag_FloatLeft;
				else if(mCurTableAlign==3)
					aFlags |= LineSegmentFlag_FloatRight;
				else 
				{
					if(mCurTableAlign==2)
						PushAlign(HorzAlign_Center);

//					aFlags |= LineSegmentFlag_PostLineBreak;
				}
				AddDisplaySegment(mCurTable,aFlags);
				if(mCurTableAlign==2)
					PopAlign();
				else if(mCurTableAlign==0)
					AddLineBreak();
			}
			else if(mParsingTableData)
				HandleEndTableData();

			break;

		case TagType_TableRow:
		case TagType_TableData:
			if(mParsingTableData && !mInTable)
				HandleEndTableData();
			break;

		case TagType_Input:
		case TagType_Select:
			if(mCurCombo.get()!=NULL)
			{
				AddAccumulatedSegment();
				mPrevSegment = new ComponentLineSegment(mCurCombo);
				mHadSpaceOnEnd = false;
				mCurCombo = NULL;
			}
			break;
	}
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::CopyFileInfo(const HTMLParser &from)
{
	mText = from.mText;
	mFile = from.mFile;
	mPath = from.mPath;
	mBackStr = from.mBackStr;
	mLineNum = from.mLineNum;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser::CopyTagInfo(const HTMLParser &from)
{
	mCurTagName = from.mCurTagName;
	mCurTagType = from.mCurTagType;
	mIsEndTag = from.mIsEndTag;
	mCurText = from.mCurText;
//	mHadLineBreak = from.mHadLineBreak;
//	mHadTextSpace = from.mHadTextSpace;
	mCurTagAttributes = from.mCurTagAttributes;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::Parse(const char *theFilePath, MultiLineElement *theDisplay)
{
	mPath = theFilePath;

	string aPath = mDocumentFetcher(theFilePath,"");
	if(aPath.empty())
		return false;
	
	mFile = fopen(aPath.c_str(),"r");
	if(mFile==NULL)
		return false;

	bool aRet = UserParse(theDisplay);

	CloseFile();
	return aRet;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ParseText(const char *theText, MultiLineElement *theDisplay)
{
	mText = theText;
	mPath = "";

	bool aRet = UserParse(theDisplay);

	mText = NULL;

	return aRet;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ParseRichText(const char *theText, int theDefaultColor, Font *theDefaultFont, MultiLineElement *theDisplay)
{
	mRichTextParse = true;
	mDefaultTextColor = theDefaultColor;
	if(theDefaultFont!=NULL)
	{
		const FontDescriptor &aDesc = theDefaultFont->GetDescriptor();
		mDefaultFontFace = aDesc .mFace;
		mBoldCount = aDesc.mSize&FontStyle_Bold?1:0;
		mItalicCount = aDesc.mSize&FontStyle_Italic?1:0;
		mUnderlineCount = aDesc.mSize&FontStyle_Underline?1:0;
	}
		

	bool aRet = ParseText(theText,theDisplay);
	mRichTextParse = false;
	return aRet;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::UserParse(MultiLineElement *theDisplay)
{
	if(theDisplay!=NULL)
		mDisplay = theDisplay;

	if(mDisplay==NULL)
		return false;

	mWindow = WindowManager::GetDefaultWindowManager()->GetDefaultWindow();
	mLineNum = 1;

	mBackStr.erase();
	mPrevSegment = NULL;
	mLastWord.erase();
	mListStack.clear();
	mFontStack.clear();
	mAlignStack.clear();
	mImageMapMap.clear();
	mLoadedImageMap.clear();
	mScaledImageMap.clear();

	mDefaultLinkColor = 0x0000ff;
	mDefaultALinkColor = 0xff0000;
	if(!mRichTextParse)
	{
		mDefaultTextColor = 0x000000;
		mDefaultFontFace = "Times New Roman";
		mBoldCount = mItalicCount = mUnderlineCount = 0;
	}
	mDefaultAlign = HorzAlign_Left;
	mParsingTableData = false;
	bool aRet = Parse();

	LineSegment::mCounter = 0;
	mDisplay->SetSelIndex();

	return aRet;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::ParseTableData(HTMLParser &theParser, MultiLineElement *theDisplay, bool isHeader,  HorzAlign theStartAlign)
{
	CopyFileInfo(theParser);
	mDisplay = theDisplay;
	mWindow = theParser.mWindow;
	mDefaultLinkColor = theParser.mDefaultLinkColor;
	mDefaultALinkColor = theParser.mDefaultALinkColor;
	mDefaultTextColor = theParser.mDefaultTextColor;
	mDefaultFontFace = theParser.mDefaultFontFace;
	mDefaultAlign = theStartAlign;
	mBoldCount = mItalicCount = mUnderlineCount = 0;
	mDocumentFetcher = theParser.mDocumentFetcher;
	mImageFetcher = theParser.mImageFetcher;
	mPath = theParser.mPath;
	if(isHeader)
	{
		mBoldCount++;
//		PushAlign(HorzAlign_Center);
	}

	mParsingTableData = true;
	mInBody = true;
	bool aRet = Parse();

	theParser.CopyFileInfo(*this);

	return aRet;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser::Parse()
{

	memset(mInTagCount,0,sizeof(int)*TagType_Max);
	mHadLineBreak = false;
	mHadParagraphBreak = true;
	mHadSpaceOnEnd = true;
	mInAnchor = false;
	mInBody = mParsingTableData || mRichTextParse;
	mInPre = false;
	mInNoBreak = false;
	mDone = false;
	mInTable = false;
	mInParagraph = false;
	mLeftMargin = mRightMargin = mTopMargin = mBottomMargin = 0;
	mLineType = 0;
	mCurTable = NULL;
	PushFont(mDefaultFontFace.c_str(),3,-1);
	if(mDefaultAlign!=HorzAlign_Left)
		PushAlign(mDefaultAlign,true);

	while(!mDone && !EndOfFile())
	{
		if(!GetNextTag())
			break;

/*		char aBuf[50];
		sprintf(aBuf,"%d\n",mLineNum);
		OutputDebugString(aBuf);*/
		if(mIsEndTag)
			HandleEndTag();
		else
			HandleStartTag();
	}

	ClearAccumulatedSegment();
	return true;
}
