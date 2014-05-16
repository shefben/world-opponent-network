#ifndef __CHILDLAYOUTMANAGER_H__
#define __CHILDLAYOUTMANAGER_H__

#include "Component.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class LayoutPoint : public Component
{
public:
	LayoutPoint(int left = 0, int top = 0, int width = 0, int height = 0) { SetPosSize(left,top,width,height); }
	LayoutPoint(bool, int left, int top, int right, int bottom) { SetPosSize(left,top,right-left,bottom-top); }
};
typedef SmartPtr<LayoutPoint> LayoutPointPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChildLayoutInstructionBase : public RefCount
{
public:
	unsigned char mId;
	bool mEnabled;

	ChildLayoutInstructionBase() : mId(0), mEnabled(true) { }
	virtual void Execute(Component * /*theParent*/) { }
	virtual bool UsesComponent(Component * /*theComponent*/) { return false; }
};
typedef SmartPtr<ChildLayoutInstructionBase> ChildLayoutInstructionBasePtr; 
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ChildLayoutManager : public WONAPI::RefCount
{
protected:
	typedef std::list<ChildLayoutInstructionBasePtr> InstructionList;
	InstructionList mInstructionList;

	typedef std::list<LayoutPointPtr> LayoutPointList;
	LayoutPointList mLayoutPointList;

public:
	void AddInstruction(ChildLayoutInstructionBase *theInstruction);
	void ExecuteInstructions(Component *theParent);
	void RemoveComponent(Component *theComponent);
	void Clear();
	void AddLayoutPoint(LayoutPoint *thePoint);

	void EnableInstructions(int theId, bool enable);
};

}  // namespace WONAPI;

#endif




