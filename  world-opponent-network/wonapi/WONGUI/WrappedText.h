#ifndef __WRAPPEDTEXT_H__
#define __WRAPPEDTEXT_H__

#include "WONCommon/SmartPtr.h"
#include "Graphics.h"
#include "Window.h"
#include "Table.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WrappedText : public RefCount
{
protected:
	GUIString mText;
	FontPtr mFont;
	MultiLineElementPtr mRichText;
	
	struct BreakInfo
	{
		unsigned short mBreakPos;
		unsigned short mNextStartPos;
		unsigned short mWidth;

		BreakInfo(int theBreakPos, int theNextStartPos, int theWidth) : mBreakPos((unsigned short)theBreakPos), mNextStartPos((unsigned short)theNextStartPos), mWidth((unsigned short)theWidth) { }
	};
	typedef std::list<BreakInfo> BreakList;
	BreakList mBreakList;

	int mWidth, mHeight, mMaxWidth;
	bool mWrap;
	bool mIsCentered;

	void CalcWrap();
	void Init();
public:
	WrappedText();
	WrappedText(const GUIString &theStr);
	void SetText(const GUIString &theStr);
	void SetRichText(const GUIString &theText, int theDefaultColor = -1);
	void SetWrap(bool wrap);
	void SetCentered(bool isCentered);
	void SetFont(Font *theFont);
	void SetMaxWidth(int theMaxWidth);

	int GetWidth() { return mWidth; }
	int GetHeight() { return (mHeight == 0 ? mFont->GetHeight() : mHeight); }

	void Paint(Graphics &g, int x, int y);

	const GUIString& GetText() { return mText; }
	Font* GetFont() { return mFont; }
	bool IsRich() { return mRichText.get()?true:false; }

};
typedef SmartPtr<WrappedText> WrappedTextPtr;

}; // namespace WONAPI

#endif