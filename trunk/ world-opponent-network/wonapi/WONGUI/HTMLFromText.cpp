#include "HTMLFromText.h"
#include "HTMLDocumentGen.h"
#include "HTMLParser.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLFromText::HTMLFromText()
{
	mFile = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLFromText::CloseFile()
{
	mBackStr.erase();
	if(mFile!=NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLFromText::EndOfFile()
{	
	if(!mFile || feof(mFile))
		return mBackStr.empty();
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLFromText::GetChar()
{
	int aChar = 0;
	if(mBackStr.empty())
	{
		if(mFile)
			aChar = fgetc(mFile);
		else 
			aChar = -1;
	}
	else
	{
		aChar = mBackStr.at(mBackStr.length()-1);
		mBackStr.resize(mBackStr.length()-1);
	}

	return aChar;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
static const char* gProtocols[] = { "HTTP://", "HTTPS://", "FTP://", "MAILTO:", "WWW." };
static const char* gProtocolPrepends[] = { "", "", "", "", "HTTP://" };	
static int gNumProtocols = sizeof(gProtocols)/sizeof(const char*);
*/

static const GUIString gProtocols[] = { "HTTP://", "HTTPS://", "FTP://", "MAILTO:", "WWW." };
static const GUIString gProtocolPrepends[] = { "", "", "", "", "HTTP://" };	
static int gNumProtocols = sizeof(gProtocols)/sizeof(GUIString);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLFromText::StartsWith(const GUIString &s1, int p1, const GUIString &s2, int p2)
{
	int c1, c2;
	do
	{
		c1 = towupper(s1.at(p1++));
		c2 = towupper(s2.at(p2++));
		if(c2=='\0')
			return true;
		else if(c1!=c2)
			return false;

	} while(c1!='\0');

	return false;
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool HTMLFromText::CheckURL(GUIString &aStr, int &aPos)
{
	for(int i=0; i<gNumProtocols; i++)
	{
		if(StartsWith(aStr,aPos,gProtocols[i],0) &&
		   (aPos==0 || aStr.at(aPos-1)== ' '))
		{
			int anEndPos = aPos;
			int anItr = aPos;
			while(anItr<aStr.length())
			{
				int aChar = aStr.at(anItr);
				if(aChar>='0' && aChar<='9' || aChar>='a' && aChar<='z' || aChar>='A' && aChar<='Z')
					anEndPos = anItr+1;
				else if(aChar==' ')
					break;
				
				anItr++;
			}
			
			if(aPos>0)
				mDocument->AddTag(new HTMLTextTag(aStr.substr(0,aPos)));

			HTMLAnchorTag *anAnchor = new HTMLAnchorTag;
			GUIString aLink = aStr.substr(aPos,anEndPos-aPos);
			GUIString anHRef = gProtocolPrepends[i];
			anHRef.append(aLink);

			anAnchor->mHRef = anHRef;
			mDocument->AddTag(anAnchor);
			mDocument->AddTag(new HTMLTextTag(aLink));

			anAnchor = new HTMLAnchorTag;
			anAnchor->SetFlags(HTMLTagFlag_IsEndTag,true);
			mDocument->AddTag(anAnchor);

			aStr = aStr.substr(anEndPos);
			aPos = 0;
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLFromText::AddStr(GUIString &theStr)
{
	int aPos = 0;

	while(aPos<theStr.length())
	{
		if(CheckURL(theStr,aPos))
			continue;
		
//		if(CheckEmoticon(aStr,aPos))
//			continue;

//		if(CheckFontInstruction(aStr,aPos,aCurFont))
//			continue;

		// check change color instruction
//		if(CheckChangeColor(aStr,aPos))
//			continue;

		aPos++;
	}

	if(!theStr.empty())
		mDocument->AddTag(new HTMLTextTag(theStr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument* HTMLFromText::Generate(bool wantBody)
{
	mDocument = new HTMLDocument;

	if(wantBody)
		mDocument->AddTag(new HTMLBodyTag);


	GUIString mCurStr(32);

	while(!EndOfFile())
	{
		int aChar = GetChar();
		if(aChar=='\n')
		{
			if(!mCurStr.empty())
			{
				AddStr(mCurStr);
				mCurStr.erase();
				mCurStr.reserve(32);
			}
			mDocument->AddTag(new HTMLLineBreakTag);
		}
		else if(aChar!=-1)
			mCurStr.append(aChar);
		else 
			break;
	}

	if(!mCurStr.empty())
		AddStr(mCurStr);
	
	if(wantBody)
	{
		HTMLBodyTag *aTag = new HTMLBodyTag;
		aTag->SetFlags(HTMLTagFlag_IsEndTag,true);
		mDocument->AddTag(aTag);
	}

	CloseFile();

	return mDocument;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument* HTMLFromText::GenerateFromFile(const GUIString &theFilePath, int theFlags)
{
	CloseFile();
	if(theFilePath.empty())
		return NULL;

	std::string aFilePath = theFilePath;
	mFile = fopen(aFilePath.c_str(),"r");
	if(mFile==NULL)
		return NULL;

	bool isHTML = theFlags&HTMLDocGenFlag_IsHTML?true:false;
	if(theFlags & HTMLDocGenFlag_IsOptionalHTML)
	{
		int aFirstChar = fgetc(mFile);
		if(aFirstChar=='<')
		{
			CloseFile();
			isHTML = true;
		}
		else if(!(theFlags & HTMLDocGenFlag_SkipFirstCharIfNotHTML)) // hack for MOTD
			ungetc(aFirstChar,mFile);
	}

	if(isHTML)
	{
		HTMLParser aParser;
		aParser.ParseFromFile(theFilePath);
		mDocument = aParser.GetDocument();
		return mDocument;
	}


	return Generate(false);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument* HTMLFromText::GenerateFromText(const GUIString &theText, int theFlags)
{
	CloseFile();
	bool isHTML = theFlags&HTMLDocGenFlag_IsHTML?true:false;
	if((theFlags & HTMLDocGenFlag_IsOptionalHTML) && !theText.empty() && theText.at(0)=='<')
		isHTML = true;

	if(isHTML)
	{
		HTMLParser aParser;
		aParser.ParseFromText(theText);
		mDocument = aParser.GetDocument();
		return mDocument;
	}

	mBackStr.resize(0);
	mBackStr.reserve(theText.length());
	for(int i = theText.length()-1; i>=0; i--)
		mBackStr.append(theText.at(i));

	if(theFlags & HTMLDocGenFlag_IsOptionalHTML) 
	{
		if((theFlags & HTMLDocGenFlag_SkipFirstCharIfNotHTML) && mBackStr.length()>0) // hack for MOTD
			mBackStr.resize(mBackStr.length()-1);
	}


	return Generate(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument* HTMLFromText::Generate(const GUIString &theText, int theFlags)
{
	if(theFlags & HTMLDocGenFlag_IsFile)
		return GenerateFromFile(theText, theFlags);
	else
		return GenerateFromText(theText, theFlags);
}
