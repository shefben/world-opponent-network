#ifndef __WON_HTMLDOCUMENTGEN_H__
#define __WON_HTMLDOCUMENTGEN_H__

#include "HTMLDocument.h"

namespace WONAPI
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum HTMLDocGenFlags
{
	HTMLDocGenFlag_IsHTML					=			0x0001,
	HTMLDocGenFlag_IsFile					=			0x0002,
	HTMLDocGenFlag_IsOptionalHTML			=			0x0004, // If first character is '<' then it's html, otherwise it's text
	HTMLDocGenFlag_SkipFirstCharIfNotHTML	=			0x0008  // pretty much a hack for MOTD
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTMLDocumentGen
{
protected:
	HTMLDocumentPtr mDocument;

	typedef std::vector<HTMLDocumentPtr> DocumentVector;
	DocumentVector mDocumentVector;

	bool AddDocPrv(const GUIString &theText, int theFlags, bool addToVector);


public:
	HTMLDocumentGen(HTMLDocument *theDocument = NULL);

	bool AddDoc(const GUIString &theText, int theFlags = 0);
	void AddTag(HTMLTag *theTag);

	bool AddDocParam(const GUIString &theText, int theFlags = 0);
	void AddTagParam(HTMLTag *theTag);

	void SetDocument(HTMLDocument *theDocument);
	HTMLDocument* GetDocument() { return mDocument; }

	void printf(const GUIString &theText);
};


};

#endif
