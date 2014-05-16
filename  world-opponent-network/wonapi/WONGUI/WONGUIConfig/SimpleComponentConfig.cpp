#include "SimpleComponentConfig.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponentConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mRectangleComponent = (RectangleComponent*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RectangleComponentConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SOLIDCOLOR")
		mRectangleComponent->SetSolidColor(ReadColor());
	else if(theInstruction=="OUTLINECOLOR")
		mRectangleComponent->SetOutlineColor(ReadColor());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RectangleComponent3DConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mRectangleComponent3D = (RectangleComponent3D*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RectangleComponent3DConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="SOLIDCOLOR")
		mRectangleComponent3D->SetSolidColor(ReadColor());
	else if(theInstruction=="LIGHTCOLOR")
		mRectangleComponent3D->SetLightColor(ReadColor());
	else if(theInstruction=="DARKCOLOR")
		mRectangleComponent3D->SetDarkColor(ReadColor());
	else if(theInstruction=="UP")
		mRectangleComponent3D->SetIsUp(ReadBool());
	else if(theInstruction=="THICKNESS")
		mRectangleComponent3D->SetThickness(ReadInt());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageComponentConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mImageComponent = (ImageComponent*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ImageComponentConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="IMAGE")
		mImageComponent->SetImage(ReadImage());
	else if(theInstruction=="TILE")
		mImageComponent->SetTile(ReadBool());
	else if(theInstruction=="STRETCH")
		mImageComponent->SetStretch(ReadBool());
	else
		return ComponentConfig::HandleInstruction(theInstruction);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimationComponentConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mAnimationComponent = (AnimationComponent*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AnimationComponentConfig::HandleInstruction(const std::string &theInstruction)
{
	if(ComponentConfig::HandleInstruction(theInstruction))
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScreenContainerConfig::SetComponent(Component *theComponent)
{
	ComponentConfig::SetComponent(theComponent);
	mScreenContainer = (ScreenContainer*)theComponent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ScreenContainerConfig::HandleInstruction(const std::string &theInstruction)
{
	if(theInstruction=="ADDSCREEN")
		mScreenContainer->AddScreen(ReadComponent());
	else if(theInstruction=="SHOWSCREEN")
		mScreenContainer->ShowScreen(ReadComponent());
	else
		return ContainerConfig::HandleInstruction(theInstruction);

	return true;
}
