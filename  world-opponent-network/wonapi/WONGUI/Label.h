#ifndef __WON_LABEL_H__
#define __WON_LABEL_H__

#include "Component.h"
#include "Background.h"
#include "Align.h"
#include "WrappedText.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Label : public Component
{
protected:
	WrappedTextPtr mText;
	int mTextColor;
	HorzAlign mHorzAlign;
	VertAlign mVertAlign;
	int mLeftPad, mRightPad, mTopPad, mBottomPad;
	bool mAdjustHeightToFitText;

	Background mBackground;

	virtual ~Label();
	void SetMaxWidth();
	void AdjustHeightToFitText();
	void Init();

public:
	virtual void Paint(Graphics &g);
	virtual void SizeChanged();
	virtual void GetDesiredSize(int &width, int &height);

public:
	Label();
	Label(const GUIString &theText);

	void SetText(const GUIString &theText, bool invalidate = true);
	void SetRichText(const GUIString &theText, bool invalidate = true);
	void SetFont(Font *theFont) { mText->SetFont(theFont); }
	void SetTextColor(int theColor) { mTextColor = theColor; }

	void SetHorzAlign(HorzAlign theAlign);
	void SetVertAlign(VertAlign theAlign) { mVertAlign = theAlign; }
	void SetHorzPad(int left, int right) { mLeftPad = left; mRightPad = right; SetMaxWidth(); }
	void SetVertPad(int top, int bottom) { mTopPad = top; mBottomPad = bottom; }

	void SetBackground(const Background &theBackground) { mBackground = theBackground; ApplyBackground(mBackground); }

	const GUIString& GetText() { return mText->GetText(); }
	void SetWrap(bool wrap);
	void SetAdjustHeightToFitText(bool adjust);

	Font* GetTextFont() { return mText->GetFont(); }
	int GetTextWidth() { return mText->GetWidth(); }
};
typedef WONAPI::SmartPtr<Label> LabelPtr;

}; // namespace WONAPI

#endif