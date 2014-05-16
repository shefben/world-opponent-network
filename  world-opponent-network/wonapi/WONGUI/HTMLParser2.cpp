#include "HTMLParser2.h"
#include "jpeglib/JPEGDecoder.h"
#include "pnglib/PNGDecoder.h"
#include "GIFDecoder.h"
#include "Window.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLParser2::HTMLParser2(TextArea *theDisplay)
{
	mDisplay = theDisplay;
	mFile = NULL;
	mLineNum = 1;

	mTagNameMap["A"] = TagType_Anchor;
	mTagNameMap["P"] = TagType_Paragraph;
	mTagNameMap["BR"] = TagType_LineBreak;
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
	mTagNameMap["LI"] = TagType_ListItem;
	mTagNameMap["IMG"] = TagType_Image;
	mTagNameMap["PRE"] = TagType_Pre;
	mTagNameMap["BODY"] = TagType_Body;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::CloseFile()
{
	if(mFile!=NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::EndOfFile()
{	
	if(!feof(mFile)) 
		return false; 
	else return 
		mBackStr.empty(); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser2::GetChar()
{
	int aChar = 0;
	if(mBackStr.empty())
		aChar = fgetc(mFile);
	else
	{
		aChar = mBackStr[mBackStr.length()-1];
		mBackStr.resize(mBackStr.length()-1);
	}

	if(aChar=='\n')
		mLineNum++;

	return aChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLParser2::PeekChar()
{
	if(mFile && !EndOfFile())
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
void HTMLParser2::UngetChar(int theChar)
{ 
	if(theChar==EOF)
		return;

	if(theChar=='\n')
		mLineNum--;

	mBackStr+=theChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::UngetString(const std::string &theStr)
{
	for(int i=theStr.length()-1; i>=0; i--)
	{
		UngetChar(theStr[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::SkipWhitespace()
{
	bool hadSpace = false;
	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(!isspace(aChar))
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
int HTMLParser2::ReadEntity()
{
	string anEntity;
	int i;
	for(i=0; i<20 && !EndOfFile(); i++)
	{
		int aChar = GetChar();
		anEntity+=aChar;
		if(aChar==';')
			break;
	}

	if(i==20)
	{
		UngetString(anEntity);
		return -1;
	}

	if(anEntity[0]=='#')
	{
		anEntity.resize(anEntity.length()-1);
		int anAsciiValue = atoi(anEntity.c_str()+1);
		if(anAsciiValue==0)
			anAsciiValue = -1;

		return anAsciiValue;
	}

	if(stricmp(anEntity.c_str(),"GT;")==0)
		return '>';
	if(stricmp(anEntity.c_str(),"LT;")==0)
		return '<';
	if(stricmp(anEntity.c_str(),"AMP;")==0)
		return '&';
	if(stricmp(anEntity.c_str(),"QUOT;")==0)
		return '"';
	if(stricmp(anEntity.c_str(),"NBSP;")==0)
		return '\xa0';

		
	UngetString(anEntity);
	return -1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::ReadWord(std::string &theWord,  const char *theStopChars, bool stopAtSpace, bool skip, bool erase)
{
	if(erase)
		theWord.erase();

	if(stopAtSpace)
		SkipWhitespace();

	while(!EndOfFile())
	{
		int aChar = GetChar();
		if((stopAtSpace && isspace(aChar))  || (theStopChars!=NULL && strchr(theStopChars,aChar)))
		{
			UngetChar(aChar);
			break;
		}

		if(aChar=='&')
		{
			aChar = ReadEntity();
			if(!skip)
			{
				if(aChar!=-1)
					theWord += aChar;
				else
					theWord += '&';
			}
		}
		else
		{
			if(!skip)
				theWord += aChar;
		}
	}

	return !theWord.empty();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::GetTextTag()
{
	mCurTagName = "_TEXT";
	mCurTagType = TagType_Text;
	mIsEndTag = false;
	mCurText.erase();

	bool needSpace = false;
	std::string aWord;
	if(mHadTextSpace && !mHadLineBreak)
		needSpace = true;

	mHadTextSpace = false;
	mHadLineBreak = false;

	if(mInPre)
	{
		bool erase = true;
		bool done = false;
		while(!EndOfFile() && !done)
		{
			ReadWord(aWord,"<\n\t",false,false,erase);
			char aChar = GetChar();
			if(aChar=='<')
			{
				UngetChar(aChar);
				done = true;
			}
			else if(aChar=='\t')
			{
				int aNumSpaces = 8 - aWord.length()%8;
				for(int i=0; i<aNumSpaces; i++)
				aWord+=' ';
				erase = false;
				continue;
			}
			else 
			{
				erase = true;
			}
			
			if(mInAnchor)
				mDisplay->AddLink(aWord.c_str(),mCurLinkParam.c_str(),!done);
			else
				mDisplay->AddSegment(aWord.c_str(),!done);
		}
		return true;
	}

	while(ReadWord(aWord,"<"))
	{
		if(needSpace)
			mCurText += ' ';
		
		mCurText += aWord;
		mHadTextSpace = isspace(PeekChar())?true:false;
		needSpace = true;
	}

	if(mCurText.length()==1 && mCurText[0]==' ')
		mCurText.erase();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::GetActualTag()
{
	mCurTagAttributes.clear();
	mIsEndTag = false;

	if(PeekChar()=='/')
	{
		GetChar(); // skip '/'
		mIsEndTag = true;
	}

	ReadWord(mCurTagName,">");

	TagNameMap::iterator anItr = mTagNameMap.find(mCurTagName);
	if(anItr==mTagNameMap.end()) // unrecognized tag... just skip it
	{
		mCurTagType = TagType_None;
		std::string aSkip;
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

		std::string anAttribute;
		std::string aValue;

		ReadWord(anAttribute,">=");
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
			else
				ReadWord(aValue,">");

			mCurTagAttributes[anAttribute] = aValue;
		}

	}

	return true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::GetNextTag()
{
	bool hadSpace = mInPre?false:SkipWhitespace();
	int aChar = GetChar();
	if(aChar=='<')
		return GetActualTag();
	else
	{
		mHadTextSpace = mHadTextSpace || hadSpace;
		UngetChar(aChar);
		return GetTextTag();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::CalcFont()
{
	FontDescriptor aDescriptor = mCurFont->GetDescriptor();
	if(mBoldCount<=0 && mHeaderCount<=0)
	{
		mBoldCount = 0;
		mHeaderCount = 0;
		aDescriptor.mStyle &= ~FontStyle_Bold;
	}
	else
		aDescriptor.mStyle |= FontStyle_Bold;

	if(mItalicCount<=0)
	{
		mItalicCount = 0;
		aDescriptor.mStyle &= ~FontStyle_Italic;
	}
	else
		aDescriptor.mStyle |= FontStyle_Italic;

	if(mUnderlineCount<=0)
	{
		mItalicCount = 0;
		aDescriptor.mStyle &= ~FontStyle_Underline;
	}
	else
		aDescriptor.mStyle |= FontStyle_Underline;

	mCurFont = mDisplay->GetFont(aDescriptor);
	mDisplay->SetFont(mCurFont);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string* HTMLParser2::GetParam(const char *theName)
{
	TagAttributes::iterator anItr = mCurTagAttributes.find(theName);
	if(anItr==mCurTagAttributes.end())
		return NULL;

	return &anItr->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::CheckAlignment()
{
	std::string *anAlignStr = GetParam("ALIGN");
	if(anAlignStr==NULL)
		return;

	if(StringCompareNoCase(*anAlignStr,"CENTER")==0)
		mDisplay->SetLineAlignment(HorzAlign_Center);
	else if(StringCompareNoCase(*anAlignStr,"RIGHT")==0)
		mDisplay->SetLineAlignment(HorzAlign_Right);
	else
		mDisplay->SetLineAlignment(HorzAlign_Left);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::DoLineBreak()
{
	mHadLineBreak = true;
	mDisplay->LineBreak(10); 
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::HandleImage()
{
	string *anImagePath = GetParam("SRC");
	if(anImagePath==NULL)
		return false;

	string aPath = mPath + *anImagePath;

//	std::string aTrick = "puke.jpg";
//	anImagePath = &aTrick;
//	string aPath = aTrick;

	char *anExt = NULL;
	anExt = strrchr(anImagePath->c_str(),'.');
	if(anExt==NULL)
		return false;

	anExt++;
	NativeImagePtr anImage;
	DisplayContext *aContext = mDisplay->GetWindow()->GetDisplayContext();
	if(stricmp(anExt,"jpg")==0 || stricmp(anExt,"jpeg")==0)
		anImage = JPEGDecoder::Decode(aContext,aPath.c_str());
	else if(stricmp(anExt,"png")==0)
		anImage = PNGDecoder::Decode(aContext,aPath.c_str());
	else if(stricmp(anExt,"gif")==0)
	{
		GIFDecoder aDecoder;
		anImage = aDecoder.Decode(aContext,aPath.c_str());
	}

	if(anImage.get()==NULL)
		return false;

	mDisplay->AddImage(anImage,"");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::HandleStartTag()
{
	switch(mCurTagType)
	{
		case TagType_Body: mInBody = true; break;
		case TagType_Anchor: 
		{
			std::string *aStr = GetParam("HREF");
			if(aStr!=NULL)
			{
				mInAnchor = true;
				mCurLinkParam = *aStr;
			}
			else
			{
				aStr = GetParam("NAME");
				if(aStr!=NULL)
					mDisplay->AddSegment(new TextAnchorInstruction(*aStr));
			}
			break; 
		}
		case TagType_Text: 
			if(!mCurText.empty() && mInBody) 
			{
				if(mInAnchor)
				{
					if(isspace(mCurText[0]))
					{
						mDisplay->AddSegment(" ");
						mDisplay->AddLink(mCurText.c_str()+1,mCurLinkParam.c_str());
					}
					else
						mDisplay->AddLink(mCurText.c_str(),mCurLinkParam.c_str());
				}
				else
					mDisplay->AddSegment(mCurText.c_str()); 
			}
			break;

		case TagType_LineBreak: DoLineBreak(); break;
		case TagType_Paragraph: DoLineBreak(); DoLineBreak(); break;
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
			mHeaderCount++;
			DoLineBreak(); DoLineBreak();
			CheckAlignment();
			FontDescriptor aDescriptor("TimesRoman",FontStyle_Bold,6 + (TagType_H6 - mCurTagType)*3);
			mCurFont = mDisplay->GetFont(aDescriptor);
			mFontStack.push_back(mCurFont);
			CalcFont();
			break;
		}
		case TagType_Code:
		{
			FontDescriptor aDescriptor = mCurFont->GetDescriptor();
			aDescriptor.mFace = "Courier";
			mCurFont = mDisplay->GetFont(aDescriptor);
			mFontStack.push_back(mCurFont);
			CalcFont();
			break;
		}
		case TagType_ListItem:
			DoLineBreak();
			mDisplay->AddSegment(new TextWidthInstruction(50));
			break;

		case TagType_Pre:
		{
			if(!mHadLineBreak)
				DoLineBreak(); 

			mInPre = true;
			FontDescriptor aDescriptor = mCurFont->GetDescriptor();
			aDescriptor.mFace = "Courier";
			mCurFont = mDisplay->GetFont(aDescriptor);
			mFontStack.push_back(mCurFont);
			CalcFont();
			break;
		}

		case TagType_Image:
			HandleImage();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLParser2::HandleEndTag()
{
	switch(mCurTagType)
	{
		case TagType_Body: mInBody = false; break;
		case TagType_Anchor:	mInAnchor = false; break; 
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
			mHeaderCount--;
			if(!mFontStack.empty())
			{
				mFontStack.pop_back();
				mCurFont = mFontStack.back();
				CalcFont();
				DoLineBreak(); DoLineBreak();
				mDisplay->SetLineAlignment(HorzAlign_Left);
			}
			break;
		}
		case TagType_Code:
		{
			if(!mFontStack.empty())
			{
				mFontStack.pop_back();
				mCurFont = mFontStack.back();
				CalcFont();
			}
			break;
		}
		case TagType_Pre:
		{
			if(!mFontStack.empty())
			{
				mFontStack.pop_back();
				mCurFont = mFontStack.back();
				CalcFont();
			}
			mInPre = false;
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLParser2::Parse(const char *theFilePath, TextArea *theDisplay)
{
	if(theDisplay!=NULL)
		mDisplay = theDisplay;

	if(mDisplay==NULL)
		return false;

	CloseFile();
	mFile = fopen(theFilePath,"r");
	if(mFile==NULL)
		return false;

	mPath = theFilePath;
	int aPos = mPath.find_last_of("\\");
	if(aPos==string::npos)
		aPos = mPath.find_last_of("/");

	if(aPos!=string::npos)
		mPath.resize(aPos+1);
	else
		mPath.erase();

	mDisplay->Clear();
	mDisplay->SetLineAlignment(HorzAlign_Left);
	mDisplay->SetWrapIndentSize(0);
	mCurFont = mDisplay->GetFont(FontDescriptor("TimesRoman",FontStyle_Plain,10));
	mFontStack.push_back(mCurFont);

	mLineNum = 1;
	mBoldCount = mItalicCount = mUnderlineCount = mHeaderCount = 0;
	mHadLineBreak = mHadTextSpace = false;
	mInAnchor = false;
	mInBody = false;
	mInPre = false;

	while(!EndOfFile())
	{
		if(!GetNextTag())
			break;

		if(mIsEndTag)
			HandleEndTag();
		else
			HandleStartTag();
	}

	CloseFile();
	return true;
}
