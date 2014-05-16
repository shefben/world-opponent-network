#include "ImageButtonConfig.h"

using namespace WONAPI;
using namespace std;

#include "MSComponentConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImageButtonConfig::ImageButtonConfig()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageButtonConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mImageButton = (ImageButton*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageButtonConfig::HandleInstruction(const std::string& theInstruction)
{
	ImageButtonType aType;
	if(theInstruction=="NORMALIMAGE")
		aType = ImageButtonType_Normal;
	else if(theInstruction=="OVERIMAGE")
		aType = ImageButtonType_Over;
	else if(theInstruction=="DOWNIMAGE")
		aType = ImageButtonType_Down;
	else if(theInstruction=="DISABLEDIMAGE")
		aType = ImageButtonType_Disabled;
	else if(theInstruction=="CHECKEDIMAGE")
		aType = ImageButtonType_Checked;
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	mImageButton->SetImage(aType,ReadImage());
	return true;
}

