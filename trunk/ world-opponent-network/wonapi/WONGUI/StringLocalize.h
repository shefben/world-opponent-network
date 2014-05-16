#ifndef __WON_STRINGLOCALIZE_H__
#define __WON_STRINGLOCALIZE_H__
#include "WONGUI/GUIString.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TextArea;
class TextLink;
class Font;


class StringLocalize 
{
public:
	static GUIString GetStr(const GUIString &theStr, const GUIString **subs, int numSubs);

	static GUIString GetStr(const GUIString &theStr, const GUIString &s1);
	static GUIString GetStr(const GUIString &theStr, const GUIString &s1, const GUIString &s2);
	static GUIString GetStr(const GUIString &theStr, const GUIString &s1, const GUIString &s2, const GUIString &s3);

	static void AddText(TextArea *theTextArea, const GUIString &theStr, 
		const GUIString **strings, int nstrings, 
		int *colors, int ncolors, 
		Font **fonts, int nfonts,
		TextLink **links, int nlinks, bool doLineBreak = false);
};

};

#endif