#include "SplitterConfig.h"
#include "WONGUI/Button.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SplitterConfig::SetComponent(Component *theComponent)
{
	ContainerConfig::SetComponent(theComponent);
	mSplitter = (Splitter*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Splitter::QuadrantPosition SplitterConfig::ReadQuadrant()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("Left")==0)
		return Splitter::Position_Left;
	else if(aStr.compareNoCase("Right")==0)
		return Splitter::Position_Right;
	else if(aStr.compareNoCase("Top")==0)
		return Splitter::Position_Top;
	else if(aStr.compareNoCase("Bottom")==0)
		return Splitter::Position_Bottom;
	else if(aStr.compareNoCase("TopLeft")==0)
		return Splitter::Position_TopLeft;
	else if(aStr.compareNoCase("TopRight")==0)
		return Splitter::Position_TopRight;
	else if(aStr.compareNoCase("BottomLeft")==0)
		return Splitter::Position_BottomLeft;
	else if(aStr.compareNoCase("BottomRight")==0)
		return Splitter::Position_BottomRight;
	else
		throw ConfigObjectException("Invalid quadrant: " + (std::string)aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Splitter::SplitterType SplitterConfig::ReadSplitterType()
{
	GUIString aStr = ReadString();
	if(aStr.compareNoCase("LeftRight")==0)
		return Splitter::Splitter_LeftRight;
	else if(aStr.compareNoCase("UpDown")==0)
		return Splitter::Splitter_UpDown;
	else if(aStr.compareNoCase("LeftRight_Top")==0)
		return Splitter::Splitter_LeftRight_Top;
	else if(aStr.compareNoCase("LeftRight_Bottom")==0)
		return Splitter::Splitter_LeftRight_Bottom;
	else if(aStr.compareNoCase("UpDown_Left")==0)
		return Splitter::Splitter_UpDown_Left;
	else if(aStr.compareNoCase("UpDown_Right")==0)
		return Splitter::Splitter_UpDown_Right;
	else if(aStr.compareNoCase("Center")==0)
		return Splitter::Splitter_Center;
	else
		throw ConfigObjectException("Invalid splitter type: " + (std::string)aStr);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SplitterConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="ADDQUADRANT")
	{
		Component *aComponent = ReadComponent(); EnsureComma();
		Splitter::QuadrantPosition aPosition = ReadQuadrant();
		mSplitter->AddQuadrant(aComponent,aPosition);
	}
	else if(theInstruction=="ADDSPLITTER")
	{
		Component *aComponent = ReadComponent(); EnsureComma();
		Splitter::SplitterType aType = ReadSplitterType();
		mSplitter->AddSplitter(aComponent,aType);
	}
	else if(theInstruction=="ADDINVISIBLESPLITTER")
	{
		Splitter::SplitterType aType = ReadSplitterType();
		int aSize = 10;
		if(!EndOfString())
		{
			EnsureComma();
			aSize = ReadInt();
		}
		mSplitter->AddInvisibleSplitter(aType,aSize);
	}
	else if(theInstruction=="ADDCOMPLEXSPLITTER")
	{
		Container *aContainer= SafeConfigGetComponent<Container*>(ReadComponent(),"Container"); EnsureComma();
		Splitter::SplitterType aType = ReadSplitterType();
		Component *aDragComponent = NULL;
		if(!EndOfString())
		{
			EnsureComma();
			aDragComponent = ReadComponent();
		}
		mSplitter->AddComplexSplitter(aContainer,aType,aDragComponent);
	}
	else if(theInstruction=="MINLEFTRIGHT")
	{
		int minLeft = ReadInt(); EnsureComma();
		int minRight = ReadInt(); 
		mSplitter->SetMinLeftRight(minLeft,minRight);
	}
	else if(theInstruction=="MINTOPBOTTOM")
	{
		int minTop = ReadInt(); EnsureComma();
		int minBottom = ReadInt();
		mSplitter->SetMinTopBottom(minTop,minBottom);
	}
	else if(theInstruction=="SPLITTERPERCENTAGES")
	{
		int leftPercent = ReadInt(); EnsureComma();
		int topPercent = ReadInt();
		mSplitter->SetPercentages(leftPercent,topPercent);
	}
	else if(theInstruction=="MAXQUADRANT")
		mSplitter->MaxQuadrant(ReadQuadrant());
	else if(theInstruction=="UNMAXQUADRAN")
		mSplitter->UnMaxQuadrant();
	else if(theInstruction=="SPLITTERPOS")
	{
		int x = ReadInt(); EnsureComma();
		int y = ReadInt();
		mSplitter->SetSplitterPos(x,y);
	}
	else if(theInstruction=="REGISTERMAXBUTTON")
	{
		Button *aButton = SafeConfigGetComponent<Button*>(ReadComponent(),"Button"); EnsureComma();
		Splitter::QuadrantPosition aPosition = ReadQuadrant();
		mSplitter->RegisterMaxButton(aButton,aPosition);
	}
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}
