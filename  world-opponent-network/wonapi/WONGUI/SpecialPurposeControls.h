#ifndef __WON_SPECIALPURPOSECONTROLS_H__
#define __WON_SPECIALPURPOSECONTROLS_H__

#include "Button.h"
#include "Container.h"
#include "SimpleComponent.h"

namespace WONAPI
{

enum SortMode
{
	SortMode_String,
	SortMode_Numeric
};

///////////////////////////////////////////////////////////////////////////////
// Column header button
class ColHeaderButton : public Button
{
protected:
	FontPtr mFont;
	std::string mText;
	DWORD mBackColor;
	bool mIsUp;
	MultiListAreaPtr mMultiListBox;
	int mCol;
	SortMode mSortMode;
	
	virtual ~ColHeaderButton();
	void DrawButtonText(Graphics &g, int xoff, int yoff);
	virtual void Activate();

public:
	virtual void Paint(Graphics &g);

public:
	ColHeaderButton();

	void SetText(const char *theText) { mText = theText; }
	void SetFont(Font *theFont) { mFont = theFont; }
	void SetBackColor(DWORD theBackColor) { mBackColor = theBackColor; }
	void SetMultiListBox(MultiListArea* theMultiListBox, int theCol, SortMode theSortMode) {mMultiListBox = theMultiListBox; mCol = theCol; mSortMode = theSortMode; }
};
typedef WONAPI::SmartPtr<ColHeaderButton> ColHeaderButtonPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ImageCheckButtonType
{
	ImageCheckButtonType_Normal				= 0,
	ImageCheckButtonType_Over				= 1,
	ImageCheckButtonType_Checked			= 2,
	ImageCheckButtonType_CheckedOver		= 3
};

class ImageCheckButton : public Button
{
protected:
	virtual ~ImageCheckButton();
	ImagePtr mImages[4];

	int mAlternateControlID;

public:
	ImageCheckButton(int theNormalControlID, int theCheckedControlID);
	virtual void Paint(Graphics &g);

	void SetImage(ImageCheckButtonType theType, Image *theImage) { mImages[theType] = theImage; }
	void AlternateControlIDs(void);
};
typedef WONAPI::SmartPtr<ImageCheckButton> ImageCheckButtonPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CheckButtonSplitterContainer : public Container
{
protected:
	virtual ~CheckButtonSplitterContainer();

	void ChangeAlternateButtonState(void);

	BorderComponent* mNormalSplitter;
	BorderComponent* mMaxMinSplitter;

	ImageCheckButton* mFirstMinButton;
	ImageCheckButton* mSecondMinButton;
	ImageCheckButton* mFirstMaxButton;
	ImageCheckButton* mSecondMaxButton;

	enum MaxMinStatus
	{
		Status_Min,
		Status_Normal,
		Status_Max
	};

	MaxMinStatus mStatus;

public:
	CheckButtonSplitterContainer(void);

	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void MouseDown(int x, int y, MouseButton theButton);

	void AddSplitterBars(BorderComponent* theNormalSplitter, BorderComponent* theMaxMinSplitter);
	void AddMinButtons(ImageCheckButton* theFirstMinButton, ImageCheckButton* theSecondMinButton);
	void AddMaxButtons(ImageCheckButton* theFirstMaxButton, ImageCheckButton* theSecondMaxButton);
};
typedef WONAPI::SmartPtr<CheckButtonSplitterContainer> CheckButtonSplitterContainerPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BorderImageButton : public ImageButton
{
protected:
	virtual ~BorderImageButton() {}
	BorderComponentPtr mImages[5];

public:
	BorderImageButton() : ImageButton() {}
	virtual void Paint(Graphics &g);
	virtual void SizeChanged();

	void SetImage(ImageButtonType theType, BorderComponent *theImage) { mImages[theType] = theImage; }
};

typedef WONAPI::SmartPtr<BorderImageButton> BorderImageButtonPtr;

};

#endif