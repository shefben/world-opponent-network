#include "Clipboard.h"
#include "GUISystem.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString Clipboard::Get()
{
	GUIString aStr;

	OpenClipboard(NULL);
	HANDLE aHandle = GetClipboardData(CF_UNICODETEXT);
	if(aHandle==NULL)
	{
		aHandle = GetClipboardData(CF_TEXT);
		if(aHandle!=NULL)
			aStr = GUIString((char*)aHandle);
	}
	else
		aStr = GUIString((wchar_t*)aHandle);

	CloseClipboard();	

	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Clipboard::Set(const GUIString &theStr)
{
    HGLOBAL hData;
    LPVOID pData;
    OpenClipboard(NULL);
    EmptyClipboard();

	static bool checkedUnicode = false;
	static bool supportUnicode = false;
	if(!checkedUnicode)
	{
		checkedUnicode = true;

		hData = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, 2);
		pData = GlobalLock(hData);
		memcpy((void*)pData,L"",2);

		supportUnicode = SetClipboardData(CF_UNICODETEXT,hData)!=NULL?true:false;
		GlobalUnlock(hData);
	}


	int aLength = theStr.length()+1;
	int aCharSize = supportUnicode?sizeof(wchar_t):sizeof(char);
	hData = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, aLength*aCharSize);
	pData = GlobalLock(hData);

	if(supportUnicode)
	{
		std::wstring aStr = theStr;
		memcpy((void*)pData, aStr.c_str(), aLength*aCharSize);
	}
	else
	{
		std::string aStr = theStr;
		memcpy((void*)pData, aStr.c_str(), aLength*aCharSize);
	}

	GlobalUnlock(hData);
    SetClipboardData(supportUnicode?CF_UNICODETEXT:CF_TEXT, hData);
    CloseClipboard();
}
