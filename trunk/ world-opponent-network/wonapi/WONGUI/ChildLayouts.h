#ifndef __CHILDLAYOUTS_H__
#define __CHILDLAYOUTS_H__

#include "ChildLayoutManager.h"

namespace WONAPI
{

	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum ChildLayoutInstructionType
{
	CLI_SameWidth		=		0x0001,
	CLI_SameHeight		=		0x0002,

	CLI_Above			=		0x0100,
	CLI_Below			=		0x0200,
	CLI_Right			=		0x0400,
	CLI_Left			=		0x0800,

	CLI_SameLeft		=		0x1000,
	CLI_SameRight		=		0x2000,
	CLI_SameTop			=		0x4000,
	CLI_SameBottom		=		0x8000,

	CLI_GrowToRight		=	   0x10000,
	CLI_GrowToLeft		=	   0x20000,
	CLI_GrowToTop		=	   0x40000,
	CLI_GrowToBottom	=	   0x80000,

	CLI_Max				=	  0x100000,

	CLI_SameSize		=		CLI_SameWidth | CLI_SameHeight,
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChildLayoutList
{
protected:
	typedef std::list<Component*> ChildList;
	mutable ChildList mChildList;
	mutable ChildList::iterator mItr;

public:
	ChildLayoutList(Component *c1);
	ChildLayoutList(Component *c1, Component *c2);
	ChildLayoutList(Component *c1, Component *c2, Component *c3);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7);
	ChildLayoutList(Component *c1, Component *c2, Component *c3, Component *c4, Component *c5, Component *c6, Component *c7, Component *c8);

	void Rewind() const;
	bool HasMore() const;
	Component* GetNext() const;

	void AddTo(ChildList &theList) const;
};
typedef SmartPtr<ChildLayoutList> ChildLayoutListPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChildLayoutInstruction : public ChildLayoutInstructionBase
{
public:
	int mType;
	Component* mTarget;
	Component* mReference;
	int mHorzPosPad, mVertPosPad, mHorzSizePad, mVertSizePad;

	ChildLayoutInstruction() { }
	ChildLayoutInstruction(Component *theTarget, int theType, Component *theRef, int hpospad = 0, int vpospad = 0, int hsizepad = 0, int vsizepad = 0)
		: mTarget(theTarget), mType(theType), mReference(theRef), 
		mHorzPosPad(hpospad), mVertPosPad(vpospad), mHorzSizePad(hsizepad), mVertSizePad(vsizepad) { }

	virtual void Execute(Component *theParent);
	virtual bool UsesComponent(Component *theComponent) { return mTarget==theComponent || mReference==theComponent; }
};
typedef SmartPtr<ChildLayoutInstruction> ChildLayoutInstructionPtr; 


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class PercentLayoutParam
{
public:
	Component *left, *top, *right, *bottom;
	int px, py, pw, ph;
	int lp, tp, rp, bp;
	int dx, dy, dw, dh;

	void Init(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp);

public:
	void DoCalc(Component *theTarget, Component *theParent);
	PercentLayoutParam(Component *ref, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1);
	PercentLayoutParam(Component *ref1, Component *ref2, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1);
	PercentLayoutParam(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1);

	void SetBorders(Component *theLeft, Component *theTop, Component *theRight, Component *theBottom);
	void SetPercentages(int thePercentX, int thePercentY, int thePercentWidth, int thePercentHeight);
	void SetPadding(int theLeftPad, int theTopPad, int theRightPad, int theBottomPad);
};


class PercentLayout : public ChildLayoutInstructionBase
{
public:
	Component *mTarget;
	PercentLayoutParam mParam;

	void Init(Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw, int ph, int lp, int tp, int rp, int bp);

	PercentLayout(Component *target, const PercentLayoutParam &theParam) : mTarget(target), mParam(theParam) { }
	PercentLayout(Component *target, Component *ref, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1);
	PercentLayout(Component *target, Component *ref1, Component *ref2, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1);
	PercentLayout(Component *target, Component *left, Component *top, Component *right, Component *bottom, int px, int py, int pw = -1, int ph = -1, int lp = -1, int tp = -1, int rp = -1, int bp = -1);

	virtual void Execute(Component *theParent);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CenterLayout : public ChildLayoutInstructionBase
{
protected:
	bool mHorz, mVert;
	Component *mCenterBox;

	typedef std::list<Component*> ComponentList;
	ComponentList mComponentList;

public:
	CenterLayout(Component *theCenterBox, bool horz, bool vert) : mHorz(horz), mVert(vert), mCenterBox(theCenterBox) { }
	CenterLayout(Component *theCenterBox, Component *theComponent, bool horz, bool vert) : mHorz(horz), mVert(vert), mCenterBox(theCenterBox) { Add(theComponent); }
	CenterLayout(Component *theCenterBox, const ChildLayoutList &theList, bool horz, bool vert) : mHorz(horz), mVert(vert), mCenterBox(theCenterBox) { Add(theList); }
	CenterLayout* Add(const ChildLayoutList &theList) { theList.AddTo(mComponentList); return this; }
	CenterLayout* Add(Component *theChild) { mComponentList.push_back(theChild); return this; }

	static void Center(Component *theParent, Component *theChild, bool horz, bool vert); 

	virtual void Execute(Component *theParent);
};
typedef SmartPtr<CenterLayout> CenterLayoutPtr; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class VerticalFlowLayout : public ChildLayoutInstructionBase
{
protected: 
	struct ItemInfo
	{
		int mTopPad;
		int mLeftPad;
		Component *mComponent;

		ItemInfo(Component *theComponent, int theTopPad, int theLeftPad) : mComponent(theComponent), mTopPad(theTopPad), mLeftPad(theLeftPad) { }
	};

	Component *mTopComp;
	typedef std::list<ItemInfo> ComponentList;
	ComponentList mComponentList;

public:
	VerticalFlowLayout(Component *theTop);
	VerticalFlowLayout* Add(Component *theChild, int theTopPad = 0, int theLeftPad = 0);

	virtual void Execute(Component *theParent);
};
typedef SmartPtr<VerticalFlowLayout> VerticalFlowLayoutPtr; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SameSizeLayout : public ChildLayoutInstructionBase
{
protected:
	typedef std::list<Component*> ComponentList;
	ComponentList mComponentList;
	bool mHorz, mVert;

public:
	SameSizeLayout(bool horz = true, bool vert = true) : mHorz(horz), mVert(vert) { }
	SameSizeLayout(Component *firstComp, bool horz = true, bool vert = true) : mHorz(horz), mVert(vert) { Add(firstComp); }
	SameSizeLayout* Add(Component *theComp);

	virtual void Execute(Component *theParent);
};
typedef SmartPtr<SameSizeLayout> SameSizeLayoutPtr; 


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ClearRightLayout : public ChildLayoutInstructionBase
{
protected:
	Component *mComponent;
	int mPad;

	typedef std::list<Component*> ComponentList;
	ComponentList mComponentList;

public:
	ClearRightLayout(Component *theComp, int thePad = 0) : mComponent(theComp), mPad(thePad) { }
	ClearRightLayout* Add(Component *theComp) { mComponentList.push_back(theComp); return this; }

	virtual void Execute(Component *theParent);
};
typedef SmartPtr<ClearRightLayout> ClearRightLayoutPtr; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SpaceEvenlyLayout : public ChildLayoutInstructionBase
{
protected:
	Component *mSpaceBox;
	int mLeftPad;
	int mRightPad;
	bool mIsHorz;

	typedef std::list<Component*> ComponentList;
	ComponentList mComponentList;

public:
	SpaceEvenlyLayout(Component *theComp, int leftPad = 0, int rightPad = 0, bool isHorz = true) : mSpaceBox(theComp), mLeftPad(leftPad), mRightPad(rightPad), mIsHorz(isHorz) { }
	SpaceEvenlyLayout* Add(Component *theComp) { mComponentList.push_back(theComp); return this; }

	virtual void Execute(Component *theParent);
};
typedef SmartPtr<SpaceEvenlyLayout> SpaceEvenlyLayoutPtr; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class NoOverlapLayout : public ChildLayoutInstructionBase
{
protected:
	Component *mRef;
	Component *mTarget;
	int mType; // Combination of CLI_Above, CLI_Below, CLI_Left, and CLI_Right
	int mHorzPad, mVertPad;

	typedef std::list<Component*> ComponentList;
	ComponentList mComponentList;

public:
	NoOverlapLayout(Component *theTarget, int theType, Component *theRef, int horzPad = 0, int vertPad = 0) : 
	  mTarget(theTarget), mType(theType), mRef(theRef), mHorzPad(horzPad), mVertPad(vertPad) { }
	
	 NoOverlapLayout* Add(Component *theComp) { mComponentList.push_back(theComp); return this; }

	virtual void Execute(Component *theParent);
};
typedef SmartPtr<NoOverlapLayout> NoOverlapLayoutPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SizeAtLeastLayout : public ChildLayoutInstructionBase
{
protected:
	Component *mTarget;
	int mHorzPad, mVertPad;
	typedef std::list<Component*> ComponentList;
	ComponentList mComponentList;

public:
	SizeAtLeastLayout(Component *theTarget, int theHorzPad = 0, int theVertPad = 0) :
		mTarget(theTarget), mHorzPad(theHorzPad), mVertPad(theVertPad) { }

	SizeAtLeastLayout* Add(Component *theComp) { mComponentList.push_back(theComp); return this; }
	virtual void Execute(Component *theParent);
};
typedef SmartPtr<SizeAtLeastLayout> SizeAtLeastLayoutPtr;


} // namespace WONAPI


#endif