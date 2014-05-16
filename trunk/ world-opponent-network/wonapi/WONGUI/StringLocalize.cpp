#include "StringLocalize.h"
#include "TextBox.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString StringLocalize::GetStr(const GUIString &theStr, const GUIString **subs, int numSubs)
{
	GUIString aNewStr;
	int aStartPos = 0;
	while(true)
	{
		int aBracketPos = theStr.find('<',aStartPos);
		if(aBracketPos<0)
			break;

		aNewStr.append(theStr,aStartPos,aBracketPos-aStartPos);
		int aNextChar = theStr.at(aBracketPos+1);
		if(aNextChar=='<') // literal '<' character
		{
			aNewStr.append('<');
			aStartPos = aBracketPos+2;
			continue;
		}
		else if(aBracketPos+1<theStr.length()) // substitution
		{
			int aNextNextChar = theStr.at(aBracketPos+2);
			int aSubId = aNextNextChar - '0' - 1;
			if(tolower(aNextChar)=='s' && aSubId>=0 && aSubId<numSubs && subs[aSubId]!=NULL)
				aNewStr.append(*(subs[aSubId]));
		}

		aStartPos = aBracketPos+1;
		aBracketPos = theStr.find('>',aStartPos); // find ending bracket
		if(aBracketPos<0)
			break;
			
		aStartPos = aBracketPos+1;
	}

	if(aStartPos < theStr.length())
		aNewStr.append(theStr,aStartPos);

	return aNewStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString StringLocalize::GetStr(const GUIString &theStr, const GUIString &s1)
{
	const GUIString *anArray[] = {&s1};
	return GetStr(theStr,anArray,1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString StringLocalize::GetStr(const GUIString &theStr, const GUIString &s1, const GUIString &s2)
{
	const GUIString *anArray[] = {&s1,&s2};
	return GetStr(theStr,anArray,2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString StringLocalize::GetStr(const GUIString &theStr, const GUIString &s1, const GUIString &s2, const GUIString &s3)
{
	const GUIString *anArray[] = {&s1,&s2,&s3};
	return GetStr(theStr,anArray,3);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StringLocalize::AddText(TextArea *theTextArea, const GUIString &theStr, 
		const GUIString **strings, int nstrings, 
		int *colors, int ncolors, 
		Font **fonts, int nfonts,
		TextLink **links, int nlinks,
		bool doLineBreak)
{
	int aColor = theTextArea->GetTextColor();
	Font *aFont = theTextArea->GetTextFont();

	
	GUIString aNewStr;
	int aStartPos = 0;
	while(true)
	{
		int aBracketPos = theStr.find('<',aStartPos);
		if(aBracketPos<0)
			break;

		aNewStr.append(theStr,aStartPos,aBracketPos-aStartPos);
		int aNextChar = theStr.at(aBracketPos+1);
		if(aNextChar=='<') // literal '<' character
		{
			aNewStr.append('<');
			aStartPos = aBracketPos+2;
			continue;
		}
		else if(aBracketPos+1<theStr.length()) // substitution
		{
			if(!aNewStr.empty())
			{
				theTextArea->AddSegment(aNewStr,aColor,aFont);
				aNewStr.erase();
			}

			int aNextNextChar = theStr.at(aBracketPos+2);
			int aSubId = aNextNextChar - '0' - 1;

			switch(tolower(aNextChar))
			{
				case 'n': 
					theTextArea->LineBreak(); 
					break;

				case 'c':
					if(aSubId>=0 && aSubId<ncolors)
						aColor = colors[aSubId];
					break;

				case 'f':
					if(aSubId>=0 && aSubId<nfonts && fonts[aSubId]!=NULL)
						aFont = fonts[aSubId];
					break;

				case 's': 
					if(aSubId>=0 && aSubId<nstrings && strings[aSubId]!=NULL)
						theTextArea->AddSegment(*(strings[aSubId]),aColor,aFont);
					break;

				case 'l':
					if(aSubId>=0 && aSubId<nlinks && links[aSubId]!=NULL)
						theTextArea->AddSegment(links[aSubId]);
					break;
			}
		}

		aStartPos = aBracketPos+1;
		aBracketPos = theStr.find('>',aStartPos); // find ending bracket
		if(aBracketPos<0)
			break;
			
		aStartPos = aBracketPos+1;
	}

	if(aStartPos < theStr.length())
		aNewStr.append(theStr,aStartPos);

	if(!aNewStr.empty() || doLineBreak)
		theTextArea->AddSegment(aNewStr,aColor,aFont, doLineBreak);
}
