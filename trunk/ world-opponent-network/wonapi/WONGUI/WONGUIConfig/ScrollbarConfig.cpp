#include "ScrollbarConfig.h"
#include "ButtonConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ScrollbarConfig::ScrollbarConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScrollbarConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mScrollbar = (Scrollbar*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollbarConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="VERTICAL")
		mScrollbar->SetVertical(ReadBool());
	else if(theInstruction=="RESIZETHUMB")
		mScrollbar->SetResizeThumb(ReadBool());
	else if(theInstruction=="MINTHUMBSIZE")
		mScrollbar->SetMinThumbSize(ReadInt());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScrollbarConfig::GetPredefinedComponent(const std::string &theName, ComponentConfig*& theConfig, Component* &theComponent)
{
	if(theName=="THUMB")
	{
		theConfig = new ComponentConfig;
		theComponent =  mScrollbar->GetThumb();
	}
	else if(theName=="UPARROW")
	{
		theConfig = new ButtonConfig;
		theComponent = mScrollbar->GetUpArrow();
	}
	else if(theName=="DOWNARROW")
	{
		theConfig = new ButtonConfig;
		theComponent = mScrollbar->GetDownArrow();
	}
	else if(theName=="SCROLLRECT")
	{
		theConfig = new ButtonConfig;
		theComponent = mScrollbar->GetScrollRect();
	}
	else
		return ContainerConfig::GetPredefinedComponent(theName,theConfig,theComponent);

	return true;
}
