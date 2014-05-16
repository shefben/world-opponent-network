#include "LabelConfig.h"

using namespace WONAPI;
using namespace std;

#include "MSComponentConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LabelConfig::LabelConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LabelConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mLabel = (Label*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LabelConfig::HandleInstruction(const std::string& theInstruction)
{
	if(theInstruction=="TEXT")
		mLabel->SetText(ReadString());
	else if(theInstruction=="FONT")
		mLabel->SetFont(ReadFont());
	else if(theInstruction=="TEXTCOLOR")
		mLabel->SetTextColor(ReadColor());
	else if(theInstruction=="BACKGROUND")
		mLabel->SetBackground(ReadBackground());
	else if(theInstruction=="VERTALIGN")
		mLabel->SetVertAlign(ReadVertAlign());
	else if(theInstruction=="HORZALIGN")
		mLabel->SetHorzAlign(ReadHorzAlign());
	else if(theInstruction=="WRAP")
		mLabel->SetWrap(ReadBool());
	else if(theInstruction=="ADJUSTHEIGHTTOFITTEXT")
		mLabel->SetAdjustHeightToFitText(ReadBool());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

