#include "HTMLDocumentGen.h"
#include "HTMLParser.h"
#include "HTMLFromText.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocumentGen::HTMLDocumentGen(HTMLDocument *theDocument)
{
	SetDocument(theDocument);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLDocumentGen::AddDocPrv(const GUIString &theText, int theFlags, bool addToVector)
{
	HTMLDocumentPtr aDocument;
	HTMLFromText aGen;
	aDocument = aGen.Generate(theText,theFlags);
	
	if(addToVector)
		mDocumentVector.push_back(aDocument);
	else
		mDocument->AddDocument(aDocument);

	return aDocument.get()!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLDocumentGen::AddDoc(const GUIString &theText, int theFlags)
{
	return AddDocPrv(theText,theFlags,false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLDocumentGen::AddDocParam(const GUIString &theText, int theFlags)
{
	return AddDocPrv(theText,theFlags,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocumentGen::AddTag(HTMLTag *theTag)
{
	if(theTag!=NULL)
		mDocument->AddTag(theTag);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocumentGen::AddTagParam(HTMLTag *theTag)
{
	HTMLDocumentPtr aDoc = new HTMLDocument;
	if(theTag!=NULL)
		aDoc->AddTag(theTag);

	mDocumentVector.push_back(aDoc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocumentGen::printf(const GUIString &theText)
{
	int aStartPos = 0;
	while(true)
	{
		int anEndPos = theText.find('%',aStartPos);
		if(anEndPos<0)
			break;

		if(theText.at(anEndPos+1)=='%')
		{
			anEndPos++;
			AddDoc(theText.substr(aStartPos,anEndPos-aStartPos),HTMLDocGenFlag_IsHTML);
			aStartPos = anEndPos+1;
			continue;
		}
			
		AddDoc(theText.substr(aStartPos,anEndPos-aStartPos),HTMLDocGenFlag_IsHTML);

		anEndPos++;
		aStartPos = anEndPos;
		while(anEndPos<theText.length() && theText.at(anEndPos)!='%')
			anEndPos++;

		GUIString aNumber = theText.substr(aStartPos,anEndPos-aStartPos);
		int aParamNum = aNumber.atoi()-1;
		if(aParamNum>=0 && aParamNum<mDocumentVector.size())
			mDocument->AddDocument(mDocumentVector[aParamNum]);

		aStartPos = anEndPos+1;
		if(aStartPos>=theText.length())
			break;
	}

	if(aStartPos<theText.length())
		AddDoc(theText.substr(aStartPos),HTMLDocGenFlag_IsHTML);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocumentGen::SetDocument(HTMLDocument *theDocument)
{
	if(theDocument==NULL)
		mDocument = new HTMLDocument;
	else
		mDocument = theDocument;
}
