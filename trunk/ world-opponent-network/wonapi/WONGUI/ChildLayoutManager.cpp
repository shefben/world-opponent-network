#include "ChildLayoutManager.h"
#include "Container.h"

using namespace WONAPI;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutManager::ExecuteInstructions(Component *theParent)
{
	for(InstructionList::iterator anItr = mInstructionList.begin(); anItr!=mInstructionList.end(); ++anItr)
	{
		ChildLayoutInstructionBase *anInstruction = *anItr;
		if(anInstruction->mEnabled)
			anInstruction->Execute(theParent);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutManager::AddInstruction(ChildLayoutInstructionBase *theInstruction)
{
	mInstructionList.push_back(theInstruction);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutManager::AddLayoutPoint(LayoutPoint *thePoint)
{
	mLayoutPointList.push_back(thePoint);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutManager::RemoveComponent(Component *theComponent)
{
	for(InstructionList::iterator anItr = mInstructionList.begin(); anItr!=mInstructionList.end();)
	{
		ChildLayoutInstructionBase *anInstruction = *anItr;
		if(anInstruction->UsesComponent(theComponent))
			mInstructionList.erase(anItr++);
		else
			++anItr;
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutManager::Clear()
{
	mInstructionList.clear();
	mLayoutPointList.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ChildLayoutManager::EnableInstructions(int theId, bool enable)
{
	for(InstructionList::iterator anItr = mInstructionList.begin(); anItr!=mInstructionList.end(); ++anItr)
	{
		ChildLayoutInstructionBase *anInstruction = *anItr;
		if(anInstruction->mId==theId)
			anInstruction->mEnabled = enable;			
	}	
}

