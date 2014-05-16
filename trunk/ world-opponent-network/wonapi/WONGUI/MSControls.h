#ifndef __WON_MSCONTROLS_H__
#define __WON_MSCONTROLS_H__

#include "Button.h"
#include "Scrollbar.h"
#include "Dialog.h"
#include "InputBox.h"
#include "ComboBox.h"
#include "MultiListBox.h"
#include "TabCtrl.h"
#include "Label.h"
#include "Spinner.h"
#include "Align.h"
#include "ColorScheme.h"
#include "WrappedText.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef ButtonPtr(*MSArrowButtonFactory)(Direction theDirection);
ButtonPtr DefaultMSArrowButtonFactory(Direction theDirection);

class MSGraphicsUtil
{
public:
	static void DrawFrame1(Graphics &g, int x, int y, int width, int height, bool down);
	static void DrawFrame2(Graphics &g, int x, int y, int width, int height, bool down);
	static void DrawFrame3(Graphics &g, int x, int y, int width, int height, bool down);

	static void DrawArrow(Graphics &g, Direction theDirection, int x, int y, int width, int height, bool disabled);
	static void DrawCheck(Graphics &g, int x, int y, int width, int height, bool down, bool checked);
	static void DrawRadio(Graphics &g, int x, int y, int width, int height, bool down, bool checked);
	static void DrawTab(Graphics &g, int x, int y, int width, int height);
	static void DrawCloseX(Graphics &g, int x, int y, int width, int height);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSButtonBase : public Button
{
protected:
	WrappedTextPtr mText;
	StandardColor mStandardTextColor;
	int mTextColor;
	unsigned char mHAlign;
	unsigned char mVAlign;
	unsigned char mFocusStyle;
	unsigned char mTopTextPad, mBottomTextPad, mLeftTextPad, mRightTextPad;
	bool mIsTransparent;
	bool mAdjustHeightToFitText;
	
	virtual ~MSButtonBase();
	void DrawButtonText(Graphics &g, int xoff, int yoff);
	virtual void SetTextHook(const GUIString &theText);

public:
	enum FocusStyle
	{
		FocusStyle_None,
		FocusStyle_AroundWindow,
		FocusStyle_AroundText
	};

	virtual void SizeChanged();
	void AdjustHeightToFitText();


public:
	MSButtonBase();

	virtual const GUIString& GetText() { return mText->GetText(); }

	void SetTextColor(int theColor) { mTextColor = theColor; }
	void SetStandardTextColor(StandardColor theColor);
	void SetRichText(const GUIString &theText) { mText->SetRichText(theText); }
	void SetFont(Font *theFont) { mText->SetFont(theFont); } //mFont = theFont; }
	void SetWrap(bool wrap) { mText->SetWrap(wrap); }
	void SetFocusStyle(FocusStyle theStyle) { mFocusStyle = (unsigned char)theStyle; }
	void SetHorzAlign(HorzAlign theHorzAlign) { mHAlign = (unsigned char)theHorzAlign; mText->SetCentered(mHAlign==HorzAlign_Center); }
	void SetVertAlign(VertAlign theVertAlign) { mVAlign = (unsigned char)theVertAlign; }
	void SetHorzTextPad(int left, int right) { mLeftTextPad = (unsigned char)left; mRightTextPad = (unsigned char)right; }
	void SetVertTextPad(int top, int bottom) { mTopTextPad = (unsigned char)top; mBottomTextPad = (unsigned char)bottom; }
//	void IncrementHorzTextPad(int left, int right) { mLeftTextPad += (unsigned char)left; mRightTextPad += (unsigned char)right; }
	void IncrementHorzTextPad(int left, int right) { mLeftTextPad = mLeftTextPad + (unsigned char)left; mRightTextPad = mRightTextPad + (unsigned char)right; }
//	void IncrementVertTextPad(int top, int bottom) { mTopTextPad += (unsigned char)top; mBottomTextPad += (unsigned char)bottom; }
	void IncrementVertTextPad(int top, int bottom) { mTopTextPad = mTopTextPad + (unsigned char)top; mBottomTextPad = mBottomTextPad + (unsigned char)bottom; }
	void SetTransparent(bool isTransparent);
	void SetAdjustHeightToFitText(bool adjust);
	int GetTextWidth() { return mText->GetWidth(); }

	virtual void GetDesiredSize(int &width, int &height);
};
typedef WONAPI::SmartPtr<MSButtonBase> MSButtonBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSButton : public MSButtonBase
{
protected:
	virtual ~MSButton();
	bool mUseFrame2;

	void Init();

public:
	MSButton();
	MSButton(const GUIString &theStr);
	virtual void Paint(Graphics &g);
	void SetUseFrame2(bool useFrame2) { mUseFrame2 = useFrame2; }
};
typedef WONAPI::SmartPtr<MSButton> MSButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSCheckbox : public MSButtonBase
{
protected:
	virtual ~MSCheckbox();
	void Init();

public:
	MSCheckbox();
	MSCheckbox(const GUIString &theText);
	virtual void Paint(Graphics &g);

	typedef void(*CheckboxPainterFunc)(Graphics &g, MSCheckbox *theCheckbox);
	static void DefaultCheckboxPainterFunc(Graphics &g, MSCheckbox *theCheckbox);
	static int mCheckboxWidth, mCheckboxHeight;
	static CheckboxPainterFunc CheckboxPainter;
};
typedef WONAPI::SmartPtr<MSCheckbox> MSCheckboxPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSRadioButton: public MSButtonBase
{
protected:
	virtual ~MSRadioButton();
	
	void Init();

public:
	MSRadioButton();
	MSRadioButton(const GUIString &theText);
	virtual void Paint(Graphics &g);

	typedef void(*RadioPainterFunc)(Graphics &g, MSRadioButton *theRadio);
	static void DefaultRadioPainterFunc(Graphics &g, MSRadioButton *theRadio);
	static int mRadioWidth, mRadioHeight;
	static RadioPainterFunc RadioPainter;
};
typedef WONAPI::SmartPtr<MSRadioButton> MSRadioButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSArrowButton : public Button
{
protected:
	virtual ~MSArrowButton();

	Direction mDirection;

public:
	MSArrowButton(Direction theDirection = Direction_Up);
	void SetDirection(Direction theDirection) { mDirection = theDirection; }
	virtual void Paint(Graphics &g);
};
typedef WONAPI::SmartPtr<MSArrowButton> MSArrowButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSCloseButton : public Button
{
public:
	MSCloseButton();
	virtual void Paint(Graphics &g);

	virtual void GetDesiredSize(int &width, int &height) { width = 16; height = 14; }
};
typedef WONAPI::SmartPtr<MSCloseButton> MSCloseButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSTabButton : public MSButtonBase
{
public:
	int mLowerAmount;

	MSTabButton(const GUIString &theText = "", bool fillWhole = true, int theLowerAmount = 3);
	virtual void Paint(Graphics &g);	

	virtual void GetDesiredSize(int &width, int &height);

	void SetLowerAmount(int theAmount) { mLowerAmount = theAmount; }
};
typedef WONAPI::SmartPtr<MSTabButton> MSTabButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSHeaderButton : public MSButton
{
public:
	MSHeaderButton(const GUIString &theText = "");
	virtual void Paint(Graphics &g);
};
typedef SmartPtr<MSHeaderButton> MSHeaderButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSScrollThumb : public Component
{
protected:
	virtual ~MSScrollThumb();

public:
	MSScrollThumb();
	virtual void Paint(Graphics &g);
};
typedef WONAPI::SmartPtr<MSScrollThumb> MSScrollThumbPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSLabel : public MSButtonBase
{
public:
	MSLabel();
	MSLabel(const GUIString &theText);

	virtual void Paint(Graphics &g);
};
typedef WONAPI::SmartPtr<MSLabel> MSLabelPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSScrollbar : public Scrollbar
{
protected:
	bool mIsTransparent;

public:
	MSScrollbar(bool vertical = true);
	void SetTransparent(bool isTransparent);

	virtual void Paint(Graphics &g);
	static MSArrowButtonFactory ArrowButtonFactory;

	typedef ComponentPtr(*ThumbFactoryFunc)(bool isVert);
	static ComponentPtr DefaultThumbFactory(bool /*isVert*/) { return new MSScrollThumb; }
	static ThumbFactoryFunc ThumbFactory;
};
typedef WONAPI::SmartPtr<MSScrollbar> MSScrollbarPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSInputBox : public InputBox
{
public:
	virtual void Paint(Graphics &g);
	virtual void GetDesiredSize(int &width, int &height);
};
typedef WONAPI::SmartPtr<MSInputBox> MSInputBoxPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSComboBox : public ComboBox
{
public:
	virtual void Paint(Graphics &g);
	virtual void SizeChanged();
	virtual void GetDesiredSize(int &width, int &height);

	bool mDrawFrame;
	static MSArrowButtonFactory ArrowButtonFactory;

public:
	MSComboBox();

	void SetDrawFrame(bool drawFrame) { mDrawFrame = drawFrame; }

};
typedef WONAPI::SmartPtr<MSComboBox> MSComboBoxPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSScrollContainer : public ScrollbarScroller
{
protected: 
	int mHeaderHeight;
	bool mDraw3DFrame;
	bool mDrawFrame;
	bool mTransparent;

public:
	MSScrollContainer(ScrollArea *theScrollArea = NULL);

	virtual void SizeChanged();
	virtual void Paint(Graphics &g);

	void SetDraw3DFrame(bool on);
	void SetDrawFrame(bool on);
	void SetTransparent(bool transparent) { mTransparent = transparent; }
};
typedef WONAPI::SmartPtr<MSScrollContainer> MSScrollContainerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSScrollButtonContainer : public ButtonScroller
{
public:
	MSScrollButtonContainer(ScrollArea *theScrollArea);
};
typedef WONAPI::SmartPtr<MSScrollButtonContainer> MSScrollButtonContainerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSPopup : public Component
{
public:
	virtual void Paint(Graphics& g);
};
typedef WONAPI::SmartPtr<MSPopup> MSPopupPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSDlgBack : public Component
{
public:
	virtual void Paint(Graphics& g);
};
typedef WONAPI::SmartPtr<MSDlgBack> MSDlgBackPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSDialog : public Dialog
{
public:
	MSDialog();
};
typedef SmartPtr<MSDialog> MSDialogPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSSeperator : public Component
{
public:
	MSSeperator(int theHeight = 6);
	virtual void Paint(Graphics& g);

	virtual void GetDesiredSize(int &width, int &height) { width = 100; height = 6; }
};
typedef WONAPI::SmartPtr<MSSeperator> MSSeperatorPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MS3DFrame : public Component
{
protected:
	bool mIsUp;
	int mType;

public:
	MS3DFrame(int theType = 3, bool up = false);
	virtual void Paint(Graphics& g);

	void SetIsUp(bool isUp) { mIsUp = isUp; }
	void SetFrameType(int theType) { mType = theType; }
};
typedef SmartPtr<MS3DFrame> MS3DFramePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSEtchedFrame : public Component
{
public:
	MSEtchedFrame();
	virtual void Paint(Graphics &g);
};
typedef WONAPI::SmartPtr<MSEtchedFrame> MSEtchedFramePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSEtchedFrameGroup : public Container
{
protected:
	LabelPtr mLabel;
	ComponentPtr mComponent;

	virtual void AddedToParent();

public:
	MSEtchedFrameGroup();
	MSEtchedFrameGroup(const GUIString &theTitle);

	void AddContainedComponent(Component *theComponent);
	void SetLabelText(const GUIString &theText);
	virtual void GetDesiredSize(int &width, int &height);
	virtual void Paint(Graphics &g);

	Label* GetLabel() { return mLabel; }
};
typedef WONAPI::SmartPtr<MSEtchedFrameGroup> MSEtchedFrameGroupPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSHeaderControl : public MultiListHeader
{
protected:
	int mTextLeftPad;

public:
	MSHeaderControl(MultiListArea *theListArea = NULL);
	void SetColumn(int theCol, const GUIString &theTitle);
	void SetTextLeftPad(int thePad) { mTextLeftPad = thePad; }
};
typedef WONAPI::SmartPtr<MSHeaderControl> MSHeaderControlPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSListCtrl : public MSScrollContainer
{
protected:
	MSHeaderControlPtr mHeader;

public:
	virtual void SizeChanged();

public:
	MSListCtrl(MultiListArea *theListArea = NULL);
	MultiListArea* GetListArea() { return (MultiListArea*)GetScrollArea(); }
	MSHeaderControl* GetHeader() { return mHeader; }
	void SetHeaderHeight(int theHeight) { mHeaderHeight = theHeight; }
};
typedef WONAPI::SmartPtr<MSListCtrl> MSListCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSToolTip : public Label
{
public:
	MSToolTip(const GUIString &theText);
};
typedef WONAPI::SmartPtr<MSToolTip> MSToolTipPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSToolButton : public MSButtonBase
{
public:
	virtual void Paint(Graphics &g);

public:
	MSToolButton(const GUIString &theText);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSTabCtrl : public TabCtrl
{
public:
	bool mIsTransparent;
	MSDlgBackPtr mBack;

	virtual void AddedToParent();

public:
	MSTabCtrl();

	void SetTransparent(bool isTransparent);
	void AddStandardTab(const GUIString &theText, Container *theContainer);
};
typedef SmartPtr<MSTabCtrl> MSTabCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSMessageBox : public MSDialog
{
protected:
	MSButtonPtr mButton1;
	MSButtonPtr mButton2;
	MSButtonPtr mButton3;

	MSLabelPtr mLabel;
	GUIString mTitle;
	int mCreateFlags;

public:
	MSMessageBox(
		const GUIString &theTitle, const GUIString &theMessage = GUIString::EMPTY_STR, 
		const GUIString &theButton1 = GUIString::EMPTY_STR, const GUIString &theButton2 = GUIString::EMPTY_STR, 
		const GUIString &theButton3 = GUIString::EMPTY_STR);
	
	static int Msg(Window *theParent, const GUIString &theTitle, const GUIString &theMessage = GUIString::EMPTY_STR, 
		const GUIString &theButton1 = GUIString::EMPTY_STR, const GUIString &theButton2 = GUIString::EMPTY_STR, 
		const GUIString &theButton3 = GUIString::EMPTY_STR);
	
	void SetCreateFlags(int theFlags) { mCreateFlags = theFlags; } // used by DoDialog to create the window
	WindowPtr PrepareWindow(Window *theParent); // creates window, adds component to window, etc...
	int DoDialog(Window *theParent);

	MSLabel* GetLabel() { return mLabel; }
};
typedef SmartPtr<MSMessageBox> MSMessageBoxPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSFileDialog : public Dialog
{
public:
	struct FilterPair
	{
		GUIString mFilter;
		GUIString mDesc;

		FilterPair() { }
		FilterPair(const GUIString &f, const GUIString &d) : mFilter(f), mDesc(d) { }
	};
	typedef std::list<FilterPair> FilterList;
	typedef std::list<GUIString> FileList;

protected:
	bool mIsSave;
	bool mGetDir;
	bool mAllowMultiple;
	bool mWarnIfExists;
	bool mFileMustExist;
	bool mNoChangeDir;
	FilterList mFilterList;
	FileList mFileList;
	GUIString mTitle;
	GUIString mOkTitle;
	GUIString mInitialFileName;
	GUIString mInitialDir;

public:
	const GUIString& GetOkTitle() { return mOkTitle; }
	bool IsGetDir() { return mGetDir; }

public:
	MSFileDialog(bool isSave = false);

	void SetTitle(const GUIString &theTitle);
	void SetOkTitle(const GUIString &theOkTitle);
	void SetInitialFileName(const GUIString &theFileName);
	void SetInitialDir(const GUIString &theDir);
	void SetAllowMultiple(bool allowMultiple);
	void SetWarnIfExists(bool warnIfExists);
	void SetFileMustExist(bool mustExist);
	void SetGetDir(bool getDir);
	void SetNoChangeDir(bool noChange);
	void AddFilter(const GUIString &theFilter, const GUIString &theDescription);

	const FileList& GetFileList();
	GUIString GetFile();

	bool DoDialog(Window *theParent);

};
typedef SmartPtr<MSFileDialog> MSFileDialogPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSSpinner : public Spinner
{
protected:
	LabelPtr mLabel;

	virtual void GetDesiredSize(int &width, int &height);
	virtual void SizeChanged();

	void Init();
public:
	MSSpinner();
	MSSpinner(const GUIString &theText, int minValue, int maxValue);

	void SetLabelText(const GUIString &theText);
	Label* GetLabel() { return mLabel; }
};
typedef SmartPtr<MSSpinner> MSSpinnerPtr;

}; // namespace WONAPI


#endif