#ifndef __WONCONTROLS_H__
#define __WONCONTROLS_H__
#include "Button.h"
#include "Scrollbar.h"
#include "InputBox.h"
#include "ComboBox.h"
#include "ScrollArea.h"
#include "WrappedText.h"
#include "MultiListBox.h"
#include "TabCtrl.h"
#include "StretchImage.h"
#include "SimpleComponent.h"

namespace WONAPI
{
class WONSkin;
typedef SmartPtr<WONSkin> WONSkinPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONFrame 
{
public:
	int mLeft, mTop, mRight, mBottom;
	ImagePtr mImage;

	void Copy(const WONFrame &from);

public:
	WONFrame(Image *theImage = NULL, int theSize = 0);
	WONFrame(Image *theImage, int theLeft, int theTop, int theRight, int theBottom);
	WONFrame(StretchImage *theImage);
	WONFrame(const WONFrame &theFrame);
	WONFrame& operator=(const WONFrame &theFrame);

	void SetImage(Image *theImage);
	void SetImage(Image *theImage, int theLeft, int theTop, int theRight, int theBottom);
	void Paint(Graphics &g, Component *theComponent);
	void Paint(Graphics &g, Component *theComponent, int lp, int tp, int rp, int bp);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSimpleButton : public Button // just a button made up of 4 possible images (normal, pressed, disabled, over)
{
protected:
	ImagePtr mImages[4];
	bool mScaleImage;
	bool mCheckMouseButtonDown;

public:
	virtual void Paint(Graphics &g);
	virtual void GetDesiredSize(int &width, int &height);

public:
	WONSimpleButton();

	void SetImage(int theState, Image *theImage) { mImages[theState] = theImage; }
	void SetScaleImage(bool doScale) { mScaleImage = doScale; }
	void SetCheckMouseButtonDown(bool check) { mCheckMouseButtonDown = check; }

	WONSimpleButton* Duplicate();
};
typedef SmartPtr<WONSimpleButton> WONSimpleButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONButton;
typedef SmartPtr<WONButton> WONButtonPtr;

class WONButton : public Button
{
protected:
	struct ButtonState
	{
		WrappedTextPtr mText;
		ImagePtr mImage;
		int mColor;

		ButtonState() : mColor(-1) { }
	};

	ButtonState mStates[5][2];

	typedef std::map<Font*,WrappedTextPtr> FontTextMap;
	FontTextMap mFontTextMap;
	bool mAdjustHeightToFitText;

	int mLeftTextPad, mTopTextPad,mRightTextPad,mBottomTextPad;
	int mLeftFocusPad, mTopFocusPad, mRightFocusPad, mBottomFocusPad;
	int mPushDX, mPushDY;
	int mDefaultTextColor;
	bool mHasCheck;
	bool mScaleImage;
	bool mWrapText;
	bool mDrawGrayText;
	bool mIsTransparent;
	unsigned char mHAlign;
	unsigned char mVAlign;
	unsigned char mFocusStyle;

	void DrawButtonText(Graphics &g, WrappedText *theText, int theColor);
	void GetButtonState(Graphics &g, int *theColor, WrappedText **theText = NULL, Image **theImage = NULL);
	virtual void SetTextHook(const GUIString &theText);

public:
	virtual void Paint(Graphics &g);
	virtual void GetTextSize(int &width, int &height);
	virtual void GetDesiredSize(int &width, int &height);
	virtual void SizeChanged();

	void AdjustHeightToFitText();

public:
	WONButton();

	virtual const GUIString& GetText();
	void SetButtonState(int theState, Image *theImage, int theColor, Font *theFont, bool checked = false);

	void SetScaleImage(bool doScale) { mScaleImage = doScale; }
	void SetHasCheck(bool hasCheck) { mHasCheck = hasCheck; }
	void SetPushOffsets(int dx, int dy) { mPushDX = dx; mPushDY = dy; }
	void SetTextPadding(int left, int top, int right, int bottom) { mLeftTextPad = left; mTopTextPad = top; mRightTextPad = right; mBottomTextPad = bottom; }
	void SetFocusPadding(int left, int top, int right, int bottom) { mLeftFocusPad = left; mTopFocusPad = top; mRightFocusPad = right; mBottomFocusPad = bottom; }
	void SetTextPaddingAtLeast(int left, int top, int right, int bottom);
	void SetHorzTextPad(int left, int right) { mLeftTextPad = left; mRightTextPad = right; }
	void SetVertTextPad(int top, int bottom) { mTopTextPad = top; mBottomTextPad = bottom; }
	void SetTextColor(int theColor);
	void SetFont(Font *theFont);
	void SetHorzAlign(HorzAlign theHorzAlign); 
	void SetVertAlign(VertAlign theVertAlign); 
	void SetFocusStyle(unsigned char theStyle) { mFocusStyle = theStyle; }
	void SetDefaultTextColor(int theColor) { mDefaultTextColor = theColor; }

	void SetAdjustHeightToFitText(bool adjust);
	void SetWrapText(bool wrap);
	void SetTransparent(bool isTransparent);
	void SetDrawGrayText(bool drawGrayText) { mDrawGrayText = drawGrayText; }

	int GetLeftTextPad() { return mLeftTextPad; }
	int GetRightTextPad() { return mRightTextPad; }
	int GetTopTextPad() { return mTopTextPad; }
	int GetBottomTextPad() { return mBottomTextPad; }

	void CopyAttributes(WONButton *from);
	WONButton* Duplicate();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONHeaderButton : public WONButton
{
public:
	WONHeaderButton();
	virtual void Paint(Graphics &g);

	WONHeaderButton* Duplicate();
};
typedef SmartPtr<WONHeaderButton> WONHeaderButtonPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONScrollThumb : public WONSimpleButton
{
public:
	WONScrollThumb();
};
typedef SmartPtr<WONScrollThumb> WONScrollThumbPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONScrollbar : public Scrollbar
{
protected:
	bool mIsTransparent;

public:
	WONScrollbar(bool vertical = true);
	void SetTransparent(bool isTransparent);
};
typedef SmartPtr<WONScrollbar> WONScrollbarPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONInputBox : public InputBox
{
protected:
	WONFrame mFrame;

public:
	WONInputBox();

	virtual void Paint(Graphics &g);
	WONInputBox* Duplicate();

	void SetFrame(const WONFrame &theFrame) { mFrame = theFrame; }
};
typedef SmartPtr<WONInputBox> WONInputBoxPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONComboBox : public ComboBox
{
protected:
	bool mDrawFrame;
	bool mButtonInFrame;
	WONFrame mFrame;

public:
	virtual void Paint(Graphics &g);
	virtual void SizeChanged();
	virtual void GetDesiredSize(int &width, int &height);

public:
	WONComboBox();

	void SetDrawFrame(bool drawFrame) { mDrawFrame = drawFrame; }
	void SetButtonInFrame(bool buttonInFrame) { mButtonInFrame = buttonInFrame; }

	WONComboBox* Duplicate();

	void SetFrame(const WONFrame &theFrame) { mFrame = theFrame; }
};
typedef WONAPI::SmartPtr<WONComboBox> WONComboBoxPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONScrollContainer : public ScrollbarScroller
{
protected: 
	int mHeaderHeight;
	bool mTransparent;
	WONFrame mFrame;
	ImagePtr mCorner;

public:
	WONScrollContainer();

	virtual void SizeChanged();
	virtual void Paint(Graphics &g);

	void SetTransparent(bool transparent) { mTransparent = transparent; }
	void SetFrame(const WONFrame& theFrame) { mFrame = theFrame; }
	void SetCorner(Image *theCorner) { mCorner = theCorner; }

	WONScrollContainer* Duplicate();
	void CopyAttributes(WONScrollContainer *from);
};
typedef WONAPI::SmartPtr<WONScrollContainer> WONScrollContainerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONHeaderControl : public MultiListHeader
{
protected:
	int mTextLeftPad;
	WONSkinPtr mSkin;

public:
	WONHeaderControl(WONSkin *theSkin = NULL);
	void SetSkin(WONSkin *theSkin);

	void SetColumn(int theCol, const GUIString &theTitle, int *theDesiredHeight = NULL);
	void SetTextLeftPad(int thePad) { mTextLeftPad = thePad; }
};
typedef WONAPI::SmartPtr<WONHeaderControl> WONHeaderControlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONListCtrl : public WONScrollContainer
{
protected:
	WONHeaderControlPtr mHeader;

public:
	virtual void SizeChanged();

public:
	WONListCtrl();
	MultiListArea* GetListArea() { return (MultiListArea*)GetScrollArea(); }
	WONHeaderControl* GetHeader() { return mHeader; }
	void SetHeader(WONHeaderControl *theHeader);
	void SetHeaderHeight(int theHeight) { mHeaderHeight = theHeight; }
	void SetStandarColumnHeader(int theCol, const GUIString &theTitle, int theWidth);
	void SetColumnHeader(int theCol, Component *theHeader, int theWidth);

	WONListCtrl* Duplicate();
};
typedef WONAPI::SmartPtr<WONListCtrl> WONListCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONTabCtrl : public TabCtrl
{
protected:
	WONSkinPtr mSkin;
	WONFrame mFrame;
	ComponentPtr mBack;
	bool mIsTransparent;

public:
	WONTabCtrl(WONSkin *theSkin = NULL);

	void AddStandardTab(const GUIString &theText, Container *theContainer);
	WONTabCtrl* Duplicate();
	void SetFrame(const WONFrame& theFrame);
	void SetSkin(WONSkin *theSkin) { mSkin = theSkin; }
	void SetTransparent(bool isTransparent);
};
typedef SmartPtr<WONTabCtrl> WONTabCtrlPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONFrameComponent : public Component
{
protected:
	WONFrame mFrame;

public:
	WONFrameComponent(Image *theImage = NULL, int theLeft = 0, int theTop = 0, int theRight = 0, int theBottom = 0);
	WONFrameComponent(const WONFrame& theFrame);

	virtual void SetImage(Image *theImage);
	virtual void SetImage(Image *theImage, int theLeft, int theTop, int theRight, int theBottom);
	virtual void SetBorders(int theLeft, int theTop, int theRight, int theBottom);
	virtual void SetLeft(int theLeft) { mFrame.mLeft = theLeft; }
	virtual void SetTop(int theTop) { mFrame.mTop = theTop; }
	virtual void SetRight(int theRight) { mFrame.mRight = theRight; }
	virtual void SetBottom(int theBottom) { mFrame.mBottom = theBottom; }

	const WONFrame& GetFrame(void) { return mFrame; }
	virtual int Left(void) { return mFrame.mLeft; }
	virtual int Top(void) { return mFrame.mTop; }
	virtual int Right(void) { return mFrame.mRight; }
	virtual int Bottom(void) { return mFrame.mBottom; }

	virtual void Paint(Graphics &g);
};
typedef SmartPtr<WONFrameComponent> WONFrameComponentPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSkin : public RefCount
{
public:
	WONButtonPtr mPushButton;
	WONButtonPtr mCheckbox;
	WONButtonPtr mRadioButton;
	WONHeaderButtonPtr mHeaderButton;
	WONButtonPtr mTabButton;
	WONSimpleButtonPtr mCloseButton;
	WONSimpleButtonPtr mComboButton;
	WONSimpleButtonPtr mScrollThumb[2];
	WONSimpleButtonPtr mScrollGutter[2];
	WONSimpleButtonPtr mArrowButton[4];
	WONScrollbarPtr mScrollbar[2];
	WONInputBoxPtr mInputBox;
	WONComboBoxPtr mComboBox;
	WONScrollContainerPtr mScrollContainer;
	WONListCtrlPtr mListCtrl;
	WONTabCtrlPtr mTabCtrl;
	WONFrameComponentPtr mPopupBack;
	WONFrameComponentPtr mDialogBack;

	typedef std::map<GUIString,WONButtonPtr,GUIStringLessNoCase> ButtonTypeMap;
	ButtonTypeMap mButtonTypeMap;

	typedef std::map<GUIString,WONSimpleButtonPtr,GUIStringLessNoCase> SimpleButtonTypeMap;
	SimpleButtonTypeMap mSimpleButtonTypeMap;

public:
	virtual ~WONSkin();

	WONButton* GetPushButton();
	WONButton* GetCheckbox();
	WONButton* GetRadioButton();
	WONHeaderButton* GetHeaderButton();
	WONButton* GetTabButton();
	WONFrameComponent* GetPopupBack();
	WONFrameComponent* GetDialogBack();

	WONSimpleButton* GetCloseButton();
	WONSimpleButton* GetArrowButton(Direction theDirection);
	WONSimpleButton* GetScrollGutter(bool isVertical);
	WONSimpleButton* GetScrollThumb(bool isVertical);
	WONScrollbar* GetScrollbar(bool isVertical);
	WONInputBox* GetInputBox();
	WONComboBox* GetComboBox();	
	WONScrollContainer* GetScrollContainer();
	WONListCtrl* GetListCtrl();
	WONTabCtrl* GetTabCtrl();

	bool CopyButtonType(const GUIString &theType, WONButton *theCopyTo);
	bool CopySimpleButtonType(const GUIString &theType, WONSimpleButton *theCopyTo);
};
typedef SmartPtr<WONSkin> WONSkinPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONMSSkin : public WONSkin
{
public:
	///////////////////////////////////////////////////
	///////////////////////////////////////////////////
	enum MSImageFlag
	{
		MSImageFlag_Down		= 0x01,
		MSImageFlag_Default		= 0x02,
		MSImageFlag_Disabled	= 0x04,
		MSImageFlag_Checked		= 0x08
	};

	enum MSImageType
	{
		MSImageType_Frame1 = 0,
		MSImageType_Frame2,
		MSImageType_Frame3,
		MSImageType_LeftArrow,
		MSImageType_RightArrow,
		MSImageType_UpArrow,
		MSImageType_DownArrow,
		MSImageType_PushButton,
		MSImageType_Checkbox,
		MSImageType_RadioButton,
		MSImageType_ScrollThumb,
		MSImageType_ScrollGutter,
		MSImageType_ScrollCorner,
		MSImageType_ListHeader,
		MSImageType_TabButton,
		MSImageType_TabCtrlFrame,
		MSImageType_ComboPopupFrame,
		MSImageType_CloseButton
	};

	class MSImage : public ProgramImage
	{
	public:
		unsigned char mImageType;
		unsigned char mImageFlags;

		MSImage(unsigned char theImageType, unsigned char theImageFlags = 0, int theWidth = 0, int theHeight = 0) :
			mImageType(theImageType), mImageFlags(theImageFlags), ProgramImage(theWidth,theHeight) { }

		void DrawPushButton(Graphics &g, bool useFrame2, bool checkDefault = false);
		void DrawArrowButton(Graphics &g, Direction theDirection);
		void DrawCloseButton(Graphics &g);

		virtual ImagePtr Duplicate() { return new MSImage(mImageType,mImageFlags,mWidth,mHeight); }
		virtual void Paint(Graphics &g);
		bool FlagSet(unsigned char theFlags) { return mImageFlags&theFlags?true:false; }
	};

public: 
	WONMSSkin();
};
typedef SmartPtr<WONMSSkin> WONMSSkinPtr;


}; // namespace WONAPI

#endif