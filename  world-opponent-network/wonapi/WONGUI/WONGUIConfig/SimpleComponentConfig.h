#ifndef __WON_SIMPLECOMPONENTCONFIG__
#define __WON_SIMPLECOMPONENTCONFIG__
#include "ContainerConfig.h"
#include "WONGUI/SimpleComponent.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RectangleComponentConfig : public ComponentConfig
{
protected:
	RectangleComponent* mRectangleComponent;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new RectangleComponent; }
	static ComponentConfig* CfgFactory() { return new RectangleComponentConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RectangleComponent3DConfig : public ComponentConfig
{
protected:
	RectangleComponent3D* mRectangleComponent3D;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new RectangleComponent3D; }
	static ComponentConfig* CfgFactory() { return new RectangleComponent3DConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageComponentConfig : public ComponentConfig
{
protected:
	ImageComponent* mImageComponent;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ImageComponent; }
	static ComponentConfig* CfgFactory() { return new ImageComponentConfig; }
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AnimationComponentConfig : public ComponentConfig
{
protected:
	AnimationComponent* mAnimationComponent;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new AnimationComponent; }
	static ComponentConfig* CfgFactory() { return new AnimationComponentConfig; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScreenContainerConfig : public ContainerConfig
{
protected:
	ScreenContainer* mScreenContainer;

	virtual bool HandleInstruction(const std::string &theInstruction);

public:
	virtual void SetComponent(Component *theComponent);

	static Component* CompFactory() { return new ScreenContainer; }
	static ComponentConfig* CfgFactory() { return new ScreenContainerConfig; }
};


}; // namespace WONAPI

#endif