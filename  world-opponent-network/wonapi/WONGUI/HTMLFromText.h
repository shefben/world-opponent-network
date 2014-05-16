#ifndef __WON_HTMLFROMTEXT_H__
#define __WON_HTMLFROMTEXT_H__

#include "HTMLDocument.h"

namespace WONAPI
{

class HTMLFromText 
{
protected:
	HTMLDocumentPtr mDocument;
	FILE *mFile;
	GUIString mBackStr;

	int GetChar();
	bool EndOfFile();
	void CloseFile();
	HTMLDocument* Generate(bool wantBody);

	bool StartsWith(const GUIString &s1, int p1, const GUIString &s2, int p2);
	bool CheckURL(GUIString &aStr, int &aPos);
	void AddStr(GUIString &theStr);

	HTMLDocument* GenerateFromFile(const GUIString &theFilePath, int theFlags);
	HTMLDocument* GenerateFromText(const GUIString &theText, int theFlags);

public:
	HTMLFromText();

	// mayBeHTML means if 1st character is '<' then parse as html instead of text
	HTMLDocument* Generate(const GUIString &theText, int theFlags = 0); // the flags are defined in HTMLDocumentGen.h
};


}; // namespace WONAPI


#endif