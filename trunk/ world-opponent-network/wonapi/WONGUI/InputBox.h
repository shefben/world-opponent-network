#ifndef __WON_INPUTBOX_H__
#define __WON_INPUTBOX_H__

#include "Component.h"
#include "Rectangle.h"
#include "Background.h"
#include "SelectionColor.h"
#include <deque>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class InputBox : public Component
{
public:
	static bool NumericFilter(InputBox *theInputBox, GUIString &theStr);

protected:
	GUIString mText;
	GUIString mActualText;
	char mPasswordChar;
	ImagePtr mIcon;

	typedef bool(*Transform)(InputBox *theInputBox, GUIString &theStr);
	Transform mTransform;

	typedef std::deque<GUIString> InputHistory;
	InputHistory mInputHistory;
	int mInputHistoryPos;
	int mMaxInputHistory;
	bool mHaveTraversedHistory;

	bool mBackwardsSel;
	int mSelStartChar;
	int mSelEndChar;
	int mHorzOffset;
	bool mSelecting;

	int mLeftPad, mTopPad, mRightPad, mBottomPad;

	int mCursorDelay;
	int mCursorOnTime;
	int mCursorOffTime;
	bool mCursorOn;
	bool mUnderlineCursor;
	bool mAllowSelection;
	bool mSelectOnFocus;
	bool mUseDisabledColors;

	int mMaxChars;

	FontPtr mFont;
	int mTextColor;
	int mDisabledTextColor;
	SelectionColor mSelColor;
	WONRectangle mTextRect;
	Background mBackground;
	Background mDisabledBackground;

	virtual void SizeChanged();
	virtual void Paint(Graphics &g);
	virtual void GotFocus();
	virtual void LostFocus();

	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseExit();
	virtual void MouseDrag(int x, int y);
	virtual bool TimerEvent(int theDelta);

	int MoveCursor(bool left, bool wholeWord, const GUIString &theStr, int thePos);
	void AdjustPosition();
	void CheckSel(int x, int y, bool newStart);
	virtual bool KeyChar(int theKey);
	virtual bool KeyDown(int theKey);


	void SetTextPrv(const GUIString &theText, bool sendEvent = true);
	void AdjustTextRect();

public:
	InputBox();

	bool IsEmpty() const { return mText.empty(); }

	virtual void GetDesiredSize(int &width, int &height);

	const GUIString& GetText() const;
	void SetText(const GUIString &theText, bool sendEvent = true);
	void Clear(bool sendEvent = true);
	void AddInputHistory(const GUIString &theText, bool clearTraversalState = true);
	void SetInputHistory(const GUIString &theText, bool clearTraversalState = false);
	bool HaveTraversedInputHistory() { return mHaveTraversedHistory; }
	void SetHaveTraversedInputHistory(bool have) { mHaveTraversedHistory = have; }
	void IncInputHistoryPos(int theAmount = 1) { mInputHistoryPos += theAmount; }
	void SetCharWidth(int theEm);

	void SetFont(Font *theFont) { mFont = theFont; }
	Font* GetFont() { return mFont; }

	void SetBackground(const Background &theBackground) { mBackground = theBackground; ApplyBackground(mBackground); }
	void SetDisabledBackground(const Background &theBackground) { mDisabledBackground = theBackground; }
	void SetIcon(Image *theIcon);

	void SetTextColor(int theColor) { mTextColor = theColor; }
	void SetDisabledTextColor(int theColor) { mDisabledTextColor = theColor; }
	void SetSelectionColor(const SelectionColor &theColor) { mSelColor = theColor; }
	void SetAllowSelection(bool allowSelection);
	void SetDoUnderlineCursor(bool doUnderline) { mUnderlineCursor = doUnderline; }
	void SetSelectOnFocus(bool selectOnFocus) { mSelectOnFocus = selectOnFocus; }
	void SetUseDisabledColors(bool useDisabledColors) { mUseDisabledColors = useDisabledColors; }

	void CopySelection();
	void SetSel(int theStartPos = 0, int theEndPos = -1);
	void SetSelText(const GUIString &theText, bool sendEvent = true);
	void SetPasswordChar(char theChar);
	void SetMaxChars(int theMaxChars) { mMaxChars = theMaxChars; }
	void SetMaxInputHistory(int theMaxHistory) { mMaxInputHistory = theMaxHistory; }
	void SetCursorBlinkRate(int theOnTime, int theOffTime = 0); // off time defaults to on time
	void SetTransform(Transform theTransform) { mTransform = theTransform; }
	void SetLeftPad(int pad) { mLeftPad = pad; }
	void SetRightPad(int pad) { mRightPad = pad; }
	void SetTopPad(int pad) { mTopPad = pad; }
	void SetBottomPad(int pad) { mBottomPad = pad; }

	int GetSelStart() { return mSelStartChar; }
	int GetSelEnd() { return mSelEndChar; }
	
};

typedef WONAPI::SmartPtr<InputBox> InputBoxPtr;

}; // namespace WONAPI

#endif