#ifndef __WON_SPLITTER_H__
#define __WON_SPLITTER_H__

#include "WONGUI/Container.h"

namespace WONAPI
{
WON_PTR_FORWARD(Button);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Splitter : public Container
{
public:
	enum QuadrantPosition
	{
		Position_Left			= 0x01,
		Position_Top			= 0x02,
		Position_Right			= 0x04,
		Position_Bottom			= 0x08,

		Position_TopLeft		= Position_Top | Position_Left,
		Position_TopRight		= Position_Top | Position_Right,
		Position_BottomLeft		= Position_Bottom | Position_Left,
		Position_BottomRight	= Position_Bottom | Position_Right
	};
	
	enum SplitterType
	{
		Splitter_LeftRight			= Position_Left | Position_Right,
		Splitter_UpDown				= Position_Top | Position_Bottom,

		Splitter_LeftRight_Top		= Splitter_LeftRight | 0x10,
		Splitter_LeftRight_Bottom	= Splitter_LeftRight | 0x20,

		Splitter_UpDown_Left		= Splitter_UpDown	| 0x40,
		Splitter_UpDown_Right		= Splitter_UpDown	| 0x80,

		Splitter_Center				= Splitter_LeftRight | Splitter_UpDown | 0x100
	};

	enum ControlIdType
	{
		ControlId_Unmax				= ControlId_SplitterMin,
		ControlId_MaxMin,
		ControlId_MaxLeft			= ControlId_MaxMin + Position_Left,
		ControlId_MaxRight			= ControlId_MaxMin + Position_Right,
		ControlId_MaxTop			= ControlId_MaxMin + Position_Top,
		ControlId_MaxBottom			= ControlId_MaxMin + Position_Bottom,
		ControlId_MaxTopLeft		= ControlId_MaxMin + Position_TopLeft,
		ControlId_MaxTopRight		= ControlId_MaxMin + Position_TopRight,
		ControlId_MaxBottomLeft		= ControlId_MaxMin + Position_BottomLeft,
		ControlId_MaxBottomRight	= ControlId_MaxMin + Position_BottomRight,
	};

protected:
	double mLeftPercent, mTopPercent; // Moved to top to avoid alignment compiler warning.

	struct Quadrant
	{
		Component *mComponent;
		QuadrantPosition mPosition;

		Quadrant(Component *theComponent, QuadrantPosition thePosition) : mComponent(theComponent), mPosition(thePosition) { }
	};
	typedef std::list<Quadrant> QuadrantList;
	QuadrantList mQuadrantList;

	struct SplitterParam
	{
		Component *mComponent;
		SplitterType mType;
		ComponentPtr mDragComponent;

		SplitterParam(Component *theComponent, SplitterType theType) : mComponent(theComponent), mType(theType) { }
	};
	typedef std::list<SplitterParam> SplitterList;
	SplitterList mSplitterList;

	typedef std::list<ButtonPtr> ButtonList;
	ButtonList mButtonList;


	WONRectangle mOldBounds;
	bool mLeftRightDrag, mUpDownDrag;
	int mDragX, mDragY;

	int mMinLeft, mMinRight, mMinTop, mMinBottom;
	bool mUpDownMax, mLeftRightMax;

	bool mSplitCoordsValid;
	int mSplitX, mSplitY, mSplitLeft, mSplitRight, mSplitTop, mSplitBottom;
	int mLeftRightSplitterWidth;
	int mUpDownSplitterHeight;

	bool mHaveMaxQuadrant;
	QuadrantPosition mCurMaxQuadrant;

	void CalcQuadrants();
	void CalcSplitterPositions();

	virtual void SizeChanged();
	virtual void MouseDown(int x, int y, MouseButton theButton);
	virtual void MouseUp(int x, int y, MouseButton theButton);
	virtual void MouseDrag(int x, int y);
	virtual void HandleComponentEvent(ComponentEvent *theEvent);

	void AddSplitterPrv(Component *theSplitter, SplitterType theType, Component *theDragComponent);

	virtual ~Splitter();

public:
	Splitter();

	bool CheckMaxButton(ComponentEvent *theEvent);

	void RegisterMaxButton(Button *theButton, QuadrantPosition thePosition);

	void AddQuadrant(Component *theComponent, QuadrantPosition thePosition);
	void AddSplitter(Component *theComponent, SplitterType theType);
	void AddComplexSplitter(Container *theContainer, SplitterType theType, Component *theDragComponent = NULL);
	void AddInvisibleSplitter(SplitterType theType, int theSize = 10);

	void SetMinLeftRight(int minLeft, int minRight);
	void SetMinTopBottom(int minTop, int minBottom);
	void SetPercentages(int theLeftPercent, int theTopPercent);

	void SetSplitterPos(int x, int y, bool rememberPercentage = true);
	void MaxQuadrant(QuadrantPosition thePosition);
	void UnMaxQuadrant();
};
typedef SmartPtr<Splitter> SplitterPtr;


};

#endif
