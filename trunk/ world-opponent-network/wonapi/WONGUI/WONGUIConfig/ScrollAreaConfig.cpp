#include "ScrollAreaConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollAreaConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mScrollArea = (ScrollArea*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollAreaConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="BACKGROUND")
		mScrollArea->SetBackground(ReadBackground());
	else if(theInstruction=="BORDERPADDING")
	{
		int left = ReadInt(); EnsureComma();
		int top = ReadInt(); EnsureComma();
		int right = ReadInt(); EnsureComma();
		int bottom = ReadInt();

		mScrollArea->SetBorderPadding(left,top,right,bottom);
	}
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollerConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mScroller = (Scroller*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollerConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SCROLLAREA")
	{
		ScrollArea *aScrollArea = dynamic_cast<ScrollArea*>(ReadComponent());
		if(aScrollArea==NULL)
			throw ConfigObjectException("Expecting ScrollArea");

		mScroller->SetScrollArea(aScrollArea);
	}
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScrollButtonCondition ButtonScrollerConfig::GetScrollButtonCondition()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("DISABLE")==0)
		return ScrollButtonCondition_Disable;
	else if(aStr.compareNoCase("ENABLE")==0)
		return ScrollButtonCondition_Enable;
	else if(aStr.compareNoCase("CONDITIONAL")==0)
		return ScrollButtonCondition_Conditional;
	else if(aStr.compareNoCase("PARTIALCONDITIONAL")==0)
		return ScrollButtonCondition_PartialConditional;
	else
		throw ConfigObjectException("Invalid scrollbar condition: " + (std::string)aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ButtonScrollerConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SCROLLBARCONDITIONS")
	{
		ScrollButtonCondition horz, vert;
		horz = GetScrollButtonCondition(); EnsureComma();
		vert = GetScrollButtonCondition();
		mButtonScroller->SetScrollButtonConditions(horz,vert);
	}
	else
		return ScrollerConfig::HandleInstruction(theInstruction);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollbarScrollerConfig::SetComponent(Component *theComponent)
{
	ScrollerConfig::SetComponent(theComponent);
	mScrollbarScroller = (ScrollbarScroller*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScrollbarCondition ScrollbarScrollerConfig::GetScrollbarCondition()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("DISABLE")==0)
		return ScrollbarCondition_Disable;
	else if(aStr.compareNoCase("ENABLE")==0)
		return ScrollbarCondition_Enable;
	else if(aStr.compareNoCase("CONDITIONAL")==0)
		return ScrollbarCondition_Conditional;
	else
		throw ConfigObjectException("Invalid scrollbar condition: " + (std::string)aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollbarScrollerConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SCROLLBARCONDITIONS")
	{
		ScrollbarCondition horz, vert;
		horz = GetScrollbarCondition(); EnsureComma();
		vert = GetScrollbarCondition();
		mScrollbarScroller->SetScrollbarConditions(horz,vert);
	}
	else
		return ScrollerConfig::HandleInstruction(theInstruction);

	return true;
}

